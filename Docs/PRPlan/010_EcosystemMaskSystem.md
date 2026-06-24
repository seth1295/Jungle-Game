# Ecosystem Mask System

Roadmap label: PR #10.

## Purpose

This pass defines the data and classification foundation that lets terrain and hydrology drive ecosystems. It does not create final PCG density, final foliage, player navigation UI, or unrelated survival systems.

## Required Masks

The shared mask contract lives in `Source/JungleGame/JungleEcosystemMaskSpec.h` and defines these world-space mask channels:

- height
- slope
- aspect
- creek distance
- coast distance
- wetness
- ridge or valley index
- land-zone proxy
- canopy
- disturbance
- sound propagation
- visibility
- sky window
- footing or injury risk
- ecosystem tells
- night danger
- hard blocker
- soft blocker
- affordance zone
- false affordance

All masks are normalized to 0.0 through 1.0 unless a later implementation PR explicitly documents a physical unit range.

## Traversal Classification

Traversal classification is intentionally terrain-first:

- `HardBlocker` means terrain or water should stop routine traversal.
- `SoftBlocker` means traversal is possible but costly, slow, noisy, risky, or friction-heavy.
- `Affordance` means local terrain invites movement without drawing an authored route line.
- `FalseAffordance` means an apparent opening, coast route, gully, creek bank, or clearing can mislead the player.
- `Open` is the default when no stronger classification applies.

Wait-a-while and dense snagging vegetation belong in the soft-blocker/friction category, not in invisible-wall logic.

## World-Space Rules

- Masks must cross Landscape Component boundaries naturally.
- Masks must not be aligned to square ecosystem cells as a design assumption.
- Player-made trail marks are runtime persistence and are not stable PCG regeneration inputs.
- Mask outputs are developer data, not player-facing map layers.

## Validation

1. Run `git diff --check` on touched files.
2. Inspect `git diff --stat` and the full diff.
3. Confirm the mask list covers terrain, hydrology, canopy, sound, visibility, sky window, footing risk, ecosystem tells, night danger, and traversal categories.
4. Confirm traversal classification distinguishes hard blockers, soft blockers, affordance zones, and false affordances.
5. Confirm no player map, minimap, compass, bearing readout, GPS marker, objective marker navigation, HUD clock, explicit time readout, or orientation-confidence feedback is introduced.
6. Confirm no generated editor or cache output is staged.

## Acceptance

This PR is accepted when named masks and deterministic traversal classification placeholders exist for PR #11 debug visualization and validation.
