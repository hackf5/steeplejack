#!/usr/bin/env bash
# spell-checker: disable

set -euo pipefail

SCRIPT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
source "${SCRIPT_DIR}/lib/common.sh"

MODE=${1:-fix}
case "${MODE}" in
  check|fix) ;;
  *)
    echo "Unknown mode '${MODE}'. Use 'fix' (default) or 'check'." >&2
    exit 1
    ;;
esac

if ! command -v run-clang-tidy.py >/dev/null 2>&1 && ! command -v run-clang-tidy >/dev/null 2>&1; then
  echo "run-clang-tidy not found. On MSYS2 UCRT, install: mingw-w64-ucrt-x86_64-clang-tools-extra" >&2
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

# Compute parallel jobs (prefer nproc, then getconf)
jobs=$(nproc 2>/dev/null || getconf _NPROCESSORS_ONLN 2>/dev/null || echo 4)

# Prefer run-clang-tidy if available for native parallel execution
if command -v run-clang-tidy.py >/dev/null 2>&1; then
  runner=run-clang-tidy.py
else
  runner=run-clang-tidy
fi

cmd=("${runner}" -p "${build_dir}" -j "${jobs}" -header-filter='^(src|include)/' -quiet)
if [[ "${MODE}" == "fix" ]]; then
  cmd+=( -fix -format )
fi
# Forward options to clang-tidy: -quiet and disable compiler warnings
cmd+=( -- -quiet -extra-arg=-w )

# Run and strip tidy's suppression hints to reduce noise
{
  "${cmd[@]}" "${FILES[@]}"
} 2>&1 | sed -u \
  -e '/^Suppressed [0-9]\+ warnings/d' \
  -e '/^Use -header-filter=.*/d' \
  -e '/^[0-9]\+ warnings generated\.$/d'

echo "clang-tidy ${MODE} completed."
