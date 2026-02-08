#include "shell/commands/echo_command.hpp"

namespace shell {

int EchoCommand::execute(std::istream& /*in*/, std::ostream& out, std::ostream& /*err*/
) {
    for (size_t i = 0; i < args_.size(); ++i) {
        if (i > 0) {
            out << ' ';
        }
        out << args_[i];
    }
    out << '\n';
    return 0;
}

void EchoCommand::setArguments(const std::vector<std::string>& args) {
    args_ = args;
}

}  // namespace shell
