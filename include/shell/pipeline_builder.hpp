#pragma once

#include "command_factory.hpp"
#include "parsed_command.hpp"
#include "pipeline.hpp"

namespace shell {

/**
 * @brief Строитель пайплайнов
 * 
 * Создаёт объекты Pipeline из AST (ParsedPipeline).
 */
class PipelineBuilder {
public:
    /**
     * @brief Создать строитель с указанной фабрикой
     * @param factory Ссылка на CommandFactory
     */
    explicit PipelineBuilder(CommandFactory& factory);

    /**
     * @brief Построить Pipeline из ParsedPipeline
     * @param parsed AST пайплайна
     * @return Готовый Pipeline
     */
    Pipeline build(const ParsedPipeline& parsed);

private:
    CommandFactory& factory_;
};

} // namespace shell
