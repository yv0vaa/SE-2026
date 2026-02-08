#include "shell/commands/cat_command.hpp"

#include <fstream>

namespace shell {

int CatCommand::execute(
    std::istream& in,
    std::ostream& out,
    std::ostream& err
) {
    // Если аргументов нет — копируем stdin в stdout
    if (filenames_.empty()) {
        out << in.rdbuf();
        return 0;
    }
    
    int exitCode = 0;
    
    for (const auto& filename : filenames_) {
        std::ifstream file(filename);
        
        if (!file.is_open()) {
            err << "cat: " << filename << ": No such file or directory\n";
            exitCode = 1;
            continue;
        }
        
        out << file.rdbuf();
    }
    
    return exitCode;
}

void CatCommand::setArguments(const std::vector<std::string>& args) {
    filenames_ = args;
}

} // namespace shell
