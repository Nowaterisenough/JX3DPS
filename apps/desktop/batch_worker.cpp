#include "batch_worker.h"

#include <atomic>
#include <chrono>
#include <cstddef>
#include <exception>
#include <limits>
#include <stdexcept>
#include <thread>
#include <utility>
#include <variant>
#include <vector>

#include <QElapsedTimer>
#include <QScopeGuard>

#include "src/core/runtime/skill_totals.hpp"

#include "diagnostics.h"
#include "runtime_backend.h"
#include "traced_rules.h"

namespace {
namespace runtime = JX3DPS::runtime;

using runtime::SkillTotals;

FightHotspots DisplayProfile(const JX3DPS::engine::ProfileSummary &summary)
{
    FightHotspots report;
    report.fights       = summary.fights;
    report.samplePoints = summary.samplePoints;
    report.workerMs     = summary.workerMs;
    report.phaseUs      = summary.phaseUs;
    for (const auto &w : summary.workers) {
        report.workers.push_back({ w.fights, w.samplePoints });
    }
    for (const auto &c : summary.calls) {
        report.calls.push_back({ QString::fromStdString(c.function), c.parent, c.inclusive, c.self });
    }
    return report;
}

void StoreSkills(SimulationResult &result, const std::vector<std::vector<SkillTotals>> &workers, desktop::Specialization specialization)
{
    result.skillsCollected = true;
    if (workers.empty()) {
        return;
    }
    for (std::size_t index = 0; index < workers[0].size(); ++index) {
        SkillTotals combined;
        for (const auto &worker : workers) {
            combined.Merge(worker[index]);
        }
        if (combined.count != 0u) {
            result.skills.push_back({ static_cast<int>(index),
                                      desktop::SkillName(specialization, static_cast<runtime::Slot>(index)),
                                      combined.damage,
                                      combined.count,
                                      combined.hit,
                                      combined.critical,
                                      combined.insight,
                                      combined.miss });
        }
    }
}

QString FormatStats(const runtime::BatchStats &stats, double seconds, double elapsed)
{
    if (stats.count == 0u) {
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

BatchWorker::BatchWorker(runtime::MacroProgram       program,
                         runtime::BatchOptions       options,
                         desktop::Config             config,
                         QObject                    *parent,
                         bool                        capture_trace,
                         bool                        calculate_gains,
                         desktop::GainComparisonMode gain_mode,
                         bool                        hotspots) :
    QThread(parent), m_program(std::move(program)), m_options(options), m_config(std::move(config)),
    m_captureTrace(capture_trace), m_calculateGains(calculate_gains), m_gainMode(gain_mode), m_hotspots(hotspots)
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
    jx3dps_cancel(m_engine.get());
    m_control.cancel_requested.store(true, std::memory_order_relaxed);
}

std::uint64_t BatchWorker::Completed() const
{
    if (!m_captureTrace) {
        return jx3dps_completed(m_engine.get());
    }
    return m_control.completed.load(std::memory_order_relaxed);
}

std::uint64_t BatchWorker::Requested() const
{
    if (!m_calculateGains) {
        return m_options.iterations;
    }
    const auto STAGES =
        runtime::GainSimulationStages(m_config);
    return m_options.iterations * STAGES;
}

void BatchWorker::run()
{
    QElapsedTimer timer;
    timer.start();
    try {
        if (!m_captureTrace) {
            const auto SPEC = desktop::ClassOf(m_config);
            auto       increments =
                desktop::GainComparisonIncrements(m_gainMode);
            const auto CALCULATED =
                JX3DPS::engine::Batch(m_engine.get(), m_config, m_program.source_text, m_options, true, m_calculateGains ? &increments : nullptr, m_hotspots);
            SimulationResult result{ CALCULATED.primary, m_options, timer.nsecsElapsed() / 1e9, false };
            result.gainsRequested  = m_calculateGains;
            result.gainMode        = m_gainMode;
            result.skillsCollected = true;
            for (const auto &s : CALCULATED.skills) {
                result.skills.push_back({ s.skill, desktop::SkillName(SPEC, s.skill), s.damage, s.count, s.hit, s.critical, s.insight, s.miss });
            }
            for (size_t i = 0; i < CALCULATED.gains.size(); ++i) {
                const auto &gain = CALCULATED.gains[i];
                if (gain.has_value()) {
                    result.gains.push_back({ desktop::AttributeGainName(i), increments[i], *gain });
                }
            }
            if (CALCULATED.profile) {
                result.hotspots = DisplayProfile(*CALCULATED.profile);
            }
            emit StatisticsReady(result);
            const auto STATUS = result.Complete() ? QStringLiteral("已完成。 ") : QStringLiteral("已取消，已完成部分的结果：");
            emit ResultReady((m_calculateGains ? QStringLiteral("属性收益：%1 / 8 项完成。 ").arg(result.gains.size()) : QString{}) +
                             STATUS + FormatStats(result.stats, JX3DPS::FramesToSeconds(m_options.duration), result.elapsedSeconds));
            return;
        }
        const auto SIMULATE = [&]<bool Measure>(runtime::BatchProfile *profile) {
            const auto &config = m_config;
            using Rules               = decltype(desktop::MakeRules(config));
            const auto SPECIALIZATION = desktop::ClassOf(m_config);
            const auto SKILL_COUNT    = desktop::Describe(SPECIALIZATION).skills.size();
            if (m_options.iterations != 1) {
                throw std::invalid_argument("战斗记录模式需要设置为一次模拟");
            }
            if (m_control.cancel_requested.load(std::memory_order_relaxed)) {
                emit StatisticsReady({ {}, m_options, timer.nsecsElapsed() / 1e9, true });
                emit ResultReady(QStringLiteral("已取消，没有已完成的模拟。"));
                return;
            }
            const auto                          RULES = desktop::MakeRules(config);
            runtime::WorkerMeasurement<Measure> measurement(profile, 0, "单次记录");
            runtime::Simulation<desktop::TracedRules<Rules>, false, Measure>
                simulation(m_program, desktop::TracedRules(RULES), m_options.damage_capacity, m_options.mutation_capacity);
            simulation.Start(m_options.duration, runtime::IterationSeed(m_options.seed, 0));
            desktop::HistoryChunk history;
            history.specialization = SPECIALIZATION;
            history.initial        = simulation.GetState();
            auto buffs             = InitialBuffEvents(simulation.GetState(), SPECIALIZATION);
            simulation.Run();
            runtime::BatchStats                   stats;
            std::vector<std::vector<SkillTotals>> skill_totals(1, std::vector<SkillTotals>(SKILL_COUNT));
            stats.Add(simulation.TotalDamage([&](const runtime::DamageIntent &hit, JX3DPS::value_t damage) {
                skill_totals[0].at(hit.skill).Add(damage, hit.outcome);
            }),
                      0);
            CombatEvents events;
            for (const auto &hit : simulation.Log().Intents()) {
                const auto NAME = desktop::SkillName(SPECIALIZATION, hit.skill);
                events.push_back({ hit.frame, hit.skill, NAME, simulation.GetRules().Reduce(hit), static_cast<int>(hit.outcome), hit.sequence });
            }
            std::size_t mutation_cursor = 0;
            buffs += ReadBuffEvents(simulation.Log().Mutations(), mutation_cursor, SPECIALIZATION);
            m_control.completed.store(1, std::memory_order_relaxed);
            emit TraceReady(events);
            emit BuffTraceReady(buffs);
            history.mutations     = simulation.Log().Mutations();
            history.attributes    = simulation.GetRules().AttributeHistory();
            history.observedFrame = simulation.GetState().now;
            history.finished      = true;
            emit             HistoryReady(history);
            SimulationResult result{ stats, m_options, timer.nsecsElapsed() / 1e9, true };
            StoreSkills(result, skill_totals, SPECIALIZATION);
            if constexpr (Measure) {
                ++profile->workers[0].fights;
                measurement.Finish();
                result.hotspots = DisplayProfile(JX3DPS::engine::Summarize(*profile));
            }
            result.elapsedSeconds = timer.nsecsElapsed() / 1e9;
            emit StatisticsReady(result);
            emit ResultReady(FormatStats(stats, JX3DPS::FramesToSeconds(m_options.duration), result.elapsedSeconds));
        };
        if (m_hotspots) {
            runtime::BatchProfile profile(1);
            SIMULATE.template     operator()<true>(&profile);
        } else {
            SIMULATE.template operator()<false>(nullptr);
        }
    } catch (const std::exception &error) {
        emit Failed(desktop::Diagnostic(QString::fromUtf8(error.what())));
    } catch (...) {
        emit Failed(QStringLiteral("模拟发生未知错误。"));
    }
}
