#!/usr/bin/env bash
# spell-checker: disable

set -euo pipefail

SCRIPT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
source "${SCRIPT_DIR}/lib/common.sh"

ROOT_DIR=$(steeplejack_root)

CONFIG=${1:-debug}
shift || true
BUILD_TYPE="Debug"
case "${CONFIG}" in
    debug)
        BUILD_TYPE="Debug"
        ;;
    release)
        BUILD_TYPE="Release"
        ;;
    *)
        echo "Unknown configuration '${CONFIG}'. Use debug or release." >&2
        exit 1
        ;;
esac

CONFIG_ARGS=("$@")

configure_and_build() {
    local build_dir=$1
    local platform_toolchain=$2
    local label=$3

    printf '==> Configuring %s (%s)\n' "${label}" "${build_dir}"

    local cmake_cmd=(cmake -S "${ROOT_DIR}" -B "${build_dir}" -G Ninja -DCMAKE_BUILD_TYPE="${BUILD_TYPE}")

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

env_name=$(steeplejack_detect_environment) || {
    echo "Unable to detect environment (expected MSYS2 UCRT64 or WSL2)." >&2
    exit 1
}

build_dir=$(steeplejack_build_dir "${env_name}" "${CONFIG}")
toolchain=$(steeplejack_toolchain_file "${env_name}")
label=$(steeplejack_env_label "${env_name}")

configure_and_build "${build_dir}" "${toolchain}" "${label}"
