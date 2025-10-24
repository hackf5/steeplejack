# Toolchain for building inside WSL2 with system GCC + Ninja.
# Usage: cmake -S . -B build-wsl -G Ninja -D CMAKE_TOOLCHAIN_FILE=cmake/toolchains/wsl2-gcc.cmake

set(CMAKE_SYSTEM_NAME Linux)

if(NOT DEFINED CMAKE_C_COMPILER)
    set(CMAKE_C_COMPILER gcc)
endif()
if(NOT DEFINED CMAKE_CXX_COMPILER)
    set(CMAKE_CXX_COMPILER g++)
endif()

set(CMAKE_MAKE_PROGRAM ninja CACHE FILEPATH "Ninja executable" FORCE)

set(VULKAN_SDK "$ENV{VULKAN_SDK}" CACHE PATH "Path to Vulkan SDK")
