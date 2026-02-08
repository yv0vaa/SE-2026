#pragma once

#include "../command.hpp"

namespace shell {

/**
 * @brief Команда wc — подсчёт строк, слов и байт
 */
class WcCommand : public Command {
public:
    int execute(std::istream& in, std::ostream& out, std::ostream& err) override;

    void setArguments(const std::vector<std::string>& args) override;

    std::string getName() const override {
        return "wc";
    }

private:
    std::vector<std::string> filenames_;

    struct Counts {
        size_t lines = 0;
        size_t words = 0;
        size_t bytes = 0;
    };

    Counts countStream(std::istream& stream);
    void printCounts(std::ostream& out, const Counts& counts, const std::string& filename = "");
};

}  // namespace shell
