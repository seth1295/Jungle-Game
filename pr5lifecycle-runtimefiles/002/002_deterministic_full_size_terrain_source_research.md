# Deterministic Full-Size Terrain Source Research

## Executive Summary

Jungle Game needs a deterministic source terrain path that moves the project from a visible Cell 0-style playable slice toward an actual loaded full-size UE world/level/landscape state. This document specifies a narrow implementation target: define a reproducible 16 km-class tropical island terrain source, generate or validate terrain analysis channels, and produce small review artifacts that prove the landform is structured, navigable, hydrologically coherent, and not arbitrary noise.

The recommended path is a **single 8129 x 8129 source heightfield at 2 m horizontal spacing**, yielding a square source domain of approximately **16.256 km x 16.256 km**. Unreal Engine's Landscape Technical Guide lists 8129 x 8129 vertices as a recommended Landscape size using 127 quads per section, 2x2 sections per component, 254 x 254 quads per component, and 32 x 32 total components. This gives the implementation PR a full-size terrain without immediately forcing a tiled 1 m production import. If later detail requires it, the same source schema can expand to a tiled 4 x 4 set of 4033-vertex tiles at 1 m spacing.

The terrain should be produced from a small, human-reviewable deterministic config rather than from a hand-authored binary heightmap or paid terrain package. The config should encode the island outline, coastline control, ocean boundary, mountain spine, ridge/spur graph, saddle controls, drainage basins, creek heads, erosion/carving parameters, coast classification thresholds, and output dimensions. Procedural noise may be used only as a subordinate seeded detail layer; it must never be the structural source of the island.

The implementation PR should commit the deterministic source config, schema notes, validator logic, a channel manifest, hashes, downsampled preview images, and CSV/Markdown review samples. Full generated rasters, `.r16` import files, and large intermediate arrays should be generated locally and ignored unless the repo already has a clear large-artifact policy.

This document intentionally excludes every player-facing navigation aid. All review tables, debug overlays, sample rasters, and inspection points described here are **developer/editor validation artifacts only**, not gameplay UI.

## Assumptions And Constraints

### Project assumptions

- Engine target: Unreal Engine 5, Linux-first development workflow.
- World target: first-person tropical island survival-simulation around 1000 CE.
- The target is a **loaded UE map/level/landscape state**, not a player-facing map screen.
- Existing PR5 source-level contracts already cover macro island terrain, biome masks, traversal categories, creek/riparian/wet-valley ecosystems, ridge/slope/canopy orientation, and validation evidence.
- The next implementation should be narrow enough for one PR but should establish the full-size terrain source path.

### Hard gameplay/UI constraints

The terrain source and its review artifacts must not introduce, recommend, or imply any player-facing:

- map;
- minimap;
- compass;
- bearing number/readout;
- GPS marker;
- objective marker;
- objective arrow;
- HUD clock;
- explicit time display;
- orientation confidence meter;
- hidden orientation meter;
- character comments that tell the player whether they are oriented;
- modern navigation tools.

Developer-only debug outputs may include coordinates, aspect, flow, and sample IDs, but those outputs must stay in source-control artifacts, editor-only previews, test reports, or validation logs.

### Terrain generation constraints

The implementation must avoid:

- random noise-only terrain;
- required paid/proprietary terrain tools;
- huge unexplained binary dumps;
- undocumented hand-authored heightmaps;
- terrain that cannot be reproduced from source parameters;
- creek lines that ignore slope and basin structure;
- coast classifications that are only painted by vibes;
- full production biome dressing before the terrain source itself is proven.

### Determinism requirements

The terrain must be regenerable from a clean checkout using a fixed config and seed. A successful regeneration should produce matching dimensions, declared channel names, and stable hashes for all generated deterministic outputs. If floating-point implementation differences prevent byte-identical rasters across all machines, the validator should define acceptable numeric tolerances and still produce stable logical summaries.

## Target Terrain Scale And Resolution

### Recommended implementation target

Use the following source-domain target for the first full-size PR:

```yaml
terrain_source:
  schema_version: 1
  world_name: jungle_game_full_island_source_002
  seed: 27002002
  source_vertices_x: 8129
  source_vertices_y: 8129
  source_cell_size_m: 2.0
  source_extent_m_x: 16256.0
  source_extent_m_y: 16256.0
  sea_level_m: 0.0
  source_min_height_m: -128.0
  source_max_height_m: 896.0
  height_range_m: 1024.0
  unreal_xy_scale_cm: 200.0
  unreal_z_scale: 200.0
  unreal_landscape_z_offset_cm: 38400.0
```

Rationale:

- `8129 x 8129` is an official recommended UE Landscape size.
- `2 m` spacing gives a 16 km-class world while keeping the first full-size source manageable.
- The full height source contains `66,080,641` samples. A single 16-bit height raster is about 126 MiB before compression; a float32 channel is about 252 MiB. This is large enough to require a deliberate artifact policy but still feasible on a development machine.
- A 2 m macro source is appropriate for island shape, highland massing, ridges, creek basins, major slope breaks, wet-valley placement, and UE World Partition map-state work. Fine footstep-scale terrain can be added later through landscape material detail, meshes, decals, local sculpt layers, or higher-resolution subregion work.

### UE import scale notes

Unreal's Landscape system uses heightmaps and supports 16-bit grayscale PNG, 8-bit `.r8`, 16-bit `.r16`, and RAW files with JSON sidecars. For UE import, keep the canonical source as `height_m` and export a deterministic `height_ue_u16.r16` or 16-bit grayscale PNG only as a generated import artifact.

Recommended mapping:

```text
height_range_m = source_max_height_m - source_min_height_m
source_mid_m   = (source_min_height_m + source_max_height_m) / 2
local_ue_m     = source_height_m - source_mid_m
unreal_z_scale = height_range_m * 100 * 0.001953125
actor_z_offset = source_mid_m * 100 cm
```

For `source_min_height_m = -128`, `source_max_height_m = 896`, and `height_range_m = 1024`, this gives:

```text
source_mid_m = 384
unreal_z_scale = 1024 * 100 * 0.001953125 = 200
actor_z_offset = 38400 cm
```

This preserves both shallow ocean/bathymetry space and high inland terrain while placing sea level at world Z 0 after actor offset. The implementation PR should include a manifest entry that records this mapping so future imports do not silently shift the ocean line.

### Future higher-resolution option

Do not start with this unless the implementation discovers a hard requirement for 1 m macro height samples:

```text
4 x 4 tiles
4033 x 4033 vertices per tile
4032 quads per tile edge
16128 m edge at 1 m per quad
stitched logical source grid: 16129 x 16129 vertices
```

This aligns with another official recommended Landscape size per tile, but it increases source and channel volume substantially. It is better as a later production import path once the deterministic source model and review strategy are proven.

## Deterministic Source Pipeline Options

| Option | Summary | Strengths | Weaknesses | Recommendation |
|---|---|---|---|---|
| A. Real DEM-derived island | Start from public elevation data, normalize/modify into the game island. | High realism; real drainage clues; real coastline complexity. | Licensing/provenance work; may create accidental real-world copy; 16 km tropical island data quality varies; harder to own the design. | Useful as reference only, not the first PR source of truth. |
| B. Deterministic structural generator | Human-readable config defines outline, highland spine, ridges, valleys, creek heads, and coast classes; generator derives channels. | Reproducible; source-controllable; design-owned; can be validated; avoids paid tools. | Needs careful validation to avoid artificial shapes. | **Recommended.** |
| C. External terrain app as source | Use Gaea/World Machine/Terragen/Houdini as the required terrain authoring path. | Strong production workflows if licensed and already adopted. | Violates required non-paid path if mandatory; source diffs often poor; automation may be brittle. | Reject as required path. Allow optional reference only. |
| D. Noise stack with island mask | Combine fractal noise, radial falloff, erosion-ish filters. | Fast prototype. | Forbidden if it is the terrain structure; produces arbitrary landforms and weak navigation evidence. | Reject except as low-amplitude detail under structural controls. |
| E. Hand-painted full-size heightmap | Artist edits a large grayscale image directly. | Direct control. | Not reproducible unless every source stroke is preserved; poor code review; binary-heavy. | Reject for this PR. |

Recommended pipeline: **Option B** with limited deterministic noise as secondary roughness. The PR should define the source config and validation contract first, then generate review samples. Full terrain import can occur after the source is validated or in the same PR only if the repo already has reliable UE import automation.

## Recommended Pipeline For Jungle Game

### Pipeline overview

The generator should transform a small deterministic source config into terrain channels through these stages:

1. **Domain setup**
   - Create an `8129 x 8129` grid at `2 m` spacing.
   - Define origin, coordinate convention, sea level, source height range, and UE import mapping.
   - All coordinates should be in source meters relative to island center or southwest grid origin; choose one convention and write it in the manifest.

2. **Island outline and signed coast distance**
   - Generate a coastline polygon from deterministic outline controls.
   - Compute signed distance to coast: positive inland, negative ocean.
   - Enforce an ocean boundary ring around the island so the terrain always resolves to water at the domain edge.

3. **Base landform field**
   - Use coast distance and highland massing to create a broad island dome/ramp.
   - Add asymmetric highland lobes so the island does not become a circular cone.
   - Reserve low coastal flats, mangrove pockets, and creek mouths before ridge detail is added.

4. **Mountain spine, ridges, spurs, saddles**
   - Define a primary mountain spine as a polyline/spline with named control points.
   - Generate ridge kernels along the spine and secondary spur lines.
   - Insert saddle controls by locally lowering spine passes.
   - Apply slope-break controls at foothill transitions.

5. **Creek heads and flow structure**
   - Define candidate creek heads near upper wet bowls and saddle-adjacent hollows.
   - Compute drainage direction and accumulation from the generated heightfield.
   - Adjust/carve channels only where they follow valid descent or where explicit geological controls justify a gorge.
   - Classify basins, valley bottoms, riparian influence, alluvial fans, creek mouths, and wet valleys.

6. **Coast classification**
   - Use coast distance, slope, local relief, flow accumulation, and exposure sectors to classify beach, rocky shore, mangrove, creek mouth, and cliff coast.
   - Do not paint coast types independently of terrain channels.

7. **Derived terrain channels**
   - Export height, slope, aspect, flow, ridge/valley, wetness proxy, coast distance, creek influence, and review masks.
   - Produce downsampled previews and tabular review samples.

8. **Validation and manifest**
   - Validate dimensions, range, determinism, hydrology, coast type distribution, basin count, sample routes, and source hashes.
   - Write a manifest with config hash, output hashes, channel stats, and UE import scale.

### Config shape

Use JSON or YAML. JSON is stricter for tooling; YAML is more readable. Either is acceptable if the parser is deterministic and the schema is documented. A sketch:

```yaml
schema_version: 1
seed: 27002002
coordinate_system:
  convention: centered_meters_xy
  x_axis: east
  y_axis: north
  origin: island_center
  cell_size_m: 2.0
  vertices: [8129, 8129]
height:
  sea_level_m: 0.0
  min_m: -128.0
  max_m: 896.0
  ocean_boundary_min_depth_m: -48.0
island_outline:
  base_shape: asymmetric_superellipse
  radius_x_m: 6900
  radius_y_m: 6100
  radial_harmonics:
    - {order: 2, amplitude_m: 360, phase_deg: 30}
    - {order: 3, amplitude_m: 240, phase_deg: 115}
    - {order: 5, amplitude_m: 160, phase_deg: 260}
  named_coastal_controls:
    - {id: north_cliff_bight, angle_deg: 18, offset_m: -280, coast_bias: cliff}
    - {id: east_mangrove_lagoon, angle_deg: 92, offset_m: 420, coast_bias: mangrove}
    - {id: south_beach_arc, angle_deg: 205, offset_m: 520, coast_bias: beach}
    - {id: west_rocky_points, angle_deg: 292, offset_m: -180, coast_bias: rocky}
mountain_spine:
  max_elevation_m: 760
  controls:
    - {id: south_pass, x_m: -2200, y_m: -4300, elevation_bias_m: 210, saddle: true}
    - {id: central_peak, x_m: -700, y_m: -1100, elevation_bias_m: 760}
    - {id: knife_ridge, x_m: 950, y_m: 1400, elevation_bias_m: 690}
    - {id: north_peak, x_m: 1600, y_m: 4100, elevation_bias_m: 620}
spur_graph:
  spur_half_width_m: 180
  slope_break_width_m: 260
  spurs:
    - {id: west_spur_01, start: central_peak, azimuth_deg: 250, length_m: 3000, falloff_m: 460}
    - {id: east_spur_01, start: knife_ridge, azimuth_deg: 105, length_m: 3600, falloff_m: 520}
    - {id: north_wet_saddle_spur, start: north_peak, azimuth_deg: 45, length_m: 2100, falloff_m: 360}
hydrology:
  flow_model: d8_for_validation_dinf_optional_later
  min_stream_accumulation_cells: 9000
  creek_heads:
    - {id: creek_red, x_m: -1120, y_m: -820, min_head_elevation_m: 380}
    - {id: creek_black, x_m: 620, y_m: 1240, min_head_elevation_m: 420}
    - {id: creek_green, x_m: 1410, y_m: 3180, min_head_elevation_m: 340}
coast_classification:
  beach_max_slope_deg: 7
  mangrove_max_elevation_m: 3.5
  mangrove_min_wetness_quantile: 0.75
  cliff_min_slope_deg: 28
  creek_mouth_max_distance_m: 120
noise_detail:
  enabled: true
  seed_offset: 4402
  max_amplitude_m: 14
  structural_weight_limit: 0.12
```

The exact control values above are examples, not final design mandates. The implementation PR should choose values deliberately and record why the first accepted configuration is fit for review.

### Determinism guardrails

- Seed all random-like sources from the top-level seed plus named deterministic offsets.
- Sort named control records before processing if the input language does not preserve object order.
- Write the generator version and config hash into the manifest.
- Quantize exported review channels consistently.
- Avoid time, hostname, absolute local path, GPU, thread scheduling, or nondeterministic parallel reductions in output generation.
- If multithreading is used, reduce in deterministic order.

## Island Outline And Coastline Model

### Coastline source model

The island outline should be generated as a controlled polygon, not an image mask. The first implementation should use:

- a base asymmetric superellipse or ellipse-like island body;
- deterministic radial harmonics for coves, headlands, and broad asymmetry;
- named coastal control points for major design-relevant areas;
- smoothing limits to prevent jagged pixel noise;
- minimum ocean ring clearance from domain edge.

The coastline should produce these source products:

```text
coast_polygon.geojson or coast_polygon.json
coastline_samples.csv
coast_signed_distance_m raster
land_mask raster
near_coast_band raster
```

The land mask should include a continuous island body. If small offshore rocks are needed later, they should be separate named controls, not accidental mask speckles.

### Ocean boundary assumption

The source domain is a square simulation/import domain, not the diegetic whole ocean. The island must sit inside an ocean boundary ring:

- domain edge must be ocean on all sides;
- minimum distance from coastline to domain edge should be at least 700 m unless deliberately overridden;
- ocean floor can slope from shallow shelf to boundary depth;
- generated bathymetry should support horizon/ocean visuals but should not dominate the terrain PR.

Recommended values:

```yaml
ocean:
  boundary_is_ocean: true
  min_coast_to_domain_edge_m: 700
  shallow_shelf_depth_m: -8
  boundary_depth_m: -48
  reef_or_rock_noise_amplitude_m: 2.0
```

### Coast type classification

Coast type should be derived from terrain and hydrology channels:

| Coast type | Suggested derivation |
|---|---|
| Beach | low near-coast slope, concave or protected coast segment, low relief behind shore, not a major creek mouth. |
| Rocky shore | moderate slope and local relief near coast, headland exposure, no wide flat wet zone. |
| Mangrove | very low elevation above sea level, low slope, high wetness proxy, protected embayment or creek-adjacent flat. |
| Creek mouth | near intersection of stream channel and coast, high flow accumulation, local alluvial flat or notch. |
| Cliff coast | steep near-coast slope, high local relief, coast distance gradient rises rapidly inland. |

The validator should reject coast classifications that cover implausible fractions of the island, such as all beach, all mangrove, or all cliff. The exact distribution is a design choice, but the review report should expose the percentages.

## Mountain Spine / Ridge / Spur / Saddle Model

### Primary mountain spine

The terrain needs a readable macro orientation structure. Use a named primary spine instead of a central blob. For example:

```text
south_pass -> central_peak -> knife_ridge -> north_peak
```

The spine should:

- create a broad highland mass, not just a narrow wall;
- include at least two named peaks or high shoulders;
- include at least two saddles/passes that create plausible crossing routes;
- bias creek heads and valley starts;
- create skyline and slope cues for diegetic navigation;
- avoid forming a continuous impassable barrier unless intentionally broken by saddles.

### Ridges and spurs

Secondary spurs should branch from the primary spine and descend toward coast or interior basins. Each spur should have:

- a named start anchor;
- azimuth or endpoint;
- length;
- half-width/falloff;
- intended role: navigation landmark, valley divider, creek boundary, coastal headland, or basin edge.

Ridge generation should combine broad ridge kernels with slope-break shaping. Spur tips should degrade into foothills or headlands rather than ending abruptly.

### Saddles and passes

Saddles matter because players can read them as crossing opportunities without any UI aid. The config should include explicit saddle controls:

```yaml
saddles:
  - {id: south_pass, x_m: -2200, y_m: -4300, target_elevation_m: 210, width_m: 520}
  - {id: central_saddle, x_m: 120, y_m: 260, target_elevation_m: 430, width_m: 360}
  - {id: north_wet_pass, x_m: 1220, y_m: 2860, target_elevation_m: 330, width_m: 460}
```

Validation should sample cross-sections over saddles and prove they are visible in height, slope, and ridge/valley channels.

### Slope breaks

Slope breaks should mark transitions among:

- coast flats;
- mangrove/wet lowlands;
- lower jungle foothills;
- steep highland slopes;
- ridge crests;
- gorge candidates.

A slope-break channel should be derived or previewed so traversal and biome systems can later consume it without recomputing fragile heuristics.

## Creek Head / Flow / Basin / Valley Model

### Hydrology model for the first PR

Use a simple, reproducible D8-style validation model first. D8 routes each cell toward the steepest-descent neighbor and is widely available in terrain analysis tooling. Later, D-infinity or multiple-flow-direction methods can be added for smoother wetness modeling, but they are not required for the first deterministic source PR.

The first implementation should produce:

```text
flow_direction_d8
flow_accumulation_cells
flow_accumulation_log
basin_id
stream_candidate_mask
creek_centerline_samples
creek_influence_m
wetness_proxy
```

### Creek heads

Creek heads should not appear randomly. They should usually emerge from:

- wet upper bowls;
- saddle-adjacent hollows;
- high-rainfall windward slopes if weather exposure is modeled;
- spring-like highland contacts;
- convergent valleys below spurs.

Each named creek head should have a candidate coordinate and should then be snapped or validated against local descent. If a head cannot connect to a downhill path without artificial uphill flow, the validator should flag it.

### Flow accumulation and streams

The initial stream mask should use flow accumulation thresholding plus terrain context:

```text
stream_candidate = flow_accumulation_cells >= min_stream_accumulation_cells
stream_candidate &= slope_deg within allowed channel range OR valley_index indicates valley bottom
stream_candidate &= land_mask
```

Use a threshold high enough to avoid every small noise hollow becoming a creek. GRASS `r.watershed` documents that lower exterior basin thresholds increase detail, runtime, and difficult-to-read basin/segment output; this is a useful warning for the PR validator. The first pass should prefer fewer, legible creek systems over a dense artificial drainage web.

### Valleys, basins, gullies, alluvial areas, gorge candidates

Derived valley classes should come from flow, slope, local relief, and ridge/valley position:

| Feature | Suggested detection |
|---|---|
| Basin | connected drainage area ending at a coast outlet or internal wetland exception. |
| Main valley | high flow accumulation, concave local terrain, downhill continuity. |
| Gully | smaller accumulation, steeper slope, narrow valley index. |
| Alluvial area | low slope below valley exit, near creek mouth or basin floor, high wetness. |
| Gorge candidate | high local relief, narrow valley floor, steep side slopes, persistent stream. |
| Wet valley | high wetness proxy, low-to-moderate slope, close to creek influence. |

The implementation should avoid overcommitting final ecology. The terrain source should mark physical candidates; biome/foliage systems can decide final art/ecology later.

### Hydrological conditioning

Generated terrain often contains pits or ambiguous flats. The first PR can use light conditioning, but every conditioning step must be deterministic and recorded:

- fill or breach tiny pits below a configured area/depth threshold;
- preserve intended basins/wetlands only if named in config;
- carve channels with bounded maximum depth and width;
- preserve ridge/saddle readability;
- re-run slope and flow after carving;
- report how many cells were changed by conditioning.

## Derived Terrain Channels

### Required channels

The implementation should define these channels and write them to a manifest even if only downsampled versions are committed:

| Channel | Type | Purpose | Commit policy |
|---|---:|---|---|
| `height_m` | float32 source raster | canonical source elevation in meters | generated; usually ignored |
| `height_ue_u16` | uint16 raster / `.r16` | UE Landscape import artifact | generated; usually ignored |
| `land_mask` | bool/uint8 | island/ocean separation | generated; downsample preview committed |
| `coast_signed_distance_m` | float32 | inland/ocean distance; coast bands | generated; preview committed |
| `slope_deg` | float32 | traversal, cliffs, coast type, validation | generated; preview/stats committed |
| `aspect_deg` | float32 | slope orientation; canopy/sky/window systems later | generated; preview/stats committed |
| `flow_direction_d8` | int8/uint8 | hydrology validation | generated; not necessarily previewed |
| `flow_accumulation_cells` | uint32/float32 | drainage and creek candidates | generated; log preview committed |
| `basin_id` | int32 | drainage basins | generated; summary committed |
| `ridge_valley_index` | float32 | ridges, valleys, saddles, navigation landforms | generated; preview committed |
| `wetness_proxy` | float32 | riparian/wet valley candidates | generated; preview committed |
| `creek_influence_m` | float32 | distance/influence from valid creek lines | generated; preview committed |
| `coast_type` | uint8 enum | beach/rock/mangrove/mouth/cliff | generated; preview and counts committed |
| `review_point_id` | int32 sparse/table | ties samples to channels | committed as CSV/Markdown |

### Slope and aspect

Slope and aspect should be derived from the height source using a documented method. GDAL's `gdaldem` documentation defines terrain-analysis outputs including slope and aspect, with aspect represented as azimuth degrees where 0 degrees faces north, 90 east, 180 south, and 270 west for a north-oriented raster. The implementation can use GDAL directly, mirror its method, or document an equivalent internal derivative method. The chosen method must be stated in the manifest.

### Ridge/valley index

Use a multi-scale topographic position concept:

```text
ridge_valley_index = height_m - mean(height_m in radius R)
```

Compute it at two or three radii, for example 60 m, 180 m, and 540 m. Positive values indicate local ridges/high positions; negative values indicate valleys/depressions. The channel does not need to be perfect geomorphology; it needs to be stable and useful for validating ridge, spur, saddle, and valley readability.

### Wetness proxy

Use a topographic wetness-style proxy:

```text
wetness_proxy = ln((specific_catchment_area + epsilon) / tan(max(slope_rad, slope_floor_rad)))
```

The Topographic Wetness Index combines upslope contributing area and slope; for this game terrain source, the output should be labeled a **wetness proxy**, not a real hydrological model. Clip slope to a floor to avoid infinity on flats, and normalize the output by quantiles for masks.

### Creek influence

Creek influence should combine:

- distance to validated stream cells or centerlines;
- flow accumulation strength;
- elevation above local channel;
- valley bottom index;
- coast-mouth/alluvial widening near outlet.

Do not make creek influence a simple circular buffer everywhere. Valleys should be wider in lowland alluvial areas and tighter in gorge candidates.

## Review Point And Sample Strategy

### Required review artifacts

The implementation PR should produce small, reviewable artifacts rather than dumping huge rasters:

```text
Saved/TerrainSource/002/manifest.json                  # committed if small
Saved/TerrainSource/002/review_points.csv             # committed
Saved/TerrainSource/002/review_transects.csv          # committed
Saved/TerrainSource/002/channel_stats.md              # committed
Saved/TerrainSource/002/previews/height_2048.png      # committed if size acceptable
Saved/TerrainSource/002/previews/slope_2048.png       # committed if size acceptable
Saved/TerrainSource/002/previews/flow_log_2048.png    # committed if size acceptable
Saved/TerrainSource/002/previews/ridge_valley_2048.png# committed if size acceptable
Saved/TerrainSource/002/previews/coast_type_2048.png  # committed if size acceptable
Intermediate/TerrainSource/002/full/*.r16             # ignored
Intermediate/TerrainSource/002/full/*.npy             # ignored
Intermediate/TerrainSource/002/full/*.tif             # ignored unless policy says otherwise
```

If the repo prefers a different artifact directory, keep the same concept: source config in repo, full generated outputs ignored, compact review artifacts committed.

### Review point table

Create 80-160 named review points. Include at least:

- 12 coast samples across all coast types;
- 12 ridge/spur crest samples;
- 8 saddle/pass samples;
- 16 creek samples from headwater to mouth;
- 12 valley/alluvial/wetland samples;
- 8 gorge or steep-slope candidates;
- 8 interior navigation landmark samples;
- 8 random-but-seeded sanity samples.

CSV columns:

```text
point_id,x_m,y_m,grid_i,grid_j,height_m,slope_deg,aspect_deg,
coast_distance_m,coast_type,basin_id,flow_accumulation_cells,
wetness_proxy,ridge_valley_index,creek_influence_m,landform_class,
expected_review_reason
```

Example rows:

```csv
point_id,x_m,y_m,height_m,slope_deg,coast_type,landform_class,expected_review_reason
P_COAST_BEACH_S01,-4200,-5650,1.8,3.4,beach,coastal_flat,"low slope beach arc with inland vegetation transition candidate"
P_SADDLE_CENTRAL,120,260,430.0,9.8,none,saddle,"cross-spine pass; should be visible in ridge/valley preview"
P_CREEK_RED_HEAD,-1120,-820,392.0,14.2,none,headwater,"upper wet hollow begins connected descent"
P_CREEK_RED_MOUTH,-5150,-3740,0.7,2.1,creek_mouth,alluvial_mouth,"flow accumulation reaches sea with flat outlet"
```

### Review transects

Create at least six transects with 100-300 samples each:

1. windward coast to central peak;
2. leeward beach to ridge saddle;
3. creek head to creek mouth;
4. mangrove embayment through wet valley;
5. rocky/cliff coast over headland spur;
6. gorge candidate cross-section.

Transect CSV columns:

```text
transect_id,sample_index,x_m,y_m,distance_m,height_m,slope_deg,
coast_distance_m,flow_accumulation_cells,wetness_proxy,ridge_valley_index,notes
```

The review report should include concise interpretations:

- Does height rise from coast to ridge in a believable way?
- Does the saddle actually dip between high points?
- Does the creek flow downhill to the coast?
- Does the gorge candidate have steep walls and a narrow floor?
- Does the mangrove/wet area sit near sea level with high wetness and low slope?

### Navigation-readability validation

Because Jungle Game relies on diegetic environmental evidence, the terrain should support orientation through landforms. Developer review should verify:

- distinct ridge lines visible in height/slope/ridge previews;
- creek systems that lead from highland to coast;
- saddles that plausibly connect basins;
- coast types that create memorable perimeter variation;
- slope/aspect changes that can later drive vegetation and sky-window differences;
- no reliance on UI markers or player-facing navigation aids.

## Artifact And Source-Control Strategy

### Commit source, not bulk outputs

Commit:

- deterministic config file;
- schema/README for config fields;
- generator or validator source if implementation includes it;
- manifest with source hash, dimensions, channel list, output hash summary, and UE scale mapping;
- preview PNGs at reduced resolution if they are small;
- review CSVs and Markdown summaries;
- `.gitignore` entries for full generated rasters.

Usually ignore:

- full `.npy` arrays;
- full `.tif`/GeoTIFF intermediates;
- full `.r16` or raw UE import heightmaps;
- local editor import caches;
- temporary hydrology work rasters;
- machine-specific logs.

If a full import file must be shared for a review build, prefer an explicit artifact-release mechanism or Git LFS only if the repo already uses it. Do not silently add hundreds of megabytes of generated binary data to normal Git history.

### Manifest requirements

The manifest should include:

```json
{
  "terrain_source_id": "jungle_game_full_island_source_002",
  "schema_version": 1,
  "seed": 27002002,
  "config_sha256": "...",
  "generator_name": "...",
  "generator_version": "...",
  "vertices": [8129, 8129],
  "cell_size_m": 2.0,
  "extent_m": [16256.0, 16256.0],
  "height_min_m": -128.0,
  "height_max_m": 896.0,
  "sea_level_m": 0.0,
  "ue_xy_scale_cm": 200.0,
  "ue_z_scale": 200.0,
  "ue_landscape_z_offset_cm": 38400.0,
  "channels": {
    "height_m": {"dtype": "float32", "sha256": "...", "commit_policy": "ignored_full"},
    "height_ue_u16": {"dtype": "uint16", "sha256": "...", "commit_policy": "ignored_full"},
    "slope_deg": {"dtype": "float32", "sha256": "...", "commit_policy": "preview_committed"}
  },
  "validation_summary": {
    "passed": true,
    "basin_count": 0,
    "stream_cell_count": 0,
    "coast_type_percentages": {}
  }
}
```

The `basin_count`, `stream_cell_count`, and coast percentages are placeholders in the sketch; the implementation should fill them from actual generated data.

### Preview image rules

Preview images are developer artifacts. They should:

- be downsampled deterministically;
- include no player UI framing;
- be clearly labeled as validation previews;
- avoid massive dimensions unless needed;
- preserve enough structure for code review.

Recommended preview size: 2048 x 2048 maximum for normal PR review, plus optional 512 x 512 thumbnails in reports.

## Implementation-Sized PR Scope

A narrow implementation PR should target this scope:

1. Add a deterministic terrain source config for the 16 km-class island.
2. Add a schema/README explaining config fields and coordinate conventions.
3. Add or update generation/validation tooling sufficient to produce the required channels locally.
4. Produce compact review artifacts: manifest, channel stats, review points, transects, and previews.
5. Add `.gitignore` coverage for full generated rasters and UE import binaries.
6. Document UE import assumptions: 8129 x 8129, 2 m XY scale, Z scale 200, actor Z offset 38400 cm, sea level at world Z 0.
7. Add validation checks for dimensions, determinism, height range, coast boundary, channel existence, hydrology sanity, and artifact sizes.

Do not expand the PR into final biome painting, foliage population, player spawn logic, map UI, quest systems, or polished packaged gameplay. The output should make the full-size source terrain reviewable and ready for a subsequent import/runtime PR.

## Files And Areas Likely Touched

Exact paths depend on the repo layout, but a likely implementation layout is:

```text
Docs/Terrain/002_deterministic_full_size_terrain_source.md
SourceData/Terrain/002/terrain_source_002.yaml
SourceData/Terrain/002/terrain_source_schema.md
SourceData/Terrain/002/review_points.csv
SourceData/Terrain/002/review_transects.csv
SourceData/Terrain/002/channel_stats.md
SourceData/Terrain/002/manifest.json
SourceData/Terrain/002/previews/*.png
Tools/Terrain/generate_terrain_source.py      # if repo uses Python tools
Tools/Terrain/validate_terrain_source.py      # if separate validator is preferred
.gitignore
```

If the UE project already has a plugin/module for terrain contracts, the implementation may instead place generation/validation code under that tool area. Keep the source config and review artifacts easy to find.

Do not touch player HUD/UI files for this terrain-source PR. If a debug visualization actor or editor utility is needed, it must be editor-only and must not create player-facing navigation features.

## Acceptance Criteria

The implementation PR is acceptable when all of the following are true:

1. **Required scale exists**
   - Source config declares an `8129 x 8129` grid at `2 m` spacing, or a justified equivalent 16 km-class UE-compatible layout.
   - The generated source extent is approximately 16 km on each side.

2. **Deterministic source exists**
   - Terrain can be regenerated from committed human-readable source config and seed.
   - Manifest records config hash, generator version, dimensions, height range, UE scale mapping, and output channel hashes or tolerant validation summaries.

3. **Terrain structure is explicit**
   - Island outline, ocean boundary, mountain spine, ridges/spurs, saddles, creek heads, basins/valleys, and coast classification rules are encoded or derived from encoded controls.
   - The terrain is not driven by random noise-only generation.

4. **Required channels are defined**
   - Height, slope, aspect, flow, ridge/valley, wetness proxy, coast distance, and creek influence channels exist in the manifest.
   - Channel stats are produced and reviewed.

5. **Hydrology is plausible enough for source approval**
   - Main creek candidates flow downhill to coast outlets unless explicitly marked as an internal wetland exception.
   - Basin IDs and stream candidates are stable across regeneration.
   - Creek mouths align with coast classification.

6. **Review artifacts prove navigability/readability**
   - Review points cover coast, ridge, spur, saddle, creek, valley, alluvial, wetland, and gorge candidates.
   - Transects demonstrate coast-to-ridge rise, saddle dips, downstream creek descent, and coast type variation.
   - Preview images show meaningful landforms rather than homogeneous noise.

7. **Source control remains sane**
   - Full generated rasters and import binaries are ignored unless an explicit large-artifact policy exists.
   - Committed preview artifacts are small enough for normal PR review.

8. **UE import assumptions are documented**
   - UE Landscape size, XY scale, Z scale, actor Z offset, sea level, and import file format are recorded.
   - The implementation notes whether the import target is a new World Partition level, existing level subregion import, or source-only pre-import PR.

9. **Forbidden player-facing navigation remains absent**
   - No map/minimap/compass/bearing/GPS/objective marker/HUD time/orientation meter/player orientation comment is introduced or recommended.

## Risks And Stop Conditions

### Risks

- **Memory pressure:** `8129 x 8129` full-size rasters are large. Multiple float channels can consume several gigabytes during generation. Use streaming, chunking, memory-mapped arrays, or generate only required channels when needed.
- **False hydrology:** Procedural terrain can create attractive creek lines that do not actually descend. Validate flow after all carving and smoothing.
- **Over-smoothing:** Excessive smoothing can erase saddles, gullies, and ridge cues.
- **Over-noising:** Detail noise can break hydrology, create speckled coast masks, or make terrain arbitrary.
- **Binary creep:** Generated import files can bloat Git history.
- **UE scale mismatch:** Wrong XY scale, Z scale, or actor offset can produce a terrain that imports but does not match sea level or intended world size.
- **Premature production art:** Foliage/material work may hide terrain-source problems instead of solving them.

### Stop conditions

Stop and report rather than forcing the PR through if:

- the generator cannot reproduce stable dimensions and hashes/summaries from the same config;
- the height source is outside the declared range or clips major terrain unintentionally;
- domain edges are not ocean;
- the island touches the domain boundary unexpectedly;
- stream candidates do not reach plausible outlets or require uphill flow;
- the terrain reads as radial/noise blob rather than island with spine, ridges, valleys, and coast types;
- generated files are too large for repo policy and no ignore/artifact strategy exists;
- UE import scale cannot be reconciled with the source manifest;
- any player-facing forbidden navigation aid appears in the implementation scope.

## Rejected Or Forbidden Approaches

### Rejected: player-facing navigation systems

No player map, minimap, compass, bearing readout, objective marker, GPS marker, clock, explicit time display, or orientation meter should be recommended or created. The terrain should support navigation through diegetic environmental evidence.

### Rejected: random noise-only terrain

Noise may help add deterministic small-scale roughness, but it must not define the island outline, mountain spine, ridge network, creek structure, coast type, or review evidence. The terrain must be explainable through source parameters.

### Rejected: mandatory paid/proprietary terrain tools

Paid terrain tools may be useful in other projects, but this source path must be reproducible without requiring them. The source of truth should remain committed text/config and open/reproducible tooling.

### Rejected: huge unexplained binary dumps

A full-size `.r16` or GeoTIFF without source config, manifest, and validation evidence is not acceptable. Large generated outputs should be ignored or managed through an explicit artifact policy.

### Rejected: visual-only terrain approval

A nice height preview is not enough. Approval requires dimensions, source config, channel stats, hydrology sanity, review points, transects, and UE import mapping.

### Rejected: map-state confusion

This work concerns actual UE world/level/landscape state. It must not become a player-facing map UI feature.

## Sources And References

1. Epic Games, **Landscape Technical Guide in Unreal Engine 5.8 Documentation**. Used for UE Landscape component math, recommended Landscape sizes including `8129 x 8129`, heightmap formats, and Z scale formula.
   https://dev.epicgames.com/documentation/unreal-engine/landscape-technical-guide-in-unreal-engine

2. Epic Games, **Importing and Exporting Landscape Heightmaps in Unreal Engine 5.8 Documentation**. Used for UE heightmap import/export formats, tiled heightmap import, World Partition import settings, and import-scale fields.
   https://dev.epicgames.com/documentation/unreal-engine/importing-and-exporting-landscape-heightmaps-in-unreal-engine

3. Epic Games, **World Partition in Unreal Engine 5.8 Documentation**. Used for large-world level management, grid-cell streaming, Open World level defaults, and World Partition conversion/import context.
   https://dev.epicgames.com/documentation/unreal-engine/world-partition-in-unreal-engine

4. GDAL, **gdaldem — GDAL Documentation**. Used for terrain-analysis derivatives including slope and aspect, and for aspect convention details.
   https://gdal.org/en/stable/programs/gdaldem.html

5. GRASS GIS, **r.watershed Manual**. Used for flow accumulation, drainage direction, stream/basin generation, and threshold cautions for drainage detail.
   https://grass.osgeo.org/grass-stable/manuals/r.watershed.html

6. TauDEM, **D8 Contributing Area Tool Documentation**. Used for D8 contributing-area behavior where each cell contributes to downslope accumulation according to D8 flow directions.
   https://hydrology.usu.edu/taudem/taudem5/help53/D8ContributingArea.html

7. SAGA GIS, **Topographic Wetness Index Tool Documentation**. Used for the slope and specific catchment area basis of TWI and the Beven & Kirkby reference lineage.
   https://saga-gis.sourceforge.io/saga_tool_doc/7.1.1/ta_hydrology_20.html

8. Sørensen, R., Zinko, U., and Seibert, J. (2006), **On the calculation of the topographic wetness index**, Hydrology and Earth System Sciences. Used for the concept that TWI combines local upslope contributing area and slope and varies by contributing-area method.
   https://hess.copernicus.org/articles/10/101/2006/
