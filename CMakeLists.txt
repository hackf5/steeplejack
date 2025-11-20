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
    set(STEEPLEJACK_CXX_LATEST_FLAG "-std=gnu++23")
endif()

if (STEEPLEJACK_CXX_LATEST_FLAG)
    add_compile_options(${STEEPLEJACK_CXX_LATEST_FLAG})
endif()

add_library(steeplejack_engine STATIC
    src/application.cpp
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

include(CheckIPOSupported)
check_ipo_supported(RESULT STEEPLEJACK_IPO_SUPPORTED OUTPUT STEEPLEJACK_IPO_ERROR)
if (NOT STEEPLEJACK_IPO_SUPPORTED)
    message(WARNING "Interprocedural optimization (LTO) not supported: ${STEEPLEJACK_IPO_ERROR}")
endif()

# Dependencies via vcpkg
find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)
find_package(spdlog REQUIRED)
find_package(glm REQUIRED)
find_package(vk-bootstrap REQUIRED)
find_package(VulkanMemoryAllocator CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)

# Source includes for steeplejack headers
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
        imgui::imgui
        EnTT::EnTT
        ${CMAKE_DL_LIBS}
)

# Add steeplejack sources (exclude any main.cpp)
file(GLOB_RECURSE STEEPLEJACK_SOURCES CONFIGURE_DEPENDS
    ${PROJECT_SOURCE_DIR}/src/vulkan/*.cpp
    ${PROJECT_SOURCE_DIR}/src/gui/*.cpp
    ${PROJECT_SOURCE_DIR}/src/model/*.cpp
    ${PROJECT_SOURCE_DIR}/src/scenes/*.cpp
    ${PROJECT_SOURCE_DIR}/src/config/*.cpp
    ${PROJECT_SOURCE_DIR}/src/util/*.cpp
    ${PROJECT_SOURCE_DIR}/src/vulkan_*context*.cpp
    ${PROJECT_SOURCE_DIR}/src/vulkan_engine.cpp
)
file(GLOB_RECURSE STEEPLEJACK_ECS_SOURCES CONFIGURE_DEPENDS
    ${PROJECT_SOURCE_DIR}/src/ecs/*.cpp
)
list(REMOVE_ITEM STEEPLEJACK_SOURCES ${PROJECT_SOURCE_DIR}/src/main.cpp)
target_sources(steeplejack_engine PRIVATE ${STEEPLEJACK_SOURCES} ${STEEPLEJACK_ECS_SOURCES})
target_sources(steeplejack_engine PRIVATE ${PROJECT_SOURCE_DIR}/src/tiny_gltf.cpp)


# Bring in assets
add_executable(steeplejack
    src/main.cpp
)
target_link_libraries(steeplejack PRIVATE steeplejack_engine)
target_compile_features(steeplejack PUBLIC cxx_std_23)
steeplejack_enable_warnings(steeplejack)

if (STEEPLEJACK_IPO_SUPPORTED)
    set_property(TARGET steeplejack_engine PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
    set_property(TARGET steeplejack PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
endif()

# Build shaders with glslangValidator into ${CMAKE_BINARY_DIR}/shaders
add_subdirectory(shaders)

# Assets after targets exist so add_dependencies() resolves
add_subdirectory(assets)

# Copy runtime configuration into the build directory
set(STEEPLEJACK_CONFIG_SOURCE_DIR ${PROJECT_SOURCE_DIR}/config)
set(STEEPLEJACK_CONFIG_OUTPUT_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/config)
add_custom_target(steeplejack_config
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${STEEPLEJACK_CONFIG_SOURCE_DIR} ${STEEPLEJACK_CONFIG_OUTPUT_DIR}
    COMMENT "Copying config files"
)
add_dependencies(steeplejack steeplejack_config)

if (STEEPLEJACK_BUILD_TESTS)
    include(CTest)
    enable_testing()
    add_subdirectory(tests)
endif()
