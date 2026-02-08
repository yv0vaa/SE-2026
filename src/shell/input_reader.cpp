#include "shell/input_reader.hpp"

namespace shell {

InputReader::InputReader(std::istream& input)
    : input_(input) {}

std::optional<std::string> InputReader::readLine() {
    if (showPrompt_) {
        std::cout << prompt_ << std::flush;
    }
    
    std::string line;
    if (std::getline(input_, line)) {
        return line;
    }
    
    return std::nullopt;
}

void InputReader::setPrompt(const std::string& prompt) {
    prompt_ = prompt;
}

void InputReader::showPrompt(bool show) {
    showPrompt_ = show;
}

} // namespace shell
