#include "shell/shell.hpp"

namespace shell {

Shell::Shell()
    : environment_(),
      inputReader_(),
      substitutor_(environment_),
      commandFactory_(environment_),
      pipelineBuilder_(commandFactory_),
      executor_(environment_) {
    environment_.initFromSystem();
}

int Shell::run() {
    while (!executor_.shouldExit()) {
        auto line = inputReader_.readLine();

        if (!line) {
            // EOF
            break;
        }

        if (line->empty()) {
            continue;
        }

        processLine(*line);
    }

    return executor_.getExitCode();
}

int Shell::processLine(const std::string& line) {
    // 1. Подстановка переменных
    std::string substituted = substitutor_.substitute(line);

    // 2. Лексический анализ
    Lexer lexer(substituted);
    std::vector<Token> tokens = lexer.tokenize();

    // 3. Синтаксический анализ
    Parser parser(std::move(tokens));
    auto parsed = parser.parse();

    if (!parsed) {
        return 0;
    }

    // 4. Выполнение
    if (parsed->isEmpty()) {
        return 0;
    }

    if (parsed->isAssignment()) {
        if (auto* assignment = dynamic_cast<ParsedAssignment*>(parsed.get())) {
            return executor_.executeAssignment(*assignment);
        }
        if (auto* assignments = dynamic_cast<ParsedAssignmentList*>(parsed.get())) {
            return executor_.executeAssignments(*assignments);
        }
    }

    if (parsed->isPipeline()) {
        auto* pipelineAst = dynamic_cast<ParsedPipeline*>(parsed.get());
        if (pipelineAst && !pipelineAst->commands.empty()) {
            Pipeline pipeline = pipelineBuilder_.build(*pipelineAst);
            return executor_.execute(pipeline);
        }
    }

    return 0;
}

}  // namespace shell
