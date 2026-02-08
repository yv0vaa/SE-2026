#pragma once

#include <optional>

#include "../command.hpp"
#include "../environment.hpp"

namespace shell {

/**
 * @brief Внешняя команда — запуск произвольной программы
 * 
 * Создаёт дочерний процесс и выполняет программу через execve.
 */
class ExternalCommand : public Command {
public:
    /**
     * @brief Создать внешнюю команду
     * @param programName Имя программы (или путь к ней)
     * @param env Ссылка на окружение
     */
    ExternalCommand(const std::string& programName, Environment& env);

    int execute(
        std::istream& in,
        std::ostream& out,
        std::ostream& err
    ) override;

    void setArguments(const std::vector<std::string>& args) override;

    std::string getName() const override { return programName_; }

private:
    std::string programName_;
    std::vector<std::string> args_;
    Environment& env_;

    /**
     * @brief Найти исполняемый файл в PATH
     * @return Полный путь к исполняемому файлу или nullopt
     */
    std::optional<std::string> findExecutable() const;
};

} // namespace shell
