#include <gtest/gtest.h>

#include <sstream>

#include "shell/command_factory.hpp"
#include "shell/environment.hpp"
#include "shell/executor.hpp"
#include "shell/pipeline.hpp"
#include "shell/pipeline_builder.hpp"

using namespace shell;

class PipelineTest : public ::testing::Test {
protected:
    Environment env;
    
    void SetUp() override {
        env.initFromSystem();
    }
};

TEST_F(PipelineTest, EmptyPipeline) {
    Pipeline pipeline;
    
    EXPECT_TRUE(pipeline.isEmpty());
    EXPECT_EQ(pipeline.size(), 0);
}

TEST_F(PipelineTest, SingleCommand) {
    CommandFactory factory(env);
    
    Pipeline pipeline;
    auto cmd = factory.create("echo");
    cmd->setArguments({"hello"});
    pipeline.addCommand(std::move(cmd));
    
    EXPECT_FALSE(pipeline.isEmpty());
    EXPECT_EQ(pipeline.size(), 1);
}

TEST_F(PipelineTest, MultipleCommands) {
    CommandFactory factory(env);
    
    Pipeline pipeline;
    
    auto echo = factory.create("echo");
    echo->setArguments({"hello", "world"});
    pipeline.addCommand(std::move(echo));
    
    auto wc = factory.create("wc");
    wc->setArguments({});
    pipeline.addCommand(std::move(wc));
    
    EXPECT_EQ(pipeline.size(), 2);
}

TEST_F(PipelineTest, BuildFromParsedPipeline) {
    CommandFactory factory(env);
    PipelineBuilder builder(factory);
    
    ParsedPipeline parsed;
    parsed.addCommand(ParsedSimpleCommand("echo", {"hello"}));
    parsed.addCommand(ParsedSimpleCommand("wc", {}));
    
    Pipeline pipeline = builder.build(parsed);
    
    EXPECT_EQ(pipeline.size(), 2);
}

TEST_F(PipelineTest, ExecuteSingleCommand) {
    CommandFactory factory(env);
    Executor executor(env);
    
    Pipeline pipeline;
    auto cmd = factory.create("echo");
    cmd->setArguments({"test"});
    pipeline.addCommand(std::move(cmd));
    
    // Перенаправляем stdout
    std::ostringstream capturedOutput;
    std::streambuf* oldCout = std::cout.rdbuf(capturedOutput.rdbuf());
    
    int result = executor.execute(pipeline);
    
    std::cout.rdbuf(oldCout);
    
    EXPECT_EQ(result, 0);
    EXPECT_EQ(capturedOutput.str(), "test\n");
}

TEST_F(PipelineTest, ExecutePipelineTwoCommands) {
    CommandFactory factory(env);
    Executor executor(env);
    
    Pipeline pipeline;
    
    auto echo = factory.create("echo");
    echo->setArguments({"hello", "world"});
    pipeline.addCommand(std::move(echo));
    
    auto wc = factory.create("wc");
    wc->setArguments({});
    pipeline.addCommand(std::move(wc));
    
    // Перенаправляем stdout
    std::ostringstream capturedOutput;
    std::streambuf* oldCout = std::cout.rdbuf(capturedOutput.rdbuf());
    
    int result = executor.execute(pipeline);
    
    std::cout.rdbuf(oldCout);
    
    EXPECT_EQ(result, 0);
    // echo "hello world" выводит 12 символов (hello world\n)
    // wc должен показать 1 1 12 (1 строка, 2 слова, 12 байт)
    EXPECT_EQ(capturedOutput.str(), "1 2 12\n");
}

TEST_F(PipelineTest, CommandFactoryBuiltins) {
    CommandFactory factory(env);
    
    EXPECT_TRUE(factory.isBuiltin("echo"));
    EXPECT_TRUE(factory.isBuiltin("cat"));
    EXPECT_TRUE(factory.isBuiltin("wc"));
    EXPECT_TRUE(factory.isBuiltin("pwd"));
    EXPECT_TRUE(factory.isBuiltin("exit"));
    
    EXPECT_FALSE(factory.isBuiltin("ls"));
    EXPECT_FALSE(factory.isBuiltin("grep"));
}
