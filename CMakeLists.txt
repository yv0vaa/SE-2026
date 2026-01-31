cmake_minimum_required(VERSION 3.16)
project(shell VERSION 1.0.0 LANGUAGES CXX)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile commands for clang-tidy
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Options
option(BUILD_TESTING "Build tests" ON)
option(ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" OFF)
option(ENABLE_SANITIZER_UNDEFINED_BEHAVIOR "Enable undefined behavior sanitizer" OFF)

# Compiler warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
        -Werror
        -Wconversion
        -Wsign-conversion
        -Wshadow
        -Wnon-virtual-dtor
        -Wold-style-cast
        -Wcast-align
        -Wunused
        -Woverloaded-virtual
        -Wformat=2
    )
endif()

# Sanitizers
if(ENABLE_SANITIZER_ADDRESS)
    add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address)
endif()

if(ENABLE_SANITIZER_UNDEFINED_BEHAVIOR)
    add_compile_options(-fsanitize=undefined -fno-omit-frame-pointer)
    add_link_options(-fsanitize=undefined)
endif()

# Source files (будут добавлены при реализации)
# set(SHELL_SOURCES
#     src/main.cpp
#     src/shell.cpp
#     src/environment.cpp
#     src/input_reader.cpp
#     src/substitutor.cpp
#     src/lexer.cpp
#     src/parser.cpp
#     src/command_factory.cpp
#     src/pipeline.cpp
#     src/pipeline_builder.cpp
#     src/executor.cpp
#     src/commands/echo_command.cpp
#     src/commands/cat_command.cpp
#     src/commands/wc_command.cpp
#     src/commands/pwd_command.cpp
#     src/commands/exit_command.cpp
#     src/commands/external_command.cpp
# )

# Main executable (будет раскомментировано при реализации)
# add_executable(shell ${SHELL_SOURCES})
# target_include_directories(shell PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Testing
if(BUILD_TESTING)
    enable_testing()
    
    # Fetch GoogleTest
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    
    # Test executable (будет раскомментировано при реализации)
    # add_executable(shell_tests
    #     tests/test_lexer.cpp
    #     tests/test_parser.cpp
    #     tests/test_substitutor.cpp
    #     tests/test_commands.cpp
    #     tests/test_pipeline.cpp
    #     tests/test_integration.cpp
    # )
    # target_include_directories(shell_tests PRIVATE ${CMAKE_SOURCE_DIR}/include)
    # target_link_libraries(shell_tests PRIVATE GTest::gtest_main)
    
    include(GoogleTest)
    # gtest_discover_tests(shell_tests)
endif()

# Install
# install(TARGETS shell RUNTIME DESTINATION bin)
