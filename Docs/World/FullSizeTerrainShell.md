# Full-Size Terrain In-Game Shell

Created: 2026-06-26
Runtime input: `pr5lifecycle-runtimefiles/002/003_full_size_terrain_ingame_shell_research.md`

## Scope

This PR introduces a source-authored full-size terrain shell actor so the packaged/editor runtime no longer depends only on the old Cell 0 slice for visible world state.

This is not final terrain import, final Landscape, final PCG, final foliage, or final Cell 0 placement. It is a bridge shell that makes the full-size world state visible and reviewable while project-owned `.umap` and Landscape import work remains pending.

## Runtime Shell

Runtime class:

```text
AJungleFullSizeTerrainShellActor
```

Spawn location:

```text
JungleGameMode::BeginPlay
```

The shell currently spawns source-authored blockout pieces for:

- central lowland/basin;
- creek valley;
- ridge spine;
- mountain shoulder;
- creek mouth/coast;
- north/south/east/west ocean edge markers.

The shell emits a runtime log line:

```text
Full-size terrain shell spawned: id=JG_FULL_TERRAIN_SHELL_002 extent_m=16256 review_points=8 source=runtime-source-authored-blockout
```

## Why Source-Authored Runtime Blocks

The repository currently has no project-owned `.umap` asset under `Content/`. Directly pointing config at `/Game/JungleGame/Maps/WLD_JungleIsland_Full` would reference a missing binary asset. This shell is therefore a bridge stage:

1. preserve project-owned full-size world authority;
2. create visible large-world structure without committing binary map churn;
3. keep Cell 0 as a review anchor rather than the whole world;
4. provide a later target for Landscape/World Partition replacement.

## Review Intent

The shell should make these macro intents visible:

| Feature | Purpose |
| --- | --- |
| Basin/lowland | proves a larger ground state exists outside Cell 0 |
| Creek valley | first drainage corridor placeholder |
| Ridge spine | high-ground navigation structure placeholder |
| Mountain shoulder | vertical range/world scale cue |
| Creek mouth/coast | terrain-to-ocean relationship placeholder |
| Ocean edges | natural boundary placeholder |

## Validation

Minimum validation for this PR:

- `git diff --check`;
- `git diff --cached --check`;
- source files stage only intended code/docs/memory;
- packaged or runtime logs should show `Full-size terrain shell spawned` on next game launch;
- no generated screenshots, logs, Saved, Intermediate, DerivedDataCache, or packaged output is committed.

## Forbidden Drift

This shell must not introduce player-facing map UI, minimap, compass, bearing readout, GPS marker, objective marker, HUD clock, explicit time display, orientation confidence UI, or character hints that tell the player whether they are oriented.

## Next PR

PR5 Batch 002 / 004 should add developer-only full-size mask/biome/debug state over this terrain shell and the deterministic source contract.
