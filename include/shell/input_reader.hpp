#pragma once

#include <iostream>
#include <optional>
#include <string>

namespace shell {

/**
 * @brief Чтение пользовательского ввода
 */
class InputReader {
public:
    /**
     * @brief Создать читатель с указанным потоком
     * @param input Входной поток (по умолчанию stdin)
     */
    explicit InputReader(std::istream& input = std::cin);

    /**
     * @brief Прочитать строку
     * @return Строка или nullopt при EOF
     */
    std::optional<std::string> readLine();

    /**
     * @brief Установить приглашение командной строки
     * @param prompt Строка приглашения
     */
    void setPrompt(const std::string& prompt);

    /**
     * @brief Включить/выключить вывод приглашения
     * @param show true — выводить приглашение
     */
    void showPrompt(bool show);

private:
    std::istream& input_;
    std::string prompt_ = "> ";
    bool showPrompt_ = true;
};

}  // namespace shell
