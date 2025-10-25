#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
BUILD_DIR="${ROOT_DIR}/build"

if [[ -d "${BUILD_DIR}" ]]; then
    echo "Removing ${BUILD_DIR}"
    find "${BUILD_DIR}" -maxdepth 1 -mindepth 1 ! -name 'vcpkg_installed' -exec rm -rf {} +
fi

# Remove stray cache files from in-tree configure attempts
find "${ROOT_DIR}" -maxdepth 2 -name 'CMakeCache.txt' -delete
echo "Clean complete."
