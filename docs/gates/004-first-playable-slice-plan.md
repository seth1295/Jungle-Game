# Gate 004 — First Playable Slice Plan

## Status

planned

## Purpose

Convert the current design lane into a tightly scoped implementation plan for the first playable slice.

Working slice name:

> Jungle Cell 0: Embers

## Allowed scope

This gate may:

- define first playable mechanics;
- define required C++ systems and placeholder actors;
- define minimal art/blockout needs;
- define validation criteria for the slice;
- add or update `docs/FIRST_PLAYABLE_SLICE.md`;
- add task breakdowns under `docs/`.

## Forbidden scope

This gate must not:

- implement gameplay systems;
- add assets;
- create levels;
- add Blueprint-heavy systems;
- expand the full game design beyond the first slice;
- add combat, gore, VR, or multiplayer;
- lock the full final game idea beyond the current first playable lane.

## Required inputs

- Gate 003 passed.
- Current project brief.
- Current README design constraints.
- UE project structure from Gate 003.

## Required validation

The slice plan must specify:

- player spawn/control baseline;
- dense jungle traversal blockout goal;
- fire/ember survival pressure;
- rain/weather state;
- camp persistence;
- bridge/crossing state change;
- evidence-of-hands moment;
- distant nonviolent watcher presence;
- pass/fail criteria;
- files likely to change in Gate 005.

## Exit criteria

This gate passes when Gate 005 can implement the slice from the docs without inventing architecture or expanding scope.

## CodeRabbit handling

Fix valid in-scope documentation consistency issues only.

## Next gate

`docs/gates/005-jungle-cell-0-implementation.md`

## Stop conditions

Stop and report if:

- the slice depends on unavailable UE features;
- the plan requires heavy binary asset work before systems are proven;
- the plan cannot keep Blueprint usage thin;
- the first playable becomes a lore/menu/document-only prototype instead of spatial gameplay.
