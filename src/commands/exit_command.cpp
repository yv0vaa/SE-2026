#include "shell/commands/exit_command.hpp"

#include <cstdlib>

namespace shell {

int ExitCommand::execute(
    std::istream& /*in*/,
    std::ostream& /*out*/,
    std::ostream& /*err*/
) {
    exitRequested_ = true;
    
    if (!args_.empty()) {
        try {
            exitCode_ = std::stoi(args_[0]);
        } catch (...) {
            exitCode_ = 0;
        }
    } else {
        exitCode_ = 0;
    }
    
    return exitCode_;
}

void ExitCommand::setArguments(const std::vector<std::string>& args) {
    args_ = args;
}

} // namespace shell
