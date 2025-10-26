#!/usr/bin/env bash
# spell-checker: disable

set -euo pipefail

SCRIPT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
source "${SCRIPT_DIR}/lib/common.sh"

MODE=${1:-run}
case "${MODE}" in
  run|fix) ;;
  *)
    echo "Unknown mode '${MODE}'. Use 'run' (default) or 'fix'." >&2
    exit 1
    ;;
esac

if ! command -v clang-tidy >/dev/null 2>&1; then
  echo "clang-tidy not found in PATH. On MSYS2 UCRT, install: mingw-w64-ucrt-x86_64-clang-tools-extra" >&2
  exit 127
fi

ROOT_DIR=$(steeplejack_root)
env_name=$(steeplejack_detect_environment) || {
  echo "Unable to detect environment (expected MSYS2 UCRT64)." >&2
  exit 1
}

# Prefer Debug build directory for compile_commands.json
build_dir=$(steeplejack_build_dir "${env_name}" "debug")
compdb="${build_dir}/compile_commands.json"
if [[ ! -f "${compdb}" ]]; then
  echo "compile_commands.json not found at ${compdb}. Run './sj build debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON' first." >&2
  exit 1
fi

cd "${ROOT_DIR}"

# Gather C++ source files tracked by git that are likely in the compile database
mapfile -d '' FILES < <(git ls-files -z -- '*.cpp' '*.cc' '*.cxx')
if [[ ${#FILES[@]} -eq 0 ]]; then
  echo "No C++ source files found."
  exit 0
fi

cmd=(clang-tidy -p "${build_dir}")
if [[ "${MODE}" == "fix" ]]; then
  cmd+=(--fix --format --format-style=file)
fi

printf '%s\0' "${FILES[@]}" | xargs -0 -n 20 "${cmd[@]}"

echo "clang-tidy ${MODE} completed."

