# Cell 0 First Playable Integration Inside The Large World

## Roadmap Item

Roadmap PR #19 — `Cell 0 first playable integration inside the large world`

Batch position: `4/4` in the current sequential large-world gameplay batch.

## Goal

Make Cell 0 behave as a placed world location inside the large-world first playable instead of a runtime blockout that follows the player's initial spawn.

This PR intentionally keeps the existing ember/rain/crossing/evidence/watcher loop, but changes the integration model so the slice has world placement, entry/exit framing, and reviewable terrain relationship contracts.

## Player-Facing Reason

Before this PR, the game could load but still feel like the first implementation because the Cell 0 director was spawned by `AJungleGameMode` and then immediately anchored to the player position. That made Cell 0 follow the player spawn rather than existing as a location in the island.

After this PR, Cell 0 has a placed-world anchor path:

- a manually placed `AJungleCell0Director` in the world wins if present;
- otherwise GameMode creates a source-only fallback Cell 0 director at a documented large-world coordinate;
- the player is moved to a Cell 0 entry point for immediate first-playable testing;
- the old player-relative anchoring remains available only as debug mode, not as primary placement.

## Source Changes

### `AJungleCell0Director`

`Source/JungleGame/JungleCell0Director.h/.cpp` now supports:

- `EJungleCell0AnchorMode::PlacedWorldLocation` as the default placement mode.
- `EJungleCell0AnchorMode::PlayerRelativeDebug` for legacy debug-only behavior.
- `ConfigureLargeWorldPlacement(...)` so GameMode or a future placed actor setup can configure the world anchor before BeginPlay builds the cell.
- placed-world anchor resolution from the director actor transform or configured transform.
- optional player move to a local Cell 0 entry point at BeginPlay.
- updated logging that reports whether Cell 0 used placed-world or player-relative debug anchoring.

### `AJungleGameMode`

`Source/JungleGame/JungleGameMode.cpp` now:

- checks whether an `AJungleCell0Director` is already placed in the world;
- uses that placed director if present;
- otherwise spawns a fallback director at a large-world coordinate;
- configures that director as placed-world Cell 0;
- moves the player into the Cell 0 entry point for immediate testing.

Fallback coordinate:

```text
X = 204800 cm
Y = -96768 cm
Z = 1200 cm
Yaw = 34 degrees
```

This is intentionally source-reviewable. Later map/World Partition work can replace the fallback by placing an authored `AJungleCell0Director` actor in the large world.

### Integration Spec

`Source/JungleGame/JungleCell0LargeWorldIntegrationSpec.h` adds reviewable contracts for:

- placed director actor integration;
- fallback large-world coordinate behavior;
- player-relative debug rejection as primary placement;
- shelter anchor relationship;
- creek crossing relationship;
- rain pressure relationship;
- trail mark recovery relationship;
- watcher line-of-sight relationship;
- entry/exit terrain affordance;
- known-safety smoke visibility.

## Dependencies

This PR consumes or aligns with:

- PR #16 navigation pillars;
- PR #17 no-map/no-compass lost-state loop;
- PR #18 day/night/weather danger prototype;
- ecosystem mask names from the large-world mask system;
- the existing Cell 0 runtime actors: fire, crossing, marker, watcher cue.

## Preserved Cell 0 Loop

The following Cell 0 loop remains intact:

- fire/ember tending;
- rain pressure;
- crossing state change;
- marker reveal;
- watcher cue reveal;
- first-person walking and interaction.

The change is placement and integration, not story expansion or final polish.

## Hard Forbidden Scope

This PR does not add:

- player map;
- minimap;
- compass;
- bearing readout;
- GPS;
- objective marker;
- objective arrow;
- HUD clock;
- explicit time readout;
- orientation confidence system;
- hidden orientation meter;
- character comments that tell the player whether they are oriented;
- final story expansion;
- final art pass;
- unrelated mechanics.

## Validation Expectations

Reviewers should confirm:

- `AJungleCell0Director` no longer defaults to player-relative anchoring;
- GameMode uses a placed world director if present;
- GameMode fallback Cell 0 is a real large-world coordinate, not player spawn;
- the player is moved to a local Cell 0 entry point only for placed-world testing;
- legacy player-relative behavior is still possible only through explicit debug mode;
- the existing ember/rain/crossing/marker/watcher loop remains intact;
- the integration spec rejects player navigation UI and full story/final art drift;
- no generated/cache/editor junk is staged.

## UE Validation Notes

The UE binaries are available when the UE5 workspace disk is mounted and PATH includes:

```text
/run/media/seth/UE5_WORKSPACE/UnrealEngine/Engine/Binaries/Linux
```

Headless commandlet validation should use safe non-rendering flags where possible because plain editor/version invocations can initialize CEF/Vulkan/GLX.

## Acceptance Criteria

- Cell 0 is no longer inherently tied to the player's starting location.
- Cell 0 can be treated as a placed world location inside the island.
- A source-only fallback makes the location testable without committing broad binary level changes.
- Entry and exit are framed as terrain/world traversal, not map UI.
- Existing Cell 0 gameplay remains playable.

## Batch Handoff Requirement

Because this is PR `4/4` in the current sequential batch, a tracked fresh-chat handoff under `handoffs/` is required after this PR merges and local `main` is synced cleanly.
