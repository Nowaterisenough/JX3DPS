#include "diagnostics.h"
#include "debug_session.h"
#include "debug_simulator.h"

DebugSession::DebugSession(QObject *parent)
    : QObject(parent), m_state(Stopped), m_simulator(std::make_unique<DebugSimulator>()),
      m_executionTimer(new QTimer(this))
{
    m_debugInfo.lineNumber = -1;
    m_executionTimer->setInterval(16);
    connect(m_executionTimer, &QTimer::timeout, this, &DebugSession::OnExecutionTimer);
}

DebugSession::~DebugSession() = default;

void DebugSession::SetSimulationOptions(const desktop::Config &config, int durationFrames,
                                        std::uint64_t seed)
{
    m_simulator->SetOptions(config, durationFrames, seed);
}

qint64 DebugSession::TotalDamage() { return m_simulator->TotalDamage(); }

void DebugSession::Start(const QString &macroText)
{
    Stop();
    QString error;
    if (!m_simulator->Initialize(macroText, error)) {
        Fail(error);
        return;
    }
    for (const int line : m_breakpoints) m_simulator->SetBreakpoint(line, true);
    m_lastError.clear();
    UpdateDebugInfo();
    SetState(Paused);
}

void DebugSession::Continue()
{
    if (m_state != Paused) return;
    SetState(Running);
    m_executionTimer->start();
}

void DebugSession::Pause()
{
    if (m_state != Running) return;
    m_executionTimer->stop();
    SetState(Paused);
}

void DebugSession::Stop()
{
    m_executionTimer->stop();
    m_simulator->Reset();
    m_debugInfo = {};
    m_debugInfo.lineNumber = -1;
    SetState(Stopped);
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
    if (m_state == Stopped || m_state == Finished) return;
    m_executionTimer->stop();
    try {
        (m_simulator.get()->*step)();
        UpdateDebugInfo();
        SetState(m_simulator->IsFinished() ? Finished : Paused);
        if (m_state == Finished) emit ExecutionFinished();
    } catch (const std::exception &error) {
        Fail(desktop::Diagnostic(QString::fromUtf8(error.what())));
    }
}

void DebugSession::StepOver() { ExecuteStep(&DebugSimulator::StepOver); }
void DebugSession::StepInto() { ExecuteStep(&DebugSimulator::StepInto); }
void DebugSession::StepOut() { ExecuteStep(&DebugSimulator::StepOut); }

void DebugSession::AddBreakpoint(int lineNumber)
{
    if (lineNumber < 1) return;
    m_breakpoints.insert(lineNumber);
    m_simulator->SetBreakpoint(lineNumber, true);
}

void DebugSession::RemoveBreakpoint(int lineNumber)
{
    m_breakpoints.remove(lineNumber);
    m_simulator->SetBreakpoint(lineNumber, false);
}

void DebugSession::ClearBreakpoints()
{
    for (const int line : m_breakpoints) m_simulator->SetBreakpoint(line, false);
    m_breakpoints.clear();
}

bool DebugSession::HasBreakpoint(int lineNumber) const { return m_breakpoints.contains(lineNumber); }
QSet<int> DebugSession::GetBreakpoints() const { return m_breakpoints; }

void DebugSession::SetState(State newState)
{
    if (m_state == newState) return;
    m_state = newState;
    emit StateChanged(newState);
}

void DebugSession::UpdateDebugInfo()
{
    const auto player = m_simulator->GetPlayerState();
    m_debugInfo.lineNumber = player.currentMacroLine;
    m_debugInfo.frameIndex = player.currentFrame;
    m_debugInfo.currentFrame = player.currentFrame;
    m_debugInfo.currentSeconds = player.currentSeconds;
    m_debugInfo.lifePercent = player.lifePercent;
    m_debugInfo.manaPercent = player.manaPercent;
    m_debugInfo.qidian = player.qidian;
    m_debugInfo.rage = player.rage;
    m_debugInfo.energy = player.energy;
    m_debugInfo.targetId = player.targetId;
    m_debugInfo.targetLifePercent = player.targetLifePercent;
    m_debugInfo.currentMacro = player.currentMacro;
    m_debugInfo.currentSkill = player.lastSkill;
    m_debugInfo.phase = player.phase;
    m_debugInfo.condition = player.condition;
    m_debugInfo.lastStep = player.lastStep;
    m_debugInfo.details = player.details;
    const auto events = m_simulator->TakeEvents();
    if (!events.isEmpty()) emit DamageEventsAvailable(events);
    const auto buffs = m_simulator->TakeBuffEvents();
    if (!buffs.isEmpty()) emit BuffEventsAvailable(buffs);
    const auto steps = m_simulator->TakeMacroSteps();
    if (!steps.isEmpty()) emit MacroStepsAvailable(steps);
    emit HistoryAvailable(m_simulator->TakeHistory());
    emit DebugInfoUpdated(m_debugInfo);
    emit LineChanged(m_debugInfo.lineNumber);
}

void DebugSession::SimulateStep()
{
    if (m_state != Running) return;
    try {
        m_simulator->Continue(2000);
        UpdateDebugInfo();
        if (m_simulator->IsFinished()) {
            m_executionTimer->stop();
            SetState(Finished);
            emit ExecutionFinished();
        } else if (m_simulator->IsPaused()) {
            m_executionTimer->stop();
            SetState(Paused);
        }
    } catch (const std::exception &error) {
        Fail(desktop::Diagnostic(QString::fromUtf8(error.what())));
    }
}

void DebugSession::OnExecutionTimer() { SimulateStep(); }
