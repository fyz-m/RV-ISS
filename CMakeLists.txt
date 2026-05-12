cmake_minimum_required(VERSION 3.28.3)

project(RV32-ISS LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# CPU core library
add_library(iss_core
    src/cpu.cpp
    src/memory.cpp
    src/decode.cpp
    src/execute.cpp
)
target_include_directories(iss_core PUBLIC src/headers)
target_compile_options(iss_core PRIVATE
    -Wall -Wextra -Wpedantic -Werror -fsanitize=address,undefined 
    $<$<CONFIG:Debug>:  -g3 -O0>
    $<$<CONFIG:Release>: -O2>
)
target_link_options(iss_core PRIVATE -fsanitize=address,undefined)

# ── Simulator executable ──────────────────────────────────────────────────────
add_executable(rv32_sim src/main.cpp)
target_link_libraries(rv32_sim PRIVATE iss_core)
target_compile_options(rv32_sim PRIVATE -fsanitize=address,undefined)
target_link_options(rv32_sim PRIVATE -fsanitize=address,undefined)



# ── GoogleTest ────────────────────────────────────────────────────────────────
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/52eb8108c5bdec04579160ae17225d66034bd723.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

# ── Test executable ───────────────────────────────────────────────────────────

add_library(iss_core_test
    src/cpu.cpp
    src/memory.cpp
    src/decode.cpp
    src/execute.cpp
)

target_include_directories(iss_core_test PUBLIC src/headers)
target_compile_options(iss_core_test PRIVATE
    -Wall -Wextra -Wpedantic -Werror -g3 -O0
)

add_executable(iss_tests 
               tests/test_decode.cpp
               tests/test_execute.cpp
               tests/test_memory.cpp
               )
            
target_link_libraries(iss_tests PRIVATE iss_core_test GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(iss_tests)