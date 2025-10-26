#!/usr/bin/env bash
# spell-checker: disable

set -euo pipefail

SCRIPT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
source "${SCRIPT_DIR}/lib/common.sh"

MODE=fix
FULL=0
# Parse args: first positional can be mode; optional --all to lint all files
if [[ ${1:-} == "check" || ${1:-} == "fix" ]]; then
  MODE=$1
  shift || true
fi
for arg in "$@"; do
  case "$arg" in
    --all|all)
      FULL=1
      ;;
  esac
done

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

# Gather C++ source files
if [[ ${FULL} -eq 1 ]]; then
  mapfile -d '' FILES < <(git ls-files -z -- '*.cpp' '*.cc' '*.cxx')
else
  # Determine a reasonable base for changed files
  if git rev-parse --abbrev-ref --symbolic-full-name @{upstream} >/dev/null 2>&1; then
    upstream_ref=$(git rev-parse --abbrev-ref --symbolic-full-name @{upstream})
    base_commit=$(git merge-base HEAD "${upstream_ref}")
  elif git show-ref --verify --quiet refs/heads/main; then
    base_commit=$(git merge-base HEAD main)
  elif git show-ref --verify --quiet refs/remotes/origin/main; then
    base_commit=$(git merge-base HEAD origin/main)
  else
    # Fallback to initial commit
    base_commit=$(git rev-list --max-parents=0 HEAD | tail -n1)
  fi
  # Collect unstaged, staged, committed-changed, and new files; unique the list (zero-terminated safe)
  mapfile -d '' FILES < <({
      git diff -z --name-only --diff-filter=ACMR HEAD -- '*.cpp' '*.cc' '*.cxx' ;
      git diff -z --name-only --diff-filter=ACMR "${base_commit}...HEAD" -- '*.cpp' '*.cc' '*.cxx' ;
      git diff -z --name-only --cached --diff-filter=ACMR -- '*.cpp' '*.cc' '*.cxx' ;
      git ls-files -z --others --exclude-standard -- '*.cpp' '*.cc' '*.cxx' ;
    } | sort -zu)
  if [[ ${#FILES[@]} -eq 0 ]]; then
    echo "No changed C++ files. Use '--all' for full lint."
    exit 0
  fi
fi
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

# Run and simplify output: drop noise, show filenames instead of long runner lines
{
  "${cmd[@]}" "${FILES[@]}"
} 2>&1 | awk '
  { sub(/\r$/, ""); }
  /:[0-9]+:[0-9]+: (warning|error): / { print "\xE2\x9D\x8C", $0; next }
  /^Suppressed [0-9]+ warnings/ { next }
  /^Use -header-filter=/ { next }
  /^[[:space:]]*[0-9]+ warnings generated\.$/ { next }
  /^\[/ { print $NF; next }
  { print }
'

echo "clang-tidy ${MODE} completed."
