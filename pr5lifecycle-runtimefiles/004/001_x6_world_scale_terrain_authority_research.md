# 6x World Scale Terrain Authority Research

## Executive Summary

Jungle Game should treat PR5 Batch 004 as a **major terrain-source scale rewrite**, not a visual zoom or minor parameter tweak. The current accepted PR5 Batch 003 terrain is a 16.256 km square world with a working deterministic coastline, beach ring, coastal shelf, ocean-below-sea behavior, and square-edge ocean/container terrain. Batch 004 must preserve those acceptance wins while moving the canonical terrain authority to a **97.536 km square world**.

The recommended implementation target is:

```text
base_world_size_m: 16256.0
linear_world_scale: 6.0
new_world_size_m: 97536.0
new_world_half_extent_m: 48768.0

current_mean_island_radius_m: ~7000.0
new_mean_island_radius_m: 42000.0

current_max_island_radius_m: ~7350.0
new_max_island_radius_m: 44100.0

new_min_square_edge_ocean_margin_m: 4668.0
new_area_scale: 36.0
```

The best UE-compatible authority grid target is **8129 x 8129 samples at exactly 12.0 m/sample**:

```text
97536.0 m / (8129 - 1) = 12.0 m/sample
```

That aligns cleanly with Unreal's recommended 8129 x 8129 Landscape size: 127 quads/section, 4 sections/component, 254 x 254 quads/component, 1024 total components in a 32 x 32 component layout. Epic notes that Landscape components are the base unit for rendering, visibility, and collision; each component carries render-thread CPU cost; each section is a draw call; and Epic recommends a maximum of 1024 Landscape Components for the largest Landscapes. This makes 8129 x 8129 a strong single-authority source size even if runtime import later becomes tiled or mesh-based. Reference: [Epic Landscape Technical Guide](https://dev.epicgames.com/documentation/unreal-engine/landscape-technical-guide-in-unreal-engine).

Vertical relief should **not** be scaled 6x. A strict 6x vertical scale would push the current ~1750 m summit toward ~10.5 km, which is not the requested terrain identity and would create extreme gameplay, traversal, fog, and precision problems. Keeping the exact current vertical relief also fails because a 1750 m peak over a ~42 km island radius reads as a flat mound. The recommended target is a **redesigned large volcanic island profile** with a summit envelope around **3600-4200 m above sea level**, defaulting to **3800 m**, plus stronger ridge, basin, gully, crater, and radial-catchment morphology that scales in horizontal footprint but only sublinearly in vertical amplitude.

## Terrain-Only Scope Boundary

This document is limited to:

- deterministic terrain source identity;
- map scale constants;
- heightfield architecture;
- coastline, beach, shelf, and ocean/container constraints;
- preview/export evidence;
- runtime terrain/LOD feasibility;
- developer-only validation metrics.

This document intentionally does **not** design:

- foliage;
- ecology;
- weather;
- audio;
- survival systems;
- NPCs;
- VFX;
- final materials;
- buildings;
- POIs;
- quests;
- player-facing UI;
- player map/minimap/compass/GPS/objective/bearing/time/orientation systems.

Any PR that uses the 6x terrain task as a vehicle for non-terrain content should be stopped for scope creep.

## Current Terrain Baseline

The current accepted PR5 Batch 003 preview evidence is:

```text
world_size_m: 16256.0
resolution_px: 1024
pixel_spacing_m: 15.890518084066471
height_min_m: -145.0
height_max_m: 1750.4691
shoreline_error_max_m: 0.0
beach_continuity_pct: 100.0
ocean_below_sea_pct: 100.0
square_edge_ocean_violations: 0
```

Interpretation:

| Area | Current Status | Must Preserve? |
|---|---:|---:|
| Deterministic terrain generation | Working | Yes |
| Coastline lock | `shoreline_error_max_m: 0.0` | Yes |
| Beach continuity | `100.0%` | Yes |
| Ocean below sea level | `100.0%` | Yes |
| Square edge ocean/container terrain | `0` violations | Yes |
| World linear size | 16.256 km | No, must become 97.536 km |
| Island interior morphology | Too small / underdeveloped for new goal | Must redesign |
| Preview-only 1024 px evidence | Useful QA thumbnail | Must not be confused with source authority |

The current coastline system is valuable. The scale rewrite must keep the coast invariants as final, explicit postconditions rather than incidental byproducts of the height generator.

## Required 6x Scale Target

The new canonical world must use a 6x linear world scale:

```text
new_world_size_m = 16256.0 * 6.0 = 97536.0
new_world_half_extent_m = 48768.0
new_area_scale = 6.0 * 6.0 = 36.0
```

The island radius target should be:

```text
new_mean_island_radius_m = 7000.0 * 6.0 = 42000.0
new_max_island_radius_m = 7350.0 * 6.0 = 44100.0
```

The minimum ocean/container margin between the maximum island radius and square world edge becomes:

```text
new_min_square_edge_ocean_margin_m = 48768.0 - 44100.0 = 4668.0
```

This margin is important. It proves the world is not merely a larger circle jammed into the same square. The outer square edge remains ocean/container terrain, while the island perimeter remains beach/coastal shelf/sea-level transition.

A rough mean-radius area proof should be emitted by validation:

```text
old_island_area_estimate_km2 = pi * 7000^2 / 1_000_000 = ~153.94 km2
new_island_area_estimate_km2 = pi * 42000^2 / 1_000_000 = ~5541.77 km2
area_scale_observed = 36.0
```

## World Constants And Derived Values

Recommended authoritative constants:

```cpp
// Terrain authority identity
constexpr double kPr5BaseWorldSizeM = 16256.0;
constexpr double kPr5WorldLinearScale = 6.0;
constexpr double kPr5WorldSizeM = 97536.0;
constexpr double kPr5WorldHalfExtentM = 48768.0;

// Island footprint
constexpr double kPr5BaseMeanIslandRadiusM = 7000.0;
constexpr double kPr5BaseMaxIslandRadiusM = 7350.0;
constexpr double kPr5MeanIslandRadiusM = 42000.0;
constexpr double kPr5MaxIslandRadiusM = 44100.0;
constexpr double kPr5MinEdgeOceanMarginM = 4668.0;

// Sea and vertical envelope
constexpr double kPr5SeaLevelM = 0.0;
constexpr double kPr5TargetSummitM = 3800.0;
constexpr double kPr5TargetSummitMinAcceptM = 3200.0;
constexpr double kPr5TargetSummitMaxAcceptM = 4300.0;
constexpr double kPr5TargetOceanFloorMinM = -600.0;    // recommended near/ocean container lower target
constexpr double kPr5TargetOceanFloorMaxM = -1200.0;   // optional deeper outer container target

// Source authority
constexpr int32 kPr5AuthorityResolutionPx = 8129;
constexpr double kPr5AuthorityPixelSpacingM = 12.0;

// QA previews
constexpr int32 kPr5PreviewResolutionPx = 1024;        // thumbnail/backward comparable
constexpr int32 kPr5ValidationPreviewResolutionPx = 4096;
```

Recommended manifest-derived values:

```json
{
  "terrain_source_version": "pr5_batch004_x6_world_scale",
  "linear_world_scale": 6.0,
  "area_scale": 36.0,
  "world_size_m": 97536.0,
  "world_half_extent_m": 48768.0,
  "mean_island_radius_m": 42000.0,
  "max_island_radius_m": 44100.0,
  "min_square_edge_ocean_margin_m": 4668.0,
  "authority_resolution_px": 8129,
  "authority_pixel_spacing_m": 12.0,
  "preview_resolution_px": 1024,
  "preview_pixel_spacing_m": 95.34310850439883,
  "validation_preview_resolution_px": 4096,
  "validation_preview_pixel_spacing_m": 23.81831501831502,
  "sea_level_m": 0.0,
  "target_summit_m": 3800.0,
  "height_max_accept_range_m": [3200.0, 4300.0]
}
```

Important distinction:

- `authority_resolution_px` is the actual deterministic terrain authority.
- `preview_resolution_px` is only a visual/QA export.
- If the 1024 preview remains, it must not be the only evidence for a 97.536 km terrain because it would produce ~95.34 m/pixel and lose too much interior morphology.
- A 4096 validation preview gives ~23.82 m/pixel and is a better CI artifact.
- An 8129 authority source gives exactly 12 m/sample.

## Horizontal Scale Strategy

The horizontal rewrite should happen at the terrain source level, not by changing a viewport camera, resampling a PNG, or changing only an import scale in UE.

Recommended approach:

1. Keep a normalized island coordinate model for macro shape:
   - normalized radius `rho = distance_from_center_m / mean_island_radius_m`;
   - signed coast distance in meters from the deterministic coastline;
   - angular coastline function `coast_radius_m(theta)`.

2. Multiply the true world constants by 6x:
   - square world size;
   - island mean radius;
   - island maximum radius;
   - major mountain/rift/catchment layout distances;
   - deterministic feature anchor positions.

3. Preserve coast constraints in **meter-space**, not preview-pixel-space:
   - sea level remains `0.0 m`;
   - beach continuity is evaluated along coastline rays and image masks;
   - shelf and ocean container remain below sea level;
   - square edge rows/columns are tested directly.

4. Avoid pure normalized rescale:
   - a normalized height function stretched over 6x without adding new interior morphology will look empty;
   - add deterministic secondary macro basins, radial ridges, secondary craters, lahar gullies, caldera/rim transitions, and catchment branching appropriate to a ~42 km radius island.

The result should feel like a much larger island with more interior landform hierarchy, not the old island inflated like a balloon.

## Vertical Relief Strategy

The vertical profile should be **redesigned sublinearly**, not kept fixed and not scaled linearly.

### Do not keep the current height unchanged

Keeping `height_max_m ~1750` over a ~42 km mean island radius gives:

```text
1750 / 42000 = 0.0417
approx macro slope angle = atan(0.0417) = ~2.39 degrees
```

That can be realistic for very broad shield terrain, but for a first-person jungle volcanic island it will likely read as too flat unless interior ridges, cliffs, ravines, caldera walls, and local relief are significantly strengthened.

### Do not scale vertical relief by 6x

Scaling the current summit linearly gives:

```text
1750.4691 * 6 = ~10502.8 m
```

That would create an implausibly extreme above-sea summit for this terrain target and would stress traversal, fog, atmosphere, collision, shadow cascades, and content layout. It also changes the project identity from large volcanic island to near-Himalayan superterrain.

### Recommended vertical target

Use a large volcanic island profile with:

```text
target_summit_m: 3800.0
acceptable_height_max_m: 3200.0 to 4300.0
recommended_ocean_floor_outer_m: -600.0 to -1200.0
recommended_shelf_floor_nearshore_m: -10.0 to -80.0
```

This makes the vertical scale factor roughly:

```text
3800.0 / 1750.4691 = ~2.17x
```

That is enough to avoid flatness while preserving a broad volcanic-island silhouette.

### Morphology recommendation

Use a compound profile:

- broad lower shield/base: gentle low-angle rise over most of the radius;
- mid-elevation dissected terrain: stronger ridges, basins, drainages, old flows, and catchments;
- central highland/caldera/rim: steeper local slopes and crater relief;
- flank/rift zones: long radial ridges and valleys that create navigable macro orientation without player-facing UI;
- coastal plain/beach/shelf: explicit terrain constraints, not procedural noise accidents.

The National Park Service describes shield volcanoes as having gentle slopes, broad width relative to height, low-viscosity lava flows, flank fissure vents, and lava flows that can travel great distances from vents. That supports broad macro slopes, but the game terrain should still use stronger local relief to avoid a stretched-flat feel. Reference: [NPS Shield Volcanoes](https://www.nps.gov/articles/000/shield-volcanoes.htm). The British Geological Survey similarly contrasts shield volcanoes with stratovolcanoes and describes shield slopes as gentle, commonly under 10 degrees, from fluid basaltic lava. Reference: [BGS Types of Volcano](https://www.bgs.ac.uk/discovering-geology/earth-hazards/volcanoes/how-volcanoes-form/).

## Coast / Beach / Shelf Preservation

The coast system should be treated as a hard postcondition layer after macro terrain generation.

Required preserved evidence:

```text
shoreline_error_max_m: 0.0 or effectively 0.0
beach_continuity_pct: 100.0
ocean_below_sea_pct: 100.0
square_edge_ocean_violations: 0
outer world edge remains ocean/container terrain
island perimeter remains beach/coastal shelf/sea-level transition
```

Recommended generation order:

1. Generate deterministic macro terrain in normalized island coordinates.
2. Generate deterministic coastline radius/signed-distance field at 6x scale.
3. Generate beach and shelf masks from signed coast distance in meters.
4. Apply macro terrain only inside the protected inner terrain region.
5. Apply beach/shelf/ocean height bands.
6. Reapply final shoreline lock so the accepted coast cannot drift.
7. Re-evaluate all coast metrics from the final heightfield, not intermediate masks.

Recommended coast bands:

| Band | Suggested Width | Scale Rule | Height Behavior |
|---|---:|---|---|
| Shoreline lock | 0-1 source sample or analytic SDF | Fixed in source/sample tolerance | exactly sea level |
| Beach face | 40-140 m | mostly fixed/sublinear | slight positive elevation, continuous |
| Back beach / coastal plain | 150-450 m | sublinear | blends to interior |
| Shallow shelf | 300-1200 m | sublinear | below sea, smooth falloff |
| Outer shelf / container ocean | to square edge | world-scale | always below sea |
| Square edge ocean guard | entire first/last rows and columns | fixed invariant | below sea |

Do **not** scale beach width by exactly 6x unless the intended visual identity is a huge unrealistic resort-like sand belt. Beach width, shoreline lock tolerance, and immediate shelf behavior should remain physically/player-scale. The island radius scales linearly; the coast transition width should scale only enough to maintain resolution robustness and avoid aliasing.

USGS describes the shoreface as a transition zone and temporary sediment reservoir between the shoreline and shelf; this supports treating beach/shelf as an explicit transition system rather than a random noise edge. Reference: [USGS shoreface morphology note](https://www.usgs.gov/centers/spcmsc/news/drs-emily-wei-and-jennifer-miselis-published-manuscript-describing-controls). USGS coastal-change guidance also emphasizes that waves, surge, currents, sediment, and morphology can strongly alter coastal terrain, which supports explicit coast validation rather than trusting visual inspection. Reference: [USGS Coastal Change](https://www.usgs.gov/centers/whcmsc/science/coastal-change).

## Mountain, Basin, Ridge, Gully, Crater Scale Rules

The terrain should use mixed scaling rules. Anything tied to world identity and travel scale should expand strongly; anything tied to human-scale terrain readability or numerical tolerances should remain bounded.

| Feature / Parameter | Recommended Scale Rule | Batch 004 Guidance |
|---|---|---|
| Square world size | Linear 6x | `16256 -> 97536 m` |
| World half extent | Linear 6x | `8128 -> 48768 m` |
| Mean island radius | Linear 6x | `~7000 -> 42000 m` |
| Max island radius | Linear 6x | `~7350 -> 44100 m` |
| Edge ocean margin | Linear 6x | `~778 -> 4668 m` |
| Major coastline lobes | Linear 6x wavelength | Preserve accepted continuity; larger bays/headlands |
| Coastline lock tolerance | Fixed / source-sample bounded | Must stay exact/effectively exact |
| Beach width | Fixed to sublinear | Recommend 40-140 m face, not 6x |
| Back beach/coastal plain | Sublinear | 150-450 m |
| Shallow shelf width | Sublinear | 300-1200 m depending coast |
| Summit height | Sublinear vertical | Default 3800 m, accept 3200-4300 m |
| Macro shield footprint | Linear 6x | Main volcano spans island |
| Caldera/crater diameter | 3-6x, terrain-dependent | Larger than old crater but not mechanically all features |
| Crater rim height/depth | 1.5-2.5x | Strong local relief; do not 6x every vertical |
| Radial ridge length | Linear 6x | Major navigational landforms |
| Radial ridge width | 3-6x | Wide enough to read at 97 km scale |
| Ridge local height | 1.5-3x | Avoid flat ridges |
| Catchment length | Linear 6x | Drainage hierarchy proves scale |
| Gully/ravine length | 4-6x | Major valleys should travel kilometers |
| Gully width | 1.5-3x | Keep playable/credible widths |
| Gully depth | 1.5-3x | Stronger relief, bounded |
| Noise macro wavelength | Linear 6x | Prevent old texture-size appearance |
| Noise meso wavelength | 2-4x plus added octaves | Avoid empty stretched terrain |
| Noise micro wavelength | Fixed/sublinear | Human-scale roughness should not become 6x blobs |
| Erosion mask blur radii | Mostly linear for macro, fixed for coast | Split by mask class |
| Validation tolerances | Fixed or stricter | Do not hide scale errors with bigger tolerances |

Required morphology additions for a convincing 97.536 km island:

- more than one interior basin;
- at least two to four major radial drainage/catchment systems;
- longer lahar/gully corridors;
- old lava-flow terraces or slope benches;
- central crater/caldera and rim that remain visible in 4096+ preview;
- slope/aspect distribution showing differentiated coast, lower flank, mid-flank, highland, and summit zones.

## Source Resolution And Runtime Mesh Consequences

### Authority source resolution

Recommended authority target:

```text
authority_resolution_px: 8129
authority_pixel_spacing_m: 12.0
```

Why this is the best default:

- It exactly fits the 97.536 km world size at 12 m/sample.
- It aligns with Epic's recommended 8129 x 8129 Landscape size.
- It uses 1024 total Landscape Components in a 32 x 32 component layout if represented as a single UE Landscape configuration.
- It is large enough to preserve terrain morphology without pretending to support 1 m geometry across a 97.536 km world.

Epic documents the 8129 x 8129 recommended Landscape size and explains the component/section rules behind valid Landscape dimensions. Reference: [Epic Landscape Technical Guide](https://dev.epicgames.com/documentation/unreal-engine/landscape-technical-guide-in-unreal-engine).

### Preview resolution

Recommended preview/export levels:

| Export | Resolution | Spacing at 97.536 km | Purpose |
|---|---:|---:|---|
| Thumbnail / compatibility preview | 1024 x 1024 | 95.3431085 m/sample | quick visual continuity only |
| CI validation preview | 4096 x 4096 | 23.8183150 m/sample | morphology/coast QA |
| UE-compatible authority export | 8129 x 8129 | 12.0 m/sample | terrain source authority |
| Optional offline high source | 16385 x 16385 | 5.953125 m/sample | heavy offline analysis only |

The old 1024 preview must not be treated as sufficient scale proof. At ~95 m/sample, gullies, ridges, beach bands, and shelf details can alias or vanish.

### Runtime mesh / LOD identity

At 97.536 km, the runtime should assume streaming and LOD from the beginning.

Epic's World Partition documentation describes it as an automatic data-management and distance-based streaming system for large worlds, replacing manually divided sublevels with a single persistent level separated into grid cells. Reference: [Epic World Partition documentation](https://dev.epicgames.com/documentation/unreal-engine/world-partition-in-unreal-engine?lang=en-US).

Epic's Large World Coordinates documentation states that UE5 uses double-precision variants to improve actor placement precision and support massive world scales. Reference: [Epic Large World Coordinates](https://dev.epicgames.com/documentation/unreal-engine/large-world-coordinates-in-unreal-engine-5).

Recommended runtime stance:

- Use UE5 Large World Coordinates assumptions throughout C++ terrain metadata and placement math.
- Avoid storing authoritative world-space terrain positions only as `float`.
- Keep generator/source math in `double` where world distances, radii, coastline SDFs, and fingerprints are calculated.
- Use World Partition or an equivalent deterministic tiled terrain/mesh streaming plan.
- Treat a single full-resolution always-loaded terrain mesh as a blocker.
- If using UE Landscape import, prefer valid/recommended sizes and component counts.
- If using custom terrain mesh tiles, keep tile IDs, origin, bounds, sample spacing, and LOD derivation deterministic and manifest-backed.

The authority source can be one deterministic heightfield, but runtime does not have to load it as one monolithic render object.

## Preview Export Consequences

Batch 004 preview export should produce artifacts whose names make the 6x authority explicit.

Recommended outputs:

```text
pr5_batch004_x6_world_scale_height_authority_8129.r16
pr5_batch004_x6_world_scale_height_authority_8129.json
pr5_batch004_x6_world_scale_preview_1024.png
pr5_batch004_x6_world_scale_validation_4096.png
pr5_batch004_x6_world_scale_masks_4096.png
pr5_batch004_x6_world_scale_manifest.json
pr5_batch004_x6_world_scale_metrics.json
```

Required manifest fields:

```json
{
  "terrain_source_version": "pr5_batch004_x6_world_scale",
  "terrain_config_version": "pr5.batch004.x6_world_scale.v1",
  "source_seed": "<existing deterministic seed or explicit replacement>",
  "source_fingerprint": "<hash of all scale-relevant config and algorithm versions>",
  "parent_source_version": "pr5_batch003",
  "world_size_m": 97536.0,
  "world_half_extent_m": 48768.0,
  "linear_world_scale": 6.0,
  "area_scale": 36.0,
  "mean_island_radius_m": 42000.0,
  "max_island_radius_m": 44100.0,
  "min_square_edge_ocean_margin_m": 4668.0,
  "sea_level_m": 0.0,
  "authority_resolution_px": 8129,
  "authority_pixel_spacing_m": 12.0,
  "preview_resolution_px": 1024,
  "preview_pixel_spacing_m": 95.34310850439883,
  "validation_preview_resolution_px": 4096,
  "validation_preview_pixel_spacing_m": 23.81831501831502,
  "height_min_m": "<observed>",
  "height_max_m": "<observed>",
  "shoreline_error_max_m": "<observed>",
  "beach_continuity_pct": "<observed>",
  "ocean_below_sea_pct": "<observed>",
  "square_edge_ocean_violations": "<observed>",
  "old_world_size_m": 16256.0,
  "old_mean_island_radius_m": 7000.0,
  "old_max_island_radius_m": 7350.0,
  "scale_proof": {
    "world_size_ratio": 6.0,
    "mean_radius_ratio": 6.0,
    "max_radius_ratio": 6.0,
    "area_ratio": 36.0
  }
}
```

Runtime logs should print the same authority fields on terrain load/generation:

```text
[PR5Terrain] source_version=pr5_batch004_x6_world_scale
[PR5Terrain] world_size_m=97536.0 half_extent_m=48768.0 linear_scale=6.0 area_scale=36.0
[PR5Terrain] mean_island_radius_m=42000.0 max_island_radius_m=44100.0 edge_ocean_margin_m=4668.0
[PR5Terrain] authority_resolution_px=8129 authority_spacing_m=12.0
[PR5Terrain] height_min_m=<observed> height_max_m=<observed> sea_level_m=0.0
[PR5Terrain] coast_metrics shoreline_error_max_m=<observed> beach_continuity_pct=<observed> ocean_below_sea_pct=<observed> square_edge_ocean_violations=<observed>
[PR5Terrain] source_fingerprint=<hash>
```

## Determinism And Config Identity

Batch 004 must produce a new deterministic identity. Do not silently reuse the Batch 003 fingerprint after changing scale.

Required identity changes:

```text
terrain_source_version: pr5_batch004_x6_world_scale
terrain_config_version: pr5.batch004.x6_world_scale.v1
parent_source_version: pr5_batch003
source_fingerprint: hash(config + algorithm versions + scale rules + source resolution + seed)
```

The fingerprint must include:

- world size;
- half extent;
- linear scale;
- area scale;
- mean island radius;
- max island radius;
- coast noise/radius parameters;
- beach/shelf band widths;
- vertical envelope;
- feature scale class table or equivalent config;
- source resolution;
- source pixel spacing;
- seed;
- generator algorithm version;
- exporter/manifest schema version.

Determinism rule:

- Same Batch 004 config + same seed -> same heightfield and masks.
- Any scale-relevant constant change -> new fingerprint.
- Any coastline function change -> new fingerprint.
- Any source resolution authority change -> new fingerprint.
- Preview resolution changes may have their own preview fingerprint, but must not alter source fingerprint unless source generation changes.

## Developer-Only Validation Metrics

Validation must prove two things:

1. The map is truly 6x larger in linear world space.
2. The accepted coast constraints remain intact.

Required scale metrics:

```text
world_size_m == 97536.0
world_half_extent_m == 48768.0
linear_world_scale == 6.0
area_scale == 36.0

mean_island_radius_m ~= 42000.0
max_island_radius_m ~= 44100.0
min_square_edge_ocean_margin_m >= 4668.0 - tolerance

old_to_new_world_size_ratio == 6.0
old_to_new_mean_radius_ratio == 6.0
old_to_new_max_radius_ratio == 6.0
old_to_new_area_ratio == 36.0
```

Required source/preview metrics:

```text
authority_resolution_px == 8129
authority_pixel_spacing_m == 12.0

preview_resolution_px == 1024
preview_pixel_spacing_m == 95.34310850439883

validation_preview_resolution_px == 4096
validation_preview_pixel_spacing_m == 23.81831501831502
```

Required coast metrics:

```text
shoreline_error_max_m <= 0.01
beach_continuity_pct == 100.0
ocean_below_sea_pct == 100.0
square_edge_ocean_violations == 0
edge_rows_below_sea_pct == 100.0
edge_columns_below_sea_pct == 100.0
outer_ocean_container_connected == true
```

Required vertical/morphology metrics:

```text
height_max_m >= 3200.0
height_max_m <= 4300.0
height_min_m <= -600.0    // if adopting deeper ocean/container target
summit_to_mean_radius_ratio = height_max_m / mean_island_radius_m
major_ridge_count >= 3
major_catchment_count >= 4
interior_basin_count >= 2
crater_or_caldera_detected == true
slope_distribution_has_coast_lowland_midflank_highland_bins == true
```

Required anti-preview-zoom metrics:

```text
scale_proof_world_extent_m == 97536.0
scale_proof_named_control_distance_ratio == 6.0
scale_proof_sample_spacing_authority_m == 12.0
scale_proof_source_resolution_changed_from_preview == true
scale_proof_not_resampled_old_heightmap == true
```

Recommended named control distances:

- center to north coast;
- center to south coast;
- center to east coast;
- center to west coast;
- center to highest summit;
- summit to main crater rim;
- summit to two longest radial ridge endpoints;
- mouth-to-head length for four major catchments.

The validation should compare Batch 003 and Batch 004 control distances where possible and emit a ratio table.

## Files And Areas Likely Touched

Exact filenames depend on the repository, but the PR likely touches these areas:

| Area | Likely Change |
|---|---|
| Canonical terrain config/constants | Replace 16.256 km authority with 97.536 km constants and 6x scale metadata |
| Terrain source/generator | Redesign macro terrain field, feature scaling, vertical relief, crater/ridge/basin/gully morphology |
| Coastline module | Preserve shore lock, beach continuity, shelf, and square-edge ocean invariants at new scale |
| Heightfield source/exporter | Add 8129 authority export and 4096 validation preview |
| Preview renderer/exporter | Add explicit x6 naming and manifest fields |
| Metrics/validation code | Add scale-proof metrics, source resolution proof, and coast regression metrics |
| Runtime terrain loader/logging | Emit terrain source version, world size, radii, resolution, spacing, fingerprint |
| Tests / golden metrics | Update expected world/radius values and preserve coast expectations |
| Documentation / research specs | Add Batch 004 authority spec and forbidden approaches |

Likely class/module patterns:

```text
*TerrainConfig*
*TerrainSource*
*Heightfield*
*TerrainGenerator*
*Coast*
*Shore*
*Beach*
*Shelf*
*PreviewExporter*
*TerrainManifest*
*TerrainMetrics*
*WorldPartition*
*Landscape*
```

Do not modify unrelated gameplay, UI, NPC, foliage, audio, or content systems in this PR.

## Large PR Implementation Scope

This should be treated as a large but scoped implementation PR.

Recommended implementation phases inside one PR:

1. **Config authority**
   - Add Batch 004 x6 constants.
   - Add config version/fingerprint schema.
   - Keep Batch 003 values visible only as parent/baseline references.

2. **Source grid authority**
   - Add 8129 authority resolution.
   - Add exact 12 m/sample spacing.
   - Ensure generator math uses meter-space and `double` for world distances.

3. **Horizontal morphology**
   - Scale island footprint and macro feature positions.
   - Add or retune larger interior feature hierarchy.
   - Avoid stretching old noise without new detail.

4. **Vertical morphology**
   - Target 3600-4200 m summit, default 3800 m.
   - Retune slope/basin/ridge/gully/crater relief.
   - Do not linearly 6x vertical relief.

5. **Coast preservation**
   - Reapply shore lock after macro terrain.
   - Preserve beach/shelf/ocean/container constraints.
   - Preserve edge ocean tests.

6. **Preview/export**
   - Generate 1024 quick preview, 4096 validation preview, 8129 authority export.
   - Add manifest and metrics JSON.

7. **Validation/tests**
   - Assert scale proof.
   - Assert coast metrics.
   - Assert vertical/morphology ranges.
   - Assert deterministic fingerprint behavior.

8. **Runtime logging**
   - Print terrain authority on load/generation.
   - Ensure logs make a 16.256 km accidental fallback obvious.

## Acceptance Criteria

A PR should be accepted only if all of the following are true.

### Scale authority

```text
world_size_m == 97536.0
world_half_extent_m == 48768.0
linear_world_scale == 6.0
area_scale == 36.0
mean_island_radius_m == approximately 42000.0
max_island_radius_m == approximately 44100.0
min_square_edge_ocean_margin_m == approximately 4668.0 or greater
```

### Source authority

```text
authority_resolution_px == 8129
authority_pixel_spacing_m == 12.0
terrain_source_version == pr5_batch004_x6_world_scale
terrain_config_version starts with pr5.batch004.x6_world_scale
source_fingerprint changes from Batch 003
```

### Coast preservation

```text
shoreline_error_max_m <= 0.01
beach_continuity_pct == 100.0
ocean_below_sea_pct == 100.0
square_edge_ocean_violations == 0
outer world edge remains ocean/container terrain
island perimeter remains beach/coastal shelf/sea-level transition
```

### Vertical and morphology

```text
height_max_m >= 3200.0
height_max_m <= 4300.0
height_min_m <= -600.0 if deeper ocean target is adopted
major radial ridges visible in validation preview
major catchments visible in validation preview
central crater/caldera visible in validation preview
coast, lower flank, mid-flank, highland, and summit zones distinguishable by slope/elevation metrics
```

### Export evidence

Required files exist or equivalent configured outputs are produced:

```text
pr5_batch004_x6_world_scale_height_authority_8129.r16
pr5_batch004_x6_world_scale_height_authority_8129.json
pr5_batch004_x6_world_scale_preview_1024.png
pr5_batch004_x6_world_scale_validation_4096.png
pr5_batch004_x6_world_scale_masks_4096.png
pr5_batch004_x6_world_scale_manifest.json
pr5_batch004_x6_world_scale_metrics.json
```

### Runtime proof

Logs clearly emit:

```text
source_version
source_fingerprint
world_size_m
half_extent_m
linear_scale
area_scale
mean_island_radius_m
max_island_radius_m
edge_ocean_margin_m
authority_resolution_px
authority_spacing_m
height_min_m
height_max_m
coast metrics
```

## Risks And Stop Conditions

The PR should stop or be rejected if any of these occur:

1. **Coast regression**
   - shoreline error is no longer zero/effectively zero;
   - beach continuity drops below 100%;
   - ocean below sea drops below 100%;
   - square edge ocean has any violations.

2. **Preview zoom masquerading as source scale**
   - only preview camera/export scale changes;
   - world constants remain 16.256 km;
   - island radii do not become ~42 km / ~44.1 km;
   - manifest lacks scale proof.

3. **Authority resolution ambiguity**
   - 1024 preview becomes the only source authority;
   - source resolution/pixel spacing are omitted;
   - source spacing remains old-scale without explaining the change.

4. **Vertical relief failure**
   - height remains ~1750 m with no added interior morphology;
   - height scales to ~10.5 km;
   - summit/relief metrics fall outside acceptance range without a documented reason.

5. **UE runtime infeasibility**
   - full-resolution terrain is assumed to be always loaded as one monolithic mesh;
   - World Partition or equivalent tile/LOD strategy is absent;
   - component counts exceed known safe/recommended Landscape assumptions without an explicit reason;
   - large coordinate math is downgraded to float in authority paths.

6. **Determinism failure**
   - same config/seed does not reproduce outputs;
   - fingerprint does not change after scale constants change;
   - fingerprint omits important terrain constants.

7. **Scope creep**
   - PR adds foliage, ecology, POIs, UI, NPCs, VFX, buildings, survival gameplay, or material finalization.

8. **Metrics are not generated from final terrain**
   - validation runs before final coastline lock;
   - metrics use intermediate masks rather than final heightfield;
   - preview looks correct but JSON evidence fails or is absent.

## Rejected Or Forbidden Approaches

Do not use these approaches:

- Do not merely scale the preview image.
- Do not merely change camera zoom.
- Do not merely set UE actor scale while leaving source authority at 16.256 km.
- Do not resample the old 1024 heightmap and claim it is a new 97.536 km source.
- Do not scale every feature blindly by 6x.
- Do not scale vertical relief blindly by 6x.
- Do not keep the current vertical/morphology unchanged without compensating terrain hierarchy.
- Do not widen beach bands 6x by default.
- Do not relax coast metrics to hide regressions.
- Do not remove square-edge ocean/container terrain.
- Do not introduce player-facing UI systems.
- Do not add foliage/ecology/content systems.
- Do not ship without a new source/config fingerprint.
- Do not ship without manifest fields proving scale, resolution, radii, and coast metrics.
- Do not make 1024 px preview evidence the sole validation artifact.

## Sources And References

- Uploaded task brief: `001_x6_world_scale_terrain_authority_prompt.md`.
- Epic Games, **Landscape Technical Guide in Unreal Engine**. Used for valid/recommended Landscape dimensions, component/section architecture, 8129 x 8129 recommendation, component performance warnings, and heightmap Z-scale context.
  <https://dev.epicgames.com/documentation/unreal-engine/landscape-technical-guide-in-unreal-engine>
- Epic Games, **World Partition in Unreal Engine**. Used for large-world streaming/data-management recommendations and runtime grid/cell framing.
  <https://dev.epicgames.com/documentation/unreal-engine/world-partition-in-unreal-engine?lang=en-US>
- Epic Games, **Large World Coordinates in Unreal Engine 5**. Used for UE5 double-precision / massive-world coordinate guidance and warnings around precision loss in float-exposed paths.
  <https://dev.epicgames.com/documentation/unreal-engine/large-world-coordinates-in-unreal-engine-5>
- National Park Service, **Shield Volcanoes**. Used for broad volcanic-island shape, gentle shield slopes, lava-flow distance, and flank/rift-zone morphology guidance.
  <https://www.nps.gov/articles/000/shield-volcanoes.htm>
- British Geological Survey, **Types of Volcano**. Used for high-level shield-vs-stratovolcano slope distinction and the guidance that shield volcanoes commonly have gentle slopes from fluid basaltic lava.
  <https://www.bgs.ac.uk/discovering-geology/earth-hazards/volcanoes/how-volcanoes-form/>
- USGS, **Coastal Change**. Used for coast-validation framing: waves, surge, currents, sediment movement, morphology, and coastal-change risk are complex enough to justify explicit metrics.
  <https://www.usgs.gov/centers/whcmsc/science/coastal-change>
- USGS, **Shoreface morphology / sediment-reservoir note**. Used to justify treating shoreline, beach, shoreface, shelf, and sediment transition as a specific coast system rather than arbitrary procedural noise.
  <https://www.usgs.gov/centers/spcmsc/news/drs-emily-wei-and-jennifer-miselis-published-manuscript-describing-controls>
- USGS EROS, **SRTM 1 Arc-Second Global DEM**. Used as an external reference point that ~30 m-class DEM data is a meaningful global terrain-analysis resolution, making 12 m authority and 23.8 m validation previews reasonable for a 97.536 km synthetic terrain source.
  <https://www.usgs.gov/centers/eros/science/usgs-eros-archive-digital-elevation-shuttle-radar-topography-mission-srtm-1>
