#pragma once

#include "../command.hpp"

namespace shell {

/**
 * @brief Команда pwd — вывод текущей директории
 */
class PwdCommand : public Command {
public:
    int execute(std::istream& in, std::ostream& out, std::ostream& err) override;

    void setArguments(const std::vector<std::string>& args) override;

    std::string getName() const override {
        return "pwd";
    }
};

}  // namespace shell
