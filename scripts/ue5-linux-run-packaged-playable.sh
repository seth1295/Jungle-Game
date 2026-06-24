#!/usr/bin/env bash
set -u

PROJECT_ROOT="${PROJECT_ROOT:-$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)}"
PACKAGE_DIR="${PACKAGE_DIR:-$PROJECT_ROOT/PackagedBuilds/PR20Smoke/Linux}"
GAME_SH="${GAME_SH:-$PACKAGE_DIR/JungleGame.sh}"
HEADLESS="${HEADLESS:-0}"

fail() {
  printf 'ERROR: %s\n' "$1" >&2
  exit 1
}

[[ -x "$GAME_SH" ]] || fail "Packaged game launcher not found or not executable: $GAME_SH"

CMD=("$GAME_SH")
if [[ "$HEADLESS" == "1" ]]; then
  CMD+=(-nullrhi -nosound -unattended -ExecCmds=quit)
fi

printf 'Launching packaged JungleGame:\n'
printf ' %q' "${CMD[@]}"
printf '\n'

exec "${CMD[@]}"
