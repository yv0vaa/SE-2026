#include "shell/pipeline_builder.hpp"

namespace shell {

PipelineBuilder::PipelineBuilder(CommandFactory& factory)
    : factory_(factory) {}

Pipeline PipelineBuilder::build(const ParsedPipeline& parsed) {
    Pipeline pipeline;
    
    for (const auto& parsedCmd : parsed.commands) {
        auto command = factory_.create(parsedCmd.commandName);
        command->setArguments(parsedCmd.arguments);
        pipeline.addCommand(std::move(command));
    }
    
    return pipeline;
}

} // namespace shell
