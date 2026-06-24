# Lowland/Alluvial/Gorge Ecosystem Pass

## Roadmap Item

Roadmap PR #15 — `Lowland/alluvial/gorge ecosystem pass`

## Goal

Implement lowland, alluvial, gorge, and transition ecosystem content as a reviewable source/docs scaffold for future PCG graph assets.

This pass gives basins, floodplains, creek mouths, mangrove/coastal transitions, cliff-coast transitions, and compressed gorge corridors distinct navigation meaning. It does not add final cinematics, full hydrology simulation, full wildlife ecology, unrelated Cell 0 polish, generated editor assets, or player-facing navigation/time/orientation UI.

## Dependency

This pass depends on:

- Roadmap PR #12 — `PCG biome framework` — merged to `main`.
- Roadmap PR #13 — `Creek/riparian/wet valley ecosystem pass` — merged before this sequential pass begins.
- Roadmap PR #14 — `Ridge/slope/mountain ecosystem pass` — merged before this sequential pass begins.

It consumes:

- `FJunglePCGBiomeSpec` for lowland/alluvial/gorge biome layer, placeholder rule, terrain friction, density budget, and deterministic sample vocabulary.
- `FJungleEcosystemMaskSpec` for normalized masks, blocker/affordance classification, and false-affordance thresholds.
- `FJungleMacroTerrainSpec` for slope, hard-blocker, soft-blocker, basin, creek-mouth, cliff-coast, gully, and affordance vocabulary.

## Source Scaffold

`Source/JungleGame/JungleLowlandAlluvialGorgeEcosystemSpec.h` adds:

- `EJungleLowlandAlluvialGorgeZone` for wet lowlands, alluvial benches, floodplain soft blockers, gorge corridors, gorge sightline compression, gorge sound compression, creek-mouth/mangrove transitions, cliff-coast transitions, island-edge density transitions, regrowth gaps, hard gorge exclusions, and false lowland affordances.
- `EJungleLowlandGorgeEvidenceCue` for environmental cues such as flat wet ground, alluvial sediment shelves, flood debris lines, mangrove root maze placeholders, creek-mouth transitions, cliff-coast dead ends, compressed gorge sound, compressed gorge sightlines, dry creek-bed false affordances, old scar/regrowth gaps, misleading clearings, and beach routes that end in blockers.
- `FJungleLowlandGorgeRuleContract` for future PCG rules with required masks, density/spacing budgets, sightline/sound compression flags, false-affordance flags, dense-performance-risk notes, and developer-only status.
- `FJungleLowlandAlluvialGorgeEcosystemSpec` for lowland flatness, alluvial wetness, gorge compression, coastal transition, creek-mouth/mangrove classification, dense-risk classification, terrain-friction resolution, placeholder rule contracts, and validation output names.

## Zone Intent

### Lowland Wet Flat

Lowlands should read as flatter and wetter through slope, wetness, footing-risk, and vegetation structure. They must not rely on arbitrary swamp vibes or UI labels.

### Alluvial Bench

Alluvial benches represent floodplain or basin structure where wetness and affordance masks make movement plausible but imperfect.

### Floodplain Soft Blocker

Floodplain soft blockers use wetness, canopy, footing risk, debris, and plant friction to slow or redirect movement. Dense lowland rules must carry performance-risk notes before final PCG density increases.

### Gorge Constrained Corridor

Gorges should compress movement through terrain shape, slope, visibility, and hard-blocker masks. Corridors must remain navigable and must not become unrecoverable traps.

### Gorge Sightline And Sound Compression

Gorges can compress sightlines and sound propagation. This pass defines placeholder contracts only; it does not implement final audio mix or final gorge geometry.

### Creek-Mouth Mangrove Transition

Creek-mouth and mangrove transitions use coast distance, creek distance, wetness, and canopy masks to create heavy plant-friction placeholders and coastal density changes.

### Cliff-Coast Transition

Cliff-coast transitions mark places where beach or coastal routes can end in terrain pressure. They should be readable from landform and ecology, not invisible walls.

### Island-Edge Density Transition

Island-edge density transitions preserve the ocean as the natural world boundary while allowing coast, mangrove, cliff, rocky, creek-mouth, and headland conditions to differ.

### Regrowth Gap Affordance

Old scars, regrowth gaps, and disturbed openings can become imperfect movement affordances when disturbance and affordance masks support them.

### False Lowland Affordance

Dry creek beds, gullies, misleading clearings, old scars, regrowth gaps, beach routes that end in cliffs, and routes that fade into mangroves can become false affordances when masks justify them. False affordances must be reviewable and environmental, not arbitrary punishment.

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

Lowland, alluvial, gorge, and coastal transition cues must remain terrain/ecology driven. They must not become authored path-line navigation.

## Performance Notes

Dense lowland, mangrove, and floodplain placeholders carry explicit performance-risk metadata. This pass does not solve final dense-vegetation performance, but it records where later PCG graph assets need budget review.

## Validation Notes

Validation for this pass is source/docs based because it intentionally avoids editor-generated PCG graph assets and final content.

Expected checks:

- inspect `git show --stat --oneline HEAD` or equivalent diff summary;
- inspect the full intended source/docs content;
- confirm the source header includes the PCG biome, ecosystem mask, and macro-terrain contracts;
- confirm lowland/alluvial/gorge zones are classified from height, slope, creek distance, coast distance, wetness, ridge/valley, canopy, disturbance, sound propagation, visibility, footing risk, blocker, affordance, and false-affordance masks;
- confirm gorge corridors preserve navigability and do not create unrecoverable player traps;
- confirm dense lowland performance risk is noted rather than hidden;
- confirm false affordances are documented and mask-derived;
- confirm no generated/cache/editor junk is staged;
- confirm no player-facing map, compass, minimap, bearing, GPS, objective marker, HUD clock, explicit time, orientation-confidence UI, hidden orientation meter, or character orientation feedback is added.

UE commandlet/build validation remains dependent on `UnrealEditor` being available on PATH in the local shell.

## Acceptance Criteria

- Lowland/alluvial/gorge source scaffold exists.
- Wet flats, alluvial benches, floodplain soft blockers, gorge corridors, sightline/sound compression, creek-mouth/mangrove transitions, cliff-coast transitions, island-edge density transitions, regrowth gaps, hard gorge exclusions, and false lowland affordances are reviewable.
- Placeholder PCG rule contracts expose density, spacing, compression, false-affordance, and dense-performance-risk metadata.
- Lowland, alluvial, and gorge areas have distinct navigation meaning.
- Gorge corridors are documented as navigable and not unrecoverable traps.
- Dense lowland performance risk is explicitly noted for later PCG graph/content passes.
- Work remains isolated from Cell 0 polish, final hydrology, final cinematics, and full wildlife ecology.

## Deferred To Later Roadmap PRs

- Actual PCG graph assets.
- Final mangrove, lowland, alluvial, and gorge art.
- Final hydrology simulation.
- Full wildlife ecology.
- Final gorge geometry and audio implementation.
- Dense lowland profiling and performance pass.
- Runtime survival effects for mud, flooding, disease, or wildlife.
- Cell 0 integration or polish.
