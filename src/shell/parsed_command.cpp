#include "shell/parsed_command.hpp"

namespace shell {

ParsedAssignment::ParsedAssignment(std::string name, std::string val)
    : variableName(std::move(name)), value(std::move(val)) {}

ParsedSimpleCommand::ParsedSimpleCommand(std::string name, std::vector<std::string> args)
    : commandName(std::move(name)), arguments(std::move(args)) {}

void ParsedPipeline::addCommand(ParsedSimpleCommand cmd) {
    commands.push_back(std::move(cmd));
}

void ParsedAssignmentList::addAssignment(ParsedAssignment assignment) {
    assignments.push_back(std::move(assignment));
}

}  // namespace shell
