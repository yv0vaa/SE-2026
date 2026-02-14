#pragma once

#include <memory>
#include <string>
#include <vector>

namespace shell {

/**
 * @brief Базовый класс для узлов AST
 */
class ParsedCommand {
public:
    virtual ~ParsedCommand() = default;
    virtual bool isAssignment() const {
        return false;
    }
    virtual bool isPipeline() const {
        return false;
    }
    virtual bool isEmpty() const {
        return false;
    }
};

/**
 * @brief Пустая команда (пустая строка или только пробелы)
 */
class ParsedEmpty : public ParsedCommand {
public:
    bool isEmpty() const override {
        return true;
    }
};

/**
 * @brief Присваивание переменной: VAR=VALUE
 */
class ParsedAssignment : public ParsedCommand {
public:
    std::string variableName;
    std::string value;

    ParsedAssignment(std::string name, std::string val);

    bool isAssignment() const override {
        return true;
    }
};

/**
 * @brief Простая команда: COMMAND ARG1 ARG2 ...
 */
class ParsedSimpleCommand {
public:
    std::string commandName;
    std::vector<std::string> arguments;

    ParsedSimpleCommand() = default;
    ParsedSimpleCommand(std::string name, std::vector<std::string> args);
};

/**
 * @brief Пайплайн: CMD1 | CMD2 | CMD3
 */
class ParsedPipeline : public ParsedCommand {
public:
    std::vector<ParsedSimpleCommand> commands;

    bool isPipeline() const override {
        return true;
    }

    void addCommand(ParsedSimpleCommand cmd);
};

/**
 * @brief Список присваиваний (x=1 y=2)
 */
class ParsedAssignmentList : public ParsedCommand {
public:
    std::vector<ParsedAssignment> assignments;

    bool isAssignment() const override {
        return true;
    }

    void addAssignment(ParsedAssignment assignment);
};

}  // namespace shell
