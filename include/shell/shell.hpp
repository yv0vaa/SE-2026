#pragma once

#include <memory>

#include "command_factory.hpp"
#include "environment.hpp"
#include "executor.hpp"
#include "input_reader.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "pipeline_builder.hpp"
#include "substitutor.hpp"

namespace shell {

/**
 * @brief Главный класс интерпретатора командной оболочки
 *
 * Управляет REPL-циклом и координирует работу всех компонентов.
 */
class Shell {
public:
    Shell();

    /**
     * @brief Запустить интерпретатор
     * @return Код возврата
     */
    int run();

    /**
     * @brief Обработать одну строку команды
     * @param line Строка команды
     * @return Код возврата
     */
    int processLine(const std::string& line);

    /**
     * @brief Получить ссылку на окружение (для тестов)
     */
    Environment& getEnvironment() {
        return environment_;
    }

private:
    Environment environment_;
    InputReader inputReader_;
    Substitutor substitutor_;
    CommandFactory commandFactory_;
    PipelineBuilder pipelineBuilder_;
    Executor executor_;
};

}  // namespace shell
