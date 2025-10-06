#include "debug_simulator.h"
#include "script_interpreter.h"
#include <QDebug>

struct DebugSimulator::Impl
{
    ScriptInterpreter *interpreter = nullptr;
};

DebugSimulator::DebugSimulator(QObject *parent)
    : QObject(parent)
    , d(std::make_unique<Impl>())
{
    d->interpreter = new ScriptInterpreter(this);
}

DebugSimulator::~DebugSimulator()
{
    Reset();
}

bool DebugSimulator::Initialize(const QString &macroText, QString &errorMessage)
{
    Reset();

    qDebug() << "[DebugSimulator] 使用 ScriptInterpreter 初始化";

    if (!d->interpreter->Initialize(macroText, errorMessage)) {
        qDebug() << "[DebugSimulator] 初始化失败:" << errorMessage;
        return false;
    }

    qDebug() << "[DebugSimulator] 初始化成功，解析了"
             << d->interpreter->GetScriptLines().size() << "条脚本指令";

    return true;
}

bool DebugSimulator::StepOne()
{
    // 保留兼容性，调用 StepOver
    return StepOver();
}

bool DebugSimulator::StepInto()
{
    if (!d->interpreter) {
        qDebug() << "[DebugSimulator::StepInto] interpreter 为空";
        return false;
    }

    auto stateBefore = d->interpreter->GetState();
    qDebug() << "\n[DebugSimulator::StepInto] 执行前状态:"
             << "行号=" << stateBefore.currentLine
             << "步骤=" << (int)stateBefore.stepType
             << "条件索引=" << stateBefore.currentConditionIndex;

    bool result = d->interpreter->StepInto();

    auto stateAfter = d->interpreter->GetState();
    qDebug() << "[DebugSimulator::StepInto] 执行后状态:"
             << "行号=" << stateAfter.currentLine
             << "步骤=" << (int)stateAfter.stepType
             << "条件索引=" << stateAfter.currentConditionIndex
             << "最后技能=" << stateAfter.lastSkill;

    return result;
}

bool DebugSimulator::StepOver()
{
    if (!d->interpreter) {
        qDebug() << "[DebugSimulator::StepOver] interpreter 为空";
        return false;
    }

    auto stateBefore = d->interpreter->GetState();
    qDebug() << "\n[DebugSimulator::StepOver] 执行前行号:" << stateBefore.currentLine;

    bool result = d->interpreter->StepOver();

    auto stateAfter = d->interpreter->GetState();
    qDebug() << "[DebugSimulator::StepOver] 执行后行号:" << stateAfter.currentLine;
    qDebug() << "[DebugSimulator::StepOver] 最后技能:" << stateAfter.lastSkill;

    return result;
}

bool DebugSimulator::StepOut()
{
    if (!d->interpreter) {
        qDebug() << "[DebugSimulator::StepOut] interpreter 为空";
        return false;
    }

    qDebug() << "\n[DebugSimulator::StepOut] 跳出作用域";

    bool result = d->interpreter->StepOut();

    return result;
}

DebugSimulator::PlayerState DebugSimulator::GetPlayerState() const
{
    PlayerState state = {};

    if (d->interpreter) {
        auto interpState = d->interpreter->GetState();

        state.currentMacroLine = interpState.currentLine;
        state.lastSkill = interpState.lastSkill;

        // 简化版状态（ScriptInterpreter 有模拟的玩家状态）
        state.lifePercent = 1.0;
        state.manaPercent = 1.0;
        state.qidian = 10;
        state.rage = 0;
        state.energy = 0;
        state.targetId = 1;
        state.targetLifePercent = 1.0;
        state.currentFrame = 0;
        state.currentSeconds = 0.0;
        state.currentMacro = "循环";

        qDebug() << "[DebugSimulator::GetPlayerState] 返回行号:" << state.currentMacroLine;
    }

    return state;
}

bool DebugSimulator::IsFinished() const
{
    if (!d->interpreter) {
        return true;
    }
    return d->interpreter->GetState().finished;
}

void DebugSimulator::Reset()
{
    if (d->interpreter) {
        d->interpreter->Reset();
    }
}
