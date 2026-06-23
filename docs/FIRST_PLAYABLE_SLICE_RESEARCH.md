# First Playable Slice Research

## Executive Summary

Gate 004 should produce the implementation-ready plan for **Jungle Cell 0: Embers**. This is a documentation/planning gate, not a gameplay implementation gate.

The first playable should be a compact authored loop around one camp, one fire/ember pressure system, one rain turn, one short jungle traversal path, one bridge/crossing state change, one evidence-of-hands moment, and one distant nonviolent watcher cue.

The purpose is not to build an open world, full survival sim, full AI simulation, or content bible. The purpose is to make Gate 005 implementable without inventing scope.

Core thesis to preserve:

> You are not being hunted. You are being evaluated.

## Gate 004 Goal

Convert the concept for `Jungle Cell 0: Embers` into a bounded, implementation-ready first playable plan.

Gate 004 must define:

- What the player does in the slice.
- Which systems are real mechanics.
- Which systems are stubs.
- Which systems are explicitly out of scope.
- What Gate 005 implementation may modify.
- What Gate 005 implementation must not modify.
- What pass/warning/blocker criteria prove readiness.

## Sources and Confidence

| Source area | Confidence | Used for |
|---|---:|---|
| Epic UE5 feature docs | High for feature mapping | PCG, World Partition/Data Layers, Niagara, MetaSounds, Chaos, first-person template, SaveGame, C++/Blueprint split. |
| Vertical slice / prototype references | Medium | Keeps slice small and integrated rather than sprawling. |
| Environmental storytelling references | Medium-high | Supports horror through place, implication, object change, and ambiguity. |
| Rainforest ecology references | High for broad environmental pressures | Rain, humidity, canopy darkness, rot, mud, poor visibility, sound masking. |
| Archaeology/material culture references | Medium | Broad material lane only; avoid exact culture claims without expert research. |
| Project-specific synthesis | Medium | Beat order, gate policy, and implementation boundaries. |

## Design Pillars

1. **First-person spatial presence is mandatory.** The slice must feel like occupying a dense, damp, enclosed jungle space, not reading UI meters over a backdrop.

2. **The jungle is normal.** It is not magic and not an enemy faction. Pressure comes from rain, rot, darkness, sound, mud, humidity, insects, water, slopes, and limited visibility.

3. **The watcher is nonviolent.** The unsettling part is not being attacked; it is realizing something intelligent is watching, testing, and changing affordances.

4. **The evidence is physical.** The player infers intelligence from tied vines, moved firewood, changed crossings, marks, tracks, and object placement.

5. **The first slice must be tiny.** One camp, one crossing, one weather turn, one evidence beat, one watcher cue.

## Slice Boundary

Included:

- One compact jungle cell.
- One camp.
- One fire/ember pressure object.
- One rain/weather state change.
- One short traversal path.
- One crossing/bridge/log/vine/stone route.
- One changed crossing state.
- One evidence-of-hands beat.
- One watcher presence cue.
- Minimal state persistence.
- Linux validation notes.

Excluded:

- Full open world.
- Full inventory.
- Full crafting.
- Full ecology simulation.
- Full AI simulation.
- Combat.
- Gore.
- Modern tools.
- Fantasy magic.
- Dialogue/language system.
- Tribe/cannibal/monster framing.
- VR.
- Multiplayer.
- Paid assets/plugins.

## Player Experience Target

The player should experience this sequence:

1. Arrive at camp before or near dusk.
2. Notice fire/embers matter.
3. Tend the fire.
4. Leave camp to cross a short jungle route.
5. Rain begins or intensifies.
6. The player feels time/ember pressure.
7. The crossing or path changes while the player is away.
8. The player discovers evidence that something with hands changed the environment.
9. A distant watcher cue confirms presence without confrontation.
10. The player returns to camp with changed state preserved.

Emotional target:

```text
I am not alone.
It saw what I did.
It changed something deliberately.
It did not attack.
It is testing me.
```

## Core Loop

```text
Maintain camp stability
→ leave camp for a necessary reason
→ jungle/weather applies pressure
→ environment changes while away
→ player infers intelligent presence
→ return to changed camp/world state
```

## Required Slice Beats

| Beat | Purpose | Required? |
|---|---|---:|
| Spawn in camp | Establish safe-ish center | Yes |
| Fire/ember visible | Establish survival pressure | Yes |
| Rain state | Stress the ember/camp | Yes |
| Short route | Prove first-person traversal | Yes |
| Crossing | Creates place for manipulation | Yes |
| Changed crossing | Proves environmental agency | Yes |
| Evidence-of-hands | Makes intelligence legible | Yes |
| Watcher cue | Locks tone | Yes |
| Return/persistence | Proves world state | Yes |

## Systems To Represent

Real mechanics for Gate 005:

- First-person movement/look/interact.
- Fire/ember state.
- Rain/weather state affecting fire.
- Crossing state change.
- Minimal event state persistence.
- Watcher presence trigger.

## Systems To Stub

Stub or fake for Gate 005:

- Jungle ecology.
- Hominin cognition.
- Weather system beyond one rain state.
- Fire chemistry/fuel taxonomy.
- Save/load beyond slice needs.
- Audio system beyond placeholder cues.
- Visual effects beyond placeholder rain/fire.
- Procedural generation beyond hand-placed or tiny proof.

## Systems Explicitly Out Of Scope

- Combat/damage/death loop.
- Full survival meters.
- Full inventory/crafting.
- Hunger/thirst/disease/injury.
- Full save/load UI.
- Animal AI.
- Hominin AI.
- Dialogue/language.
- Quest/objective system.
- Full map/biome.
- Marketplace/Fab asset pipeline.
- Production foliage pipeline.
- Cinematics.

## Environmental Interaction Scope

Allowed interactions:

- Tend fire.
- Inspect or interact with crossing.
- Observe changed object state.
- Follow/notice path markers or physical clues.
- Trigger simple watcher/evidence events.

Not allowed yet:

- Cutting trees.
- Building shelter system.
- Crafting tools.
- Full physics manipulation.
- Destruction system.
- Rope simulation.

## Fire / Embers / Rain Scope

Fire/embers should be the simplest survival pressure:

- Fire has state: dead / ember / burning.
- Ember decays over time.
- Rain accelerates decay.
- Player can tend fire through one interaction.
- Fire state changes light/audio/visual placeholder state.
- Fire state participates in persistence.

Do not build:

- full combustion simulation;
- full fuel inventory;
- cooking;
- fire spread;
- burn damage;
- thermal simulation.

## Camp Persistence Scope

Minimal persistence should prove that the world remembers meaningful slice state.

Allowed:

- Fire state persists during the slice.
- Crossing/evidence/watcher events do not reset immediately.
- Optional single-slot save only if Gate 005 requires restart persistence.

Preferred for first playable:

- Same-session persistence unless Gate 004 explicitly requires disk persistence.

Do not build:

- save/load menu;
- multiple profiles;
- checkpoint UI;
- inventory persistence;
- save migration.

## Bridge / Crossing State Scope

The crossing is the first strong proof of outside intelligence.

Allowed crossing forms:

- log crossing;
- vine/branch bridge;
- stepping stones;
- root crossing;
- small creek crossing.

Allowed state change:

- vine tied/untied;
- branch shifted;
- stones rearranged;
- route made easier/harder;
- marker changed;
- player's previous action mirrored or modified.

Meaning:

```text
Something observed the player.
Something used hands.
Something changed the path.
It is not attacking.
It is evaluating.
```

Do not build:

- construction system;
- damage trap;
- rope physics;
- puzzle chain;
- chase sequence.

## Evidence-Of-Hands Moment

Allowed evidence:

- arranged sticks;
- tied vine;
- moved firewood;
- scraped bark;
- stone pattern;
- partial handprint;
- changed bridge lash;
- tool-like mark in mud/wood;
- dry material moved into rain.

Forbidden evidence:

- gore;
- corpses;
- cannibal signs;
- horror shrine;
- racist tribal coding;
- fantasy runes;
- modern object;
- explicit writing/dialogue;
- cutscene explanation.

## Watcher Presence Scope

The watcher should be presence only.

Allowed:

- distant silhouette;
- rustle/audio cue;
- movement behind leaves;
- object changes after line-of-sight break;
- one watcher marker;
- one simple visibility event;
- disappears before contact.

Forbidden:

- combat;
- chase;
- damage;
- grab/capture;
- dialogue;
- behavior tree;
- perception architecture;
- full NPC sim;
- detailed model/animation;
- monster framing;
- fail state.

## 1000 CE Material Culture Lane

Use broad material categories only:

- wood;
- vine/fiber;
- bark;
- stone;
- clay;
- bone/shell;
- fire/charcoal;
- woven basket/pack/gourd-like containers if needed.

Avoid exact named cultural claims unless separately researched. The setting should remain fictional/tropical unless the project later chooses a specific cultural/geographic model with care.

## UE5 System Mapping For Later Implementation

| UE5 system | Gate 005 use | Stance |
|---|---|---|
| GameMode | Default player/map rules | Minimal |
| Pawn/Character | First-person shell | Real |
| Enhanced Input | Move/look/interact | Real |
| Actors/Components | Fire, crossing, evidence, watcher marker | Real, tiny |
| SaveGame | Optional persistence | Use only if needed |
| Data Assets | Optional tuning | Avoid framework creep |
| Niagara | Rain/fire placeholders | Minimal |
| MetaSounds/audio | Rain/fire/watcher cue | Optional/minimal |
| PCG | Jungle dressing | Defer unless tiny proof approved |
| World Partition | Large world streaming | Defer |
| Data Layers | Runtime layer/state | Defer |
| Chaos | Physics | Defer heavy simulation |

## Pass / Warning / Blocker Table

| Area | Pass | Warning | Blocker |
|---|---|---|---|
| Slice size | One compact authored cell | Slightly broad but still bounded | Open-world scope |
| Fire/rain | Simple observable pressure | Debug-only visuals | No survival pressure visible |
| Crossing | Clear changed affordance | Needs art clarity | Reads as random bug |
| Evidence | Implies hands/intelligence | Too subtle | Gore/fantasy/racist/modern framing |
| Watcher | Nonviolent presence cue | Too obvious/subtle | Combat/chase/full AI |
| Persistence | Required state remembered | Only session-local | State resets before validation |
| UE scope | Plan only | Needs Gate 005 details | Implementation in Gate 004 |

## Gate 005 Readiness Checklist

Gate 005 is ready only when Gate 004 defines:

- Exact slice loop.
- Required systems.
- Stub systems.
- Forbidden systems.
- Player path/camp/crossing/evidence/watcher beats.
- Persistence target.
- Binary asset policy.
- Linux validation expectations.

## PR Scope Recommendation

Recommended target file: `docs/FIRST_PLAYABLE_SLICE_RESEARCH.md`

Allowed PR changes:

- Add/update first playable slice research docs.
- Add/update implementation scope boundaries for Gate 005.
- Add/update pass/fail criteria for Jungle Cell 0: Embers.
- Update Gate 004 status only if the plan is complete.
- Update active next gate only if Gate 004 has enough detail to pass.

Forbidden PR changes:

- No gameplay implementation.
- No `.uproject` changes.
- No `Source/` changes.
- No `Config/` changes.
- No `Content/` changes.
- No code changes.
- No binary assets.
- No level creation.
- No Blueprint creation.

Gate recommendation:

- Gate 004 status: **Research/scope ready.**
- Is Gate 005 implementation scope ready?: **Mostly yes after Gate 003 project state is known.**
- Next active gate: **Gate 005 after Gate 003 project bootstrap and this Gate 004 plan are merged.**
- Blockers: engine/project not bootstrapped yet; exact project/module/map state unknown.
- Warnings: PCG, World Partition, full AI, full survival, and art production are major scope traps.
- Manual decisions required: exact crossing type, persistence target, watcher cue type, and whether Cell 0 uses simple level only.

## Sources

- Epic Games — PCG Overview: https://dev.epicgames.com/documentation/unreal-engine/procedural-content-generation-overview
- Epic Games — World Partition: https://dev.epicgames.com/documentation/unreal-engine/world-partition-in-unreal-engine
- Epic Games — Lumen Global Illumination and Reflections: https://dev.epicgames.com/documentation/unreal-engine/lumen-global-illumination-and-reflections-in-unreal-engine
- Epic Games — Nanite Virtualized Geometry: https://dev.epicgames.com/documentation/unreal-engine/nanite-virtualized-geometry-in-unreal-engine
- Epic Games — Creating a New Project: https://dev.epicgames.com/documentation/unreal-engine/creating-a-new-project-in-unreal-engine
- Rami Ismail — Prototypes and Vertical Slice: https://ltpf.ramiismail.com/prototypes-and-vertical-slice/
- Ask A Game Dev — Vertical Slice glossary: https://askagamedev.tumblr.com/post/77406994278/game-development-glossary-the-vertical-slice
- Game Developer — Environmental storytelling: https://www.gamedeveloper.com/design/what-you-give-is-what-you-get-environmental-storytelling-in-games
- National Geographic Education — Rainforest: https://education.nationalgeographic.org/resource/rain-forest/
- Britannica — Tropical rainforest environment: https://www.britannica.com/science/tropical-rainforest/Environment
- Queensland Government WetlandInfo — Rainforest processes: https://wetlandinfo.detsi.qld.gov.au/wetlands/ecology/processes-systems/nitrogen-concept-model/rainforest/processes.html
