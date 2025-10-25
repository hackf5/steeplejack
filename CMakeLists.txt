cmake_minimum_required(VERSION 3.24)

project(steeplejack VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Ensure runtime output (for assets/shaders) is the build dir
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

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

# Dependencies via vcpkg
find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)
find_package(spdlog REQUIRED)
find_package(glm REQUIRED)
find_package(vk-bootstrap REQUIRED)
find_package(VulkanMemoryAllocator CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

# Source includes for levin headers
target_include_directories(steeplejack_engine PRIVATE ${PROJECT_SOURCE_DIR}/src)

# Link common libs
target_link_libraries(steeplejack_engine
    PRIVATE
        Vulkan::Vulkan
        glfw
        spdlog::spdlog
        glm::glm
        vk-bootstrap::vk-bootstrap
        GPUOpen::VulkanMemoryAllocator
        OpenGL::GL
        imgui::imgui
        ${CMAKE_DL_LIBS}
)

# Add levin sources (exclude any main.cpp)
file(GLOB_RECURSE LEVIN_SOURCES CONFIGURE_DEPENDS
    ${PROJECT_SOURCE_DIR}/src/vulkan/*.cpp
    ${PROJECT_SOURCE_DIR}/src/gui/*.cpp
    ${PROJECT_SOURCE_DIR}/src/model/*.cpp
    ${PROJECT_SOURCE_DIR}/src/scenes/*.cpp
    ${PROJECT_SOURCE_DIR}/src/vulkan_*context*.cpp
    ${PROJECT_SOURCE_DIR}/src/vulkan_engine.cpp
)
list(REMOVE_ITEM LEVIN_SOURCES ${PROJECT_SOURCE_DIR}/src/main.cpp)
target_sources(steeplejack_engine PRIVATE ${LEVIN_SOURCES})
target_sources(steeplejack_engine PRIVATE ${PROJECT_SOURCE_DIR}/src/third_party/stb_image.cpp)


# Bring in imgui and assets
find_package(OpenGL REQUIRED)
add_executable(steeplejack
    src/main.cpp
)
target_link_libraries(steeplejack PRIVATE steeplejack_engine)
target_compile_features(steeplejack PUBLIC cxx_std_23)
steeplejack_enable_warnings(steeplejack)

# Build shaders with glslangValidator into ${CMAKE_BINARY_DIR}/shaders
add_subdirectory(shaders)

# Assets after targets exist so add_dependencies() resolves
add_subdirectory(assets)

if (STEEPLEJACK_BUILD_TESTS)
    include(CTest)
    enable_testing()
    add_subdirectory(tests)
endif()
