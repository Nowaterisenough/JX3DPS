#ifndef DEBUG_SESSION_H
#define DEBUG_SESSION_H

#include <functional>
#include <memory>

#include <QObject>
#include <QSet>
#include <QString>
#include <QStringList>
#include <QTimer>

#include "buff_trace.h"
#include "combat_event.h"
#include "combat_history.h"
#include "macro_step.h"
#include "runtime_catalog.h"

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
    enum State
    {
        STOPPED, // 未启动
        RUNNING, // 运行中
        PAUSED,  // 已暂停
        FINISHED // 执行完成
    };

    struct DebugInfo
    {
        int     lineNumber{}; // 当前执行行号（1-based）
        int     frameIndex{}; // 当前帧索引
        QString currentMacro; // 当前宏名称
        QString currentSkill; // 当前技能名称

        // 玩家状态
        double lifePercent{};
        double manaPercent{};
        int    qidian{};
        int    rage{};
        int    energy{};

        // 目标状态
        int    targetId{};
        double targetLifePercent{};

        // 时间
        int         currentFrame{};   // 当前游戏帧
        double      currentSeconds{}; // 当前秒数
        QString     phase;
        QString     condition;
        QString     lastStep;
        QStringList details;
    };

    explicit DebugSession(QObject *parent = nullptr);
    ~DebugSession() override;

    void   SetSimulationOptions(const desktop::Config &config, int duration_frames, std::uint64_t seed = 0);
    qint64 TotalDamage();

    // 调试控制
    void Start(const QString &macro_text);
    void Continue();
    void Pause();
    void Stop();
    void StepOver();
    void StepInto();
    void StepOut();

    // 断点管理
    void      AddBreakpoint(int line_number);
    void      RemoveBreakpoint(int line_number);
    void      ClearBreakpoints();
    bool      HasBreakpoint(int line_number) const;
    QSet<int> GetBreakpoints() const;

    // 状态查询
    State GetState() const { return m_state; }

    DebugInfo GetDebugInfo() const { return m_debugInfo; }

    QString GetLastError() const { return m_lastError; }

signals:
    void StateChanged(State new_state);
    void DebugInfoUpdated(const DebugInfo &info);
    void ErrorOccurred(const QString &error);
    void LineChanged(int line_number);
    void ExecutionFinished();
    void DamageEventsAvailable(const CombatEvents &events);
    void BuffEventsAvailable(const BuffEvents &events);
    void MacroStepsAvailable(const MacroSteps &steps);
    void HistoryAvailable(const desktop::HistoryChunk &history);

private slots:
    void OnExecutionTimer();

private:
    void SetState(State new_state);
    void UpdateDebugInfo();
    void ExecuteStep(bool (DebugSimulator::*step)());
    void Fail(const QString &message);
    void SimulateStep();

    State                           m_state{ STOPPED };
    QSet<int>                       m_breakpoints;
    DebugInfo                       m_debugInfo{};
    QString                         m_lastError;
    std::unique_ptr<DebugSimulator> m_simulator;      // JX3DPS模拟器
    QTimer                         *m_executionTimer; // 用于持续执行的定时器
};

#endif // DEBUG_SESSION_H
