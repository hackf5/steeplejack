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

    return 1
}

steeplejack_env_label() {
    case "$1" in
        ucrt64) echo "MSYS2 UCRT64" ;;
        *) echo "unknown" ;;
    esac
}

steeplejack_toolchain_file() {
    local env=$1
    local root
    root=$(steeplejack_root)
    case "${env}" in
        ucrt64) echo "${root}/cmake/toolchains/msys2-ucrt.cmake" ;;
        *) return 1 ;;
    esac
}

steeplejack_build_dir() {
    local env=$1
    local config=$2
    local root
    case "${env}" in
        ucrt64)
            root=$(steeplejack_root)
            echo "${root}/build/ucrt/${config}"
            ;;
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
        *) return 1 ;;
    esac
}
