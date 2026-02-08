#include "shell/commands/external_command.hpp"

#include <sys/wait.h>
#include <unistd.h>

#include <cstring>
#include <sstream>

namespace shell {

ExternalCommand::ExternalCommand(const std::string& programName, Environment& env)
    : programName_(programName), env_(env) {}

int ExternalCommand::execute(
    std::istream& in,
    std::ostream& out,
    std::ostream& err
) {
    // Ищем исполняемый файл
    auto execPath = findExecutable();
    if (!execPath) {
        err << programName_ << ": command not found\n";
        return 127;
    }

    // Создаём каналы для stdin и stdout
    int stdinPipe[2];
    int stdoutPipe[2];

    if (pipe(stdinPipe) < 0 || pipe(stdoutPipe) < 0) {
        err << programName_ << ": pipe creation failed\n";
        return 1;
    }

    pid_t pid = fork();

    if (pid < 0) {
        err << programName_ << ": fork failed\n";
        return 1;
    }

    if (pid == 0) {
        // Дочерний процесс
        
        // Перенаправляем stdin
        close(stdinPipe[1]);
        dup2(stdinPipe[0], STDIN_FILENO);
        close(stdinPipe[0]);

        // Перенаправляем stdout
        close(stdoutPipe[0]);
        dup2(stdoutPipe[1], STDOUT_FILENO);
        close(stdoutPipe[1]);

        // Формируем массив аргументов
        std::vector<char*> argv;
        argv.push_back(const_cast<char*>(execPath->c_str()));
        for (const auto& arg : args_) {
            argv.push_back(const_cast<char*>(arg.c_str()));
        }
        argv.push_back(nullptr);

        // Формируем окружение
        std::vector<std::string> envStrings = env_.toEnvp();
        std::vector<char*> envp;
        for (auto& s : envStrings) {
            envp.push_back(const_cast<char*>(s.c_str()));
        }
        envp.push_back(nullptr);

        // Запускаем программу
        execve(execPath->c_str(), argv.data(), envp.data());

        // Если execve вернулся — ошибка
        _exit(127);
    }

    // Родительский процесс
    
    // Закрываем ненужные концы каналов
    close(stdinPipe[0]);
    close(stdoutPipe[1]);

    // Отправляем входные данные
    std::stringstream inputBuffer;
    inputBuffer << in.rdbuf();
    std::string inputData = inputBuffer.str();
    
    if (!inputData.empty()) {
        ssize_t written = write(stdinPipe[1], inputData.c_str(), inputData.size());
        (void)written; // Игнорируем возможные ошибки записи
    }
    close(stdinPipe[1]);

    // Читаем вывод
    char buffer[4096];
    ssize_t bytesRead;
    while ((bytesRead = read(stdoutPipe[0], buffer, sizeof(buffer))) > 0) {
        out.write(buffer, bytesRead);
    }
    close(stdoutPipe[0]);

    // Ожидаем завершения дочернего процесса
    int status;
    waitpid(pid, &status, 0);

    if (WIFEXITED(status)) {
        return WEXITSTATUS(status);
    } else if (WIFSIGNALED(status)) {
        return 128 + WTERMSIG(status);
    }

    return 1;
}

void ExternalCommand::setArguments(const std::vector<std::string>& args) {
    args_ = args;
}

std::optional<std::string> ExternalCommand::findExecutable() const {
    // Если путь содержит / — используем как есть
    if (programName_.find('/') != std::string::npos) {
        if (access(programName_.c_str(), X_OK) == 0) {
            return programName_;
        }
        return std::nullopt;
    }

    // Ищем в PATH
    std::string pathVar = env_.get("PATH");
    if (pathVar.empty()) {
        // Стандартные пути
        pathVar = "/usr/local/bin:/usr/bin:/bin";
    }

    std::stringstream ss(pathVar);
    std::string dir;

    while (std::getline(ss, dir, ':')) {
        std::string fullPath = dir + "/" + programName_;
        if (access(fullPath.c_str(), X_OK) == 0) {
            return fullPath;
        }
    }

    return std::nullopt;
}

} // namespace shell
