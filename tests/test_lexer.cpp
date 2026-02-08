#include <gtest/gtest.h>

#include "shell/lexer.hpp"

using namespace shell;

class LexerTest : public ::testing::Test {};

TEST_F(LexerTest, EmptyInput) {
    Lexer lexer("");
    auto tokens = lexer.tokenize();

    ASSERT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].type, TokenType::END_OF_INPUT);
}

TEST_F(LexerTest, SingleWord) {
    Lexer lexer("echo");
    auto tokens = lexer.tokenize();

    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].type, TokenType::WORD);
    EXPECT_EQ(tokens[0].value, "echo");
    EXPECT_EQ(tokens[1].type, TokenType::END_OF_INPUT);
}

TEST_F(LexerTest, MultipleWords) {
    Lexer lexer("echo hello world");
    auto tokens = lexer.tokenize();

    ASSERT_EQ(tokens.size(), 4);
    EXPECT_EQ(tokens[0].value, "echo");
    EXPECT_EQ(tokens[1].value, "hello");
    EXPECT_EQ(tokens[2].value, "world");
    EXPECT_EQ(tokens[3].type, TokenType::END_OF_INPUT);
}

TEST_F(LexerTest, Pipe) {
    Lexer lexer("cat file.txt | wc");
    auto tokens = lexer.tokenize();

    ASSERT_EQ(tokens.size(), 5);
    EXPECT_EQ(tokens[0].value, "cat");
    EXPECT_EQ(tokens[1].value, "file.txt");
    EXPECT_EQ(tokens[2].type, TokenType::PIPE);
    EXPECT_EQ(tokens[3].value, "wc");
    EXPECT_EQ(tokens[4].type, TokenType::END_OF_INPUT);
}

TEST_F(LexerTest, DoubleQuotes) {
    Lexer lexer("echo \"hello world\"");
    auto tokens = lexer.tokenize();

    ASSERT_EQ(tokens.size(), 3);
    EXPECT_EQ(tokens[0].value, "echo");
    EXPECT_EQ(tokens[1].value, "hello world");
    EXPECT_EQ(tokens[2].type, TokenType::END_OF_INPUT);
}

TEST_F(LexerTest, SingleQuotes) {
    Lexer lexer("echo 'hello world'");
    auto tokens = lexer.tokenize();

    ASSERT_EQ(tokens.size(), 3);
    EXPECT_EQ(tokens[0].value, "echo");
    EXPECT_EQ(tokens[1].value, "hello world");
    EXPECT_EQ(tokens[2].type, TokenType::END_OF_INPUT);
}

TEST_F(LexerTest, Assignment) {
    Lexer lexer("FOO=bar");
    auto tokens = lexer.tokenize();

    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].type, TokenType::WORD);
    EXPECT_EQ(tokens[0].value, "FOO=bar");
    EXPECT_EQ(tokens[1].type, TokenType::END_OF_INPUT);
}

TEST_F(LexerTest, MultiplePipes) {
    Lexer lexer("cat file | grep pattern | wc -l");
    auto tokens = lexer.tokenize();

    ASSERT_EQ(tokens.size(), 9);  // 8 tokens + END_OF_INPUT
    EXPECT_EQ(tokens[0].value, "cat");
    EXPECT_EQ(tokens[1].value, "file");
    EXPECT_EQ(tokens[2].type, TokenType::PIPE);
    EXPECT_EQ(tokens[3].value, "grep");
    EXPECT_EQ(tokens[4].value, "pattern");
    EXPECT_EQ(tokens[5].type, TokenType::PIPE);
    EXPECT_EQ(tokens[6].value, "wc");
    EXPECT_EQ(tokens[7].value, "-l");
    EXPECT_EQ(tokens[8].type, TokenType::END_OF_INPUT);
}

TEST_F(LexerTest, QuotesInsideWord) {
    Lexer lexer("echo hello\"world\"");
    auto tokens = lexer.tokenize();

    ASSERT_EQ(tokens.size(), 3);
    EXPECT_EQ(tokens[0].value, "echo");
    EXPECT_EQ(tokens[1].value, "helloworld");
}

TEST_F(LexerTest, EscapeInDoubleQuotes) {
    Lexer lexer("echo \"hello\\\"world\"");
    auto tokens = lexer.tokenize();

    ASSERT_EQ(tokens.size(), 3);
    EXPECT_EQ(tokens[0].value, "echo");
    EXPECT_EQ(tokens[1].value, "hello\"world");
}

TEST_F(LexerTest, WhitespaceOnly) {
    Lexer lexer("   \t  ");
    auto tokens = lexer.tokenize();

    ASSERT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].type, TokenType::END_OF_INPUT);
}
