#ifndef DEBUG_SESSION_H
#define DEBUG_SESSION_H

#include <QObject>
#include <QSet>
#include <QString>
#include <QStringList>
#include <QTimer>
#include <functional>
#include <memory>

#include "runtime_catalog.h"
#include "combat_event.h"
#include "buff_trace.h"
#include "macro_step.h"
#include "combat_history.h"

class DebugSimulator;

/**
 * @brief JX3DPS调试会话
 *
 * 管理调试状态、断点、单步执行
 * 与JX3DPS模拟器引擎交互
 */
class DebugSession : public QObject
{
    Q_OBJECT

public:
    enum State {
        Stopped,    // 未启动
        Running,    // 运行中
        Paused,     // 已暂停
        Finished    // 执行完成
    };

    struct DebugInfo {
        int lineNumber;          // 当前执行行号（1-based）
        int frameIndex;          // 当前帧索引
        QString currentMacro;    // 当前宏名称
        QString currentSkill;    // 当前技能名称

        // 玩家状态
        double lifePercent;
        double manaPercent;
        int qidian;
        int rage;
        int energy;

        // 目标状态
        int targetId;
        double targetLifePercent;

        // 时间
        int currentFrame;       // 当前游戏帧
        double currentSeconds;  // 当前秒数
        QString phase;
        QString condition;
        QString lastStep;
        QStringList details;
    };

    explicit DebugSession(QObject *parent = nullptr);
    ~DebugSession();

    void SetSimulationOptions(const desktop::Config &config, int durationFrames, std::uint64_t seed = 0);
    qint64 TotalDamage();

    // 调试控制
    void Start(const QString &macroText);
    void Continue();
    void Pause();
    void Stop();
    void StepOver();
    void StepInto();
    void StepOut();

    // 断点管理
    void AddBreakpoint(int lineNumber);
    void RemoveBreakpoint(int lineNumber);
    void ClearBreakpoints();
    bool HasBreakpoint(int lineNumber) const;
    QSet<int> GetBreakpoints() const;

    // 状态查询
    State GetState() const { return m_state; }
    DebugInfo GetDebugInfo() const { return m_debugInfo; }
    QString GetLastError() const { return m_lastError; }

signals:
    void StateChanged(State newState);
    void DebugInfoUpdated(const DebugInfo &info);
    void ErrorOccurred(const QString &error);
    void LineChanged(int lineNumber);
    void ExecutionFinished();
    void DamageEventsAvailable(const CombatEvents &events);
    void BuffEventsAvailable(const BuffEvents &events);
    void MacroStepsAvailable(const MacroSteps &steps);
    void HistoryAvailable(const desktop::HistoryChunk &history);

private slots:
    void OnExecutionTimer();

private:
    void SetState(State newState);
    void UpdateDebugInfo();
    void ExecuteStep(bool (DebugSimulator::*step)());
    void Fail(const QString &message);
    void SimulateStep();

private:
    State m_state;
    QSet<int> m_breakpoints;
    DebugInfo m_debugInfo{};
    QString m_lastError;
    std::unique_ptr<DebugSimulator> m_simulator;  // JX3DPS模拟器
    QTimer *m_executionTimer;  // 用于持续执行的定时器
};

#endif // DEBUG_SESSION_H
