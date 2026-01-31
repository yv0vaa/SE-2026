cmake_minimum_required(VERSION 3.16)
project(shell VERSION 1.0.0 LANGUAGES CXX)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile commands for clang-tidy
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Options
option(BUILD_TESTING "Build tests" OFF)
option(ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" OFF)
option(ENABLE_SANITIZER_UNDEFINED_BEHAVIOR "Enable undefined behavior sanitizer" OFF)

# Note: Compiler warnings will be applied per-target when source files are added
# This prevents warnings from affecting third-party dependencies like GoogleTest

# Sanitizers (applied globally, safe for dependencies)
if(ENABLE_SANITIZER_ADDRESS)
    add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address)
endif()

if(ENABLE_SANITIZER_UNDEFINED_BEHAVIOR)
    add_compile_options(-fsanitize=undefined -fno-omit-frame-pointer)
    add_link_options(-fsanitize=undefined)
endif()

# =============================================================================
# Source files - uncomment when implementation begins
# =============================================================================

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

# add_executable(shell ${SHELL_SOURCES})
# target_include_directories(shell PRIVATE ${CMAKE_SOURCE_DIR}/include)

# # Apply strict warnings only to our code, not dependencies
# target_compile_options(shell PRIVATE
#     $<$<CXX_COMPILER_ID:GNU,Clang>:
#         -Wall -Wextra -Wpedantic -Werror
#         -Wconversion -Wsign-conversion -Wshadow
#         -Wnon-virtual-dtor -Wold-style-cast -Wcast-align
#         -Wunused -Woverloaded-virtual -Wformat=2
#     >
# )

# =============================================================================
# Testing - uncomment when implementation begins
# =============================================================================

# if(BUILD_TESTING)
#     enable_testing()
#     
#     include(FetchContent)
#     FetchContent_Declare(
#         googletest
#         GIT_REPOSITORY https://github.com/google/googletest.git
#         GIT_TAG v1.14.0
#     )
#     set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
#     # Disable warnings for GoogleTest
#     set(CMAKE_CXX_FLAGS_BACKUP "${CMAKE_CXX_FLAGS}")
#     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")
#     FetchContent_MakeAvailable(googletest)
#     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_BACKUP}")
#     
#     add_executable(shell_tests
#         tests/test_lexer.cpp
#         tests/test_parser.cpp
#         tests/test_substitutor.cpp
#         tests/test_commands.cpp
#         tests/test_pipeline.cpp
#         tests/test_integration.cpp
#     )
#     target_include_directories(shell_tests PRIVATE ${CMAKE_SOURCE_DIR}/include)
#     target_link_libraries(shell_tests PRIVATE GTest::gtest_main)
#     
#     # Apply strict warnings only to test code
#     target_compile_options(shell_tests PRIVATE
#         $<$<CXX_COMPILER_ID:GNU,Clang>:
#             -Wall -Wextra -Wpedantic
#         >
#     )
#     
#     include(GoogleTest)
#     gtest_discover_tests(shell_tests)
# endif()

# =============================================================================
# Install - uncomment when implementation begins
# =============================================================================

# install(TARGETS shell RUNTIME DESTINATION bin)

message(STATUS "")
message(STATUS "=== Shell Project Configuration ===")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build testing: ${BUILD_TESTING}")
message(STATUS "Address Sanitizer: ${ENABLE_SANITIZER_ADDRESS}")
message(STATUS "UB Sanitizer: ${ENABLE_SANITIZER_UNDEFINED_BEHAVIOR}")
message(STATUS "")
message(STATUS "Note: No source files yet. This is a design-only phase.")
message(STATUS "===================================")
message(STATUS "")
