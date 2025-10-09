#include "script_interpreter.h"

#include <unordered_map>

#include <QDebug>
#include <QRegularExpression>

#include "src/core/context.h"
#include "src/class/tai_xu_jian_yi/tai_xu_jian_yi_ids.h"

struct ScriptInterpreter::Impl
{
    std::vector<ScriptInterpreter::ScriptLine> scriptLines;
    int                                        currentLine = 1;
    QString                                    lastSkill;
    bool                                       finished = false;
    QString                                    errorMessage;

    // 调试状态
    ScriptInterpreter::DebugStepType stepType              = ScriptInterpreter::DebugStepType::LINE;
    int                              currentConditionIndex = -1; // 当前正在评估的条件索引
    QString                          currentCondition;           // 当前条件文本
    bool                             conditionResult    = false; // 当前条件结果
    int                              currentScriptIndex = -1;    // 当前脚本行在 scriptLines 中的索引

    // 使用 JX3DPS 核心库的 SimContext
    JX3DPS::SimContext simContext;

    // 技能和BUFF名称到ID的映射（使用太虚剑意的ID）
    std::unordered_map<std::string, JX3DPS::jx3id_t> skillNameToId;
    std::unordered_map<std::string, JX3DPS::jx3id_t> buffNameToId;

    // 简化的玩家状态（用于UI显示）
    struct PlayerState
    {
        int    qidian            = 10;  // 气点
        double lifePercent       = 1.0; // 生命百分比
        double manaPercent       = 1.0; // 内力百分比
        int    targetId          = 1;
        double targetLifePercent = 1.0;
    } player;

    // 初始化技能和BUFF映射
    void InitializeIdMaps()
    {
        using namespace JX3DPS::太虚剑意;

        // 技能映射
        skillNameToId["破招"]     = static_cast<JX3DPS::jx3id_t>(SkillId::破招);
        skillNameToId["无我无剑"] = static_cast<JX3DPS::jx3id_t>(SkillId::无我无剑);
        skillNameToId["八荒归元"] = static_cast<JX3DPS::jx3id_t>(SkillId::八荒归元);
        skillNameToId["三环套月"] = static_cast<JX3DPS::jx3id_t>(SkillId::三环套月);
        skillNameToId["万剑归宗"] = static_cast<JX3DPS::jx3id_t>(SkillId::万剑归宗);
        skillNameToId["人剑合一"] = static_cast<JX3DPS::jx3id_t>(SkillId::人剑合一);
        skillNameToId["三柴剑法"] = static_cast<JX3DPS::jx3id_t>(SkillId::三柴剑法);
        skillNameToId["生太极"]   = static_cast<JX3DPS::jx3id_t>(SkillId::生太极);
        skillNameToId["碎星辰"]   = static_cast<JX3DPS::jx3id_t>(SkillId::碎星辰);
        skillNameToId["吞日月"]   = static_cast<JX3DPS::jx3id_t>(SkillId::吞日月);
        skillNameToId["紫气东来"] = static_cast<JX3DPS::jx3id_t>(SkillId::紫气东来);
        skillNameToId["镜花影"]   = static_cast<JX3DPS::jx3id_t>(SkillId::镜花影);

        // BUFF映射
        buffNameToId["叠刃"]     = static_cast<JX3DPS::jx3id_t>(BuffId::叠刃);
        buffNameToId["万象归元"] = static_cast<JX3DPS::jx3id_t>(BuffId::万象归元);
        buffNameToId["人剑合一"] = static_cast<JX3DPS::jx3id_t>(BuffId::人剑合一);
        buffNameToId["紫气东来"] = static_cast<JX3DPS::jx3id_t>(BuffId::紫气东来);
        buffNameToId["玄门"]     = static_cast<JX3DPS::jx3id_t>(BuffId::玄门);
        buffNameToId["风势"]     = static_cast<JX3DPS::jx3id_t>(BuffId::风势);
        buffNameToId["裂云"]     = static_cast<JX3DPS::jx3id_t>(BuffId::裂云);
        buffNameToId["气盛"]     = static_cast<JX3DPS::jx3id_t>(BuffId::气盛);
        buffNameToId["剑入"]     = static_cast<JX3DPS::jx3id_t>(BuffId::剑入);
        buffNameToId["镜花影"]   = static_cast<JX3DPS::jx3id_t>(BuffId::镜花影);
        buffNameToId["持盈"]     = static_cast<JX3DPS::jx3id_t>(BuffId::持盈);
    }
};

ScriptInterpreter::ScriptInterpreter(QObject *parent) : QObject(parent), d(std::make_unique<Impl>())
{
    d->InitializeIdMaps();
}

ScriptInterpreter::~ScriptInterpreter() = default;

bool ScriptInterpreter::Initialize(const QString &macroText, QString &errorMessage)
{
    Reset();

    QStringList lines = macroText.split('\n');

    // 解析每一行
    for (int i = 0; i < lines.size(); ++i) {
        QString line       = lines[i].trimmed();
        int     lineNumber = i + 1;

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

    // 初始化 SimContext（不需要额外的初始化，TickCache 已经准备好）
    // 技能和BUFF的状态存储在 simContext.cache 中

    // 设置 currentLine 为第一条有效指令的行号
    if (!d->scriptLines.empty()) {
        d->currentLine = d->scriptLines[0].lineNumber;
        qDebug() << "初始行号设置为:" << d->currentLine;
    }

    return true;
}

ScriptInterpreter::ScriptLine ScriptInterpreter::ParseLine(int lineNumber, const QString &line)
{
    ScriptLine scriptLine;
    scriptLine.lineNumber = lineNumber;
    scriptLine.rawLine    = line;

    // 使用正则表达式解析指令
    // 格式: /cast [条件] 技能名
    // 格式: /scast [条件] 技能名
    // 格式: /switch [条件] 宏名

    QRegularExpression      re(R"(^(/\w+)\s+(?:\[(.*?)\]\s+)?(.+)$)");
    QRegularExpressionMatch match = re.match(line);

    if (match.hasMatch()) {
        scriptLine.command   = match.captured(1); // /cast, /scast, etc.
        QString conditions   = match.captured(2); // 条件部分
        scriptLine.skillName = match.captured(3); // 技能名

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
            foundNext      = true;
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

    QRegularExpression      re(R"(^(\w+)([><]=?)(.+)$)");
    QRegularExpressionMatch match = re.match(condition);

    if (!match.hasMatch()) {
        qDebug() << "无法解析条件:" << condition;
        return true; // 无法解析的条件默认为真
    }

    QString var   = match.captured(1);
    QString op    = match.captured(2);
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
        // 从 SimContext 查询技能冷却
        QString     skillName    = var.mid(9); // 去掉 "skill_cd:" 前缀
        std::string skillNameStd = skillName.toStdString();

        // 查找技能ID
        auto it = d->skillNameToId.find(skillNameStd);
        if (it != d->skillNameToId.end()) {
            JX3DPS::jx3id_t skillId = it->second;
            varValue = d->simContext.cache.skill_cooldown[skillId] / 16.0; // 转换为秒 (16 ticks/秒)
        } else {
            varValue = 0; // 未找到技能，假设已冷却
        }
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
    // 使用 SimContext 检查技能是否就绪
    std::string skillNameStd = skillName.toStdString();

    // 查找技能ID
    auto it = d->skillNameToId.find(skillNameStd);
    if (it == d->skillNameToId.end()) {
        qDebug() << "  [错误] 未找到技能:" << skillName;
        return false;
    }

    JX3DPS::jx3id_t skillId = it->second;

    // 检查技能冷却和GCD
    if (d->simContext.cache.skill_cooldown[skillId] > 0) {
        qDebug() << "  [失败] 技能冷却中:" << skillName;
        return false;
    }

    if (d->simContext.globalCooldownCurrent > 0) {
        qDebug() << "  [失败] 全局冷却中";
        return false;
    }

    qDebug() << "  [成功] 施放技能:" << skillName;

    // 设置技能冷却 (假设所有技能冷却为 160 ticks = 10秒)
    d->simContext.cache.skill_cooldown[skillId] = 160;

    // 设置全局冷却 (24 ticks = 1.5秒)
    d->simContext.globalCooldownCurrent = 24;

    // 推进时间（1 tick）
    d->simContext.Update(1);

    // 模拟消耗气点
    if (d->player.qidian > 0) {
        d->player.qidian--;
    }

    d->lastSkill = skillName;

    return true;
}

ScriptInterpreter::ExecutionState ScriptInterpreter::GetState() const
{
    ExecutionState state;
    state.currentLine           = d->currentLine;
    state.stepType              = d->stepType;
    state.currentConditionIndex = d->currentConditionIndex;
    state.currentCondition      = d->currentCondition;
    state.conditionResult       = d->conditionResult;
    state.lastSkill             = d->lastSkill;
    state.finished              = d->finished;
    state.errorMessage          = d->errorMessage;
    return state;
}

void ScriptInterpreter::Reset()
{
    d->scriptLines.clear();
    d->currentLine = 1;
    d->lastSkill.clear();
    d->finished = false;
    d->errorMessage.clear();

    // 重置调试状态
    d->stepType              = DebugStepType::LINE;
    d->currentConditionIndex = -1;
    d->currentCondition.clear();
    d->conditionResult    = false;
    d->currentScriptIndex = -1;

    d->player.qidian      = 10;
    d->player.lifePercent = 1.0;
    d->player.manaPercent = 1.0;

    // 重置 SimContext
    d->simContext = JX3DPS::SimContext(); // 创建新的 SimContext 以重置所有状态
}

const std::vector<ScriptInterpreter::ScriptLine> &ScriptInterpreter::GetScriptLines() const
{
    return d->scriptLines;
}

bool ScriptInterpreter::StepInto()
{
    if (d->finished) {
        return false;
    }

    // 状态机：LINE -> CONDITION -> CONDITION -> ... -> SKILL_CAST -> LINE

    if (d->stepType == DebugStepType::LINE) {
        // 找到当前行对应的脚本
        ScriptLine *currentScript = nullptr;
        for (size_t i = 0; i < d->scriptLines.size(); ++i) {
            if (d->scriptLines[i].lineNumber == d->currentLine) {
                currentScript         = &d->scriptLines[i];
                d->currentScriptIndex = i;
                break;
            }
        }

        if (!currentScript) {
            // 当前行没有指令，移动到下一行
            d->currentLine++;
            if (d->currentLine > 100) {
                d->finished = true;
                return false;
            }
            return StepInto();
        }

        qDebug() << "[StepInto] 进入行" << currentScript->lineNumber << ":" << currentScript->rawLine;

        // 如果有条件，进入第一个条件
        if (!currentScript->conditions.isEmpty()) {
            d->stepType              = DebugStepType::CONDITION;
            d->currentConditionIndex = 0;
            d->currentCondition      = currentScript->conditions[0];
            d->conditionResult       = EvaluateCondition(d->currentCondition);
            qDebug() << "  [条件" << d->currentConditionIndex << "] " << d->currentCondition << " = " << d->conditionResult;
            return true;
        } else {
            // 没有条件，直接执行技能
            d->stepType = DebugStepType::SKILL_CAST;
            return StepInto(); // 递归进入技能施放
        }
    } else if (d->stepType == DebugStepType::CONDITION) {
        // 当前在条件判定中
        ScriptLine *currentScript = &d->scriptLines[d->currentScriptIndex];

        // 如果当前条件为 false，整个指令失败，跳到下一行
        if (!d->conditionResult) {
            qDebug() << "  [条件失败] 跳过此行";
            d->stepType              = DebugStepType::LINE;
            d->currentConditionIndex = -1;
            d->currentCondition.clear();
            d->currentLine++;
            // 找到下一条有效指令
            bool foundNext = false;
            for (const auto &script : d->scriptLines) {
                if (script.lineNumber >= d->currentLine) {
                    d->currentLine = script.lineNumber;
                    foundNext      = true;
                    break;
                }
            }
            if (!foundNext) {
                d->finished = true;
            }
            return true;
        }

        // 移动到下一个条件
        d->currentConditionIndex++;
        if (d->currentConditionIndex < currentScript->conditions.size()) {
            d->currentCondition = currentScript->conditions[d->currentConditionIndex];
            d->conditionResult  = EvaluateCondition(d->currentCondition);
            qDebug() << "  [条件" << d->currentConditionIndex << "] " << d->currentCondition << " = " << d->conditionResult;
            return true;
        } else {
            // 所有条件都通过，进入技能施放
            d->stepType              = DebugStepType::SKILL_CAST;
            d->currentConditionIndex = -1;
            return StepInto();
        }
    } else if (d->stepType == DebugStepType::SKILL_CAST) {
        // 执行技能施放
        ScriptLine *currentScript = &d->scriptLines[d->currentScriptIndex];

        if (currentScript->command == "/cast" || currentScript->command == "/scast") {
            if (CastSkill(currentScript->skillName)) {
                qDebug() << "  [技能施放] 成功:" << currentScript->skillName;
                d->lastSkill = currentScript->skillName;
            } else {
                qDebug() << "  [技能施放] 失败:" << currentScript->skillName;
            }
        } else if (currentScript->command == "/switch") {
            qDebug() << "  [切换宏] " << currentScript->skillName;
        }

        // 移动到下一行
        d->stepType = DebugStepType::LINE;
        d->currentLine++;

        // 找到下一条有效指令
        bool foundNext = false;
        for (const auto &script : d->scriptLines) {
            if (script.lineNumber >= d->currentLine) {
                d->currentLine = script.lineNumber;
                foundNext      = true;
                break;
            }
        }

        if (!foundNext) {
            d->finished = true;
            qDebug() << "[StepInto] 脚本执行完成";
        }

        return true;
    }

    return false;
}

bool ScriptInterpreter::StepOver()
{
    if (d->finished) {
        return false;
    }

    // StepOver: 执行完整的一行（包括所有条件和技能施放）
    int startLine = d->currentLine;

    qDebug() << "[StepOver] 开始执行，起始行:" << startLine << "，当前步骤类型:" << (int)d->stepType
             << "，条件索引:" << d->currentConditionIndex;

    // 如果当前在条件或技能施放中，先完成当前步骤
    if (d->stepType != DebugStepType::LINE) {
        qDebug() << "[StepOver] 当前在条件/技能中，先完成当前步骤";
        // 持续执行直到回到 LINE 状态或行号改变
        while (!d->finished && d->currentLine == startLine && d->stepType != DebugStepType::LINE) {
            if (!StepInto()) {
                break;
            }
        }
    }

    // 如果还在同一行且是 LINE 状态，执行完整的一行
    if (!d->finished && d->currentLine == startLine && d->stepType == DebugStepType::LINE) {
        qDebug() << "[StepOver] 开始执行完整的一行";
        // 持续执行直到行号改变
        while (!d->finished && d->currentLine == startLine) {
            if (!StepInto()) {
                break;
            }
        }
    }

    qDebug() << "[StepOver] 完成，结束行:" << d->currentLine << "，步骤类型:" << (int)d->stepType;

    return true;
}

bool ScriptInterpreter::StepOut()
{
    if (d->finished) {
        return false;
    }

    // StepOut: 对于宏脚本，跳到脚本末尾
    // TODO: 如果未来有循环或函数调用，这里需要跟踪调用栈
    d->finished = true;
    qDebug() << "[StepOut] 跳出脚本";
    return true;
}
