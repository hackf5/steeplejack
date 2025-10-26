#!/usr/bin/env bash
# spell-checker: disable

set -euo pipefail

ROOT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
SCRIPTS_DIR="${ROOT_DIR}/scripts"

usage() {
    cat <<USAGE
Usage: $(basename "$0") <build|clean|run|test|format|lint> [debug|release] [-- extra args]

Commands:
  build [config]  Build the project (config defaults to debug). Extra args are passed to CMake configure.
  clean [config]  Clean build artifacts for the config.
  run   [config]  Execute the latest build for the config. Extra args are passed to the executable.
  test  [config]  Build and run the test suite for the config.
  format [mode]   Run clang-format. mode: fix (default) or check.
  lint  [mode]    Run clang-tidy. mode: run (default) or fix.
USAGE
    exit 1
}

command=${1:-}
if [[ -z "${command}" ]]; then
    usage
fi

shift || true

config="debug"
if [[ $# -gt 0 ]]; then
    case "$1" in
        debug|release)
            config="$1"
            shift
            ;;
    esac
fi

extra_args=("$@")

case "${command}" in
    build)
        "${SCRIPTS_DIR}/build.sh" "${config}" "${extra_args[@]}"
        ;;
    clean)
        "${SCRIPTS_DIR}/clean.sh" "${config}" "${extra_args[@]}"
        ;;
    run)
        "${SCRIPTS_DIR}/run.sh" "${config}" "${extra_args[@]}"
        ;;
    test)
        "${SCRIPTS_DIR}/test.sh" "${config}" "${extra_args[@]}"
        ;;
    format)
        # format does not use config in the same way; pass any extra args
        "${SCRIPTS_DIR}/format.sh" "${extra_args[@]}"
        ;;
    lint)
        # lint does not use config; pass mode (run|fix)
        "${SCRIPTS_DIR}/lint.sh" "${extra_args[@]}"
        ;;
    *)
        usage
        ;;
esac
