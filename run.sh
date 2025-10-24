#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)

UCRT_BIN="${ROOT_DIR}/build-ucrt/steeplejack.exe"
WSL_BIN="${ROOT_DIR}/build-wsl/steeplejack"

RUN_ARGS=("$@")

detect_environment() {
    if [[ "${MSYSTEM:-}" == "UCRT64" ]]; then
        echo "ucrt64"
        return 0
    fi

    if [[ -n "${WSL_DISTRO_NAME:-}" ]]; then
        echo "wsl"
        return 0
    fi

    if [[ -r /proc/version ]] && grep -qi microsoft /proc/version; then
        echo "wsl"
        return 0
    fi

    return 1
}

select_binary() {
    local env_name=$1
    case "${env_name}" in
        ucrt64)
            echo "${UCRT_BIN}"
            ;;
        wsl)
            echo "${WSL_BIN}"
            ;;
        *)
            return 1
            ;;
    esac
}

env_name=$(detect_environment) || {
    echo "Unable to detect environment (expected MSYS2 UCRT64 or WSL2)." >&2
    exit 1
}

binary=$(select_binary "${env_name}")

if [[ ! -x "${binary}" ]]; then
    echo "Executable '${binary}' not found or not executable. Run ./build.sh first." >&2
    exit 1
fi

printf '==> Running %s build: %s\n' "${env_name^^}" "${binary}"

if [[ ${#RUN_ARGS[@]} -gt 0 ]]; then
    exec "${binary}" "${RUN_ARGS[@]}"
else
    exec "${binary}"
fi
