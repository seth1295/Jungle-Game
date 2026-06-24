# Creek/Riparian/Wet Valley Ecosystem Pass

## Roadmap Item

Roadmap PR #13 — `Creek/riparian/wet valley ecosystem pass`

## Goal

Implement the first creek, riparian, and wet-valley ecosystem content scaffold as a source/docs contract that future PCG graph assets can consume.

This pass keeps creek navigation terrain-driven. It does not add final water simulation, final audio mix, full wildlife AI, player-facing navigation UI, or generated editor assets.

## Dependency

This pass depends on Roadmap PR #12 — `PCG biome framework` — being merged to `main`.

It consumes:

- `FJunglePCGBiomeSpec` for biome layer, placeholder rule, terrain friction, density budget, and deterministic mask-sample vocabulary.
- `FJungleEcosystemMaskSpec` for normalized mask names and traversal classification.
- `FJungleMacroTerrainSpec` for slope and landform traversal thresholds.
- `FJungleEcosystemDebugSpec` indirectly through the PCG biome framework debug-output contract.

## Source Scaffold

`Source/JungleGame/JungleCreekRiparianWetValleyEcosystemSpec.h` adds:

- `EJungleCreekRiparianWetValleyZone` for creek channel, bank, crossing, riparian bench, wet valley, soft-blocker, sound-anchor, and false-affordance zones.
- `EJungleCreekEvidenceCue` for reviewable placeholder cues such as bank vegetation changes, wet rocks, mud shelves, deadfall, shallow shelves, leech/tick notes, frog/insect notes, muffled rain-water notes, and false bank openings.
- `FJungleCreekRiparianRuleContract` for future PCG rules with required masks, density budgets, spacing budgets, crossing-candidate flags, false-affordance flags, and developer-only status.
- `FJungleCreekRiparianWetValleyEcosystemSpec` for creek influence thresholds, crossing-candidate classification, creek-zone classification, friction resolution, required masks, placeholder contracts, and validation output names.

## Zone Intent

### Creek Channel Hard Water

The channel itself can become a hard blocker where the hard-blocker mask is active near the channel edge. This pass does not implement final water simulation.

### Creek Bank Affordance

Creek banks can become imperfect movement affordances where creek distance, visibility, and affordance masks support readable travel. They must not become guaranteed authored route lines.

### Creek Crossing Candidate

Crossings are only candidates when creek distance, slope, footing risk, affordance, and hard-blocker masks make the location plausible. The contract marks reviewable crossing candidates without implementing final water interaction.

### Riparian Bench And Wet Valley Floor

Riparian benches and wet valley floors use wetness, creek distance, slope, and visibility to create terrain evidence. The player should infer wet-zone logic from structure, not from UI labels.

### Muddy Deadfall And Wait-A-While Soft Blockers

Mud, roots, deadfall, and wait-a-while remain soft blockers or readable plant friction. They should slow, redirect, or increase risk rather than create arbitrary collision noise.

### Water Sound Anchor

Water-sound anchors are placeholder contracts for future audio placement. Frogs, insects, waterfalls, and rain-muffled water are evidence cues, not final audio implementation in this pass.

### False Creek Affordance

A bank opening, dry-looking shelf, or vegetation gap can look promising but become a false affordance when masks show soft-blocker or false-affordance conditions. False affordances must be documented and reviewable, not arbitrary punishment.

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

Creeks can provide environmental evidence through terrain, vegetation, sound placeholders, wetness, footing risk, channel shape, valley width, and crossing readability. They must not become explicit route overlays.

## Validation Notes

Validation for this pass is source/docs based because it intentionally avoids editor-generated PCG graph assets and final content.

Expected checks:

- inspect `git diff --stat`;
- inspect the full diff;
- run `git diff --check`;
- confirm the source header includes the PCG biome, ecosystem mask, and macro-terrain contracts;
- confirm creek bands are classified from creek distance, wetness, slope, canopy, footing risk, sound propagation, visibility, blocker, affordance, and false-affordance masks;
- confirm crossing candidates are mask-derived and not authored route markers;
- confirm no generated/cache/editor junk is staged;
- confirm no player-facing map, compass, minimap, bearing, GPS, objective marker, HUD clock, explicit time, orientation-confidence UI, hidden orientation meter, or character orientation feedback is added.

UE commandlet/build validation remains dependent on `UnrealEditor` being available on PATH in the local shell.

## Acceptance Criteria

- Creek/riparian/wet-valley source scaffold exists.
- Creek channel, bank, crossing, wet valley, soft-blocker, sound-anchor, and false-affordance contracts are reviewable.
- Placeholder PCG rule contracts expose density and spacing budgets.
- Creek crossing candidates are derived from terrain and masks.
- Creek sound anchors are placeholder contracts only.
- Leech/tick risk remains a placeholder note and does not become full wildlife AI.
- The player can later infer creek route logic from environmental evidence without explicit UI guidance.

## Deferred To Later Roadmap PRs

- Final water simulation.
- Final audio mix.
- Actual PCG graph assets.
- Final foliage/art asset selection.
- Full wildlife AI.
- Runtime survival effects for leeches, ticks, infection, or disease.
- Full performance pass for dense riparian content.
