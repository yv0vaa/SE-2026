#pragma once

#include <memory>
#include <vector>

#include "parsed_command.hpp"
#include "token.hpp"

namespace shell {

/**
 * @brief Синтаксический анализатор
 *
 * Строит AST (абстрактное синтаксическое дерево) из токенов.
 */
class Parser {
public:
    /**
     * @brief Создать парсер для списка токенов
     * @param tokens Вектор токенов
     */
    explicit Parser(std::vector<Token> tokens);

    /**
     * @brief Выполнить парсинг
     * @return Указатель на корневой узел AST
     */
    std::unique_ptr<ParsedCommand> parse();

private:
    std::vector<Token> tokens_;
    size_t position_;

    const Token& current() const;
    const Token& advance();
    bool isAtEnd() const;
    bool check(TokenType type) const;
    bool match(TokenType type);

    std::unique_ptr<ParsedCommand> parseCommandLine();
    ParsedSimpleCommand parseSimpleCommand();
    bool isAssignmentToken(const Token& token) const;
};

}  // namespace shell
