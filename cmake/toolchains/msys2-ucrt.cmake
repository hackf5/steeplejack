# Toolchain settings for building with MSYS2 UCRT64 environment using GCC.
# Invoke CMake with: cmake -S . -B build-msys -G "Ninja" -D CMAKE_TOOLCHAIN_FILE=cmake/toolchains/msys2-ucrt.cmake

set(CMAKE_SYSTEM_NAME Windows)

if(NOT DEFINED CMAKE_C_COMPILER)
    set(CMAKE_C_COMPILER gcc)
endif()
if(NOT DEFINED CMAKE_CXX_COMPILER)
    set(CMAKE_CXX_COMPILER g++)
endif()

set(CMAKE_MAKE_PROGRAM ninja CACHE FILEPATH "Ninja executable" FORCE)

set(VULKAN_SDK "$ENV{VULKAN_SDK}" CACHE PATH "Path to Vulkan SDK")
