#!/usr/bin/env bash
set -euo pipefail

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${BUILD_DIR:-build/ubuntu-release}"
INSTALL_ROOT="${INSTALL_ROOT:-deploy}"
PACKAGE_NAME="${PACKAGE_NAME:-untitled-linux-x86_64}"
PACKAGE_ROOT="${PROJECT_ROOT}/dist/${PACKAGE_NAME}"
BUILD_PATH="${PROJECT_ROOT}/${BUILD_DIR}"
INSTALL_PATH="${BUILD_PATH}/${INSTALL_ROOT}"
APP_BIN="${INSTALL_PATH}/opt/untitled/bin/untitled"
BIN_DIR="${PACKAGE_ROOT}/bin"
LIB_DIR="${PACKAGE_ROOT}/lib"
PLUGIN_DIR="${PACKAGE_ROOT}/plugins/platforms"
QT_PLUGIN_ROOT="${QT_PLUGIN_ROOT:-$(qmake6 -query QT_INSTALL_PLUGINS)}"
ARCHIVE_PATH="${PROJECT_ROOT}/dist/${PACKAGE_NAME}.tar.gz"

if [[ ! -x "${APP_BIN}" ]]; then
  echo "Packaged binary not found: ${APP_BIN}" >&2
  echo "Run ./deploy_ubuntu.sh first." >&2
  exit 1
fi

mkdir -p "${PACKAGE_ROOT}" "${BIN_DIR}" "${LIB_DIR}" "${PLUGIN_DIR}"
rm -rf "${PACKAGE_ROOT:?}/"*
mkdir -p "${BIN_DIR}" "${LIB_DIR}" "${PLUGIN_DIR}"

cp -f "${APP_BIN}" "${BIN_DIR}/untitled"

if [[ ! -f "${QT_PLUGIN_ROOT}/platforms/libqxcb.so" ]]; then
  echo "Qt platform plugin not found: ${QT_PLUGIN_ROOT}/platforms/libqxcb.so" >&2
  exit 1
fi
cp -L "${QT_PLUGIN_ROOT}/platforms/libqxcb.so" "${PLUGIN_DIR}/"

declare -A copied=()

should_skip_lib() {
  local lib="$1"
  case "${lib}" in
    /lib/x86_64-linux-gnu/ld-linux-x86-64.so.2|/lib64/ld-linux-x86-64.so.2)
      return 0
      ;;
    /lib/x86_64-linux-gnu/libc.so.6|/lib/x86_64-linux-gnu/libm.so.6|/lib/x86_64-linux-gnu/libpthread.so.0|/lib/x86_64-linux-gnu/librt.so.1|/lib/x86_64-linux-gnu/libdl.so.2|/lib/x86_64-linux-gnu/libgcc_s.so.1|/lib/x86_64-linux-gnu/libstdc++.so.6)
      return 0
      ;;
  esac
  return 1
}

copy_library() {
  local src="$1"
  local base
  base="$(basename "${src}")"
  if [[ -z "${copied[${base}]:-}" ]]; then
    cp -L "${src}" "${LIB_DIR}/${base}"
    copied["${base}"]=1
  fi
}

collect_deps() {
  local target="$1"
  while IFS= read -r dep; do
    [[ -z "${dep}" ]] && continue
    [[ "${dep}" == "linux-vdso.so.1" ]] && continue
    if should_skip_lib "${dep}"; then
      continue
    fi
    copy_library "${dep}"
  done < <(
    ldd "${target}" | awk '
      /=>/ {
        if ($3 ~ /^\//) {
          print $3
        }
        next
      }
      /^\// {
        print $1
      }
    '
  )
}

collect_deps "${BIN_DIR}/untitled"
collect_deps "${PLUGIN_DIR}/libqxcb.so"

cat > "${PACKAGE_ROOT}/run.sh" <<'EOF'
#!/usr/bin/env bash
set -euo pipefail

APP_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
export LD_LIBRARY_PATH="${APP_DIR}/lib${LD_LIBRARY_PATH:+:${LD_LIBRARY_PATH}}"
export QT_PLUGIN_PATH="${APP_DIR}/plugins${QT_PLUGIN_PATH:+:${QT_PLUGIN_PATH}}"
export QT_QPA_PLATFORM_PLUGIN_PATH="${APP_DIR}/plugins/platforms"

exec "${APP_DIR}/bin/untitled" "$@"
EOF
chmod +x "${PACKAGE_ROOT}/run.sh"

cat > "${PACKAGE_ROOT}/README.txt" <<'EOF'
This package contains a portable Linux build of the app.

Run it with:
  ./run.sh

If you move the folder, keep the directory layout intact:
  bin/
  lib/
  plugins/
  run.sh
EOF

tar -C "${PROJECT_ROOT}/dist" -czf "${ARCHIVE_PATH}" "${PACKAGE_NAME}"

echo "Portable package created:"
echo "  ${PACKAGE_ROOT}"
echo "Archive:"
echo "  ${ARCHIVE_PATH}"
