#include "shell/environment.hpp"

#include <cstdlib>

extern char** environ;

namespace shell {

std::string Environment::get(const std::string& name) const {
    auto it = variables_.find(name);
    if (it != variables_.end()) {
        return it->second;
    }
    return "";
}

void Environment::set(const std::string& name, const std::string& value) {
    variables_[name] = value;
}

void Environment::unset(const std::string& name) {
    variables_.erase(name);
}

bool Environment::contains(const std::string& name) const {
    return variables_.find(name) != variables_.end();
}

std::vector<std::string> Environment::toEnvp() const {
    std::vector<std::string> result;
    result.reserve(variables_.size());
    for (const auto& [name, value] : variables_) {
        result.push_back(name + "=" + value);
    }
    return result;
}

void Environment::initFromSystem() {
    if (environ == nullptr) {
        return;
    }

    for (char** env = environ; *env != nullptr; ++env) {
        std::string entry(*env);
        size_t pos = entry.find('=');
        if (pos != std::string::npos) {
            std::string name = entry.substr(0, pos);
            std::string value = entry.substr(pos + 1);
            variables_[name] = value;
        }
    }

    // Инициализируем специальную переменную для кода возврата
    variables_["?"] = "0";
}

} // namespace shell
