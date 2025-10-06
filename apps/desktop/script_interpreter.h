#ifndef SCRIPT_INTERPRETER_H
#define SCRIPT_INTERPRETER_H

#include <QString>
#include <QStringList>
#include <QObject>
#include <vector>
#include <memory>

/**
 * @brief 简单的宏脚本解释器
 *
 * 功能：
 * 1. 逐行解析宏脚本
 * 2. 执行指令（/cast, /scast, /switch等）
 * 3. 评估条件表达式
 * 4. 跟踪当前执行行号
 * 5. 提供单步调试接口
 */
class ScriptInterpreter : public QObject
{
    Q_OBJECT

public:
    explicit ScriptInterpreter(QObject *parent = nullptr);
    ~ScriptInterpreter();

    /**
     * @brief 脚本行数据结构
     */
    struct ScriptLine {
        int lineNumber;           // 行号（1-based）
        QString command;          // 指令类型 (/cast, /scast, /switch, etc.)
        QString skillName;        // 技能名称或技能ID
        QStringList conditions;   // 条件列表
        QString rawLine;          // 原始文本
    };

    /**
     * @brief 执行状态
     */
    struct ExecutionState {
        int currentLine;          // 当前行号
        QString lastSkill;        // 最后施放的技能
        bool finished;            // 是否执行完成
        QString errorMessage;     // 错误信息
    };

    /**
     * @brief 初始化解释器
     * @param macroText 宏脚本文本
     * @return 成功返回true
     */
    bool Initialize(const QString &macroText, QString &errorMessage);

    /**
     * @brief 单步执行（执行下一条指令）
     * @return 成功返回true
     */
    bool StepOne();

    /**
     * @brief 获取当前执行状态
     */
    ExecutionState GetState() const;

    /**
     * @brief 重置解释器
     */
    void Reset();

    /**
     * @brief 获取所有解析的脚本行
     */
    const std::vector<ScriptLine>& GetScriptLines() const;

private:
    /**
     * @brief 解析一行脚本
     */
    ScriptLine ParseLine(int lineNumber, const QString &line);

    /**
     * @brief 评估条件表达式
     * @param condition 条件字符串（如 "qidian>5", "skill_cd:技能名<2"）
     * @return 条件是否满足
     */
    bool EvaluateCondition(const QString &condition);

    /**
     * @brief 执行技能施放
     * @param skillName 技能名称
     * @return 成功返回true
     */
    bool CastSkill(const QString &skillName);

    struct Impl;
    std::unique_ptr<Impl> d;
};

#endif // SCRIPT_INTERPRETER_H
