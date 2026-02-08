#pragma once

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

#include "command.hpp"
#include "environment.hpp"

namespace shell {

/**
 * @brief Фабрика команд
 *
 * Создаёт объекты команд по имени. Для встроенных команд
 * создаёт соответствующие классы, для остальных — ExternalCommand.
 */
class CommandFactory {
public:
    /**
     * @brief Создать фабрику с указанным окружением
     * @param env Ссылка на Environment
     */
    explicit CommandFactory(Environment& env);

    /**
     * @brief Создать команду по имени
     * @param name Имя команды
     * @return Указатель на созданную команду
     */
    std::unique_ptr<Command> create(const std::string& name);

    /**
     * @brief Проверить, является ли команда встроенной
     * @param name Имя команды
     * @return true, если команда встроенная
     */
    bool isBuiltin(const std::string& name) const;

private:
    Environment& env_;
    std::unordered_map<std::string, std::function<std::unique_ptr<Command>()>> builtinFactories_;

    void registerBuiltins();
};

}  // namespace shell
