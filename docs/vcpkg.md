# vcpkg Setup

Steeplejack uses vcpkg’s manifest mode to fetch third-party libraries (GLFW, GLM, spdlog, stb, VMA, …). This guide walks through the shared checkout approach so both MSYS2 UCRT64 and WSL2 reuse the same dependency cache.

## Install vcpkg

1. Clone once on the Windows side (adjust the path if you prefer another drive):

   ```powershell
   git clone https://github.com/microsoft/vcpkg C:\tools\vcpkg
   ```

2. Bootstrap the executable once from the Windows/MSYS2 side:

   ```bash
   /c/tools/vcpkg/bootstrap-vcpkg.bat
   ```

   WSL can invoke the resulting `vcpkg.exe` via `/mnt/c/tools/vcpkg/vcpkg.exe`, so no second bootstrap is required.

## Environment variables

Point each environment at the shared checkout so the helper scripts can find vcpkg’s toolchain file.

- **MSYS2 UCRT64** (`~/.zshrc`):

  ```bash
  export VCPKG_ROOT=/c/tools/vcpkg
  export VCPKG_DEFAULT_TRIPLET=x64-mingw-dynamic
  export VCPKG_DEFAULT_HOST_TRIPLET=x64-mingw-dynamic
  ```

- **WSL2** (`~/.bashrc` or `~/.zshrc`):

  ```bash
  export VCPKG_ROOT=/mnt/c/tools/vcpkg
  # Defaults already point at x64-linux, so no extra triplet overrides needed.
  ```

## Installing dependencies

You don’t need to call vcpkg manually—`./sj.sh build ...` runs CMake with `vcpkg.json`, which automatically installs the manifest dependencies on first configure for each triplet.

If you prefer to prefetch them (e.g., before heading offline), run:

```bash
# Inside MSYS2 UCRT64
$VCPKG_ROOT/vcpkg install --triplet x64-mingw-dynamic

# Inside WSL2
$VCPKG_ROOT/vcpkg install --triplet x64-linux
```

## Updating the baseline

The repo pins `builtin-baseline` `271a5b8850aa50f9a40269cbf3cf414b36e333d6`. When you update vcpkg and want to move the baseline forward:

1. `git -C $VCPKG_ROOT pull` to grab the latest tool/ports.
2. Pick the commit you want to standardize on (`git -C $VCPKG_ROOT rev-parse HEAD`).
3. Update `vcpkg.json`’s `builtin-baseline` field and commit the change alongside any new package versions.

## Troubleshooting

- **Missing `versions/baseline.json`**: make sure your local vcpkg clone actually contains the requested baseline (`git -C $VCPKG_ROOT fetch --all` or re-clone).
- **Triplet mismatch**: if CMake complains about `*_x64-mingw-static.list` files while you expect dynamic builds, confirm `VCPKG_DEFAULT_TRIPLET` is set in your MSYS shell (and restart the terminal so the variable is active).

Once vcpkg is configured, the `sj.sh` task runner takes care of calling the right triplet/toolchain, so you can focus on writing code.
