#ifndef JX3DPS_LEXER_HPP
#define JX3DPS_LEXER_HPP

#include <string>

enum class TokenType
{
    CAST,
    FCAST,
    SWITCH,
    USE,
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
    ERROR,
    END
};

struct Token
{
    TokenType   type;
    std::string value;

    Token() = default;

    Token(TokenType t, std::string v) : type(t), value(std::move(v)) { }
};

class Lexer
{
private:
    std::string _input;
    size_t      _position = 0;

public:
    explicit Lexer(std::string _input) : _input(std::move(_input)) { }

    Token NextToken()
    {
        SkipWhitespace();
        if (_position >= _input.length()) {
            return { TokenType::END, "" };
        }

        char current = _input[_position];
        if (current == '/') {
            _position++;
            if (_position < _input.length() && _input[_position] == 'c') {
                _position += 4; // Skip "cast"
                return { TokenType::CAST, "/cast" };
            } else if (_position < _input.length() && _input[_position] == 'f') {
                _position += 5; // Skip "fcast"
                return { TokenType::FCAST, "/fcast" };
            } else if (_position < _input.length() && _input[_position] == 's') {
                _position += 6; // Skip "switch"
                return { TokenType::SWITCH, "/switch" };
            } else if (_position < _input.length() && _input[_position] == 'u') {
                _position += 3; // Skip "use"
                return { TokenType::USE, "/use" };
            } else {
                return { TokenType::ERROR, "/" };
            }
        }
        if (current == '[') {
            _position++;
            return { TokenType::LBRACKET, std::string(1, current++) };
        }
        if (current == ']') {
            _position++;
            return { TokenType::RBRACKET, std::string(1, current++) };
        }
        if (current == '&') {
            _position++;
            return { TokenType::AND, std::string(1, current++) };
        }
        if (current == '|') {
            _position++;
            return { TokenType::OR, std::string(1, current++) };
        }
        if (current == ':') {
            _position++;
            return { TokenType::COLON, std::string(1, current++) };
        }

        if (current == '<' || current == '>' || current == '=' || current == '!') {
            std::string op;
            op += current;
            _position++;
            if (_position < _input.length() && _input[_position] == '=') {
                op += '=';
                _position++;
            }
            if (op == "<") {
                return { TokenType::LT, op };
            }
            if (op == "<=") {
                return { TokenType::LE, op };
            }
            if (op == "==") {
                return { TokenType::EQ, op };
            }
            if (op == "!=") {
                return { TokenType::NE, op };
            }
            if (op == ">") {
                return { TokenType::GT, op };
            }
            if (op == ">=") {
                return { TokenType::GE, op };
            }
        }

        if ((current & 0x80) || std::isalpha(current) || current == '_') {
            return Identifier();
        }
        if (std::isdigit(current)) {
            return Number();
        }

        return { TokenType::ERROR, std::string(1, current) };
    }

private:
    void SkipWhitespace()
    {
        while (_position < _input.length() && std::isspace(_input[_position])) {
            _position++;
        }
    }

    Token Identifier()
    {
        size_t start = _position;
        while (_position < _input.length()) {
            // 检查是否为中文字符或字母、数字、下划线
            if ((_input[_position] & 0x80) || std::isalnum(_input[_position]) || _input[_position] == '_')
            {
                _position++;
            } else {
                break;
            }
        }
        return { TokenType::IDENTIFIER, _input.substr(start, _position - start) };
    }

    Token Number()
    {
        size_t start = _position;
        while (_position < _input.length() && std::isdigit(_input[_position])) {
            _position++;
        }
        return { TokenType::NUMBER, _input.substr(start, _position - start) };
    }
};

#endif // JX3DPS_LEXER_HPP