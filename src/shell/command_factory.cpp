#include "shell/command_factory.hpp"

#include "shell/commands/cat_command.hpp"
#include "shell/commands/echo_command.hpp"
#include "shell/commands/exit_command.hpp"
#include "shell/commands/external_command.hpp"
#include "shell/commands/grep_command.hpp"
#include "shell/commands/pwd_command.hpp"
#include "shell/commands/wc_command.hpp"

namespace shell {

CommandFactory::CommandFactory(Environment& env) : env_(env) {
    registerBuiltins();
}

std::unique_ptr<Command> CommandFactory::create(const std::string& name) {
    auto it = builtinFactories_.find(name);
    if (it != builtinFactories_.end()) {
        return it->second();
    }

    // Внешняя команда
    return std::make_unique<ExternalCommand>(name, env_);
}

bool CommandFactory::isBuiltin(const std::string& name) const {
    return builtinFactories_.find(name) != builtinFactories_.end();
}

void CommandFactory::registerBuiltins() {
    builtinFactories_["echo"] = []() { return std::make_unique<EchoCommand>(); };

    builtinFactories_["cat"] = []() { return std::make_unique<CatCommand>(); };

    builtinFactories_["wc"] = []() { return std::make_unique<WcCommand>(); };

    builtinFactories_["pwd"] = []() { return std::make_unique<PwdCommand>(); };

    builtinFactories_["exit"] = []() { return std::make_unique<ExitCommand>(); };

    builtinFactories_["grep"] = []() { return std::make_unique<GrepCommand>(); };
}

}  // namespace shell
