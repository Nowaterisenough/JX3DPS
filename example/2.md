#include <cctype>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

// 前向声明
class Node;
class Expression;
class Condition;
class Action;

// Token 类型
enum class TokenType
{
    CAST,
    LBRACKET,
    RBRACKET,
    AND,
    OR,
    COLON,
    LT,
    LE,
    EQ,
    NE,
    GT,
    GE,
    IDENTIFIER,
    NUMBER,
    END
};

// Token 结构
struct Token
{
    TokenType   type;
    std::string value;

    Token() { }

    Token(TokenType t, std::string v) : type(t), value(std::move(v)) { }
};

// Lexer 类
class Lexer
{
private:
    std::string input;
    size_t      position = 0;

public:
    explicit Lexer(std::string input) : input(std::move(input)) { }

    Token nextToken()
    {
        skipWhitespace();

        if (position >= input.length()) {
            return { TokenType::END, "" };
        }

        char current = input[position];

        if (current == '/') {
            position++;
            if (position < input.length() && input[position] == 'c') {
                position += 4; // Skip "cast"
                return { TokenType::CAST, "/cast" };
            }
        }

        if (current == '[') {
            position++;
            return { TokenType::LBRACKET, std::string(1, current++) };
        }
        if (current == ']') {
            position++;
            return { TokenType::RBRACKET, std::string(1, current++) };
        }
        if (current == '&') {
            position++;
            return { TokenType::AND, std::string(1, current++) };
        }
        if (current == '|') {
            position++;
            return { TokenType::OR, std::string(1, current++) };
        }
        if (current == ':') {
            position++;
            return { TokenType::COLON, std::string(1, current++) };
        }

        if (current == '<') {
            if (position + 1 < input.length() && input[position + 1] == '=') {
                position += 2;
                return { TokenType::LE, "<=" };
            }
            position++;
            return { TokenType::LT, "<" };
        }

        if (current == '>') {
            if (position + 1 < input.length() && input[position + 1] == '=') {
                position += 2;
                return { TokenType::GE, ">=" };
            }
            position++;
            return { TokenType::GT, ">" };
        }

        if (current == '=') {
            if (position + 1 < input.length() && input[position + 1] == '=') {
                position += 2;
                return { TokenType::EQ, "==" };
            }
        }

        if (current == '!') {
            if (position + 1 < input.length() && input[position + 1] == '=') {
                position += 2;
                return { TokenType::NE, "!=" };
            }
        }

        if (std::isalpha(current) || current == '_') {
            return identifier();
        }

        if (std::isdigit(current)) {
            return number();
        }

        throw std::runtime_error("Unexpected character: " + std::string(1, current));
    }

private:
    void skipWhitespace()
    {
        while (position < input.length() && std::isspace(input[position])) {
            position++;
        }
    }

    Token identifier()
    {
        size_t start = position;
        while (position < input.length() && (std::isalnum(input[position]) || input[position] == '_'))
        {
            position++;
        }
        return { TokenType::IDENTIFIER, input.substr(start, position - start) };
    }

    Token number()
    {
        size_t start = position;
        while (position < input.length() && std::isdigit(input[position])) {
            position++;
        }
        return { TokenType::NUMBER, input.substr(start, position - start) };
    }
};

// AST 节点基类
class Node
{
public:
    virtual ~Node()                      = default;
    virtual std::string toString() const = 0;
};

// 表达式节点
class Expression : public Node
{
public:
    virtual bool evaluate() const = 0;
};

// 条件节点
class Condition : public Expression
{
public:
    std::string type;
    std::string name;
    std::string op;
    int         value;

    Condition(std::string type, std::string name, std::string op = "", int value = 0) :
        type(std::move(type)), name(std::move(name)), op(std::move(op)), value(value)
    {
    }

    bool evaluate() const override
    {
        // 这里应该实现实际的条件评估逻辑
        return true; // 简化示例
    }

    std::string toString() const override
    {
        std::ostringstream oss;
        oss << type << ":" << name;
        if (!op.empty()) {
            oss << op << value;
        }
        return oss.str();
    }
};

// 逻辑运算节点
class LogicalOp : public Expression
{
public:
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;
    std::string                 op;

    LogicalOp(std::unique_ptr<Expression> left, std::unique_ptr<Expression> right, std::string op) :
        left(std::move(left)), right(std::move(right)), op(std::move(op))
    {
    }

    bool evaluate() const override
    {
        if (op == "&") {
            return left->evaluate() && right->evaluate();
        } else if (op == "|") {
            return left->evaluate() || right->evaluate();
        }
        throw std::runtime_error("Unknown logical operator: " + op);
    }

    std::string toString() const override
    {
        return "(" + left->toString() + " " + op + " " + right->toString() + ")";
    }
};

// 动作节点
class Action : public Node
{
public:
    std::string name;

    explicit Action(std::string name) : name(std::move(name)) { }

    std::string toString() const override { return "Action: " + name; }
};

// 宏命令节点
class Macro : public Node
{
public:
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Action>     action;

    Macro(std::unique_ptr<Expression> condition, std::unique_ptr<Action> action) :
        condition(std::move(condition)), action(std::move(action))
    {
    }

    std::string toString() const override
    {
        return "Macro[" + condition->toString() + "] " + action->toString();
    }
};

// Parser 类
class Parser
{
private:
    Lexer lexer;
    Token currentToken;

public:
    explicit Parser(const std::string &input) : lexer(input) { currentToken = lexer.nextToken(); }

    std::unique_ptr<Macro> parseMacro()
    {
        consumeToken(TokenType::CAST);
        consumeToken(TokenType::LBRACKET);
        auto condition = parseExpression();
        consumeToken(TokenType::RBRACKET);
        auto action = parseAction();
        return std::make_unique<Macro>(std::move(condition), std::move(action));
    }

private:
    void consumeToken(TokenType expectedType)
    {
        std::cout << "-----" << currentToken.value << std::endl;
        if (currentToken.type == expectedType) {
            currentToken = lexer.nextToken();
        } else {
            throw std::runtime_error("Unexpected token: " + currentToken.value);
        }
    }

    std::unique_ptr<Expression> parseExpression()
    {
        auto left = parseCondition();
        while (currentToken.type == TokenType::AND || currentToken.type == TokenType::OR) {
            std::string op = currentToken.value;
            consumeToken(currentToken.type);
            auto right = parseCondition();
            left       = std::make_unique<LogicalOp>(std::move(left), std::move(right), op);
        }
        return left;
    }

    std::unique_ptr<Expression> parseCondition()
    {
        std::string type = currentToken.value;
        consumeToken(TokenType::IDENTIFIER);
        consumeToken(TokenType::COLON);
        std::string name = currentToken.value;
        consumeToken(TokenType::IDENTIFIER);

        if (currentToken.type == TokenType::LT || currentToken.type == TokenType::LE ||
            currentToken.type == TokenType::EQ || currentToken.type == TokenType::NE ||
            currentToken.type == TokenType::GT || currentToken.type == TokenType::GE)
        {
            std::string op = currentToken.value;
            consumeToken(currentToken.type);
            int value = std::stoi(currentToken.value);
            consumeToken(TokenType::NUMBER);
            return std::make_unique<Condition>(type, name, op, value);
        }

        return std::make_unique<Condition>(type, name);
    }

    std::unique_ptr<Action> parseAction()
    {
        std::string name = currentToken.value;
        consumeToken(TokenType::IDENTIFIER);
        return std::make_unique<Action>(name);
    }
};

int main()
{
    std::vector<std::string> macros = { "/cast [buff:A&buff:F|cd:D<3] B", "/cast [nobuff:C|buff:A] D", "/cast [cd:B>2&nobuff:A] E" };

    for (const auto &macroStr : macros) {
        try {
            Parser parser(macroStr);
            auto   macro = parser.parseMacro();
            std::cout << "Parsed Macro: " << macro->toString() << std::endl;
        } catch (const std::exception &e) {
            std::cerr << "Error parsing macro: " << e.what() << std::endl;
        }
    }

    return 0;
}