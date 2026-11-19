cmake_minimum_required(VERSION 3.22)
project(Pokemon_battle_lab LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Shared build settings for our targets.
add_library(project_options INTERFACE)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(project_options INTERFACE
        -Wall -Wextra -Wpedantic
        $<$<CONFIG:Debug>:-fsanitize=address,undefined>
        $<$<CONFIG:Debug>:-fno-omit-frame-pointer>
    )
    target_link_options(project_options INTERFACE
        $<$<CONFIG:Debug>:-fsanitize=address,undefined>
    )
endif()

add_library(engine STATIC src/engine/engine.cpp)
target_include_directories(engine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_link_libraries(engine PRIVATE project_options)

add_executable(battle_cli src/cli/main.cpp)
target_link_libraries(battle_cli PRIVATE engine project_options)

include(CTest)
if(BUILD_TESTING)
    include(FetchContent)
    FetchContent_Declare(googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
    )
    # Use extraction timestamps with newer CMake versions.
    if(POLICY CMP0135)
        cmake_policy(SET CMP0135 NEW)
    endif()
    FetchContent_MakeAvailable(googletest)

    add_executable(engine_tests tests/engine_test.cpp)
    target_link_libraries(engine_tests PRIVATE engine project_options GTest::gtest_main)

    include(GoogleTest)
    gtest_discover_tests(engine_tests)
endif()
