#include "shell/commands/pwd_command.hpp"

#include <unistd.h>
#include <climits>

namespace shell {

int PwdCommand::execute(
    std::istream& /*in*/,
    std::ostream& out,
    std::ostream& err
) {
    char buffer[PATH_MAX];
    
    if (getcwd(buffer, sizeof(buffer)) != nullptr) {
        out << buffer << '\n';
        return 0;
    }
    
    err << "pwd: error getting current directory\n";
    return 1;
}

void PwdCommand::setArguments(const std::vector<std::string>& /*args*/) {
    // pwd игнорирует аргументы
}

} // namespace shell
