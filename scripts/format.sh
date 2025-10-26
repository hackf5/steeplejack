#!/usr/bin/env bash
# spell-checker: disable

set -euo pipefail

SCRIPT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
source "${SCRIPT_DIR}/lib/common.sh"

ROOT_DIR=$(steeplejack_root)

MODE=${1:-fix}
case "${MODE}" in
  fix|check) ;;
  *)
    echo "Unknown mode '${MODE}'. Use 'fix' (default) or 'check'." >&2
    exit 1
    ;;
esac

if ! command -v clang-format >/dev/null 2>&1; then
  echo "clang-format not found in PATH. On MSYS2 UCRT, install: mingw-w64-ucrt-x86_64-clang-tools-extra" >&2
  exit 127
fi

cd "${ROOT_DIR}"

# Gather tracked C/C++ files
mapfile -d '' FILES < <(git ls-files -z -- '*.[ch]' '*.[ch]pp' '*.[ch]xx' '*.cc')

if [[ ${#FILES[@]} -eq 0 ]]; then
  echo "No C/C++ files found to format."
  exit 0
fi

if [[ "${MODE}" == "check" ]]; then
  # Dry run with Werror to fail on diffs
  printf '%s\0' "${FILES[@]}" | xargs -0 -n 50 clang-format --dry-run --Werror -style=file
  echo "Format check passed."
else
  printf '%s\0' "${FILES[@]}" | xargs -0 -n 50 clang-format -i -style=file
  echo "Formatted ${#FILES[@]} files."
fi

