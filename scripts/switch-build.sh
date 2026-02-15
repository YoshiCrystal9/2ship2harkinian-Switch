#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
IMAGE_NAME=${IMAGE_NAME:-2ship-switch-build:latest}
# Match CI build type (Release) but keep a docker-specific build dir by default
# to avoid CMakeCache path/toolchain mismatches when building on macOS.
BUILD_DIR=${BUILD_DIR:-build-switch-docker}
BUILD_TYPE=${BUILD_TYPE:-Release}
JOBS=${JOBS:-}

usage() {
  cat <<'EOF'
Usage: scripts/switch-build.sh [--release] [--clean] [--configure] [--rebuild-image] [--jobs N]

Builds the Switch NRO in a devkitPro docker container with persistent ccache.

Env overrides:
  IMAGE_NAME   Docker image to use (default: 2ship-switch-build:latest)
  BUILD_DIR    CMake build directory (default: build-switch-docker)
  BUILD_TYPE   CMake build type (default: Release)
  JOBS         Parallel build jobs (default: container nproc)

Outputs:
  dist/switch/2ship.nro
EOF
}

CLEAN=0
REBUILD_IMAGE=0
FORCE_CONFIGURE=0
UPDATE_SUBMODULE=0
ZERO_CCACHE=0

while [[ $# -gt 0 ]]; do
  case "$1" in
    --release)
      BUILD_TYPE=Release
      shift
      ;;
    --debug)
      BUILD_TYPE=Debug
      shift
      ;;
    --clean)
      CLEAN=1
      shift
      ;;
    --rebuild-image)
      REBUILD_IMAGE=1
      shift
      ;;
    --configure)
      FORCE_CONFIGURE=1
      shift
      ;;
    --jobs)
      JOBS=${2:-}
      shift 2
      ;;
    --update-submodule)
      UPDATE_SUBMODULE=1
      shift
      ;;
    --ccache-zero)
      ZERO_CCACHE=1
      shift
      ;;
    -h|--help)
      usage
      exit 0
      ;;
    *)
      echo "Unknown arg: $1" >&2
      usage
      exit 2
      ;;
  esac
done

# If the build directory already exists but was configured with a different
# build type, force a reconfigure. This avoids accidentally producing a Debug
# binary when the caller expects Release (or vice versa).
if [[ -f "$ROOT_DIR/$BUILD_DIR/CMakeCache.txt" ]]; then
  CACHE_BUILD_TYPE=$(grep -E '^CMAKE_BUILD_TYPE:STRING=' "$ROOT_DIR/$BUILD_DIR/CMakeCache.txt" | head -n 1 | cut -d= -f2 || true)
  if [[ -n "${CACHE_BUILD_TYPE}" ]] && [[ "${CACHE_BUILD_TYPE}" != "${BUILD_TYPE}" ]]; then
    echo "Build dir '$BUILD_DIR' configured as ${CACHE_BUILD_TYPE}; switching to ${BUILD_TYPE} -> forcing reconfigure" >&2
    FORCE_CONFIGURE=1
  fi
fi

if ! command -v docker >/dev/null 2>&1; then
  echo "docker not found; install Docker Desktop" >&2
  exit 1
fi

mkdir -p "$ROOT_DIR/dist/switch"

CCACHE_HOST_DIR=${CCACHE_HOST_DIR:-"$HOME/.ccache/2ship-switch"}
mkdir -p "$CCACHE_HOST_DIR"

if [[ $REBUILD_IMAGE -eq 1 ]] || ! docker image inspect "$IMAGE_NAME" >/dev/null 2>&1; then
  echo "Building docker image: $IMAGE_NAME" >&2
  docker build -f "$ROOT_DIR/Dockerfile.switch-build" -t "$IMAGE_NAME" "$ROOT_DIR"
fi

echo "Updating libultraship submodule (host)" >&2
# In a git submodule, .git is typically a *file* (gitdir: ...), not a directory.
if [[ $UPDATE_SUBMODULE -eq 1 ]] || [[ ! -e "$ROOT_DIR/libultraship/.git" ]]; then
  git -C "$ROOT_DIR" submodule update --init --recursive libultraship
else
  echo "(skipped) libultraship already initialized; use --update-submodule to sync" >&2
fi

if [[ $CLEAN -eq 1 ]]; then
  echo "Cleaning build dir: $BUILD_DIR" >&2
  rm -rf "$ROOT_DIR/$BUILD_DIR"
fi

BUILD_CMD=$(cat <<EOF
set -euo pipefail
cd /src

if [ "${ZERO_CCACHE}" = "1" ]; then
  ccache -z || true
fi

if [ "${FORCE_CONFIGURE}" = "1" ] || [ ! -f "${BUILD_DIR}/CMakeCache.txt" ]; then
  cmake -S . -B "$BUILD_DIR" -G Ninja \
    -DCMAKE_TOOLCHAIN_FILE=/opt/devkitpro/cmake/Switch.cmake \
    -DCMAKE_BUILD_TYPE:STRING="$BUILD_TYPE" \
    -DCMAKE_C_COMPILER_LAUNCHER=ccache \
    -DCMAKE_CXX_COMPILER_LAUNCHER=ccache \
    -DFETCHCONTENT_UPDATES_DISCONNECTED=ON
fi

if [ -n "${JOBS}" ]; then
  JOBS_TO_USE="${JOBS}"
else
  JOBS_TO_USE="\$(nproc)"
fi

cmake --build "$BUILD_DIR" --target 2ship_nro --parallel "\$JOBS_TO_USE"

ccache -s || true
EOF
)

echo "Running container build ($BUILD_TYPE)" >&2
docker run --rm \
  -v "$ROOT_DIR":/src \
  -v "$CCACHE_HOST_DIR":/ccache \
  -w /src \
  "$IMAGE_NAME" \
  bash -lc "$BUILD_CMD"

NRO_SRC=$(ls -1 "$ROOT_DIR/$BUILD_DIR"/mm/*.nro 2>/dev/null | head -n 1 || true)
if [[ -z "$NRO_SRC" ]]; then
  echo "Could not find built .nro in $BUILD_DIR/mm" >&2
  exit 1
fi

cp -f "$NRO_SRC" "$ROOT_DIR/dist/switch/2ship.nro"
echo "Built: dist/switch/2ship.nro" >&2
