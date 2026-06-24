# Day/Night/Weather Danger Prototype

## Roadmap Item

Roadmap PR #18 — `Day/night/weather danger prototype`

## Goal

Prototype time and weather as survival-navigation pressure without adding final weather simulation, final audio mix, final sky/star art, explicit clock UI, map, compass, bearing, GPS, objective markers, orientation-confidence UI, combat, chase, or full damage/death systems.

This pass goes hard on gameplay structure: night removes or weakens sun-based direction, weather attacks cue reliability, terrain amplifies risk, trail marks degrade, creek/weather conditions change traversal meaning, and known-safety anchors matter only when physically plausible.

## Research Notes

This prototype is aligned with Unreal Engine atmospheric/audio systems but does not create final assets or editor-generated content in this PR.

Primary engine references used:

- Epic Sky Atmosphere documentation: Sky Atmosphere is a physically based atmosphere-rendering system with time-of-day features; it supports atmospheric directional lights, and Epic documents using Atmosphere Sun Light Index `0` for sun and `1` for moon.
  - https://dev.epicgames.com/documentation/en-us/unreal-engine/sky-atmosphere-component-in-unreal-engine
- Epic Volumetric Cloud documentation: Volumetric Clouds support dynamic time-of-day setups with Sky Atmosphere and Sky Light real-time capture; Epic recommends a component set of Directional Light, Sky Atmosphere, Sky Light, and Volumetric Cloud for initial setup.
  - https://dev.epicgames.com/documentation/en-us/unreal-engine/volumetric-cloud-component-in-unreal-engine
- Epic Environment Light Mixer documentation: Environment Light Mixer creates/edits atmospheric lighting components for sky, clouds, atmosphere lights, and sky lighting in one editor window.
  - https://dev.epicgames.com/documentation/en-us/unreal-engine/environment-light-mixer-in-unreal-engine
- Epic Sound Attenuation documentation: Unreal's built-in sound attenuation can enable occlusion checks and apply configured occlusion settings.
  - https://dev.epicgames.com/documentation/en-us/unreal-engine/sound-attenuation-in-unreal-engine

## Dependency

This pass depends on:

- Roadmap PR #16 — `Navigation pillars: sun, sky, sound, coast, creeks, ridges, landmarks`.
- Roadmap PR #17 — `No-map/no-compass lost-state gameplay loop`.

It consumes:

- `FJungleNavigationPillarsSpec` for diegetic cue pillars, cue degradation, environmental state, and forbidden UI guards.
- `FJungleLostStateGameplaySpec` for lost-state pressure, recovery evidence, trail-mark interactions, and known-safety pressure contracts.
- `FJungleEcosystemMaskSpec` and `FJunglePCGBiomeInputSample` for normalized terrain/ecosystem masks.
- `FJungleMacroTerrainSpec` for slope and hard-blocker thresholds.

## Source Scaffold

`Source/JungleGame/JungleDayNightWeatherDangerSpec.h` adds:

- `EJungleDayNightPhase` for pre-dawn, low-sun morning/afternoon, midday, dusk, moon/star night, and overcast night phases.
- `EJungleWeatherDangerState` for clear readability, humid haze, light rain, heavy rain, fog/low cloud, and storm-cell prototypes.
- `EJungleWeatherNavigationAttack` for concrete attacks against navigation: sun cue removed, shadow flattened, light colour ambiguous, sky blocked, landmark visibility reduced, sound muffled, creek swollen, footing worsened, trail marks degraded, known safety occluded, and night danger increased.
- `EJungleAtmosphereImplementationHook` for future engine-facing hooks: Sky Atmosphere, sun/moon Directional Lights, Sky Light real-time capture, Volumetric Cloud, Exponential Height Fog, sound attenuation occlusion, and Environment Light Mixer validation.
- `EJungleWeatherDangerPhase` for internal/debug danger classification.
- `FJungleDayNightSkySample` for sky phase, sun/moon/star signals, shadow length, light warmth, and southern-sky visibility.
- `FJungleWeatherPrototypeSample` for cloud opacity, rain intensity, fog density, wind exposure, sound muffling, creek swelling, trail washout, lightning/storm gust prototype flags.
- `FJungleDayNightWeatherDangerInput` for full weather danger scoring input and trail mark presence.
- `FJungleDayNightWeatherDangerOutput` for developer-only danger scores.
- `FJungleDayNightWeatherRuleContract` for reviewable pressure/recovery rules with explicit forbidden scope guards.
- `FJungleDayNightWeatherDangerSpec` for cue loss, weather pressure, night pressure, terrain amplification, trail-mark loss, known-safety relief, final danger output, engine hook names, rule contracts, and validation outputs.

## Gameplay Intent

### Daylight Is Useful But Fragile

Readable daylight supports sun position, shadow direction/length, light colour, sky-window, ridge, mountain, creek, slope, and coast/ocean interpretation. Weather and canopy can degrade it instead of replacing it with UI.

Low sun should create strong shadows and warm light cues. Midday can make shadows shorter and direction harder to read. Dusk becomes dangerous because sun-based certainty collapses before full night recovery cues are established.

### Night Changes The Rules

Night pressure is not instant failure. It removes sun navigation, weakens landmarks, makes sky windows more important, increases dependence on moon/stars/southern sky where visible, and elevates sound, creek, coast/ocean, slope, and known-safety evidence.

Overcast night is a high-pressure state because the sky route is blocked and sun cues are gone.

### Weather Attacks Navigation

Weather should attack player interpretation directly:

- cloud cover removes sun and shadow certainty;
- fog/low cloud reduces mountain/ridge/known-safety visibility;
- rain muffles creek/ocean/wildlife/distant sound;
- heavy rain worsens footing and trail mark reliability;
- storm cells raise exposure pressure without adding combat/chase horror;
- creek swelling changes crossings, banks, and lowland/gorge affordances;
- false affordances become more dangerous when visibility and footing degrade.

### Terrain Amplifies Weather

The same weather is not equally dangerous everywhere:

- gorges compress sightlines and sound;
- ridges expose the player to wind/lightning/storm risk later;
- steep slopes and wet roots increase footing pressure;
- creek edges and crossings become harder under creek swell;
- lowlands and mangroves become heavier soft blockers;
- dense canopy removes sky navigation but may reduce direct rain exposure.

### Known Safety Is Physical, Not Magic

Known safety can relieve danger only when it is physically plausible:

- inside an actual known shelter;
- near a known safety anchor;
- line of sight to shelter/fire/smoke where such a source exists;
- audible environmental evidence that still survives weather/terrain occlusion.

Known safety must never become a GPS point, objective marker, minimap icon, compass bearing, global beacon, or character comment telling the player they are oriented.

### Trail Marks Are Runtime And Fragile

Player-made trail marks remain runtime interactions. Weather can degrade them through rain washout, visibility loss, wetness, canopy cover, disturbance, and line-of-sight constraints. They remain useful only if the world still supports seeing/interpreting them.

They must not become stable PCG generation inputs. No trail mark means no trail-mark weather loss.

## Engine-Aligned Prototype Plan

Future editor implementation should use the following as the likely minimal engine-facing lane:

- `SkyAtmosphere` for time-of-day sky colour and sun/moon atmospheric context.
- Directional Light with Atmosphere Sun Light Index `0` as the sun.
- Directional Light with Atmosphere Sun Light Index `1` as the moon.
- `SkyLight` with real-time capture only where performance permits and validation says it is needed.
- `VolumetricCloud` prototype material/settings for cloud cover, overcast, storm-cell, and sky-window degradation tests.
- Exponential Height Fog only as a prototype visibility modifier if terrain/cloud/fog validation needs it.
- Sound Attenuation occlusion hooks for rain muffling, gorge compression, and known-safety/water/ocean sound degradation.
- Environment Light Mixer as an editor validation path, not runtime gameplay.

This PR does not add those assets. It defines the contracts that make those assets testable later.

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
- final weather simulation;
- final audio mix;
- final sky/star art;
- full damage/death system;
- horror chase system;
- combat.

`FJungleDayNightWeatherDangerSpec` rejects these categories through guard functions and rule-contract flags.

## PR Boundary

This pass stays isolated from later roadmap items:

- PR #19 owns Cell 0 integration inside the large world.
- PR #20 owns packaged large-world first-playable smoke/performance checks and release notes.

PR #18 defines strong prototype contracts only. It does not add final lighting, final clouds, final rain, final audio, final damage, or final authored Cell 0 placement.

## Developer-Only Debug Outputs

The scaffold defines debug output names for reviewer validation and future editor-only tools:

- `day_night_weather_rule_contracts`
- `daylight_navigation_signal`
- `night_sky_navigation_signal`
- `sky_occlusion_pressure`
- `weather_pressure`
- `night_pressure`
- `terrain_amplified_weather_risk`
- `trail_mark_weather_loss`
- `known_safety_weather_relief`
- `final_weather_danger_score`
- `weather_forbidden_ui_absence_check`

These outputs must stay developer-only. They must not become a clock, weather radar, objective hint, map, compass, bearing, lost meter, or orientation-confidence UI.

## Validation Notes

Validation for this pass is source/docs based because it intentionally avoids broad UE editor assets, final materials, final audio, final sky art, final weather simulation, generated cache output, and binary churn.

Expected checks:

- inspect `Source/JungleGame/JungleDayNightWeatherDangerSpec.h` and this document;
- confirm the scaffold consumes PR #16 navigation pillars and PR #17 lost-state contracts;
- confirm day/night phases alter navigation evidence without exposing explicit time;
- confirm weather attacks navigation through cloud, rain, fog, sound muffling, creek swell, footing, trail-mark degradation, and known-safety occlusion;
- confirm terrain amplifies risk through slope, blocker, footing, creek, gorge, canopy, and false-affordance masks;
- confirm engine hook names align with Sky Atmosphere, sun/moon Directional Lights, Sky Light, Volumetric Cloud, fog, Environment Light Mixer, and sound attenuation occlusion lanes;
- confirm developer-only debug outputs do not become player-facing UI;
- confirm no generated/cache/editor junk is staged;
- confirm work remains isolated from PR #19 Cell 0 integration and PR #20 packaged smoke/performance checks.

UE commandlet/build validation remains dependent on `UnrealEditor` being available on PATH in the local shell.

## Acceptance Criteria

- Day/night/weather danger source scaffold exists.
- Day/night phases, weather states, navigation attacks, engine hooks, input/output samples, rule contracts, scoring helpers, and validation outputs are reviewable.
- Night can remove sun navigation, reduce landmark visibility, and preserve imperfect moon/star/southern-sky recovery where sky windows/weather permit.
- Weather can degrade sun/shadow/light colour, sky windows, landmarks, sound, creek crossings, footing, trail marks, known safety, and night danger.
- Known safety is physical and local, never a global UI rescue.
- The hard no-map/no-compass/no-clock/no-objective-marker/no-orientation-confidence rule remains intact.

## Deferred To Later Roadmap PRs

- Actual editor actor placement for Sky Atmosphere, Directional Lights, Sky Light, Volumetric Cloud, fog, and sound attenuation assets.
- Final weather simulation.
- Final clouds, sky, moon, star, and lighting art.
- Final rain, creek swell, and water visuals/audio.
- Final trail-mark mechanics.
- Full damage/death or survival stat stack.
- Cell 0 integration inside the large world.
- Packaged first-playable smoke/performance validation.
