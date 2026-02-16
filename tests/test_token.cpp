#include <gtest/gtest.h>

#include "shell/token.hpp"

using namespace shell;

/**
 * Юнит-тесты для Token и tokenTypeToString.
 * Проверяют: создание токена, равенство/неравенство, строковое представление типа.
 */

class TokenTest : public ::testing::Test {};

// Проверяет: конструктор Token сохраняет type и value.
// Вход: TokenType::WORD, value "echo". Выход: type == WORD, value == "echo".
TEST_F(TokenTest, ConstructorStoresTypeAndValue) {
    Token t(TokenType::WORD, "echo");
    EXPECT_EQ(t.type, TokenType::WORD);
    EXPECT_EQ(t.value, "echo");
}

// Проверяет: конструктор с одним аргументом (value по умолчанию "").
// Вход: TokenType::PIPE. Выход: value == "".
TEST_F(TokenTest, ConstructorDefaultValue) {
    Token t(TokenType::PIPE);
    EXPECT_EQ(t.type, TokenType::PIPE);
    EXPECT_EQ(t.value, "");
}

// Проверяет: operator== возвращает true при совпадении type и value.
// Вход: два токена (WORD, "echo"). Выход: true.
TEST_F(TokenTest, OperatorEqualsSameTypeAndValue) {
    Token a(TokenType::WORD, "echo");
    Token b(TokenType::WORD, "echo");
    EXPECT_TRUE(a == b);
}

// Проверяет: operator== возвращает false при разном type.
// Вход: (WORD, "echo") и (PIPE, ""). Выход: false.
TEST_F(TokenTest, OperatorEqualsDifferentType) {
    Token a(TokenType::WORD, "echo");
    Token b(TokenType::PIPE, "");
    EXPECT_FALSE(a == b);
}

// Проверяет: operator== возвращает false при разном value.
// Вход: (WORD, "echo") и (WORD, "cat"). Выход: false.
TEST_F(TokenTest, OperatorEqualsDifferentValue) {
    Token a(TokenType::WORD, "echo");
    Token b(TokenType::WORD, "cat");
    EXPECT_FALSE(a == b);
}

// Проверяет: operator!= противоположен operator==.
// Вход: одинаковые токены. Выход: false. Разные токены — true.
TEST_F(TokenTest, OperatorNotEquals) {
    Token a(TokenType::WORD, "echo");
    Token b(TokenType::WORD, "echo");
    Token c(TokenType::WORD, "cat");
    EXPECT_FALSE(a != b);
    EXPECT_TRUE(a != c);
}

// Проверяет: tokenTypeToString для каждого TokenType.
// Вход: WORD, PIPE, ASSIGNMENT, END_OF_INPUT. Выход: "WORD", "PIPE", "ASSIGNMENT", "END_OF_INPUT".
TEST_F(TokenTest, TokenTypeToStringWord) {
    EXPECT_EQ(tokenTypeToString(TokenType::WORD), "WORD");
}

TEST_F(TokenTest, TokenTypeToStringPipe) {
    EXPECT_EQ(tokenTypeToString(TokenType::PIPE), "PIPE");
}

TEST_F(TokenTest, TokenTypeToStringAssignment) {
    EXPECT_EQ(tokenTypeToString(TokenType::ASSIGNMENT), "ASSIGNMENT");
}

TEST_F(TokenTest, TokenTypeToStringEndOfInput) {
    EXPECT_EQ(tokenTypeToString(TokenType::END_OF_INPUT), "END_OF_INPUT");
}
