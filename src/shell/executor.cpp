#include "shell/executor.hpp"

#include <iostream>
#include <sstream>

#include "shell/commands/exit_command.hpp"

namespace shell {

Executor::Executor(Environment& env) : env_(env) {}

int Executor::execute(Pipeline& pipeline) {
    if (pipeline.isEmpty()) {
        return 0;
    }

    if (pipeline.size() == 1) {
        return executeSingleCommand(pipeline.getCommand(0));
    }

    return executePipeline(pipeline);
}

int Executor::executeAssignment(const ParsedAssignment& assignment) {
    env_.set(assignment.variableName, assignment.value);
    return 0;
}

int Executor::executeAssignments(const ParsedAssignmentList& assignments) {
    for (const auto& assignment : assignments.assignments) {
        env_.set(assignment.variableName, assignment.value);
    }
    return 0;
}

bool Executor::shouldExit() const {
    return exitRequested_;
}

int Executor::getExitCode() const {
    return exitCode_;
}

int Executor::executeSingleCommand(Command& cmd) {
    std::istringstream emptyInput;
    int returnCode = cmd.execute(emptyInput, std::cout, std::cerr);

    // Проверяем, была ли это команда exit
    if (auto* exitCmd = dynamic_cast<ExitCommand*>(&cmd)) {
        if (exitCmd->wasExitRequested()) {
            exitRequested_ = true;
            exitCode_ = exitCmd->getExitCode();
        }
    }

    // Обновляем переменную $?
    env_.set("?", std::to_string(returnCode));

    return returnCode;
}

int Executor::executePipeline(Pipeline& pipeline) {
    int returnCode = 0;

    // Буфер для передачи данных между командами
    std::stringstream buffer;

    for (size_t i = 0; i < pipeline.size(); ++i) {
        Command& cmd = pipeline.getCommand(i);

        // Входной поток — результат предыдущей команды
        std::istringstream inputStream(buffer.str());
        buffer.str("");
        buffer.clear();

        // Определяем выходной поток
        if (i == pipeline.size() - 1) {
            // Последняя команда — пишем в stdout
            returnCode = cmd.execute(inputStream, std::cout, std::cerr);
        } else {
            // Промежуточная команда — пишем в буфер
            returnCode = cmd.execute(inputStream, buffer, std::cerr);
        }

        // Проверяем команду exit
        if (auto* exitCmd = dynamic_cast<ExitCommand*>(&cmd)) {
            if (exitCmd->wasExitRequested()) {
                exitRequested_ = true;
                exitCode_ = exitCmd->getExitCode();
                break;
            }
        }
    }

    // Обновляем переменную $?
    env_.set("?", std::to_string(returnCode));

    return returnCode;
}

}  // namespace shell
