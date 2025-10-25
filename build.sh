#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)

UCRT_BUILD_DIR="${ROOT_DIR}/build/ucrt"
WSL_BUILD_DIR="${ROOT_DIR}/build/wsl"

CONFIG_ARGS=("$@")

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

configure_and_build() {
    local build_dir=$1
    local platform_toolchain=$2
    local label=$3

    printf '==> Configuring %s (%s)\n' "${label}" "${build_dir}"

    local cmake_cmd=(cmake -S "${ROOT_DIR}" -B "${build_dir}" -G Ninja)

    if [[ -n "${VCPKG_ROOT:-}" ]]; then
        local vcpkg_toolchain="${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        if [[ ! -f "${vcpkg_toolchain}" ]]; then
            echo "Requested VCPKG_ROOT (${VCPKG_ROOT}) does not contain scripts/buildsystems/vcpkg.cmake" >&2
            exit 1
        fi
        cmake_cmd+=(
            -DCMAKE_TOOLCHAIN_FILE="${vcpkg_toolchain}"
            -DVCPKG_CHAINLOAD_TOOLCHAIN_FILE="${platform_toolchain}"
        )
    else
        cmake_cmd+=(-DCMAKE_TOOLCHAIN_FILE="${platform_toolchain}")
    fi

    if [[ ${#CONFIG_ARGS[@]} -gt 0 ]]; then
        cmake_cmd+=("${CONFIG_ARGS[@]}")
    fi

    "${cmake_cmd[@]}"

    printf '==> Building %s\n' "${label}"
    cmake --build "${build_dir}"
}

env_name=$(detect_environment) || {
    echo "Unable to detect environment (expected MSYS2 UCRT64 or WSL2)." >&2
    exit 1
}

case "${env_name}" in
    ucrt64)
        configure_and_build "${UCRT_BUILD_DIR}" "${ROOT_DIR}/cmake/toolchains/msys2-ucrt.cmake" "MSYS2 UCRT64"
        ;;
    wsl)
        configure_and_build "${WSL_BUILD_DIR}" "${ROOT_DIR}/cmake/toolchains/wsl2-gcc.cmake" "WSL2"
        ;;
esac
