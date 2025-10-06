#include "script_interpreter.h"
#include <QDebug>
#include <QRegularExpression>

struct ScriptInterpreter::Impl
{
    std::vector<ScriptInterpreter::ScriptLine> scriptLines;
    int currentLine = 1;
    QString lastSkill;
    bool finished = false;
    QString errorMessage;

    // 模拟的玩家状态（简化版）
    struct PlayerState {
        int qidian = 10;         // 气点
        double lifePercent = 1.0; // 生命百分比
        double manaPercent = 1.0; // 内力百分比
        int targetId = 1;
        double targetLifePercent = 1.0;
    } player;
};

ScriptInterpreter::ScriptInterpreter(QObject *parent)
    : QObject(parent)
    , d(std::make_unique<Impl>())
{
}

ScriptInterpreter::~ScriptInterpreter() = default;

bool ScriptInterpreter::Initialize(const QString &macroText, QString &errorMessage)
{
    Reset();

    QStringList lines = macroText.split('\n');

    // 解析每一行
    for (int i = 0; i < lines.size(); ++i) {
        QString line = lines[i].trimmed();
        int lineNumber = i + 1;

        // 跳过空行和注释
        if (line.isEmpty() || line.startsWith('#')) {
            continue;
        }

        // 跳过宏定义行
        if (line.startsWith("macro ")) {
            continue;
        }

        // 解析指令行
        ScriptLine scriptLine = ParseLine(lineNumber, line);
        if (!scriptLine.command.isEmpty()) {
            d->scriptLines.push_back(scriptLine);
        }
    }

    qDebug() << "脚本解释器初始化完成，解析了" << d->scriptLines.size() << "条指令";

    if (d->scriptLines.empty()) {
        errorMessage = "未找到有效的脚本指令";
        return false;
    }

    return true;
}

ScriptInterpreter::ScriptLine ScriptInterpreter::ParseLine(int lineNumber, const QString &line)
{
    ScriptLine scriptLine;
    scriptLine.lineNumber = lineNumber;
    scriptLine.rawLine = line;

    // 使用正则表达式解析指令
    // 格式: /cast [条件] 技能名
    // 格式: /scast [条件] 技能名
    // 格式: /switch [条件] 宏名

    QRegularExpression re(R"(^(/\w+)\s+(?:\[(.*?)\]\s+)?(.+)$)");
    QRegularExpressionMatch match = re.match(line);

    if (match.hasMatch()) {
        scriptLine.command = match.captured(1);      // /cast, /scast, etc.
        QString conditions = match.captured(2);       // 条件部分
        scriptLine.skillName = match.captured(3);     // 技能名

        // 解析条件列表
        if (!conditions.isEmpty()) {
            // 条件可能用逗号或&分隔
            QStringList condList = conditions.split(QRegularExpression("[,&]"), Qt::SkipEmptyParts);
            for (auto &cond : condList) {
                scriptLine.conditions.append(cond.trimmed());
            }
        }
    } else {
        qDebug() << "无法解析行" << lineNumber << ":" << line;
    }

    return scriptLine;
}

bool ScriptInterpreter::StepOne()
{
    if (d->finished) {
        return false;
    }

    // 找到当前行号对应的指令
    ScriptLine *currentScript = nullptr;
    for (auto &script : d->scriptLines) {
        if (script.lineNumber == d->currentLine) {
            currentScript = &script;
            break;
        }
    }

    if (!currentScript) {
        // 当前行没有指令，跳到下一行
        d->currentLine++;
        if (d->currentLine > 100) { // 防止无限循环
            d->finished = true;
            return false;
        }
        return StepOne(); // 递归查找下一条指令
    }

    qDebug() << "执行行" << currentScript->lineNumber << ":" << currentScript->rawLine;

    // 评估所有条件
    bool allConditionsMet = true;
    for (const QString &condition : currentScript->conditions) {
        if (!EvaluateCondition(condition)) {
            qDebug() << "  条件不满足:" << condition;
            allConditionsMet = false;
            break;
        }
    }

    if (allConditionsMet) {
        // 执行指令
        if (currentScript->command == "/cast" || currentScript->command == "/scast") {
            if (CastSkill(currentScript->skillName)) {
                qDebug() << "  成功施放技能:" << currentScript->skillName;
                d->lastSkill = currentScript->skillName;
            }
        } else if (currentScript->command == "/switch") {
            qDebug() << "  切换到宏:" << currentScript->skillName;
            // TODO: 实现宏切换
        }
    } else {
        qDebug() << "  条件不满足，跳过";
    }

    // 移动到下一行
    d->currentLine++;

    // 找到下一条有效指令
    bool foundNext = false;
    for (const auto &script : d->scriptLines) {
        if (script.lineNumber >= d->currentLine) {
            d->currentLine = script.lineNumber;
            foundNext = true;
            break;
        }
    }

    if (!foundNext) {
        d->finished = true;
        qDebug() << "脚本执行完成";
    }

    return true;
}

bool ScriptInterpreter::EvaluateCondition(const QString &condition)
{
    // 简化版条件评估
    // 支持格式：
    // - qidian>5
    // - qidian>=10
    // - tlife:1<0.4 (目标1生命小于40%)
    // - skill_cd:技能名<2 (技能冷却小于2秒)

    QRegularExpression re(R"(^(\w+)([><]=?)(.+)$)");
    QRegularExpressionMatch match = re.match(condition);

    if (!match.hasMatch()) {
        qDebug() << "无法解析条件:" << condition;
        return true; // 无法解析的条件默认为真
    }

    QString var = match.captured(1);
    QString op = match.captured(2);
    QString value = match.captured(3);

    // 获取变量值
    double varValue = 0;
    if (var == "qidian") {
        varValue = d->player.qidian;
    } else if (var == "life") {
        varValue = d->player.lifePercent;
    } else if (var == "mana") {
        varValue = d->player.manaPercent;
    } else if (var.startsWith("tlife:")) {
        varValue = d->player.targetLifePercent;
    } else if (var.startsWith("skill_cd:")) {
        // TODO: 查询技能冷却
        varValue = 0; // 假设技能已冷却
    } else {
        qDebug() << "未知的变量:" << var;
        return true;
    }

    // 转换值
    double compareValue = value.toDouble();

    // 比较
    if (op == ">") {
        return varValue > compareValue;
    } else if (op == ">=") {
        return varValue >= compareValue;
    } else if (op == "<") {
        return varValue < compareValue;
    } else if (op == "<=") {
        return varValue <= compareValue;
    } else if (op == "=") {
        return qAbs(varValue - compareValue) < 0.001;
    }

    return true;
}

bool ScriptInterpreter::CastSkill(const QString &skillName)
{
    // 简化版技能施放
    // TODO: 调用 JX3DPS 引擎的技能施放
    qDebug() << "  [模拟] 施放技能:" << skillName;

    // 模拟消耗气点
    if (d->player.qidian > 0) {
        d->player.qidian--;
    }

    return true;
}

ScriptInterpreter::ExecutionState ScriptInterpreter::GetState() const
{
    ExecutionState state;
    state.currentLine = d->currentLine;
    state.lastSkill = d->lastSkill;
    state.finished = d->finished;
    state.errorMessage = d->errorMessage;
    return state;
}

void ScriptInterpreter::Reset()
{
    d->scriptLines.clear();
    d->currentLine = 1;
    d->lastSkill.clear();
    d->finished = false;
    d->errorMessage.clear();
    d->player.qidian = 10;
    d->player.lifePercent = 1.0;
    d->player.manaPercent = 1.0;
}

const std::vector<ScriptInterpreter::ScriptLine>& ScriptInterpreter::GetScriptLines() const
{
    return d->scriptLines;
}
