#include "debug_session.h"
#include "debug_simulator.h"
#include <QDebug>
#include <QRegularExpression>

DebugSession::DebugSession(QObject *parent)
    : QObject(parent)
    , m_state(Stopped)
    , m_stepMode(NoStep)
    , m_currentLineIndex(-1)
    , m_stepStartLine(-1)
    , m_simulator(std::make_unique<DebugSimulator>())
    , m_executionTimer(new QTimer(this))
{
    m_debugInfo = {};

    // 配置执行定时器
    m_executionTimer->setInterval(10);  // 10ms一次，模拟快速执行
    connect(m_executionTimer, &QTimer::timeout, this, &DebugSession::OnExecutionTimer);
}

DebugSession::~DebugSession() = default;

void DebugSession::Start(const QString &macroText)
{
    if (m_state != Stopped) {
        Stop();
    }

    m_macroText = macroText;
    m_macroLines = macroText.split('\n');
    m_currentLineIndex = 0;
    m_stepMode = NoStep;

    // 初始化JX3DPS模拟器
    QString errorMsg;
    if (!m_simulator->Initialize(macroText, errorMsg)) {
        m_lastError = errorMsg;
        emit ErrorOccurred(errorMsg);
        SetState(Stopped);
        qDebug() << "模拟器初始化失败:" << errorMsg;
        return;
    }

    // 初始化调试信息（从模拟器获取真实状态）
    auto playerState = m_simulator->GetPlayerState();
    m_debugInfo = {};
    m_debugInfo.lineNumber = 1;
    m_debugInfo.frameIndex = 0;
    m_debugInfo.currentFrame = playerState.currentFrame;
    m_debugInfo.currentSeconds = playerState.currentSeconds;
    m_debugInfo.lifePercent = playerState.lifePercent;
    m_debugInfo.manaPercent = playerState.manaPercent;
    m_debugInfo.qidian = playerState.qidian;
    m_debugInfo.rage = playerState.rage;
    m_debugInfo.energy = playerState.energy;
    m_debugInfo.targetId = playerState.targetId;
    m_debugInfo.targetLifePercent = playerState.targetLifePercent;
    m_debugInfo.currentMacro = playerState.currentMacro;
    m_debugInfo.currentSkill = playerState.lastSkill;

    SetState(Paused);  // 启动后立即暂停，等待用户操作
    emit DebugInfoUpdated(m_debugInfo);

    qDebug() << "调试会话已启动（使用真实JX3DPS引擎）";
}

void DebugSession::Continue()
{
    if (m_state != Paused) {
        return;
    }

    m_stepMode = NoStep;
    SetState(Running);
    m_executionTimer->start();  // 启动持续执行

    qDebug() << "调试继续执行";
}

void DebugSession::Pause()
{
    if (m_state != Running) {
        return;
    }

    m_executionTimer->stop();  // 停止持续执行
    SetState(Paused);
    qDebug() << "调试已暂停";
}

void DebugSession::Stop()
{
    m_executionTimer->stop();  // 停止持续执行
    m_currentLineIndex = -1;
    m_stepMode = NoStep;
    m_breakpoints.clear();
    SetState(Stopped);

    qDebug() << "调试已停止";
}

void DebugSession::StepOver()
{
    if (m_state != Paused && m_state != Stopped) {
        return;
    }

    if (m_state == Stopped) {
        m_lastError = "请先启动调试会话";
        emit ErrorOccurred(m_lastError);
        return;
    }

    m_stepMode = StepOverMode;
    m_stepStartLine = m_debugInfo.lineNumber;
    SimulateStep();

    qDebug() << "单步跳过：从行" << m_stepStartLine;
}

void DebugSession::StepInto()
{
    if (m_state != Paused && m_state != Stopped) {
        return;
    }

    if (m_state == Stopped) {
        m_lastError = "请先启动调试会话";
        emit ErrorOccurred(m_lastError);
        return;
    }

    m_stepMode = StepIntoMode;
    SimulateStep();

    qDebug() << "单步进入";
}

void DebugSession::StepOut()
{
    if (m_state != Paused) {
        return;
    }

    m_stepMode = StepOutMode;
    SetState(Running);

    qDebug() << "单步跳出";
}

void DebugSession::AddBreakpoint(int lineNumber)
{
    m_breakpoints.insert(lineNumber);
    qDebug() << "添加断点：行" << lineNumber;
}

void DebugSession::RemoveBreakpoint(int lineNumber)
{
    m_breakpoints.remove(lineNumber);
    qDebug() << "移除断点：行" << lineNumber;
}

void DebugSession::ClearBreakpoints()
{
    m_breakpoints.clear();
    qDebug() << "清除所有断点";
}

bool DebugSession::HasBreakpoint(int lineNumber) const
{
    return m_breakpoints.contains(lineNumber);
}

QSet<int> DebugSession::GetBreakpoints() const
{
    return m_breakpoints;
}

void DebugSession::SetState(State newState)
{
    if (m_state != newState) {
        m_state = newState;
        emit StateChanged(newState);
    }
}

void DebugSession::UpdateDebugInfo()
{
    emit DebugInfoUpdated(m_debugInfo);
    emit LineChanged(m_debugInfo.lineNumber);
}

bool DebugSession::ShouldBreak(int lineNumber)
{
    // 检查断点
    if (m_breakpoints.contains(lineNumber)) {
        return true;
    }

    // 检查单步模式
    if (m_stepMode == StepOverMode || m_stepMode == StepIntoMode) {
        return true;
    }

    return false;
}

void DebugSession::SimulateStep()
{
    // 检查模拟器是否完成
    if (m_simulator->IsFinished()) {
        SetState(Finished);
        emit ExecutionFinished();
        qDebug() << "模拟器执行完成";
        return;
    }

    // 执行真实的模拟器步骤
    if (!m_simulator->StepOne()) {
        qDebug() << "模拟器执行步骤失败";
    }

    // 从模拟器获取真实玩家状态（包括当前行号）
    auto playerState = m_simulator->GetPlayerState();
    m_debugInfo.lineNumber = playerState.currentMacroLine;  // 使用模拟器返回的行号
    m_debugInfo.currentFrame = playerState.currentFrame;
    m_debugInfo.currentSeconds = playerState.currentSeconds;
    m_debugInfo.lifePercent = playerState.lifePercent;
    m_debugInfo.manaPercent = playerState.manaPercent;
    m_debugInfo.qidian = playerState.qidian;
    m_debugInfo.rage = playerState.rage;
    m_debugInfo.energy = playerState.energy;
    m_debugInfo.targetId = playerState.targetId;
    m_debugInfo.targetLifePercent = playerState.targetLifePercent;
    m_debugInfo.currentMacro = playerState.currentMacro;
    m_debugInfo.currentSkill = playerState.lastSkill;

    // 检查是否应该暂停
    if (ShouldBreak(m_debugInfo.lineNumber)) {
        SetState(Paused);
        m_stepMode = NoStep;
    }

    UpdateDebugInfo();

    // 获取当前行文本用于日志
    QString currentLine;
    int lineIndex = m_debugInfo.lineNumber - 1;
    if (lineIndex >= 0 && lineIndex < m_macroLines.size()) {
        currentLine = m_macroLines[lineIndex].trimmed();
    }

    qDebug() << "执行行" << m_debugInfo.lineNumber << ":" << currentLine
             << "| 帧:" << m_debugInfo.currentFrame
             << "| 气点:" << m_debugInfo.qidian
             << "| 技能:" << m_debugInfo.currentSkill;
}

void DebugSession::OnExecutionTimer()
{
    if (m_state != Running) {
        m_executionTimer->stop();
        return;
    }

    SimulateStep();

    // 如果被断点暂停，停止定时器
    if (m_state == Paused) {
        m_executionTimer->stop();
    }
}
