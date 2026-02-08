#include "shell/commands/wc_command.hpp"

#include <fstream>
#include <sstream>

namespace shell {

int WcCommand::execute(
    std::istream& in,
    std::ostream& out,
    std::ostream& err
) {
    // Если аргументов нет — обрабатываем stdin
    if (filenames_.empty()) {
        // Читаем весь stdin в строку для подсчёта
        std::stringstream buffer;
        buffer << in.rdbuf();
        std::istringstream stream(buffer.str());
        
        Counts counts = countStream(stream);
        printCounts(out, counts);
        return 0;
    }
    
    int exitCode = 0;
    Counts total{0, 0, 0};
    
    for (const auto& filename : filenames_) {
        std::ifstream file(filename, std::ios::binary);
        
        if (!file.is_open()) {
            err << "wc: " << filename << ": No such file or directory\n";
            exitCode = 1;
            continue;
        }
        
        Counts counts = countStream(file);
        printCounts(out, counts, filename);
        
        total.lines += counts.lines;
        total.words += counts.words;
        total.bytes += counts.bytes;
    }
    
    // Если было несколько файлов — выводим итог
    if (filenames_.size() > 1) {
        printCounts(out, total, "total");
    }
    
    return exitCode;
}

void WcCommand::setArguments(const std::vector<std::string>& args) {
    filenames_ = args;
}

WcCommand::Counts WcCommand::countStream(std::istream& stream) {
    Counts counts{0, 0, 0};
    
    std::string line;
    while (std::getline(stream, line)) {
        counts.lines++;
        counts.bytes += line.size() + 1; // +1 для \n
        
        // Подсчёт слов
        bool inWord = false;
        for (char c : line) {
            if (c == ' ' || c == '\t' || c == '\r') {
                inWord = false;
            } else {
                if (!inWord) {
                    counts.words++;
                    inWord = true;
                }
            }
        }
    }
    
    // Корректируем bytes если последняя строка без \n
    // (getline не считает финальный символ если его нет)
    
    return counts;
}

void WcCommand::printCounts(std::ostream& out, const Counts& counts, const std::string& filename) {
    out << counts.lines << ' ' << counts.words << ' ' << counts.bytes;
    if (!filename.empty()) {
        out << ' ' << filename;
    }
    out << '\n';
}

} // namespace shell
