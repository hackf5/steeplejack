#!/usr/bin/env bash
# spell-checker: disable

set -euo pipefail

SCRIPT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
source "${SCRIPT_DIR}/lib/common.sh"

ROOT_DIR=$(steeplejack_root)

CONFIG=${1:-debug}
case "${CONFIG}" in
    debug|release)
        ;;
    *)
        echo "Unknown configuration '${CONFIG}'. Use debug or release." >&2
        exit 1
        ;;
esac

clean_tree() {
    local dir=$1
    if [[ -d "${dir}" ]]; then
        echo "Cleaning ${dir}"
        find "${dir}" -mindepth 1 -maxdepth 1 ! -name 'vcpkg_installed' -exec rm -rf {} +
        find "${dir}" -name 'CMakeCache.txt' -delete
        rmdir --ignore-fail-on-non-empty "${dir}" >/dev/null 2>&1 || true
    else
        echo "No build directory at ${dir} (nothing to clean)."
    fi
}

env_name=$(steeplejack_detect_environment) || {
    echo "Unable to detect environment (expected MSYS2 UCRT64 or WSL2)." >&2
    exit 1
}

build_dir=$(steeplejack_build_dir "${env_name}" "${CONFIG}")
clean_tree "${build_dir}"

echo "Clean complete."
