#include "shell/shell.hpp"

#include <csignal>

int main() {
    // На Linux запись в pipe с закрытым читающим концом даёт SIGPIPE (например echo x | false).
    std::signal(SIGPIPE, SIG_IGN);

    shell::Shell shell;
    return shell.run();
}
