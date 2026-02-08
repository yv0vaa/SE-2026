#include <gtest/gtest.h>

#include "shell/environment.hpp"
#include "shell/substitutor.hpp"

using namespace shell;

class SubstitutorTest : public ::testing::Test {
protected:
    Environment env;
    
    void SetUp() override {
        env.set("FOO", "bar");
        env.set("HOME", "/home/user");
        env.set("EMPTY", "");
        env.set("MULTI", "hello world");
    }
};

TEST_F(SubstitutorTest, NoSubstitution) {
    Substitutor sub(env);
    EXPECT_EQ(sub.substitute("hello world"), "hello world");
}

TEST_F(SubstitutorTest, SimpleVariable) {
    Substitutor sub(env);
    EXPECT_EQ(sub.substitute("echo $FOO"), "echo bar");
}

TEST_F(SubstitutorTest, BracedVariable) {
    Substitutor sub(env);
    EXPECT_EQ(sub.substitute("echo ${FOO}"), "echo bar");
}

TEST_F(SubstitutorTest, VariableAtEnd) {
    Substitutor sub(env);
    EXPECT_EQ(sub.substitute("path=$HOME"), "path=/home/user");
}

TEST_F(SubstitutorTest, MultipleVariables) {
    Substitutor sub(env);
    EXPECT_EQ(sub.substitute("$FOO and $HOME"), "bar and /home/user");
}

TEST_F(SubstitutorTest, UndefinedVariable) {
    Substitutor sub(env);
    EXPECT_EQ(sub.substitute("$UNDEFINED"), "");
}

TEST_F(SubstitutorTest, VariableInDoubleQuotes) {
    Substitutor sub(env);
    EXPECT_EQ(sub.substitute("echo \"$FOO\""), "echo \"bar\"");
}

TEST_F(SubstitutorTest, VariableInSingleQuotes) {
    Substitutor sub(env);
    // В одинарных кавычках подстановка не выполняется
    EXPECT_EQ(sub.substitute("echo '$FOO'"), "echo '$FOO'");
}

TEST_F(SubstitutorTest, AdjacentVariables) {
    Substitutor sub(env);
    env.set("A", "hello");
    env.set("B", "world");
    EXPECT_EQ(sub.substitute("$A$B"), "helloworld");
}

TEST_F(SubstitutorTest, VariableWithSuffix) {
    Substitutor sub(env);
    EXPECT_EQ(sub.substitute("${FOO}baz"), "barbaz");
}

TEST_F(SubstitutorTest, EmptyVariable) {
    Substitutor sub(env);
    EXPECT_EQ(sub.substitute("x$EMPTY y"), "x y");
}

TEST_F(SubstitutorTest, DollarSign) {
    Substitutor sub(env);
    // Одинокий $ в конце
    EXPECT_EQ(sub.substitute("price is 100$"), "price is 100$");
}

TEST_F(SubstitutorTest, SpecialVariableQuestionMark) {
    Substitutor sub(env);
    env.set("?", "0");
    EXPECT_EQ(sub.substitute("exit code: $?"), "exit code: 0");
}

TEST_F(SubstitutorTest, MixedQuotes) {
    Substitutor sub(env);
    EXPECT_EQ(sub.substitute("'$FOO' \"$FOO\""), "'$FOO' \"bar\"");
}

TEST_F(SubstitutorTest, NestedBraces) {
    Substitutor sub(env);
    // ${FOO} работает
    EXPECT_EQ(sub.substitute("${FOO}"), "bar");
}

TEST_F(SubstitutorTest, VariableWithDigits) {
    Substitutor sub(env);
    env.set("VAR123", "test");
    EXPECT_EQ(sub.substitute("$VAR123"), "test");
}
