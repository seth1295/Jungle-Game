# Full-Size Terrain Shell V2

## Purpose

PR5 Batch 003 / 001 replaces the first full-size cube-block shell with a deterministic procedural terrain substrate. The prior cube shell proved runtime world authority, full-size spawn, and screenshot capture, but its slab geometry was too artificial for creek, ridge, slope, and biome-readability decisions.

This terrain shell is still a bridge, not final Landscape import or final art.

## Runtime contract

`AJungleFullSizeTerrainShellActor` now creates a coarse continuous procedural mesh heightfield at runtime.

Required properties:

- deterministic source-owned generation;
- fixed 16.256 km world authority identity;
- basin, creek-valley, ridge-spine, mountain-shoulder, coast, and ocean-edge height logic;
- collision enabled for developer traversal smoke;
- existing cube blockout available only as explicit debug fallback through `-JungleDebugCubeShell` or actor setting;
- no player-facing map, minimap, compass, GPS, bearing, objective marker, HUD clock, explicit time readout, or orientation confidence UI.

## Runtime log tokens

The old Batch 002 token remains for compatibility:

```text
Full-size terrain shell spawned: id=JG_FULL_TERRAIN_SHELL_002 extent_m=16256 review_points=8 source=runtime-source-authored-blockout
```

The new terrain-substrate token is:

```text
Full-size terrain shell v2 ready: id=JG_FULL_TERRAIN_SHELL_003 grid=129x129 extent_m=16256 source=deterministic-procedural-heightfield cube_fallback=available
```

## Terrain smoke capture

Use `-JungleTerrainShellSmoke` when validating the full-size terrain substrate. This moves the player view to a terrain review point, requests `HighResShot 1920x1080`, and exits after the shot.

Use `-JungleVisualSmoke` only for Cell 0 visual validation. That path deliberately photographs the Cell 0 playable slice.

## Validation status

The terrain shell v2 compiled through `JungleGameEditor Linux Development`, emitted the `JG_FULL_TERRAIN_SHELL_003` runtime token, and was manually inspected in a live game window. The player could stand on the procedural mesh and the visible-relief pass produced a sloped hill rather than a flat cube slab.

Automatic `-JungleTerrainShellSmoke` currently requests `HighResShot 1920x1080` but exits before a screenshot file is written. Treat live/manual visual inspection as the current evidence until the smoke timing is widened.

## Next work

Continue with creek/riparian traversal, ridge/slope readability, lowland/coast transitions, diegetic navigation cues, and day/night/weather traversal pressure. If automated visual evidence is required first, widen the terrain smoke screenshot/exit timing.
