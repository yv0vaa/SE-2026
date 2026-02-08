#pragma once

#include <string>

#include "environment.hpp"

namespace shell {

/**
 * @brief Подстановщик переменных окружения
 * 
 * Заменяет вхождения $VAR и ${VAR} на соответствующие значения
 * из Environment. Выполняется ДО токенизации.
 * 
 * Важно: внутри одинарных кавычек подстановка не выполняется.
 */
class Substitutor {
public:
    /**
     * @brief Создать подстановщик с указанным окружением
     * @param env Ссылка на объект Environment
     */
    explicit Substitutor(Environment& env);

    /**
     * @brief Выполнить подстановку переменных
     * @param input Входная строка
     * @return Строка с подставленными значениями переменных
     */
    std::string substitute(const std::string& input) const;

private:
    Environment& env_;

    bool isValidVarChar(char c) const;
    bool isValidVarStartChar(char c) const;
};

} // namespace shell
