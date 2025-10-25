# Toolchain settings for building with MSYS2 UCRT64 environment using GCC.
# This toolchain is chained from vcpkg's toolchain via VCPKG_CHAINLOAD_TOOLCHAIN_FILE

set(CMAKE_SYSTEM_NAME Windows)

# Set vcpkg triplet for MSYS2 UCRT64
set(VCPKG_TARGET_TRIPLET "x64-mingw-dynamic" CACHE STRING "")
set(VCPKG_HOST_TRIPLET "x64-mingw-dynamic" CACHE STRING "")

if(NOT DEFINED CMAKE_C_COMPILER)
    set(CMAKE_C_COMPILER gcc)
endif()
if(NOT DEFINED CMAKE_CXX_COMPILER)
    set(CMAKE_CXX_COMPILER g++)
endif()

set(CMAKE_MAKE_PROGRAM ninja CACHE FILEPATH "Ninja executable" FORCE)

set(VULKAN_SDK "$ENV{VULKAN_SDK}" CACHE PATH "Path to Vulkan SDK")
