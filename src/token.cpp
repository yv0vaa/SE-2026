#include "shell/token.hpp"

namespace shell {

Token::Token(TokenType type, std::string value)
    : type(type), value(std::move(value)) {}

bool Token::operator==(const Token& other) const {
    return type == other.type && value == other.value;
}

bool Token::operator!=(const Token& other) const {
    return !(*this == other);
}

std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::WORD:
            return "WORD";
        case TokenType::PIPE:
            return "PIPE";
        case TokenType::ASSIGNMENT:
            return "ASSIGNMENT";
        case TokenType::END_OF_INPUT:
            return "END_OF_INPUT";
        default:
            return "UNKNOWN";
    }
}

} // namespace shell
