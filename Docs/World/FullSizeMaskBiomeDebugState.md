# Full-Size Mask / Biome Debug State

Created: 2026-06-26
Runtime input: `pr5lifecycle-runtimefiles/002/004_full_size_mask_biome_debug_state_research.md`

## Scope

This PR defines developer-only mask, biome, and traversal debug state for the full-size terrain source and runtime shell. It does not add final foliage, final PCG density, final ecology simulation, or player-facing navigation UI.

## Required Debug Masks

The full-size world debug state tracks these mask names:

- `height`;
- `slope`;
- `aspect`;
- `ridge_valley`;
- `creek_influence`;
- `coast_distance`;
- `wetness`;
- `sky_window`;
- `hard_blocker`;
- `soft_blocker`;
- `affordance`;
- `false_affordance`;
- `biome_class`.

All values are developer validation data. They must not become player-facing map overlays or navigation aids.

## Debug Biome Classes

Initial full-size debug classes:

| Class | Meaning |
| --- | --- |
| `Ocean` | water/outside-island boundary |
| `Coast` | generic coast edge |
| `Mangrove` | low wet coastal blocker/transition |
| `CreekMouth` | creek-to-ocean transition |
| `RiparianWetValley` | creek/wet valley corridor |
| `InteriorRainforest` | baseline jungle interior |
| `RidgeForest` | high-ground ridge forest |
| `MountainShoulder` | upland transition |
| `BasinLowland` | flat/low basin |
| `AlluvialFlat` | deposited lowland/creek flat |
| `Gorge` | compressed steep corridor |

## Traversal Classes

Initial traversal classes:

| Class | Meaning |
| --- | --- |
| `Open` | normal movement |
| `SlowVegetation` | vegetation friction / slower movement |
| `WetSlow` | wet/muddy/slippery slow movement |
| `SteepSlow` | steep fatigue / slower movement |
| `Scramble` | difficult but possible scramble |
| `HardBlocked` | not a valid route |
| `FalseAffordance` | appears route-like but fails locally |

## Review Samples

Initial developer-only sample intents are encoded in `FJGFullSizeMaskBiomeDebugSpec`:

- coast sample;
- creek/wet-valley sample;
- ridge sample;
- basin sample;
- gorge candidate sample.

Each sample records review point identity, biome class, traversal class, height, slope, creek influence, coast distance, and wetness.

## Drift Controls

This debug state is forbidden from adding or implying:

- player map;
- minimap;
- compass;
- bearing readout;
- GPS marker;
- objective marker;
- HUD clock;
- explicit time display;
- orientation confidence UI;
- character comments that reveal orientation state.

## Validation

Minimum validation for this PR:

- `git diff --cached --check`;
- source/header drift scan for forbidden player navigation terms where practical;
- source files are developer-only contracts/specs;
- no generated screenshots, package output, Saved, Intermediate, DerivedDataCache, or full-resolution raster artifacts are committed.

## Next PR

PR5 Batch 002 / 005 should validate packaged full-size world smoke evidence, multi-point review capture, repo hygiene, and the batch handoff.
