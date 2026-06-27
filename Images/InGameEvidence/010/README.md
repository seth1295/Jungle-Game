# In-Game Evidence 010 — Cell 0 Surface Spawn

Runtime: `SELF-ITERATIVE/010_cell0_surface_spawn_alignment.md`

This folder contains tracked in-game visual evidence for the Cell 0 surface-spawn fix.

## Evidence

- `Cell0SurfaceSpawn_After.png` — captured from Unreal with `-RenderOffscreen`, `-JungleVisualSmoke`, and `-JungleTrackedVisualEvidence=Images/InGameEvidence/010/Cell0SurfaceSpawn_After.png` after aligning Cell 0 to the deterministic terrain surface.

## Acceptance

Accepted visual state for this runtime:

- visible terrain is present;
- player/camera is on the playable surface;
- the old view from below the terrain shell is not reproduced.

## Supporting log signals

The accepted run logged:

- `Cell 0 surface alignment`
- `Player moved to Cell 0 large-world entry`
- `Cell 0 visibility diagnostic` with `down_hit=true`
- `Cell 0 tracked visual smoke shot requested`
