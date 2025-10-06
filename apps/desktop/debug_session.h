#ifndef DEBUG_SESSION_H
#define DEBUG_SESSION_H

#include <QObject>
#include <QSet>
#include <QString>
#include <QStringList>
#include <functional>
#include <memory>

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

    enum StepMode {
        NoStep,      // 不单步
        StepOverMode,   // 单步跳过
        StepIntoMode,   // 单步进入（宏切换时也停）
        StepOutMode     // 运行到宏结束
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
    };

    explicit DebugSession(QObject *parent = nullptr);
    ~DebugSession();

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

private:
    void SetState(State newState);
    void UpdateDebugInfo();
    bool ShouldBreak(int lineNumber);

    // 模拟执行（简化版本，实际需要与JX3DPS引擎集成）
    void SimulateStep();

private:
    State m_state;
    StepMode m_stepMode;
    QSet<int> m_breakpoints;
    DebugInfo m_debugInfo;
    QString m_lastError;
    QString m_macroText;
    QStringList m_macroLines;

    int m_currentLineIndex;  // 0-based 行索引
    int m_stepStartLine;     // 单步起始行

    std::unique_ptr<DebugSimulator> m_simulator;  // JX3DPS模拟器
};

#endif // DEBUG_SESSION_H
