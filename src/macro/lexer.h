#ifndef JX3DPS_MACRO_LEXER_H
#define JX3DPS_MACRO_LEXER_H

#include <string>
#include <cctype>

namespace JX3DPS {

/**
 * @brief Token 类型
 */
enum class TokenType {
    CAST,       // /cast
    FCAST,      // /fcast (强制施放)
    LBRACKET,   // [
    RBRACKET,   // ]
    AND,        // &
    OR,         // |
    COLON,      // :
    LT,         // <
    LE,         // <=
    EQ,         // ==
    NE,         // !=
    GT,         // >
    GE,         // >=
    IDENTIFIER, // 标识符 (buff名、技能名等)
    NUMBER,     // 数字
    ERROR,      // 错误
    END         // 结束
};

/**
 * @brief Token 结构
 */
struct Token {
    TokenType   type;
    std::string value;

    Token() = default;
    Token(TokenType t, std::string v) : type(t), value(std::move(v)) {}
};

/**
 * @brief 词法分析器
 *
 * 支持的宏格式:
 * - /cast 技能名
 * - /cast [条件] 技能名
 * - /fcast [条件] 技能名
 *
 * 条件格式:
 * - buff:BUFF名           (BUFF存在)
 * - nobuff:BUFF名         (BUFF不存在)
 * - buff:BUFF名>N         (BUFF层数大于N)
 * - bufftime:BUFF名>N     (BUFF剩余时间大于N帧)
 * - tbuff:BUFF名          (目标BUFF存在)
 * - cd:技能名<N           (技能冷却小于N帧)
 * - skill_energy:技能名>N (技能充能数大于N)
 * - 条件1&条件2           (与)
 * - 条件1|条件2           (或)
 */
class Lexer {
public:
    explicit Lexer(std::string input) : m_input(std::move(input)), m_position(0) {}

    /**
     * @brief 获取下一个Token
     */
    Token NextToken() {
        SkipWhitespace();

        if (m_position >= m_input.length()) {
            return {TokenType::END, ""};
        }

        char current = m_input[m_position];

        // 解析命令
        if (current == '/') {
            return ParseCommand();
        }

        // 解析符号
        if (current == '[') {
            m_position++;
            return {TokenType::LBRACKET, "["};
        }
        if (current == ']') {
            m_position++;
            return {TokenType::RBRACKET, "]"};
        }
        if (current == '&') {
            m_position++;
            return {TokenType::AND, "&"};
        }
        if (current == '|') {
            m_position++;
            return {TokenType::OR, "|"};
        }
        if (current == ':') {
            m_position++;
            return {TokenType::COLON, ":"};
        }

        // 解析比较运算符
        if (current == '<' || current == '>' || current == '=' || current == '!') {
            return ParseOperator();
        }

        // 解析标识符 (支持中文)
        if (IsIdentifierStart(current)) {
            return ParseIdentifier();
        }

        // 解析数字
        if (std::isdigit(current)) {
            return ParseNumber();
        }

        // 未知字符
        m_position++;
        return {TokenType::ERROR, std::string(1, current)};
    }

private:
    std::string m_input;
    size_t      m_position;

    void SkipWhitespace() {
        while (m_position < m_input.length() && std::isspace(m_input[m_position])) {
            m_position++;
        }
    }

    bool IsIdentifierStart(char c) {
        // 支持字母、下划线、中文 (UTF-8高位为1)
        return std::isalpha(c) || c == '_' || (static_cast<unsigned char>(c) & 0x80);
    }

    bool IsIdentifierPart(char c) {
        // 标识符可以包含字母、数字、下划线、中文
        return std::isalnum(c) || c == '_' || (static_cast<unsigned char>(c) & 0x80);
    }

    Token ParseCommand() {
        m_position++; // Skip '/'

        if (m_position >= m_input.length()) {
            return {TokenType::ERROR, "/"};
        }

        // 解析命令名
        size_t start = m_position;
        while (m_position < m_input.length() && std::isalpha(m_input[m_position])) {
            m_position++;
        }

        std::string cmd = m_input.substr(start, m_position - start);

        if (cmd == "cast") {
            return {TokenType::CAST, "/cast"};
        }
        if (cmd == "fcast") {
            return {TokenType::FCAST, "/fcast"};
        }

        return {TokenType::ERROR, "/" + cmd};
    }

    Token ParseOperator() {
        std::string op;
        op += m_input[m_position];
        m_position++;

        // 检查是否为双字符运算符
        if (m_position < m_input.length() && m_input[m_position] == '=') {
            op += '=';
            m_position++;
        }

        if (op == "<")  return {TokenType::LT, op};
        if (op == "<=") return {TokenType::LE, op};
        if (op == "==") return {TokenType::EQ, op};
        if (op == "!=") return {TokenType::NE, op};
        if (op == ">")  return {TokenType::GT, op};
        if (op == ">=") return {TokenType::GE, op};

        return {TokenType::ERROR, op};
    }

    Token ParseIdentifier() {
        size_t start = m_position;

        while (m_position < m_input.length() && IsIdentifierPart(m_input[m_position])) {
            m_position++;
        }

        return {TokenType::IDENTIFIER, m_input.substr(start, m_position - start)};
    }

    Token ParseNumber() {
        size_t start = m_position;

        while (m_position < m_input.length() && std::isdigit(m_input[m_position])) {
            m_position++;
        }

        return {TokenType::NUMBER, m_input.substr(start, m_position - start)};
    }
};

} // namespace JX3DPS

#endif // JX3DPS_MACRO_LEXER_H
