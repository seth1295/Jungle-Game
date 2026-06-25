# UE5 Packaged Visual Capture

Use this skill when validating that the packaged Jungle Game build is visually playable, not just that it starts or logs spawn messages.

## Purpose

The approved visual smoke path launches the packaged Linux build, lets game code move the player to a deterministic Cell 0 validation view, asks Unreal to capture a high-resolution screenshot, and exits.

This catches regressions where logs say Cell 0 spawned but the player sees only sky, ocean, or empty edge views.

## Command

From the repo root, after building and packaging:

```bash
CAPTURE_VIEW=1 bash scripts/ue5-linux-run-packaged-playable.sh
```

The game-side trigger is the command-line parameter:

```text
-JungleVisualSmoke
```

## Evidence

The screenshot is generated under the ignored package output tree:

```text
PackagedBuilds/PR20Smoke/Linux/JungleGame/Saved/Screenshots/Linux/
```

Generated screenshots are validation evidence only. Do not commit them.

## Expected log signals

Look for these in the packaged game log:

```text
Cell 0 visual smoke capture scheduled.
Cell 0 visual smoke view set
Cell 0 visual smoke shot requested.
Cell 0 visibility diagnostic
```

The visibility diagnostic should report a downward hit and radial hits around the player.

## Scope guard

This workflow must not add player-facing map, minimap, compass, bearing readout, GPS, objective arrows, HUD clock, orientation-confidence UI, or binary map assets unless a later gate explicitly allows them.
