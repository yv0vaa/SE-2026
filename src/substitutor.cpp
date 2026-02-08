#include "shell/substitutor.hpp"

namespace shell {

Substitutor::Substitutor(Environment& env)
    : env_(env) {}

std::string Substitutor::substitute(const std::string& input) const {
    std::string result;
    size_t i = 0;

    while (i < input.size()) {
        char c = input[i];

        // Одинарные кавычки — без подстановки
        if (c == '\'') {
            result += c;
            i++;
            while (i < input.size() && input[i] != '\'') {
                result += input[i];
                i++;
            }
            if (i < input.size()) {
                result += input[i]; // закрывающая кавычка
                i++;
            }
        }
        // Подстановка переменных
        else if (c == '$') {
            i++; // пропускаем $

            if (i >= input.size()) {
                result += '$';
                continue;
            }

            // ${VAR} формат
            if (input[i] == '{') {
                i++; // пропускаем {
                std::string varName;
                while (i < input.size() && input[i] != '}') {
                    varName += input[i];
                    i++;
                }
                if (i < input.size()) {
                    i++; // пропускаем }
                }
                result += env_.get(varName);
            }
            // $VAR формат
            else if (isValidVarStartChar(input[i])) {
                std::string varName;
                while (i < input.size() && isValidVarChar(input[i])) {
                    varName += input[i];
                    i++;
                }
                result += env_.get(varName);
            }
            // $? — специальная переменная для кода возврата
            else if (input[i] == '?') {
                result += env_.get("?");
                i++;
            }
            // Просто $ без переменной
            else {
                result += '$';
            }
        }
        else {
            result += c;
            i++;
        }
    }

    return result;
}

bool Substitutor::isValidVarChar(char c) const {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
           (c >= '0' && c <= '9') ||
           c == '_';
}

bool Substitutor::isValidVarStartChar(char c) const {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
           c == '_';
}

} // namespace shell
