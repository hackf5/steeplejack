# Scripts

Helper scripts for shader compilation, asset baking, CI setup, etc. The root-level `steeplejack.sh` wrapper delegates to:

- `build.sh` – configures & builds the current platform (MSYS2 UCRT or WSL).
- `clean.sh` – removes generated build products for the selected configuration while keeping `vcpkg` installs.
- `run.sh` – launches the built executable with pass-through CLI args.
- `lib/common.sh` – shared helpers (root path resolution, platform detection, build/binary path utilities).
