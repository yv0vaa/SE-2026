#include "shell/pipeline.hpp"

namespace shell {

void Pipeline::addCommand(std::unique_ptr<Command> command) {
    commands_.push_back(std::move(command));
}

size_t Pipeline::size() const {
    return commands_.size();
}

Command& Pipeline::getCommand(size_t index) {
    return *commands_.at(index);
}

bool Pipeline::isEmpty() const {
    return commands_.empty();
}

}  // namespace shell
