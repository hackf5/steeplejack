# vcpkg Setup (MSYS2 UCRT64)

Steeplejack uses vcpkg’s manifest mode to fetch third-party libraries (GLFW, GLM, spdlog, stb, VMA, …).
This guide is MSYS2‑specific and uses a shared checkout at `/c/tools/vcpkg` so dependency caches persist across builds.

## Install vcpkg

1. Clone once from the MSYS2 UCRT64 shell:

   ```bash
   mkdir -p /c/tools
   cd /c/tools
   git clone https://github.com/microsoft/vcpkg
   ```

2. Bootstrap the executable once (still in MSYS2 UCRT64):

   ```bash
   /c/tools/vcpkg/bootstrap-vcpkg.sh
   ```

## Environment variables

Point your MSYS2 environment at the shared checkout so the helper scripts can find vcpkg’s toolchain file.

- **MSYS2 UCRT64** (`~/.zshrc`):

  ```bash
  export VCPKG_ROOT=/c/tools/vcpkg
  ```

## Installing dependencies

You don’t need to call vcpkg manually—`./sj build ...` runs CMake with `vcpkg.json` and installs the manifest dependencies on first configure.

## Updating the baseline

The repo pins `builtin-baseline` `271a5b8850aa50f9a40269cbf3cf414b36e333d6`. When you update vcpkg and want to move the baseline forward:

1. `git -C $VCPKG_ROOT pull` to grab the latest tool/ports.
2. Pick the commit you want to standardize on (`git -C $VCPKG_ROOT rev-parse HEAD`).
3. Update `vcpkg.json`’s `builtin-baseline` field and commit the change alongside any new package versions.

## Troubleshooting

- **Missing `versions/baseline.json`**: make sure your local vcpkg clone actually contains the requested baseline (`git -C $VCPKG_ROOT fetch --all` or re-clone).
- **Triplet mismatch**: the CMake toolchain hardcodes the project triplets. If you see cache files for the wrong variant, clear your build directory (`./sj clean`) and reconfigure.

Once vcpkg is configured, the `sj` task runner takes care of calling the right triplet/toolchain, so you can focus on writing code.
