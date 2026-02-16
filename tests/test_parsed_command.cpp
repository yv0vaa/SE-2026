#include <gtest/gtest.h>

#include "shell/parsed_command.hpp"

using namespace shell;

/**
 * Юнит-тесты для типов AST: ParsedEmpty, ParsedAssignment, ParsedSimpleCommand,
 * ParsedPipeline, ParsedAssignmentList.
 * Проверяют: конструкторы, isAssignment/isPipeline/isEmpty, addCommand/addAssignment.
 */

class ParsedCommandTest : public ::testing::Test {};

// --- ParsedEmpty ---

// Проверяет: ParsedEmpty::isEmpty() возвращает true.
// Вход: —. Выход: isEmpty() == true, isAssignment() == false, isPipeline() == false.
TEST_F(ParsedCommandTest, ParsedEmptyIsEmpty) {
    ParsedEmpty empty;
    EXPECT_TRUE(empty.isEmpty());
    EXPECT_FALSE(empty.isAssignment());
    EXPECT_FALSE(empty.isPipeline());
}

// --- ParsedAssignment ---

// Проверяет: конструктор ParsedAssignment сохраняет имя и значение.
// Вход: "FOO", "bar". Выход: variableName=="FOO", value=="bar", isAssignment()==true.
TEST_F(ParsedCommandTest, ParsedAssignmentConstructor) {
    ParsedAssignment a("FOO", "bar");
    EXPECT_EQ(a.variableName, "FOO");
    EXPECT_EQ(a.value, "bar");
    EXPECT_TRUE(a.isAssignment());
    EXPECT_FALSE(a.isPipeline());
    EXPECT_FALSE(a.isEmpty());
}

// Проверяет: пустое значение допустимо.
// Вход: "X", "". Выход: value == "".
TEST_F(ParsedCommandTest, ParsedAssignmentEmptyValue) {
    ParsedAssignment a("X", "");
    EXPECT_EQ(a.variableName, "X");
    EXPECT_EQ(a.value, "");
}

// --- ParsedSimpleCommand ---

// Проверяет: конструктор по умолчанию и с параметрами.
// Вход: ParsedSimpleCommand("echo", {"a","b"}). Выход: commandName=="echo", arguments==["a","b"].
TEST_F(ParsedCommandTest, ParsedSimpleCommandConstructor) {
    ParsedSimpleCommand cmd("echo", {"a", "b"});
    EXPECT_EQ(cmd.commandName, "echo");
    ASSERT_EQ(cmd.arguments.size(), 2u);
    EXPECT_EQ(cmd.arguments[0], "a");
    EXPECT_EQ(cmd.arguments[1], "b");
}

// Проверяет: конструктор по умолчанию — пустое имя и аргументы.
// Вход: ParsedSimpleCommand(). Выход: commandName=="", arguments.empty().
TEST_F(ParsedCommandTest, ParsedSimpleCommandDefault) {
    ParsedSimpleCommand cmd;
    EXPECT_EQ(cmd.commandName, "");
    EXPECT_TRUE(cmd.arguments.empty());
}

// --- ParsedPipeline ---

// Проверяет: addCommand и isPipeline.
// Вход: addCommand(ParsedSimpleCommand("echo", {"x"})). Выход: commands.size()==1,
// commandName=="echo".
TEST_F(ParsedCommandTest, ParsedPipelineAddCommand) {
    ParsedPipeline pipeline;
    EXPECT_TRUE(pipeline.commands.empty());
    EXPECT_TRUE(pipeline.isPipeline());
    EXPECT_FALSE(pipeline.isAssignment());

    pipeline.addCommand(ParsedSimpleCommand("echo", {"x"}));
    ASSERT_EQ(pipeline.commands.size(), 1u);
    EXPECT_EQ(pipeline.commands[0].commandName, "echo");
    ASSERT_EQ(pipeline.commands[0].arguments.size(), 1u);
    EXPECT_EQ(pipeline.commands[0].arguments[0], "x");
}

// Проверяет: несколько addCommand.
// Вход: addCommand(echo), addCommand(wc). Выход: commands.size()==2.
TEST_F(ParsedCommandTest, ParsedPipelineMultipleCommands) {
    ParsedPipeline pipeline;
    pipeline.addCommand(ParsedSimpleCommand("cat", {"file.txt"}));
    pipeline.addCommand(ParsedSimpleCommand("wc", {}));
    ASSERT_EQ(pipeline.commands.size(), 2u);
    EXPECT_EQ(pipeline.commands[0].commandName, "cat");
    EXPECT_EQ(pipeline.commands[1].commandName, "wc");
}

// --- ParsedAssignmentList ---

// Проверяет: addAssignment и isAssignment.
// Вход: addAssignment(ParsedAssignment("x","1")). Выход: assignments.size()==1, variableName=="x".
TEST_F(ParsedCommandTest, ParsedAssignmentListAddAssignment) {
    ParsedAssignmentList list;
    EXPECT_TRUE(list.isAssignment());
    EXPECT_FALSE(list.isPipeline());

    list.addAssignment(ParsedAssignment("x", "1"));
    ASSERT_EQ(list.assignments.size(), 1u);
    EXPECT_EQ(list.assignments[0].variableName, "x");
    EXPECT_EQ(list.assignments[0].value, "1");
}

// Проверяет: несколько addAssignment.
// Вход: addAssignment(x=1), addAssignment(y=2). Выход: assignments.size()==2.
TEST_F(ParsedCommandTest, ParsedAssignmentListMultiple) {
    ParsedAssignmentList list;
    list.addAssignment(ParsedAssignment("x", "1"));
    list.addAssignment(ParsedAssignment("y", "2"));
    ASSERT_EQ(list.assignments.size(), 2u);
    EXPECT_EQ(list.assignments[0].variableName, "x");
    EXPECT_EQ(list.assignments[1].variableName, "y");
}
