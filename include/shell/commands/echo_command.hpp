#pragma once

#include "../command.hpp"

namespace shell {

/**
 * @brief Команда echo — вывод аргументов на экран
 */
class EchoCommand : public Command {
public:
    int execute(
        std::istream& in,
        std::ostream& out,
        std::ostream& err
    ) override;

    void setArguments(const std::vector<std::string>& args) override;

    std::string getName() const override { return "echo"; }

private:
    std::vector<std::string> args_;
};

} // namespace shell
