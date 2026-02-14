#include <sstream>

#include <gtest/gtest.h>

#include "shell/input_reader.hpp"

using namespace shell;

/**
 * Юнит-тесты для InputReader.
 * Проверяют: readLine (с данными и EOF), setPrompt, showPrompt.
 * Вход: содержимое istream. Выход: optional<string> или nullopt.
 */

class InputReaderTest : public ::testing::Test {};

// Проверяет: readLine возвращает строку до первого \n.
// Вход: поток "hello\n". Выход: optional("hello").
TEST_F(InputReaderTest, ReadLineReturnsLine) {
    std::istringstream in("hello\n");
    InputReader reader(in);
    reader.showPrompt(false);

    auto line = reader.readLine();
    ASSERT_TRUE(line.has_value());
    EXPECT_EQ(*line, "hello");
}

// Проверяет: readLine при EOF возвращает nullopt.
// Вход: пустой поток. Выход: nullopt.
TEST_F(InputReaderTest, ReadLineEofReturnsNullopt) {
    std::istringstream in("");
    InputReader reader(in);
    reader.showPrompt(false);

    auto line = reader.readLine();
    EXPECT_FALSE(line.has_value());
}

// Проверяет: вторая readLine после одной строки при пустом потоке — nullopt.
// Вход: "one\n" (после первой readLine поток пуст). Выход: первая "one", вторая nullopt.
TEST_F(InputReaderTest, ReadLineSecondCallEof) {
    std::istringstream in("one\n");
    InputReader reader(in);
    reader.showPrompt(false);

    auto first = reader.readLine();
    ASSERT_TRUE(first.has_value());
    EXPECT_EQ(*first, "one");

    auto second = reader.readLine();
    EXPECT_FALSE(second.has_value());
}

// Проверяет: несколько строк читаются по одной.
// Вход: "a\nb\nc\n". Выход: "a", "b", "c".
TEST_F(InputReaderTest, ReadLineMultipleLines) {
    std::istringstream in("a\nb\nc\n");
    InputReader reader(in);
    reader.showPrompt(false);

    auto a = reader.readLine();
    auto b = reader.readLine();
    auto c = reader.readLine();
    ASSERT_TRUE(a.has_value() && b.has_value() && c.has_value());
    EXPECT_EQ(*a, "a");
    EXPECT_EQ(*b, "b");
    EXPECT_EQ(*c, "c");
}

// Проверяет: setPrompt меняет приглашение (проверяем через то, что readLine не падает и возвращает данные).
// Вход: setPrompt("$$ "), поток "x\n". Выход: readLine() == "x" (приглашение уходит в cout, здесь не перехватываем).
TEST_F(InputReaderTest, SetPromptThenReadLine) {
    std::istringstream in("x\n");
    InputReader reader(in);
    reader.showPrompt(false);
    reader.setPrompt("$$ ");

    auto line = reader.readLine();
    ASSERT_TRUE(line.has_value());
    EXPECT_EQ(*line, "x");
}

// Проверяет: showPrompt(false) — при readLine приглашение не выводится (тест не проверяет cout, только что readLine работает).
// Вход: showPrompt(false), "test\n". Выход: readLine() == "test".
TEST_F(InputReaderTest, ShowPromptFalseReadLineStillWorks) {
    std::istringstream in("test\n");
    InputReader reader(in);
    reader.showPrompt(false);

    auto line = reader.readLine();
    ASSERT_TRUE(line.has_value());
    EXPECT_EQ(*line, "test");
}

// Проверяет: пустая строка (только \n) возвращается как "".
// Вход: "\n". Выход: optional("").
TEST_F(InputReaderTest, ReadLineEmptyLine) {
    std::istringstream in("\n");
    InputReader reader(in);
    reader.showPrompt(false);

    auto line = reader.readLine();
    ASSERT_TRUE(line.has_value());
    EXPECT_EQ(*line, "");
}
