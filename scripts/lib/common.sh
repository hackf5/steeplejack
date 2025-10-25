#!/usr/bin/env bash
# spell-checker: disable

if [[ -z "${STEEPLEJACK_ROOT:-}" ]]; then
    STEEPLEJACK_ROOT=$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)
fi

steeplejack_root() {
    echo "${STEEPLEJACK_ROOT}"
}

steeplejack_detect_environment() {
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

steeplejack_env_label() {
    case "$1" in
        ucrt64) echo "MSYS2 UCRT64" ;;
        wsl) echo "WSL2" ;;
        *) echo "unknown" ;;
    esac
}

steeplejack_toolchain_file() {
    local env=$1
    local root
    root=$(steeplejack_root)
    case "${env}" in
        ucrt64) echo "${root}/cmake/toolchains/msys2-ucrt.cmake" ;;
        wsl) echo "${root}/cmake/toolchains/wsl2-gcc.cmake" ;;
        *) return 1 ;;
    esac
}

steeplejack_build_dir() {
    local env=$1
    local config=$2
    local root
    root=$(steeplejack_root)
    case "${env}" in
        ucrt64) echo "${root}/build/ucrt/${config}" ;;
        wsl) echo "${root}/build/wsl/${config}" ;;
        *) return 1 ;;
    esac
}

steeplejack_binary_path() {
    local env=$1
    local config=$2
    local build_dir
    build_dir=$(steeplejack_build_dir "${env}" "${config}")
    case "${env}" in
        ucrt64) echo "${build_dir}/steeplejack.exe" ;;
        wsl) echo "${build_dir}/steeplejack" ;;
        *) return 1 ;;
    esac
}
