#include "shell/commands/grep_command.hpp"

#include <fstream>
#include <set>
#include <sstream>

#include <CLI/CLI.hpp>

namespace shell {

bool GrepCommand::isWordChar(unsigned char c) {
    return std::isalnum(static_cast<unsigned char>(c)) || c == '_';
}

bool GrepCommand::isWordBoundaryMatch(const std::string& line, std::ptrdiff_t start,
                                      std::ptrdiff_t length) {
    auto n = static_cast<std::ptrdiff_t>(line.size());
    bool ok_before =
        (start == 0) ||
        !isWordChar(static_cast<unsigned char>(line[static_cast<std::size_t>(start - 1)]));
    std::ptrdiff_t end_pos = start + length;
    bool ok_after =
        (end_pos >= n) ||
        !isWordChar(static_cast<unsigned char>(line[static_cast<std::size_t>(end_pos)]));
    return ok_before && ok_after;
}

void GrepCommand::grepStream(const std::vector<std::string>& lines,
                             std::vector<std::size_t>& out_line_indices) const {
    std::regex::flag_type flags = std::regex::ECMAScript;
    if (case_insensitive_) {
        flags |= std::regex::icase;
    }

    std::regex re;
    try {
        re.assign(pattern_, flags);
    } catch (const std::regex_error&) {
        return;
    }

    std::set<std::size_t> to_print;
    for (std::size_t i = 0; i < lines.size(); ++i) {
        const std::string& line = lines[i];
        std::smatch m;
        if (!std::regex_search(line, m, re)) {
            continue;
        }
        if (word_only_ && !isWordBoundaryMatch(line, m.position(), m.length())) {
            continue;
        }
        for (int a = 0; a <= lines_after_; ++a) {
            std::size_t idx = i + static_cast<std::size_t>(a);
            if (idx < lines.size()) {
                to_print.insert(idx);
            }
        }
    }

    out_line_indices.assign(to_print.begin(), to_print.end());
}

int GrepCommand::execute(std::istream& in, std::ostream& out, std::ostream& err) {
    // Определение опций через CLI11 (библиотека для разбора аргументов)
    CLI::App app("grep");
    app.failure_message(CLI::FailureMessage::help);
    bool flag_w = false;
    bool flag_i = false;
    int flag_A = 0;

    app.add_flag("-w,--word-regexp", flag_w, "Match only whole words");
    app.add_flag("-i,--ignore-case", flag_i, "Ignore case");
    app.add_option("-A,--after-context", flag_A, "Print N lines after match")
        ->default_val(0)
        ->check(CLI::NonNegativeNumber);

    // Разбор: отделяем опции от позиционных аргументов (паттерн и файлы)
    std::vector<std::string> positionals;
    for (size_t i = 0; i < args_.size(); ++i) {
        if (args_[i] == "-w") {
            flag_w = true;
        } else if (args_[i] == "-i") {
            flag_i = true;
        } else if (args_[i] == "-A" && i + 1 < args_.size()) {
            try {
                int val = std::stoi(args_[i + 1]);
                if (val < 0) {
                    err << "grep: -A requires non-negative number\n";
                    return 2;
                }
                flag_A = val;
            } catch (const std::exception&) {
                err << "grep: -A requires a number\n";
                return 2;
            }
            ++i;
        } else {
            positionals.push_back(args_[i]);
        }
    }
    if (positionals.empty()) {
        err << "grep: pattern is required\n";
        return 2;
    }
    if (positionals[0].size() >= 1 && positionals[0][0] == '-') {
        err << "grep: pattern is required\n";
        return 2;
    }
    pattern_ = positionals[0];
    filenames_.assign(positionals.begin() + 1, positionals.end());

    case_insensitive_ = flag_i;
    word_only_ = flag_w;
    lines_after_ = flag_A;

    auto readLines = [](std::istream& stream) {
        std::vector<std::string> lines;
        std::string line;
        while (std::getline(stream, line)) {
            lines.push_back(line);
        }
        return lines;
    };

    int exit_code = 1;
    bool had_match = false;

    if (filenames_.empty()) {
        std::vector<std::string> lines = readLines(in);
        std::vector<std::size_t> indices;
        grepStream(lines, indices);
        for (std::size_t idx : indices) {
            out << lines[idx] << "\n";
            had_match = true;
        }
        if (had_match) {
            exit_code = 0;
        }
        return exit_code;
    }

    for (const auto& filename : filenames_) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            err << "grep: " << filename << ": No such file or directory\n";
            exit_code = 1;
            continue;
        }
        std::vector<std::string> lines = readLines(file);
        std::vector<std::size_t> indices;
        grepStream(lines, indices);
        for (std::size_t idx : indices) {
            out << lines[idx] << "\n";
            had_match = true;
        }
    }

    return had_match ? 0 : 1;
}

void GrepCommand::setArguments(const std::vector<std::string>& args) {
    args_ = args;
}

}  // namespace shell
