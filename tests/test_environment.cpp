#include <algorithm>
#include <gtest/gtest.h>

#include "shell/environment.hpp"

using namespace shell;

/**
 * Юнит-тесты для Environment.
 * Проверяют: get, set, unset, contains, toEnvp, initFromSystem.
 * Вход/выход указаны в комментариях к каждому тесту.
 */

class EnvironmentTest : public ::testing::Test {
protected:
    Environment env;
};

// Проверяет: get несуществующей переменной возвращает пустую строку.
// Вход: name "UNDEF". Выход: "".
TEST_F(EnvironmentTest, GetUndefinedReturnsEmpty) {
    EXPECT_EQ(env.get("UNDEF"), "");
}

// Проверяет: set и get — значение сохраняется и возвращается.
// Вход: set("FOO", "bar"). Выход: get("FOO") == "bar".
TEST_F(EnvironmentTest, SetAndGet) {
    env.set("FOO", "bar");
    EXPECT_EQ(env.get("FOO"), "bar");
}

// Проверяет: повторный set перезаписывает значение.
// Вход: set("X","1"); set("X","2"). Выход: get("X") == "2".
TEST_F(EnvironmentTest, SetOverwrites) {
    env.set("X", "1");
    env.set("X", "2");
    EXPECT_EQ(env.get("X"), "2");
}

// Проверяет: set пустого значения допустим.
// Вход: set("EMPTY", ""). Выход: get("EMPTY") == "".
TEST_F(EnvironmentTest, SetEmptyValue) {
    env.set("EMPTY", "");
    EXPECT_EQ(env.get("EMPTY"), "");
}

// Проверяет: contains возвращает false для несуществующей переменной.
// Вход: —. Выход: contains("NONE") == false.
TEST_F(EnvironmentTest, ContainsUndefined) {
    EXPECT_FALSE(env.contains("NONE"));
}

// Проверяет: contains возвращает true после set.
// Вход: set("K", "v"). Выход: contains("K") == true.
TEST_F(EnvironmentTest, ContainsAfterSet) {
    env.set("K", "v");
    EXPECT_TRUE(env.contains("K"));
}

// Проверяет: unset удаляет переменную; get возвращает "", contains — false.
// Вход: set("T","1"); unset("T"). Выход: get("T")=="", contains("T")==false.
TEST_F(EnvironmentTest, UnsetRemovesVariable) {
    env.set("T", "1");
    EXPECT_TRUE(env.contains("T"));
    env.unset("T");
    EXPECT_FALSE(env.contains("T"));
    EXPECT_EQ(env.get("T"), "");
}

// Проверяет: unset несуществующей переменной не падает.
// Вход: unset("NO_SUCH_VAR"). Выход: без исключений.
TEST_F(EnvironmentTest, UnsetNonexistentNoThrow) {
    env.unset("NO_SUCH_VAR");
    EXPECT_EQ(env.get("NO_SUCH_VAR"), "");
}

// Проверяет: toEnvp возвращает строки формата "NAME=VALUE".
// Вход: set("A","1"); set("B","2"). Выход: вектор содержит "A=1", "B=2" (порядок может быть любой).
TEST_F(EnvironmentTest, ToEnvpFormat) {
    env.set("A", "1");
    env.set("B", "2");
    auto vec = env.toEnvp();
    EXPECT_EQ(vec.size(), 2u);
    EXPECT_TRUE(std::find(vec.begin(), vec.end(), "A=1") != vec.end());
    EXPECT_TRUE(std::find(vec.begin(), vec.end(), "B=2") != vec.end());
}

// Проверяет: toEnvp пустого окружения — пустой вектор.
// Вход: новый Environment без set. Выход: toEnvp().size() == 0.
TEST_F(EnvironmentTest, ToEnvpEmpty) {
    Environment emptyEnv;
    EXPECT_TRUE(emptyEnv.toEnvp().empty());
}

// Проверяет: initFromSystem не падает и после него окружение не пустое или содержит "?".
// Вход: initFromSystem(). Выход: не бросает; toEnvp() не пустой ИЛИ get("?") доступен (зависит от реализации).
TEST_F(EnvironmentTest, InitFromSystemNoThrow) {
    Environment sysEnv;
    EXPECT_NO_THROW(sysEnv.initFromSystem());
    // После init либо есть переменные из системы, либо хотя бы "?"
    auto vec = sysEnv.toEnvp();
    bool hasQuestion = sysEnv.contains("?");
    EXPECT_TRUE(!vec.empty() || hasQuestion);
}
