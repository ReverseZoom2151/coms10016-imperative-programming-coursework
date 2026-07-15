cmake_minimum_required(VERSION 3.20)
project(imperative_programming_toolkit VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(TOOLKIT_ENABLE_SANITIZERS "Enable AddressSanitizer and UBSan" OFF)
option(TOOLKIT_ENABLE_COVERAGE "Enable GCC/Clang coverage instrumentation" OFF)

add_library(imperative_core STATIC
    src/toolkit.c
    src/binary.c
    src/list.c
    src/generic_list.c
    src/sketch.c
)
target_compile_options(imperative_core PUBLIC -Wall -Wextra -Wpedantic -Werror)
target_include_directories(imperative_core PUBLIC include)

if(TOOLKIT_ENABLE_SANITIZERS AND CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(imperative_core PUBLIC -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(imperative_core INTERFACE -fsanitize=address,undefined)
endif()

if(TOOLKIT_ENABLE_COVERAGE AND CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(imperative_core PUBLIC --coverage)
    target_link_options(imperative_core INTERFACE --coverage)
endif()

enable_testing()

add_executable(binary-tool app/binary_main.c)
target_link_libraries(binary-tool PRIVATE imperative_core)

add_executable(binary-tests test/binary_test.c)
target_link_libraries(binary-tests PRIVATE imperative_core)
add_test(NAME binary-tests COMMAND binary-tests)

add_executable(list-tests test/list_test.c)
target_link_libraries(list-tests PRIVATE imperative_core)
add_test(NAME list-tests COMMAND list-tests)

add_executable(generic-list-tests test/generic_list_test.c)
target_link_libraries(generic-list-tests PRIVATE imperative_core)
add_test(NAME generic-list-tests COMMAND generic-list-tests)

add_executable(sketch-view app/sketch_view_main.c)
target_link_libraries(sketch-view PRIVATE imperative_core)

add_executable(sketch-pgm app/sketch_pgm_main.c)
target_link_libraries(sketch-pgm PRIVATE imperative_core)

add_executable(sketch-inspect app/sketch_inspect_main.c)
target_link_libraries(sketch-inspect PRIVATE imperative_core)

add_executable(sketch-svg app/sketch_svg_main.c)
target_link_libraries(sketch-svg PRIVATE imperative_core)

add_executable(sketch-gif app/sketch_gif_main.c)
target_link_libraries(sketch-gif PRIVATE imperative_core)

add_executable(sketch-tests test/sketch_test.c)
target_link_libraries(sketch-tests PRIVATE imperative_core)
add_test(NAME sketch-tests COMMAND sketch-tests)

add_executable(robustness-tests test/robustness_test.c)
target_link_libraries(robustness-tests PRIVATE imperative_core)
add_test(NAME robustness-tests COMMAND robustness-tests)
