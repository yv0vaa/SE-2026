#pragma once

#include <iostream>
#include <string>
#include <vector>

namespace shell {

/**
 * @brief Базовый интерфейс для всех команд
 *
 * Все команды (встроенные и внешние) реализуют этот интерфейс.
 */
class Command {
public:
    virtual ~Command() = default;

    /**
     * @brief Выполнить команду
     * @param inputStream Входной поток (от предыдущей команды в пайпе или stdin)
     * @param outputStream Выходной поток для результата
     * @param errorStream Поток для ошибок
     * @return Код возврата (0 — успех)
     */
    virtual int execute(std::istream& inputStream, std::ostream& outputStream,
                        std::ostream& errorStream) = 0;

    /**
     * @brief Установить аргументы команды
     * @param args Вектор аргументов
     */
    virtual void setArguments(const std::vector<std::string>& args) = 0;

    /**
     * @brief Получить имя команды
     * @return Имя команды
     */
    virtual std::string getName() const = 0;
};

}  // namespace shell
