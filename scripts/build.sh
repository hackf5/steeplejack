#!/usr/bin/env bash
# spell-checker: disable

set -euo pipefail

SCRIPT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
source "${SCRIPT_DIR}/lib/common.sh"

ROOT_DIR=$(steeplejack_root)

CONFIG=${1:-debug}
shift || true
case "${CONFIG}" in
    debug|release)
        ;;
    *)
        echo "Unknown configuration '${CONFIG}'. Use debug or release." >&2
        exit 1
        ;;
esac

CONFIG_ARGS=("$@")

env_name=$(steeplejack_detect_environment) || {
    echo "Unable to detect environment (expected MSYS2 UCRT64)." >&2
    exit 1
}

label=$(steeplejack_env_label "${env_name}")

# Map config to CMake preset
case "${CONFIG}" in
    debug) preset="ucrt-debug" ;;
    release) preset="ucrt-release" ;;
esac

printf '==> Configuring %s via preset: %s\n' "${label}" "${preset}"
cmake --preset "${preset}" ${CONFIG_ARGS:+"${CONFIG_ARGS[@]}"}

printf '==> Building %s via preset: %s\n' "${label}" "${preset}"
cmake --build --preset "${preset}"
