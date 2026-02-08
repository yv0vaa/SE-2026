#pragma once

#include "environment.hpp"
#include "parsed_command.hpp"
#include "pipeline.hpp"

namespace shell {

/**
 * @brief Исполнитель команд и пайплайнов
 */
class Executor {
public:
    /**
     * @brief Создать исполнитель с указанным окружением
     * @param env Ссылка на Environment
     */
    explicit Executor(Environment& env);

    /**
     * @brief Выполнить пайплайн
     * @param pipeline Пайплайн для выполнения
     * @return Код возврата последней команды
     */
    int execute(Pipeline& pipeline);

    /**
     * @brief Выполнить присваивание переменной
     * @param assignment Присваивание
     * @return Код возврата (всегда 0)
     */
    int executeAssignment(const ParsedAssignment& assignment);

    /**
     * @brief Выполнить список присваиваний
     * @param assignments Список присваиваний
     * @return Код возврата (всегда 0)
     */
    int executeAssignments(const ParsedAssignmentList& assignments);

    /**
     * @brief Проверить, был ли запрошен выход
     */
    bool shouldExit() const;

    /**
     * @brief Получить код выхода
     */
    int getExitCode() const;

private:
    Environment& env_;
    bool exitRequested_ = false;
    int exitCode_ = 0;

    int executeSingleCommand(Command& cmd);
    int executePipeline(Pipeline& pipeline);
};

}  // namespace shell
