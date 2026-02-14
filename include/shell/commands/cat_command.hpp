#pragma once

#include "../command.hpp"

namespace shell {

/**
 * @brief Команда cat — вывод содержимого файла
 *
 * Если аргументы не указаны, копирует входной поток в выходной.
 */
class CatCommand : public Command {
public:
    int execute(std::istream& in, std::ostream& out, std::ostream& err) override;

    void setArguments(const std::vector<std::string>& args) override;

    std::string getName() const override {
        return "cat";
    }

private:
    std::vector<std::string> filenames_;
};

}  // namespace shell
