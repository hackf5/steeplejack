#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
source "${SCRIPT_DIR}/lib/common.sh"

CONFIG=${1:-debug}
shift || true
case "${CONFIG}" in
    debug|release) ;;
    *) echo "Unknown configuration '${CONFIG}'. Use debug or release." >&2; exit 1 ;;
esac

ROOT_DIR=$(steeplejack_root)
env_name=$(steeplejack_detect_environment) || {
    echo "Unable to detect environment (expected MSYS2 UCRT64 or WSL2)." >&2
    exit 1
}

build_dir=$(steeplejack_build_dir "${env_name}" "${CONFIG}")

if [[ ! -f "${build_dir}/CMakeCache.txt" ]]; then
  echo "Config not found for ${CONFIG}; running initial build..."
  "${SCRIPT_DIR}/build.sh" "${CONFIG}"
fi

echo "==> Building tests"
cmake --build "${build_dir}" --target steeplejack_tests

echo "==> Running tests"
ctest --test-dir "${build_dir}" --output-on-failure

