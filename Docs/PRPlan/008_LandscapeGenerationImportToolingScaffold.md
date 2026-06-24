# Landscape Generation Import Tooling Scaffold

Roadmap label: PR #8.

## Purpose

This scaffold defines the repeatable terrain-source contract for the large island landscape without locking final terrain art.

The scaffold targets the PR #7 foundation:

- 8129 samples per side
- 2 m sample spacing
- 16.256 km side length

## Required Source Products

Future tooling should produce or import these aligned layers:

- `height_meters`
- `slope_degrees`
- `aspect_degrees`
- `flow_accumulation`
- `ridge_valley_index`
- `coast_distance_meters`

The C++ contract is recorded in `Source/JungleGame/JungleTerrainSourceSpec.h` so later commandlets or editor utilities can validate terrain-source dimensions and layer names before import.

## Import Rules

- Height data must match the foundation size unless the design source of truth changes.
- Derived layers must share world-space alignment with the height layer.
- Tooling must be deterministic enough to rerun from documented inputs.
- Large binary artifacts should not be staged unless a later PR explicitly justifies their size and format.
- Player-made trail marks are runtime persistence and are not stable terrain-source inputs.

## Validation

1. Run `git diff --check` on touched files.
2. Inspect `git diff --stat` and the full diff.
3. Confirm the layer contract matches `FJungleWorldPartitionSpec`.
4. Confirm no generated or cache output is staged.

## Acceptance

This PR is accepted when the repo has a reusable, reviewable terrain-source contract that future generation or import commands can target.
