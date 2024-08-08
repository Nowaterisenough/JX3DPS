#include <proxy.h>

#include "Ast.hpp"
#include "Lexer.hpp"

enum class ParserError
{
    SUCCESS,
    UNEXPECTED_TOKEN,
    INVALID_CONDITION_TYPE,
    INVALID_COMPARISON_OPERATOR
};

class MacroInterpreter
{
private:
    class Parser
    {
    private:
        Lexer _lexer;
        Token _current_token;

    public:
        explicit Parser(const std::string &input) : _lexer(input)
        {
            _current_token = _lexer.NextToken();
        }

        std::pair<pro::proxy<spec::AstNode>, ParserError> ParseMacro()
        {
            auto macro_type = _current_token.type;
            auto error = ConsumeToken(TokenType::CAST, TokenType::FCAST, TokenType::SWITCH, TokenType::USE);
            if (error != ParserError::SUCCESS) {
                return std::make_pair(nullptr, error);
            }

            pro::proxy<spec::AstNode> condition = nullptr;

            // 检查是否有条件部分
            if (_current_token.type == TokenType::LBRACKET) {
                error = ConsumeToken(TokenType::LBRACKET);
                if (error != ParserError::SUCCESS) {
                    return std::make_pair(nullptr, error);
                }

                auto [parsed_condition, cond_error] = ParseExpression();
                if (cond_error != ParserError::SUCCESS) {
                    return std::make_pair(nullptr, cond_error);
                }
                condition = std::move(parsed_condition);

                error = ConsumeToken(TokenType::RBRACKET);
                if (error != ParserError::SUCCESS) {
                    return std::make_pair(nullptr, error);
                }
            }

            auto [action, act_error] = ParseAction();
            if (act_error != ParserError::SUCCESS) {
                return std::make_pair(nullptr, act_error);
            }

            // 如果没有条件，直接返回动作节点
            if (condition == nullptr) {
                return std::make_pair(std::move(action), ParserError::SUCCESS);
            }

            // 根据宏类型创建不同的节点
            switch (macro_type) {
                case TokenType::CAST:
                case TokenType::FCAST:
                    return std::make_pair(
                        pro::make_proxy<spec::AstNode, AndNodeImpl>(std::move(condition), std::move(action)),
                        ParserError::SUCCESS);
                case TokenType::SWITCH:
                case TokenType::USE:
                    return std::make_pair(
                        pro::make_proxy<spec::AstNode, AndNodeImpl>(std::move(condition), std::move(action)),
                        ParserError::SUCCESS);
                default: return std::make_pair(nullptr, ParserError::UNEXPECTED_TOKEN);
            }
        }

    private:
        template <typename... TokenTypes>
        ParserError ConsumeToken(TokenTypes... expected_types)
        {
            auto match = (... || (_current_token.type == expected_types));
            if (match) {
                _current_token = _lexer.NextToken();
                return ParserError::SUCCESS;
            }
            return ParserError::UNEXPECTED_TOKEN;
        }

        std::pair<pro::proxy<spec::AstNode>, ParserError> ParseExpression()
        {
            auto [left, error] = ParseCondition();
            if (error != ParserError::SUCCESS) {
                return std::make_pair(nullptr, error);
            }

            while (_current_token.type == TokenType::AND || _current_token.type == TokenType::OR) {
                TokenType op  = _current_token.type;
                auto      err = ConsumeToken(op);
                if (err != ParserError::SUCCESS) {
                    return std::make_pair(nullptr, err);
                }

                auto [right, right_error] = ParseCondition();
                if (right_error != ParserError::SUCCESS) {
                    return std::make_pair(nullptr, right_error);
                }

                if (op == TokenType::AND) {
                    left = pro::make_proxy<spec::AstNode, AndNodeImpl>(std::move(left), std::move(right));
                } else {
                    left = pro::make_proxy<spec::AstNode, OrNodeImpl>(std::move(left), std::move(right));
                }
            }
            return std::make_pair(std::move(left), ParserError::SUCCESS);
        }

        std::pair<pro::proxy<spec::AstNode>, ParserError> ParseCondition()
        {
            if (_current_token.type != TokenType::IDENTIFIER) {
                return std::make_pair(nullptr, ParserError::UNEXPECTED_TOKEN);
            }
            std::string type  = _current_token.value;
            auto        error = ConsumeToken(TokenType::IDENTIFIER);
            if (error != ParserError::SUCCESS) {
                return std::make_pair(nullptr, error);
            }

            error = ConsumeToken(TokenType::COLON);
            if (error != ParserError::SUCCESS) {
                return std::make_pair(nullptr, error);
            }

            if (_current_token.type != TokenType::IDENTIFIER) {
                return std::make_pair(nullptr, ParserError::UNEXPECTED_TOKEN);
            }
            std::string name = _current_token.value;
            error            = ConsumeToken(TokenType::IDENTIFIER);
            if (error != ParserError::SUCCESS) {
                return std::make_pair(nullptr, error);
            }

            if (_current_token.type == TokenType::LT || _current_token.type == TokenType::LE ||
                _current_token.type == TokenType::EQ || _current_token.type == TokenType::NE ||
                _current_token.type == TokenType::GT || _current_token.type == TokenType::GE)
            {
                std::string op = _current_token.value;
                error          = ConsumeToken(_current_token.type);
                if (error != ParserError::SUCCESS) {
                    return std::make_pair(nullptr, error);
                }
                if (_current_token.type != TokenType::NUMBER) {
                    return std::make_pair(nullptr, ParserError::UNEXPECTED_TOKEN);
                }
                int value = std::stoi(_current_token.value);
                error     = ConsumeToken(TokenType::NUMBER);
                if (error != ParserError::SUCCESS) {
                    return std::make_pair(nullptr, error);
                } 

                return ParseComparisonCondition(type, name, op, value);
            }

            return ParseExistsCondition(type, name);
        }

        std::pair<pro::proxy<spec::AstNode>, ParserError> ParseAction()
        {
            if (_current_token.type != TokenType::IDENTIFIER) {
                return std::make_pair(nullptr, ParserError::UNEXPECTED_TOKEN);
            }
            std::string name  = _current_token.value;
            auto        error = ConsumeToken(TokenType::IDENTIFIER);
            if (error != ParserError::SUCCESS) {
                return std::make_pair(nullptr, error);
            }

            return std::make_pair(pro::make_proxy<spec::AstNode, ActionNodeImpl>(std::move(name)),
                                  ParserError::SUCCESS);
        }

        std::pair<pro::proxy<spec::AstNode>, ParserError> ParseComparisonCondition(
            const std::string &type,
            const std::string &name,
            const std::string &op,
            int                value)
        {
            id_t id = (type == "buff" || type == "tbuff") ? GetBuffId(name) : GetSkillId(name);
            if (type == "buff") {
                return ParseComparisonCondition<BuffStacksProxy>(id, op, value);
            }
            if (type == "tbuff") {
                return ParseComparisonCondition<TBuffStacksProxy>(id, op, value);
            }
            if (type == "cd") {
                return ParseComparisonCondition<SkillCooldownProxy>(id, op, value);
            }
            if (type == "skill_energy") {
                return ParseComparisonCondition<SkillEnergyProxy>(id, op, value);
            }
            return std::make_pair(nullptr, ParserError::INVALID_CONDITION_TYPE);
        }

        std::pair<pro::proxy<spec::AstNode>, ParserError> ParseExistsCondition(const std::string &type,
                                                                               const std::string &name)
        {
            std::cout << "name: " << name << std::endl;
            id_t id = GetBuffId(name);
            if (type == "buff") {
                return std::make_pair(MakeConditionNode<BuffExistsProxy<Exists>>(id), ParserError::SUCCESS);
            }
            if (type == "nobuff") {
                return std::make_pair(MakeConditionNode<BuffExistsProxy<NotExists>>(id), ParserError::SUCCESS);
            }
            if (type == "tbuff") {
                return std::make_pair(MakeConditionNode<TBuffExistsProxy<Exists>>(id), ParserError::SUCCESS);
            }
            if (type == "tnobuff") {
                return std::make_pair(MakeConditionNode<TBuffExistsProxy<NotExists>>(id), ParserError::SUCCESS);
            }
            return std::make_pair(nullptr, ParserError::INVALID_CONDITION_TYPE);
        }

        template <typename ProxyType, typename... Args>
        pro::proxy<spec::AstNode> MakeConditionNode(Args &&...args)
        {
            return pro::make_proxy<spec::AstNode, ConditionNodeImpl>(
                pro::make_proxy<spec::Evaluator>(ProxyType(std::forward<Args>(args)...)));
        }

        template <template <typename> class ProxyType>
        std::pair<pro::proxy<spec::AstNode>, ParserError> ParseComparisonCondition(id_t id, const std::string &op, int value)
        {
            if (op == "<") {
                return std::make_pair(MakeConditionNode<ProxyType<LessThan>>(id, value), ParserError::SUCCESS);
            }
            if (op == "<=") {
                return std::make_pair(MakeConditionNode<ProxyType<LessEqual>>(id, value), ParserError::SUCCESS);
            }
            if (op == "==") {
                return std::make_pair(MakeConditionNode<ProxyType<Equal>>(id, value), ParserError::SUCCESS);
            }
            if (op == "!=") {
                return std::make_pair(MakeConditionNode<ProxyType<NotEqual>>(id, value), ParserError::SUCCESS);
            }
            if (op == ">") {
                return std::make_pair(MakeConditionNode<ProxyType<GreaterThan>>(id, value), ParserError::SUCCESS);
            }
            if (op == ">=") {
                return std::make_pair(MakeConditionNode<ProxyType<GreaterEqual>>(id, value), ParserError::SUCCESS);
            }
            return std::make_pair(nullptr, ParserError::INVALID_COMPARISON_OPERATOR);
        }
    };

public:
    std::pair<pro::proxy<spec::AstNode>, ErrorCode> ParseMacro(const std::string &macro)
    {
        Parser parser(macro);
        auto [node, error] = parser.ParseMacro();
        if (error != ParserError::SUCCESS) {
            return std::make_pair(nullptr, ErrorCode::INVALID_MACRO_FORMAT);
        }
        return std::make_pair(std::move(node), ErrorCode::SUCCESS);
    }
};