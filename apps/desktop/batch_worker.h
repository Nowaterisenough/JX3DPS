#ifndef BATCH_WORKER_H
#define BATCH_WORKER_H

#include <QThread>
#include "combat_event.h"
#include "buff_trace.h"
#include "combat_history.h"
#include "simulation_result.h"

#include "runtime_catalog.h"
#include "src/core/runtime/batch.hpp"

class BatchWorker final : public QThread
{
    Q_OBJECT

public:
    BatchWorker(JX3DPS::runtime::MacroProgram program,
                JX3DPS::runtime::BatchOptions options,
                desktop::Config config,
                QObject *parent = nullptr, bool captureTrace = false, bool calculateGains = false);
    ~BatchWorker() override;

    void RequestCancel();
    std::uint64_t Completed() const;
    std::uint64_t Requested() const;

signals:
    void ResultReady(const QString &result);
    void StatisticsReady(const SimulationResult &result);
    void Failed(const QString &error);
    void TraceReady(const CombatEvents &events);
    void BuffTraceReady(const BuffEvents &events);
    void HistoryReady(const desktop::HistoryChunk &history);

protected:
    void run() override;

private:
    JX3DPS::runtime::MacroProgram m_program;
    JX3DPS::runtime::BatchOptions m_options;
    desktop::Config m_config;
    JX3DPS::runtime::BatchControl m_control;
    bool m_captureTrace = false;
    bool m_calculateGains = false;
    std::atomic<std::uint64_t> m_completedStages{0};
};

#endif // BATCH_WORKER_H
