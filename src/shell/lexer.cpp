#include "shell/lexer.hpp"

#include <stdexcept>

namespace shell {

Lexer::Lexer(const std::string& input) : input_(input), position_(0) {}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;

    while (position_ < input_.size()) {
        skipWhitespace();

        if (position_ >= input_.size()) {
            break;
        }

        char c = peek();

        if (c == '|') {
            advance();
            tokens.emplace_back(TokenType::PIPE, "|");
        } else if (c == '\'' || c == '"') {
            tokens.push_back(readQuotedString(c));
        } else {
            tokens.push_back(readWord());
        }
    }

    tokens.emplace_back(TokenType::END_OF_INPUT, "");
    return tokens;
}

char Lexer::peek() const {
    if (position_ >= input_.size()) {
        return '\0';
    }
    return input_[position_];
}

char Lexer::advance() {
    if (position_ >= input_.size()) {
        return '\0';
    }
    return input_[position_++];
}

void Lexer::skipWhitespace() {
    while (position_ < input_.size() && (input_[position_] == ' ' || input_[position_] == '\t')) {
        position_++;
    }
}

Token Lexer::readWord() {
    std::string value;

    while (position_ < input_.size()) {
        char c = peek();

        if (c == ' ' || c == '\t' || c == '|') {
            break;
        }

        if (c == '\'' || c == '"') {
            // Кавычки внутри слова — читаем строку в кавычках и добавляем к слову
            Token quoted = readQuotedString(c);
            value += quoted.value;
        } else {
            value += advance();
        }
    }

    return Token(TokenType::WORD, value);
}

Token Lexer::readQuotedString(char quote) {
    advance();  // Пропускаем открывающую кавычку
    std::string value;

    while (position_ < input_.size()) {
        char c = peek();

        if (c == quote) {
            advance();  // Пропускаем закрывающую кавычку
            return Token(TokenType::WORD, value);
        }

        // Обработка escape-последовательностей внутри двойных кавычек
        if (quote == '"' && c == '\\' && position_ + 1 < input_.size()) {
            char next = input_[position_ + 1];
            if (next == '"' || next == '\\' || next == '$') {
                advance();  // Пропускаем backslash
                value += advance();
                continue;
            }
        }

        value += advance();
    }

    // Незакрытая кавычка — возвращаем то, что прочитали
    return Token(TokenType::WORD, value);
}

bool Lexer::isSpecialChar(char c) const {
    return c == '|' || c == '=' || c == '\'' || c == '"';
}

bool Lexer::isWordChar(char c) const {
    return c != ' ' && c != '\t' && c != '|' && c != '\0';
}

}  // namespace shell
