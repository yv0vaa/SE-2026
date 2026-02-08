#include "shell/parser.hpp"

#include <stdexcept>

namespace shell {

Parser::Parser(std::vector<Token> tokens)
    : tokens_(std::move(tokens)), position_(0) {}

std::unique_ptr<ParsedCommand> Parser::parse() {
    return parseCommandLine();
}

const Token& Parser::current() const {
    if (position_ >= tokens_.size()) {
        static Token endToken(TokenType::END_OF_INPUT, "");
        return endToken;
    }
    return tokens_[position_];
}

const Token& Parser::advance() {
    if (!isAtEnd()) {
        position_++;
    }
    return tokens_[position_ - 1];
}

bool Parser::isAtEnd() const {
    return position_ >= tokens_.size() || 
           tokens_[position_].type == TokenType::END_OF_INPUT;
}

bool Parser::check(TokenType type) const {
    return !isAtEnd() && current().type == type;
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::isAssignmentToken(const Token& token) const {
    if (token.type != TokenType::WORD) {
        return false;
    }
    
    const std::string& value = token.value;
    size_t eqPos = value.find('=');
    
    // Должен быть знак = и непустое имя переменной до него
    if (eqPos == std::string::npos || eqPos == 0) {
        return false;
    }
    
    // Проверяем, что имя переменной валидное
    for (size_t i = 0; i < eqPos; i++) {
        char c = value[i];
        if (i == 0) {
            // Первый символ — буква или _
            if (!((c >= 'a' && c <= 'z') || 
                  (c >= 'A' && c <= 'Z') || 
                  c == '_')) {
                return false;
            }
        } else {
            // Остальные — буква, цифра или _
            if (!((c >= 'a' && c <= 'z') || 
                  (c >= 'A' && c <= 'Z') ||
                  (c >= '0' && c <= '9') || 
                  c == '_')) {
                return false;
            }
        }
    }
    
    return true;
}

std::unique_ptr<ParsedCommand> Parser::parseCommandLine() {
    // Пустая строка
    if (isAtEnd()) {
        return std::make_unique<ParsedEmpty>();
    }
    
    // Проверяем, начинается ли строка с присваиваний
    std::vector<ParsedAssignment> assignments;
    
    while (!isAtEnd() && isAssignmentToken(current())) {
        const std::string& value = current().value;
        size_t eqPos = value.find('=');
        std::string varName = value.substr(0, eqPos);
        std::string varValue = value.substr(eqPos + 1);
        assignments.emplace_back(std::move(varName), std::move(varValue));
        advance();
    }
    
    // Если после присваиваний ничего нет — это только присваивания
    if (isAtEnd()) {
        if (assignments.empty()) {
            return std::make_unique<ParsedEmpty>();
        }
        if (assignments.size() == 1) {
            return std::make_unique<ParsedAssignment>(
                std::move(assignments[0].variableName),
                std::move(assignments[0].value)
            );
        }
        auto list = std::make_unique<ParsedAssignmentList>();
        for (auto& a : assignments) {
            list->addAssignment(std::move(a));
        }
        return list;
    }
    
    // Иначе — парсим пайплайн
    // (присваивания перед командой пока игнорируем — для полной совместимости
    // с bash нужна более сложная логика)
    
    auto pipeline = std::make_unique<ParsedPipeline>();
    
    // Первая команда
    pipeline->addCommand(parseSimpleCommand());
    
    // Остальные команды через |
    while (match(TokenType::PIPE)) {
        pipeline->addCommand(parseSimpleCommand());
    }
    
    return pipeline;
}

ParsedSimpleCommand Parser::parseSimpleCommand() {
    ParsedSimpleCommand cmd;
    
    if (isAtEnd()) {
        return cmd;
    }
    
    // Первый токен — имя команды
    if (current().type == TokenType::WORD) {
        cmd.commandName = current().value;
        advance();
    }
    
    // Остальные токены — аргументы
    while (!isAtEnd() && current().type == TokenType::WORD) {
        cmd.arguments.push_back(current().value);
        advance();
    }
    
    return cmd;
}

} // namespace shell
