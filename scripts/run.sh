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

RUN_ARGS=("$@")

env_name=$(steeplejack_detect_environment) || {
    echo "Unable to detect environment (expected MSYS2 UCRT64 or WSL2)." >&2
    exit 1
}

binary=$(steeplejack_binary_path "${env_name}" "${CONFIG}")
label=$(steeplejack_env_label "${env_name}")

if [[ ! -x "${binary}" ]]; then
    echo "Executable '${binary}' not found or not executable. Run ./steeplejack.sh build ${CONFIG} first." >&2
    exit 1
fi

printf '==> Running %s %s build: %s\n' "${label}" "${CONFIG}" "${binary}"

if [[ ${#RUN_ARGS[@]} -gt 0 ]]; then
    exec "${binary}" "${RUN_ARGS[@]}"
else
    exec "${binary}"
fi
