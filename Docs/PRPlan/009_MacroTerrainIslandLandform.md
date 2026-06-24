# Macro Terrain Island Landform Pass

Roadmap label: PR #9.

## Purpose

This pass records the first shared macro-terrain vocabulary for a readable island terrain. It does not import final terrain data or commit editor-generated landscape assets.

## Required Landform Families

The initial terrain pass must support:

- ocean boundary, beaches, mangrove edge, cliff coast, rocky headlands, and creek mouths;
- mountain spine, ridges, spurs, saddles, slope breaks, valleys, gullies, creek channels, and basins;
- traversal categories for hard blockers, soft blockers, affordance zones, and false affordances.

The shared C++ vocabulary lives in `Source/JungleGame/JungleMacroTerrainSpec.h` so later generation, mask, PCG, and validation slices can classify landforms consistently.

## Slope Guardrails

The scaffold separates routine walkable slopes from soft and hard blockers:

- routine walkable: up to 32 degrees;
- soft blocker: 38 to under 48 degrees;
- hard blocker: 48 degrees and above.

These are first-pass guardrails for avoiding arbitrary steep noise and constant sliding traversal. Later playtesting can tune values without changing the terrain-first design source of truth.

## Validation

1. Run `git diff --check` on touched files.
2. Inspect `git diff --stat` and the full diff.
3. Confirm the terrain classes include coastline, mountain, ridge, creek, basin, blocker, affordance, and false-affordance concepts.
4. Confirm no generated editor or cache output is staged.

## Acceptance

This PR is accepted when macro landform and traversal-affordance categories are available to future terrain generation, ecosystem masks, and debug validation.
