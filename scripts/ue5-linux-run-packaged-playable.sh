#!/usr/bin/env bash
set -u

PROJECT_ROOT="${PROJECT_ROOT:-$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)}"
PACKAGE_DIR="${PACKAGE_DIR:-$PROJECT_ROOT/PackagedBuilds/PR20Smoke/Linux}"
GAME_SH="${GAME_SH:-$PACKAGE_DIR/JungleGame.sh}"
CAPTURE_DIR="${CAPTURE_DIR:-$PACKAGE_DIR/JungleGame/Saved/Screenshots/Linux}"
HEADLESS="${HEADLESS:-0}"
CAPTURE_VIEW="${CAPTURE_VIEW:-0}"

fail() {
  printf 'ERROR: %s\n' "$1" >&2
  exit 1
}

[[ -x "$GAME_SH" ]] || fail "Packaged game launcher not found or not executable: $GAME_SH"

CMD=("$GAME_SH")
if [[ "$HEADLESS" == "1" ]]; then
  CMD+=(-nullrhi -nosound -unattended -ExecCmds=quit)
fi
if [[ "$CAPTURE_VIEW" == "1" ]]; then
  CMD+=(-JungleVisualSmoke -ResX=1920 -ResY=1080 -windowed -nosplash -unattended)
fi

printf 'Launching packaged JungleGame:\n'
printf ' %q' "${CMD[@]}"
printf '\n'

if [[ "$CAPTURE_VIEW" == "1" ]]; then
  "${CMD[@]}"
  exit $?
fi

exec "${CMD[@]}"
