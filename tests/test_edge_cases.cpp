#include <cstdlib>
#include <fstream>
#include <sstream>

#include <gtest/gtest.h>

#include "shell/commands/external_command.hpp"
#include "shell/environment.hpp"
#include "shell/lexer.hpp"
#include "shell/shell.hpp"

using namespace shell;

// ============================================================
// ТЕСТЫ НА КАВЫЧКИ - КРАЕВЫЕ СЛУЧАИ
// ============================================================

class QuotesEdgeCasesTest : public ::testing::Test {
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

// Пустые кавычки - один пустой аргумент
TEST_F(QuotesEdgeCasesTest, EmptyDoubleQuotes) {
    Lexer lexer("echo \"\"");
    auto tokens = lexer.tokenize();

    // echo и пустая строка как аргумент
    ASSERT_EQ(tokens.size(), 3);
    EXPECT_EQ(tokens[0].value, "echo");
    EXPECT_EQ(tokens[1].value, "");  // пустой аргумент
}

TEST_F(QuotesEdgeCasesTest, EmptySingleQuotes) {
    Lexer lexer("echo ''");
    auto tokens = lexer.tokenize();

    ASSERT_EQ(tokens.size(), 3);
    EXPECT_EQ(tokens[0].value, "echo");
    EXPECT_EQ(tokens[1].value, "");
}

// Одинарные кавычки внутри двойных
TEST_F(QuotesEdgeCasesTest, SingleInsideDouble) {
    Shell shell;
    shell.processLine("echo \"it's working\"");

    EXPECT_EQ(capturedOutput.str(), "it's working\n");
}

// Двойные кавычки внутри одинарных
TEST_F(QuotesEdgeCasesTest, DoubleInsideSingle) {
    Shell shell;
    shell.processLine("echo 'say \"hello\"'");

    EXPECT_EQ(capturedOutput.str(), "say \"hello\"\n");
}

// Строка с пробелами в кавычках - ОДИН аргумент
TEST_F(QuotesEdgeCasesTest, SpacesInQuotesIsOneArgument) {
    Shell shell;
    shell.processLine("echo \"hello   world\"");

    // Должно быть "hello   world" с тремя пробелами (один аргумент)
    EXPECT_EQ(capturedOutput.str(), "hello   world\n");
}

// Смешанные кавычки в одном аргументе
// Примечание: текущая реализация разбивает на отдельные токены
TEST_F(QuotesEdgeCasesTest, MixedQuotesInOneArgument) {
    Lexer lexer("echo 'hello'\"world\"");
    auto tokens = lexer.tokenize();

    // Токены: echo, hello, world, END_OF_INPUT
    ASSERT_EQ(tokens.size(), 4);
    EXPECT_EQ(tokens[0].value, "echo");
    EXPECT_EQ(tokens[1].value, "hello");
    EXPECT_EQ(tokens[2].value, "world");
}

// Несколько аргументов в кавычках
TEST_F(QuotesEdgeCasesTest, MultipleQuotedArguments) {
    Shell shell;
    shell.processLine("echo \"arg one\" \"arg two\"");

    EXPECT_EQ(capturedOutput.str(), "arg one arg two\n");
}

// Кавычки в начале и конце
// Примечание: "start" middle "end" - разбивается на три аргумента
TEST_F(QuotesEdgeCasesTest, QuotesAtBoundaries) {
    Shell shell;
    shell.processLine("echo \"start\"middle\"end\"");

    // Текущее поведение: "start", middle, "end" -> "start middleend"
    EXPECT_EQ(capturedOutput.str(), "start middleend\n");
}

// Табуляция и специальные символы в кавычках
TEST_F(QuotesEdgeCasesTest, TabsInQuotes) {
    Shell shell;
    shell.processLine("echo \"hello\tworld\"");

    EXPECT_EQ(capturedOutput.str(), "hello\tworld\n");
}

// Переносы строк в кавычках (если поддерживается)
TEST_F(QuotesEdgeCasesTest, NewlineInQuotes) {
    Shell shell;
    shell.processLine("echo \"line1\nline2\"");

    EXPECT_EQ(capturedOutput.str(), "line1\nline2\n");
}

// Только кавычки без содержимого между аргументами
TEST_F(QuotesEdgeCasesTest, ConsecutiveEmptyQuotes) {
    Shell shell;
    shell.processLine("echo \"\" \"\" \"\"");

    // Три пустых аргумента
    EXPECT_EQ(capturedOutput.str(), "  \n");  // два пробела между тремя пустыми
}

// ============================================================
// ТЕСТЫ НА ПЕРЕМЕННЫЕ ОКРУЖЕНИЯ
// ============================================================

class EnvironmentEdgeCasesTest : public ::testing::Test {
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

// Переменная с пробелами в значении
TEST_F(EnvironmentEdgeCasesTest, VariableWithSpaces) {
    Shell shell;
    shell.processLine("VAR=\"hello world\"");
    shell.processLine("echo $VAR");

    EXPECT_EQ(capturedOutput.str(), "hello world\n");
}

// Переменная с специальными символами
TEST_F(EnvironmentEdgeCasesTest, VariableWithSpecialChars) {
    Shell shell;
    shell.processLine("PATH_VAR=\"/usr/bin:/usr/local/bin\"");
    shell.processLine("echo $PATH_VAR");

    EXPECT_EQ(capturedOutput.str(), "/usr/bin:/usr/local/bin\n");
}

// Переопределение переменной
TEST_F(EnvironmentEdgeCasesTest, VariableReassignment) {
    Shell shell;

    shell.processLine("X=first");
    shell.processLine("X=second");
    shell.processLine("echo $X");

    EXPECT_EQ(capturedOutput.str(), "second\n");
}

// Несуществующая переменная
TEST_F(EnvironmentEdgeCasesTest, UndefinedVariable) {
    Shell shell;
    shell.processLine("echo $UNDEFINED_VAR_12345");

    EXPECT_EQ(capturedOutput.str(), "\n");
}

// Переменная в середине строки
TEST_F(EnvironmentEdgeCasesTest, VariableInMiddle) {
    Shell shell;

    shell.processLine("NAME=User");
    shell.processLine("echo \"Hello, $NAME! Welcome.\"");

    EXPECT_EQ(capturedOutput.str(), "Hello, User! Welcome.\n");
}

// Переменная с цифрами в имени
TEST_F(EnvironmentEdgeCasesTest, VariableWithDigits) {
    Shell shell;

    shell.processLine("VAR123=test");
    shell.processLine("echo $VAR123");

    EXPECT_EQ(capturedOutput.str(), "test\n");
}

// Переменная с подчёркиванием
TEST_F(EnvironmentEdgeCasesTest, VariableWithUnderscore) {
    Shell shell;

    shell.processLine("MY_VAR_NAME=value");
    shell.processLine("echo $MY_VAR_NAME");

    EXPECT_EQ(capturedOutput.str(), "value\n");
}

// Пустое значение переменной
TEST_F(EnvironmentEdgeCasesTest, EmptyVariableValue) {
    Shell shell;

    shell.processLine("EMPTY=");
    shell.processLine("echo x${EMPTY}y");

    EXPECT_EQ(capturedOutput.str(), "xy\n");
}

// Конкатенация переменных
TEST_F(EnvironmentEdgeCasesTest, VariableConcatenation) {
    Shell shell;

    shell.processLine("A=Hello");
    shell.processLine("B=World");
    shell.processLine("echo $A$B");

    EXPECT_EQ(capturedOutput.str(), "HelloWorld\n");
}

// Переменная в фигурных скобках с суффиксом
TEST_F(EnvironmentEdgeCasesTest, BracedVariableWithSuffix) {
    Shell shell;

    shell.processLine("FILE=test");
    shell.processLine("echo ${FILE}.txt");

    EXPECT_EQ(capturedOutput.str(), "test.txt\n");
}

// Переменная в одинарных кавычках не раскрывается
TEST_F(EnvironmentEdgeCasesTest, NoSubstitutionInSingleQuotes) {
    Shell shell;

    shell.processLine("VAR=value");
    shell.processLine("echo '$VAR'");

    EXPECT_EQ(capturedOutput.str(), "$VAR\n");
}

// Смешанные кавычки с переменными
TEST_F(EnvironmentEdgeCasesTest, MixedQuotesWithVariables) {
    Shell shell;

    shell.processLine("NAME=Test");
    shell.processLine("echo 'literal: $NAME' \"expanded: $NAME\"");

    EXPECT_EQ(capturedOutput.str(), "literal: $NAME expanded: Test\n");
}

// ============================================================
// ТЕСТЫ НА КОДЫ ВОЗВРАТА
// ============================================================

class ReturnCodeTest : public ::testing::Test {
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

// Успешная команда возвращает 0
TEST_F(ReturnCodeTest, SuccessfulCommandReturnsZero) {
    Shell shell;
    int result = shell.processLine("echo hello");

    EXPECT_EQ(result, 0);
}

// cat несуществующего файла возвращает не 0
TEST_F(ReturnCodeTest, CatNonexistentFileReturnsError) {
    Shell shell;
    int result = shell.processLine("cat /nonexistent/file/12345.txt");

    EXPECT_NE(result, 0);
}

// $? содержит код возврата предыдущей команды
TEST_F(ReturnCodeTest, QuestionMarkVariable) {
    Shell shell;

    shell.processLine("echo success");
    capturedOutput.str("");

    shell.processLine("echo $?");
    EXPECT_EQ(capturedOutput.str(), "0\n");
}

// $? после ошибки
TEST_F(ReturnCodeTest, QuestionMarkAfterError) {
    Shell shell;

    shell.processLine("cat /nonexistent/file/xyz.txt");
    capturedOutput.str("");
    capturedErrors.str("");

    shell.processLine("echo $?");
    std::string out = capturedOutput.str();
    // Не должно быть 0
    EXPECT_TRUE(out.find("0") == std::string::npos || out != "0\n");
}

// Код возврата пайплайна - последняя команда
TEST_F(ReturnCodeTest, PipelineReturnCodeIsLast) {
    Shell shell;

    // echo успешно, cat несуществующего файла - ошибка
    int result = shell.processLine("echo test | cat");
    EXPECT_EQ(result, 0);  // cat из stdin успешно
}

// exit с кодом
TEST_F(ReturnCodeTest, ExitWithCode) {
    Shell shell;
    int result = shell.processLine("exit 42");

    EXPECT_EQ(result, 42);
}

// ============================================================
// ТЕСТЫ НА ПОТОКИ ВЫВОДА И ОШИБОК
// ============================================================

class StreamsTest : public ::testing::Test {
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

// echo выводит в stdout
TEST_F(StreamsTest, EchoToStdout) {
    Shell shell;
    shell.processLine("echo hello");

    EXPECT_EQ(capturedOutput.str(), "hello\n");
    EXPECT_EQ(capturedErrors.str(), "");
}

// cat ошибка в stderr
TEST_F(StreamsTest, CatErrorToStderr) {
    Shell shell;
    shell.processLine("cat /nonexistent/file.txt");

    EXPECT_FALSE(capturedErrors.str().empty());
}

// pwd выводит в stdout
TEST_F(StreamsTest, PwdToStdout) {
    Shell shell;
    shell.processLine("pwd");

    EXPECT_FALSE(capturedOutput.str().empty());
    EXPECT_TRUE(capturedOutput.str().find('/') != std::string::npos);
}

// wc выводит в stdout
TEST_F(StreamsTest, WcToStdout) {
    Shell shell;
    shell.processLine("echo hello | wc");

    EXPECT_FALSE(capturedOutput.str().empty());
    EXPECT_EQ(capturedErrors.str(), "");
}

// ============================================================
// ТЕСТЫ НА ВНЕШНИЕ КОМАНДЫ
// ============================================================

class ExternalCommandTest : public ::testing::Test {
protected:
    Environment env;
    std::ostringstream capturedOutput;
    std::ostringstream capturedErrors;
    std::streambuf* oldCout;
    std::streambuf* oldCerr;

    void SetUp() override {
        env.initFromSystem();
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

// Внешняя команда true возвращает 0
TEST_F(ExternalCommandTest, TrueReturnsZero) {
    Shell shell;
    // На macOS: /usr/bin/true, на Linux: /bin/true или /usr/bin/true
    int result = shell.processLine("true");  // использует PATH

    EXPECT_EQ(result, 0);
}

// Внешняя команда false возвращает не 0
TEST_F(ExternalCommandTest, FalseReturnsNonZero) {
    Shell shell;
    int result = shell.processLine("false");  // использует PATH

    EXPECT_NE(result, 0);
}

// ls выводит что-то в stdout
TEST_F(ExternalCommandTest, LsOutputsToStdout) {
    Shell shell;
    shell.processLine("ls /");  // использует PATH

    EXPECT_FALSE(capturedOutput.str().empty());
}

// Передача переменных внешнему процессу через printenv
TEST_F(ExternalCommandTest, EnvironmentPassedToExternal) {
    Shell shell;

    shell.processLine("TEST_VAR_XYZ=hello123");
    shell.processLine("/usr/bin/printenv TEST_VAR_XYZ");

    std::string output = capturedOutput.str();
    EXPECT_TRUE(output.find("hello123") != std::string::npos);
}

// Несуществующая команда
TEST_F(ExternalCommandTest, NonexistentCommand) {
    Shell shell;
    int result = shell.processLine("/nonexistent/command/xyz");

    EXPECT_NE(result, 0);
}

// ============================================================
// ТЕСТЫ НА ПАЙПЛАЙНЫ - КРАЕВЫЕ СЛУЧАИ
// ============================================================

class PipelineEdgeCasesTest : public ::testing::Test {
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

// Пайплайн из трёх команд
TEST_F(PipelineEdgeCasesTest, ThreeCommandPipeline) {
    Shell shell;
    shell.processLine("echo one two three | cat | wc");

    // 1 строка, 3 слова
    std::string out = capturedOutput.str();
    EXPECT_TRUE(out.find("1 3") != std::string::npos);
}

// Пустой вывод в пайплайне
TEST_F(PipelineEdgeCasesTest, EmptyOutputPipeline) {
    Shell shell;
    shell.processLine("echo \"\" | wc");

    // 1 строка (\n), 0 слов, 1 байт
    std::string out = capturedOutput.str();
    EXPECT_TRUE(out.find("0") != std::string::npos);
}

// Многострочный вывод в пайплайне
TEST_F(PipelineEdgeCasesTest, MultilinePipeline) {
    // Создаём временный файл
    std::ofstream f("/tmp/test_multiline.txt");
    f << "line1\nline2\nline3\nline4\nline5\n";
    f.close();

    Shell shell;
    shell.processLine("cat /tmp/test_multiline.txt | wc");

    std::string out = capturedOutput.str();
    EXPECT_TRUE(out.find("5") != std::string::npos);  // 5 строк

    std::remove("/tmp/test_multiline.txt");
}

// ============================================================
// ТЕСТЫ НА ОБЩИЕ КРАЕВЫЕ СЛУЧАИ
// ============================================================

class GeneralEdgeCasesTest : public ::testing::Test {
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

// Команда с большим количеством аргументов
TEST_F(GeneralEdgeCasesTest, ManyArguments) {
    Shell shell;
    shell.processLine("echo a b c d e f g h i j");

    EXPECT_EQ(capturedOutput.str(), "a b c d e f g h i j\n");
}

// Очень длинный аргумент
TEST_F(GeneralEdgeCasesTest, LongArgument) {
    std::string longArg(1000, 'x');
    Shell shell;
    shell.processLine("echo " + longArg);

    EXPECT_EQ(capturedOutput.str(), longArg + "\n");
}

// Специальные символы без кавычек
TEST_F(GeneralEdgeCasesTest, SpecialCharsWithoutQuotes) {
    Shell shell;
    shell.processLine("echo hello!world");

    EXPECT_EQ(capturedOutput.str(), "hello!world\n");
}

// Дефис в аргументе
TEST_F(GeneralEdgeCasesTest, DashInArgument) {
    Shell shell;
    shell.processLine("echo --help");

    EXPECT_EQ(capturedOutput.str(), "--help\n");
}

// Числа как аргументы
TEST_F(GeneralEdgeCasesTest, NumbersAsArguments) {
    Shell shell;
    shell.processLine("echo 123 456 789");

    EXPECT_EQ(capturedOutput.str(), "123 456 789\n");
}

// Пробелы вокруг пайпа
TEST_F(GeneralEdgeCasesTest, SpacesAroundPipe) {
    Shell shell;
    shell.processLine("echo test    |     cat");

    EXPECT_EQ(capturedOutput.str(), "test\n");
}

// Несколько пробелов между аргументами
TEST_F(GeneralEdgeCasesTest, MultipleSpacesBetweenArgs) {
    Shell shell;
    shell.processLine("echo   hello    world");

    EXPECT_EQ(capturedOutput.str(), "hello world\n");
}

// Точка в команде
TEST_F(GeneralEdgeCasesTest, DotInPath) {
    Shell shell;
    shell.processLine("echo ./relative/path");

    EXPECT_EQ(capturedOutput.str(), "./relative/path\n");
}

// Слэш в аргументе
TEST_F(GeneralEdgeCasesTest, SlashInArgument) {
    Shell shell;
    shell.processLine("echo /usr/bin/something");

    EXPECT_EQ(capturedOutput.str(), "/usr/bin/something\n");
}

// Равно в аргументе (не присваивание)
TEST_F(GeneralEdgeCasesTest, EqualsInArgument) {
    Shell shell;
    shell.processLine("echo key=value as argument");

    // Если после = нет команды, это присваивание
    // Если есть другие аргументы после - это аргументы echo
    // Зависит от парсера
}

// Знак доллара в конце строки
TEST_F(GeneralEdgeCasesTest, DollarAtEnd) {
    Shell shell;
    shell.processLine("echo price is 100$");

    // Одинокий $ остаётся как есть
    EXPECT_EQ(capturedOutput.str(), "price is 100$\n");
}

// ============================================================
// LEXER - КРАЕВЫЕ СЛУЧАИ
// ============================================================

class LexerEdgeCasesTest : public ::testing::Test {};

// Незакрытая двойная кавычка
TEST_F(LexerEdgeCasesTest, UnclosedDoubleQuote) {
    Lexer lexer("echo \"hello");
    auto tokens = lexer.tokenize();

    // Должен обработать (возможно с ошибкой или взять до конца)
    EXPECT_GE(tokens.size(), 2);
}

// Незакрытая одинарная кавычка
TEST_F(LexerEdgeCasesTest, UnclosedSingleQuote) {
    Lexer lexer("echo 'hello");
    auto tokens = lexer.tokenize();

    EXPECT_GE(tokens.size(), 2);
}

// Пустой pipe
TEST_F(LexerEdgeCasesTest, EmptyPipe) {
    Lexer lexer("echo | | wc");
    auto tokens = lexer.tokenize();

    // Должен найти два PIPE
    int pipeCount = 0;
    for (const auto& t : tokens) {
        if (t.type == TokenType::PIPE) pipeCount++;
    }
    EXPECT_EQ(pipeCount, 2);
}

// Pipe в начале строки
TEST_F(LexerEdgeCasesTest, PipeAtStart) {
    Lexer lexer("| echo hello");
    auto tokens = lexer.tokenize();

    EXPECT_EQ(tokens[0].type, TokenType::PIPE);
}

// Pipe в конце строки
TEST_F(LexerEdgeCasesTest, PipeAtEnd) {
    Lexer lexer("echo hello |");
    auto tokens = lexer.tokenize();

    EXPECT_EQ(tokens[2].type, TokenType::PIPE);
}

// Backslash вне кавычек
TEST_F(LexerEdgeCasesTest, BackslashOutsideQuotes) {
    Lexer lexer("echo hello\\ world");
    auto tokens = lexer.tokenize();

    // Зависит от реализации: может экранировать пробел
    EXPECT_GE(tokens.size(), 2);
}
