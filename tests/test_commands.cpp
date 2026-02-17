#include <algorithm>
#include <fstream>
#include <sstream>

#include <gtest/gtest.h>

#include "shell/commands/cat_command.hpp"
#include "shell/commands/echo_command.hpp"
#include "shell/commands/exit_command.hpp"
#include "shell/commands/grep_command.hpp"
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
// Вход: аргументы через setArguments, пустой input. Выход: код 0, stdout — аргументы через пробел +
// \n.

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
    // Проверяет: cat с путём к существующему файлу выводит его содержимое. Вход: args=[filename],
    // in=игнор. Выход: 0, out=содержимое.
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

// Проверяет: cat .gitignore выводит содержимое файла (критерий ДЗ). Вход: args=["../.gitignore"]
// если файл есть. Выход: 0, out=содержимое.
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

// Проверяет: wc с аргументом-файлом возвращает строки/слова/байты файла. Вход: args=[path]. Выход:
// 0, строка "L W B\n".
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

// Проверяет: wc .gitignore (критерий ДЗ). Вход: args=["../.gitignore"] если файл есть. Выход: 0,
// строка с числами.
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

// ============== Grep Tests ==============

TEST_F(CommandsTest, GrepGetName) {
    GrepCommand cmd;
    EXPECT_EQ(cmd.getName(), "grep");
}

TEST_F(CommandsTest, GrepBasicMatch) {
    GrepCommand cmd;
    cmd.setArguments({"foo"});

    std::istringstream input("line one\nfoo bar\nbaz\n");
    int result = cmd.execute(input, output, errors);

    EXPECT_EQ(result, 0);
    EXPECT_EQ(output.str(), "foo bar\n");
}

TEST_F(CommandsTest, GrepNoMatch) {
    GrepCommand cmd;
    cmd.setArguments({"xyz"});

    std::istringstream input("line one\nfoo bar\n");
    int result = cmd.execute(input, output, errors);

    EXPECT_EQ(result, 1);
    EXPECT_TRUE(output.str().empty());
}

TEST_F(CommandsTest, GrepCaseInsensitive) {
    GrepCommand cmd;
    cmd.setArguments({"-i", "minimal"});

    std::istringstream input("MINIMAL\nminimal\nMinimal\n");
    int result = cmd.execute(input, output, errors);

    EXPECT_EQ(result, 0);
    std::string out = output.str();
    EXPECT_TRUE(out.find("MINIMAL") != std::string::npos);
    EXPECT_TRUE(out.find("minimal") != std::string::npos);
    EXPECT_TRUE(out.find("Minimal") != std::string::npos);
}

TEST_F(CommandsTest, GrepWordBoundaryMatch) {
    GrepCommand cmd;
    cmd.setArguments({"-w", "foo"});

    std::istringstream input("foo bar\nfoobar\nfoo\n");
    int result = cmd.execute(input, output, errors);

    EXPECT_EQ(result, 0);
    EXPECT_TRUE(output.str().find("foo bar") != std::string::npos);
    EXPECT_TRUE(output.str().find("foo\n") != std::string::npos);
    EXPECT_TRUE(output.str().find("foobar") == std::string::npos);
}

TEST_F(CommandsTest, GrepWordBoundaryNoMatch) {
    GrepCommand cmd;
    cmd.setArguments({"-w", "foo"});

    std::istringstream input("foobar\n");
    int result = cmd.execute(input, output, errors);

    EXPECT_EQ(result, 1);
    EXPECT_TRUE(output.str().empty());
}

TEST_F(CommandsTest, GrepAfterContext) {
    GrepCommand cmd;
    cmd.setArguments({"-A", "1", "II"});

    std::istringstream input("first\nII\nnext\nlast\n");
    int result = cmd.execute(input, output, errors);

    EXPECT_EQ(result, 0);
    EXPECT_EQ(output.str(), "II\nnext\n");
}

TEST_F(CommandsTest, GrepAfterContextOverlapping) {
    GrepCommand cmd;
    cmd.setArguments({"-A", "2", "x"});

    std::istringstream input("a\nx\nb\nx\nc\n");
    int result = cmd.execute(input, output, errors);

    EXPECT_EQ(result, 0);
    std::string out = output.str();
    EXPECT_TRUE(out.find("x") != std::string::npos);
    EXPECT_TRUE(out.find("b") != std::string::npos);
    EXPECT_TRUE(out.find("c") != std::string::npos);
    EXPECT_EQ(std::count(out.begin(), out.end(), '\n'), 4u);
}

TEST_F(CommandsTest, GrepFromFile) {
    const std::string filename = "/tmp/test_grep_file.txt";
    {
        std::ofstream f(filename);
        f << "alpha\nМинимальный синтаксис grep\nbeta\n";
    }

    GrepCommand cmd;
    cmd.setArguments({"Минимальный", filename});

    int result = cmd.execute(emptyInput, output, errors);

    EXPECT_EQ(result, 0);
    EXPECT_TRUE(output.str().find("Минимальный синтаксис grep") != std::string::npos);

    std::remove(filename.c_str());
}

TEST_F(CommandsTest, GrepMissingPatternReturnsError) {
    GrepCommand cmd;
    cmd.setArguments({"-i"});

    std::istringstream input("any\n");
    int result = cmd.execute(input, output, errors);

    EXPECT_EQ(result, 2);
    EXPECT_FALSE(errors.str().empty());
}
