#include <fstream>
#include <sstream>

#include <gtest/gtest.h>

#include "shell/commands/cat_command.hpp"
#include "shell/commands/echo_command.hpp"
#include "shell/commands/exit_command.hpp"
#include "shell/commands/pwd_command.hpp"
#include "shell/commands/wc_command.hpp"
#include "shell/environment.hpp"

using namespace shell;

class CommandsTest : public ::testing::Test {
protected:
    std::istringstream emptyInput;
    std::ostringstream output;
    std::ostringstream errors;

    void SetUp() override {
        output.str("");
        errors.str("");
    }
};

// ============== getName (все команды) ==============
// Проверяют: getName() возвращает ожидаемое имя. Вход: созданная команда. Выход: строка имени.

TEST_F(CommandsTest, EchoGetName) {
    EchoCommand cmd;
    EXPECT_EQ(cmd.getName(), "echo");
}

TEST_F(CommandsTest, CatGetName) {
    CatCommand cmd;
    EXPECT_EQ(cmd.getName(), "cat");
}

TEST_F(CommandsTest, WcGetName) {
    WcCommand cmd;
    EXPECT_EQ(cmd.getName(), "wc");
}

TEST_F(CommandsTest, PwdGetName) {
    PwdCommand cmd;
    EXPECT_EQ(cmd.getName(), "pwd");
}

TEST_F(CommandsTest, ExitGetName) {
    ExitCommand cmd;
    EXPECT_EQ(cmd.getName(), "exit");
}

// ============== Echo Tests ==============
// Вход: аргументы через setArguments, пустой input. Выход: код 0, stdout — аргументы через пробел + \n.

TEST_F(CommandsTest, EchoNoArgs) {
    EchoCommand cmd;
    cmd.setArguments({});

    int result = cmd.execute(emptyInput, output, errors);

    EXPECT_EQ(result, 0);
    EXPECT_EQ(output.str(), "\n");
}

TEST_F(CommandsTest, EchoSingleArg) {
    EchoCommand cmd;
    cmd.setArguments({"hello"});

    int result = cmd.execute(emptyInput, output, errors);

    EXPECT_EQ(result, 0);
    EXPECT_EQ(output.str(), "hello\n");
}

TEST_F(CommandsTest, EchoMultipleArgs) {
    EchoCommand cmd;
    cmd.setArguments({"hello", "world"});

    int result = cmd.execute(emptyInput, output, errors);

    EXPECT_EQ(result, 0);
    EXPECT_EQ(output.str(), "hello world\n");
}

// ============== Cat Tests ==============

TEST_F(CommandsTest, CatFromStdin) {
    CatCommand cmd;
    cmd.setArguments({});

    std::istringstream input("hello from stdin");
    int result = cmd.execute(input, output, errors);

    EXPECT_EQ(result, 0);
    EXPECT_EQ(output.str(), "hello from stdin");
}

TEST_F(CommandsTest, CatFileNotFound) {
    CatCommand cmd;
    cmd.setArguments({"nonexistent_file_12345.txt"});

    int result = cmd.execute(emptyInput, output, errors);

    EXPECT_EQ(result, 1);
    EXPECT_TRUE(errors.str().find("No such file") != std::string::npos);
}

TEST_F(CommandsTest, CatExistingFile) {
    // Проверяет: cat с путём к существующему файлу выводит его содержимое. Вход: args=[filename], in=игнор. Выход: 0, out=содержимое.
    const std::string filename = "/tmp/test_cat_file.txt";
    {
        std::ofstream f(filename);
        f << "test content";
    }

    CatCommand cmd;
    cmd.setArguments({filename});

    int result = cmd.execute(emptyInput, output, errors);

    EXPECT_EQ(result, 0);
    EXPECT_EQ(output.str(), "test content");

    std::remove(filename.c_str());
}

// Проверяет: cat .gitignore выводит содержимое файла (критерий ДЗ). Вход: args=["../.gitignore"] если файл есть. Выход: 0, out=содержимое.
TEST_F(CommandsTest, CatGitignore) {
    const std::string path = "../.gitignore";
    std::ifstream check(path);
    if (!check.good()) {
        GTEST_SKIP() << ".gitignore not found (run from build/ with repo root as parent)";
    }
    CatCommand cmd;
    cmd.setArguments({path});
    int result = cmd.execute(emptyInput, output, errors);
    EXPECT_EQ(result, 0);
    EXPECT_FALSE(output.str().empty());
}

// ============== Wc Tests ==============

TEST_F(CommandsTest, WcFromStdin) {
    WcCommand cmd;
    cmd.setArguments({});

    std::istringstream input("hello world\n");
    int result = cmd.execute(input, output, errors);

    EXPECT_EQ(result, 0);
    // 1 строка, 2 слова, 12 байт
    EXPECT_EQ(output.str(), "1 2 12\n");
}

TEST_F(CommandsTest, WcMultipleLines) {
    WcCommand cmd;
    cmd.setArguments({});

    std::istringstream input("line one\nline two\nline three\n");
    int result = cmd.execute(input, output, errors);

    EXPECT_EQ(result, 0);
    // 3 строки, 6 слов (getline поведение может отличаться)
    std::string out = output.str();
    EXPECT_TRUE(out.find("3 6") != std::string::npos);
}

TEST_F(CommandsTest, WcEmpty) {
    WcCommand cmd;
    cmd.setArguments({});

    std::istringstream input("");
    int result = cmd.execute(input, output, errors);

    EXPECT_EQ(result, 0);
    EXPECT_EQ(output.str(), "0 0 0\n");
}

// Проверяет: wc с аргументом-файлом возвращает строки/слова/байты файла. Вход: args=[path]. Выход: 0, строка "L W B\n".
TEST_F(CommandsTest, WcExistingFile) {
    const std::string filename = "/tmp/test_wc_file.txt";
    {
        std::ofstream f(filename);
        f << "one two\nthree\n";
    }
    WcCommand cmd;
    cmd.setArguments({filename});
    int result = cmd.execute(emptyInput, output, errors);
    EXPECT_EQ(result, 0);
    std::string out = output.str();
    EXPECT_TRUE(out.find("2") != std::string::npos);  // 2 lines
    EXPECT_TRUE(out.find("3") != std::string::npos);  // 3 words
    std::remove(filename.c_str());
}

// Проверяет: wc .gitignore (критерий ДЗ). Вход: args=["../.gitignore"] если файл есть. Выход: 0, строка с числами.
TEST_F(CommandsTest, WcGitignore) {
    const std::string path = "../.gitignore";
    std::ifstream check(path);
    if (!check.good()) {
        GTEST_SKIP() << ".gitignore not found (run from build/)";
    }
    WcCommand cmd;
    cmd.setArguments({path});
    int result = cmd.execute(emptyInput, output, errors);
    EXPECT_EQ(result, 0);
    EXPECT_FALSE(output.str().empty());
}

// ============== Pwd Tests ==============

TEST_F(CommandsTest, PwdReturnsDirectory) {
    PwdCommand cmd;
    cmd.setArguments({});

    int result = cmd.execute(emptyInput, output, errors);

    EXPECT_EQ(result, 0);
    // Должен вернуть какой-то путь
    EXPECT_FALSE(output.str().empty());
    EXPECT_TRUE(output.str().find('/') != std::string::npos);
}

// ============== Exit Tests ==============

TEST_F(CommandsTest, ExitDefault) {
    ExitCommand cmd;
    cmd.setArguments({});

    int result = cmd.execute(emptyInput, output, errors);

    EXPECT_EQ(result, 0);
    EXPECT_TRUE(cmd.wasExitRequested());
    EXPECT_EQ(cmd.getExitCode(), 0);
}

TEST_F(CommandsTest, ExitWithCode) {
    ExitCommand cmd;
    cmd.setArguments({"42"});

    int result = cmd.execute(emptyInput, output, errors);

    EXPECT_EQ(result, 42);
    EXPECT_TRUE(cmd.wasExitRequested());
    EXPECT_EQ(cmd.getExitCode(), 42);
}
