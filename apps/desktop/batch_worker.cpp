#include "batch_worker.h"

#include <exception>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

#include <QElapsedTimer>

#include "diagnostics.h"
#include "runtime_backend.h"
#include "traced_rules.h"

namespace {
namespace runtime = JX3DPS::runtime;

struct SkillTotals {
    JX3DPS::value_t damage = 0;
    std::uint64_t count = 0, hit = 0, critical = 0, insight = 0, miss = 0;
    void Add(JX3DPS::value_t value, JX3DPS::RollResult outcome) {
        if (value < 0 || damage > std::numeric_limits<JX3DPS::value_t>::max() - value)
            throw std::overflow_error("skill damage statistics overflow");
        damage += value;
        ++count;
        if (outcome == JX3DPS::RollResult::HIT) ++hit;
        else if (outcome == JX3DPS::RollResult::DOUBLE) ++critical;
        else if (outcome == JX3DPS::RollResult::INSIGHT) ++insight;
        else ++miss;
    }
    void Merge(const SkillTotals &other) {
        if (damage > std::numeric_limits<JX3DPS::value_t>::max() - other.damage)
            throw std::overflow_error("skill damage statistics overflow");
        damage += other.damage;
        count += other.count; hit += other.hit; critical += other.critical;
        insight += other.insight; miss += other.miss;
    }
};

void StoreSkills(SimulationResult &result, const std::vector<std::vector<SkillTotals>> &workers,
                 desktop::Specialization specialization) {
    result.skillsCollected = true;
    if (workers.empty()) return;
    for (std::size_t index = 0; index < workers[0].size(); ++index) {
        SkillTotals combined;
        for (const auto &worker : workers) combined.Merge(worker[index]);
        if (combined.count)
            result.skills.push_back({static_cast<int>(index), desktop::SkillName(specialization, static_cast<runtime::Slot>(index)),
                combined.damage, combined.count, combined.hit, combined.critical, combined.insight, combined.miss});
    }
}

QString FormatStats(const runtime::BatchStats &stats, double seconds, double elapsed)
{
    if (!stats.count) {
        return QStringLiteral("没有已完成的模拟。");
    }
    return QStringLiteral("%1 次模拟 | 平均秒伤 %2 | 秒伤标准差 %3\n最低秒伤 %4 | 最高秒伤 %5\n校验值 %6 | 耗时 %7 秒")
        .arg(stats.count)
        .arg(stats.mean_damage / seconds, 0, 'f', 1)
        .arg(stats.PopulationDeviation() / seconds, 0, 'f', 1)
        .arg(stats.min_damage / seconds, 0, 'f', 1)
        .arg(stats.max_damage / seconds, 0, 'f', 1)
        .arg(stats.checksum)
        .arg(elapsed, 0, 'f', 2);
}

} // namespace

BatchWorker::BatchWorker(runtime::MacroProgram program, runtime::BatchOptions options, desktop::Config config, QObject *parent, bool captureTrace, bool calculateGains, desktop::GainComparisonMode gainMode) :
    QThread(parent), m_program(std::move(program)), m_options(options), m_config(std::move(config)),
    m_captureTrace(captureTrace), m_calculateGains(calculateGains), m_gainMode(gainMode)
{
    qRegisterMetaType<CombatEvents>();
    qRegisterMetaType<BuffEvents>();
    qRegisterMetaType<desktop::HistoryChunk>();
    qRegisterMetaType<SimulationResult>();
}

BatchWorker::~BatchWorker()
{
    RequestCancel();
    wait();
}

void BatchWorker::RequestCancel()
{
    m_control.cancel_requested.store(true, std::memory_order_relaxed);
}

std::uint64_t BatchWorker::Completed() const
{
    return m_control.completed.load(std::memory_order_relaxed);
}

std::uint64_t BatchWorker::Requested() const
{
    return m_options.iterations *
           (m_calculateGains ? runtime::AttributeGainSimulationStages : 1);
}

void BatchWorker::run()
{
    QElapsedTimer timer;
    timer.start();
    try {
        std::visit(
            [&](const auto &config) {
                using Rules               = decltype(desktop::MakeRules(config));
                const auto specialization = desktop::ClassOf(m_config);
                const auto skillCount = desktop::Describe(specialization).skills.size();
                if (m_captureTrace) {
                    if (m_options.iterations != 1) {
                        throw std::invalid_argument("战斗记录模式需要设置为一次模拟");
                    }
                    if (m_control.cancel_requested.load(std::memory_order_relaxed)) {
                        emit StatisticsReady({ {}, m_options, timer.nsecsElapsed() / 1e9, true });
                        emit ResultReady(QStringLiteral("已取消，没有已完成的模拟。"));
                        return;
                    }
                    auto rules = desktop::MakeRules(config);
                    runtime::Simulation<desktop::TracedRules<Rules>> simulation(m_program, desktop::TracedRules(rules), m_options.damage_capacity, m_options.mutation_capacity);
                    simulation.Start(m_options.duration, runtime::IterationSeed(m_options.seed, 0));
                    desktop::HistoryChunk history;
                    history.specialization = specialization;
                    history.initial = simulation.GetState();
                    auto buffs = InitialBuffEvents(simulation.GetState(), specialization);
                    simulation.Run();
                    runtime::BatchStats stats;
                    std::vector<std::vector<SkillTotals>> skillTotals(1, std::vector<SkillTotals>(skillCount));
                    stats.Add(simulation.TotalDamage([&](const runtime::DamageIntent &hit, JX3DPS::value_t damage) {
                        skillTotals[0].at(hit.skill).Add(damage, hit.outcome);
                    }), 0);
                    CombatEvents events;
                    for (const auto &hit : simulation.Log().Intents()) {
                        const auto name = desktop::SkillName(specialization, hit.skill);
                        events.push_back({ hit.frame, hit.skill, name, simulation.GetRules().Reduce(hit), static_cast<int>(hit.outcome), hit.sequence });
                    }
                    std::size_t mutationCursor = 0;
                    buffs += ReadBuffEvents(simulation.Log().Mutations(), mutationCursor, specialization);
                    m_control.completed.store(1, std::memory_order_relaxed);
                    emit TraceReady(events);
                    emit BuffTraceReady(buffs);
                    history.mutations = simulation.Log().Mutations();
                    history.attributes = simulation.GetRules().AttributeHistory();
                    history.observedFrame = simulation.GetState().now;
                    history.finished = true;
                    emit HistoryReady(history);
                    SimulationResult result{ stats, m_options, timer.nsecsElapsed() / 1e9, true };
                    StoreSkills(result, skillTotals, specialization);
                    emit StatisticsReady(result);
                    emit ResultReady(FormatStats(stats, m_options.duration / 16.0, timer.elapsed() / 1000.0));
                    return;
                }
                SimulationResult result{ {}, m_options, timer.nsecsElapsed() / 1e9, false };
                result.gainsRequested = m_calculateGains;
                result.gainMode = m_gainMode;
                const auto workerCount = static_cast<unsigned>(std::min<std::uint64_t>(m_options.workers, m_options.iterations));
                std::vector<std::vector<SkillTotals>> skillTotals(workerCount, std::vector<SkillTotals>(skillCount));
                auto reduce = [&](auto &simulation, unsigned worker) {
                    return simulation.TotalDamage([&](const runtime::DamageIntent &hit, JX3DPS::value_t damage) {
                        skillTotals[worker].at(hit.skill).Add(damage, hit.outcome);
                    });
                };
                if (m_calculateGains) {
                    const auto increments = desktop::GainComparisonIncrements(m_gainMode, specialization == desktop::Specialization::MoWen);
                    const auto calculated = runtime::RunAttributeGainsControlled(m_program, m_options, m_control, config, reduce, increments);
                    result.stats = calculated.primary;
                    for (std::size_t index = 0; index < calculated.gains.size(); ++index) {
                        if (calculated.gains[index])
                            result.gains.push_back({desktop::AttributeGainName(index), increments[index],
                                                    *calculated.gains[index]});
                    }
                } else {
                    result.stats = runtime::RunBatchControlledWithReducer(m_program, m_options, m_control, desktop::MakeRules(config), reduce);
                }
                StoreSkills(result, skillTotals, specialization);
                result.elapsedSeconds = timer.nsecsElapsed() / 1e9;
                emit StatisticsReady(result);
                const auto status = result.Complete() ? QStringLiteral("已完成。 ") : QStringLiteral("已取消，已完成部分的结果：");
                emit ResultReady((m_calculateGains ? QStringLiteral("属性收益：%1 / 8 项完成。 ").arg(result.gains.size()) : QString{}) +
                                 status + FormatStats(result.stats, m_options.duration / 16.0, timer.elapsed() / 1000.0));
            },
            m_config);
    } catch (const std::exception &error) {
        emit Failed(desktop::Diagnostic(QString::fromUtf8(error.what())));
    } catch (...) {
        emit Failed(QStringLiteral("模拟发生未知错误。"));
    }
}
