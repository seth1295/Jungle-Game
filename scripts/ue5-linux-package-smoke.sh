#!/usr/bin/env bash
set -u

PROJECT_ROOT="${PROJECT_ROOT:-$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)}"
PROJECT_FILE="${PROJECT_FILE:-$PROJECT_ROOT/JungleGame.uproject}"
UE5_ROOT="${UE5_ROOT:-/run/media/seth/UE5_WORKSPACE/UnrealEngine}"
ARCHIVE_DIR="${ARCHIVE_DIR:-$PROJECT_ROOT/PackagedBuilds/PR20Smoke}"
CLIENT_CONFIG="${CLIENT_CONFIG:-Development}"
TARGET_PLATFORM="${TARGET_PLATFORM:-Linux}"
JUNGLE_SKIP_BUILD="${JUNGLE_SKIP_BUILD:-0}"
RUN_UAT="$UE5_ROOT/Engine/Build/BatchFiles/RunUAT.sh"

if [[ "${1:-}" == "--dry-run" ]]; then
  DRY_RUN=1
else
  DRY_RUN=0
fi

fail() {
  printf 'ERROR: %s\n' "$1" >&2
  exit 1
}

[[ -f "$PROJECT_FILE" ]] || fail "Project file not found: $PROJECT_FILE"
[[ -x "$RUN_UAT" ]] || fail "RunUAT.sh not found or not executable. Set UE5_ROOT to your UnrealEngine root."

printf 'Project: %s\n' "$PROJECT_FILE"
printf 'UE5_ROOT: %s\n' "$UE5_ROOT"
printf 'Archive: %s\n' "$ARCHIVE_DIR"
printf 'Platform: %s\n' "$TARGET_PLATFORM"
printf 'Config: %s\n' "$CLIENT_CONFIG"
printf 'Skip build: %s\n' "$JUNGLE_SKIP_BUILD"

MAP_COUNT=$(find "$PROJECT_ROOT/Content" -type f -name '*.umap' 2>/dev/null | wc -l | tr -d ' ')
if [[ "$MAP_COUNT" == "0" ]]; then
  printf 'WARNING: No project .umap files found under Content/. Current DefaultEngine.ini uses engine template maps.\n'
fi

if [[ ! -f "$UE5_ROOT/Engine/Binaries/Linux/ShaderCompileWorker.modules" ]]; then
  printf 'WARNING: ShaderCompileWorker.modules is missing; cook may fail when shader compilation starts. Run scripts/ue5-linux-package-preflight.sh first.\n'
fi

if [[ ! -f "$UE5_ROOT/Engine/Binaries/Linux/UnrealPak.modules" ]]; then
  printf 'WARNING: UnrealPak.modules is missing; pak/archive may fail. Run scripts/ue5-linux-package-preflight.sh first.\n'
fi

if [[ ! -f "$PROJECT_ROOT/Binaries/Linux/JungleGame.target" ]]; then
  printf 'WARNING: JungleGame.target receipt is missing; stage may fail. Run scripts/ue5-linux-package-preflight.sh first or run without JUNGLE_SKIP_BUILD=1.\n'
fi

CMD=(
  "$RUN_UAT"
  BuildCookRun
  "-project=$PROJECT_FILE"
  -noP4
  "-platform=$TARGET_PLATFORM"
  "-clientconfig=$CLIENT_CONFIG"
  -cook
  -stage
  -pak
  -archive
  "-archivedirectory=$ARCHIVE_DIR"
  -build
  -unattended
  -utf8output
  -nullrhi
  -ddc=InstalledNoZenLocalFallback
  '-AdditionalCookerOptions=-skipzenstore'
)

if [[ "$JUNGLE_SKIP_BUILD" == "1" ]]; then
  for index in "${!CMD[@]}"; do
    if [[ "${CMD[$index]}" == "-build" ]]; then
      CMD[$index]="-skipbuild"
      break
    fi
  done
  CMD+=(
    -nocompile
    -nocompileeditor
  )
fi

printf 'Command:\n'
printf ' %q' "${CMD[@]}"
printf '\n'

if [[ "$DRY_RUN" == "1" ]]; then
  printf 'Dry run only. No package command executed.\n'
  exit 0
fi

exec "${CMD[@]}"
