#!/usr/bin/env bash
set -euo pipefail

BUILD_DIR="${1:-build/ubuntu-release}"
INSTALL_ROOT="${2:-deploy}"
QMAKE_BIN="${QMAKE_BIN:-qmake6}"
MAKE_BIN="${MAKE_BIN:-make}"

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_FILE="${PROJECT_ROOT}/untitled.pro"
BUILD_PATH="${PROJECT_ROOT}/${BUILD_DIR}"
INSTALL_PATH="${BUILD_PATH}/${INSTALL_ROOT}"
DEPLOY_BIN_PATH="${INSTALL_PATH}/opt/untitled/bin"

assert_exists() {
  local path="$1"
  local desc="$2"
  if [[ ! -e "${path}" ]]; then
    echo "${desc} does not exist: ${path}" >&2
    exit 1
  fi
}

assert_exists "${PROJECT_FILE}" "Project file"
command -v "${QMAKE_BIN}" >/dev/null 2>&1 || {
  echo "qmake not found: ${QMAKE_BIN}" >&2
  exit 1
}
command -v "${MAKE_BIN}" >/dev/null 2>&1 || {
  echo "make not found: ${MAKE_BIN}" >&2
  exit 1
}

mkdir -p "${BUILD_PATH}"
rm -rf "${INSTALL_PATH}"

echo "==> Project root: ${PROJECT_ROOT}"
echo "==> Build path : ${BUILD_PATH}"
echo "==> qmake      : ${QMAKE_BIN}"

pushd "${BUILD_PATH}" >/dev/null
"${QMAKE_BIN}" "${PROJECT_FILE}" CONFIG+=release
"${MAKE_BIN}" -j"$(nproc)"
"${MAKE_BIN}" install INSTALL_ROOT="./${INSTALL_ROOT}"

if [[ ! -d "${DEPLOY_BIN_PATH}" ]]; then
  echo "Deploy bin path was not created: ${DEPLOY_BIN_PATH}" >&2
  exit 1
fi

echo
echo "Deployment complete: ${DEPLOY_BIN_PATH}"
echo "Executable path    : ${DEPLOY_BIN_PATH}/untitled"

if command -v linuxdeployqt >/dev/null 2>&1; then
  echo "==> Running linuxdeployqt..."
  linuxdeployqt "${DEPLOY_BIN_PATH}/untitled" -bundle-non-qt-libs || true
  echo "linuxdeployqt finished (non-fatal if it reports warnings)."
else
  echo "linuxdeployqt not found, skipped."
fi
popd >/dev/null
