# Development Environments

Steeplejack targets **Windows** using the MSYS2 **UCRT64 + GCC** toolchain.

## MSYS2 UCRT64 (primary)

- Produces the shipping Windows binaries (Vulkan requires Windows/Mingw runtime)
- Uses GCC, Ninja, and the Vulkan SDK provided via MSYS2 packages + vcpkg
- Follow [MSYS2](./MSYS2.md) for setup (terminal profiles, packages, env vars)

## WSL2

WSL is not supported for this project.

## Why both?

All tooling should run via MSYS2.

## Bonus: no Visual Studio required

All toolchains are GCC-based with CMake + Ninja — no Visual Studio installation is needed.
