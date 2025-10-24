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

Requirements: CMake ≥ 3.24, Ninja, GCC (MSYS2 UCRT or WSL2), and the Vulkan SDK (for link headers/libs once the renderer is in place).

### MSYS2 UCRT64

```bash
cmake -S . -B build-ucrt -G Ninja -D CMAKE_TOOLCHAIN_FILE=cmake/toolchains/msys2-ucrt.cmake
cmake --build build-ucrt
```

### WSL2 (non-GPU)

```bash
cmake -S . -B build-wsl -G Ninja -D CMAKE_TOOLCHAIN_FILE=cmake/toolchains/wsl2-gcc.cmake
cmake --build build-wsl
```

### Convenience

`build.sh` auto-detects whether it’s running in MSYS2 UCRT64 or WSL2 and builds the appropriate target.

```bash
./build.sh
```

Any extra flags you pass (for example `-DSTEEPLEJACK_WARNINGS_AS_ERRORS=ON`) are forwarded to the underlying CMake configure step.

### Running

`run.sh` mirrors the same environment detection and runs the most recent build output, forwarding any extra arguments to the executable:

```bash
./run.sh --example-flag
```

The current executable just instantiates a stub `steeplejack::Application`. As Vulkan code lands we will gate GPU-dependent targets so CI/WSL can still run core/unit tests.
