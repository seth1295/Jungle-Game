# Ridge/Slope/Mountain Ecosystem Pass

## Roadmap Item

Roadmap PR #14 — `Ridge/slope/mountain ecosystem pass`

## Goal

Implement upland, ridge, mountain shoulder, and steep-slope ecosystem content as a reviewable source/docs scaffold for future PCG graph assets.

This pass protects ridge and mountain readability as no-map navigation evidence. It does not add a climbing system, final mountain art, full weather simulation, unrelated enemy/gameplay systems, generated editor assets, or player-facing navigation/time/orientation UI.

## Dependency

This pass depends on:

- Roadmap PR #12 — `PCG biome framework` — merged to `main`.
- Roadmap PR #13 — `Creek/riparian/wet valley ecosystem pass` — merged before this sequential pass begins.

It consumes:

- `FJunglePCGBiomeSpec` for biome layer, placeholder rule, terrain friction, density budget, deterministic sample, sky-window, and landmark-silhouette vocabulary.
- `FJungleEcosystemMaskSpec` for normalized masks and traversal classification.
- `FJungleMacroTerrainSpec` for routine walkable, soft-blocker, hard-blocker, and ridge-readability thresholds.

## Source Scaffold

`Source/JungleGame/JungleRidgeSlopeMountainEcosystemSpec.h` adds:

- `EJungleRidgeSlopeMountainZone` for ridge crests, spurs, saddles, slope breaks, mountain shoulders, exposed slopes, sheltered slopes, steep-slope risk, hard-slope exclusions, and false skyline affordances.
- `EJungleRidgeSlopeEvidenceCue` for environmental cues such as sky windows, skyline silhouettes, wind/open-canopy cues, exposed/sheltered vegetation, readable slope breaks, spur movement, saddle dips, loose footing, fatigue pressure, and false open ridge lines.
- `FJungleRidgeSlopeRuleContract` for future PCG rules with required masks, density/spacing budgets, sky-window preservation, silhouette anchors, future HLOD review flags, and developer-only status.
- `FJungleRidgeSlopeMountainEcosystemSpec` for ridge/slope thresholds, aspect variation, sky-window preservation, mountain shoulder silhouette classification, steep-slope fatigue classification, terrain-friction resolution, placeholder contracts, and validation output names.

## Zone Intent

### Ridge Crest Sky Window

Ridge crests should preserve canopy breaks and sky windows that let the player read sun, sky, mountain silhouette, weather, and terrain shape. These are environmental cues, not UI aids.

### Spur Movement Affordance

Spurs can become discovered movement affordances when ridge/valley, slope, visibility, and affordance masks support travel. They must not become explicit authored route lines.

### Saddle Transition Affordance

Saddles are transition affordances where terrain naturally suggests crossing or route reconsideration. They should remain subtle first-person landform cues.

### Slope Break Route Decision

Slope breaks should help the player decide whether to climb, contour, descend, or reconsider without requiring a compass, map, bearing, GPS marker, or objective marker.

### Mountain Shoulder Silhouette

Mountain shoulders and skyline anchors can provide long-range orientation through silhouette and sky-window relationships. They must not become artificial signage, icons, painted beacons, or globally magical markers.

### Exposed Open-Canopy Slope

Exposed aspect variation can create open canopy, wind, light, and visibility cues. These cues can support environmental literacy while remaining imperfect.

### Sheltered Dense Slope

Sheltered aspects can support denser canopy and reduced sky visibility. These zones should contrast with exposed slopes without turning every ridge into uniform dense foliage.

### Steep Slope Fatigue Risk

Steep slopes should create fatigue, time loss, noise, footing risk, injury risk, and route reconsideration. They should not become a constant physics-sliding simulator or unrecoverable player trap.

### False Skyline Affordance

An apparently open ridge or skyline line can become a false affordance when terrain, footing, or vegetation pressure makes it misleading. False affordances must be mask-derived and reviewable, not arbitrary punishment.

## Navigation Rules Preserved

This pass must not add any player-facing navigation/time/orientation UI.

Forbidden across this pass:

- player map;
- minimap;
- compass;
- bearing number/readout;
- GPS marker;
- objective marker navigation;
- HUD clock;
- explicit time readout;
- orientation-confidence UI/system;
- hidden orientation meter;
- character comments or UI-like feedback telling the player whether they are oriented.

Ridges, spurs, saddles, slope breaks, open canopy, wind, sky windows, mountain silhouettes, and environmental contrast can provide evidence. The player interprets that evidence without explicit UI confirmation.

## HLOD And Streaming Notes

This pass only creates source/docs contracts, but future PCG/content work must preserve silhouette anchors and sky-window readability through HLOD and World Partition streaming review.

The contract flags sky-window and silhouette placeholder rules as needing future HLOD review because erasing those shapes would break no-map navigation readability.

## Validation Notes

Validation for this pass is source/docs based because it intentionally avoids editor-generated PCG graph assets and final content.

Expected checks:

- inspect `git show --stat --oneline HEAD` or equivalent diff summary;
- inspect the full intended source/docs content;
- confirm the source header includes the PCG biome, ecosystem mask, and macro-terrain contracts;
- confirm ridge/slope zones are classified from height, slope, aspect, ridge/valley, sky-window, visibility, canopy, sound propagation, footing-risk, blocker, affordance, and false-affordance masks;
- confirm sky-window and silhouette rules preserve environmental readability without artificial signage;
- confirm steep slopes create friction/fatigue/risk contracts rather than climbing mechanics or constant sliding;
- confirm no generated/cache/editor junk is staged;
- confirm no player-facing map, compass, minimap, bearing, GPS, objective marker, HUD clock, explicit time, orientation-confidence UI, hidden orientation meter, or character orientation feedback is added.

UE commandlet/build validation remains dependent on `UnrealEditor` being available on PATH in the local shell.

## Acceptance Criteria

- Ridge/slope/mountain source scaffold exists.
- Ridge crests, spurs, saddles, slope breaks, mountain shoulders, exposed slopes, sheltered slopes, steep-slope risk, hard-slope exclusions, and false skyline affordance contracts are reviewable.
- Placeholder PCG rule contracts expose density, spacing, sky-window, silhouette, and HLOD-review metadata.
- Sky windows and mountain silhouettes remain environmental navigation cues, not signage.
- Steep-slope logic supports fatigue, time loss, noise, injury risk, and route reconsideration without implementing climbing or full weather systems.
- Work remains isolated from lowland/alluvial/gorge scope.

## Deferred To Later Roadmap PRs

- Actual PCG graph assets.
- Final mountain art.
- HLOD generation and screenshot validation.
- Climbing or mantle systems, if ever approved.
- Full weather simulation.
- Full navigation pillar implementation.
- Runtime fatigue/injury gameplay effects.
