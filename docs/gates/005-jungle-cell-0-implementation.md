# Gate 005 — Jungle Cell 0 Implementation

## Status

planned

## Purpose

Implement the first real playable slice after the UE project exists and the slice plan is complete.

Slice name:

> Jungle Cell 0: Embers

## Allowed scope

This gate may implement prototype versions of:

- first-person movement/control shell;
- compact jungle traversal blockout;
- fire/ember survival pressure;
- rain/weather state;
- camp persistence;
- bridge/crossing state change;
- evidence-of-hands event;
- distant nonviolent watcher presence stub;
- basic Linux build/cook/smoke validation notes.

## Forbidden scope

This gate must not:

- add combat systems;
- add gore;
- add modern tools or current-day setting elements;
- add VR or multiplayer;
- add large asset packs;
- depend on paid assets or unproven plugins;
- build a full open world;
- add complex NPC simulation beyond the watcher-presence stub;
- add heavy Blueprint architecture where C++ systems should own state.

## Required inputs

- Gate 003 passed.
- Gate 004 passed.
- `docs/FIRST_PLAYABLE_SLICE.md` exists and is current.
- UE project builds before gameplay changes begin.

## Required validation

Run or document:

- C++ build result;
- editor launch result, if available;
- Linux package/cook result, if available;
- smoke launch result, if available;
- basic manual test path through the slice;
- diff/stat inspection for binary/generated junk.

## Exit criteria

This gate passes when the repo contains a minimal spatial 3D playable slice that demonstrates the lane without relying on menus, lore-only content, gore, combat, or asset bloat.

## CodeRabbit handling

Fix valid in-scope findings only. Do not expand into future systems unless the finding proves the current slice is broken.

## Next gate

Create the next gate only after evaluating the first playable result. Likely candidates:

- jungle rendering/performance pass;
- world-state persistence expansion;
- watcher behavior expansion;
- environmental audio/weather pass.

## Stop conditions

Stop and report if:

- build/cook/package fails for engine/platform reasons;
- performance is too poor for the baseline hardware;
- implementing the slice requires unplanned binary asset work;
- the prototype loses the agreed lane of 1000 CE dense jungle survival-simulation with nonviolent social/anthropological horror.
