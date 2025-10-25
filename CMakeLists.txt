cmake_minimum_required(VERSION 3.24)

project(steeplejack VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(STEEPLEJACK_BUILD_TESTS "Enable building test targets" ON)
option(STEEPLEJACK_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(STEEPLEJACK_CXX_LATEST_FLAG "")
if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(STEEPLEJACK_CXX_LATEST_FLAG "/std:c++latest")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(STEEPLEJACK_CXX_LATEST_FLAG "-std=gnu++2b")
endif()

if (STEEPLEJACK_CXX_LATEST_FLAG)
    add_compile_options(${STEEPLEJACK_CXX_LATEST_FLAG})
endif()

add_library(steeplejack_engine STATIC
    src/core/Application.cpp
)
target_include_directories(steeplejack_engine
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)
target_compile_features(steeplejack_engine PUBLIC cxx_std_23)

function(steeplejack_enable_warnings target)
    if (MSVC)
        target_compile_options(${target} PRIVATE /W4)
        if (STEEPLEJACK_WARNINGS_AS_ERRORS)
            target_compile_options(${target} PRIVATE /WX)
        endif()
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
        if (STEEPLEJACK_WARNINGS_AS_ERRORS)
            target_compile_options(${target} PRIVATE -Werror)
        endif()
    endif()
endfunction()

steeplejack_enable_warnings(steeplejack_engine)

add_executable(steeplejack
    src/main.cpp
)
target_link_libraries(steeplejack PRIVATE steeplejack_engine)
target_compile_features(steeplejack PUBLIC cxx_std_23)
steeplejack_enable_warnings(steeplejack)

if (STEEPLEJACK_BUILD_TESTS)
    enable_testing()
    # add_subdirectory(tests) # Populate when tests exist.
endif()
