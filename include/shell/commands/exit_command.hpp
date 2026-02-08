#pragma once

#include "../command.hpp"

namespace shell {

/**
 * @brief Команда exit — выход из интерпретатора
 *
 * Устанавливает флаг завершения. Не завершает процесс напрямую,
 * а сигнализирует главному циклу о необходимости выхода.
 */
class ExitCommand : public Command {
public:
    int execute(std::istream& in, std::ostream& out, std::ostream& err) override;

    void setArguments(const std::vector<std::string>& args) override;

    std::string getName() const override {
        return "exit";
    }

    /**
     * @brief Проверить, был ли запрошен выход
     */
    bool wasExitRequested() const {
        return exitRequested_;
    }

    /**
     * @brief Получить код выхода
     */
    int getExitCode() const {
        return exitCode_;
    }

private:
    bool exitRequested_ = false;
    int exitCode_ = 0;
    std::vector<std::string> args_;
};

}  // namespace shell
