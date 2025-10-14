#ifndef JX3DPS_MACRO_INTERPRETER_H
#define JX3DPS_MACRO_INTERPRETER_H

#include "ast.h"
#include "lexer.h"
#include "src/global/types.h"
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

namespace JX3DPS {

/**
 * @brief 解析错误码
 */
enum class ParserError {
    SUCCESS,
    UNEXPECTED_TOKEN,
    INVALID_CONDITION_TYPE,
    INVALID_COMPARISON_OPERATOR,
    MISSING_BRACKET,
    MISSING_ACTION
};

/**
 * @brief 宏解释器
 *
 * 用法:
 * ```cpp
 * MacroInterpreter interpreter;
 *
 * // 设置ID映射
 * interpreter.SetBuffIdGetter([](const std::string& name) { return GetBuffId(name); });
 * interpreter.SetSkillIdGetter([](const std::string& name) { return GetSkillId(name); });
 *
 * // 解析宏
 * auto [node, error] = interpreter.ParseMacro("/cast [buff:破|cd:无我无剑<2] 无我无剑");
 *
 * // 评估宏
 * if (error == ParserError::SUCCESS && node->Evaluate()) {
 *     std::string skill = node->GetAction();
 *     CastSkill(skill);
 * }
 * ```
 */
class MacroInterpreter {
public:
    MacroInterpreter() = default;

    /**
     * @brief 设置BUFF ID获取函数
     */
    void SetBuffIdGetter(func_t<jx3id_t(const std::string&)> getter) {
        m_get_buff_id = std::move(getter);
    }

    /**
     * @brief 设置技能ID获取函数
     */
    void SetSkillIdGetter(func_t<jx3id_t(const std::string&)> getter) {
        m_get_skill_id = std::move(getter);
    }

    /**
     * @brief 解析宏语句
     *
     * @param macro 宏字符串
     * @return 返回 AST根节点和错误码
     *
     * 支持的格式:
     * - /cast 技能名
     * - /cast [条件] 技能名
     * - /fcast [条件] 技能名
     *
     * 条件格式:
     * - buff:BUFF名
     * - nobuff:BUFF名
     * - buff:BUFF名>N
     * - bufftime:BUFF名>N
     * - tbuff:BUFF名
     * - tnobuff:BUFF名
     * - tbufftime:BUFF名>N
     * - cd:技能名<N
     * - 条件1&条件2
     * - 条件1|条件2
     */
    std::pair<std::unique_ptr<AstNode>, ParserError> ParseMacro(const std::string& macro) {
        Lexer lexer(macro);
        return ParseMacroImpl(lexer);
    }

private:
    func_t<jx3id_t(const std::string&)> m_get_buff_id;
    func_t<jx3id_t(const std::string&)> m_get_skill_id;

    std::pair<std::unique_ptr<AstNode>, ParserError> ParseMacroImpl(Lexer& lexer) {
        Token token = lexer.NextToken();

        // 解析命令类型
        if (token.type != TokenType::CAST && token.type != TokenType::FCAST) {
            return {nullptr, ParserError::UNEXPECTED_TOKEN};
        }

        token = lexer.NextToken();

        std::unique_ptr<AstNode> condition = nullptr;

        // 检查是否有条件
        if (token.type == TokenType::LBRACKET) {
            token = lexer.NextToken(); // Skip '['

            auto [cond, error] = ParseExpression(lexer, token);
            if (error != ParserError::SUCCESS) {
                return {nullptr, error};
            }
            condition = std::move(cond);

            // 期望 ']'
            if (token.type != TokenType::RBRACKET) {
                return {nullptr, ParserError::MISSING_BRACKET};
            }

            token = lexer.NextToken(); // Skip ']'
        }

        // 解析动作 (技能名)
        if (token.type != TokenType::IDENTIFIER) {
            return {nullptr, ParserError::MISSING_ACTION};
        }

        auto action = std::make_unique<ActionNode>(token.value);

        // 如果没有条件，直接返回动作
        if (!condition) {
            return {std::move(action), ParserError::SUCCESS};
        }

        // 组合条件和动作
        auto root = std::make_unique<AndNode>(std::move(condition), std::move(action));
        return {std::move(root), ParserError::SUCCESS};
    }

    /**
     * @brief 解析表达式 (处理 & 和 | 运算符)
     */
    std::pair<std::unique_ptr<AstNode>, ParserError> ParseExpression(Lexer& lexer, Token& token) {
        auto [left, error] = ParseCondition(lexer, token);
        if (error != ParserError::SUCCESS) {
            return {nullptr, error};
        }

        while (token.type == TokenType::AND || token.type == TokenType::OR) {
            TokenType op = token.type;
            token = lexer.NextToken();

            auto [right, right_error] = ParseCondition(lexer, token);
            if (right_error != ParserError::SUCCESS) {
                return {nullptr, right_error};
            }

            if (op == TokenType::AND) {
                left = std::make_unique<AndNode>(std::move(left), std::move(right));
            } else {
                left = std::make_unique<OrNode>(std::move(left), std::move(right));
            }
        }

        return {std::move(left), ParserError::SUCCESS};
    }

    /**
     * @brief 解析单个条件
     */
    std::pair<std::unique_ptr<AstNode>, ParserError> ParseCondition(Lexer& lexer, Token& token) {
        if (token.type != TokenType::IDENTIFIER) {
            return {nullptr, ParserError::UNEXPECTED_TOKEN};
        }

        std::string type = token.value;
        token = lexer.NextToken();

        // 期望 ':'
        if (token.type != TokenType::COLON) {
            return {nullptr, ParserError::UNEXPECTED_TOKEN};
        }

        token = lexer.NextToken();

        // 解析名称
        if (token.type != TokenType::IDENTIFIER) {
            return {nullptr, ParserError::UNEXPECTED_TOKEN};
        }

        std::string name = token.value;
        token = lexer.NextToken();

        // 检查是否有比较运算符
        if (token.type >= TokenType::LT && token.type <= TokenType::GE) {
            std::string op = TokenTypeToString(token.type);
            token = lexer.NextToken();

            if (token.type != TokenType::NUMBER) {
                return {nullptr, ParserError::UNEXPECTED_TOKEN};
            }

            int value = std::stoi(token.value);
            token = lexer.NextToken();

            return ParseComparisonCondition(type, name, op, value);
        }

        // 存在性条件
        return ParseExistsCondition(type, name);
    }

    /**
     * @brief 解析比较条件
     */
    std::pair<std::unique_ptr<AstNode>, ParserError> ParseComparisonCondition(
        const std::string& type,
        const std::string& name,
        const std::string& op,
        int value)
    {
        if (type == "bufftime") {
            jx3id_t id = m_get_buff_id(name);

            // 注册宏时间阈值到 context
            RegisterMacroTimeThreshold(MacroTrigger::Type::BUFF_DURATION, id, value);

            return MakeComparisonNode<BuffDurationCondition>(id, op, value);
        }

        if (type == "tbufftime") {
            jx3id_t id = m_get_buff_id(name);

            // 注册宏时间阈值到 context
            RegisterMacroTimeThreshold(MacroTrigger::Type::TBUFF_DURATION, id, value);

            return MakeComparisonNode<TBuffDurationCondition>(id, op, value);
        }

        if (type == "cd") {
            jx3id_t id = m_get_skill_id(name);

            // 注册宏时间阈值到 context
            RegisterMacroTimeThreshold(MacroTrigger::Type::SKILL_COOLDOWN, id, value);

            return MakeComparisonNode<SkillCooldownCondition>(id, op, value);
        }

        return {nullptr, ParserError::INVALID_CONDITION_TYPE};
    }

    /**
     * @brief 注册宏时间阈值
     *
     * 将宏条件中的时间阈值注册到 Context，用于生成精确的关键帧
     *
     * @param type 阈值类型
     * @param id BUFF 或技能的 ID
     * @param threshold 阈值（帧数）
     */
    void RegisterMacroTimeThreshold(MacroTrigger::Type type, jx3id_t id, tick_t threshold) {
        // 检查是否已经注册过相同的阈值，避免重复
        for (const auto& existing : context.macro_triggers) {
            if (existing.type == type &&
                existing.cache_index == static_cast<size_t>(id) &&
                existing.threshold == threshold) {
                return; // 已存在，不重复添加
            }
        }

        // 添加新的时间阈值
        context.macro_triggers.emplace_back(
            type,
            static_cast<size_t>(id),
            threshold
        );
    }

    /**
     * @brief 解析存在性条件
     */
    std::pair<std::unique_ptr<AstNode>, ParserError> ParseExistsCondition(
        const std::string& type,
        const std::string& name)
    {
        jx3id_t id = m_get_buff_id(name);

        if (type == "buff") {
            return MakeConditionNode<BuffExistsCondition<Comparator::Exists>>(id);
        }

        if (type == "nobuff") {
            return MakeConditionNode<BuffExistsCondition<Comparator::NotExists>>(id);
        }

        if (type == "tbuff") {
            return MakeConditionNode<TBuffExistsCondition<Comparator::Exists>>(id);
        }

        if (type == "tnobuff") {
            return MakeConditionNode<TBuffExistsCondition<Comparator::NotExists>>(id);
        }

        return {nullptr, ParserError::INVALID_CONDITION_TYPE};
    }

    /**
     * @brief 创建条件节点
     */
    template <typename CondType, typename... Args>
    std::pair<std::unique_ptr<AstNode>, ParserError> MakeConditionNode(Args&&... args) {
        auto evaluator = std::make_unique<CondType>(std::forward<Args>(args)...);
        auto node = std::make_unique<ConditionNode>(std::move(evaluator));
        return {std::move(node), ParserError::SUCCESS};
    }

    /**
     * @brief 创建比较条件节点
     */
    template <template <typename> class CondType>
    std::pair<std::unique_ptr<AstNode>, ParserError> MakeComparisonNode(
        jx3id_t id,
        const std::string& op,
        int value)
    {
        if (op == "<") {
            return MakeConditionNode<CondType<Comparator::LessThan>>(id, value);
        }
        if (op == "<=") {
            return MakeConditionNode<CondType<Comparator::LessEqual>>(id, value);
        }
        if (op == "==") {
            return MakeConditionNode<CondType<Comparator::Equal>>(id, value);
        }
        if (op == "!=") {
            return MakeConditionNode<CondType<Comparator::NotEqual>>(id, value);
        }
        if (op == ">") {
            return MakeConditionNode<CondType<Comparator::GreaterThan>>(id, value);
        }
        if (op == ">=") {
            return MakeConditionNode<CondType<Comparator::GreaterEqual>>(id, value);
        }

        return {nullptr, ParserError::INVALID_COMPARISON_OPERATOR};
    }

    std::string TokenTypeToString(TokenType type) {
        switch (type) {
            case TokenType::LT: return "<";
            case TokenType::LE: return "<=";
            case TokenType::EQ: return "==";
            case TokenType::NE: return "!=";
            case TokenType::GT: return ">";
            case TokenType::GE: return ">=";
            default: return "";
        }
    }
};

} // namespace JX3DPS

#endif // JX3DPS_MACRO_INTERPRETER_H
