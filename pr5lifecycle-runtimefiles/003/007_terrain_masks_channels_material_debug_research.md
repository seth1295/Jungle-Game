# Terrain Masks, Channels, And Debug Material Research

## Executive Summary

Jungle Game’s PR5 Batch 003 terrain work should introduce a **source-owned terrain channel contract**: every generated landform mask is produced by the terrain generator, stored with deterministic metadata, validated by automated checks, and visualized only through developer/debug tooling. The important architectural decision is that the coastline is not a cosmetic layer. It is a hard terrain invariant driven by shared sea-level, signed-island-distance, island, ocean, beach-ring, and coastal-shelf channels.

The implementation should split terrain outputs into two groups:

1. **Continuous scalar fields** used for generation, blending, thresholds, and diagnostics. Examples: `height_m`, `signed_island_distance_m`, `island_weight`, `ocean_depth_m`, `beach_weight`, `shelf_weight`, `massif_weight`, `ridge_strength`, `gully_strength`, `catchment_area_norm`, `lava_heat_norm`, `crust_integrity`, `slope_degrees`, and `relief_norm`.
2. **Discrete class masks** used for validation, debug views, future downstream system queries, and blocker rules. Examples: `terrain_domain_class`, `slope_class`, `elevation_band_class`, `relief_class`, `catchment_id`, `crater_class`, `lava_crust_class`, and `hard_blocker_class`.

The terrain generator must export a deterministic **channel atlas manifest** next to any generated debug/validation images. The manifest should record the seed, config hash, world scale, sea-level value, island radius fields, channel names, quantization rules, and generator version. The debug material should be deliberately non-player-facing: high-saturation false-color, editor-only naming, build exclusion or development-only cook rules, and log tokens that prove which terrain config produced the screenshot.

The next implementation stage is acceptable when automated validation proves that the island edge, shoreline, ocean floor, beach ring, shelf, slope classes, ridges, gullies, catchments, crater, lava/crust, unsafe crust, and hard blockers are all internally consistent and reproducible.

## Terrain-Only Scope Boundary

This document is **terrain generation and terrain diagnostics only**.

Allowed:

- Terrain heightfield generation.
- Terrain mask/channel generation.
- Developer-only false-color materials and overlay modes.
- Offline or editor validation maps.
- Metadata, logs, manifests, and source-control rules for generated terrain artifacts.
- Data contracts that later systems can consume.

Forbidden in this batch:

- Foliage, ecology, plants, animals, weather, audio, survival, NPCs, story, final VFX, final art materials, final player-facing water, or environment art direction.
- Player UI, including minimap, map, compass, GPS, objective markers, bearing indicators, time/orientation widgets, or terrain-debug overlays exposed to players.
- Runtime gameplay mechanics based on unsafe crust or blockers. This batch may expose data; it must not design gameplay around that data.
- Hand-painted final art layers that become the source of truth. Terrain channels must be generated and owned by the generator.

## Current Terrain Baseline

No repository inspection is assumed. The baseline inferred from the PR5 Batch 003 prompt is:

- Jungle Game is a **UE5 Linux-first first-person project**.
- PR5 Batch 003 is about a **major terrain-generation batch** for a realistic active volcanic island.
- Current terrain work needs explicit channels for island/ocean/coast/landforms so later systems are not forced to infer semantics from height alone.
- Developer diagnostics are required to prove the terrain generator, not merely to make the terrain look good.

Relevant engine constraints:

- UE Landscape height data has architectural limits around components, sections, valid heightmap sizes, and heightmap import ranges. Epic’s Landscape Technical Guide states that Landscape components are the base unit for rendering, visibility, and collision, and that component/section sizes constrain valid heightmap dimensions. It also documents UE Landscape height scaling and recommended sizes such as 4033×4033 and 2017×2017 vertices. [Epic Landscape Technical Guide][src-epic-landscape-technical]
- UE Landscape layer materials can blend per-layer values through `LandscapeLayerBlend`; Epic documents weight, alpha, and height blend modes and notes that layer names should be unique and descriptive. [Epic Landscape Materials][src-epic-landscape-materials]
- UE provides a Landscape Layer Debug visualizer that can show selected target-layer channels in the viewport; PR5 can mirror that idea for generator-owned channel visualization without turning it into player UI. [Epic Landscape Technical Guide][src-epic-landscape-technical]

## Major Implementation Goal

Implement a terrain-channel system where every generated terrain cell has a coherent set of channel values with the following guarantees:

1. **Sea-level correctness**: ocean outside the island boundary is below sea level; the beach ring follows the 0 m contour; the coastal shelf surrounds the beach ring; edge terrain cannot be accidentally raised by unrelated landform channels.
2. **Landform explainability**: massif, ridges, gullies, catchments, crater, lava/crust, unsafe crust, blockers, and slope classes are inspectable as individual masks.
3. **Downstream stability**: future systems can consume named channels and classes instead of reverse-engineering terrain meaning from height, slope, or material color.
4. **Deterministic validation**: the same config identity always produces the same channel fields, manifests, log tokens, and validation screenshots/maps.
5. **Debug-only visualization**: materials and overlays exist to validate generation; they are not final art and not player-facing HUD/UI.

Recommended implementation model:

```text
TerrainConfig
  -> TerrainHeightPass
  -> TerrainDomainMaskPass      island/ocean/beach/shelf/lowland/massif
  -> TerrainGeomorphologyPass   slope/relief/ridge/gully/catchment
  -> TerrainVolcanicPass        crater/lava/crust/unsafe crust/blockers
  -> TerrainChannelAtlas        named channels + metadata
  -> TerrainValidationReport    machine checks + screenshots/maps
  -> DevOnlyDebugMaterials      false-color material modes
```

## Required Terrain Channels

The terrain generator should emit a minimum channel set. Names below are intentionally explicit and should remain stable once implemented.

### Core coordinate and elevation channels

| Channel | Type | Range / units | Ownership | Purpose |
|---|---:|---|---|---|
| `height_m` | continuous scalar | meters relative to sea level; sea level = `0.0` | generator | Final generated terrain elevation. |
| `base_height_m` | continuous scalar | meters | generator | Pre-landform base island profile before ridges/gullies/lava perturbations. |
| `sea_level_m` | constant scalar | normally `0.0` | config | Datum used by every coastline and ocean assertion. |
| `normalized_radius` | continuous scalar | `0..sqrt(2)` or project-defined | generator | Center-relative radius normalized by nominal island radius. |
| `signed_island_distance_m` | continuous scalar | negative inside island, zero at island edge, positive outside | generator | Main coast invariant field. |
| `edge_falloff_weight` | continuous scalar | `0..1` | generator | Prevents non-coast landforms from raising outer edge. |

### Domain/coast channels

| Channel | Type | Range / classes | Purpose |
|---|---:|---|---|
| `island_weight` | continuous scalar | `0..1` | Organic island mask; 1 inland, 0 ocean exterior, smooth transition near coast. |
| `ocean_weight` | continuous scalar | `0..1` | Complement/derived mask for outside-island ocean domain. |
| `ocean_depth_m` | continuous scalar | `>=0` below sea level | Positive depth value outside island and on underwater shelf. |
| `shoreline_weight` | continuous scalar | `0..1`, peak at `height_m == 0` | Thin band centered on sea-level contour. |
| `beach_weight` | continuous scalar | `0..1` | Ring around 0 m contour, constrained by low slope and sea-level proximity. |
| `coastal_shelf_weight` | continuous scalar | `0..1` | Underwater/nearshore shelf around the beach ring. |
| `rocky_coast_weight` | continuous scalar | `0..1` | Explicit exception path for cliff/rock coast variants. Must still intersect sea level. |
| `lowland_weight` | continuous scalar | `0..1` | Inland low terrain above beach/shelf. |
| `terrain_domain_class` | discrete class | enum | Debug and validation domain class. |

Recommended `terrain_domain_class` enum:

```cpp
enum class ETerrainDomainClass : uint8
{
    OceanExterior = 0,
    CoastalShelf = 1,
    BeachRing = 2,
    RockyCoast = 3,
    Lowland = 4,
    Massif = 5,
    Crater = 6,
    LavaField = 7,
    HardBlocked = 8
};
```

### Volcanic and geomorphology channels

| Channel | Type | Range / classes | Purpose |
|---|---:|---|---|
| `massif_weight` | continuous scalar | `0..1` | Main volcanic mass / high island body. |
| `ridge_strength` | continuous scalar | `0..1` | Ridges/spines from massif or crater morphology. |
| `ridge_axis_id` | discrete id | `0 = none`, positive id | Stable region id for ridge axes. |
| `gully_strength` | continuous scalar | `0..1` | Incised drainage/gully features. |
| `gully_axis_id` | discrete id | `0 = none`, positive id | Stable region id for gully axes. |
| `flow_dir_rad` | continuous scalar | radians, `0..2π`, nodata outside island | Downslope flow direction for catchment derivation. |
| `flow_accum_cells` | continuous/integer scalar | `>=1`, log-scaled for debug | Contributing-cell count or weighted accumulation. |
| `catchment_area_norm` | continuous scalar | `0..1`, log normalized | Catchment magnitude, used for gullies and validation. |
| `catchment_id` | discrete id | `0 = none/ocean`, positive id | Drainage basin identity. |
| `crater_weight` | continuous scalar | `0..1` | Overall crater influence. |
| `crater_rim_weight` | continuous scalar | `0..1` | Rim uplift band. |
| `crater_floor_weight` | continuous scalar | `0..1` | Interior depression/floor. |
| `crater_breach_weight` | continuous scalar | `0..1` | Low rim breach/overflow direction. |
| `lava_flow_weight` | continuous scalar | `0..1` | Flow emplacement footprint. |
| `lava_channel_weight` | continuous scalar | `0..1` | Active or recent flow-path centerlines. |
| `crust_age_norm` | continuous scalar | `0..1`; 0 newest, 1 oldest | Relative lava-crust maturity. |
| `crust_integrity` | continuous scalar | `0..1`; 0 unsafe, 1 stable | Terrain-only structural diagnostic. |
| `unsafe_crust_weight` | continuous scalar | `0..1` | Thin/unstable crust areas for debug/data only. |
| `hard_blocker_weight` | continuous scalar | `0..1` | Areas that must be treated as terrain blockers. |
| `hard_blocker_class` | discrete class | enum | Blocker reason; not gameplay design. |

Recommended `hard_blocker_class` enum:

```cpp
enum class ETerrainHardBlockerClass : uint8
{
    None = 0,
    OceanDeep = 1,
    CliffTooSteep = 2,
    CraterVent = 3,
    UnsafeCrust = 4,
    LavaOpenOrThinCrust = 5,
    GeneratorBoundary = 6
};
```

### Slope/elevation/relief channels

| Channel | Type | Range / classes | Purpose |
|---|---:|---|---|
| `slope_degrees` | continuous scalar | `0..90` | Derived from final `height_m`. |
| `slope_percent` | continuous scalar | `>=0` | Optional derived value for terrain-analysis comparison. |
| `slope_class` | discrete class | enum | Stable class map for debug/validation. |
| `aspect_rad` | continuous scalar | `0..2π`, nodata on flats | Optional; validates flow/ridge orientation. |
| `curvature_profile` | continuous scalar | project-normalized | Convex/concave along slope; helps ridge/gully detection. |
| `curvature_plan` | continuous scalar | project-normalized | Flow convergence/divergence; helps catchments. |
| `local_relief_m` | continuous scalar | meters over configured kernel | Local elevation range. |
| `relief_norm` | continuous scalar | `0..1` | Normalized local terrain energy. |
| `elevation_band_class` | discrete class | enum | Below sea / beach / lowland / upland / summit. |
| `relief_class` | discrete class | enum | Flat / rolling / broken / steep / extreme. |

GRASS GIS documents `r.slope.aspect` as producing slope, aspect, curvatures, and derivative rasters from true elevation data, while `r.watershed` produces flow accumulation, drainage direction, streams, and watershed basins. These are useful external references for the kinds of derived terrain channels PR5 should compute or emulate inside its own generator pipeline. [GRASS r.slope.aspect][src-grass-slope-aspect] [GRASS r.watershed][src-grass-watershed]

## Sea-Level / Island / Beach / Ocean Masks

### Required representation

The coastline should be controlled by a **signed distance field**, not by ad hoc thresholding of final height alone.

Recommended fields:

```text
sea_level_m = 0.0
signed_island_distance_m < 0  inside island domain
signed_island_distance_m = 0  intended island boundary / coast control curve
signed_island_distance_m > 0  exterior ocean domain
island_weight = saturate(smoothstep(+coast_feather_m, -coast_feather_m, signed_island_distance_m))
ocean_weight = 1 - island_weight, with optional hardened exterior threshold
```

The generator can still derive organic island shape from radial noise, volcanic asymmetry, erosion, and crater placement. The key is that those details feed the **signed island distance** first, then landform passes respect that distance.

### Sea-level invariant

The generator should enforce this after every additive terrain pass:

```text
if signed_island_distance_m >= outer_edge_guard_m:
    height_m <= sea_level_m - min_ocean_depth_m
```

Recommended defaults:

| Parameter | Suggested initial value | Purpose |
|---|---:|---|
| `sea_level_m` | `0.0` | Global vertical datum. |
| `coast_feather_m` | `40..120 m` | Smooth organic transition around coast mask. |
| `beach_half_width_m` | `15..45 m` | Target beach ring half-width around 0 m contour. |
| `shelf_width_m` | `120..350 m` | Nearshore shelf width. |
| `min_ocean_depth_m` | `2..8 m` | Minimum exterior ocean depth outside guard. |
| `outer_edge_guard_m` | `20..80 m` outside island | Prevents edge pop-up. |
| `max_beach_slope_deg` | `3..8°` | Beach ring should be low slope unless rocky coast variant. |
| `rocky_coast_min_slope_deg` | `20..35°` | Explicit cliff/rock exception. |

### Beach ring calculation

Beach should not be a painted stripe. It should be a derived continuous field:

```text
height_proximity = 1 - saturate(abs(height_m - sea_level_m) / beach_vertical_tolerance_m)
slope_allowance = 1 - saturate((slope_degrees - max_beach_slope_deg) / beach_slope_fade_deg)
coast_proximity = 1 - saturate(abs(signed_island_distance_m) / beach_horizontal_tolerance_m)
beach_weight = height_proximity * slope_allowance * coast_proximity * island_or_shoreline_gate
```

Recommended tolerances:

- `beach_vertical_tolerance_m`: `0.5..2.0 m`.
- `beach_horizontal_tolerance_m`: usually equal to `beach_half_width_m`.
- `beach_slope_fade_deg`: `4..8°`.

Acceptance consequence: if there is no continuous `beach_weight` ring around the 0 m contour, the coastline pass fails even if the final terrain looks plausible.

### Coastal shelf calculation

The shelf is not the same as beach. It is a nearshore underwater/low-elevation belt that gives the island a believable transition into ocean.

Recommended calculation:

```text
shelf_distance_band = smooth_band(signed_island_distance_m, -inner_shelf_m, +outer_shelf_m)
underwater_gate = saturate((sea_level_m + shelf_above_tolerance_m - height_m) / shelf_depth_fade_m)
coastal_shelf_weight = shelf_distance_band * underwater_gate
```

Rules:

- Shelf may extend slightly inland only where `height_m` is near sea level.
- Exterior shelf must remain below sea level except in beach/rocky-coast transition pixels.
- Deep ocean should be a separate falloff beyond the shelf: `ocean_depth_m` increases outward.
- Shelf debug must show a continuous belt, not isolated speckles.

### Ocean mask calculation

`ocean_weight` should be derived from `signed_island_distance_m` and checked against height:

```text
ocean_weight = saturate(smoothstep(-coast_feather_m, +coast_feather_m, signed_island_distance_m))
ocean_exterior_mask = signed_island_distance_m > outer_edge_guard_m
```

Hard rule:

```text
For all ocean_exterior_mask samples:
    height_m < sea_level_m
    ocean_depth_m = sea_level_m - height_m
```

Validation should report:

- Count of exterior-ocean samples with `height_m >= sea_level_m`.
- Maximum exterior-ocean height.
- Minimum ocean depth outside guard.
- Largest connected invalid exterior-ocean island, if any.

### Rocky/cliff coast exception

A rocky coast is allowed only through an explicit variant channel:

```text
rocky_coast_weight > threshold
terrain_domain_class == RockyCoast
```

Even then:

- The coast must intersect sea level.
- Exterior ocean still remains below sea level.
- The cliff cannot create a raised wall around the entire circular island edge unless the config explicitly asks for a predominantly rocky island.
- Rocky coast must be reported separately in validation coverage percentages.

## Massif / Ridge / Gully / Catchment Masks

### Massif

The massif is the main volcanic bulk. It should be generated as a broad continuous field before smaller ridge/gully detail.

Recommended fields:

```text
massif_weight:        0..1 broad volcano body
massif_height_delta_m additive or target elevation contribution
massif_axis_angle_rad optional dominant asymmetry direction
massif_core_id        optional id if multiple high masses are supported later
```

Rules:

- `massif_weight` must be gated by `island_weight` and `edge_falloff_weight`.
- Massif contribution must be near zero in the beach/shelf/ocean zone unless rocky coast explicitly permits steep transition.
- Massif center should be able to offset from world center to avoid artificial symmetry.

Example contribution rule:

```text
height_m += massif_height_delta_m * massif_weight * edge_falloff_weight
```

### Ridges

Ridges should be represented as both centerline identity and continuous influence.

Recommended fields:

```text
ridge_strength     0..1, antialiased ridge influence
ridge_axis_id      0 none, positive stable id
ridge_height_delta_m signed contribution, usually positive
ridge_distance_m   distance to nearest ridge axis
```

Rules:

- Ridges must not override coastline invariants.
- Ridge axes should usually descend from crater/massif toward lower flanks.
- Ridge debug should distinguish axis ids from strength.
- Validation should catch disconnected, single-pixel, or ocean-crossing ridges.

Suggested generation constraints:

- Ridge start points: crater rim / upper massif.
- Ridge end points: lowland or coastal zone, not exterior ocean.
- Ridge width: `20..120 m`, widening downslope only if configured.
- Ridge relief: strongest in mid/upland zones, faded near beach.

### Gullies

Gullies should be linked to drainage/catchment fields instead of pure noise scratches.

Recommended fields:

```text
gully_strength        0..1 incision influence
gully_axis_id         0 none, positive stable id
gully_depth_delta_m   negative height contribution
gully_distance_m      distance to nearest gully axis
```

Rules:

- Gullies should align with flow accumulation and local slope.
- Gullies should reduce height locally, not raise it.
- Gully strength should fade in beach/shelf/ocean domains unless a drainage outlet reaches shoreline.
- Validation should flag gullies that climb uphill for long runs or terminate inland without a basin/lowland reason.

### Catchments

Catchments are key for proving gullies and drainage are coherent. Flow accumulation and basin id fields should be derived from the final or near-final elevation field after base landforms but before minor visual-only noise.

Recommended fields:

```text
flow_dir_rad           0..2π, nodata where undefined
flow_accum_cells       number of contributing cells, >=1
flow_accum_log_norm    log-normalized 0..1 debug field
catchment_area_norm    0..1 normalized catchment size
catchment_id           discrete basin id
stream_candidate       0..1 derived from accumulation + slope threshold
```

Flow-accumulation references generally define accumulation as the amount of upslope area/cells draining through a cell; GRASS `r.watershed` and related hydrology tooling produce drainage direction, flow accumulation, stream, and basin maps from elevation rasters. [GRASS r.watershed][src-grass-watershed] D-infinity methods distribute flow across triangular facets and are useful references when single-direction flow creates artifacts in low-relief terrain. [TauDEM D-Infinity][src-taudem-dinf]

Implementation guidance:

- Start with deterministic D8 or MFD-style flow for PR5; prefer simplicity and validation over hydrology perfection.
- Store `flow_accum_cells` as high precision or 32-bit integer/float; debug views should use log normalization.
- `catchment_id` is discrete and should be stable for the same seed/config.
- Catchment validation should ignore exterior ocean and classify drainage outlets at shoreline separately from invalid sinks.

## Crater / Lava-Crust / Hard-Blocker Masks

### Crater

The crater should be represented by several fields, not a single mask.

Recommended fields:

| Channel | Type | Range | Purpose |
|---|---:|---|---|
| `crater_weight` | continuous | `0..1` | Overall crater influence. |
| `crater_rim_weight` | continuous | `0..1` | Raised rim ring. |
| `crater_floor_weight` | continuous | `0..1` | Interior floor/depression. |
| `crater_wall_weight` | continuous | `0..1` | Steep inner/outer wall band. |
| `crater_breach_weight` | continuous | `0..1` | Lower rim breach / drainage or lava outlet. |
| `crater_class` | discrete | enum | Dominant crater zone. |

Recommended enum:

```cpp
enum class ETerrainCraterClass : uint8
{
    None = 0,
    OuterApron = 1,
    Rim = 2,
    InnerWall = 3,
    Floor = 4,
    Vent = 5,
    Breach = 6
};
```

Rules:

- Rim/floor/wall weights may overlap smoothly, but `crater_class` must choose a dominant class for debug and validation.
- Crater rim must not create impossible spikes at the coastline due to broad noise or island-edge blending.
- Crater breach must be explicit if lava/gully paths cross the rim.
- Crater floor should have sensible slope/relief stats distinct from rim/wall.

### Lava and crust

USGS explains that magma may erupt as fluid lava flows and that volcanic material can move downslope under gravity; the USGS glossary also describes features such as tumuli forming when slow-moving molten lava pushes overlying crust upward. These references support representing lava flow, crust age/integrity, and flow/channel features as terrain fields rather than purely final art layers. [USGS About Volcanoes][src-usgs-about-volcanoes] [USGS Volcano Glossary][src-usgs-volcano-glossary]

Recommended lava/crust fields:

| Channel | Type | Range / classes | Purpose |
|---|---:|---|---|
| `lava_flow_weight` | continuous | `0..1` | Flow footprint. |
| `lava_channel_weight` | continuous | `0..1` | Centerline/channelized flow. |
| `lava_levee_weight` | continuous | `0..1` | Raised edges, if implemented. |
| `lava_crust_weight` | continuous | `0..1` | Crusted-over flow surface. |
| `crust_age_norm` | continuous | `0..1` | Relative age/maturity. |
| `crust_integrity` | continuous | `0..1` | Structural diagnostic field. |
| `unsafe_crust_weight` | continuous | `0..1` | Thin/unstable crust diagnostic. |
| `lava_crust_class` | discrete | enum | Dominant lava/crust class. |

Recommended enum:

```cpp
enum class ETerrainLavaCrustClass : uint8
{
    None = 0,
    OldCrustedFlow = 1,
    RecentCrustedFlow = 2,
    ThinUnsafeCrust = 3,
    OpenVentOrLava = 4,
    Levee = 5,
    Channel = 6
};
```

Rules:

- Lava channels should obey downslope or configured vent-to-lowland routing unless deliberately ponded in crater floor.
- `unsafe_crust_weight` must not be inferred only from color/material; it must be generated data.
- `crust_integrity` is a terrain diagnostic, not a survival/gameplay rule in this batch.
- Lava/crust fields must not raise exterior ocean or break beach/shelf invariants.

### Hard blockers

Hard blockers should be generated and validated as **terrain constraints**, not gameplay design.

Recommended fields:

```text
hard_blocker_weight 0..1
hard_blocker_class  enum reason
```

Initial blocker reasons:

- `OceanDeep`: exterior/deep ocean domain.
- `CliffTooSteep`: slope above configured maximum.
- `CraterVent`: vent/open crater center.
- `UnsafeCrust`: terrain-only unstable crust field above threshold.
- `LavaOpenOrThinCrust`: open/recent lava diagnostic.
- `GeneratorBoundary`: world edge guard or invalid exterior region.

Validation should report hard blocker coverage by class and ensure classes do not appear where their input cause is absent.

## Slope / Elevation / Relief Classes

### Continuous terrain attributes

Slope, aspect, curvature, relief, and catchment metrics are derived attributes. They should not be authored independently from the heightfield.

Recommended continuous outputs:

```text
slope_degrees        0..90
slope_percent        tan(slope) * 100, optional
aspect_rad           0..2π, nodata on flats
curvature_profile    signed normalized
curvature_plan       signed normalized
local_relief_m       max(height) - min(height) over kernel
relief_norm          0..1 normalized local relief
```

GRASS `r.slope.aspect` is a useful reference because it derives slope, aspect, curvature, and derivative rasters from true elevation values and supports slope output in degrees or percent. [GRASS r.slope.aspect][src-grass-slope-aspect]

### Slope classes

The project should define slope classes in degrees for engine/debug consistency. Percent slope can be logged for external terrain-analysis comparison. The FAO land evaluation framework describes land classification as grouping areas by suitability for a defined use; PR5 should borrow the idea of explicit classes while using project-specific thresholds for terrain validation rather than agriculture or land-use suitability. [FAO Land Suitability][src-fao-land-suitability]

Recommended `slope_class`:

```cpp
enum class ETerrainSlopeClass : uint8
{
    Flat_0_2 = 0,        // 0 <= slope < 2°
    Gentle_2_5 = 1,     // 2 <= slope < 5°
    Rolling_5_15 = 2,   // 5 <= slope < 15°
    Steep_15_30 = 3,    // 15 <= slope < 30°
    Cliff_30_45 = 4,    // 30 <= slope < 45°
    Extreme_45Plus = 5  // slope >= 45°
};
```

Coast-specific assertions:

- `BeachRing` should mostly be `Flat_0_2`, `Gentle_2_5`, or low `Rolling_5_15` depending on configured beach slope.
- `RockyCoast` may be `Steep_15_30`, `Cliff_30_45`, or `Extreme_45Plus`, but must still intersect sea level.
- `OceanExterior` may have low/moderate slope but must remain below sea level.
- `CraterWall` and `Ridge` can be steep, but should not dominate the whole island unless configured.

### Elevation band classes

Recommended `elevation_band_class`:

```cpp
enum class ETerrainElevationBandClass : uint8
{
    DeepOcean = 0,       // height_m < -20
    ShallowOcean = 1,    // -20 <= height_m < -2
    Intertidal = 2,      // -2 <= height_m <= +2
    BeachLowland = 3,    // +2 < height_m <= +15
    Lowland = 4,         // +15 < height_m <= +80
    Upland = 5,          // +80 < height_m <= +250
    SummitMassif = 6     // height_m > +250, tune per island scale
};
```

Tune thresholds to the chosen world scale and target island height. The important part is not the exact numbers; it is stable, logged thresholds that validation can test.

### Relief classes

Recommended `relief_class` based on `local_relief_m` over a configured kernel, such as 9×9, 17×17, or a world-meter radius:

```cpp
enum class ETerrainReliefClass : uint8
{
    Smooth = 0,
    LowRelief = 1,
    Broken = 2,
    Rugged = 3,
    Extreme = 4
};
```

Relief classes help validation distinguish a beach that is low slope but noisy/bumpy from a genuinely smooth sea-level strand.

## Continuous Versus Discrete Channel Rules

### Continuous scalar fields

Use continuous scalar fields when a value needs blending, thresholding, gradients, or visual interpolation.

Rules:

- Range should be documented per channel.
- Normalize to `0..1` only when the physical unit is not needed.
- Preserve physical units where they matter: `height_m`, `ocean_depth_m`, `slope_degrees`, `local_relief_m`, `signed_island_distance_m`.
- Store signed fields as signed formats; do not pack signed distance into `0..1` without manifest scale/offset.
- Anti-aliased masks should remain scalar even if validation later thresholds them.

### Discrete class masks

Use discrete class masks when a cell has one dominant semantic category or stable id.

Rules:

- Store as integer/enum channels, not blended colors.
- Class `0` should always mean `None`, `Unknown`, or the most neutral domain depending on channel.
- Every enum must be included in the manifest with names and thresholds.
- Discrete class maps must be derived from scalar sources and should never be the only source of information.

### Packing rules

Recommended internal representation:

- High precision arrays in generator memory: `float32` for scalar fields, `uint8`/`uint16`/`uint32` for class/id maps.
- Optional debug atlas textures: `RGBA16F` or `RGBA8` depending on precision need.
- Offline export: `.exr` for high-dynamic-range continuous maps where supported; `.png` for visual-only screenshots; `.json` manifest; optional `.tif` only if the project chooses geospatial-style metadata tooling.

GDAL’s raster model is a useful external reference because raster datasets are organized into bands with metadata, no-data handling, scale, offset, color interpretation, and georeferencing concepts. Even if PR5 does not adopt GDAL files, the same discipline should be applied to the terrain channel atlas manifest. [GDAL Raster Data Model][src-gdal-raster-data-model]

### Nodata rules

Define nodata explicitly:

| Channel family | Nodata policy |
|---|---|
| Domain fields | No nodata inside world; every cell belongs to some domain. |
| Flow/aspect fields | Nodata allowed for exterior ocean, flats, or invalid cells. |
| Catchment id | `0` means no catchment / exterior / invalid. |
| Ridge/gully ids | `0` means no feature. |
| Scalar masks | Use `0` for absent, not nodata, unless the channel is truly undefined. |

If writing GeoTIFF-like exports, note that GDAL’s GeoTIFF driver stores nodata values in GDAL-specific TIFF metadata for default-profile files; PR5 should avoid hidden semantics by writing nodata explicitly into its own manifest regardless of image format. [GDAL GeoTIFF][src-gdal-gtiff]

## Developer-Only Debug Material Modes

### Non-player-facing requirements

Debug material assets must be unmistakably development-only.

Recommended naming:

```text
M_Dev_TerrainChannelDebug
MI_Dev_TerrainChannelDebug_Default
MPC_Dev_TerrainDebugParams
EDevTerrainDebugMode
```

Rules:

- Store under an editor/dev path such as `Content/Developer/TerrainDebug/` or project-equivalent.
- Exclude from shipping builds or gate activation behind editor/development console flags.
- Use high-saturation false-color palettes that are unsuitable as final art.
- Never expose these modes through player UI.
- Log every activation with config identity and selected mode.

Epic documents Material Parameter Collections as global scalar/vector parameter containers that can be referenced by materials, which is appropriate for editor-only debug mode selection, sea-level value, threshold toggles, and overlay opacity. [Epic Material Parameter Collections][src-epic-mpc] Epic’s Material Analyzer can identify material instances, static switches, and override groupings, which is useful for keeping debug materials from exploding shader permutations. [Epic Material Analyzer][src-epic-material-analyzer]

### Debug mode enum

Recommended enum:

```cpp
enum class EDevTerrainDebugMode : uint8
{
    Off = 0,
    DomainClass = 1,
    CoastInvariant = 2,
    IslandOceanBeachShelf = 3,
    HeightBands = 4,
    SlopeClass = 5,
    ReliefClass = 6,
    MassifRidgeGully = 7,
    Catchments = 8,
    Crater = 9,
    LavaCrust = 10,
    UnsafeCrust = 11,
    HardBlockers = 12,
    ChannelScalarSingle = 13,
    ValidationFailures = 14
};
```

### Color palette

Use one canonical palette for consistency across material, screenshots, and report legend.

#### Domain colors

| Class / channel | Color | Meaning |
|---|---|---|
| `OceanExterior` | deep blue `#0033CC` | Exterior ocean below sea level. |
| `CoastalShelf` | cyan `#00D5FF` | Nearshore shelf. |
| `BeachRing` | yellow `#FFE066` | Sea-level beach ring. |
| `RockyCoast` | orange `#FF8C00` | Explicit cliff/rock coast. |
| `Lowland` | green `#35D04F` | Low inland terrain. |
| `Massif` | brown `#8B5A2B` | Volcanic mass. |
| `Crater` | magenta `#CC00FF` | Crater-dominant cells. |
| `LavaField` | red `#FF2020` | Lava/crust field. |
| `HardBlocked` | black `#000000` with white hatch | Terrain hard-blocked. |
| `Invalid` | hot pink `#FF00AA` flashing/checker | Validation failure. |

#### Coast invariant overlay

`CoastInvariant` mode should show failures aggressively:

| Condition | Overlay |
|---|---|
| Valid exterior ocean below sea level | blue gradient by `ocean_depth_m`. |
| Beach ring within vertical/slope tolerance | yellow narrow band. |
| Shelf valid and below/near sea level | cyan band. |
| Exterior ocean sample `height_m >= 0` | hot pink checker. |
| Missing beach segment | red stroke along gap. |
| Rocky coast exception | orange with black edge line. |

#### Scalar channel mode

`ChannelScalarSingle` should visualize any selected scalar channel as:

```text
0.0 = black
0.5 = middle gray or channel-specific midpoint
1.0 = white
invalid/nodata = purple checker
```

For signed channels:

```text
negative = blue
zero = white contour
positive = red
```

This is especially useful for `signed_island_distance_m`.

#### Catchment mode

`Catchments` should use stable hash colors by `catchment_id`, plus:

- High-flow accumulation cells: white line overlay.
- Invalid sinks: hot pink dots.
- Outlet cells at shoreline: yellow/cyan markers in screenshots only, not player UI.

#### Ridge/gully mode

`MassifRidgeGully` should blend:

- `massif_weight`: brown intensity.
- `ridge_strength`: white/yellow ridgelines.
- `gully_strength`: dark blue incisions.
- Axis ids: optional stable color lines in screenshot/report mode.

#### Crater mode

| Crater class | Color |
|---|---|
| Outer apron | muted brown |
| Rim | bright magenta |
| Inner wall | purple |
| Floor | dark violet |
| Vent | black/red |
| Breach | yellow/orange |

#### Lava/crust mode

| Lava/crust class | Color |
|---|---|
| Old crusted flow | dark gray |
| Recent crusted flow | red-orange |
| Thin unsafe crust | white-hot pink |
| Open vent/lava | emissive red for debug only |
| Levee | orange lines |
| Channel | bright red centerline |

### Overlay mechanics

Material should support:

```text
DebugMode              scalar enum as float/int
DebugOpacity           0..1
SelectedScalarChannel  index/id
SeaLevelM              float
FailureFlashPhase      editor/dev time only
ChannelMin/Max         per selected scalar
```

Implementation should prefer a small set of static switches and dynamic scalar/vector parameters to avoid generating a unique material permutation for every channel. Use texture arrays or channel atlas sampling if UE asset pipeline supports it cleanly; otherwise, keep the first PR simple with one atlas texture plus mode parameters.

## Runtime Metadata And Log Tokens

Every generated terrain channel atlas and validation output needs a deterministic identity. Logs and filenames should be machine-greppable.

### Manifest file

Required file:

```text
TerrainChannelAtlas_<MapName>_<ConfigHash>.json
```

Required fields:

```json
{
  "schema": "JungleGame.TerrainChannelAtlas.v1",
  "project": "JungleGame",
  "batch": "PR5.Batch003",
  "map_name": "ValidationIsland_001",
  "generator_version": "TerrainGen/0.5.0",
  "config_name": "VolcanicIsland_Default",
  "config_hash": "sha256:...",
  "seed": 123456789,
  "world_size_m": 4096.0,
  "heightmap_resolution": [4033, 4033],
  "meters_per_sample": 1.016,
  "sea_level_m": 0.0,
  "island_radius_nominal_m": 1800.0,
  "coast_feather_m": 80.0,
  "beach_half_width_m": 30.0,
  "shelf_width_m": 240.0,
  "created_utc": "2026-06-26T00:00:00Z",
  "channels": [
    {
      "name": "height_m",
      "kind": "continuous",
      "units": "m",
      "range_expected": [-80.0, 520.0],
      "storage": "float32",
      "nodata": null,
      "source_pass": "TerrainHeightPass"
    }
  ],
  "class_enums": {
    "terrain_domain_class": {
      "0": "OceanExterior",
      "1": "CoastalShelf",
      "2": "BeachRing"
    }
  },
  "validation": {
    "status": "PASS",
    "report_path": "...",
    "screenshot_set": "..."
  }
}
```

### Log tokens

Use compact tokens that survive CI logs and screenshot filenames.

Required log token fields:

```text
[JG_TERRAIN_CHANNELS]
map=<MapName>
seed=<Seed>
config=<ConfigName>
hash=<ConfigHashShort>
res=<Width>x<Height>
world_m=<WorldSizeM>
sea_m=<SeaLevelM>
channels=<Count>
status=<PASS|FAIL>
```

Example:

```text
[JG_TERRAIN_CHANNELS] map=ValidationIsland_001 seed=123456789 config=VolcanicIsland_Default hash=9f31c4aa res=4033x4033 world_m=4096 sea_m=0 channels=42 status=PASS
```

Validation-specific token:

```text
[JG_TERRAIN_VALIDATE] check=CoastlineSeaLevel status=PASS invalid_ocean_px=0 max_ocean_height_m=-2.18 beach_coverage=0.997 shelf_coverage=0.991 hash=9f31c4aa
```

Failure example:

```text
[JG_TERRAIN_VALIDATE] check=CoastlineSeaLevel status=FAIL invalid_ocean_px=184 max_ocean_height_m=1.42 hash=9f31c4aa artifact=ValidationFailures_CoastlineSeaLevel_9f31c4aa.png
```

### Config identity

`config_hash` should include:

- Generator version.
- Terrain config values.
- Seed.
- Channel schema version.
- Heightmap resolution.
- World size/meters-per-sample.
- Sea level.
- Coast/beach/shelf thresholds.
- Any enabled rocky coast/lava/crater variants.

It should not include absolute local machine paths, usernames, timestamps, or nondeterministic editor state.

## Validation Map Requirements

### Required validation map

Create one dedicated map/level for this batch:

```text
L_Dev_TerrainChannels_Validation
```

The validation map should contain:

- One generated active volcanic island using the default PR5 Batch 003 terrain config.
- Developer-only debug material instance assigned to terrain.
- Deterministic camera bookmarks or screenshot viewpoints.
- No gameplay UI requirements.
- No foliage/ecology/weather/audio/story/NPC dependencies.

### Required screenshots/maps

Every validation run should produce PNG screenshots or offline rendered maps with identical names based on config hash.

Required set:

```text
01_DomainClass_<hash>.png
02_CoastInvariant_<hash>.png
03_IslandOceanBeachShelf_<hash>.png
04_HeightBands_<hash>.png
05_SlopeClass_<hash>.png
06_ReliefClass_<hash>.png
07_MassifRidgeGully_<hash>.png
08_Catchments_<hash>.png
09_Crater_<hash>.png
10_LavaCrust_<hash>.png
11_UnsafeCrust_<hash>.png
12_HardBlockers_<hash>.png
13_ValidationFailures_<hash>.png
```

Optional but useful:

```text
14_SignedIslandDistance_<hash>.png
15_BeachWeightScalar_<hash>.png
16_OceanDepthScalar_<hash>.png
17_FlowAccumLog_<hash>.png
```

### Required numeric report

Required file:

```text
TerrainValidationReport_<MapName>_<ConfigHash>.md
```

Minimum report table:

| Check | Metric | Required result |
|---|---|---|
| Ocean exterior below sea | `count(height_m >= sea_level_m where signed_island_distance_m > guard)` | `0` invalid cells. |
| Beach ring continuity | connected coverage around shoreline | `>= 99%` of shoreline samples, unless rocky exception coverage is logged. |
| Shelf continuity | shelf belt coverage around shoreline | `>= 98%`, excluding configured rocky coast gaps. |
| Island mask closed | connected island domain count | exactly 1 major island unless config says otherwise. |
| Edge guard | raised edge cells | `0`. |
| Slope classes valid | class coverage sum | `100%` of terrain cells classified. |
| Catchments valid | invalid sinks | `0` unexpected sinks after accepted depressions. |
| Ridge/gully bounds | feature cells in exterior ocean | `0` except shoreline outlets for gullies. |
| Crater coherent | rim/floor/wall overlap constraints | no missing rim unless breach configured. |
| Lava/crust coherent | unsafe crust subset relation | unsafe crust must be subset of lava/crust or configured vent/crater area. |
| Hard blockers coherent | class reason present | every blocker class has matching source condition. |
| Determinism | rerun hash and channel diffs | same config/seed produces byte-identical or tolerance-identical channels. |

### Source-control handling of generated artifacts

Recommended approach:

- Commit source code, configs, enum definitions, debug material assets, and a small golden manifest/report if useful.
- Do **not** commit large generated channel atlases by default.
- Commit a small, downsampled validation image set only if it is intentionally used as a regression/golden artifact.
- Put generated full-resolution outputs under a gitignored path such as:

```text
Saved/TerrainValidation/
DerivedDataCache/
Intermediate/TerrainChannels/
```

- If golden images are committed, place them under a clear test path:

```text
SourceTests/Golden/TerrainChannels/<ConfigName>/<hash>/
```

- Every committed golden image must have a matching manifest with config hash and generator version.
- PRs must not include accidental editor cache, DDC, full-resolution generated atlases, or screenshots without manifest/report context.

## Large PR Implementation Scope

This is a large implementation batch. It should still be scoped around terrain-channel infrastructure, not general worldbuilding.

Recommended PR scope:

1. Add terrain channel schema/types.
2. Add channel atlas data container and manifest serialization.
3. Add generator passes for core domain/coast fields.
4. Add derived terrain analysis pass for slope/relief/catchment basics.
5. Add volcanic fields for crater/lava/crust/blockers at data level.
6. Add developer-only debug material and mode enum.
7. Add validation map and deterministic screenshot/report workflow.
8. Add automated validation checks.
9. Add documentation for channel contract and source-control artifact policy.

Keep out of scope:

- Final landscape material art.
- Foliage/material biome systems.
- Gameplay use of hard blockers or unsafe crust.
- Player-facing UI.
- Runtime minimaps or navigation aids.
- Cinematic/environment presentation work.

## Files And Areas Likely Touched

Exact paths depend on the current repo layout. Likely areas:

### C++ / source modules

```text
Source/JungleGame/Terrain/
  TerrainChannelTypes.h
  TerrainChannelAtlas.h/.cpp
  TerrainChannelManifest.h/.cpp
  TerrainGeneratorConfig.h
  TerrainGenerator.h/.cpp
  TerrainMaskPass_DomainCoast.h/.cpp
  TerrainMaskPass_Geomorphology.h/.cpp
  TerrainMaskPass_Volcanic.h/.cpp
  TerrainValidation.h/.cpp
  TerrainDebugMode.h
```

### Editor/developer tooling

```text
Source/JungleGameEditor/Terrain/
  TerrainValidationCommandlet.h/.cpp
  TerrainDebugScreenshotTool.h/.cpp
```

or project-equivalent Python/Blutility tooling if the repo already uses editor scripts.

### Content assets

```text
Content/Developer/TerrainDebug/
  M_Dev_TerrainChannelDebug.uasset
  MI_Dev_TerrainChannelDebug_Default.uasset
  MPC_Dev_TerrainDebugParams.uasset
  L_Dev_TerrainChannels_Validation.umap
```

### Config/docs/tests

```text
Config/Terrain/VolcanicIsland_Default.json
Docs/Terrain/TerrainChannelContract.md
Docs/Terrain/TerrainValidation.md
Tests/Terrain/TerrainChannelValidationTests.cpp
Tests/Terrain/Golden/...
```

### Generated artifacts

```text
Saved/TerrainValidation/<ConfigHash>/
  TerrainChannelAtlas_<MapName>_<ConfigHash>.json
  TerrainValidationReport_<MapName>_<ConfigHash>.md
  *.png
```

Generated artifacts should normally remain uncommitted unless explicitly selected as golden regression fixtures.

## Acceptance Criteria

### Channel schema acceptance

- All required channels in this document exist or have explicitly documented deferred status.
- Every channel has a stable name, type, range/units, owner pass, nodata rule, and debug mode mapping.
- Continuous scalar fields and discrete class masks are not conflated.
- Enums are documented in code and manifest output.

### Coastline acceptance

Required pass conditions:

```text
invalid_ocean_px == 0
max(height_m where ocean_exterior_mask) < sea_level_m
beach_ring_coverage >= 0.99, excluding logged rocky coast exception spans
coastal_shelf_coverage >= 0.98, excluding logged rocky coast exception spans
raised_edge_px == 0
```

Additionally:

- `signed_island_distance_m == 0` contour and `height_m == sea_level_m` contour should align within configured tolerance.
- Beach ring must be visible as a continuous band in `02_CoastInvariant` and `03_IslandOceanBeachShelf` screenshots.
- Shelf must visibly surround the beach/coast and remain distinct from deep ocean.
- Rocky coast coverage must be logged separately and cannot silently hide beach failures.

### Landform acceptance

- `massif_weight` is connected, inland, and fades before coast unless rocky coast config says otherwise.
- `ridge_strength` cells are inside island/coast domains and have stable axis ids where applicable.
- `gully_strength` aligns with flow/catchment channels and does not create uphill artifacts above configured tolerance.
- `catchment_id` produces stable basins with valid outlets or explicitly accepted internal depressions.
- `crater_rim_weight`, `crater_floor_weight`, `crater_wall_weight`, and `crater_breach_weight` produce a coherent crater class map.
- Lava/crust/unsafe crust fields are terrain-data outputs, not material-only effects.
- `unsafe_crust_weight` is a subset of lava/crust/vent-related terrain regions unless documented otherwise.
- Hard blocker classes match their source conditions.

### Debug material acceptance

- Developer-only debug material can show every required debug mode.
- Debug material activation is not exposed as player UI.
- Debug material path/name makes development-only intent obvious.
- Material parameters include debug mode, opacity, scalar selection, sea level, min/max range, and failure overlay controls.
- Material Analyzer or equivalent review confirms the debug material does not create unreasonable shader-permutation bloat for this batch. Epic documents Material Analyzer as a tool for identifying material instances, static switches, and override groupings. [Epic Material Analyzer][src-epic-material-analyzer]

### Metadata/log acceptance

- Every generation run writes a manifest with schema, seed, config hash, generator version, world scale, sea level, resolution, and channels.
- Every validation run logs `[JG_TERRAIN_CHANNELS]` and `[JG_TERRAIN_VALIDATE]` tokens.
- Screenshot filenames include config hash.
- Same seed/config rerun is deterministic within declared tolerance.

### Source-control acceptance

- Full-resolution generated atlases are not accidentally committed.
- Any committed golden images are downsampled or deliberately sized, reviewed, and paired with manifest/report files.
- No DDC, cache, temp, or unrelated editor artifacts are included.
- Documentation explains how to regenerate validation outputs.

## Risks And Stop Conditions

### Risks

| Risk | Why it matters | Mitigation |
|---|---|---|
| Coastline drift after later height passes | Ridges/noise/lava may raise ocean edge | Apply coast invariant after every height-changing pass and final clamp/check. |
| Beach ring breaks into fragments | Height-only thresholding can fail on noisy coast | Use signed distance + sea-level proximity + slope allowance. |
| Shelf confused with beach | Both are coastal but serve different validation roles | Separate `beach_weight`, `shoreline_weight`, and `coastal_shelf_weight`. |
| Debug colors become material art | False-color can accidentally become design direction | Use dev-only paths, high-saturation palettes, build gates, and docs. |
| Class masks hide useful gradients | Discrete output alone loses blend/threshold detail | Always keep scalar source fields. |
| Catchment derivation overfits synthetic terrain | Flow algorithms can produce artifacts on flats/noise | Start simple, log invalid sinks, use smoothing/depression handling only when explicit. |
| Generated artifacts bloat repo | Full-res maps can be huge | Gitignore generated outputs; commit only curated golden fixtures. |
| Config hash misses important inputs | Reproducibility becomes false confidence | Hash schema/generator/config/seed/resolution/sea-level thresholds together. |

### Stop conditions

Stop and fix before moving to next stage if any are true:

- Any exterior ocean cell outside guard is at or above sea level.
- Beach ring cannot be proven around the shoreline and rocky coast exceptions are not explicitly logged.
- Terrain debug material is reachable as player-facing UI or final gameplay presentation.
- Required channels exist only as material colors and not as generator-owned data.
- Manifest lacks seed/config hash/sea-level/channel schema.
- Validation screenshots are produced without matching manifest and log token.
- Generated full-resolution artifacts appear in source control unintentionally.
- Hard blocker or unsafe crust fields are implemented as gameplay systems instead of terrain diagnostics/data.

## Rejected Or Forbidden Approaches

Rejected:

- **Height-only coastline inference**: deriving island/ocean/beach only by thresholding final height after all noise passes. This does not prove the outer edge invariant.
- **Material-only masks**: encoding terrain meaning only in shader colors without source-owned data channels.
- **Player-facing debug overlays**: any map/minimap/compass/GPS/objective/time/orientation UI.
- **Final art material work**: this batch is not about polished sand/rock/lava shading.
- **Foliage/ecology/weather coupling**: terrain masks may later support such systems, but they are not designed here.
- **Implicit rocky coast exceptions**: cliff coast must be a named variant/channel, not an excuse for failed beach/shelf validation.
- **Unversioned generated outputs**: screenshots or atlases without config hash, manifest, and generator version.
- **Single blended mega-mask as source of truth**: convenient debug colors cannot replace named channels.
- **Nondeterministic validation**: random editor state, timestamps, or local paths cannot affect generated terrain identity.

## Sources And References

1. <a id="src-epic-landscape-technical"></a>Epic Games, “Landscape Technical Guide in Unreal Engine,” UE 5.8 documentation. Used for Landscape component/section architecture, valid/recommended heightmap dimensions, height scaling, and Layer Debug visualizer behavior. https://dev.epicgames.com/documentation/unreal-engine/landscape-technical-guide-in-unreal-engine
2. <a id="src-epic-landscape-materials"></a>Epic Games, “Landscape Materials in Unreal Engine,” UE 5.8 documentation. Used for Landscape layer blending concepts, unique layer naming, blend types, and debug-friendly constant layer inputs. https://dev.epicgames.com/documentation/unreal-engine/landscape-materials-in-unreal-engine
3. <a id="src-epic-mpc"></a>Epic Games, “Using Material Parameter Collections in Unreal Engine,” UE 5.8 documentation. Used for global scalar/vector material parameters suitable for developer debug mode controls. https://dev.epicgames.com/documentation/unreal-engine/using-material-parameter-collections-in-unreal-engine
4. <a id="src-epic-material-analyzer"></a>Epic Games, “Unreal Engine Material Analyzer Tool,” UE 5.8 documentation. Used for keeping developer debug materials auditable for static switches, overrides, shader permutations, and material instance organization. https://dev.epicgames.com/documentation/unreal-engine/unreal-engine-material-analyzer-tool
5. <a id="src-grass-slope-aspect"></a>GRASS GIS, “r.slope.aspect,” GRASS 8.5 manual. Used as a reference for deriving slope, aspect, curvatures, and terrain derivatives from elevation rasters. https://grass.osgeo.org/grass-stable/manuals/r.slope.aspect.html
6. <a id="src-grass-watershed"></a>GRASS GIS, “r.watershed,” GRASS 8.5 manual. Used as a reference for flow accumulation, drainage direction, streams, and watershed basin outputs from elevation data. https://grass.osgeo.org/grass-stable/manuals/r.watershed.html
7. <a id="src-taudem-dinf"></a>Utah State University Hydrology Research Group, TauDEM “D-Infinity Contributing Area” and “D-Infinity Flow Directions” documentation. Used as a reference for specific catchment area and multiple-flow-direction terrain drainage concepts. https://hydrology.usu.edu/taudem/taudem5/help53/DInfinityContributingArea.html and https://hydrology.usu.edu/taudem/taudem5/help53/DInfinityFlowDirections.html
8. <a id="src-gdal-raster-data-model"></a>GDAL, “Raster Data Model.” Used as a reference for raster bands, metadata, scale/offset, color interpretation, nodata, and disciplined multi-channel raster storage concepts. https://gdal.org/en/stable/user/raster_data_model.html
9. <a id="src-gdal-gtiff"></a>GDAL, “GTiff -- GeoTIFF File Format.” Used for nodata/metadata considerations when exporting raster-like channel artifacts. https://gdal.org/en/stable/drivers/raster/gtiff.html
10. <a id="src-usgs-about-volcanoes"></a>U.S. Geological Survey Volcano Hazards Program, “About Volcanoes.” Used for basic volcanic eruption/lava-flow and tephra context relevant to terrain-only volcanic channel semantics. https://www.usgs.gov/programs/VHP/about-volcanoes
11. <a id="src-usgs-volcano-glossary"></a>U.S. Geological Survey Volcano Hazards Program, “Glossary.” Used for lava crust/tumulus terminology and terrain feature context. https://www.usgs.gov/glossary/volcano-hazards-program-glossary
12. <a id="src-fao-land-suitability"></a>Food and Agriculture Organization of the United Nations, “Chapter 3: Land suitability classifications.” Used as a general reference for classifying land units by explicit classes; PR5 uses project-specific terrain classes, not FAO agricultural suitability classes. https://www.fao.org/4/x5310e/x5310e04.htm
13. <a id="src-flow-accumulation-grid"></a>Haverkort, H. and Janssen, J., “Simple I/O-efficient flow accumulation on grid terrains,” arXiv:1211.1857. Used as a computational reference for grid-terrain flow accumulation as a matrix of flow directions and contributing cells. https://arxiv.org/abs/1211.1857
14. <a id="src-geomorphometry"></a>Florinsky, I. V., “Geomorphometric modeling and mapping of Antarctic oases,” arXiv:2305.07523. Used as a broad geomorphometry reference for derived variables such as slope, aspect, curvature, catchment area, topographic index, and stream power index. https://arxiv.org/abs/2305.07523


[src-epic-landscape-technical]: https://dev.epicgames.com/documentation/unreal-engine/landscape-technical-guide-in-unreal-engine
[src-epic-landscape-materials]: https://dev.epicgames.com/documentation/unreal-engine/landscape-materials-in-unreal-engine
[src-epic-mpc]: https://dev.epicgames.com/documentation/unreal-engine/using-material-parameter-collections-in-unreal-engine
[src-epic-material-analyzer]: https://dev.epicgames.com/documentation/unreal-engine/unreal-engine-material-analyzer-tool
[src-grass-slope-aspect]: https://grass.osgeo.org/grass-stable/manuals/r.slope.aspect.html
[src-grass-watershed]: https://grass.osgeo.org/grass-stable/manuals/r.watershed.html
[src-taudem-dinf]: https://hydrology.usu.edu/taudem/taudem5/help53/DInfinityContributingArea.html
[src-gdal-raster-data-model]: https://gdal.org/en/stable/user/raster_data_model.html
[src-gdal-gtiff]: https://gdal.org/en/stable/drivers/raster/gtiff.html
[src-usgs-about-volcanoes]: https://www.usgs.gov/programs/VHP/about-volcanoes
[src-usgs-volcano-glossary]: https://www.usgs.gov/glossary/volcano-hazards-program-glossary
[src-fao-land-suitability]: https://www.fao.org/4/x5310e/x5310e04.htm
[src-flow-accumulation-grid]: https://arxiv.org/abs/1211.1857
[src-geomorphometry]: https://arxiv.org/abs/2305.07523
