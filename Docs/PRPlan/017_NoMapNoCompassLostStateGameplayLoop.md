# No-Map/No-Compass Lost-State Gameplay Loop

## Roadmap Item

Roadmap PR #17 — `No-map/no-compass lost-state gameplay loop`

## Goal

Implement the first source/docs scaffold for lost-state pressure and recovery without adding map, compass, bearing, GPS, clock, objective marker, character commentary, or orientation-confidence rescue UI.

Getting lost is gameplay. The world should make the player question routes, experience pressure, and recover through environmental evidence rather than UI rescue.

## Dependency

This pass depends on:

- Roadmap PR #16 — `Navigation pillars: sun, sky, sound, coast, creeks, ridges, landmarks` — merged to `main`.

It consumes:

- `FJungleNavigationPillarsSpec` for diegetic cue pillars, cue degradation, cue readability, environmental state hooks, player-made trail mark contracts, and forbidden UI guard functions.
- `FJunglePCGBiomeInputSample` for terrain/mask sample data.
- `FJungleEcosystemMaskSpec` for blocker, affordance, false-affordance, footing, night danger, visibility, sky-window, sound propagation, canopy, wetness, and disturbance masks.

## Source Scaffold

`Source/JungleGame/JungleLostStateGameplaySpec.h` adds:

- `EJungleLostStatePressurePhase` for internal pressure phases: grounded by world evidence, questioning route, lost pressure, recovery evidence available, and critical exposure pressure.
- `EJungleLostStateEvidenceSource` for distance-from-known-safety, readable cue loss, darkness, rain-muffled sound, canopy occlusion, gorge compression, steep slopes, hard blockers, false affordances, trail-mark degradation, known-safety anchors, and recovery cues.
- `EJungleLostRecoveryCue` for diegetic recovery evidence: player-made trail marks, creek flow, coast/ocean sound, ridge sky windows, mountain silhouettes, slope fall-line evidence, southern-sky windows, and known-safety line of sight.
- `FJungleLostStateCueSnapshot` for developer-only counts of readable, degraded, and unavailable world cues.
- `FJungleLostStateInputSample` for distance, terrain, cue, trail-mark, known-safety, recovery, darkness, rain, and false-affordance inputs.
- `FJungleLostStateRuleContract` for pressure/recovery rule contracts with explicit forbidden UI guard flags.
- `FJungleLostStateGameplaySpec` for pressure scoring, pressure phase resolution, recovery evidence detection, required mask names, rule contracts, and developer-only debug output names.

## Gameplay Intent

### Lost-State Pressure

Lost-state pressure is derived from environmental conditions:

- distance from known safety;
- cue loss or cue degradation;
- darkness pressure;
- rain-muffled sound;
- dense canopy occlusion;
- gorge compression;
- steep slopes and footing risk;
- hard blockers and soft blockers;
- false affordances;
- trail-mark degradation.

This is not a player-facing meter. It is an internal source/docs contract for future gameplay logic and developer-only validation.

### Recovery Evidence

Recovery should come from the same world systems that made the player lost:

- player-made trail marks that still survive rain, visibility, and canopy conditions;
- creek flow and water sound;
- coast/ocean sound or coastline evidence;
- ridge sky windows;
- mountain silhouettes;
- slope fall-line evidence;
- southern-sky windows at night;
- known-safety line of sight where a shelter, fire, smoke, or other anchor actually exists and is not magically global.

Recovery evidence must remain imperfect and diegetic. It must not become route-line navigation or a UI rescue state.

### Known Safety

Known safety is a discovered-world concept. Future systems may define shelter, fire, smoke, camp, or safe terrain anchors, but PR #17 only defines contracts for distance and line-of-sight pressure.

Known-safety anchors must not become:

- GPS points;
- objective markers;
- minimap icons;
- compass bearings;
- global beacons;
- character callouts telling the player they are or are not oriented.

### Player-Made Trail Marks

Player-made trail marks are runtime interactions. They can help recovery if visible and intact, but they can degrade or disappear under rain, canopy occlusion, wetness, terrain disturbance, or line-of-sight constraints.

Trail marks remain runtime persistence and must not become stable PCG generation inputs.

## Navigation Rules Preserved

Forbidden across this pass:

- player map;
- minimap;
- compass;
- bearing number/readout;
- GPS marker;
- objective marker navigation;
- objective arrow;
- HUD clock;
- explicit time readout;
- modern navigation tools;
- orientation-confidence UI/system;
- hidden orientation meter;
- character comments or UI-like feedback telling the player whether they are oriented;
- full survival stat stack;
- unrelated combat, enemy, or story systems.

`FJungleLostStateGameplaySpec` explicitly rejects these categories through guard functions and rule-contract flags.

## PR Boundary

This pass stays isolated from later roadmap items:

- PR #18 owns day/night/weather danger prototype behavior.
- PR #19 owns Cell 0 placement inside the large world.
- PR #20 owns packaged large-world first-playable smoke, performance checks, and release notes.

PR #17 only defines the first no-map/no-compass lost-state pressure and recovery scaffold.

## Developer-Only Debug Outputs

The scaffold defines debug output names for reviewer validation and future editor-only tools:

- `lost_state_rule_contracts`
- `lost_state_distance_pressure`
- `lost_state_cue_loss_pressure`
- `lost_state_terrain_pressure`
- `lost_state_recovery_evidence`
- `lost_state_trail_mark_runtime_interaction`
- `lost_state_forbidden_ui_absence_check`

These outputs must remain developer-only. They must not become player-facing map, compass, bearing, clock, objective marker, lost meter, or orientation-confidence UI.

## Validation Notes

Validation for this pass is source/docs based because it intentionally avoids UI implementation, final weather, final audio, final survival stat stack, final trail-mark mechanics, broad UE editor assets, generated cache output, and binary churn.

Expected checks:

- inspect `Source/JungleGame/JungleLostStateGameplaySpec.h` and this document;
- confirm the scaffold consumes `FJungleNavigationPillarsSpec` from PR #16;
- confirm pressure rules use distance, cue loss, darkness, rain, canopy, gorge, slope, hard/soft blockers, false affordances, trail marks, and known-safety/recovery evidence;
- confirm recovery evidence is diegetic and imperfect;
- confirm player-made trail marks remain runtime interactions and are not accepted as PCG generation inputs;
- confirm no forbidden navigation/time/orientation UI is added;
- confirm no character commentary tells the player whether they are oriented;
- confirm no generated/cache/editor junk is staged;
- confirm work remains isolated from PR #18 day/night/weather danger prototype and PR #19 Cell 0 integration.

UE commandlet/build validation remains dependent on `UnrealEditor` being available on PATH in the local shell.

## Acceptance Criteria

- Lost-state gameplay source scaffold exists.
- Pressure phases, pressure evidence, recovery cues, input samples, rule contracts, and debug output names are reviewable.
- Distance-from-known-safety, cue loss, darkness, rain-muffled sound, dense canopy, gorge compression, steep slopes, blockers, false affordances, trail-mark degradation, known safety, and recovery cues are represented.
- Player-made trail mark interactions are planned without becoming PCG generation inputs or UI rescue.
- The no-map/no-compass/no-clock/no-objective-marker/no-orientation-confidence rule remains intact.

## Deferred To Later Roadmap PRs

- Day/night/weather danger implementation.
- Final weather simulation.
- Final audio mix.
- Final trail-mark interaction mechanics.
- Survival stat stack.
- Damage/death system.
- Cell 0 integration inside the large world.
- Packaged first-playable smoke/performance validation.
