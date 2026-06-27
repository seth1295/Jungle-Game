# Terrain Math Implementation PR Sequence Research

## Executive Summary

Jungle Game should treat the next terrain rewrite as a **sequenced terrain-source replacement**, not as a polish pass on the current volcano-centered preview. The existing 97.536 km preview is valuable because it proves that the coast, beach, shelf, ocean, and square-edge constraints can pass at the intended scale. It is not the mathematical base to tune. **Batch 004 is historical failure evidence. New implementation should begin from the Batch 005 / Batch 006 lifecycle inputs:** geomorphology math core, hydrology/catchment solver, volcanic subsystem, feature grammar, DEM benchmarking, offline generator/runtime bridge, and morphology validation.

The safest ambitious sequence is ten implementation PRs:

1. establish config identity, deterministic source authority, and replacement composition boundaries;
2. build the landform region graph so the island is interesting before volcanology;
3. add hydrology, catchments, drainage, erosion-inspired incision, and fan/deposition passes;
4. rebuild the volcano as a compact local subsystem embedded in the larger terrain graph;
5. add multi-scale feature grammar and detail fields that support landforms without owning them;
6. expand preview/export outputs enough that reviewers can inspect every stage;
7. promote morphology validation to hard gates;
8. connect the generated terrain source to the runtime terrain shell;
9. produce DEM-style calibration/benchmark evidence;
10. hand off the accepted terrain lane and block later non-terrain work until the terrain contract is stable.

Every PR must preserve or explicitly restore these non-negotiable gates:

```text
world_size_m: 97536.0 target
shoreline_error_max_m: 0 or effectively 0
beach_continuity_pct: 100%
ocean_below_sea_pct: 100%
square_edge_ocean_violations: 0
outer world edge remains ocean/container terrain
```

The implementation must be source-producing and evidence-producing. A PR that only adds prose, renames constants, or changes screenshots without creating generator behavior, typed data, validation, or runtime bridge behavior should be rejected.

## Terrain-Only Scope Boundary

This sequence is limited to:

- terrain source math;
- deterministic generator configuration;
- heightfield and terrain-channel generation;
- graph, hydrology, erosion-inspired, volcanic, and feature-grammar passes;
- preview/export tooling;
- morphology and coast validation;
- runtime terrain shell integration;
- generated terrain evidence and handoff.

It must not design or implement:

- foliage;
- ecology;
- weather;
- audio;
- NPCs;
- buildings;
- survival/gameplay systems;
- final materials;
- UI;
- map, compass, minimap, GPS, objective markers, bearing displays, or player-facing navigation overlays;
- water rendering polish or player-facing water gameplay.

Developer-only maps, overlays, manifests, diagnostic screenshots, and terrain validation images are allowed only as terrain-source evidence.

## Implementation Philosophy

### 1. Replace terrain authority before adding new terrain features

Hydrology, volcanology, feature grammar, DEM comparison, preview validation, and runtime bridge work all depend on a stable terrain authority. Do not put serious hydrology into the current radial volcano authority. Do not make the volcano rewrite before the island has independent landform regions. Do not build runtime bridge assumptions before the source pipeline records deterministic identity.

### 2. Coast invariants are always final constraints

The island may become more complex, but the coast gates are already proven and must remain locked. Every PR should run coast regression validation. Feature passes may create candidate elevations, but final composition must reapply or verify:

```text
shoreline lock
beach continuity
ocean-below-sea
square-edge ocean containment
outer world edge ocean/container terrain
```

### 3. Batch 004 is failure evidence, not the template

Batch 004 demonstrated that a 97.536 km world and coast gates are feasible, but it also demonstrated the structural failure modes:

- one dominant volcano controls the island;
- crater-centered coordinates organize too much terrain;
- ridges/gullies read as turbine/flower/fan-blade spokes;
- the volcano-disabled island is weak;
- landform regions, basins, valleys, plateaus, coastal flats, saddles, old lava history, and secondary highlands are missing.

The implementation should preserve Batch 004 coast metrics, not Batch 004 interior math.

### 4. Evidence is part of source

Each PR must produce:

- source code changes;
- deterministic config/version changes;
- automated tests/validation;
- generated preview evidence;
- a manifest diff showing identity and metrics;
- a rollback boundary.

### 5. Large enough to matter, small enough to review

Each PR should land one architecture layer or terrain subsystem. Avoid tiny fake slices such as “add config names only,” “add unused masks,” or “export one new image without using it.” Also avoid a mega-PR that rewrites config, hydrology, volcanology, preview, runtime bridge, and validation at once.

## Dependency Graph

```text
PR 1 Terrain Math Core And Config Identity
  ├─ prerequisite for all later PRs
  ├─ owns world size, seed, algorithm version, config fingerprint, pass order
  └─ preserves coast gates from current evidence

PR 2 Landform Region Graph
  ├─ depends on PR 1
  ├─ prerequisite for hydrology basin competition
  ├─ prerequisite for volcano embedding
  └─ prerequisite for feature grammar ownership

PR 3 Hydrology / Catchment / Erosion Passes
  ├─ depends on PR 1 and PR 2
  ├─ prerequisite for lahar corridors, lava routing, fan deposits
  └─ prerequisite for catchment/radial validation

PR 4 Volcanic Subsystem Rework
  ├─ depends on PR 1, PR 2, PR 3
  ├─ uses regional terrain and drainage instead of owning the world
  └─ supplies volcano masks to preview/validation

PR 5 Feature Grammar And Multi-Scale Detail
  ├─ depends on PR 1-4
  ├─ adds non-authority detail fields and named feature stamps
  └─ uses region/hydrology/volcano ownership maps

PR 6 Preview Exporter Expansion
  ├─ can begin earlier, but should land after PR 1-5 for full coverage
  └─ exports all stage products and manifest identity

PR 7 Morphology Validation Gates
  ├─ depends on PR 6 outputs
  ├─ promotes morphology metrics to hard gates
  └─ blocks visually plausible but structurally fake terrain

PR 8 Runtime Terrain Shell Integration
  ├─ depends on accepted generated source/manifest schema
  ├─ consumes heightfield/channels, not ad hoc actor math
  └─ logs runtime terrain authority

PR 9 DEM Calibration / Benchmark Report
  ├─ depends on PR 6/7 evidence and PR 8 runtime parity
  └─ tunes metrics against reference families without copying a real island

PR 10 Acceptance Handoff And Next Terrain Lane
  ├─ depends on all gates passing
  └─ freezes terrain contract for later traversal/environment work
```

## Proposed PR Sequence Overview

| PR | Main outcome | Must land before | Core evidence |
|---:|---|---|---|
| 1 | Deterministic terrain source identity and pass framework | everything | config fingerprint, coast regression manifest, source-vs-preview parity |
| 2 | Named landform region graph and non-volcanic island authority | hydrology, volcano, feature grammar | landform region map, volcano-disabled height, region metrics |
| 3 | Catchment, flow, erosion-inspired incision/deposition | volcano/lahar routing, validation | catchment map, flow accumulation, stream order, ridge divide map |
| 4 | Compact embedded volcanic subsystem | detail grammar, runtime masks | volcano footprint, crater/breach/collapse, lava/lahar corridors |
| 5 | Feature grammar and multi-scale detail | final previews and gates | feature atlas, scale-band atlas, warp vectors, ownership map |
| 6 | Preview/export expansion | hard validation gates | tracked review previews, local high-res artifacts, JSON manifest |
| 7 | Morphology validation gates | runtime acceptance | radial artifact, volcano dominance, catchment diversity gates |
| 8 | Runtime terrain shell bridge | DEM benchmark finalization | runtime log token, shell loads generated terrain/channels |
| 9 | DEM calibration/benchmark report | final acceptance | calibration report JSON/Markdown, slope/hypsometry/basin comparison |
| 10 | Acceptance handoff and next lane | non-terrain resumption | terrain-only handoff, known limits, freeze checklist |

## PR 1 — Terrain Math Core And Config Identity

### Goal

Create the replacement terrain authority skeleton. This PR should not solve hydrology or volcanology yet. It should create the deterministic source identity and composition boundary that every later PR uses.

### Required source changes

Likely files/classes:

```text
Source/JungleGame/Public/Terrain/JGTerrainTypes.h
Source/JungleGame/Public/Terrain/JGTerrainConfig.h
Source/JungleGame/Public/Terrain/JGTerrainBuildMetadata.h
Source/JungleGame/Public/Terrain/JGTerrainSourceGrid.h
Source/JungleGame/Public/Terrain/JGTerrainGenerator.h
Source/JungleGame/Public/Terrain/JGTerrainPassContext.h
Source/JungleGame/Public/Terrain/JGTerrainChannels.h

Source/JungleGame/Private/Terrain/JGTerrainConfig.cpp
Source/JungleGame/Private/Terrain/JGTerrainBuildMetadata.cpp
Source/JungleGame/Private/Terrain/JGTerrainSourceGrid.cpp
Source/JungleGame/Private/Terrain/JGTerrainGenerator.cpp
Source/JungleGame/Private/Terrain/JGTerrainChannels.cpp

Source/JungleGame/Private/Tests/JGTerrainConfigIdentityTests.cpp
Source/JungleGame/Private/Tests/JGTerrainCoastRegressionTests.cpp
```

Core data structures:

```cpp
struct FJungleTerrainDomainConfig;
struct FJungleTerrainSeedConfig;
struct FJungleTerrainConfig;
struct FJungleTerrainBuildMetadata;
struct FJungleTerrainSourceGrid;
struct FJungleTerrainChannelSet;
struct FJungleTerrainPassContext;
struct FJungleTerrainBuildResult;
class FJungleTerrainGenerator;
```

Config/version names:

```text
terrain_schema: jungle_terrain.v006
terrain_source_version: JG_TERRAIN_MATH_CORE_006_001
terrain_config_version: pr5.batch006.terrain_math_core.v1
algorithm_family: batch005006_lifecycle
batch004_status: historical_failure_evidence
```

### Required implementation behavior

- Define `world_size_m = 97536.0`.
- Define sea level as `0.0 m`.
- Define authority, preview, and runtime sample resolution profiles.
- Add deterministic root seed and named subsystem seed derivation.
- Add canonical config serialization and SHA-256 or project-stable hash.
- Add pass-order declaration, even if only coast/base placeholder passes are live.
- Preserve existing coast lock logic or wrap it behind a new coast authority interface.
- Emit a `FJungleTerrainBuildMetadata` record in every preview/runtime build.

### Preview evidence

Output folder:

```text
Images/TerrainPreview/Batch006/PR01_TerrainMathCore/
```

Required outputs:

```text
pr01_color_relief.png
pr01_height.png
pr01_coast_error.png
pr01_beach_continuity.png
pr01_ocean_mask.png
pr01_manifest.json
pr01_metrics.md
```

Required manifest fields:

```json
{
  "terrain_schema": "jungle_terrain.v006",
  "terrain_source_version": "JG_TERRAIN_MATH_CORE_006_001",
  "world_size_m": 97536.0,
  "sea_level_m": 0.0,
  "authority_resolution_px": 8129,
  "preview_resolution_px": 1024,
  "config_hash": "sha256:...",
  "root_seed": "...",
  "batch004_status": "historical_failure_evidence",
  "coast_gates": {
    "shoreline_error_max_m": 0.0,
    "beach_continuity_pct": 100.0,
    "ocean_below_sea_pct": 100.0,
    "square_edge_ocean_violations": 0
  }
}
```

### Validation to run

```text
Terrain.ConfigIdentity.DeterministicHash
Terrain.ConfigIdentity.FingerprintChangesOnMeaningfulChange
Terrain.Core.WorldSizeIs97536
Terrain.Core.SeaLevelIsZero
Terrain.Coast.ShorelineError
Terrain.Coast.BeachContinuity
Terrain.Coast.OceanBelowSea
Terrain.Coast.SquareEdgeOcean
Terrain.Core.NoGlobalRandomUse
```

### Acceptance metrics

```text
world_size_m == 97536.0
shoreline_error_max_m <= 0.01 m or project-defined effective zero
beach_continuity_pct == 100.0
ocean_below_sea_pct == 100.0
square_edge_ocean_violations == 0
config_hash stable across repeated runs
heightfield_hash stable across repeated runs
config_hash changes when source version/seed/pass config changes
```

### Stop conditions

Stop before PR 2 if:

- world size regresses;
- any coast hard gate fails;
- generation depends on actor construction order, wall-clock time, global RNG, or unordered iteration;
- config hashes are not stable;
- runtime shell still owns the terrain math directly with no source boundary;
- the PR is docs-only or only adds unused config names.

## PR 2 — Landform Region Graph

### Goal

Make the island structurally interesting before hydrology and before the volcano exists. This PR replaces crater-centered island authority with named regions, uplands, lowlands, saddles, basins-in-waiting, coastal flats, and macro relief.

### Required source changes

Likely files/classes:

```text
Source/JungleGame/Public/Terrain/JGTerrainLandformGraph.h
Source/JungleGame/Public/Terrain/JGTerrainRegion.h
Source/JungleGame/Public/Terrain/JGTerrainImplicitFields.h
Source/JungleGame/Public/Terrain/JGTerrainRegionMetrics.h

Source/JungleGame/Private/Terrain/JGTerrainLandformGraph.cpp
Source/JungleGame/Private/Terrain/JGTerrainRegion.cpp
Source/JungleGame/Private/Terrain/JGTerrainImplicitFields.cpp
Source/JungleGame/Private/Terrain/JGTerrainRegionMetrics.cpp

Source/JungleGame/Private/Tests/JGTerrainLandformGraphTests.cpp
```

Core structures:

```cpp
struct FLandformRegion;
struct FLandformNode;
struct FLandformEdge;
struct FRegionalUpland;
struct FCoastalLowland;
struct FIslandSdfField;
struct FRegionBlendField;
struct FLandformRegionMetrics;
```

### Required implementation behavior

- Generate an organic island envelope using implicit fields/SDFs rather than square bounds.
- Add named landform regions such as:
  - windward highland spine;
  - old eroded upland block;
  - coastal lowland belt;
  - saddle/pass systems;
  - secondary hills;
  - broad interior basin shelves;
  - fan-capable coastal outlets;
  - old volcanic bench candidates.
- Add `landform_region_id` and `landform_region_weight` channels.
- Add a `volcano_disabled_height` mode.
- Make macro relief exist with the volcano contribution disabled.
- Use anisotropic region fields and non-radial axes; no global polar fan control.

### Preview evidence

Output folder:

```text
Images/TerrainPreview/Batch006/PR02_LandformRegionGraph/
```

Required outputs:

```text
pr02_color_relief.png
pr02_volcano_disabled_height.png
pr02_landform_region_id.png
pr02_region_blend_weights.png
pr02_macro_relief_only.png
pr02_coastal_lowland_mask.png
pr02_saddle_candidates.png
pr02_manifest.json
```

### Validation to run

```text
Terrain.Landform.RegionCount
Terrain.Landform.RegionCoverage
Terrain.Landform.VolcanoDisabledRelief
Terrain.Landform.LowlandHighlandBalance
Terrain.Landform.NoSquareEdgeLand
Terrain.Landform.NoGlobalRadialAuthority
Terrain.Coast.*
```

### Acceptance metrics

Recommended first gates:

```text
named_landform_region_count >= 8
land_cells_with_region_id_pct >= 99.0
volcano_disabled_relief_m >= 600
volcano_disabled_named_region_count >= 6
coastal_lowland_area_pct between 12 and 35
highland_area_pct above 300 m between 15 and 45
largest_single_region_area_pct <= 35
global_radial_alignment_score <= 0.35
coast hard gates unchanged
```

The exact thresholds can be tuned, but the PR must fail if the volcano-disabled terrain is a bland dome, a radial flower, or a single noisy blob.

### Stop conditions

Stop before PR 3 if:

- hydrology would still be forced to use crater-centered coordinates;
- fewer than six meaningful regions exist;
- region IDs are decorative and not used in elevation composition;
- volcano-disabled preview has no readable terrain;
- coast metrics fail;
- the PR only adds maps without changing the terrain authority.

## PR 3 — Hydrology / Catchment / Erosion Passes

### Goal

Add drainage logic so ridges, valleys, gullies, basins, fans, and later lahar/lava corridors are terrain-derived. This should be a hybrid graph-plus-raster solver, not vague “erosion noise.”

### Required source changes

Likely files/classes:

```text
Source/JungleGame/Public/Terrain/JGTerrainHydrology.h
Source/JungleGame/Public/Terrain/JGTerrainCatchment.h
Source/JungleGame/Public/Terrain/JGTerrainErosionPass.h
Source/JungleGame/Public/Terrain/JGTerrainFlowRouting.h
Source/JungleGame/Public/Terrain/JGTerrainDrainageGraph.h

Source/JungleGame/Private/Terrain/JGTerrainHydrology.cpp
Source/JungleGame/Private/Terrain/JGTerrainCatchment.cpp
Source/JungleGame/Private/Terrain/JGTerrainErosionPass.cpp
Source/JungleGame/Private/Terrain/JGTerrainFlowRouting.cpp
Source/JungleGame/Private/Terrain/JGTerrainDrainageGraph.cpp

Source/JungleGame/Private/Tests/JGTerrainHydrologyTests.cpp
```

Core structures:

```cpp
struct FTerrainOutlet;
struct FCatchmentBasin;
struct FDrainageNode;
struct FDrainageEdge;
struct FErosionPassConfig;
struct FHydrologyMetrics;
struct FFlowDirectionGrid;
struct FFlowAccumulationGrid;
struct FStreamOrderGrid;
```

### Required implementation behavior

- Seed irregular coastal outlets from region/coast fields, not equal angular spacing.
- Fill or route depressions deterministically for preview and authority grids.
- Implement at least one flow-routing mode:
  - D8 as first practical implementation;
  - D∞ as target or optional enhancement;
  - multiple-flow direction as later extension if needed.
- Add flow accumulation and stream initiation threshold.
- Label catchments by outlet.
- Extract ridge divides from watershed boundaries.
- Add erosion-inspired stream-power incision:
  - `E = K * A^m * S^n`, clamped by region/coast constraints.
- Add hillslope diffusion / slope relaxation:
  - `dh/dt = D * laplacian(h)` approximation over limited iterations.
- Add deposition/fan masks near outlets and valley mouths.
- Preserve beach/coast lock after incision/deposition.

### Preview evidence

Output folder:

```text
Images/TerrainPreview/Batch006/PR03_HydrologyCatchments/
```

Required outputs:

```text
pr03_catchment_id.png
pr03_flow_direction.png
pr03_flow_accumulation_log.png
pr03_stream_order.png
pr03_ridge_divides.png
pr03_valley_floor_mask.png
pr03_erosion_depth.png
pr03_deposition_fan_mask.png
pr03_hydrology_overlay_on_relief.png
pr03_manifest.json
```

### Validation to run

```text
Terrain.Hydrology.OutletCountAndSpacing
Terrain.Hydrology.CatchmentCoverage
Terrain.Hydrology.FlowNoUphillMajorChannels
Terrain.Hydrology.StreamOrderExists
Terrain.Hydrology.RidgeDividesMatchCatchments
Terrain.Hydrology.RadialAlignmentLow
Terrain.Hydrology.FanNearOutlets
Terrain.Coast.*
```

### Acceptance metrics

```text
major_catchment_count between 16 and 48
catchment_coverage_land_pct >= 98.0
largest_catchment_area_pct <= 20
outlet_spacing_cv >= 0.25
drainage_direction_entropy >= configured warning threshold
mean_stream_curvature_score >= 0.20
radial_stream_alignment_score <= 0.35
ridge_divide_cells_pct between 2 and 12
flow_accumulation/channel map aligns with visible incised valleys
beach_continuity_pct == 100.0 after erosion
```

### Stop conditions

Stop before PR 4 if:

- catchments are equal-angle pizza slices;
- flow paths do not align with visible terrain;
- erosion cuts the beach ring or breaks the ocean mask;
- catchment IDs are generated but not used by elevation/mask composition;
- lava/lahar routing would still need radial splines;
- hydrology is implemented only as noise.

## PR 4 — Volcanic Subsystem Rework

### Goal

Rebuild the volcano as a local geological subsystem embedded in the landform graph and hydrology. The active cone should remain a major landmark but must not own the island.

### Required source changes

Likely files/classes:

```text
Source/JungleGame/Public/Terrain/JGTerrainVolcanicSubsystem.h
Source/JungleGame/Public/Terrain/JGTerrainCrater.h
Source/JungleGame/Public/Terrain/JGTerrainLavaFlow.h
Source/JungleGame/Public/Terrain/JGTerrainLahar.h
Source/JungleGame/Public/Terrain/JGTerrainVolcanicMetrics.h

Source/JungleGame/Private/Terrain/JGTerrainVolcanicSubsystem.cpp
Source/JungleGame/Private/Terrain/JGTerrainCrater.cpp
Source/JungleGame/Private/Terrain/JGTerrainLavaFlow.cpp
Source/JungleGame/Private/Terrain/JGTerrainLahar.cpp
Source/JungleGame/Private/Terrain/JGTerrainVolcanicMetrics.cpp

Source/JungleGame/Private/Tests/JGTerrainVolcanicSubsystemTests.cpp
```

Core structures:

```cpp
struct FVolcanicConstruct;
struct FCraterRim;
struct FCollapseScar;
struct FRiftLine;
struct FSecondaryCone;
struct FLavaFlowPath;
struct FLavaLobe;
struct FLaharCorridor;
struct FVolcanicMetrics;
```

### Required implementation behavior

- Offset/attach active construct to regional terrain instead of placing it as global center authority.
- Limit local polar/radial math to the active cone/crater neighborhood.
- Add compact active cone, summit crater, broken rim, breach sector, and crater floor.
- Add collapse scar/horseshoe amphitheater oriented by local slope/catchment context.
- Add rift/fissure line and secondary cones following regional stress/region axes, not equal angles.
- Route lava paths with terrain descent / hydrology assistance.
- Route lahar corridors through catchment data.
- Add old lava benches as terrain/heightfield and masks.
- Add terrain-derived hard-blocker/unstable crust masks only as terrain channels, not gameplay effects.
- Re-run final coast clamp and coast validation after volcanic composition.

### Preview evidence

Output folder:

```text
Images/TerrainPreview/Batch006/PR04_VolcanicSubsystem/
```

Required outputs:

```text
pr04_volcano_footprint_mask.png
pr04_active_cone_mask.png
pr04_crater_rim_floor_breach.png
pr04_collapse_scar.png
pr04_rift_secondary_cones.png
pr04_lava_path_graph.png
pr04_lava_lobe_mask.png
pr04_lahar_catchment_coupling.png
pr04_old_lava_bench_mask.png
pr04_volcano_disabled_height.png
pr04_manifest.json
```

### Validation to run

```text
Terrain.Volcano.ActiveConeFootprint
Terrain.Volcano.BroaderHighlandFootprint
Terrain.Volcano.LocalRadialOnly
Terrain.Volcano.CraterRimFloorGeometry
Terrain.Volcano.CollapseScarGeometry
Terrain.Volcano.LavaPathsFollowSlope
Terrain.Volcano.LaharPathsFollowCatchments
Terrain.Volcano.VolcanoDisabledTerrainStillInteresting
Terrain.Coast.*
```

### Acceptance metrics

```text
active_cone_land_area_pct <= 8
volcanic_highland_land_area_pct <= 30
volcano_disabled_relief_m >= 600
volcano_disabled_catchment_count >= 12
crater_floor_below_rim_m between 40 and 250
lava_path_downhill_compliance_pct >= 95
lahar_corridor_catchment_alignment_pct >= 90
global_radial_alignment_score <= 0.35
local_cone_radial_window_area_pct explicitly recorded
coast hard gates unchanged
```

### Stop conditions

Stop before PR 5 if:

- active cone dominates the whole island;
- volcano-disabled preview fails;
- crater-centered coordinates still drive global ridges/gullies;
- lava/lahar paths ignore catchments;
- volcanic masks are decorative only;
- hard-blocker masks imply gameplay/damage behavior instead of terrain geometry constraints;
- coast gates fail.

## PR 5 — Feature Grammar And Multi-Scale Detail

### Goal

Add named feature grammar and detail fields that enrich terrain identity without replacing graph, hydrology, or volcanology. Detail must support landforms; it must not become terrain authority.

### Required source changes

Likely files/classes:

```text
Source/JungleGame/Public/Terrain/JGTerrainFeatureGrammar.h
Source/JungleGame/Public/Terrain/JGTerrainFeatureStamp.h
Source/JungleGame/Public/Terrain/JGTerrainNoiseScaleBand.h
Source/JungleGame/Public/Terrain/JGTerrainDomainWarp.h
Source/JungleGame/Public/Terrain/JGTerrainIdentityMetrics.h

Source/JungleGame/Private/Terrain/JGTerrainFeatureGrammar.cpp
Source/JungleGame/Private/Terrain/JGTerrainFeatureStamp.cpp
Source/JungleGame/Private/Terrain/JGTerrainNoiseScaleBand.cpp
Source/JungleGame/Private/Terrain/JGTerrainDomainWarp.cpp
Source/JungleGame/Private/Terrain/JGTerrainIdentityMetrics.cpp

Source/JungleGame/Private/Tests/JGTerrainFeatureGrammarTests.cpp
```

Core structures:

```cpp
struct FTerrainFeatureGrammar;
struct FTerrainFeatureStamp;
struct FNoiseScaleBand;
struct FDomainWarpField;
struct FRegionFeatureRule;
struct FTerrainIdentityMetrics;
```

### Required implementation behavior

Add physical scale bands:

| Band | Approx. wavelength | Allowed use | Forbidden use |
|---|---:|---|---|
| Island macro | 24-80 km | island envelope, broad asymmetry | high-frequency ridge texture |
| Regional | 8-24 km | upland blocks, old benches, basin shelves | circular crater fans |
| Basin | 2-8 km | catchment relief, ridge families, valley floors | coastline-breaking offsets |
| Feature | 500 m-2 km | scarps, terraces, saddles, fans, old lava lobes | replacing hydrology |
| Detail | 80-500 m | roughness, minor benches, secondary channels | dominant terrain structure |
| Micro preview/debug only | 12-80 m | local roughness placeholders | runtime authority without LOD plan |

Add feature stamps for:

```text
plateau
bench
scarp
saddle
terrace
coastal_flat
alluvial_fan
floodplain
old_lava_field
secondary_hill
collapse_apron
minor_valley_floor
```

Add domain warp fields:

- vector-based;
- region/axis-aligned;
- seeded by named streams;
- no global circular warp;
- no equal-angle radial repetition.

### Preview evidence

Output folder:

```text
Images/TerrainPreview/Batch006/PR05_FeatureGrammar/
```

Required outputs:

```text
pr05_feature_stamp_atlas.png
pr05_scale_band_contribution_atlas.png
pr05_warp_vector_map.png
pr05_feature_ownership_map.png
pr05_composition_stage_00_macro.png
pr05_composition_stage_01_hydrology.png
pr05_composition_stage_02_volcano.png
pr05_composition_stage_03_feature_grammar.png
pr05_composition_stage_04_final_coast_clamped.png
pr05_manifest.json
```

### Validation to run

```text
Terrain.FeatureGrammar.FeatureCount
Terrain.FeatureGrammar.ScaleBandBudget
Terrain.FeatureGrammar.NoDetailAsAuthority
Terrain.FeatureGrammar.WarpNonRadial
Terrain.FeatureGrammar.FeatureOwnershipCoverage
Terrain.FeatureGrammar.CompositionStageParity
Terrain.Coast.*
```

### Acceptance metrics

```text
feature_stamp_count >= 24
named_feature_type_count >= 8
scale_band_manifest_count >= 5
largest_detail_band_elevation_pct_of_relief <= 10
feature_ownership_land_pct >= 60
unowned_noise_mush_area_pct <= 25
warp_vector_global_radial_score <= 0.30
coast hard gates unchanged after final composition
```

### Stop conditions

Stop before PR 6 if:

- detail noise creates the main terrain instead of region/hydrology/volcano passes;
- feature stamps are not deterministic or not manifest-recorded;
- warp fields create circular artifacts;
- feature grammar breaks hydrology or coast continuity;
- no stage previews show contribution separation.

## PR 6 — Preview Exporter Expansion

### Goal

Make terrain review possible without launching the full game and without guessing. This PR should formalize tracked preview evidence, local heavy artifacts, manifest schema, and reproducibility fields.

### Required source/tooling changes

Likely files/classes/scripts:

```text
Source/JungleGame/Public/Terrain/JGTerrainPreviewExport.h
Source/JungleGame/Public/Terrain/JGTerrainEvidenceManifest.h
Source/JungleGame/Public/Terrain/JGTerrainRasterExport.h

Source/JungleGame/Private/Terrain/JGTerrainPreviewExport.cpp
Source/JungleGame/Private/Terrain/JGTerrainEvidenceManifest.cpp
Source/JungleGame/Private/Terrain/JGTerrainRasterExport.cpp

Source/JungleGame/Private/Commandlets/JGTerrainGenerateCommandlet.cpp

scripts/terrain/generate_terrain_preview.py
scripts/terrain/validate_terrain_manifest.py
scripts/terrain/check_generated_artifacts.sh
```

### Required implementation behavior

- Add a commandlet or script entry point for generating previews from a clean checkout.
- Export 1024 px review previews that are tracked.
- Export 4096 px validation previews as optional tracked-or-release evidence depending on repo policy.
- Export 8129-class authority artifacts locally under ignored folders unless intentionally stored by artifact release.
- Generate JSON manifest and Markdown summary.
- Add output checksums for generated rasters.
- Add `git check-ignore` validation for large/local outputs.
- Add clear separation between tracked evidence and ignored generated products.

### Preview evidence

Output folder:

```text
Images/TerrainPreview/Batch006/PR06_PreviewExporter/
```

Tracked outputs:

```text
pr06_color_relief_1024.png
pr06_height_1024.png
pr06_slope_1024.png
pr06_terrain_mask_atlas_1024.png
pr06_landform_region_id_1024.png
pr06_catchment_id_1024.png
pr06_flow_accumulation_log_1024.png
pr06_ridge_valley_overlay_1024.png
pr06_radial_alignment_diagnostic_1024.png
pr06_volcano_footprint_1024.png
pr06_manifest.json
pr06_metrics.md
```

Ignored local outputs:

```text
Saved/TerrainBuilds/Batch006/PR06/
Intermediate/TerrainBuilds/Batch006/PR06/
DerivedDataCache/
*.r16
*.raw
*.geotiff
*.tif
*.tiff
*.exr
```

### Validation to run

```text
Terrain.PreviewExporter.OutputSetComplete
Terrain.PreviewExporter.ManifestSchemaValid
Terrain.PreviewExporter.DeterministicChecksums
Terrain.PreviewExporter.TrackedEvidenceSmallEnough
Terrain.PreviewExporter.LargeArtifactsIgnored
Terrain.Coast.*
Terrain.Morphology.SmokeMetricsOnly
```

### Acceptance metrics

```text
all required 1024 preview outputs exist
manifest validates against schema
output checksums stable across repeated generation
tracked preview folder size within project-defined budget
8129 authority artifacts generated locally but ignored
manifest includes source_revision or explicit "unknown"
manifest includes pass/fail reason strings
coast hard gates unchanged
```

### Stop conditions

Stop before PR 7 if:

- preview output is incomplete;
- manifest cannot be read in a fresh chat/review;
- large generated artifacts are staged;
- evidence requires manual editor clicking with no repeatable command;
- preview images do not correspond to the manifest config hash.

## PR 7 — Morphology Validation Gates

### Goal

Turn morphology review from “looks better” into automated pass/warn/fail gates. Coast metrics are necessary but not enough; this PR catches radial artifacts, one-feature terrain, volcano dominance, weak catchment diversity, and boring volcano-disabled terrain.

### Required source changes

Likely files/classes:

```text
Source/JungleGame/Public/Terrain/JGTerrainMorphologyValidation.h
Source/JungleGame/Public/Terrain/JGTerrainRadialArtifactMetrics.h
Source/JungleGame/Public/Terrain/JGTerrainVolcanoDominanceMetrics.h
Source/JungleGame/Public/Terrain/JGTerrainCatchmentDiversityMetrics.h
Source/JungleGame/Public/Terrain/JGTerrainPreviewOutputSet.h

Source/JungleGame/Private/Terrain/JGTerrainMorphologyValidation.cpp
Source/JungleGame/Private/Terrain/JGTerrainRadialArtifactMetrics.cpp
Source/JungleGame/Private/Terrain/JGTerrainVolcanoDominanceMetrics.cpp
Source/JungleGame/Private/Terrain/JGTerrainCatchmentDiversityMetrics.cpp
Source/JungleGame/Private/Terrain/JGTerrainPreviewOutputSet.cpp

Source/JungleGame/Private/Tests/JGTerrainMorphologyValidationTests.cpp
```

Core structures:

```cpp
struct FTerrainMorphologyMetrics;
struct FVolcanoDominanceMetrics;
struct FRadialArtifactMetrics;
struct FCatchmentDiversityMetrics;
struct FPreviewEvidenceManifest;
struct FTerrainPreviewOutputSet;
```

### Required implementation behavior

Hard gates should include:

```text
volcano_disabled_relief_m
volcano_disabled_region_count
volcano_disabled_catchment_count
global_radial_alignment_score
ridge_gully_radial_alignment_score
largest_single_feature_footprint_pct
active_cone_land_area_pct
catchment_count
catchment_area_distribution
outlet_spacing_cv
drainage_curvature_score
landform_region_count
coast hard gates
```

Warnings should include:

```text
slope histogram drift
hypsometric curve drift
feature stamp density
old lava bench area
fan area distribution
minor circular artifact score
visual review recommended
```

### Preview evidence

Output folder:

```text
Images/TerrainPreview/Batch006/PR07_MorphologyGates/
```

Required outputs:

```text
pr07_morphology_gate_report.md
pr07_morphology_gate_report.json
pr07_radial_alignment_diagnostic.png
pr07_volcano_disabled_height.png
pr07_volcano_dominance_map.png
pr07_catchment_diversity_map.png
pr07_landform_diversity_map.png
pr07_slope_histogram.csv
pr07_hypsometry.csv
```

### Validation to run

```text
Terrain.Morphology.RadialSyntheticFixtureFails
Terrain.Morphology.NonRadialFixturePasses
Terrain.Morphology.VolcanoDominanceFailsWhenOversized
Terrain.Morphology.VolcanoDisabledFlatFails
Terrain.Morphology.CatchmentDiversityFailsEqualSlices
Terrain.Morphology.ManifestReasonsPresent
Terrain.Coast.*
```

### Acceptance metrics

Initial hard-fail thresholds:

```text
global_radial_alignment_score <= 0.35
ridge_gully_radial_alignment_score <= 0.40
active_cone_land_area_pct <= 8
volcanic_highland_land_area_pct <= 30
volcano_disabled_relief_m >= 600
volcano_disabled_landform_region_count >= 6
major_catchment_count between 16 and 48
outlet_spacing_cv >= 0.25
largest_catchment_area_pct <= 20
named_landform_region_count >= 8
largest_single_region_area_pct <= 35
coast hard gates unchanged
```

### Stop conditions

Stop before PR 8 if:

- radial synthetic terrain passes;
- equal-angle catchments pass;
- volcano-only terrain passes;
- pass/fail reasons are missing;
- hard gates are only logged but not capable of failing CI/validation;
- coast gates are downgraded to warnings.

## PR 8 — Runtime Terrain Shell Integration

### Goal

Connect the generated terrain source to the UE runtime terrain shell. The runtime shell should consume generated heightfield/channels and metadata, not reimplement source math inside an actor.

### Required source changes

Likely files/classes:

```text
Source/JungleGame/Public/Terrain/JGTerrainRuntimeBridge.h
Source/JungleGame/Public/Terrain/JGTerrainRuntimeComponent.h
Source/JungleGame/Public/Terrain/JGTerrainMeshBuilder.h
Source/JungleGame/Public/Terrain/JGTerrainTileStreaming.h
Source/JungleGame/Public/Terrain/JGTerrainCollisionBuild.h

Source/JungleGame/Private/Terrain/JGTerrainRuntimeBridge.cpp
Source/JungleGame/Private/Terrain/JGTerrainRuntimeComponent.cpp
Source/JungleGame/Private/Terrain/JGTerrainMeshBuilder.cpp
Source/JungleGame/Private/Terrain/JGTerrainTileStreaming.cpp
Source/JungleGame/Private/Terrain/JGTerrainCollisionBuild.cpp

Source/JungleGame/Private/Tests/JGTerrainRuntimeBridgeTests.cpp
```

### Required implementation behavior

- Add a runtime terrain bridge that reads `FJungleTerrainBuildResult` or generated artifact metadata.
- Build runtime mesh/Landscape-compatible output from source heightfield.
- Preserve coordinate frame and sea level.
- Attach mask/channel data for debug materials or terrain consumers.
- Emit log tokens that prove runtime/source parity.
- Keep actor/component thin.

Required runtime log tokens:

```text
JG_TERRAIN_RUNTIME_BRIDGE_006_008
JG_TERRAIN_SOURCE_VERSION=JG_TERRAIN_MATH_CORE_006_001
JG_TERRAIN_WORLD_SIZE_M=97536.0
JG_TERRAIN_SEA_LEVEL_M=0.0
JG_TERRAIN_CONFIG_HASH=sha256:...
JG_TERRAIN_HEIGHTFIELD_HASH=sha256:...
JG_TERRAIN_COAST_GATES=PASS
JG_TERRAIN_MORPHOLOGY_GATES=PASS
```

### Runtime evidence

Output folder:

```text
Images/TerrainPreview/Batch006/PR08_RuntimeBridge/
```

Required outputs:

```text
pr08_runtime_source_parity_report.md
pr08_runtime_source_parity.json
pr08_runtime_overview_screenshot.png
pr08_runtime_coast_review_points.png
pr08_runtime_highland_review_points.png
pr08_runtime_channel_debug_atlas.png
pr08_runtime_log_excerpt.txt
```

### Validation to run

```text
Terrain.Runtime.SourceHeightParity
Terrain.Runtime.MaskChannelParity
Terrain.Runtime.WorldScaleLog
Terrain.Runtime.SeaLevelLog
Terrain.Runtime.NoFallbackToOldActorMath
Terrain.Runtime.TileSeamSmoke
Terrain.Runtime.CollisionSmoke
Terrain.Coast.*
Terrain.Morphology.*
```

### Acceptance metrics

```text
runtime_world_size_m == 97536.0
runtime_heightfield_hash matches generated or records approved quantized conversion hash
runtime_sample_height_error_max_m <= 0.25 for source sample points
runtime_channel_id_match_pct >= 99.5 for sampled debug points
runtime log contains source version/config hash/heightfield hash
no fallback to Batch 004 radial terrain source
coast and morphology gates pass before runtime load
```

### Stop conditions

Stop before PR 9 if:

- runtime rebuilds different terrain from preview;
- logs cannot prove source/config identity;
- runtime actor still owns hidden terrain math;
- collision or seams break coast/sea-level assumptions;
- old terrain source can silently load with the new manifest;
- runtime evidence is screenshots without identity logs.

## PR 9 — DEM Calibration / Benchmark Report

### Goal

Benchmark the generated terrain against real volcanic-island topographic principles without copying a real island. This PR should make the terrain measurable as a plausible synthetic terrain system, not merely a nicer procedural island.

### Required source/tooling changes

Likely files/classes/scripts:

```text
Source/JungleGame/Public/Terrain/JGTerrainDemCalibration.h
Source/JungleGame/Public/Terrain/JGTerrainTopographicBenchmark.h
Source/JungleGame/Public/Terrain/JGTerrainCalibrationProfile.h

Source/JungleGame/Private/Terrain/JGTerrainDemCalibration.cpp
Source/JungleGame/Private/Terrain/JGTerrainTopographicBenchmark.cpp
Source/JungleGame/Private/Terrain/JGTerrainCalibrationProfile.cpp

scripts/terrain/terrain_dem_benchmark.py
scripts/terrain/export_topographic_metric_tables.py
```

Core structures:

```cpp
struct FTopographicBenchmarkMetrics;
struct FDemComparisonTarget;
struct FVolcanicIslandCalibrationProfile;
struct FHypsometryMetrics;
struct FSlopeHistogramMetrics;
struct FBasinAreaDistributionMetrics;
```

### Required implementation behavior

- Add synthetic calibration profiles:
  - shield volcanic island;
  - stratovolcanic island;
  - eroded tropical volcanic island;
  - flank-collapse volcanic island;
  - old lava bench / coastal fan island family.
- Export:
  - hypsometry;
  - slope histogram;
  - basin area histogram;
  - drainage density;
  - ridge/valley curvature;
  - lowland/highland/volcano footprint proportions;
  - volcano-disabled benchmark metrics.
- Compare to target ranges, not to a copied DEM.
- Record calibration version and profile in the manifest.

### Preview evidence

Output folder:

```text
Images/TerrainPreview/Batch006/PR09_DEMCalibration/
```

Required outputs:

```text
pr09_calibration_report.md
pr09_calibration_report.json
pr09_hypsometry.csv
pr09_slope_histogram.csv
pr09_basin_area_histogram.csv
pr09_drainage_density.csv
pr09_ridge_valley_curvature_map.png
pr09_lowland_highland_volcano_proportions.png
pr09_dem_style_summary.png
```

### Validation to run

```text
Terrain.DEMCalibration.HypsometryProfile
Terrain.DEMCalibration.SlopeHistogramBins
Terrain.DEMCalibration.BasinAreaDistribution
Terrain.DEMCalibration.DrainageDensityRange
Terrain.DEMCalibration.VolcanoFootprintRange
Terrain.DEMCalibration.VolcanoDisabledProfile
Terrain.Coast.*
Terrain.Morphology.*
Terrain.Runtime.SourceParity
```

### Acceptance metrics

Use target ranges as reviewable gates/warnings, not as exact copying rules:

```text
coastal_lowland_area_pct: 12-35
mid_slope_area_pct: 25-55
highland_area_pct: 15-45
active_cone_land_area_pct <= 8
volcanic_highland_land_area_pct <= 30
major_catchment_count: 16-48
largest_catchment_area_pct <= 20
slope_histogram has lowland, mid-slope, steep, and blocker classes
hypsometric curve is not flat+single-spike and not one smooth cone
volcano_disabled profile still passes morphology gates
```

### Stop conditions

Stop before PR 10 if:

- calibration is a hand-written claim with no metric tables;
- terrain only matches one selected screenshot angle;
- DEM comparison copies a real island rather than using reference-family ranges;
- calibration ignores volcano-disabled terrain;
- slope/hypsometry/basin metrics contradict morphology pass/fail results.

## PR 10 — Acceptance Handoff And Next Terrain Lane

### Goal

Freeze the accepted terrain contract and define the next terrain lane. This is not a gameplay handoff. It is a terrain-only acceptance artifact that lets later traversal/environment systems consume stable terrain assumptions.

### Required source/docs changes

Likely files:

```text
Docs/Terrain/Batch006_TerrainMathAcceptanceHandoff.md
Docs/Terrain/TerrainChannelContract.md
Docs/Terrain/TerrainGeneratedArtifactPolicy.md
Docs/Terrain/TerrainKnownLimitations.md
Images/TerrainPreview/Batch006/PR10_Acceptance/
```

### Required handoff contents

```text
accepted terrain source version
accepted config hash
accepted world size
authority resolution
preview resolution
runtime bridge status
height range
sea level
coordinate frame
mask/channel table
coast gate results
morphology gate results
DEM calibration results
runtime evidence manifest
artifact hygiene proof
known limitations
next terrain lane options
explicit forbidden-scope reminder
```

### Preview evidence

Output folder:

```text
Images/TerrainPreview/Batch006/PR10_Acceptance/
```

Required outputs:

```text
pr10_final_color_relief.png
pr10_final_height.png
pr10_final_slope.png
pr10_final_mask_atlas.png
pr10_final_landform_regions.png
pr10_final_catchments.png
pr10_final_runtime_evidence_contact_sheet.png
pr10_final_acceptance_manifest.json
pr10_final_acceptance_report.md
```

### Validation to run

```text
Terrain.Acceptance.AllHardGatesPass
Terrain.Acceptance.ManifestComplete
Terrain.Acceptance.HandoffReferencesAcceptedHashes
Terrain.Acceptance.GeneratedArtifactPolicyPasses
Terrain.Acceptance.LaterSystemResumeGatePresent
Terrain.Coast.*
Terrain.Morphology.*
Terrain.Runtime.*
Terrain.DEMCalibration.*
```

### Acceptance metrics

```text
all coast hard gates pass
all morphology hard gates pass
runtime source parity pass
DEM calibration report complete
tracked preview evidence complete
large generated artifacts ignored
handoff references exact accepted config/source/heightfield hashes
later non-terrain systems explicitly blocked unless handoff is accepted
```

### Stop conditions

Stop acceptance if:

- any hard terrain gate fails;
- handoff omits config/source/hash identity;
- later-system assumptions are vague;
- docs imply foliage/ecology/gameplay has been solved;
- generated artifacts are staged accidentally;
- evidence cannot be regenerated.

## Validation Matrix

| Gate | PR1 | PR2 | PR3 | PR4 | PR5 | PR6 | PR7 | PR8 | PR9 | PR10 |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| World size 97536 m | Hard | Hard | Hard | Hard | Hard | Hard | Hard | Hard | Hard | Hard |
| Shoreline error zero/effective zero | Hard | Hard | Hard | Hard | Hard | Hard | Hard | Hard | Hard | Hard |
| Beach continuity 100% | Hard | Hard | Hard | Hard | Hard | Hard | Hard | Hard | Hard | Hard |
| Ocean below sea 100% | Hard | Hard | Hard | Hard | Hard | Hard | Hard | Hard | Hard | Hard |
| Square-edge ocean violations 0 | Hard | Hard | Hard | Hard | Hard | Hard | Hard | Hard | Hard | Hard |
| Deterministic config hash | Hard | Hard | Hard | Hard | Hard | Hard | Hard | Hard | Hard | Hard |
| Source heightfield hash | Hard | Hard | Hard | Hard | Hard | Hard | Hard | Hard | Hard | Hard |
| Landform region count/diversity | Smoke | Hard | Hard | Hard | Hard | Hard | Hard | Hard | Hard | Hard |
| Volcano-disabled relief | Smoke | Hard | Hard | Hard | Hard | Hard | Hard | Hard | Hard | Hard |
| Catchment coverage/diversity | N/A | Smoke | Hard | Hard | Hard | Hard | Hard | Hard | Hard | Hard |
| Flow accumulation/stream order | N/A | N/A | Hard | Hard | Hard | Hard | Hard | Hard | Hard | Hard |
| Volcano compactness | N/A | N/A | N/A | Hard | Hard | Hard | Hard | Hard | Hard | Hard |
| Lava/lahar terrain following | N/A | N/A | Smoke | Hard | Hard | Hard | Hard | Hard | Hard | Hard |
| Feature grammar ownership | N/A | N/A | N/A | N/A | Hard | Hard | Hard | Hard | Hard | Hard |
| Preview manifest completeness | Smoke | Smoke | Smoke | Smoke | Smoke | Hard | Hard | Hard | Hard | Hard |
| Radial artifact diagnostics | Smoke | Hard | Hard | Hard | Hard | Hard | Hard | Hard | Hard | Hard |
| Runtime source parity | N/A | N/A | N/A | N/A | N/A | N/A | Smoke | Hard | Hard | Hard |
| DEM calibration report | N/A | N/A | N/A | N/A | N/A | N/A | Smoke | Smoke | Hard | Hard |
| Artifact hygiene | Smoke | Smoke | Smoke | Smoke | Smoke | Hard | Hard | Hard | Hard | Hard |

## Files And Areas Likely Touched

The exact names should be adapted to the repo’s current conventions, but the implementation should converge toward this ownership model.

```text
Source/JungleGame/Public/Terrain/
  JGTerrainTypes.h
  JGTerrainConfig.h
  JGTerrainBuildMetadata.h
  JGTerrainSourceGrid.h
  JGTerrainChannels.h
  JGTerrainGenerator.h
  JGTerrainPassContext.h
  JGTerrainImplicitFields.h
  JGTerrainLandformGraph.h
  JGTerrainHydrology.h
  JGTerrainCatchment.h
  JGTerrainErosionPass.h
  JGTerrainVolcanicSubsystem.h
  JGTerrainFeatureGrammar.h
  JGTerrainPreviewExport.h
  JGTerrainMorphologyValidation.h
  JGTerrainRuntimeBridge.h
  JGTerrainDemCalibration.h

Source/JungleGame/Private/Terrain/
  matching .cpp files for all terrain source classes

Source/JungleGame/Private/Commandlets/
  JGTerrainGenerateCommandlet.cpp
  JGTerrainValidateCommandlet.cpp

Source/JungleGame/Private/Tests/
  JGTerrainConfigIdentityTests.cpp
  JGTerrainCoastRegressionTests.cpp
  JGTerrainLandformGraphTests.cpp
  JGTerrainHydrologyTests.cpp
  JGTerrainVolcanicSubsystemTests.cpp
  JGTerrainFeatureGrammarTests.cpp
  JGTerrainMorphologyValidationTests.cpp
  JGTerrainRuntimeBridgeTests.cpp

scripts/terrain/
  generate_terrain_preview.py
  validate_terrain_manifest.py
  export_topographic_metric_tables.py
  check_generated_artifacts.sh

Docs/Terrain/
  Batch006_TerrainMathAcceptanceHandoff.md
  TerrainChannelContract.md
  TerrainGeneratedArtifactPolicy.md
  TerrainKnownLimitations.md

Images/TerrainPreview/Batch006/
  PR01_TerrainMathCore/
  PR02_LandformRegionGraph/
  PR03_HydrologyCatchments/
  PR04_VolcanicSubsystem/
  PR05_FeatureGrammar/
  PR06_PreviewExporter/
  PR07_MorphologyGates/
  PR08_RuntimeBridge/
  PR09_DEMCalibration/
  PR10_Acceptance/
```

Generated/local ignored areas:

```text
Saved/TerrainBuilds/
Intermediate/TerrainBuilds/
DerivedDataCache/
Build/
Binaries/
Saved/
*.r16
*.raw
*.geotiff
*.tif
*.tiff
*.exr
```

Do not infer that these exact ignore patterns are already correct. Add tests or checks that prove heavy generated products are ignored before merge.

## Runtime Evidence Expectations

Runtime evidence should not be “it looked right in editor.” It should prove source identity and terrain parity.

Required runtime log fields:

```text
JG_TERRAIN_RUNTIME_BRIDGE_006_008
terrain_schema
terrain_source_version
terrain_config_version
config_hash
heightfield_hash
channel_set_hash
world_size_m
authority_resolution_px
runtime_tile_count
runtime_vertex_spacing_m
sea_level_m
min_height_m
max_height_m
coast_gate_status
morphology_gate_status
runtime_source_parity_status
```

Required runtime review points:

```text
coast_north
coast_south
coast_east
coast_west
coastal_fan_major_01
lowland_region_01
major_valley_01
ridge_divide_01
old_lava_bench_01
active_cone_flank_01
crater_rim_01
collapse_scar_01
```

Required runtime outputs:

```text
runtime log excerpt
runtime source parity JSON
height sample comparison CSV
channel sample comparison CSV
contact sheet of review point screenshots
debug channel atlas screenshot
tile seam/coast seam smoke screenshot
```

Runtime should be allowed to use UE Landscape, DynamicMesh, ProceduralMesh, or a staged shell depending on current project constraints. However, the runtime representation is a consumer, not the terrain authority. UE Landscape has documented valid component/heightmap size constraints and 16-bit heightmap import/export paths; UProceduralMeshComponent is documented by Epic as experimental; UDynamicMeshComponent uses an internal `UDynamicMesh`/`FDynamicMesh3` and may be a better runtime/debug bridge for dynamic geometry depending on project needs. These constraints should guide PR 8 rather than being discovered after the math rewrite.

## Risks And Stop Conditions

### Cross-PR hard stops

Do not proceed to the next PR if any of these are true:

```text
world_size_m != 97536.0
shoreline_error_max_m is above effective zero
beach_continuity_pct < 100.0
ocean_below_sea_pct < 100.0
square_edge_ocean_violations > 0
outer world edge is not ocean/container terrain
config/hash identity is unstable
generated evidence cannot be reproduced
large generated artifacts are staged
terrain output depends on global RNG, wall clock, object pointer order, or editor-only manual state
```

### Architecture risks

| Risk | Why it matters | Stop/fix condition |
|---|---|---|
| Actor still owns math | Runtime and preview drift | Move math into pure/source-owned generator |
| Python mirror becomes second authority | Tool/runtime mismatch | Add hash/parity tests or make C++ source canonical |
| Hydrology added before region graph | Catchments become radial/pizza slices | Landform graph must exist first |
| Volcano added before hydrology | Lava/lahar routing becomes radial splines | Catchment/flow graph must exist first |
| Preview exporter added too late | Subsystems cannot be reviewed | Expand smoke previews early; full suite in PR 6 |
| Validation only visual | Fake terrain can pass | Promote morphology gates in PR 7 |
| DEM calibration overfits | Synthetic island copies real DEM | Use reference-family ranges, not DEM cloning |
| Runtime bridge hides fallback | Old Batch 004 math can silently load | Require runtime source/config log tokens |

### Quality risks

| Risk | Detection |
|---|---|
| One-feature terrain | volcano-disabled relief/region/catchment gates |
| Radial/turbine ridges | radial alignment diagnostic |
| Equal-angle outlets | outlet spacing coefficient of variation |
| Noise mush | feature ownership and landform diversity metrics |
| Over-smoothed terrain | slope histogram and relief distribution |
| Coast breaks after erosion | post-pass coast gates |
| Volcano too large | active cone and highland footprint metrics |
| Feature stamps override hydrology | channel/ownership conflict validation |

## Rejected Or Forbidden Approaches

Reject these implementation patterns:

- “Tune Batch 004 radial volcano until it looks less radial.”
- Use crater-centered polar coordinates as global island authority.
- Add hydrology as decorative blue lines or masks only.
- Add erosion as generic noise.
- Add volcano/lava/lahar splines that ignore drainage and slope.
- Add feature grammar before landform/hydrology ownership exists.
- Let detail fields become the main source of terrain structure.
- Accept screenshots without manifest, source version, config hash, and metrics.
- Store massive generated heightfields in Git without an explicit artifact policy.
- Generate evidence manually through editor clicks only.
- Let runtime terrain differ from preview/export terrain.
- Downgrade coast gates to warnings.
- Treat player-facing UI, map, minimap, GPS, compass, objective markers, foliage, ecology, weather, NPCs, buildings, audio, gameplay, or final materials as terrain progress.
- Accept docs-only or tiny fake PRs that do not change terrain source/tooling/validation/runtime behavior.

## Sources And References

### Internal Batch 005 / Batch 006 input map

These are implementation inputs, not final runtime docs:

- **Batch 005 / 001 — Geomorphology Math Core Foundation**: source authority hierarchy, implicit fields, landform graph, volcano-disabled acceptance, deterministic identity.
- **Batch 005 / 002 — Hydrology, Erosion, And Catchment Solver**: outlets, catchments, D8/D∞/MFD strategy, stream-power incision, diffusion, fans, anti-radial metrics.
- **Batch 005 / 004 — Volcanic Subsystem, Lava, Collapse, And Rift Math**: compact embedded volcano, local-only radial math, crater/breach/collapse, lava/lahar terrain coupling.
- **Batch 005 / 005 — Multi-Scale Noise Feature Grammar**: scale bands, feature stamps, domain warp, feature ownership, deterministic terrain identity.
- **Batch 006 / 001 — DEM Benchmarking And Real Volcanic Island Calibration**: hypsometry, slope histograms, basin distributions, drainage density, volcano footprint calibration.
- **Batch 006 / 002 — Offline Generator And Runtime Bridge Architecture**: canonical source, offline/editor generation, artifact policy, runtime bridge, manifest schema.
- **Batch 006 / 003 — Morphology Validation And Preview Evidence Suite**: preview outputs, radial artifact metrics, volcano dominance metrics, catchment diversity metrics.
- **Batch 006 / 004 — Terrain Math Implementation PR Sequence**: this sequenced implementation plan.

### External technical references

[^ue-landscape]: Epic Games, **Landscape Technical Guide in Unreal Engine**. Documents Landscape architecture, valid/recommended dimensions, and examples including 8129 x 8129 and 4033 x 4033. https://dev.epicgames.com/documentation/unreal-engine/landscape-technical-guide-in-unreal-engine

[^ue-heightmap]: Epic Games, **Importing and Exporting Landscape Heightmaps in Unreal Engine**. Documents 16-bit grayscale PNG/R16 heightmap import/export support. https://dev.epicgames.com/documentation/unreal-engine/importing-and-exporting-landscape-heightmaps-in-unreal-engine

[^ue-world-partition]: Epic Games, **World Partition in Unreal Engine**. Documents World Partition as an automatic data-management and distance-based level-streaming system for large worlds. https://dev.epicgames.com/documentation/unreal-engine/world-partition-in-unreal-engine

[^ue-data-validation]: Epic Games, **Data Validation in Unreal Engine**. Documents custom scripted validation rulesets for assets. https://dev.epicgames.com/documentation/en-us/unreal-engine/data-validation-in-unreal-engine

[^ue-automation]: Epic Games, **Automation System Overview**. Documents Unreal automation tests for unit, feature, and content stress testing. https://dev.epicgames.com/documentation/en-us/unreal-engine/automation-system-overview

[^ue-asset-management]: Epic Games, **Asset Management in Unreal Engine**. Documents Primary/Secondary Assets and Primary Asset IDs, useful for stable terrain config assets. https://dev.epicgames.com/documentation/unreal-engine/asset-management-in-unreal-engine

[^ue-random-streams]: Epic Games, **Random Streams in Unreal Engine**. Documents repeatable random streams for procedural distributions. https://dev.epicgames.com/documentation/unreal-engine/random-streams-in-unreal-engine

[^ue-procedural-mesh]: Epic Games, **UProceduralMeshComponent API Reference**. Documents the custom triangle mesh component and notes that it is experimental. https://dev.epicgames.com/documentation/unreal-engine/API/Plugins/ProceduralMeshComponent/UProceduralMeshComponent

[^ue-dynamic-mesh]: Epic Games, **UDynamicMeshComponent API Reference**. Documents runtime dynamic mesh component based on `UDynamicMesh` / `FDynamicMesh3`. https://dev.epicgames.com/documentation/unreal-engine/API/Runtime/GeometryFramework/UDynamicMeshComponent

[^gitignore]: Git, **gitignore Documentation**. Documents that `.gitignore` specifies intentionally untracked files for Git to ignore. https://git-scm.com/docs/gitignore

[^git-check-ignore]: Git, **git-check-ignore Documentation**. Documents checking whether paths are excluded by ignore rules. https://git-scm.com/docs/git-check-ignore

[^qgis-hydro]: QGIS Documentation, **Hydrological analysis**. Describes catchment area / flow accumulation and channel initiation thresholds. https://docs.qgis.org/latest/en/docs/training_manual/processing/hydro.html

[^saga-morphometry]: SAGA GIS, **Slope, Aspect, Curvature**. Documents terrain morphometric parameters including slope, aspect, and curvature. https://saga-gis.sourceforge.io/saga_tool_doc/9.9.2/ta_morphometry_0.html

[^taudem-dinf]: TauDEM, **D-Infinity Flow Directions**. Describes Tarboton’s D∞ method using steepest downslope triangular facets and angle-proportioned flow. https://hydrology.usu.edu/taudem/taudem5/help53/DInfinityFlowDirections.html

[^priority-flood]: Barnes, Lehman, and Mulla, **Priority-Flood: An Optimal Depression-Filling and Watershed-Labeling Algorithm for Digital Elevation Models**. Describes Priority-Flood depression filling and watershed labeling for DEMs. https://arxiv.org/abs/1511.04463

[^stream-power]: Whipple and Tucker, **Dynamics of the stream-power river incision model**. Journal of Geophysical Research: Solid Earth, 1999. https://asu.elsevierpure.com/en/publications/dynamics-of-the-stream-power-river-incision-model-implications-fo/

[^grass-stream-order]: GRASS GIS, **r.stream.order**. Documents Strahler and other stream hierarchy methods for drainage networks. https://grass.osgeo.org/grass-stable/manuals/addons/r.stream.order.html

[^hack-law]: Sassolas-Serrayet et al., **The shape of watersheds**. Discusses Hack’s law relationship between watershed area and main stream length. https://pmc.ncbi.nlm.nih.gov/articles/PMC6141598/

[^musgrave]: Musgrave, Kolb, and Mace, **The Synthesis and Rendering of Eroded Fractal Terrains**. SIGGRAPH 1989. Establishes erosion-aware fractal terrain synthesis concepts. https://dl.acm.org/doi/10.1145/74334.74337

[^usgs-glossary]: U.S. Geological Survey, **Volcano Hazards Program Glossary**. Defines lahar and other volcano-hazard terms. https://www.usgs.gov/glossary/volcano-hazards-program-glossary

[^usgs-lahars]: U.S. Geological Survey, **Lahars move rapidly down valleys like rivers of concrete**. Explains lahars as water/rock-fragment flows that typically enter river valleys. https://www.usgs.gov/programs/VHP/lahars-move-rapidly-down-valleys-rivers-concrete

[^bgs-volcanic-hazards]: British Geological Survey, **Volcanic hazards**. Lists volcanic hazards including lava flows, pyroclastic flows, lahars, landslides, and debris avalanches. https://www.bgs.ac.uk/discovering-geology/earth-hazards/volcanoes/volcanic-hazards/

[^lava-model]: Costa and Macedonio, **Numerical simulation of lava flows based on depth-averaged equations**. Describes simplified 2D lava flow modeling approaches. https://arxiv.org/abs/physics/0504049

[^gdal-cog]: GDAL, **COG — Cloud Optimized GeoTIFF generator**. Documents COG creation and overview handling, relevant for optional large raster artifact workflows. https://gdal.org/en/stable/drivers/raster/cog.html
