# PCG Biome Framework

## Roadmap Item

Roadmap PR #12 — `PCG biome framework`

## Goal

Create a reviewable PCG biome framework scaffold that consumes the terrain, macro-terrain, ecosystem-mask, and ecosystem-debug vocabulary already added by the large-world batch.

This PR intentionally does not add final foliage, final art assets, large generated PCG graph assets, or editor cache output. It defines the source-level contracts that later PCG graph assets and ecosystem passes must follow.

## Consumed Contracts

The framework aligns with these existing source contracts:

- `FJungleWorldPartitionSpec` for the 8129 x 8129 source grid, 2 m spacing, and 16.256 km island target.
- `FJungleTerrainSourceSpec` for deterministic height, slope, aspect, flow, ridge/valley, and coast-distance source layers.
- `FJungleMacroTerrainSpec` for slope thresholds, ridge readability, hard blockers, soft blockers, and affordance vocabulary.
- `FJungleEcosystemMaskSpec` for normalized mask names and traversal classification.
- `FJungleEcosystemDebugSpec` for developer-only validation outputs that must not become player navigation UI.

## Source Scaffold

`Source/JungleGame/JunglePCGBiomeSpec.h` adds:

- `EJunglePCGBiomeLayer` for island edge, creek/riparian/wet valley, ridge/slope/mountain, lowland/alluvial/gorge, and transition graph lanes.
- `EJunglePCGSpawnRuleKind` for placeholder output rule types.
- `EJunglePCGTerrainFrictionClass` for terrain-friction categories consumed by future traversal and ecosystem passes.
- `FJunglePCGBiomeInputSample` as a deterministic terrain and mask input interface.
- `FJunglePCGSpawnRuleContract` as a placeholder spawn-rule contract with density and spacing budgets.
- `FJunglePCGBiomeFrameworkSpec` for foundation matching, biome-layer classification, terrain-friction resolution, required PCG input names, placeholder output names, and required debug output names.

## Biome Layer Intent

### Island Edge

Island edge rules consume coast distance and false-affordance masks. The ocean remains the natural world boundary. PCG should support beach, mangrove edge, cliff coast, rocky headland, creek mouth, exposed headland, and difficult coastal terrain without making the coastline a guaranteed safe loop road.

### Creek / Riparian / Wet Valley

Creek and wet-valley rules consume creek distance, wetness, soft-blocker, visibility, footing-risk, and sound-propagation masks. Later passes can place creek-edge vegetation, wet rocks, roots, deadfall, crossing candidates, and sound anchors in response to terrain conditions.

### Ridge / Slope / Mountain

Ridge and slope rules consume ridge/valley, slope, sky-window, visibility, and affordance masks. Later passes should preserve silhouettes, sky windows, slope-break readability, spurs, saddles, and ridge-following affordances without drawing primary trail lines first.

### Lowland / Alluvial / Gorge

Lowland and gorge rules consume wetness, slope, ridge/valley, visibility, soft-blocker, false-affordance, and footing-risk masks. Later passes can add denser debris, flood-prone vegetation, constrained gorge movement, dry creek-bed false affordances, and alluvial transition cues.

### Transition

Transition rules absorb areas that do not strongly classify into the other lanes. They are useful for blending, debugging, and future semi-random world-location spawns that should not assume all structure radiates from Cell 0 or a camp.

## Terrain-Friction Rules

The framework resolves PCG terrain friction from the existing traversal classifier:

1. hard blockers become `HardExcluded` PCG zones;
2. false affordances become `FalseAffordance` PCG zones;
3. soft blockers become readable or heavy plant friction based on soft-blocker strength;
4. open and affordance zones stay `Open` for framework purposes.

Wait-a-while and equivalent rainforest friction must read as plant friction and landform pressure. It must not become arbitrary collision noise.

## Density Budget Contract

Placeholder spawn rules must expose:

- biome layer;
- rule kind;
- terrain-friction class;
- required mask names;
- maximum instances per hectare;
- minimum spacing in meters;
- deterministic salt or seed;
- whether the rule preserves sky windows;
- whether it is developer-debug output.

This gives later PCG graph assets a stable budget vocabulary before final foliage art and performance tuning.

## Determinism And Region Scope

The framework uses a default deterministic seed and a 512 m region scope target. Later graph assets should regenerate predictably for a region, avoid whole-world regeneration for small changes, and stay compatible with World Partition loading.

Runtime player-made trail marks are explicitly not static PCG generation inputs. PCG may provide terrain-derived trail persistence potential later, but actual marks must be handled by runtime gameplay persistence.

## Debug Outputs

Required debug output names extend the ecosystem debug vocabulary with:

- `pcg_biome_layer`;
- `pcg_spawn_rule_budget`;
- `pcg_terrain_friction`;
- `pcg_region_seed`;
- `pcg_streaming_region_scope`.

These are developer-only outputs. They must not become a player-facing map, minimap, compass, bearing readout, GPS marker, objective marker, HUD clock, explicit time readout, orientation-confidence UI, hidden orientation meter, or character comment telling the player whether they are oriented.

## Validation Notes

Validation for this scaffold is source/docs based because no editor-generated PCG graph assets are committed in this PR.

Expected checks:

- inspect `git diff --stat`;
- inspect the full diff;
- run `git diff --check`;
- confirm the source header references the existing terrain, macro-terrain, mask, and debug specs;
- confirm no generated/cache/editor junk is staged;
- confirm no player-facing navigation or explicit-time UI is added.

UE commandlet/build validation remains dependent on `UnrealEditor` being available on PATH in the local shell.

## Acceptance Criteria

- PCG biome layer vocabulary exists and aligns with the roadmap.
- Placeholder spawn-rule contracts exist without binding final art assets.
- Deterministic terrain/mask input interface exists.
- Terrain-friction logic consumes existing traversal masks.
- Density budget fields exist for later PCG graph assets.
- Developer debug outputs are named and explicitly non-player-facing.
- Creek, ridge, slope, lowland, gorge, and island-edge passes can be added without rebuilding the framework vocabulary.

## Deferred To Later Roadmap PRs

- Actual PCG graph assets.
- Final foliage art and asset selection.
- Creek/riparian/wet valley ecosystem content.
- Ridge/slope/mountain ecosystem content.
- Lowland/alluvial/gorge ecosystem content.
- Full performance pass, HLOD generation, and dense-area profiling.
- Runtime player-made trail mark gameplay persistence.
