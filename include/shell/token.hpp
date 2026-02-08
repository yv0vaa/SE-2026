#pragma once

#include <string>

namespace shell {

/**
 * @brief Типы токенов лексического анализа
 */
enum class TokenType {
    WORD,           ///< Обычное слово или строка в кавычках
    PIPE,           ///< Символ |
    ASSIGNMENT,     ///< Оператор = (в контексте VAR=VALUE)
    END_OF_INPUT    ///< Конец ввода
};

/**
 * @brief Токен - минимальная лексическая единица
 */
class Token {
public:
    TokenType type;
    std::string value;

    Token(TokenType type, std::string value = "");

    bool operator==(const Token& other) const;
    bool operator!=(const Token& other) const;
};

/**
 * @brief Преобразовать тип токена в строку для отладки
 */
std::string tokenTypeToString(TokenType type);

} // namespace shell
