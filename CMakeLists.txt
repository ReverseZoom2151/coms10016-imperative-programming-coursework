cmake_minimum_required(VERSION 3.20)
project(imperative_programming_toolkit VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_library(imperative_core STATIC src/toolkit.c src/binary.c)
target_compile_options(imperative_core PRIVATE -Wall -Wextra -Wpedantic -Werror)
target_include_directories(imperative_core PUBLIC include)

enable_testing()

add_executable(binary-tool app/binary_main.c)
target_link_libraries(binary-tool PRIVATE imperative_core)

add_executable(binary-tests test/binary_test.c)
target_link_libraries(binary-tests PRIVATE imperative_core)
add_test(NAME binary-tests COMMAND binary-tests)
