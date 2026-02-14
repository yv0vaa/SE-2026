#include <sstream>

#include <gtest/gtest.h>

#include "shell/command_factory.hpp"
#include "shell/environment.hpp"
#include "shell/executor.hpp"
#include "shell/parsed_command.hpp"
#include "shell/pipeline.hpp"

using namespace shell;

/**
 * Юнит-тесты для Executor.
 * Проверяют: execute (пустой пайплайн, одна команда, пайплайн), executeAssignment,
 * executeAssignments, shouldExit, getExitCode, обновление переменной "?".
 */

class ExecutorTest : public ::testing::Test {
protected:
    Environment env;
    std::ostringstream capturedOut;
    std::ostringstream capturedErr;
    std::streambuf* oldCout = nullptr;
    std::streambuf* oldCerr = nullptr;

    void SetUp() override {
        env.initFromSystem();
        capturedOut.str("");
        capturedErr.str("");
        oldCout = std::cout.rdbuf(capturedOut.rdbuf());
        oldCerr = std::cerr.rdbuf(capturedErr.rdbuf());
    }

    void TearDown() override {
        if (oldCout)
            std::cout.rdbuf(oldCout);
        if (oldCerr)
            std::cerr.rdbuf(oldCerr);
    }
};

// Проверяет: выполнение пустого пайплайна возвращает 0.
// Вход: Pipeline(), isEmpty()==true. Выход: execute() == 0.
TEST_F(ExecutorTest, ExecuteEmptyPipelineReturnsZero) {
    Executor executor(env);
    Pipeline pipeline;
    EXPECT_TRUE(pipeline.isEmpty());
    int code = executor.execute(pipeline);
    EXPECT_EQ(code, 0);
}

// Проверяет: executeAssignment записывает переменную в Environment.
// Вход: ParsedAssignment("VAR", "value"). Выход: env.get("VAR") == "value", код 0.
TEST_F(ExecutorTest, ExecuteAssignmentSetsVariable) {
    Executor executor(env);
    ParsedAssignment a("VAR", "value");
    int code = executor.executeAssignment(a);
    EXPECT_EQ(code, 0);
    EXPECT_EQ(env.get("VAR"), "value");
}

// Проверяет: executeAssignments записывает все присваивания.
// Вход: ParsedAssignmentList с x=1, y=2. Выход: env.get("x")=="1", env.get("y")=="2".
TEST_F(ExecutorTest, ExecuteAssignmentsSetsAll) {
    Executor executor(env);
    ParsedAssignmentList list;
    list.addAssignment(ParsedAssignment("x", "1"));
    list.addAssignment(ParsedAssignment("y", "2"));
    int code = executor.executeAssignments(list);
    EXPECT_EQ(code, 0);
    EXPECT_EQ(env.get("x"), "1");
    EXPECT_EQ(env.get("y"), "2");
}

// Проверяет: после выполнения команды env["?"] содержит код возврата.
// Вход: пайплайн с echo (успех). Выход: env.get("?") == "0".
TEST_F(ExecutorTest, ExecuteSetsQuestionMarkVariable) {
    CommandFactory factory(env);
    Executor executor(env);
    Pipeline pipeline;
    auto cmd = factory.create("echo");
    cmd->setArguments({"ok"});
    pipeline.addCommand(std::move(cmd));

    executor.execute(pipeline);
    EXPECT_EQ(env.get("?"), "0");
}

// Проверяет: shouldExit() и getExitCode() после команды exit.
// Вход: пайплайн с exit 42. Выход: shouldExit()==true, getExitCode()==42.
TEST_F(ExecutorTest, ExitCommandSetsFlagAndCode) {
    CommandFactory factory(env);
    Executor executor(env);
    Pipeline pipeline;
    auto cmd = factory.create("exit");
    cmd->setArguments({"42"});
    pipeline.addCommand(std::move(cmd));

    executor.execute(pipeline);
    EXPECT_TRUE(executor.shouldExit());
    EXPECT_EQ(executor.getExitCode(), 42);
}

// Проверяет: exit без аргумента — код 0.
// Вход: exit без аргументов. Выход: getExitCode() == 0.
TEST_F(ExecutorTest, ExitWithoutArgCodeZero) {
    CommandFactory factory(env);
    Executor executor(env);
    Pipeline pipeline;
    auto cmd = factory.create("exit");
    cmd->setArguments({});
    pipeline.addCommand(std::move(cmd));

    executor.execute(pipeline);
    EXPECT_TRUE(executor.shouldExit());
    EXPECT_EQ(executor.getExitCode(), 0);
}

// Проверяет: выполнение одной команды echo выводит в stdout и возвращает 0.
// Вход: Pipeline с EchoCommand("hello"). Выход: код 0, capturedOut == "hello\n".
TEST_F(ExecutorTest, ExecuteSingleEcho) {
    CommandFactory factory(env);
    Executor executor(env);
    Pipeline pipeline;
    auto cmd = factory.create("echo");
    cmd->setArguments({"hello"});
    pipeline.addCommand(std::move(cmd));

    int code = executor.execute(pipeline);
    EXPECT_EQ(code, 0);
    EXPECT_EQ(capturedOut.str(), "hello\n");
}

// Проверяет: пайплайн echo | wc — код возврата 0, вывод "1 2 12\n" (1 строка, 2 слова, 12 байт).
// Вход: echo "hello world" | wc. Выход: код 0, строка вида "1 2 12\n".
TEST_F(ExecutorTest, ExecutePipelineEchoWc) {
    CommandFactory factory(env);
    Executor executor(env);
    Pipeline pipeline;
    auto echoCmd = factory.create("echo");
    echoCmd->setArguments({"hello", "world"});
    pipeline.addCommand(std::move(echoCmd));
    auto wcCmd = factory.create("wc");
    wcCmd->setArguments({});
    pipeline.addCommand(std::move(wcCmd));

    int code = executor.execute(pipeline);
    EXPECT_EQ(code, 0);
    EXPECT_EQ(capturedOut.str(), "1 2 12\n");
}

// Проверяет: до выполнения exit shouldExit() == false.
// Вход: Executor без вызова execute с exit. Выход: shouldExit() == false.
TEST_F(ExecutorTest, ShouldExitFalseInitially) {
    Executor executor(env);
    EXPECT_FALSE(executor.shouldExit());
}
