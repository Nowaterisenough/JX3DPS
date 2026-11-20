#include "debug_session.h"

#include <memory>

#include "debug_simulator.h"
#include "diagnostics.h"

DebugSession::DebugSession(QObject *parent) :
    QObject(parent), m_simulator(std::make_unique<DebugSimulator>()), m_executionTimer(new QTimer(this))
{
    m_debugInfo.lineNumber = -1;
    m_executionTimer->setInterval(16);
    connect(m_executionTimer, &QTimer::timeout, this, &DebugSession::OnExecutionTimer);
}

DebugSession::~DebugSession() = default;

void DebugSession::SetSimulationOptions(const desktop::Config &config, int duration_frames, std::uint64_t seed)
{
    m_simulator->SetOptions(config, duration_frames, seed);
}

qint64 DebugSession::TotalDamage()
{
    return m_simulator->TotalDamage();
}

void DebugSession::Start(const QString &macro_text)
{
    Stop();
    QString error;
    if (!m_simulator->Initialize(macro_text, error)) {
        Fail(error);
        return;
    }
    for (const int LINE : m_breakpoints) {
        m_simulator->SetBreakpoint(LINE, true);
    }
    m_lastError.clear();
    UpdateDebugInfo();
    SetState(PAUSED);
}

void DebugSession::Continue()
{
    if (m_state != PAUSED) {
        return;
    }
    SetState(RUNNING);
    m_executionTimer->start();
}

void DebugSession::Pause()
{
    if (m_state != RUNNING) {
        return;
    }
    m_executionTimer->stop();
    SetState(PAUSED);
}

void DebugSession::Stop()
{
    m_executionTimer->stop();
    m_simulator->Reset();
    m_debugInfo            = {};
    m_debugInfo.lineNumber = -1;
    SetState(STOPPED);
    emit LineChanged(-1);
}

void DebugSession::Fail(const QString &message)
{
    Stop();
    m_lastError = message;
    emit ErrorOccurred(message);
}

void DebugSession::ExecuteStep(bool (DebugSimulator::*step)())
{
    if (m_state == STOPPED || m_state == FINISHED) {
        return;
    }
    m_executionTimer->stop();
    try {
        (m_simulator.get()->*step)();
        UpdateDebugInfo();
        SetState(m_simulator->IsFinished() ? FINISHED : PAUSED);
        if (m_state == FINISHED) {
            emit ExecutionFinished();
        }
    } catch (const std::exception &error) {
        Fail(desktop::Diagnostic(QString::fromUtf8(error.what())));
    }
}

void DebugSession::StepOver()
{
    ExecuteStep(&DebugSimulator::StepOver);
}

void DebugSession::StepInto()
{
    ExecuteStep(&DebugSimulator::StepInto);
}

void DebugSession::StepOut()
{
    ExecuteStep(&DebugSimulator::StepOut);
}

void DebugSession::AddBreakpoint(int line_number)
{
    if (line_number < 1) {
        return;
    }
    m_breakpoints.insert(line_number);
    m_simulator->SetBreakpoint(line_number, true);
}

void DebugSession::RemoveBreakpoint(int line_number)
{
    m_breakpoints.remove(line_number);
    m_simulator->SetBreakpoint(line_number, false);
}

void DebugSession::ClearBreakpoints()
{
    for (const int LINE : m_breakpoints) {
        m_simulator->SetBreakpoint(LINE, false);
    }
    m_breakpoints.clear();
}

bool DebugSession::HasBreakpoint(int line_number) const
{
    return m_breakpoints.contains(line_number);
}

QSet<int> DebugSession::GetBreakpoints() const
{
    return m_breakpoints;
}

void DebugSession::SetState(State new_state)
{
    if (m_state == new_state) {
        return;
    }
    m_state = new_state;
    emit StateChanged(new_state);
}

void DebugSession::UpdateDebugInfo()
{
    const auto PLAYER             = m_simulator->GetPlayerState();
    m_debugInfo.lineNumber        = PLAYER.currentMacroLine;
    m_debugInfo.frameIndex        = PLAYER.currentFrame;
    m_debugInfo.currentFrame      = PLAYER.currentFrame;
    m_debugInfo.currentSeconds    = PLAYER.currentSeconds;
    m_debugInfo.lifePercent       = PLAYER.lifePercent;
    m_debugInfo.manaPercent       = PLAYER.manaPercent;
    m_debugInfo.qidian            = PLAYER.qidian;
    m_debugInfo.rage              = PLAYER.rage;
    m_debugInfo.energy            = PLAYER.energy;
    m_debugInfo.targetId          = PLAYER.targetId;
    m_debugInfo.targetLifePercent = PLAYER.targetLifePercent;
    m_debugInfo.currentMacro      = PLAYER.currentMacro;
    m_debugInfo.currentSkill      = PLAYER.lastSkill;
    m_debugInfo.phase             = PLAYER.phase;
    m_debugInfo.condition         = PLAYER.condition;
    m_debugInfo.lastStep          = PLAYER.lastStep;
    m_debugInfo.details           = PLAYER.details;
    const auto EVENTS             = m_simulator->TakeEvents();
    if (!EVENTS.isEmpty()) {
        emit DamageEventsAvailable(EVENTS);
    }
    const auto BUFFS = m_simulator->TakeBuffEvents();
    if (!BUFFS.isEmpty()) {
        emit BuffEventsAvailable(BUFFS);
    }
    const auto STEPS = m_simulator->TakeMacroSteps();
    if (!STEPS.isEmpty()) {
        emit MacroStepsAvailable(STEPS);
    }
    emit HistoryAvailable(m_simulator->TakeHistory());
    emit DebugInfoUpdated(m_debugInfo);
    emit LineChanged(m_debugInfo.lineNumber);
}

void DebugSession::SimulateStep()
{
    if (m_state != RUNNING) {
        return;
    }
    try {
        m_simulator->Continue(2000);
        UpdateDebugInfo();
        if (m_simulator->IsFinished()) {
            m_executionTimer->stop();
            SetState(FINISHED);
            emit ExecutionFinished();
        } else if (m_simulator->IsPaused()) {
            m_executionTimer->stop();
            SetState(PAUSED);
        }
    } catch (const std::exception &error) {
        Fail(desktop::Diagnostic(QString::fromUtf8(error.what())));
    }
}

void DebugSession::OnExecutionTimer()
{
    SimulateStep();
}
