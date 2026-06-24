#!/usr/bin/env bash
set -u

PROJECT_ROOT="${PROJECT_ROOT:-$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)}"
PROJECT_FILE="${PROJECT_FILE:-$PROJECT_ROOT/JungleGame.uproject}"
UE5_ROOT="${UE5_ROOT:-/run/media/seth/UE5_WORKSPACE/UnrealEngine}"
BUILD_SH="$UE5_ROOT/Engine/Build/BatchFiles/Linux/Build.sh"
UE_BIN="$UE5_ROOT/Engine/Binaries/Linux"
DRY_RUN=0

if [[ "${1:-}" == "--dry-run" ]]; then
  DRY_RUN=1
fi

fail() {
  printf 'ERROR: %s\n' "$1" >&2
  exit 1
}

[[ -f "$PROJECT_FILE" ]] || fail "Project file not found: $PROJECT_FILE"
[[ -x "$BUILD_SH" ]] || fail "Build.sh not found or not executable. Set UE5_ROOT to your UnrealEngine root."

printf 'Project: %s\n' "$PROJECT_FILE"
printf 'UE5_ROOT: %s\n' "$UE5_ROOT"

build_target() {
  local target="$1"
  shift
  local cmd=("$BUILD_SH" "$target" Linux Development "$@" -WaitMutex)
  printf 'Build command:'
  printf ' %q' "${cmd[@]}"
  printf '\n'
  if [[ "$DRY_RUN" == "0" ]]; then
    "${cmd[@]}" || return $?
  fi
}

if [[ ! -f "$UE_BIN/ShaderCompileWorker.modules" ]]; then
  printf 'Missing ShaderCompileWorker.modules; building ShaderCompileWorker.\n'
  build_target ShaderCompileWorker
else
  printf 'OK: ShaderCompileWorker.modules exists.\n'
fi

if [[ ! -f "$UE_BIN/UnrealPak.modules" ]]; then
  printf 'Missing UnrealPak.modules; building UnrealPak.\n'
  build_target UnrealPak
else
  printf 'OK: UnrealPak.modules exists.\n'
fi

if [[ ! -f "$PROJECT_ROOT/Binaries/Linux/JungleGame.target" ]]; then
  printf 'Missing JungleGame.target receipt; building standalone game target.\n'
  build_target JungleGame "$PROJECT_FILE"
else
  printf 'OK: JungleGame.target exists.\n'
fi

if [[ "$DRY_RUN" == "1" ]]; then
  printf 'Dry run only. No build commands executed.\n'
else
  printf 'Package preflight complete.\n'
fi
