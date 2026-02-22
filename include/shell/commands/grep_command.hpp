#pragma once

#include <cstddef>
#include <regex>
#include <string>
#include <vector>

#include "../command.hpp"

namespace shell {

/**
 * @brief Команда grep — поиск по регулярному выражению
 *
 * Поддерживает:
 * - регулярные выражения в запросе (ECMAScript);
 * - -w — только совпадения целого слова (граница — не буква/цифра/подчёркивание);
 * - -i — регистронезависимый поиск;
 * - -A N — печать N строк после совпадения (-A 0 — только строка совпадения).
 * Разбор аргументов выполняется библиотекой CLI11.
 */
class GrepCommand : public Command {
public:
    int execute(std::istream& in, std::ostream& out, std::ostream& err) override;

    void setArguments(const std::vector<std::string>& args) override;

    std::string getName() const override {
        return "grep";
    }

private:
    std::vector<std::string> args_;

    /// Считается ли символ «словесным» для -w (буква, цифра, подчёркивание)
    static bool isWordChar(unsigned char c);

    /// Проверка, что совпадение regex в строке — целое слово (для -w)
    static bool isWordBoundaryMatch(const std::string& line, std::ptrdiff_t start,
                                    std::ptrdiff_t length);

    /// Поиск по одному потоку: строки в lines, записать номера строк для вывода в out_line_indices
    void grepStream(const std::vector<std::string>& lines,
                    std::vector<std::size_t>& out_line_indices) const;

    /// Собранные после разбора аргументов опции (валидны во время execute)
    std::string pattern_;
    bool case_insensitive_ = false;
    bool word_only_ = false;
    int lines_after_ = 0;
    std::vector<std::string> filenames_;
};

}  // namespace shell
