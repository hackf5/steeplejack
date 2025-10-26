# steeplejack

Experimental C++/Vulkan playground for Windows. Everything builds via MSYS2 UCRT64; WSL2 is optional for tooling/tests (no Vulkan runtime there).

## Prerequisites

- Follow [docs/MSYS2.md](docs/MSYS2.md) to set up MSYS2 UCRT64, compilers, and the required vcpkg defaults.
- Follow [docs/vcpkg.md](docs/vcpkg.md) to bootstrap the package manager.
- (Optional) Read [docs/environments.md](docs/environments.md) if you want the WSL2 helper environment for tooling/tests.

## Build / Run

Use the project task runner `sj.sh` at the repo root. It detects the host (MSYS2 UCRT vs WSL2), selects the right toolchain, keeps separate Debug/Release build folders, chains through vcpkg when `VCPKG_ROOT` is set, and preserves dependency caches on clean.

```bash
./sj.sh build [debug|release]   # configure + build (defaults to debug)
./sj.sh test  [debug|release]   # build + run tests for the config
./sj.sh run   [debug|release] -- <args>
./sj.sh clean [debug|release]
```

Extra flags after the config go straight to CMake (for `build`) or the executable (for `run`).

## Notes

- Dependencies come from `vcpkg.json`. With `VCPKG_ROOT` set (per the docs), the build script pulls them automatically.
- VS Code users can select any of the MSYS2 presets in `CMakePresets.json` (e.g., `ucrt-debug`) if they prefer the IDE workflow.
