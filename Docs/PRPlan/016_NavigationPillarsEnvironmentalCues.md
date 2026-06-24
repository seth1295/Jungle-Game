# Navigation Pillars: Environmental Cues

## Roadmap Item

Roadmap PR #16 — `Navigation pillars: sun, sky, sound, coast, creeks, ridges, landmarks`

## Goal

Implement the non-map navigation and terrain-first traversal cue scaffold as reviewable source/docs contracts.

The player should orient from environmental evidence: sun position, shadows, light colour, creek flow, slope, coastline/ocean evidence, ridges, spurs, saddles, mountain silhouettes, sky windows, southern-sky night logic, environmental sound, traversal affordances, and player-made trail marks.

This pass does not implement a final weather simulation, final audio mix, final sky/star art, lost-state gameplay, or player-facing navigation rescue UI.

## Dependency

This pass depends on the merged large-world and ecosystem scaffold:

- Roadmap PR #12 — `PCG biome framework`.
- Roadmap PR #13 — `Creek/riparian/wet valley ecosystem pass`.
- Roadmap PR #14 — `Ridge/slope/mountain ecosystem pass`.
- Roadmap PR #15 — `Lowland/alluvial/gorge ecosystem pass`.

It consumes:

- `FJunglePCGBiomeSpec` for terrain/mask sample vocabulary, biome-layer context, coast edge, creek influence, and terrain-friction contracts.
- `FJungleEcosystemMaskSpec` for normalized mask values and hard/soft blocker, affordance, and false-affordance classification.
- `FJungleMacroTerrainSpec` for slope, ridge, spur, saddle, basin, creek, coast, and blocker vocabulary.
- `FJungleCreekRiparianWetValleyEcosystemSpec` for creek influence, creek-bank/crossing, water sound, and wet-valley contracts.
- `FJungleRidgeSlopeMountainEcosystemSpec` for ridge sky windows, spur/saddle movement, slope-break decisions, mountain silhouettes, and exposed/sheltered aspect contracts.
- `FJungleLowlandAlluvialGorgeEcosystemSpec` for coast, creek-mouth, gorge, sound compression, sightline compression, and false-affordance contracts.

## Source Scaffold

`Source/JungleGame/JungleNavigationPillarsSpec.h` adds:

- `EJungleNavigationCuePillar` for sun, shadow, light colour, creek flow, slope, coast/ocean, ridge/spur/saddle, mountain landmark, sky-window, southern-sky, sound, traversal-affordance, and player-made trail-mark cue pillars.
- `EJungleNavigationCueModality` for visual light, visual landform, auditory environment, traversal friction, and runtime player mark channels.
- `EJungleNavigationCueDegrader` for dense canopy, cloud cover, rain muffling, gorge occlusion, night-without-sun, no sky window, and blocked line of sight.
- `EJungleNavigationCueReadability` for developer-only coarse cue state: unavailable, degraded, or readable.
- `FJungleNavigationEnvironmentalState` as transient canopy/sky/rain/gorge/night/ocean/landmark hooks. This is not final weather simulation.
- `FJungleNavigationCueContract` as the reviewable contract for future actors, components, materials, VFX placeholders, audio hooks, and validation views.
- `FJungleNavigationCueSample` as the source/docs sample surface for terrain masks plus transient cue signal hooks.
- `FJungleNavigationPillarsSpec` for forbidden UI guard functions, cue degradation helpers, cue-specific readability helpers, required mask names, cue contracts, and developer-only debug output names.

## Cue Pillar Intent

### Sun Position

Sun position is a diegetic visual cue only. It can be readable when daylight, sky windows, and visibility support it. Dense canopy, cloud cover, gorge occlusion, and night can degrade or remove it.

### Shadow Direction And Length

Shadow cues are contracts for world lighting and occluder readability. They must never become bearing numbers, compass UI, or explicit time readouts. Shadow direction and length can help the player infer time of day, but the game must not display a clock or exact time.

### Light Colour

Light colour cue contracts support morning/evening warmth, overhead daylight, canopy filtering, and weather/terrain degradation. This pass records the hook only; it does not implement final lighting art.

### Creek Flow

Creek flow cues consume creek influence, wetness, sound propagation, and affordance masks. The contract allows future water/edge/particle/audio hooks to show flow direction environmentally without drawing route lines or giving objective arrows.

### Slope Gradient

Slope cues consume slope, footing risk, hard blocker, soft blocker, and affordance masks. Slope fall-line data is developer/contract data only; it must not be exposed as a player bearing or direction meter.

### Coastline And Ocean

Coast/ocean cues use coast distance, ocean audibility, sound propagation, visibility, hard blockers, and false affordances. Ocean is the natural island boundary, but coastal routes may still be broken by mangroves, cliffs, creek mouths, rocky shore, or false affordances.

### Ridges, Spurs, Saddles, And Slope Breaks

Ridge/spur/saddle cues consume ridge/valley, slope, sky-window, visibility, and affordance masks. They should expose landform structure and movement pressure rather than prescribe paths.

### Mountain Landmarks

Mountain landmarks are skyline and silhouette contracts backed by height, ridge/valley, sky-window, and visibility masks. They must remain natural terrain silhouettes, not artificial signage.

### Sky Windows

Sky windows are openings in canopy or terrain where the player can observe sun, moon, stars, sky glow, weather, or silhouettes. Dense canopy, clouds, and terrain occlusion can remove them.

### Stars, Moon, And Southern Sky

Southern-sky logic is recorded as a night cue contract. It depends on night state, visible sky windows, moon visibility, southern-sky visibility, and weather/canopy degradation. This pass does not implement final star art, astronomy simulation, or an explicit time readout.

### Environmental Sound

Environmental sound cue hooks consume sound propagation, creek distance, coast distance, ridge/valley, visibility, gorge compression, creek influence, and ocean audibility. Rain and gorge terrain can muffle or distort these cues. This pass does not implement the final audio mix.

### Traversal Affordance

Traversal-affordance cues consume slope, footing risk, hard blocker, soft blocker, affordance, and false-affordance masks. They tell the player what the terrain affords through world structure, vegetation, footing, water, deadfall, cliffs, and openings, not UI route lines.

### Player-Made Trail Marks

Player-made trail marks are planned as runtime persistence only. They can be affected by wetness, canopy, visibility, disturbance, rain, and line of sight. They must not become stable PCG generation inputs, global beacons, GPS markers, minimap icons, objective arrows, or orientation-confidence feedback.

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
- character comments or UI-like feedback telling the player whether they are oriented.

`FJungleNavigationPillarsSpec` explicitly rejects those categories through guard functions and cue-contract flags.

## PR Boundary

This pass stays isolated from later roadmap items:

- PR #17 owns no-map/no-compass lost-state gameplay pressure and recovery rules.
- PR #18 owns day/night/weather danger prototype behavior.
- PR #19 owns Cell 0 placement inside the large world.

PR #16 only defines the cue scaffold that those future passes can consume.

## Developer-Only Debug Outputs

The scaffold defines debug output names for reviewers and future editor-only tools:

- `navigation_pillar_contracts`
- `navigation_sun_position_signal`
- `navigation_shadow_signal`
- `navigation_light_colour_signal`
- `navigation_creek_flow_signal`
- `navigation_slope_signal`
- `navigation_coast_ocean_signal`
- `navigation_ridge_spur_saddle_signal`
- `navigation_mountain_landmark_signal`
- `navigation_sky_window_signal`
- `navigation_southern_sky_signal`
- `navigation_environmental_sound_signal`
- `navigation_traversal_affordance_signal`
- `navigation_player_trail_mark_runtime_contract`
- `navigation_forbidden_ui_absence_check`

These outputs are for development validation only. They must not become player-facing navigation UI.

## Validation Notes

Validation for this pass is source/docs based because it intentionally avoids broad UE editor assets, final materials, final audio, final sky art, final weather, generated cache output, and binary churn.

Expected checks:

- inspect `Source/JungleGame/JungleNavigationPillarsSpec.h` and this document;
- confirm cue contracts are diegetic and environmental, not UI-driven;
- confirm sun, shadow, light colour, sky-window, and southern-sky cues can degrade under canopy, cloud, gorge, rain, or night conditions;
- confirm creek, coast, ridge, slope, sky-window, landmark, sound, traversal-affordance, and trail-mark contracts consume existing terrain/ecosystem masks;
- confirm player-made trail marks are runtime persistence only and are not accepted as PCG generation inputs;
- confirm developer-only debug outputs do not expose map, compass, bearing, explicit time, objective marker, or orientation-confidence feedback to the player;
- confirm no generated/cache/editor junk is staged;
- confirm work remains isolated from PR #17 lost-state gameplay, PR #18 day/night/weather danger, and PR #19 Cell 0 integration.

UE commandlet/build validation remains dependent on `UnrealEditor` being available on PATH in the local shell.

## Acceptance Criteria

- Navigation pillar source scaffold exists.
- Sun-position, shadow, light-colour, creek-flow, slope, coast/ocean, ridge/spur/saddle, mountain landmark, sky-window, southern-sky, environmental sound, traversal-affordance, and player-made trail-mark cue contracts are reviewable.
- Cue degradation from canopy, cloud, rain, gorge terrain, blocked line of sight, no sky window, and night state is represented.
- Cue contracts consume existing terrain/ecosystem masks and ecosystem-pass helpers from PR #12 through PR #15.
- Runtime player-made trail marks are planned without turning into PCG generation inputs or UI rescue.
- The hard no-map/no-compass/no-clock/no-objective-marker/no-orientation-confidence rule remains intact.

## Deferred To Later Roadmap PRs

- Lost-state gameplay loop and recovery pressure.
- Day/night cycle prototype.
- Weather danger prototype.
- Final sky, moon, star, lighting, and cloud art.
- Final audio mix and sound propagation implementation.
- Final trail-mark interaction mechanics.
- Cell 0 integration inside the large world.
- Runtime UI polish unrelated to navigation rescue.
