#include "batch_worker.h"

#include <exception>
#include <array>
#include <optional>
#include <utility>

#include <QElapsedTimer>

#include "diagnostics.h"
#include "runtime_backend.h"
#include "traced_rules.h"

namespace {
namespace runtime = JX3DPS::runtime;
namespace tx      = JX3DPS::runtime::tai_xu;

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

BatchWorker::BatchWorker(runtime::MacroProgram program, runtime::BatchOptions options, desktop::Config config, QObject *parent, bool captureTrace, bool calculateGains) :
    QThread(parent), m_program(std::move(program)), m_options(options), m_config(std::move(config)),
    m_captureTrace(captureTrace), m_calculateGains(calculateGains)
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
    return m_completedStages.load(std::memory_order_relaxed) + m_control.completed.load(std::memory_order_relaxed);
}

std::uint64_t BatchWorker::Requested() const
{
    return m_options.iterations *
           (m_calculateGains ? 1 + desktop::AttributeGains.size() : 1);
}

void BatchWorker::run()
{
    QElapsedTimer timer;
    timer.start();
    try {
        std::visit(
            [&](const auto &config) {
                auto rules                = desktop::MakeRules(config);
                using Rules               = decltype(rules);
                const auto specialization = desktop::ClassOf(m_config);
                if (m_captureTrace) {
                    if (m_options.iterations != 1) {
                        throw std::invalid_argument("战斗记录模式需要设置为一次模拟");
                    }
                    if (m_control.cancel_requested.load(std::memory_order_relaxed)) {
                        emit StatisticsReady({ {}, m_options, timer.nsecsElapsed() / 1e9, true });
                        emit ResultReady(QStringLiteral("已取消，没有已完成的模拟。"));
                        return;
                    }
                    runtime::Simulation<desktop::TracedRules<Rules>> simulation(m_program, desktop::TracedRules(rules), m_options.damage_capacity, m_options.mutation_capacity);
                    simulation.Start(m_options.duration, runtime::IterationSeed(m_options.seed, 0));
                    desktop::HistoryChunk history;
                    history.specialization = specialization;
                    history.initial = simulation.GetState();
                    auto buffs = InitialBuffEvents(simulation.GetState(), specialization);
                    simulation.Run();
                    runtime::BatchStats stats;
                    stats.Add(simulation.TotalDamage(), 0);
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
                    emit StatisticsReady({ stats, m_options, timer.nsecsElapsed() / 1e9, true });
                    emit ResultReady(FormatStats(stats, m_options.duration / 16.0, timer.elapsed() / 1000.0));
                    return;
                }
                SimulationResult result{ {}, m_options, timer.nsecsElapsed() / 1e9, false };
                result.gainsRequested = m_calculateGains;
                std::array<std::optional<runtime::BatchStats>, 8> gainStats;
                m_completedStages.store(0, std::memory_order_relaxed);

                if (m_calculateGains) {
                    const auto replay = desktop::MakeDamageReplay(config, desktop::DamageOnlyGainIndexes);
                    const auto combined = runtime::RunBatchControlledObserved(
                        m_program, m_options, m_control, rules, replay.Count(), 1 + replay.Count(), replay);
                    result.stats = combined.first;
                    for (std::size_t i = 0; i < desktop::DamageOnlyGainIndexes.size(); ++i)
                        gainStats[desktop::DamageOnlyGainIndexes[i]] = combined.second[i];
                } else {
                    result.stats = runtime::RunBatchControlled(m_program, m_options, m_control, rules);
                }
                // Only these gains can change the causal roll stream or the
                // macro timing. The other five were reduced from the same
                // event log above, so their random outcomes stay identical.
                for (std::size_t stage = 0; m_calculateGains && stage < desktop::RotationGainIndexes.size(); ++stage) {
                    if (m_control.cancel_requested.load(std::memory_order_relaxed)) break;
                    const auto index = desktop::RotationGainIndexes[stage];
                    const auto gainRules = desktop::MakeRules(desktop::ApplyAttributeGain(config, index));
                    m_control.completed.store(0, std::memory_order_relaxed);
                    m_completedStages.store(m_options.iterations *
                                                (1 + desktop::DamageOnlyGainIndexes.size() + stage),
                                            std::memory_order_relaxed);
                    const auto gain = runtime::RunBatchControlled(m_program, m_options, m_control, gainRules);
                    if (gain.count == m_options.iterations) gainStats[index] = gain;
                }
                if (m_calculateGains && result.stats.count == m_options.iterations) {
                    for (std::size_t index = 0; index < desktop::AttributeGains.size(); ++index) {
                        if (!gainStats[index]) continue;
                        result.gains.push_back({QString::fromUtf8(desktop::AttributeGains[index].name),
                                                100, *gainStats[index]});
                    }
                    result.elapsedSeconds = timer.nsecsElapsed() / 1e9;
                }
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
