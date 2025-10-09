#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

// Condition function type
using ConditionFunction = std::function<bool()>;

// Enumeration for node types in the AST
enum class NodeType
{
    OPERAND,
    AND_EXPRESSION,
    OR_EXPRESSION
};

// Base class for expression nodes in the AST
struct Node
{
    NodeType type;

    explicit Node(NodeType type) : type(type) { }

    virtual bool Evaluate() = 0;
    virtual ~Node()         = default;
};

// Operand node representing a condition function
struct OperandNode : Node
{
    ConditionFunction func;

    explicit OperandNode(ConditionFunction func) :
        Node(NodeType::OPERAND), func(std::move(func))
    {
    }

    bool Evaluate() override { return func(); }
};

// AndExpression node for logical AND expressions
struct AndExpressionNode : Node
{
    std::vector<std::unique_ptr<Node>> children;

    AndExpressionNode() : Node(NodeType::AND_EXPRESSION) { }

    bool Evaluate() override
    {
        for (auto &child : children) {
            if (!child->Evaluate()) {
                return false; // short-circuit logic
            }
        }
        return true;
    }
};

// OrExpression node for logical OR expressions
struct OrExpressionNode : Node
{
    std::vector<std::unique_ptr<Node>> children;

    OrExpressionNode() : Node(NodeType::OR_EXPRESSION) { }

    bool Evaluate() override
    {
        for (auto &child : children) {
            if (child->Evaluate()) {
                return true; // short-circuit logic
            }
        }
        return false;
    }
};

// Interpreter class for parsing expressions and building the AST
class Interpreter
{
private:
    std::unordered_map<char, ConditionFunction> &_functions_map;

    // Helper function to create expression nodes from operators
    static std::unique_ptr<Node> CreateExpressionNode(char operator_symbol,
                                                      std::unique_ptr<Node> left,
                                                      std::unique_ptr<Node> right)
    {
        if (operator_symbol == '&') {
            auto and_node = std::make_unique<AndExpressionNode>();
            and_node->children.push_back(std::move(left));
            and_node->children.push_back(std::move(right));
            return and_node;
        } else if (operator_symbol == '|') {
            auto or_node = std::make_unique<OrExpressionNode>();
            or_node->children.push_back(std::move(left));
            or_node->children.push_back(std::move(right));
            return or_node;
        }
        throw std::runtime_error("Unknown operator");
    }

    // Parses an operand from the expression
    std::unique_ptr<Node> ParseOperand(const std::string &expression, size_t &position)
    {
        if (position < expression.size() &&
            _functions_map.find(expression[position]) != _functions_map.end())
        {
            auto operand_node = std::make_unique<OperandNode>(_functions_map[expression[position]]);
            position++; // Advance to the next character
            return operand_node;
        }
        throw std::runtime_error("Invalid operand");
    }

    // Parses an AND expression considering short-circuit logic
    std::unique_ptr<Node> ParseAndExpression(const std::string &expression, size_t &position)
    {
        auto left_node = ParseOperand(expression, position);
        while (position < expression.size() && expression[position] == '&') {
            position++; // Skip '&'
            auto right_node = ParseOperand(expression, position);
            left_node = CreateExpressionNode('&', std::move(left_node), std::move(right_node));
        }
        return left_node;
    }

    // Parses an OR expression using recursion to handle nesting and precedence of AND
    std::unique_ptr<Node> ParseOrExpression(const std::string &expression, size_t &position)
    {
        auto node = ParseAndExpression(expression, position);
        while (position < expression.size() && expression[position] == '|') {
            position++; // Skip '|'
            auto right_node = ParseAndExpression(expression, position);
            node = CreateExpressionNode('|', std::move(node), std::move(right_node));
        }
        return node;
    }

public:
    explicit Interpreter(std::unordered_map<char, ConditionFunction> &functions_map) :
        _functions_map(functions_map)
    {
    }

    // Parses the expression and builds the AST
    std::unique_ptr<Node> ParseExpression(const std::string &expression, bool is_first_model)
    {
        size_t position = 0;
        if (is_first_model) {
            // First interpretation mode: '&' has precedence over '|'
            return ParseOrExpression(expression, position);
        } else {
            // Second interpretation mode: Evaluate from left to right
            std::unique_ptr<Node> node = ParseOperand(expression, position);
            while (position < expression.size()) {
                char op = expression[position];
                if (op == '|' || op == '&') {
                    position++; // Skip the operator
                    auto right_node = ParseOperand(expression, position);
                    node = CreateExpressionNode(op, std::move(node), std::move(right_node));
                } else {
                    throw std::runtime_error("Unexpected character in expression");
                }
            }
            return node;
        }
    }
};
