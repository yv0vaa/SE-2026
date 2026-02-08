#include <gtest/gtest.h>

#include <fstream>
#include <sstream>

#include "shell/shell.hpp"

using namespace shell;

class IntegrationTest : public ::testing::Test {
protected:
    std::ostringstream capturedOutput;
    std::ostringstream capturedErrors;
    std::streambuf* oldCout;
    std::streambuf* oldCerr;
    
    void SetUp() override {
        capturedOutput.str("");
        capturedErrors.str("");
        oldCout = std::cout.rdbuf(capturedOutput.rdbuf());
        oldCerr = std::cerr.rdbuf(capturedErrors.rdbuf());
    }
    
    void TearDown() override {
        std::cout.rdbuf(oldCout);
        std::cerr.rdbuf(oldCerr);
    }
};

TEST_F(IntegrationTest, SimpleEcho) {
    Shell shell;
    int result = shell.processLine("echo Hello, World!");
    
    EXPECT_EQ(result, 0);
    EXPECT_EQ(capturedOutput.str(), "Hello, World!\n");
}

TEST_F(IntegrationTest, VariableAssignment) {
    Shell shell;
    
    shell.processLine("FOO=bar");
    EXPECT_EQ(shell.getEnvironment().get("FOO"), "bar");
}

TEST_F(IntegrationTest, VariableSubstitution) {
    Shell shell;
    
    shell.processLine("FOO=hello");
    shell.processLine("echo $FOO");
    
    EXPECT_EQ(capturedOutput.str(), "hello\n");
}

TEST_F(IntegrationTest, VariableInQuotes) {
    Shell shell;
    
    shell.processLine("NAME=World");
    shell.processLine("echo \"Hello, $NAME!\"");
    
    EXPECT_EQ(capturedOutput.str(), "Hello, World!\n");
}

TEST_F(IntegrationTest, PipelineEchoToWc) {
    Shell shell;
    int result = shell.processLine("echo hello world | wc");
    
    EXPECT_EQ(result, 0);
    // hello world\n = 12 bytes, 1 line, 2 words
    EXPECT_EQ(capturedOutput.str(), "1 2 12\n");
}

TEST_F(IntegrationTest, PipelineEchoToCat) {
    Shell shell;
    int result = shell.processLine("echo test | cat");
    
    EXPECT_EQ(result, 0);
    EXPECT_EQ(capturedOutput.str(), "test\n");
}

TEST_F(IntegrationTest, MultipleAssignments) {
    Shell shell;
    
    shell.processLine("x=1 y=2");
    
    EXPECT_EQ(shell.getEnvironment().get("x"), "1");
    EXPECT_EQ(shell.getEnvironment().get("y"), "2");
}

TEST_F(IntegrationTest, VariableConcatenation) {
    Shell shell;
    
    shell.processLine("A=hello");
    shell.processLine("B=world");
    shell.processLine("echo $A$B");
    
    EXPECT_EQ(capturedOutput.str(), "helloworld\n");
}

TEST_F(IntegrationTest, SingleQuotesNoSubstitution) {
    Shell shell;
    
    shell.processLine("FOO=bar");
    shell.processLine("echo '$FOO'");
    
    EXPECT_EQ(capturedOutput.str(), "$FOO\n");
}

TEST_F(IntegrationTest, EmptyLine) {
    Shell shell;
    int result = shell.processLine("");
    
    EXPECT_EQ(result, 0);
    EXPECT_EQ(capturedOutput.str(), "");
}

TEST_F(IntegrationTest, WhitespaceOnlyLine) {
    Shell shell;
    int result = shell.processLine("   ");
    
    EXPECT_EQ(result, 0);
    EXPECT_EQ(capturedOutput.str(), "");
}

TEST_F(IntegrationTest, ExitCommand) {
    Shell shell;
    shell.processLine("exit");
    
    // После exit должен работать shouldExit
    // Но в processLine мы не проверяем это напрямую
}

TEST_F(IntegrationTest, ReturnCodeVariable) {
    Shell shell;
    
    // Успешная команда
    shell.processLine("echo test");
    capturedOutput.str(""); // очищаем
    
    shell.processLine("echo $?");
    EXPECT_EQ(capturedOutput.str(), "0\n");
}

TEST_F(IntegrationTest, ComplexPipeline) {
    Shell shell;
    
    // Создаём временный файл
    std::ofstream f("/tmp/test_integration.txt");
    f << "line one\nline two\nline three\n";
    f.close();
    
    shell.processLine("cat /tmp/test_integration.txt | wc");
    
    // 3 строки, 6 слов (точное количество байт зависит от реализации)
    std::string out = capturedOutput.str();
    EXPECT_TRUE(out.find("3 6") != std::string::npos);
    
    std::remove("/tmp/test_integration.txt");
}

TEST_F(IntegrationTest, EchoWithQuotedSpaces) {
    Shell shell;
    shell.processLine("echo \"hello world\"");
    
    EXPECT_EQ(capturedOutput.str(), "hello world\n");
}
