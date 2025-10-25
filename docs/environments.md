# Development Environments

Steeplejack targets **Windows** using the MSYS2 **UCRT64 + GCC** toolchain, but we keep a parallel **WSL2 (Ubuntu)** environment for tooling and CI.

## MSYS2 UCRT64 (primary)

- Produces the shipping Windows binaries (Vulkan requires Windows/Mingw runtime)
- Uses GCC, Ninja, and the Vulkan SDK provided via MSYS2 packages + vcpkg
- Follow [MSYS2](./MSYS2.md) for setup (terminal profiles, packages, env vars)

## WSL2 (supporting)

- Builds the same code with Linux GCC so we can run unit tests, code formatters, and AI coding tools that expect a GNU/Linux shell
- Vulkan runtime is unavailable inside WSL today, so the executable will fail at runtime—limit usage to non-GPU tests and linting
- Follow the standard Ubuntu instructions (install gcc, CMake, Ninja, Vulkan headers) and set `VCPKG_ROOT=/mnt/c/tools/vcpkg`

## Why both?

Many AI-powered CLIs (OpenAI’s codex era tooling, Anthropic Claude, etc.) expect WSL/Unix shells and struggle with MSYS2. WSL2 provides a “neutral” GNU userland for these tools while MSYS2 handles the actual Windows/Vulkan builds. This split also lets CI exercise the non-GPU components on Linux while you keep using Vulkan on Windows.

## Bonus: no Visual Studio required

All toolchains are GCC-based with CMake + Ninja — no Visual Studio installation is needed.
