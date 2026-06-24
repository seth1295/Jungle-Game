# First Playable Slice — Jungle Cell 0: Embers

## Purpose

`Jungle Cell 0: Embers` is the first playable slice for Jungle Game. It proves a compact first-person survival-simulation loop in a dense jungle cell, with nonviolent social horror emerging through changed physical evidence rather than combat, gore, exposition, or monster behavior.

Core thesis:

> You are not being hunted. You are being evaluated.

## Gate 004 Decision

Gate 004 converts the existing research lane into an implementation-ready plan for Gate 005.

This plan is intentionally small. Gate 005 should implement the minimum playable loop, not the whole game.

## Player Experience

The player should experience this sequence:

1. Spawn in a small camp at dusk or late afternoon.
2. Notice a fire/ember object that can decay.
3. Tend the fire through a single interaction.
4. Leave camp along a short, enclosed jungle route.
5. Rain begins or intensifies.
6. The player feels pressure to return before the embers die.
7. A crossing changes while the player is away.
8. The player discovers physical evidence that something with hands changed the route.
9. A distant watcher cue confirms presence without attack or conversation.
10. The player returns to camp and sees state persisted.

Emotional target:

```text
I am not alone.
It saw what I did.
It changed something deliberately.
It did not attack.
It is testing me.
```

## Required Playable Loop

```text
Camp
→ Ember pressure
→ Rain pressure
→ Jungle traversal
→ Crossing state change
→ Evidence of hands
→ Distant watcher cue
→ Return to persistent camp
```

## Required Mechanics For Gate 005

| Mechanic | Required implementation | Acceptance signal |
|---|---|---|
| First-person spawn/control | Player can spawn, look, move, and interact | PIE/commandlet validation shows configured GameMode/player path |
| Camp center | One compact camp space exists | Player begins near fire and can return |
| Fire/ember state | Fire has at least `Dead`, `Ember`, and `Burning` or equivalent scalar state | Fire state changes over time and on interaction |
| Rain state | One deterministic rain event or debug-triggered rain state | Rain affects ember decay or fire state visibly/logically |
| Short jungle traversal | A short route from camp to crossing exists | Player can leave and return without open-world scope |
| Crossing state | Crossing has at least two states | State changes after player leaves or triggers route beat |
| Evidence-of-hands | One physical clue implies deliberate manipulation | Tied/moved/arranged object or marker, no gore/trope framing |
| Watcher cue | One nonviolent distant cue | Audio/visual/debug placeholder confirms presence without combat |
| Slice state | Relevant event state persists during the slice | Fire/crossing/evidence/watcher state does not instantly reset |

## Implementation Architecture

Gate 005 should keep C++ as the owner of runtime state. Blueprint may be used as a thin placement layer only when necessary for editor workflow.

Recommended C++ classes/components:

| Area | Preferred shape | Notes |
|---|---|---|
| Player | Minimal `AJunglePlayerCharacter` or pawn | Move/look/interact only |
| Game mode | Minimal `AJungleGameMode` | Sets player class and default rules |
| Interaction | Interface or small component | Keep generic enough for fire/crossing/evidence |
| Fire | Actor or component with ember state | No inventory/fuel taxonomy yet |
| Rain/weather | Actor/subsystem with deterministic state | One weather transition only |
| Crossing | Actor with pre/post state | State change is scripted, not AI-driven |
| Evidence | Actor or marker state | Physical clue, no lore dump |
| Watcher | Trigger/stub actor | Distant cue only; no full AI/perception |
| Slice state | Small world actor or subsystem | Same-session persistence preferred |

## Map / Content Policy

Gate 005 may add one compact playable map and minimal placeholder content only if required to prove the loop.

Allowed content:

```text
Content/JungleCell0/Maps/...
Content/JungleCell0/Materials/...
Content/JungleCell0/Blueprints/...
Content/JungleCell0/Placeholders/...
```

Allowed placeholders:

- simple greybox terrain/path/camp/crossing geometry;
- simple fire/ember visual placeholder;
- simple rain visual/audio placeholder;
- simple moved branch/vine/stone/evidence marker;
- simple watcher cue marker.

Forbidden content:

- asset packs;
- paid assets/plugins;
- gore/corpse/horror-shrine assets;
- modern tools, cameras, radios, flashlights, GPS, drones, firearms;
- full jungle art pass;
- photoreal foliage dump;
- large binary asset import.

## Blueprint Policy

Blueprint is allowed only for thin placement or editor-facing wrappers.

Allowed:

- placing C++ actors;
- setting exposed C++ variables;
- simple visual/audio placeholder hookups;
- map composition.

Forbidden:

- heavy Blueprint gameplay architecture;
- core state owned only by Blueprint;
- Blueprint-only inventory/crafting/weather/AI systems;
- large Blueprint graphs that hide logic from review.

## Systems Explicitly Out Of Scope

Gate 005 must not implement:

- combat;
- weapons;
- damage/death loop;
- monster chase;
- gore;
- modern equipment;
- full inventory;
- full crafting;
- hunger/thirst/disease/injury;
- full weather simulation;
- full ecology simulation;
- full hominin AI;
- behavior trees/perception systems for watcher logic;
- dialogue/language;
- quest/objective system;
- VR;
- multiplayer;
- online subsystem;
- Steam packaging;
- Marketplace/Fab pipeline.

## Pass Criteria For Gate 005

Gate 005 passes when a reviewer can confirm:

1. The project builds on Linux.
2. The project opens or commandlet-validates on Linux.
3. A playable map/slice can be launched in editor or documented command path.
4. The player can move/look/interact in first person.
5. Fire/ember state exists and can change.
6. Rain state exists and affects fire/ember state.
7. A short jungle route exists.
8. A crossing changes state.
9. Evidence-of-hands moment is present.
10. A distant watcher cue is present.
11. Relevant state persists during the slice.
12. No generated/cache/build junk is staged.
13. Binary assets are minimal, named, and justified.
14. The slice remains nonviolent and physically grounded.

## Warning Criteria

Warnings are acceptable if recorded and non-blocking:

- placeholder art is ugly but scoped;
- package/cook remains slow or timeboxed if editor/build path works;
- CEF/embedded browser warnings continue but core editor is stable;
- local Zen DDC warning persists but build/editor validation passes;
- minimal Blueprint wrappers exist only for placement.

## Blockers

Gate 005 must stop if:

- first-person spawn/control cannot be made to run;
- C++ compile fails and the fix is outside the gate;
- project open fails for engine-version reasons;
- required binary assets balloon beyond reviewable scope;
- implementation requires paid assets/plugins;
- the slice becomes combat/gore/chase/monster framed;
- the diff stages generated build/cache folders;
- the map/content footprint cannot be inspected well enough to review.

## Expected Gate 005 File Areas

Likely paths:

```text
Source/JungleGame/
Config/
Content/JungleCell0/
docs/
```

Gate 005 should update validation notes with:

- build command/result;
- editor/commandlet open result;
- PIE or launch path;
- package/cook result or blocker;
- binary asset list and justification;
- known warnings.

## Asset Lane Note

Codex App / Computer Use may later be used as a scoped asset/tool operator, especially through Blender or UE GUI, but only under explicit asset tickets. For Gate 005, asset work should remain placeholder-grade unless a specific prototype asset is intentionally scoped.
