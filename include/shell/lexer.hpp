#pragma once

#include <string>
#include <vector>

#include "token.hpp"

namespace shell {

/**
 * @brief Лексический анализатор командной строки
 *
 * Разбивает входную строку на токены с учётом:
 * - Кавычек (одинарных и двойных)
 * - Специальных символов (|, =)
 * - Пробелов как разделителей
 */
class Lexer {
public:
    /**
     * @brief Создать лексер для входной строки
     * @param input Строка для анализа
     */
    explicit Lexer(const std::string& input);

    /**
     * @brief Выполнить токенизацию
     * @return Вектор токенов
     */
    std::vector<Token> tokenize();

private:
    std::string input_;
    size_t position_;

    char peek() const;
    char advance();
    void skipWhitespace();
    Token readWord();
    Token readQuotedString(char quote);
    bool isSpecialChar(char c) const;
    bool isWordChar(char c) const;
};

}  // namespace shell
