# Full-Size Mask And Biome Debug State Research

## Executive Summary

Jungle Game should treat full-size mask state as **developer-only, terrain-derived world evidence** attached to the actual loaded island level, not as a player-facing map system. The next implementation PR should define a narrow debug/validation layer that samples the loaded UE Landscape and relevant world actors, derives repeatable classification masks, and lets developers inspect those masks at review points and through isolated debug visualization modes.

The recommended model is a tiled, world-space classification dataset with stable canonical names. Each sampled cell stores both raw terrain measures and discrete categories:

- raw measures: height, slope degrees, aspect degrees, curvature/relative position, flow accumulation proxy, creek distance, coast distance, and obstruction score;
- derived classes: height band, slope class, shelter/aspect class, landform class, wetness class, coastal transition, lowland/alluvial/gorge candidate, traversal class, false affordance candidate, and biome placeholder;
- audit fields: source landscape actor, sample resolution, threshold profile ID, generator version, timestamp, World Partition cell or grid coordinate, and seam/continuity status.

The implementation should deliberately stay one PR wide: define data structures, canonical enums, review point reporting, debug render modes, packaged-build exclusion rules, and validation checks. It should **not** add player navigation UI, final ecology, procedural world generation, full hydrology simulation, or production gameplay logic.

The main design constraint is that all debug output must help developers prove that survival/navigation evidence comes from terrain and ecology. It must never teach the player through a map, compass, marker, orientation meter, HUD time display, or character hint. Debug may exist in editor and Development/Test builds only, and the PR must prove that it is excluded or inert in Shipping builds.

## Assumptions And Constraints

### Project assumptions

- The project uses Unreal Engine 5 and targets Linux-first development.
- The island is large enough that World Partition or an equivalent large-world streaming workflow is relevant.
- The terrain source is a UE Landscape or Landscape-backed heightfield with possible supporting actors such as creek splines, coastline/ocean volumes, blocking meshes, foliage volumes, or biome placeholder volumes.
- The current project already has PR5 contracts for macro island terrain, biome mask vocabulary, traversal categories, creek/riparian/wet-valley systems, ridge/slope/canopy orientation, lowland/alluvial/gorge concepts, route evidence, and validation evidence.
- This document specifies **map state** as loaded level/landscape classification state. It does not specify a map UI.

### Hard player-facing boundary

The implementation must not expose, imply, or leave dormant hooks for player-facing navigation aids. Forbidden player-facing features include:

- map, minimap, compass, bearing number, heading readout, GPS marker, objective marker, objective arrow;
- HUD clock, explicit time display, orientation confidence meter, hidden orientation meter;
- character comments that tell the player whether they are oriented;
- any modern navigation tool or diegetic equivalent that functions as a map/compass.

Developer debug can visualize masks only when clearly isolated from normal play. A reviewer should be able to inspect the PR and answer: “Can a player ever turn this into a navigation aid?” The required answer is “no.”

### Engine assumptions and version notes

- Unreal’s World Partition stores a world in one persistent level and subdivides it into streamable grid cells loaded around streaming sources. This is directly relevant to seam validation because derived masks must not change at cell edges merely because a neighboring cell is unloaded. See Epic’s World Partition documentation.
- Unreal Data Layers can organize actors in World Partition and have Editor and Runtime forms. For this PR, debug inspection actors should prefer **Editor-only Data Layers** unless there is a proven need for Development runtime inspection.
- Landscape Edit Layers support non-destructive heightmap and weightmap edits. Debug masks should not require destructive edits to the landscape source.
- Unreal provides debug drawing infrastructure such as UDebugDrawComponent, UDebugDrawService, and UDrawDebugLibrary. These are suitable references for developer visualization, but this PR should avoid Blueprint-only debug dependencies where a C++ editor/developer module is cleaner.
- Shipping builds strip console commands, stats, and profiling tools according to Unreal build-configuration documentation, but the PR should not rely only on that. It should also compile-gate and cook-exclude debug data.

Version-dependent API names and plugin availability must be checked against the project’s pinned UE5 version before coding. This research uses public UE5 documentation current at research time.

## Terrain-Derived Mask Model

### Core representation

Use a **world-space tiled raster model** over the island landscape. Each tile covers a fixed world-space square and contains samples at a fixed spacing, for example 4 m, 8 m, or 16 m depending on island scale and performance. The tile grid should not be the same as the visual biome grid, World Partition grid, or landscape component grid. It should be an analysis grid with stable origin, spacing, and halo padding.

Recommended conceptual asset names:

```text
UJGMasksDefinitionAsset
UJGMaskThresholdProfile
FJGMaskSample
FJGMaskTile
UJGMasksWorldState
AJGMaskReviewPoint
AJGMaskDebugVisualizer
```

These are not required class names, but the implementation should keep this vocabulary explicit.

### Sample payload sketch

A representative sample should carry enough evidence to explain every discrete class:

```yaml
sample:
  world_xy: [12840.0, -3944.0]
  height_m: 43.2
  slope_deg: 18.6
  aspect_deg: 132.0
  relative_position: "toe_slope"
  plan_curvature: -0.18
  profile_curvature: 0.22
  creek_distance_m: 21.4
  coast_distance_m: 640.0
  ocean_signed_distance_m: 640.0
  flow_accum_proxy: 0.77
  wetness_proxy: 0.69
  blocker_score: 0.18
  classes:
    height_band: "JGHeight_Lowland"
    slope_class: "JGSlope_Moderate"
    shelter_class: "JGShelter_Leeward"
    landform_class: "JGLandform_ValleyFloor"
    creek_influence: "JGWetness_RiparianInfluence"
    coastal_transition: "JGCoast_Inland"
    candidate_landscape_unit: "JGLandscapeUnit_AlluvialFlat"
    traversal_hard: false
    traversal_soft: true
    traversal_affordance: "JGTraverse_CreekCorridor"
    false_affordance: "JGFalseAffordance_None"
    biome_placeholder: "JGBiome_LowlandWetForest"
  audit:
    source: "LoadedLandscape"
    threshold_profile: "IslandDebug_v001"
    generator_version: "MaskDebugState_004"
    tile_id: "T_012_009"
    wp_region: "WP_X12_Y09"
```

### Derivation order

The PR should define the pipeline in a deterministic order so reports are explainable:

1. **Sample terrain source**: height and local neighbor heights.
2. **Compute first-order terrain derivatives**: slope and aspect.
3. **Compute local morphology**: curvature, relative elevation window, ridge/valley/spur/saddle/slope-break candidate.
4. **Compute water adjacency proxies**: creek distance, downstream valley distance, flow accumulation proxy, topographic wetness proxy.
5. **Compute coast adjacency**: signed distance to ocean/coastline, beach/strand/coastal forest transition.
6. **Compute traversal blockers**: cliffs, water, dense collision/foliage obstruction, slope hard limits.
7. **Compute traversal affordances**: creek corridor, ridge crest, open understory, beach travel, saddle/pass.
8. **Compute false affordance candidates**: terrain that visually suggests a route but fails continuity, safety, slope, wetness, or blocker checks.
9. **Compute biome placeholder** from height, landform, wetness, coast distance, aspect/shelter, and traversal context.
10. **Write audit and seam status** for review points and validation.

### Halo and normalization rule

Every tile must be computed with a **halo** around its border. The halo should be at least the largest radius used by any derivative or neighborhood rule. For example, if landform classification uses a 64 m search radius and wetness uses a 96 m smoothing radius, the tile should load or cache at least 96 m of neighbor samples.

Do not normalize slope, wetness, height bands, or biome class thresholds per World Partition cell. Cell-local normalization creates fake square/grid-aligned biomes and is one of the major errors this PR must detect. Thresholds should be global per island or regional only when the region boundary is a named authored ecological/terrain boundary, not a streaming boundary.

### Terrain science basis

The model should stay pragmatic rather than pretending to simulate full tropical ecology. Terrain derivatives are still a valid starting point:

- hydrologic derivative datasets commonly include DEM, flow direction, flow accumulation, slope, compound topographic index, streams, and catchments;
- topographic wetness index is a standard proxy derived from upslope contributing area and slope, but it is a proxy, not proof of standing water;
- ecological sampling and ecosystem mapping commonly stratify by elevation, landform, slope, aspect, moisture, soil/geology, and vegetation patterns;
- geomorphon-style landform classification is useful as a conceptual reference for ridge, valley, slope, spur, hollow, and similar forms, but Jungle Game should use a simpler explainable subset first.

## Recommended Mask Set For Jungle Game

### Canonical mask naming rules

Use stable names that describe the **meaning** of the mask, not the current implementation method.

Rules:

- Prefix project-specific classes with `JG`.
- Use singular category names for enum values.
- Avoid vague names such as `Good`, `Bad`, `Area1`, `Area2`, `WetMaybe`, or `GreenBiome`.
- Avoid player-facing language such as `NavigationScore`, `ObjectiveRoute`, `CorrectPath`, or `PlayerLost`.
- Keep raw scalar fields separate from discrete class fields.

### Required canonical masks

| Mask | Canonical field | Type | Purpose |
|---|---:|---:|---|
| Height band | `height_band` | enum | Coarse elevation zone for terrain/ecology/debug reports. |
| Slope class | `slope_class` | enum | Movement cost, blocker detection, erosion/landform support. |
| Aspect/shelter | `shelter_class` | enum | Leeward/windward exposure, sun/moisture/canopy placeholder logic. |
| Landform | `landform_class` | enum | Ridge, valley, spur, saddle, slope break, hollow, midslope. |
| Creek influence | `creek_influence` | enum + distance | Active channel, bank, riparian, wet valley, seep, dry swale. |
| Wetness proxy | `wetness_proxy` | scalar 0..1 + class | Moisture likelihood from terrain and creek influence. |
| Coastal transition | `coastal_transition` | enum + distance | Ocean, surf, beach, strand, coastal forest, inland. |
| Candidate landscape unit | `candidate_landscape_unit` | enum | Lowland/alluvial/gorge/toe-slope/terrace candidates. |
| Traversal hard blocker | `traversal_hard_blocker` | bool + reason flags | Impassable or should-be-impassable terrain state. |
| Traversal soft blocker | `traversal_soft_blocker` | bool + reason flags | Heavy cost/delay/risk without absolute blockage. |
| Traversal affordance | `traversal_affordance` | enum + confidence | Terrain-supported movement opportunity. |
| False affordance | `false_affordance` | enum + reason flags | Appears traversable but fails validation. |
| Biome placeholder | `biome_placeholder` | enum + confidence | Early biome zoning for inspection only. |
| Review point class | `review_point_class` | enum | Hand-authored/audited inspection site category. |

### Height band enum

Suggested enum:

```text
EJGHeightBand
- JGHeight_OceanBelowDatum
- JGHeight_BeachAndStrand
- JGHeight_CoastalLow
- JGHeight_Lowland
- JGHeight_Foothill
- JGHeight_Upland
- JGHeight_RidgeHigh
- JGHeight_PeakOrCliffTop
```

Thresholds should be in a profile asset, not hard-coded. Example first-pass thresholds:

| Class | Example range | Notes |
|---|---:|---|
| `OceanBelowDatum` | `< 0 m` | Use island/ocean datum, not absolute world Z. |
| `BeachAndStrand` | `0–4 m` | Only valid near coast. Inland depressions should not become beach. |
| `CoastalLow` | `4–15 m` | Coastal transition support. |
| `Lowland` | `15–80 m` | Primary wet tropical lowland zone. |
| `Foothill` | `80–180 m` | Transition. |
| `Upland` | `180–320 m` | Cloud/cooler/wind-exposed placeholder support if island height permits. |
| `RidgeHigh` | `320–500 m` | Relative high terrain. |
| `PeakOrCliffTop` | `> 500 m` | Optional; use relative relief if the island is lower. |

For a stylized island, relative height bands may be more useful than real-world meters. Store both absolute height and normalized island relief percentile.

### Slope class enum

Suggested enum:

```text
EJGSlopeClass
- JGSlope_Flat
- JGSlope_Gentle
- JGSlope_Moderate
- JGSlope_Steep
- JGSlope_VerySteep
- JGSlope_Cliff
```

Example first-pass thresholds:

| Class | Degrees | Gameplay/debug interpretation |
|---|---:|---|
| `Flat` | `0–3°` | Floodplain, beach, valley floor, terrace candidate. |
| `Gentle` | `3–10°` | Efficient travel, lowland/alluvial support. |
| `Moderate` | `10–22°` | Traversable slope with cost. |
| `Steep` | `22–35°` | Soft blocker candidate unless route exists. |
| `VerySteep` | `35–50°` | Hard/near-hard blocker unless authored climb/scramble. |
| `Cliff` | `> 50°` | Hard blocker by default. |

### Aspect/shelter class enum

Aspect by itself is not enough for jungle survival readability. Pair it with shelter/exposure:

```text
EJGShelterClass
- JGShelter_FlatOrUndefined
- JGShelter_WindwardExposed
- JGShelter_LeewardSheltered
- JGShelter_CoastalExposed
- JGShelter_RidgeExposed
- JGShelter_ValleySheltered
- JGShelter_Mixed
```

Inputs:

- aspect direction;
- prevailing wind direction profile;
- coast-facing direction;
- ridge/valley class;
- canopy openness or sky-window proxy when available.

The first implementation can use a simple dot product between aspect vector and configured prevailing wind/coast vectors, then override with landform class where obvious: ridge crest is exposed, valley floor is sheltered, beach/strand is coastal exposed.

### Landform class enum

Use a simple, explainable geomorphology vocabulary:

```text
EJGLandformClass
- JGLandform_Flat
- JGLandform_RidgeCrest
- JGLandform_Shoulder
- JGLandform_Spur
- JGLandform_Midslope
- JGLandform_Hollow
- JGLandform_ValleyFloor
- JGLandform_Channel
- JGLandform_Saddle
- JGLandform_SlopeBreak
- JGLandform_CliffFace
- JGLandform_Unknown
```

Minimum evidence per class:

| Class | Required evidence |
|---|---|
| `RidgeCrest` | local high relative elevation, divergent slopes, elongated crest continuity. |
| `Shoulder` | convex break below ridge/high point, downslope steepening. |
| `Spur` | narrow convex projection descending from ridge, valley on both sides. |
| `Midslope` | neither local high nor local low, consistent slope. |
| `Hollow` | concave local depression/swale without active creek classification. |
| `ValleyFloor` | local low, convergent slopes, low gradient, near channel or wetness. |
| `Channel` | creek spline/channel or high flow accumulation and very low relative position. |
| `Saddle` | local pass between two highs with valley/hollow axes on opposing sides. |
| `SlopeBreak` | local curvature transition with significant slope class change. |
| `CliffFace` | very steep slope and high local relief. |

A full geomorphon implementation is not required in this PR. The PR should define the category contract and implement a conservative, explainable first pass.

### Wetness and creek influence enum

```text
EJGWetnessClass
- JGWetness_DryRidge
- JGWetness_DrySlope
- JGWetness_MesicSlope
- JGWetness_WetValley
- JGWetness_RiparianInfluence
- JGWetness_ActiveChannel
- JGWetness_CoastalWet
- JGWetness_StandingWaterOrSwamp
```

Inputs:

- creek distance;
- creek/order tag if authored;
- flow accumulation proxy;
- slope;
- concavity/valley floor class;
- coastal low elevation and ocean/lagoon distance;
- optional rainfall/exposure profile.

The key rule: label this as `wetness_proxy`, not “water truth.” Terrain can suggest wetness, but final water/soil ecology needs later authored or simulated validation.

### Coast/ocean distance enum

```text
EJGCoastalTransitionClass
- JGCoast_Ocean
- JGCoast_SurfOrIntertidal
- JGCoast_Beach
- JGCoast_BackBeachStrand
- JGCoast_CoastalScrub
- JGCoast_CoastalForest
- JGCoast_Inland
- JGCoast_CliffCoast
- JGCoast_RiverMouthOrEstuary
```

Use signed distance to ocean/coastline plus height and slope:

- low, flat, near ocean: beach/intertidal/strand;
- high, steep, near ocean: cliff coast;
- near creek mouth and low elevation: river mouth/estuary;
- inland distance beyond threshold: inland.

### Lowland/alluvial/gorge candidate enum

```text
EJGLandscapeUnitCandidate
- JGLandscapeUnit_None
- JGLandscapeUnit_LowlandFlat
- JGLandscapeUnit_AlluvialFlat
- JGLandscapeUnit_Floodplain
- JGLandscapeUnit_Terrace
- JGLandscapeUnit_ToeSlopeFan
- JGLandscapeUnit_GorgeFloor
- JGLandscapeUnit_GorgeWall
- JGLandscapeUnit_SaddlePass
- JGLandscapeUnit_CoastalPlain
```

Candidate logic:

- `AlluvialFlat`: low slope, valley floor, near creek, low relative elevation.
- `Floodplain`: alluvial flat plus very high wetness/flow proxy and low height above channel.
- `Terrace`: flat/gentle surface near valley but elevated above current channel.
- `ToeSlopeFan`: slope break at base of steep terrain, fan-like concavity/planform.
- `GorgeFloor`: narrow valley floor, close to channel, steep walls nearby.
- `GorgeWall`: very steep slopes flanking a channel/valley.
- `SaddlePass`: saddle landform plus traversable slope continuity.
- `CoastalPlain`: low height, low slope, coastal transition not ocean/beach.

## Biome Placeholder Classification Model

### Purpose

Biome placeholders are not final ecology. They are **debug zoning labels** that let developers inspect whether terrain-derived evidence is coherent enough to support future vegetation, wildlife, weather, soundscape, and traversal authoring.

The model should answer:

- Does this terrain location currently read as coastal, lowland, ridge, wet valley, gorge, or upland?
- Are placeholder biome boundaries following terrain features?
- Are any biome patches square, grid-aligned, or streaming-cell aligned?
- Do review points show the evidence expected by PR5 world-validation contracts?

### Biome placeholder enum

```text
EJGBiomePlaceholder
- JGBiome_Unknown
- JGBiome_OceanOrOpenWater
- JGBiome_BeachStrand
- JGBiome_CoastalScrub
- JGBiome_CoastalForest
- JGBiome_MangroveOrEstuaryPlaceholder
- JGBiome_LowlandWetForest
- JGBiome_LowlandDryerForest
- JGBiome_RiparianGallery
- JGBiome_AlluvialFlatForest
- JGBiome_GorgeWetForest
- JGBiome_SlopeForest
- JGBiome_RidgeForest
- JGBiome_ExposedRidgeScrub
- JGBiome_UplandCloudForestPlaceholder
- JGBiome_CliffOrRock
- JGBiome_DisturbedOrDebugOnly
```

### First-pass decision table

| Biome placeholder | Required terrain evidence |
|---|---|
| `OceanOrOpenWater` | Below ocean datum or water body. |
| `BeachStrand` | Near coast, low height, low slope, coastal transition beach/backbeach. |
| `CoastalScrub` | Near coast, exposed shelter, above beach, low-to-moderate slope. |
| `CoastalForest` | Coastal influence but outside beach/scrub strip, lowland/foothill. |
| `MangroveOrEstuaryPlaceholder` | Very low coastal wet/river-mouth candidate; only if island concept supports it. |
| `LowlandWetForest` | Lowland, mesic/wet, not coast, not active channel. |
| `LowlandDryerForest` | Lowland, dry/mesic, ridge or slope away from creek. |
| `RiparianGallery` | Riparian influence or active channel edge. |
| `AlluvialFlatForest` | Alluvial flat/floodplain/terrace with moderate wetness. |
| `GorgeWetForest` | Gorge floor/wall, high shelter, wetness/creek influence. |
| `SlopeForest` | Midslope/shoulder/spur, moderate slope, non-coastal. |
| `RidgeForest` | Ridge/spur/shoulder, drier or exposed, traversable. |
| `ExposedRidgeScrub` | Ridge high/exposed, shallow/steep/dry proxy. |
| `UplandCloudForestPlaceholder` | Upland/ridge high, high wetness/shelter if island height warrants it. |
| `CliffOrRock` | Cliff/very steep hard blocker. |
| `DisturbedOrDebugOnly` | Explicit debug/authored placeholder; never inferred silently. |

### Boundary quality requirements

Biome placeholder boundaries should be explainable by one or more terrain drivers:

- coast distance threshold;
- elevation band transition;
- ridge/valley morphology;
- wetness/creek influence;
- slope/landform break;
- authored ecology override with explicit reason.

A biome boundary is suspicious when it:

- runs exactly along a World Partition cell edge;
- forms rectangles with identical width/height as grid cells;
- ignores creek corridors, ridges, and coastal shape;
- changes abruptly at a landscape component boundary without terrain change;
- has large flat patches with no terrain-derived evidence.

### Confidence score

Each biome placeholder should include `biome_confidence` from `0.0` to `1.0`.

Use confidence to prioritize review, not gameplay:

- `0.85–1.0`: multiple terrain signals agree.
- `0.60–0.85`: enough evidence for debug placeholder.
- `0.35–0.60`: weak/mixed evidence; needs review.
- `< 0.35`: unknown or likely false classification.

Do not expose confidence to the player.

## Traversal Classification Model

### Traversal state split

Traversal should be split into **hard blockers**, **soft blockers**, and **affordance zones**. These are separate masks because a terrain location can be costly but still part of a plausible route, or visually open but blocked by slope/wetness/collision.

Recommended fields:

```text
traversal_hard_blocker: bool
traversal_hard_reasons: bitflags
traversal_soft_blocker: bool
traversal_soft_reasons: bitflags
traversal_affordance: EJGTraversalAffordance
traversal_confidence: float
```

### Hard blocker reasons

```text
EJGHardBlockerReason
- JGHardBlocker_None
- JGHardBlocker_CliffSlope
- JGHardBlocker_DeepWater
- JGHardBlocker_Ocean
- JGHardBlocker_DenseCollision
- JGHardBlocker_ImpassableVegetation
- JGHardBlocker_GorgeWall
- JGHardBlocker_AuthoredNoTraverse
- JGHardBlocker_OutOfWorld
```

Hard blocker threshold examples:

- slope above cliff threshold;
- ocean/deep water or surf zones not designed for traversal;
- collision/foliage density above hard threshold;
- narrow gorge wall/rock face;
- explicit authored volume.

### Soft blocker reasons

```text
EJGSoftBlockerReason
- JGSoftBlocker_None
- JGSoftBlocker_SteepSlope
- JGSoftBlocker_WetMud
- JGSoftBlocker_DenseUnderstory
- JGSoftBlocker_LowVisibilityCanopy
- JGSoftBlocker_UnstableBank
- JGSoftBlocker_ThornScrubPlaceholder
- JGSoftBlocker_RockyGround
- JGSoftBlocker_AuthoredSlow
```

Soft blockers should support survival simulation later, but this PR only needs classification state and debug inspection.

### Traversal affordance enum

```text
EJGTraversalAffordance
- JGTraverse_None
- JGTraverse_BeachCorridor
- JGTraverse_CreekCorridor
- JGTraverse_RidgeCrestRoute
- JGTraverse_SaddlePass
- JGTraverse_OpenUnderstory
- JGTraverse_AlluvialFlat
- JGTraverse_GorgeFloor
- JGTraverse_GameTrailPlaceholder
- JGTraverse_DrySwale
```

Evidence examples:

| Affordance | Required evidence |
|---|---|
| `BeachCorridor` | Beach/backbeach, low slope, continuous coast-parallel strip. |
| `CreekCorridor` | Riparian/wet valley, local floor not active deep channel, slope not hard. |
| `RidgeCrestRoute` | Ridge/spur continuity, moderate slope, not cliff, not dense hard blocker. |
| `SaddlePass` | Saddle morphology connecting two traversable slopes. |
| `OpenUnderstory` | Low obstruction proxy or authored foliage volume with open-travel tag. |
| `AlluvialFlat` | Low slope valley/alluvial unit with no deep wet blocker. |
| `GorgeFloor` | Narrow valley floor with creek corridor and no water/cliff hard blocker. |
| `GameTrailPlaceholder` | Authored/debug-only path-like ecological affordance; must not become objective route. |
| `DrySwale` | Hollow/dry swale with gentle slope and continuity. |

### Route evidence without route guidance

The classification may identify terrain affordances, but it must not tell the player the “right route.” For developer reports, use wording such as:

- “affordance candidate: creek corridor”
- “continuity: 420 m local connected component”
- “blocking reason: steep slope + dense collision”

Do not use wording such as:

- “correct path”
- “go here”
- “objective route”
- “player should follow this”
- “lost/oriented confidence”

## False Affordance Classification

### Purpose

False affordance detection identifies locations that appear traversable or navigationally meaningful but fail the terrain/obstruction/continuity checks. This is vital for avoiding unfair world reads.

A false affordance candidate is not automatically bad. Dense jungle can mislead. The goal is to distinguish **intentional survival ambiguity** from **broken terrain communication**.

### False affordance enum

```text
EJGFalseAffordanceClass
- JGFalseAffordance_None
- JGFalseAffordance_DeadEndCreek
- JGFalseAffordance_BlockedRidge
- JGFalseAffordance_FakeBeachCorridor
- JGFalseAffordance_VisualGapBlockedByCollision
- JGFalseAffordance_GentleSlopeEndsInCliff
- JGFalseAffordance_OpenUnderstoryTooShort
- JGFalseAffordance_WetFlatIsImpassable
- JGFalseAffordance_GridArtifact
- JGFalseAffordance_ConflictingSignals
```

### Detection rules

A candidate should be flagged when one or more of these occur:

- **Dead-end creek**: creek corridor affordance component terminates quickly in hard blocker without visible terrain explanation.
- **Blocked ridge**: ridge crest appears continuous but has collision/foliage hard blockers or cliff breaks not visually readable.
- **Fake beach corridor**: coastal strip is classified as beach travel but interrupted by cliff coast, surf, deep water, or collision within a short distance.
- **Visual gap blocked by collision**: obstruction/collision mask says hard blocked while terrain/slope says open.
- **Gentle slope ends in cliff**: consecutive gentle samples abruptly transition into cliff without slope-break/canopy/rock evidence.
- **Open understory too short**: affordance component length below minimum useful travel length.
- **Wet flat impassable**: alluvial/wet lowland looks flat but water/mud/swamp hard or soft blocker dominates.
- **Grid artifact**: class patch boundary aligns strongly with analysis, landscape, or World Partition grid.
- **Conflicting signals**: high confidence biome, traversal, and landform labels disagree.

### Required report fields

Every false affordance report should include:

```yaml
false_affordance:
  class: "JGFalseAffordance_BlockedRidge"
  severity: "warning"
  location: [10520.0, -8120.0, 164.0]
  evidence:
    landform_class: "JGLandform_RidgeCrest"
    traversal_affordance: "JGTraverse_RidgeCrestRoute"
    hard_blocker: true
    hard_reasons: ["JGHardBlocker_DenseCollision"]
    connected_length_m: 34.0
    required_min_length_m: 120.0
  recommendation: "Review collision/foliage density or add visual terrain cue."
```

### Severity levels

| Severity | Meaning |
|---|---|
| `info` | Useful ambiguity or expected local interruption. |
| `warning` | Likely readability issue; needs review point or screenshot. |
| `blocker` | Breaks route evidence, creates fake biome/grid patch, or contradicts hard rules. |

## Developer-Only Debug View Strategy

### Debug views

Implement debug as isolated visualizer modes, not gameplay UI.

Recommended modes:

```text
JG.DebugMask.HeightBand
JG.DebugMask.SlopeClass
JG.DebugMask.Landform
JG.DebugMask.Wetness
JG.DebugMask.Coast
JG.DebugMask.TraversalHard
JG.DebugMask.TraversalSoft
JG.DebugMask.TraversalAffordance
JG.DebugMask.FalseAffordance
JG.DebugMask.BiomePlaceholder
JG.DebugMask.ReviewPoints
JG.DebugMask.Seams
```

The names above can be console commands, editor toolbar commands, or visualizer-mode IDs. In Shipping they must not exist or must be inert behind compile gates.

### Visualization approach

Preferred implementation pattern:

1. **Editor/developer actor or component** reads `UJGMasksWorldState`.
2. It draws colored tile overlays, outlines, labels, and review-point markers.
3. It uses debug drawing or a debug material overlay only in editor/dev contexts.
4. It never creates UMG widgets, HUD elements, player inventory objects, map textures, minimaps, objective arrows, or compass-like indicators.

Acceptable strategies:

- `UDebugDrawComponent` / `UDebugDrawService` style scene rendering for labels and outlines.
- `UDrawDebugLibrary` or DrawDebug helpers for simple lines/boxes/spheres in Development.
- Editor-only visualization actor in an Editor Data Layer.
- Temporary debug material instance applied by editor command, provided it is not cooked and cannot be accessed by players.
- Generated Markdown/JSON reports saved under a developer diagnostics directory.

Avoid:

- persistent gameplay HUD widgets;
- map texture assets representing the whole island;
- player-accessible debug menus;
- runtime inventory item “survey map”;
- compass needle or heading display;
- blueprint-only debug logic hidden in gameplay actors.

### Color policy

Colors should be documented in the debug definition asset, but the PR should not depend on exact colors for correctness. Reports and tests must use class names and reason flags, not screenshots alone.

For visual inspection, use distinct color families:

- height bands: sequential ramp;
- slope: low to high intensity;
- wetness: dry to wet;
- coast: ocean/beach/inland bands;
- blockers: hard blocker high contrast, soft blocker secondary;
- affordances: route-like candidates but explicitly labeled developer-only;
- false affordances: warning colors;
- seams: edge mismatch overlays.

### Developer-only activation

Recommended gates:

- `WITH_EDITOR` for editor visualization tooling.
- `WITH_EDITORONLY_DATA` for editor-only debug assets and actor properties.
- `!UE_BUILD_SHIPPING` for any Development/Test runtime diagnostic command.
- Project setting or CVar defaulting to disabled, for example `jg.MaskDebug.Enabled 0`.
- Cook rules excluding debug assets, review reports, and debug material overlays from Shipping.
- Automated packaging check that fails if debug assets/classes appear in Shipping manifest.

### Debug data lifecycle

Debug state should be regenerable. Do not make it authoritative gameplay data.

Recommended storage:

```text
/Content/World/MaskDebug/Definitions/DA_JGMaskDefinitions.uasset
/Content/World/MaskDebug/ThresholdProfiles/DA_JGMaskThreshold_IslandDebug_v001.uasset
/Saved/JungleGame/MaskReports/004_full_size_mask_debug/*.json
/Saved/JungleGame/MaskReports/004_full_size_mask_debug/*.md
```

If the project uses source control for generated debug snapshots, keep snapshots small and deliberate. Large raster dumps should go to saved artifacts, not normal content, unless the team explicitly wants reviewed golden fixtures.

## Review Point Classification Reports

### Purpose

Review points are hand-selected or sampled points in the actual loaded world that produce textual evidence. They are the bridge between “mask colors look right” and “this world state has explainable terrain-derived survival evidence.”

### Review point types

```text
EJGReviewPointClass
- JGReview_CoastBeach
- JGReview_CoastCliff
- JGReview_RiverMouth
- JGReview_CreekCorridor
- JGReview_WetValley
- JGReview_RidgeCrest
- JGReview_Spur
- JGReview_Saddle
- JGReview_SlopeBreak
- JGReview_AlluvialFlat
- JGReview_GorgeFloor
- JGReview_GorgeWall
- JGReview_FalseAffordance
- JGReview_BiomeBoundary
- JGReview_WorldPartitionSeam
```

### Report format

Produce both JSON and Markdown for review. JSON is machine-checkable; Markdown is human-readable.

Representative JSON:

```json
{
  "report_version": "JGMaskReview_004",
  "world": "/Game/Maps/JungleIsland",
  "threshold_profile": "IslandDebug_v001",
  "review_point": {
    "name": "RP_Creek_Valley_017",
    "class": "JGReview_CreekCorridor",
    "location": [12840.0, -3944.0, 43.2]
  },
  "sample": {
    "height_band": "JGHeight_Lowland",
    "slope_class": "JGSlope_Gentle",
    "shelter_class": "JGShelter_ValleySheltered",
    "landform_class": "JGLandform_ValleyFloor",
    "creek_influence": "JGWetness_RiparianInfluence",
    "wetness_proxy": 0.72,
    "coastal_transition": "JGCoast_Inland",
    "candidate_landscape_unit": "JGLandscapeUnit_AlluvialFlat",
    "traversal_hard_blocker": false,
    "traversal_soft_blocker": true,
    "traversal_soft_reasons": ["JGSoftBlocker_WetMud"],
    "traversal_affordance": "JGTraverse_CreekCorridor",
    "false_affordance": "JGFalseAffordance_None",
    "biome_placeholder": "JGBiome_RiparianGallery",
    "biome_confidence": 0.88
  },
  "neighborhood": {
    "radius_m": 64,
    "dominant_landform": "JGLandform_ValleyFloor",
    "dominant_biome": "JGBiome_RiparianGallery",
    "affordance_connected_length_m": 460,
    "nearest_hard_blocker_m": 94
  },
  "continuity": {
    "world_partition_seam_nearby": false,
    "tile_edge_mismatch_count": 0,
    "grid_artifact_score": 0.03
  },
  "verdict": "pass"
}
```

Representative Markdown:

```markdown
### RP_Creek_Valley_017 — Creek Corridor

- Location: X 12840.0, Y -3944.0, Z 43.2
- Terrain: Lowland, gentle slope, valley floor
- Water evidence: Riparian influence, wetness proxy 0.72
- Traversal: Creek corridor affordance; soft blocker wet mud; no hard blocker
- Biome placeholder: Riparian gallery, confidence 0.88
- Continuity: 460 m connected affordance, no seam mismatch
- Verdict: PASS
```

### Required review set

The PR should include or support at least these review points:

| Minimum count | Point class |
|---:|---|
| 2 | Coast beach/backbeach |
| 1 | Coast cliff |
| 2 | Creek corridor |
| 2 | Wet valley/alluvial flat |
| 2 | Ridge/spur |
| 1 | Saddle/pass |
| 1 | Gorge floor or gorge wall |
| 2 | Biome boundary |
| 2 | World Partition seam |
| 2 | False affordance candidate |

The exact number can be adjusted, but the first implementation must cover every target category in the research prompt.

### Verdict logic

Reports should classify each review point as:

```text
pass
warning
fail
blocked_by_missing_source
```

Examples:

- `pass`: terrain evidence and classification agree.
- `warning`: mixed evidence or low confidence, but no forbidden/systemic issue.
- `fail`: contradiction, grid artifact, missing boundary continuity, or bad blocker state.
- `blocked_by_missing_source`: required creek/coast/collision source unavailable.

Do not mark missing sources as pass.

## World Partition / Boundary Continuity Checks

### Why continuity matters

Large-world streaming can create false classification edges if masks are generated independently per cell, if neighboring terrain is missing during derivation, or if thresholds are normalized locally. Unreal World Partition’s grid-cell streaming model makes this a core validation risk.

### Required seam checks

For every mask tile edge and every World Partition-adjacent region, compute:

```text
edge_class_mismatch_count
edge_class_mismatch_ratio
edge_scalar_delta_mean
edge_scalar_delta_max
hard_blocker_edge_flip_count
affordance_edge_break_count
biome_edge_flip_count
wetness_edge_delta_max
landform_edge_transition_reason
```

A mismatch is acceptable when terrain evidence changes at the seam. A mismatch is suspicious when the seam itself explains the change.

### Boundary check report

Example seam report:

```yaml
seam:
  id: "WP_X12_Y09_to_WP_X13_Y09"
  sample_spacing_m: 8
  halo_m: 96
  checks:
    biome_edge_flip_ratio: 0.04
    hard_blocker_edge_flip_count: 0
    wetness_delta_max: 0.09
    landform_mismatch_ratio: 0.08
    grid_artifact_score: 0.01
  verdict: "pass"
  notes:
    - "Biome flips coincide with creek crossing and slope break."
```

Fail example:

```yaml
seam:
  id: "WP_X07_Y04_to_WP_X08_Y04"
  checks:
    biome_edge_flip_ratio: 0.63
    hard_blocker_edge_flip_count: 18
    wetness_delta_max: 0.52
    grid_artifact_score: 0.91
  verdict: "fail"
  notes:
    - "Biome boundary follows World Partition vertical edge with no terrain, creek, coast, or slope evidence."
```

### Grid-aligned biome artifact detector

Implement a lightweight detector before any fancy ecology work.

Signals:

- biome boundary overlaps WP cell edge longer than threshold;
- class patch bounding boxes align to tile/cell coordinates;
- boundary orientation histogram has unnatural spikes at 0°/90°;
- patch dimensions match WP cell or landscape component dimensions;
- adjacent cells have same terrain metrics but different biome labels;
- sudden changes in class distribution at cell borders.

Suggested metrics:

```text
grid_edge_overlap_ratio = boundary_length_on_grid_lines / total_boundary_length
rectangularity = patch_area / patch_bounding_box_area
cell_dimension_match = abs(patch_width - wp_cell_width) < tolerance
axis_aligned_boundary_ratio = boundary_length_at_0_90_deg / total_boundary_length
terrain_explained_boundary_ratio = boundary_length_near_slope_creek_coast_break / total_boundary_length
```

Fail when grid evidence is high and terrain-explained evidence is low.

### Continuity across unloaded regions

For editor validation, the generator should load or access enough neighbor data to compute full halo context. If neighbor terrain is unavailable, mark samples `blocked_by_missing_source` rather than silently deriving weak classifications.

Runtime developer inspection should display a clear warning when a tile was generated with missing neighbor context.

## Packaged-Build Safety For Debug Data

### Required safety stance

Debug mask data is powerful. It effectively exposes the hidden structure of the island. It must be treated as developer diagnostics and excluded from player builds.

### Compile-time gates

The PR should use multiple layers:

```cpp
#if WITH_EDITOR
// editor visualizer, editor report generation
#endif

#if !UE_BUILD_SHIPPING
// development-only runtime diagnostics, if any
#endif

#if WITH_EDITORONLY_DATA
// editor-only asset fields, debug colors, review metadata
#endif
```

Do not place essential gameplay data inside editor-only masks. If future gameplay needs terrain classes, create a separate production-safe data path with a separate review.

### Cook/package gates

Required checks:

- Debug assets are placed in a known debug/developer path.
- Shipping cook excludes debug paths or marks debug data as never-cook.
- Generated reports under `Saved/` are never included in packaged content.
- Debug visualizer actor is absent from Shipping worlds or inert and stripped.
- Console commands/CVars for debug masks do not exist or do nothing in Shipping.
- A packaged-build audit checks that no asset/class name matching `MaskDebug`, `ReviewPoint`, or `JG.DebugMask` is present in the Shipping manifest unless explicitly whitelisted for non-debug metadata.

### Runtime gates

If Development runtime inspection is needed:

- require a CVar that defaults off;
- require a developer build target;
- render only through debug draw/scene visualizer;
- never write to gameplay save data;
- never expose through input bindings available in normal play;
- never replicate debug data to clients unless specifically running a developer multiplayer test.

### Acceptance proof

The implementation PR should include one of the following:

- command/output from a Shipping package audit;
- automated test that asserts debug assets are absent from the cook manifest;
- build log grep showing debug module/editor plugin not included;
- documented manual verification if automation is not practical yet.

Manual proof is acceptable for the first PR, but the PR should leave a clear path to automate it.

## Implementation-Sized PR Scope

### Recommended one-PR objective

Add **Full-Size Mask Debug State v004**: a developer-only schema, generator/reporting contract, and debug visualization strategy for loaded island terrain.

The PR should be able to land without final art, final ecology, or complete procedural generation.

### In scope

- Define canonical mask enums/data categories.
- Define threshold/profile data asset or config struct.
- Define sample/tile/review report structures.
- Add developer-only review point actor or data record.
- Add report generation for selected review points.
- Add debug visualization mode registry or stub visualizer.
- Add seam/grid-artifact validation functions or report stubs.
- Add packaged-build safety rules/checklist.
- Add documentation describing usage and forbidden scope.

### Out of scope

- Player map or minimap.
- Player compass, bearing, heading, objective, or orientation UI.
- Final biome ecology.
- Final hydrology simulation.
- Procedural creek generation overhaul.
- Production AI navigation.
- Complete foliage placement.
- Save-game integration.
- Runtime player-facing traversal hints.
- Networked debug tooling.
- Major world-generation refactor.

### Narrow PR success path

A good implementation PR can be considered complete when a developer can:

1. open the full-size island world in editor;
2. run a developer-only command or editor action;
3. inspect named mask categories over terrain;
4. select review points and generate Markdown/JSON reports;
5. see seam/grid-artifact warnings;
6. verify debug data is absent or inert in Shipping;
7. review the diff and confirm no player-facing navigation aid was added.

## Files And Areas Likely Touched

Actual paths depend on the repository layout. Likely areas:

```text
Source/JungleGame/Public/World/Masks/
Source/JungleGame/Private/World/Masks/
Source/JungleGameEditor/Public/MaskDebug/
Source/JungleGameEditor/Private/MaskDebug/
Content/World/MaskDebug/Definitions/
Content/World/MaskDebug/ThresholdProfiles/
Content/World/MaskDebug/ReviewPoints/
Docs/World/FullSizeMaskDebugState.md
Config/DefaultGame.ini
Config/DefaultEditor.ini
```

Suggested file responsibilities:

| Area | Responsibility |
|---|---|
| `World/Masks/JGMaskTypes.*` | Enums, bitflags, sample structs, report structs. |
| `World/Masks/JGMaskThresholdProfile.*` | Thresholds for height, slope, wetness, coast, blockers. |
| `World/Masks/JGMasksWorldState.*` | Tiled state container and query API. |
| `Editor/MaskDebug/JGMaskReviewPoint.*` | Review point actor or data object. |
| `Editor/MaskDebug/JGMaskReportWriter.*` | Markdown/JSON report generation. |
| `Editor/MaskDebug/JGMaskDebugVisualizer.*` | Editor-only draw/overlay logic. |
| `Editor/MaskDebug/JGMaskContinuityValidator.*` | Seam and grid-artifact checks. |
| `Docs/World/FullSizeMaskDebugState.md` | Human-facing developer spec. |
| `Config` | CVar defaults, cook exclusion, debug path policy. |

If the project does not already have an editor module, this PR should consider adding one rather than putting editor-only debug visualization into the runtime game module.

## Acceptance Criteria

### Functional criteria

- The project defines canonical enums or equivalent data categories for all required masks:
  - height bands;
  - slope classes;
  - aspect/shelter classes;
  - ridge/valley/spur/saddle/slope-break classification;
  - creek influence and wetness proxy;
  - coast/ocean distance and coastal transitions;
  - lowland/alluvial/gorge candidates;
  - traversal hard blockers;
  - traversal soft blockers;
  - traversal affordance zones;
  - false affordance candidates;
  - biome placeholder zones;
  - review point classifications.
- A loaded island world can produce deterministic review point reports.
- Review reports include raw terrain evidence, derived classes, reasons, confidence where applicable, and pass/warning/fail verdict.
- Debug visualizer modes are developer-only and clearly named.
- World Partition/tile seam checks exist and report continuity status.
- Grid-aligned biome error detection exists at least as a report metric.
- Missing source data produces `blocked_by_missing_source`, not a false pass.

### Safety criteria

- No player-facing map, minimap, compass, bearing, objective marker, objective arrow, HUD clock, orientation meter, or equivalent is added.
- No character hint/comment system is added for orientation state.
- Debug commands are disabled, unavailable, or inert in Shipping.
- Debug assets/reports are not cooked into Shipping packages.
- Debug visualizer code is editor-only or non-shipping gated.
- No production gameplay logic depends on debug-only masks.

### Evidence criteria

The PR description should include:

- sample review point report;
- screenshot or text output of at least one debug visualization mode in editor/development context;
- seam check output;
- grid-artifact detector output;
- packaged-build safety proof;
- statement that forbidden player-facing navigation scope was not implemented.

### Quality criteria

- Thresholds are profile/config driven.
- Mask names are stable and documented.
- Classifications are deterministic for the same world state.
- Derived classes store enough evidence to be audited.
- Tile derivation uses halo context for neighborhood operations.
- No per-cell normalization creates streaming/grid artifacts.
- Reports are small enough to review in PR.

## Risks And Stop Conditions

### Main risks

| Risk | Why it matters | Mitigation |
|---|---|---|
| Debug masks become player navigation | Violates project core design. | Editor/non-shipping gates, no HUD/map assets, shipping audit. |
| Grid-aligned biome patches | Looks artificial and breaks terrain readability. | Global thresholds, halo sampling, artifact detector. |
| Per-cell normalization | Creates discontinuity at streaming boundaries. | Island/global profiles, seam checks. |
| Wetness proxy treated as truth | Produces wrong ecology and unfair traversal. | Name as proxy; require later validation. |
| Over-broad PR | Turns into terrain generator/ecology rewrite. | Keep to schema, reports, debug visualization, validation. |
| Missing neighbor data | Bad landform/wetness at tile edges. | Halo and `blocked_by_missing_source`. |
| Debug data cooked into Shipping | Leaks island structure. | Cook exclusion and package audit. |
| Collision/foliage mismatch | Terrain says passable, world collision says blocked. | Include obstruction/collision reason flags. |

### Stop conditions

Stop implementation and report clearly if:

- the current world has no accessible landscape/height source;
- creek/coast source actors are absent and the PR cannot infer them safely;
- World Partition or landscape streaming prevents halo sampling and no workaround is available;
- packaged-build exclusion cannot be verified;
- implementation requires adding player-facing navigation UI;
- classifications require destructive landscape edits;
- generated masks are strongly grid-aligned and cannot be explained by terrain;
- review reports cannot produce evidence for required categories.

### Deferred decisions

These should be explicitly deferred:

- final biome/ecology taxonomy;
- final wetness/hydrology simulation;
- final traversal stamina/speed gameplay;
- procedural vegetation placement;
- AI animal/human route usage;
- player-facing environmental tutorialization;
- any diegetic navigation tool.

## Rejected Or Forbidden Approaches

### Explicitly forbidden

- Player map.
- Player minimap.
- Player compass.
- Bearing/heading number.
- GPS marker.
- Objective marker.
- Objective arrow.
- HUD clock or explicit time display.
- Orientation confidence meter, hidden or visible.
- Character comments that tell the player whether they are oriented.
- Modern navigation tools.
- Any debug texture or report accessible from normal gameplay.

### Rejected technical approaches

| Approach | Reason rejected |
|---|---|
| One giant island texture mask as gameplay data | Hard to audit, likely leaks map-like information, poor World Partition seam behavior. |
| Per-World-Partition-cell biome normalization | Creates fake square biome patches and seam flips. |
| Debug UMG overlay | Too close to player-facing HUD. Use editor/developer scene visualizer instead. |
| Destructive Landscape painting for debug masks | Pollutes source terrain and art workflow. |
| Final ecology in this PR | Too broad; placeholders are enough. |
| Full hydrology simulation in this PR | Too broad; wetness proxy/reporting is enough. |
| Route/pathfinding labels named “correct path” | Violates navigation design and biases world authoring. |
| Shipping build debug unlock | Leaks traversal and biome structure. |
| Blueprint-only hidden debug in gameplay actors | Harder to audit and easier to accidentally ship. |
| Silent fallback when sources are missing | Produces false confidence; use blocked verdicts. |

### Allowed alternatives

These are acceptable if they preserve the hard boundary:

- Editor-only overlay material for selected mask categories.
- Non-shipping console command for local developer inspection.
- Markdown/JSON reports saved to `Saved/`.
- Editor Data Layer containing review point actors.
- Small golden fixture reports for tests.
- Authored override volumes when they record an explicit reason.

## Sources And References

- Epic Games, [World Partition in Unreal Engine](https://dev.epicgames.com/documentation/unreal-engine/world-partition-in-unreal-engine). Used for the large-world streaming/cell-continuity assumptions.
- Epic Games, [World Partition — Data Layers in Unreal Engine](https://dev.epicgames.com/documentation/unreal-engine/world-partition---data-layers-in-unreal-engine). Used for Editor/Runtime Data Layer separation and debug organization.
- Epic Games, [Landscape Edit Layers in Unreal Engine](https://dev.epicgames.com/documentation/unreal-engine/landscape-edit-layers-in-unreal-engine). Used for the non-destructive landscape-layer recommendation.
- Epic Games, [UDebugDrawComponent API Reference](https://dev.epicgames.com/documentation/unreal-engine/API/Runtime/Engine/UDebugDrawComponent). Used for developer-only scene debug visualization strategy.
- Epic Games, [UDrawDebugLibrary API Reference](https://dev.epicgames.com/documentation/unreal-engine/API/Plugins/DrawDebugLibrary/UDrawDebugLibrary). Used as a reference for debug draw functions and unified debug drawing concepts.
- Epic Games, [Build Configurations Reference](https://dev.epicgames.com/documentation/unreal-engine/build-configurations-reference-for-unreal-engine). Used for Shipping/Development/Test build safety assumptions.
- Epic Games, [Packaging Unreal Engine Projects](https://dev.epicgames.com/documentation/unreal-engine/packaging-your-project). Used for packaging configuration and development-vs-distribution framing.
- Verdin, K. L. (2017), [Hydrologic Derivatives for Modeling and Analysis — A new global high-resolution database](https://pubs.usgs.gov/publication/ds1053), U.S. Geological Survey Data Series 1053. Used for hydrologic derivative layer vocabulary: DEM, flow direction, flow accumulation, slope, compound topographic index, streams, and catchments.
- Sørensen, R., Zinko, U., and Seibert, J. (2006), [On the calculation of the topographic wetness index: evaluation of different methods based on field observations](https://hess.copernicus.org/articles/10/101/2006/hess-10-101-2006.pdf), Hydrology and Earth System Sciences. Used for TWI as a wetness proxy and the formula concept `ln(a/tan β)`.
- GRASS GIS, [r.geomorphon manual](https://grass.osgeo.org/grass-stable/manuals/r.geomorphon.html). Used for landform-classification background and references to Jasiewicz and Stepinski’s geomorphon method.
- Clarke, K. et al. (2017), [GEOSS Ecosystem Mapping for Australia](https://www.tern.org.au/wp-content/uploads/TERN_Landscape_AusCover__GEOSS_Ecosystem_Mapping_for_Australia__Report_Final.pdf). Used for landform/topographic moisture potential as ecosystem drivers and for the idea that combinations of physical drivers can define ecological facets.
- USDA / NRCS / Interagency Ecological Site Handbook for Rangelands (2013), [Interagency Ecological Site Handbook](https://www.ars.usda.gov/ARSUserFiles/30501000/InteragencyEcolSiteHandbook.pdf). Used for ecological site sampling/stratification factors including elevation, landform, slope, aspect, parent material, and vegetation patterns.
