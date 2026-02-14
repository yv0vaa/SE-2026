#include <gtest/gtest.h>

#include "shell/lexer.hpp"
#include "shell/parser.hpp"

using namespace shell;

/**
 * Юнит-тесты для Parser.
 * Проверяют: разбор токенов в AST — пустая строка, простая команда, пайплайн, присваивание.
 * Вход: vector<Token>. Выход: unique_ptr<ParsedCommand>.
 */
class ParserTest : public ::testing::Test {};

// Проверяет: пустой ввод → ParsedEmpty. Вход: токены от "". Выход: result->isEmpty().
TEST_F(ParserTest, EmptyInput) {
    Lexer lexer("");
    auto tokens = lexer.tokenize();
    Parser parser(std::move(tokens));

    auto result = parser.parse();
    ASSERT_NE(result, nullptr);
    EXPECT_TRUE(result->isEmpty());
}

TEST_F(ParserTest, SimpleCommand) {
    Lexer lexer("echo hello");
    auto tokens = lexer.tokenize();
    Parser parser(std::move(tokens));

    auto result = parser.parse();
    ASSERT_NE(result, nullptr);
    EXPECT_TRUE(result->isPipeline());

    auto* pipeline = dynamic_cast<ParsedPipeline*>(result.get());
    ASSERT_NE(pipeline, nullptr);
    ASSERT_EQ(pipeline->commands.size(), 1);
    EXPECT_EQ(pipeline->commands[0].commandName, "echo");
    ASSERT_EQ(pipeline->commands[0].arguments.size(), 1);
    EXPECT_EQ(pipeline->commands[0].arguments[0], "hello");
}

TEST_F(ParserTest, CommandWithMultipleArgs) {
    Lexer lexer("echo hello world");
    auto tokens = lexer.tokenize();
    Parser parser(std::move(tokens));

    auto result = parser.parse();
    auto* pipeline = dynamic_cast<ParsedPipeline*>(result.get());

    ASSERT_NE(pipeline, nullptr);
    ASSERT_EQ(pipeline->commands.size(), 1);
    EXPECT_EQ(pipeline->commands[0].commandName, "echo");
    ASSERT_EQ(pipeline->commands[0].arguments.size(), 2);
    EXPECT_EQ(pipeline->commands[0].arguments[0], "hello");
    EXPECT_EQ(pipeline->commands[0].arguments[1], "world");
}

TEST_F(ParserTest, PipelineOfTwo) {
    Lexer lexer("cat file.txt | wc");
    auto tokens = lexer.tokenize();
    Parser parser(std::move(tokens));

    auto result = parser.parse();
    auto* pipeline = dynamic_cast<ParsedPipeline*>(result.get());

    ASSERT_NE(pipeline, nullptr);
    ASSERT_EQ(pipeline->commands.size(), 2);

    EXPECT_EQ(pipeline->commands[0].commandName, "cat");
    ASSERT_EQ(pipeline->commands[0].arguments.size(), 1);
    EXPECT_EQ(pipeline->commands[0].arguments[0], "file.txt");

    EXPECT_EQ(pipeline->commands[1].commandName, "wc");
    EXPECT_TRUE(pipeline->commands[1].arguments.empty());
}

TEST_F(ParserTest, PipelineOfThree) {
    Lexer lexer("cat file | grep pattern | wc -l");
    auto tokens = lexer.tokenize();
    Parser parser(std::move(tokens));

    auto result = parser.parse();
    auto* pipeline = dynamic_cast<ParsedPipeline*>(result.get());

    ASSERT_NE(pipeline, nullptr);
    ASSERT_EQ(pipeline->commands.size(), 3);

    EXPECT_EQ(pipeline->commands[0].commandName, "cat");
    EXPECT_EQ(pipeline->commands[1].commandName, "grep");
    EXPECT_EQ(pipeline->commands[2].commandName, "wc");
}

TEST_F(ParserTest, SimpleAssignment) {
    Lexer lexer("FOO=bar");
    auto tokens = lexer.tokenize();
    Parser parser(std::move(tokens));

    auto result = parser.parse();
    ASSERT_NE(result, nullptr);
    EXPECT_TRUE(result->isAssignment());

    auto* assignment = dynamic_cast<ParsedAssignment*>(result.get());
    ASSERT_NE(assignment, nullptr);
    EXPECT_EQ(assignment->variableName, "FOO");
    EXPECT_EQ(assignment->value, "bar");
}

TEST_F(ParserTest, MultipleAssignments) {
    Lexer lexer("x=1 y=2");
    auto tokens = lexer.tokenize();
    Parser parser(std::move(tokens));

    auto result = parser.parse();
    ASSERT_NE(result, nullptr);
    EXPECT_TRUE(result->isAssignment());

    auto* list = dynamic_cast<ParsedAssignmentList*>(result.get());
    ASSERT_NE(list, nullptr);
    ASSERT_EQ(list->assignments.size(), 2);
    EXPECT_EQ(list->assignments[0].variableName, "x");
    EXPECT_EQ(list->assignments[0].value, "1");
    EXPECT_EQ(list->assignments[1].variableName, "y");
    EXPECT_EQ(list->assignments[1].value, "2");
}

TEST_F(ParserTest, AssignmentWithEmptyValue) {
    Lexer lexer("FOO=");
    auto tokens = lexer.tokenize();
    Parser parser(std::move(tokens));

    auto result = parser.parse();
    auto* assignment = dynamic_cast<ParsedAssignment*>(result.get());

    ASSERT_NE(assignment, nullptr);
    EXPECT_EQ(assignment->variableName, "FOO");
    EXPECT_EQ(assignment->value, "");
}

TEST_F(ParserTest, CommandNoArgs) {
    Lexer lexer("pwd");
    auto tokens = lexer.tokenize();
    Parser parser(std::move(tokens));

    auto result = parser.parse();
    auto* pipeline = dynamic_cast<ParsedPipeline*>(result.get());

    ASSERT_NE(pipeline, nullptr);
    ASSERT_EQ(pipeline->commands.size(), 1);
    EXPECT_EQ(pipeline->commands[0].commandName, "pwd");
    EXPECT_TRUE(pipeline->commands[0].arguments.empty());
}
