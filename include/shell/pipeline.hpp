#pragma once

#include <memory>
#include <vector>

#include "command.hpp"

namespace shell {

/**
 * @brief Пайплайн — последовательность команд
 *
 * Представляет конвейер команд, связанных через pipe.
 */
class Pipeline {
public:
    Pipeline() = default;

    /**
     * @brief Добавить команду в конец пайплайна
     * @param command Указатель на команду
     */
    void addCommand(std::unique_ptr<Command> command);

    /**
     * @brief Получить количество команд
     */
    size_t size() const;

    /**
     * @brief Получить команду по индексу
     * @param index Индекс команды
     * @return Ссылка на команду
     */
    Command& getCommand(size_t index);

    /**
     * @brief Проверить, пуст ли пайплайн
     */
    bool isEmpty() const;

    /**
     * @brief Получить итератор на начало
     */
    auto begin() {
        return commands_.begin();
    }
    auto end() {
        return commands_.end();
    }

private:
    std::vector<std::unique_ptr<Command>> commands_;
};

}  // namespace shell
