# Self-Iterative Runtime 010 — Cell 0 Surface Spawn Alignment

## Goal

Make the first in-game Cell 0 view start on visible terrain surface.

## Acceptance Gate

Accepted only when the launched game shows visible terrain and the player view is on the playable surface.

## Diagnosis

The fallback Cell 0 placement uses a fixed world Z. The procedural terrain shell uses the deterministic terrain model height at the same X/Y, so the Cell 0 blockout and player entry can be vertically misaligned against the generated terrain.

## Scoped Fix

Before Cell 0 builds its actors or moves the player, align Cell 0 world Z to the deterministic terrain height sampled at the player entry X/Y. Keep the Cell 0 ground a small clearance above the terrain surface to avoid z-fighting.

## Evidence Required

- `JungleGameEditor Linux Development` compile succeeds.
- Visual smoke or manual launch evidence shows visible terrain with the player view on the playable surface.
- Accepted image evidence is tracked under `Images/`.
