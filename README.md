# steeplejack

Experimental C++/Vulkan playground targeting Windows (MSYS2 UCRT) with optional WSL2 builds for non-GPU checks.

## Layout

- `src/` – engine + game sources (`core/`, `renderer/`, `platform/windows/`, `game/`).
- `include/` – public headers for engine modules.
- `shaders/` – GLSL/HLSL plus compiled SPIR-V in `shaders/bin/`.
- `assets/` – pipeline inputs (textures, models, audio).
- `external/` – vendored third-party libraries with license notes.
- `cmake/` – helper modules + toolchains (`msys2-ucrt.cmake`, `wsl2-gcc.cmake`).
- `scripts/`, `tools/`, `tests/`, `docs/` – automation, utilities, tests, and design notes.

## Building

Requirements: CMake ≥ 3.24, Ninja, and a compiler with `-std=c++2b`/C++26 support (GCC 13+/Clang 17+) plus the Vulkan SDK (for link headers/libs once the renderer is in place). The build system automatically adds `/std:c++latest` (MSVC) or `-std=gnu++2b` (GCC/Clang) so you always compile against the upcoming standard.

### MSYS2 UCRT64

```bash
cmake -S . -B build/ucrt -G Ninja -D CMAKE_TOOLCHAIN_FILE=cmake/toolchains/msys2-ucrt.cmake
cmake --build build/ucrt
```

### WSL2 (non-GPU)

```bash
cmake -S . -B build/wsl -G Ninja -D CMAKE_TOOLCHAIN_FILE=cmake/toolchains/wsl2-gcc.cmake
cmake --build build/wsl
```

### VS Code / Presets

`CMakePresets.json` defines four configurations (`ucrt-debug`, `ucrt-release`, `wsl-debug`, `wsl-release`). VS Code (CMake Tools) and CLI builds can reference them directly; the release presets also turn on `-O3 -march=native`.

```bash
cmake --preset wsl-debug
cmake --build --preset wsl-debug
```

### CLI helper

`steeplejack.sh` is a wrapper that auto-detects MSYS2 UCRT vs WSL2, selects the matching toolchain, and keeps separate Debug/Release build folders:

```bash
# Build (config defaults to debug; pass extra CMake cache args afterward)
./steeplejack.sh build [debug|release] -DSTEEPLEJACK_WARNINGS_AS_ERRORS=ON

# Clean the chosen configuration (keeps vcpkg-installed deps)
./steeplejack.sh clean [debug|release]
```

### vcpkg integration

Dependency management is handled through `vcpkg.json` (GLFW, GLM, spdlog, stb, Vulkan Memory Allocator).

1. Clone vcpkg once somewhere on the Windows filesystem (e.g. `git clone https://github.com/microsoft/vcpkg C:\tools\vcpkg`). Both MSYS2 (`/c/tools/vcpkg`) and WSL (`/mnt/c/tools/vcpkg`) can reference that same checkout.
2. Bootstrap it (`/mnt/c/tools/vcpkg/bootstrap-vcpkg.sh` inside WSL, `C:\tools\vcpkg\bootstrap-vcpkg.bat` in MSYS2/Windows).
3. Set `VCPKG_ROOT` in each environment to the translated path (`export VCPKG_ROOT=/mnt/c/tools/vcpkg` in WSL, `export VCPKG_ROOT=/c/tools/vcpkg` in MSYS2). That way both shells share downloads/installed packages.
4. For MSYS2 UCRT builds, define the default MinGW triplets in your shell profile (e.g. `~/.bashrc` or `~/.zshrc`):

   ```bash
   export VCPKG_DEFAULT_TRIPLET=x64-mingw-dynamic
   export VCPKG_DEFAULT_HOST_TRIPLET=x64-mingw-dynamic
   ```

   (WSL doesn’t need extra settings—its default `x64-linux` triplets already match what we expect.)
5. Run `./steeplejack.sh build [debug|release]` or a preset. When `VCPKG_ROOT` is set, the script automatically chains the platform toolchain file through vcpkg’s toolchain so manifest dependencies restore on first configure.

To drive CMake manually with vcpkg, pass the vcpkg toolchain and chainload ours, for example:

```bash
cmake -S . -B build/ucrt -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" \
  -DVCPKG_CHAINLOAD_TOOLCHAIN_FILE=cmake/toolchains/msys2-ucrt.cmake \
  -DVCPKG_TARGET_TRIPLET=x64-mingw-static
```

The manifest currently pins `builtin-baseline` `271a5b8850aa50f9a40269cbf3cf414b36e333d6`; update it whenever you intentionally roll the dependency set forward (`git -C "$VCPKG_ROOT" rev-parse HEAD` is a handy way to capture the version you want to standardize on).

### Running

`./steeplejack.sh run [debug|release] -- <game args>` mirrors the same environment detection and runs the most recent build output, forwarding any extra arguments to the executable:

```bash
./steeplejack.sh run release --example-flag
```

The current executable just instantiates a stub `steeplejack::Application`. As Vulkan code lands we will gate GPU-dependent targets so CI/WSL can still run core/unit tests.
