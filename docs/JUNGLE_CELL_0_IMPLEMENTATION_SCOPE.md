# Jungle Cell 0 Implementation Scope Research

## Executive Summary

`Jungle Cell 0: Embers` should be the first real playable slice for Jungle Game: a compact, Linux-first UE5 first-person survival-simulation prototype proving control feel, jungle traversal, ember pressure, rain state, camp persistence, crossing manipulation, environmental evidence, and distant watcher presence.

This document defines the allowed implementation scope for a later PR. It is not an implementation patch, not a prompt, not a game design bible, and not approval to add large architecture.

The future implementation must stay narrow:

- first-person control shell;
- one compact jungle traversal cell;
- one simple fire/ember survival loop;
- one simple rain/weather state that affects fire;
- only the slice states that matter;
- one bridge/crossing state change;
- one evidence-of-hands environmental event;
- one nonviolent watcher-presence stub;
- Linux build/cook/smoke notes.

Core thesis:

> You are not being hunted. You are being evaluated.

## Gate 005 Goal

Implement the first playable version of:

```text
Jungle Cell 0: Embers
```

Gate 005 should prove that the game works as a first-person spatial survival-simulation with nonviolent social horror.

Target loop:

```text
Camp -> Ember pressure -> Rain pressure -> Jungle traversal -> Crossing change -> Evidence of hands -> Watcher cue -> Return to persistent camp
```

## Required Inputs From Gate 004

Gate 005 depends on:

- UE version selected.
- Minimal UE C++ project already bootstrapped.
- First playable slice plan merged.
- Project/module name known.
- Existing input/map/content conventions known.
- Binary asset policy known.
- Linux build/editor baseline known.

If these are missing, Gate 005 may inspect and document blockers, but should not invent broad architecture.

## Slice Success Definition

The slice passes when a player can:

1. Launch the slice in Linux editor PIE, and ideally in a packaged Linux build.
2. Spawn in a dense jungle camp blockout.
3. Move/look/interact in first person.
4. Tend or inspect fire/embers.
5. See fire/ember state change over time.
6. See rain/weather affect fire/ember pressure.
7. Leave camp and traverse a short jungle path.
8. Reach a bridge/crossing.
9. Observe a clear crossing state change.
10. Discover evidence that something with hands changed the environment.
11. Notice a distant watcher presence without combat, gore, chase, or exposition.
12. Confirm relevant state persists according to the agreed target.

## Future Implementation PR Allowed Scope

Allowed paths in the future implementation PR:

```text
Source/
Config/
Content/
docs/
```

Allowed implementation areas:

- First-person shell.
- Compact jungle traversal blockout.
- Fire/ember prototype.
- Rain/weather prototype.
- Camp persistence prototype.
- Bridge/crossing state prototype.
- Evidence-of-hands prototype.
- Watcher presence stub.
- Linux build/cook/smoke validation notes.

Allowed source changes:

- Minimal gameplay classes.
- Minimal player pawn or character class.
- Minimal player controller if needed.
- Minimal GameMode setup if needed.
- Minimal interactable/state actors.
- Minimal components for reusable slice state.
- Optional small subsystem only if it clearly reduces coupling.
- Optional minimal SaveGame object if restart persistence is required.

Allowed config changes:

- Default map.
- Default GameMode.
- Input mappings.
- Collision channel additions only if justified.
- Packaging settings only if required for Linux validation.

Allowed content changes:

- One compact playable map.
- Minimal placeholder materials.
- Minimal placeholder static meshes.
- Minimal input assets.
- Minimal rain/fire/audio placeholders.
- Minimal Blueprint wrappers only where they expose C++ state or enable editor placement.

## Future Implementation PR Forbidden Scope

Forbidden:

- Combat systems.
- Weapons.
- Gore.
- Monster attacks.
- Cannibal/lost-tribe pulp framing.
- Racist tribe/primitive-human caricature framing.
- Fantasy magic.
- Modern/current-day tools.
- Firearms.
- Flashlights.
- Cameras.
- Radios.
- Batteries.
- GPS.
- Drones.
- Modern survival gear.
- VR.
- Multiplayer.
- Online subsystem work.
- Full open world.
- Full inventory.
- Full crafting.
- Full AI behavior tree/perception architecture.
- Full ecology simulation.
- Dialogue system.
- Quest system.
- Full save/load UI.
- Steam packaging.
- Marketplace/Fab asset packs.
- Paid assets/plugins.
- Heavy Blueprint architecture.
- Generated/cache/build outputs.

## Minimal UE5 System Map

| UE5 System | Cell 0 Use | Stance |
|---|---|---|
| GameMode | Select default player and map rules | Real, minimal |
| Pawn/Character | First-person player shell | Real |
| PlayerController | Input/possession if needed | Real if needed |
| Enhanced Input | Move/look/interact | Real, small |
| Actors | Campfire, crossing, evidence, watcher marker | Real |
| Actor Components | Ember/interactable/weather-reactive state | Real only if shared |
| Subsystem | Slice state owner | Optional |
| SaveGame | Restart persistence | Optional/minimal |
| Data Assets | Tunable fire/weather values | Optional |
| PCG | Jungle dressing | Defer by default |
| Niagara | Rain/fire placeholders | Minimal |
| MetaSounds/audio | Jungle/rain/fire/watcher audio | Optional/minimal |
| Chaos | Crossing physics | Mostly defer |
| World Partition | Large world streaming | Defer |
| Data Layers | Runtime layers | Defer |

## First-Person Shell Scope

Allowed:

- Walk.
- Look.
- Interact.
- Crouch only if the path needs it.
- Jump only if traversal needs it.
- Optional sprint only if it does not introduce stamina.
- First-person camera.
- Basic collision.
- Basic slope/step traversal.
- Minimal debug HUD/logs for validation.

Forbidden:

- Weapon handling.
- Combat stance.
- Full body animation.
- First-person arms polish.
- Parkour/climbing/swimming systems.
- Stamina/exhaustion/injury.
- Immersive body IK.

## Jungle Traversal Blockout Scope

Allowed:

- One camp area.
- One short path network.
- One bridge/crossing.
- One return route or loopback.
- Dense visual blockers.
- Terrain slope/root/rock obstacles.
- Placeholder foliage/trees/rocks/vines.
- Wet ground indicators.
- Hand-placed blockout geometry.

Forbidden:

- Full biome.
- Kilometer-scale map.
- Large procedural generation.
- Streaming production pipeline.
- Animal ecology.
- High-density asset packs.
- Photoreal art pass.

## Fire / Ember Prototype Scope

Allowed:

- Fire states: dead / embers / burning.
- Ember value or warmth value.
- Ember decay over time.
- Rain accelerates ember decay.
- Player can tend the fire with one interaction.
- Fire affects light/audio/visual state.
- Fire state persists according to slice target.

Forbidden:

- Full combustion simulation.
- Fuel taxonomy.
- Inventory/crafting/cooking.
- Fire spread.
- Burns/heat transfer/smoke inhalation.

## Rain / Weather Prototype Scope

Allowed:

- Weather states: clear / wet / raining.
- Deterministic weather transition.
- Manual debug trigger if needed.
- Rain affects ember decay.
- Rain affects visuals/audio.

Forbidden:

- Full climate simulation.
- Seasonal simulation.
- Cloud simulation.
- Dynamic hydrology.
- Flooding/lightning system.
- Global material wetness framework.

## Camp Persistence Prototype Scope

Allowed persistent states:

- Fire/ember state.
- Whether fire was tended.
- Weather state if relevant.
- Bridge/crossing state.
- Evidence event state.
- Watcher event state.

Preferred:

- Same-session persistence unless Gate 004 requires restart persistence.
- One explicit minimal save object only if restart persistence is required.

Forbidden:

- Save/load menu.
- Multiple profiles.
- Autosave UX.
- Inventory/player stat persistence.
- Save migration/checkpoint framework.

## Bridge / Crossing State Prototype Scope

Allowed state changes:

- Rope/vine moved.
- Vine tied or untied.
- Log shifted.
- Branch broken or placed.
- Stone path rearranged.
- Crossing blocked or altered.
- Route readability changed after the player leaves.

Forbidden:

- Physics destruction set piece.
- Complex rope simulation.
- Bridge-building system.
- Trap damage.
- Combat encounter.
- Full puzzle chain.

Design goal:

```text
Something observed the player.
Something used hands.
Something changed the affordance.
It is not attacking.
It is evaluating.
```

## Evidence-Of-Hands Prototype Scope

Allowed evidence:

- Arranged sticks.
- Arranged stones.
- Tied vine.
- Moved object near camp.
- Scraped mark.
- Cleared patch.
- Fire altered while player was away.
- Bridge/crossing manipulated.
- Natural material placed in intentional pattern.

Forbidden evidence:

- Corpses.
- Gore.
- Cannibal signs.
- Horror shrine.
- Racist tribal coding.
- Written language system.
- Fantasy rune.
- Modern object.
- Direct exposition.
- Cutscene/dialogue/jump scare.

## Watcher Presence Stub Scope

Allowed:

- Distant silhouette.
- Rustle/audio cue.
- Movement behind foliage.
- Object change after line-of-sight break.
- One placed watcher marker.
- One simple visibility event.
- Disappears before contact.
- Nonviolent observation beat.
- Debug log confirming trigger.

Forbidden:

- Combat.
- Chase.
- Damage.
- Grab/capture.
- Dialogue.
- Behavior tree.
- Full NPC simulation.
- Detailed model/animation.
- Monster framing.
- Fail state.

## Asset And Binary File Policy

Allowed future PR assets:

- One minimal Cell 0 map.
- Minimal assets required for input, placeholder materials, placeholder meshes, placeholder effects, placeholder audio, or Blueprint wrappers.
- Minimal docs.

Forbidden committed files:

```text
Binaries/
Intermediate/
Saved/
DerivedDataCache/
.vs/
.idea/
packaged builds
local editor logs
shader caches
crash dumps
large imported asset packs
paid plugin files
Marketplace or Fab dumps
temporary screenshots/videos
```

Policy:

- Binary assets must be minimal and intentional.
- Asset additions must be named around Cell 0.
- Do not commit generated build/cook/package outputs.
- If a packaged build is required for validation, document command/result rather than committing the build artifact.

## Performance Risk Notes

Target concern:

```text
RTX 2080 8 GB
Linux-first UE5 development
Dense tropical jungle
```

Primary risks:

| Risk | Why It Matters | Cell 0 Mitigation |
|---|---|---|
| Foliage overdraw | Dense leaves can dominate GPU cost | Keep foliage simple and bounded |
| Shadow cost | Canopy/fire lights can be expensive | Conservative lighting |
| VRAM pressure | 8 GB is viable but tight | Placeholder materials/textures |
| Actor count | Thousands of actors create overhead | Use grouped/instanced placeholders |
| Shader churn | Material experimentation slows iteration | Keep materials simple |
| Rain/fire VFX | Translucency/lights can stack | Bound Niagara/audio work |
| PCG overreach | Procedural generation can explode scope | Defer unless approved |
| World scale creep | Large maps delay playable loop | One compact cell |

## Linux Build / Cook / Smoke Validation Scope

Minimum validation:

- Project opens on Linux.
- Cell 0 map opens.
- PIE launches.
- Player spawns.
- Move/look/interact works.
- Fire/ember state changes.
- Rain affects ember state.
- Bridge/crossing state changes.
- Evidence event appears.
- Watcher stub triggers.
- No fatal log spam.
- No missing asset/map/input failure.
- Linux cook/package path is documented.
- Packaged Linux build launches if environment supports it.

## Pass / Warning / Blocker Table

| Area | Pass | Warning | Blocker | Future Work |
|---|---|---|---|---|
| First-person shell | Player can move/look/interact | Camera feel rough | Player cannot spawn/control | Body/arms polish |
| Jungle blockout | Compact path playable | Visual dressing sparse | Route impossible | Larger biome |
| Fire/ember | Fire state observable | Debug visuals crude | Fire loop invisible | Deep fire sim |
| Rain/weather | Rain affects fire | Manual trigger only | Weather breaks flow | Climate system |
| Camp persistence | Required states persist | Session-only | State resets before target | Save UI |
| Crossing | Clear changed route | Visual crude | Blocks completion | Rope/physics |
| Evidence | Natural intentional clue appears | Needs art pass | Gore/fantasy/modern/racist read | Lore chain |
| Watcher | Nonviolent presence triggers | Too subtle/obvious | Combat/chase/full AI | AI sim |
| Linux editor | PIE works | Minor warnings | Project/map will not open | Automation |
| Performance | Playable enough | Some dips | Severe stutter/crash/VRAM | Profiling pass |
| Scope hygiene | Intended files only | Minor cleanup | Generated/cache/asset dump | Asset pipeline |

## Future Work Explicitly Deferred

Deferred:

- Full survival system.
- Hunger/thirst/injury/disease.
- Cooking/inventory/crafting/toolmaking.
- Animal AI.
- Hominin AI.
- Dialogue/language/lore bible.
- Quest system.
- Full save/load UI.
- Multiple saves.
- Full open world.
- PCG biome generation.
- Nanite foliage production pass.
- Full weather/climate/water system.
- Combat/horror chase systems.
- VR/multiplayer.
- Steam packaging.
- Marketplace asset integration.
- Final art/audio/cinematics.

## PR Scope Recommendation

Recommended target file: `docs/JUNGLE_CELL_0_IMPLEMENTATION_SCOPE.md`

Allowed future implementation PR changes:

- First-person shell.
- Compact jungle blockout.
- Fire/ember prototype.
- Rain/weather prototype.
- Camp persistence prototype.
- Bridge/crossing state prototype.
- Evidence-of-hands prototype.
- Watcher presence stub.
- Linux build/cook/smoke validation notes.

Forbidden future implementation PR changes:

- No combat systems.
- No gore.
- No modern tools/current-day setting elements.
- No VR.
- No multiplayer.
- No large asset packs.
- No paid assets/plugins.
- No full open world.
- No complex NPC simulation.
- No heavy Blueprint architecture.
- No generated/cache files.

Gate recommendation:

- Gate 005 status: **Ready as implementation-scope document.**
- Is implementation ready?: **Conditionally yes, after Gate 004 inputs and repo inspection are confirmed.**
- Next likely gate: **Gate 006 — Cell 0 validation and Linux smoke result.**
- Blockers: missing Gate 004 plan, unknown engine version, unknown Linux build baseline, unknown project/module names.
- Warnings: dense jungle can become rendering scope; watcher can become full AI; persistence can become full save system.
- Manual decisions required: exact UE version, exact persistence target, crossing type, watcher cue type, and whether package must pass now.

## Sources

- Epic Games — Gameplay Framework: https://dev.epicgames.com/documentation/unreal-engine/gameplay-framework-in-unreal-engine
- Epic Games — Enhanced Input: https://dev.epicgames.com/documentation/unreal-engine/enhanced-input-in-unreal-engine
- Epic Games — Programming Subsystems: https://dev.epicgames.com/documentation/unreal-engine/programming-subsystems-in-unreal-engine
- Epic Games — Saving and Loading: https://dev.epicgames.com/documentation/unreal-engine/saving-and-loading-your-game-in-unreal-engine
- Epic Games — PCG Overview: https://dev.epicgames.com/documentation/unreal-engine/procedural-content-generation-overview
- Epic Games — Niagara Overview: https://dev.epicgames.com/documentation/unreal-engine/overview-of-niagara-effects-for-unreal-engine
- Epic Games — MetaSounds: https://dev.epicgames.com/documentation/unreal-engine/metasounds-the-next-generation-sound-sources-in-unreal-engine
- Epic Games — Physics / Chaos: https://dev.epicgames.com/documentation/unreal-engine/physics-in-unreal-engine
- Epic Games — Build Operations: https://dev.epicgames.com/documentation/unreal-engine/build-operations-cooking-packaging-deploying-and-running-projects-in-unreal-engine
- Ask A Game Dev — Vertical Slice glossary: https://askagamedev.tumblr.com/post/77406994278/game-development-glossary-the-vertical-slice
- Frictional Games — Defenseless protagonist design: https://frictionalgames.com/2017-05-6-reasons-for-having-a-defenseless-protagonist/
