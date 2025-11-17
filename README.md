# steeplejack

Experimental C++/Vulkan playground for Windows. Everything builds via MSYS2 UCRT64.

## Prerequisites

- Follow [docs/MSYS2.md](docs/MSYS2.md) to set up MSYS2 UCRT64, compilers, and the required vcpkg defaults.
- Follow [docs/vcpkg.md](docs/vcpkg.md) to bootstrap the package manager.

## Build / Run

Use the project task runner `sj` at the repo root. It targets MSYS2 UCRT64, keeps separate Debug/Release build folders, chains through vcpkg when `VCPKG_ROOT` is set, and preserves dependency caches on clean.

```bash
./sj build [debug|release]   # configure + build (defaults to debug)
./sj test  [debug|release]   # build + run tests for the config
./sj run   [debug|release] -- <args>
./sj clean [debug|release]
```

Extra flags after the config go straight to CMake (for `build`) or the executable (for `run`).

The build uses CMake presets under the hood (`ucrt-debug`/`ucrt-release`). Ensure `VCPKG_ROOT` is set per docs.

## Notes

- Dependencies come from `vcpkg.json`. With `VCPKG_ROOT` set (per the docs), the build script pulls them automatically.
- VS Code users can select any of the MSYS2 presets in `CMakePresets.json` (e.g., `ucrt-debug`) if they prefer the IDE workflow.
- The MSYS2 toolchain defaults to Clang (`cmake/toolchains/msys2-ucrt.cmake`); ensure `mingw-w64-ucrt-x86_64-clang` is installed per the setup guide.
