#ifndef DEBUG_SIMULATOR_H
#define DEBUG_SIMULATOR_H

#include <QString>
#include <QObject>
#include <memory>
#include <QStringList>
#include "combat_event.h"
#include "buff_trace.h"
#include "macro_step.h"
#include "combat_history.h"

#include "runtime_catalog.h"

/**
 * @brief 调试模拟器 - 连接DebugSession和JX3DPS引擎
 *
 * 负责：
 * Compiles macros once and exposes the debuggable runtime's state and events.
 */
class DebugSimulator : public QObject
{
    Q_OBJECT

public:
    explicit DebugSimulator(QObject *parent = nullptr);
    ~DebugSimulator();

    void SetOptions(const desktop::Config &config, int durationFrames, std::uint64_t seed = 0);

    // 初始化模拟器（解析宏，创建Player）
    bool Initialize(const QString &macroText, QString &errorMessage);

    // 单步进入（进入条件判定，逐个评估条件）
    bool StepInto();

    // 单步跳过（执行完整的一行指令）
    bool StepOver();

    // 单步跳出（跳出当前作用域）
    bool StepOut();

    // Run until a compiled breakpoint, the fight end, or the step budget.
    bool Continue(std::size_t maxSteps = 10000);

    bool IsPaused() const;
    bool BudgetExhausted() const;

    // Breakpoints are resolved against the compiled source map.
    bool SetBreakpoint(int sourceLine, bool enabled = true);

    // Compatibility alias for stepping one condition/action.
    bool StepOne();

    // 获取当前状态
    struct PlayerState {
        double lifePercent;
        double manaPercent;
        int qidian;
        int rage;
        int energy;
        int targetId;
        double targetLifePercent;
        int currentFrame;
        double currentSeconds;
        QString currentMacro;
        QString lastSkill;
        int currentMacroLine;  // 当前执行的宏行号（1-based）
        QString phase;
        QString condition;
        QString lastStep;
        QStringList details;
    };
    PlayerState GetPlayerState() const;
    CombatEvents TakeEvents();
    BuffEvents TakeBuffEvents();
    MacroSteps TakeMacroSteps();
    desktop::HistoryChunk TakeHistory();
    qint64 TotalDamage();

    // 检查是否执行完成
    bool IsFinished() const;

    // 重置模拟器
    void Reset();

private:
    struct Impl;
    std::unique_ptr<Impl> d;
};

#endif // DEBUG_SIMULATOR_H
