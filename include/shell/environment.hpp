#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace shell {

/**
 * @brief Класс для управления переменными окружения
 *
 * Хранит переменные окружения и предоставляет методы для их
 * чтения, установки и удаления. При инициализации может
 * загружать системное окружение.
 */
class Environment {
public:
    Environment() = default;

    /**
     * @brief Получить значение переменной
     * @param name Имя переменной
     * @return Значение переменной или пустая строка, если не определена
     */
    std::string get(const std::string& name) const;

    /**
     * @brief Установить значение переменной
     * @param name Имя переменной
     * @param value Значение переменной
     */
    void set(const std::string& name, const std::string& value);

    /**
     * @brief Удалить переменную
     * @param name Имя переменной
     */
    void unset(const std::string& name);

    /**
     * @brief Проверить, определена ли переменная
     * @param name Имя переменной
     * @return true, если переменная определена
     */
    bool contains(const std::string& name) const;

    /**
     * @brief Преобразовать в формат для execve
     * @return Вектор строк в формате "NAME=VALUE"
     */
    std::vector<std::string> toEnvp() const;

    /**
     * @brief Инициализировать из системного окружения
     */
    void initFromSystem();

private:
    std::unordered_map<std::string, std::string> variables_;
};

}  // namespace shell
