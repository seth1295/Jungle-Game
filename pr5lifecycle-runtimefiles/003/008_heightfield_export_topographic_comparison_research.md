# Heightfield Export And Topographic Comparison Research

## Executive Summary

Jungle Game needs a deterministic terrain-validation pipeline that proves what the runtime terrain actually is, not what the terrain generator was expected to create. The validation work should export authoritative height/mesh/mask data from the same terrain configuration used by the running UE5 project, produce topographic evidence artifacts, compare terrain iterations numerically and visually, and hard-gate major terrain PRs against coastline, relief, slope, hydrology, ridge/gully, crater, and artifact-hygiene criteria.

The current known validation baseline is approximately:

| Metric | Current observed value | Interpretation |
|---|---:|---|
| Minimum elevation | about -101 m | ocean/depression exists, but source/runtime provenance must be proven |
| Maximum elevation | about +140 m | far below the 1400 m volcanic island goal |
| Elevation relief | about 241 m | insufficient macro-relief for a volcanic island target |
| Slope range | about 0 to 7 degrees | terrain is visually/physically too flat for major volcanic terrain |

The key implementation goal is not merely to make screenshots. It is to create a repeatable developer-only evidence loop:

1. Run the terrain generator in a known map/config/seed.
2. Export the actual runtime mesh vertices and/or source heightfield samples.
3. Generate DEM-style maps and metrics from the exported data.
4. Compare the new run against a named baseline run.
5. Fail the PR if provenance, coastline, relief, hydrology, masks, or artifact hygiene are invalid.
6. Paste a compact evidence report into the PR while leaving generated maps, CSVs, logs, and rasters ignored and unstaged.

This spec intentionally avoids player-facing UI, art direction, ecology, survival mechanics, materials, minimaps, compasses, GPS, and navigation UI. It is a terrain engineering validation layer only.

## Terrain-Only Scope Boundary

This work is strictly limited to developer-only terrain export, topographic map generation, numeric validation, and terrain-iteration comparison.

Allowed:

- Runtime terrain vertex export.
- Source heightfield export.
- Terrain mask export.
- Topographic map generation.
- Coastline, beach, ocean, shelf, slope, relief, flow, catchment, ridge, gully, crater, and hazard-mask metrics.
- Metadata/provenance capture for terrain config and generator identity.
- Local/manual validation commands.
- CI-safe artifact hygiene checks.
- PR evidence report format.

Forbidden:

- Player UI.
- Minimap, compass, bearing, GPS, objective marker, time, or orientation UI.
- Final art, foliage, ecology, weather, audio, survival mechanics, NPCs, story, VFX, or material design.
- Terrain screenshots that are not backed by exported runtime/source data.
- Vibes-only ground inspection.
- Hand-edited validation CSVs/maps.
- Committing generated validation rasters, PNGs, CSVs, logs, or bulky evidence artifacts.

## Current Validation Baseline

The existing terrain was mapped after commit and appeared to produce roughly -101 m to +140 m elevation and 0 to 7 degree slopes. This is a useful sanity snapshot, but it is not sufficient for future decisions unless it is reproducible from an exact runtime/source export.

Treat the current baseline as a historical reference until a new provenance-complete baseline is generated.

Required baseline replacement:

```text
Saved/TerrainValidation/baselines/pr5_batch003_baseline_runtime/
  manifest.json
  height_runtime_m_f32.tif
  height_source_m_f32.tif              # if source heightfield exists
  vertices_runtime.csv.zst             # if runtime mesh exists
  triangles_runtime.csv.zst            # if runtime mesh exists
  masks_intended_island_u8.tif
  masks_ocean_u8.tif
  metrics.json
  report.md
```

Baseline replacement is complete only when `manifest.json` proves:

- exact git commit;
- dirty/clean state;
- map/package path;
- generator class/module;
- config asset path;
- config hash;
- terrain seed;
- UE version;
- platform;
- world origin/scale;
- sample spacing;
- sea level;
- island center/radius;
- beach ring parameters;
- export command line;
- UTC timestamp;
- output artifact hashes.

## Major Implementation Goal

Implement a Linux-first developer terrain-validation command that can run from the editor, commandlet, automation test, or packaged development build.

Recommended command surface:

```text
jg.Terrain.ExportValidation
  --map=/Game/Maps/JungleValidation
  --config=/Game/Terrain/Configs/IslandTerrainConfig
  --seed=123456
  --run-label=pr5_batch003_<shortsha>
  --compare-to=Saved/TerrainValidation/baselines/pr5_batch003_baseline_runtime/manifest.json
  --out=Saved/TerrainValidation/runs/pr5_batch003_<shortsha>/
```

Recommended Unreal-side shapes:

```text
UTerrainValidationSubsystem
FTerrainValidationExportCommandlet
FTerrainValidationManifest
FTerrainValidationHeightGrid
FTerrainValidationMeshSnapshot
FTerrainValidationMaskSnapshot
FTerrainValidationMetrics
```

Recommended external analysis surface:

```bash
python3 Tools/TerrainValidation/terrain_validate.py \
  --run Saved/TerrainValidation/runs/pr5_batch003_<shortsha>/manifest.json \
  --compare Saved/TerrainValidation/baselines/pr5_batch003_baseline_runtime/manifest.json \
  --write-maps \
  --write-report
```

The Unreal exporter should own source-of-truth capture. The Python/GDAL/GRASS layer should own map rendering, derived DEM metrics, comparison reports, and artifact hygiene checks.

## Actual Runtime Mesh / Source Heightfield Export Requirements

### Export priority order

Use the most authoritative data available, in this order:

1. **Generator source heightfield before mesh creation**: best source for exact height samples, masks, seed, and config-level decisions.
2. **Runtime mesh arrays immediately before section/component submission**: best proof of what the engine actually received for rendering/collision.
3. **UE Landscape height data through `FLandscapeEditDataInterface`**: required if the project uses Landscape actors/components.
4. **Static mesh / mesh-description extraction**: acceptable only for editor-authored terrain assets or generated static meshes.
5. **Rendered screenshot inspection**: forbidden as primary evidence; allowed only as supplementary visual context.

### Source heightfield export

If the terrain generator produces a grid before mesh creation, export it directly.

Required files:

```text
height_source_m_f32.tif
height_source_m_f32.npy
height_source_m_f32.csv.zst            # optional, for human/debug diffing
source_grid_metadata.json
```

Required grid fields:

| Field | Required value |
|---|---|
| `x_index` / `y_index` | integer sample index |
| `x_world_m` / `y_world_m` | world-space horizontal position in metres |
| `height_m` | elevation relative to sea level |
| `mask_island_intended` | 0/1 intended circular island mask |
| `mask_ocean_intended` | 0/1 intended ocean/outside mask |
| `mask_beach_band` | 0/1 intended beach/coastal shelf band |
| `mask_crater` | 0/1 if crater/hazard mask is generated |
| `source_layer_tags` | optional bitfield/string tags for generator layer attribution |

Source grid export rules:

- Export in metres, not Unreal centimetres.
- Store sea level as `0.0 m`.
- Preserve native resolution first; derive analysis/downsampled grids second.
- Use `Float32` for height rasters.
- Use `UInt8` for masks.
- Store `nodata` only for unavailable samples; ocean is real data, not nodata.
- Hash the raw height array bytes and include the hash in `manifest.json`.

### Runtime mesh export

If the terrain is built through a generated mesh component, export the exact arrays passed to the runtime mesh/procedural mesh creation path.

Required files:

```text
vertices_runtime.csv.zst
triangles_runtime.csv.zst
mesh_sections_runtime.json
height_runtime_resampled_m_f32.tif
runtime_mesh_metadata.json
```

`vertices_runtime.csv.zst` schema:

```csv
vertex_id,section_id,x_cm,y_cm,z_cm,x_m,y_m,z_m,u,v,nx,ny,nz,source_grid_x,source_grid_y,mask_bits
```

`triangles_runtime.csv.zst` schema:

```csv
triangle_id,section_id,i0,i1,i2,material_slot,area_m2,slope_deg,aspect_deg,centroid_x_m,centroid_y_m,centroid_z_m
```

Runtime mesh export rules:

- Export immediately before the component/section is created or updated.
- Do not scrape only post-render buffers if the generator arrays are available.
- Preserve section IDs and triangle indices.
- Convert Unreal centimetres to metres in additional columns; do not lose the original centimetre values.
- Include world transform and local transform in metadata.
- Include vertex and triangle counts per section.
- Include a bounding box in centimetres and metres.
- Include collision mesh identity if collision differs from render mesh.
- Fail if runtime mesh extents disagree with source heightfield extents by more than one grid cell.

### UE Landscape export path

If the project uses Unreal Landscape, use the Landscape editing/runtime access layer rather than screenshots. Epic documents valid Landscape heightmap sizing as one more vertex than the number of quads and supports heightmap import formats including 16-bit grayscale PNG, r8, and r16. Landscape height values are stored with 16-bit precision and scaled into world height by the Landscape Z scale. `FLandscapeEditDataInterface` exposes height and weight data accessors such as `GetHeightData`, `GetHeightDataFast`, and `GetWeightData`, which makes it the appropriate source for height/mask extraction when Landscape is the terrain representation. [UE-Landscape] [UE-LandscapeEdit]

Landscape export rules:

- Export `uint16` raw height values and converted metres.
- Export component bounds and component coordinates.
- Export Landscape actor transform and scale.
- Export layer/weight masks relevant to island/ocean/beach/crater if they exist.
- Preserve exact Landscape dimensions: vertices = quads + 1.
- Fail if converted metres cannot be reproduced from raw height + scale metadata.

### Static mesh / MeshDescription export path

If terrain is converted to a static mesh or editor asset, use MeshDescription/editor mesh data when available. Epic documents `FMeshDescription` as the data structure containing mesh topology operations and a bounding-box computation, while `UMeshDescriptionBase::Vertices()` exposes the vertex array. [UE-MeshDescription] [UE-MeshDescriptionVertices]

Static mesh export rules:

- Use asset mesh description in editor validation.
- Use runtime arrays for generated meshes wherever possible.
- Export LOD identity; terrain validation must use the highest/source LOD unless explicitly validating LOD simplification.
- Fail if the analyzed LOD is not recorded.

### Runtime/source parity checks

When both source grid and runtime mesh exist, calculate:

| Check | Pass criterion |
|---|---:|
| Runtime vertex count vs expected grid count | exact match for regular grid, or documented triangulation/masking reason |
| Runtime height sampled back to source grid | RMSE <= 0.10 m for direct mesh-from-grid terrain |
| Runtime XY bounds vs source bounds | <= 1 cell spacing mismatch |
| Runtime Z min/max vs source min/max | <= 0.10 m mismatch unless mesh simplification is documented |
| Runtime masks vs source masks | IoU >= 0.995 for binary masks after rasterization |
| Runtime triangle winding | no inverted normals for terrain-facing triangles |

## Required Topographic Map Outputs

The validation run must produce machine-readable rasters and developer-readable PNGs. PNG maps are evidence views; rasters/JSON are the authoritative data.

Recommended directory layout:

```text
Saved/TerrainValidation/runs/<run_id>/
  manifest.json
  metrics.json
  report.md
  data/
    height_source_m_f32.tif
    height_runtime_m_f32.tif
    vertices_runtime.csv.zst
    triangles_runtime.csv.zst
    masks_intended_island_u8.tif
    masks_measured_land_u8.tif
    masks_ocean_u8.tif
    masks_beach_band_u8.tif
    masks_crater_u8.tif
  maps/
    elevation_color_4096.png
    hillshade_4096.png
    elevation_hillshade_overlay_4096.png
    contours_minor25_major100_4096.png
    contour_zero_beach_ring_4096.png
    slope_degrees_4096.png
    aspect_4096.png
    island_mask_overlay_4096.png
    beach_shelf_band_4096.png
    ocean_mask_edge_artifacts_4096.png
    flow_accumulation_log_4096.png
    catchments_4096.png
    ridge_gully_overlay_4096.png
    crater_hazard_mask_4096.png
    diff_height_vs_baseline_4096.png
    diff_contour_vs_baseline_4096.png
  profiles/
    profile_ns.csv
    profile_ew.csv
    profile_radial_000.csv
    profile_radial_045.csv
    profile_radial_090.csv
    profile_radial_135.csv
    profile_radial_180.csv
    profile_radial_225.csv
    profile_radial_270.csv
    profile_radial_315.csv
```

### Map resolution tiers

| Tier | Resolution | Purpose | Requirement |
|---|---:|---|---|
| Native | source/native | authoritative measurements | always export when source grid exists |
| Evidence | 4096 x 4096 or native if smaller | PR evidence maps | required for major terrain PRs |
| Fast | 2048 x 2048 | local iteration | recommended default |
| Preview | 1024 x 1024 | quick debug | optional |

Use a fixed world extent per terrain config. Comparisons are invalid if extent or resolution changes without explicit resampling metadata.

### Elevation map

Required outputs:

```text
height_runtime_m_f32.tif
height_source_m_f32.tif
maps/elevation_color_4096.png
```

Required features:

- Sea level fixed at 0 m.
- Ocean/depth color interval separate from land interval.
- Elevation ticks: -200, -100, -50, -20, -10, 0, 25, 50, 100, 250, 500, 750, 1000, 1250, 1400 m.
- Min/max labels in report metadata, not as player-facing UI.
- No smoothing unless the smoothing kernel and source map are both preserved.

### Hillshade map

Required outputs:

```text
maps/hillshade_4096.png
maps/elevation_hillshade_overlay_4096.png
```

Required settings:

| Setting | Value |
|---|---:|
| Azimuth | 315 degrees |
| Altitude | 45 degrees |
| Z/horizontal scale | 1.0 when all units are metres |
| Secondary QA azimuths | 45 and 225 degrees optional for artifact hunting |

GDAL's `gdaldem` provides hillshade, slope, aspect, color-relief, TRI, TPI, and roughness operations, and its documentation explicitly calls out x/y/z unit scale handling for hillshade and slope. Because Jungle Game uses local metric game coordinates, analysis should write metre-based rasters and use scale 1.0. [GDAL-gdaldem]

Example:

```bash
gdaldem hillshade data/height_runtime_m_f32.tif maps/hillshade_4096.tif \
  -az 315 -alt 45 -s 1.0 -of GTiff
```

### Contour map

Required outputs:

```text
maps/contours_minor25_major100_4096.png
maps/contour_zero_beach_ring_4096.png
vectors/contours_minor25.gpkg
vectors/contours_major100.gpkg
vectors/contour_zero.gpkg
```

Contour intervals:

| Contour type | Levels |
|---|---|
| Shoreline | exactly 0 m |
| Beach/shelf QA | -20, -10, -5, -2, 0, +2, +5, +10, +20 m |
| Minor land contours | every 25 m |
| Major land contours | every 100 m |
| Volcanic summit QA | 1000, 1200, 1300, 1400 m |

GDAL's `gdal_contour` generates vector contour lines from an input elevation raster and supports fixed intervals and explicit levels. Use it to create the 0 m coastline contour separately from general contours so beach-ring validation cannot be hidden by styling. [GDAL-contour]

Example:

```bash
gdal_contour -fl 0 -a elev_m data/height_runtime_m_f32.tif vectors/contour_zero.gpkg

gdal_contour -i 25 -a elev_m data/height_runtime_m_f32.tif vectors/contours_minor25.gpkg

gdal_contour -i 100 -a elev_m data/height_runtime_m_f32.tif vectors/contours_major100.gpkg
```

### Slope map

Required outputs:

```text
slope_degrees_f32.tif
maps/slope_degrees_4096.png
```

Required slope classes:

| Class | Degrees | Meaning |
|---|---:|---|
| Flat | 0-2 | water/flat shelf/plateau candidate |
| Gentle | 2-8 | beach, shelf, walkable lowland |
| Rolling | 8-15 | foothills |
| Steep | 15-30 | mountain slopes/gullies |
| Very steep | 30-45 | cliffs/hazard terrain candidate |
| Extreme | >45 | likely cliff/artifact unless intentionally masked |

Required metrics:

- mean slope inside island;
- median slope inside island;
- p90/p95/p99 slope inside island;
- max slope inside island;
- percent of land in each slope class;
- beach-band median and p95 slope;
- square-edge slope artifacts.

### Aspect map

Required outputs:

```text
aspect_degrees_f32.tif
maps/aspect_4096.png
```

Rules:

- Aspect values 0-360 degrees.
- Cells with slope < 1 degree should be `nodata` or classified as flat.
- Report aspect distribution in 8 compass bins for terrain balance only; do not turn this into player navigation UI.

### Island mask map

Required outputs:

```text
masks_intended_island_u8.tif
masks_measured_land_u8.tif
maps/island_mask_overlay_4096.png
```

Definitions:

```text
intended_island_mask = distance_from_island_center <= intended_island_radius
measured_land_mask   = height_m >= 0
outside_island_mask  = not intended_island_mask
```

Required metrics:

- intended island area;
- measured land area;
- intended/measured mask IoU;
- false land outside intended island;
- false ocean inside intended island;
- connected component count for land;
- island-ring circularity.

### Beach ring and ocean mask maps

Required outputs:

```text
masks_beach_band_u8.tif
masks_ocean_u8.tif
maps/beach_shelf_band_4096.png
maps/ocean_mask_edge_artifacts_4096.png
```

Beach/coastal shelf definitions should be explicit in config:

```json
{
  "sea_level_m": 0.0,
  "beach_elevation_min_m": -2.0,
  "beach_elevation_max_m": 5.0,
  "coastal_shelf_min_m": -20.0,
  "coastal_shelf_max_m": 20.0,
  "beach_ring_expected_radius_m": 3000.0,
  "beach_ring_allowed_median_error_m": 10.0,
  "beach_ring_allowed_p95_error_m": 30.0
}
```

### Catchment and flow maps

Required outputs:

```text
flow_direction_d8_u8.tif
flow_accumulation_f32.tif
catchments_u16.tif
maps/flow_accumulation_log_4096.png
maps/catchments_4096.png
```

Hydrology outputs should be derived from a copy of the heightfield. If sinks are filled/breached for flow analysis, preserve:

```text
height_runtime_filled_for_flow_m_f32.tif
fill_depth_m_f32.tif
```

GRASS `r.watershed` generates flow accumulation, drainage direction, stream locations, and watershed basins, making it a strong reference implementation for validating the project's local hydrology-style terrain metrics. [GRASS-watershed]

Required metrics:

- number of catchments;
- catchment area distribution;
- flow outlet count at coast;
- percent of flow paths terminating at ocean;
- internal pit count and total filled volume;
- stream/gully length above accumulation threshold;
- drainage density: stream length / land area;
- radial drainage alignment for volcanic island shape.

### Ridge/gully map

Required outputs:

```text
ridge_candidates_u8.tif
gully_candidates_u8.tif
curvature_profile_f32.tif
curvature_plan_f32.tif
tpi_f32.tif
maps/ridge_gully_overlay_4096.png
```

Recommended derivation:

- gullies: high flow accumulation + negative profile/plan curvature + local topographic low;
- ridges: high positive TPI + low local flow accumulation + convex curvature;
- stream/gully candidate threshold: top 2-5% of log flow accumulation inside island;
- ridge candidate threshold: top 5-10% of positive TPI inside island after excluding beach band.

Required metrics:

- ridge length;
- gully length;
- ridge/gully density per km²;
- percent of gullies reaching ocean or beach band;
- percent of ridges terminating before/at beach band rather than punching into ocean artifacts;
- average radial alignment from summit/crater center;
- coastline termination coherence.

### Crater/hazard map

Required outputs:

```text
masks_crater_u8.tif
masks_hazard_steep_u8.tif
maps/crater_hazard_mask_4096.png
```

Required only if the terrain config includes a crater/hazard feature.

Crater metrics:

- crater mask area;
- crater centroid;
- crater rim mean elevation;
- crater floor mean elevation;
- rim-floor relief;
- rim continuity percent;
- crater circularity/ellipse ratio;
- crater slope p95;
- overlap with steep/hazard mask;
- drainage behavior: closed crater, breached crater, or outward-draining crater.

## Sea-Level Beach Ring Validation

The circular island edge is a hard validation target.

The workflow must explicitly test:

```text
0 m contour must track the intended beach ring
outside island mask must be ocean/below sea level
beach/coastal shelf band must be measurable
square-world edge must not contain raised terrain artifacts
ridge/gully systems must terminate coherently at coast
```

### Definitions

| Symbol | Definition |
|---|---|
| `C` | intended island center in world XY metres |
| `R` | intended beach-ring radius in metres |
| `d(x,y)` | distance from `C` |
| `h(x,y)` | height in metres relative to sea level |
| `coast_intended` | `abs(d(x,y) - R) <= ring_tolerance_m` |
| `land_measured` | `h(x,y) >= 0` |
| `ocean_required` | `d(x,y) > R + outside_margin_m` |

### Coastline contour tracking

Extract the 0 m contour and compare it to the intended circular ring.

Required numeric checks:

| Check | Pass criterion | Fail meaning |
|---|---:|---|
| 0 m contour exists | yes | no measurable shoreline |
| Largest 0 m contour closed | yes | broken coastline |
| Median radial error from intended ring | <= 10 m | beach ring drift |
| p95 radial error | <= 30 m | local coastline spikes/notches |
| Max radial error | <= 75 m unless intentionally configured | severe coastal artifact |
| 0 m contour coverage around 360 degrees | >= 99% angular bins present | coastline gap |
| Measured land connected components | exactly 1 major component | stray islands or split island |
| False land outside intended island | <= 0.1% outside area | raised ocean/square-edge artifact |

### Outside island ocean/below sea-level validation

All terrain outside the intended island plus margin must remain ocean or below sea level.

Pass criteria:

| Metric | Pass criterion |
|---|---:|
| Outside-mask p99 height | <= -0.5 m |
| Outside-mask max height | <= +0.25 m, or <= +1.0 m only for documented transition cells |
| Outside cells above 0 m | <= 0.05% of outside area |
| Square-edge margin p99 height | <= -5.0 m |
| Raised connected components touching square edge | 0 |
| Edge artifact slope spikes | no contiguous >15 degree ridge along world boundary |

Recommended edge margin:

```text
edge_margin_m = max(100 m, 2 * native_cell_size_m)
```

### Beach/coastal shelf band validation

The beach and coastal shelf must be measurable, not just visually implied.

Recommended pass criteria:

| Metric | Pass criterion |
|---|---:|
| Beach band area | >= 2% and <= 12% of intended island area unless config says otherwise |
| Beach elevation band | -2 m to +5 m |
| Coastal shelf elevation band | -20 m to +20 m |
| Beach median slope | <= 5 degrees |
| Beach p95 slope | <= 10 degrees |
| Beach radial continuity | >= 98% angular coverage |
| Shelf width median | config target ± 20% |
| Shelf width p95 | config target ± 40% |

### Ridge/gully coastal termination validation

Ridges and gullies should terminate coherently at the coast, not produce square-edge spikes or underwater raised seams.

Pass criteria:

| Metric | Pass criterion |
|---|---:|
| Gully outlets entering beach/ocean | >= 80% of major gullies |
| Major gullies ending in inland pits | <= 10% unless crater config intentionally closes drainage |
| Ridge candidates crossing outside island mask | <= 1% of ridge length |
| Ridge/gully edge contact with square-world boundary | 0 major features |
| Flow outlets on 0 m contour/beach band | >= 90% of major drainage outlets |

## Elevation / Relief / Slope Metrics

### Required elevation metrics

`metrics.json` must include:

```json
{
  "elevation_m": {
    "min": -101.0,
    "p01": -80.0,
    "p05": -50.0,
    "mean": 0.0,
    "median": 0.0,
    "p95": 500.0,
    "p99": 1100.0,
    "max": 1400.0,
    "relief": 1501.0,
    "land_area_m2": 0.0,
    "ocean_area_m2": 0.0,
    "volume_above_sea_m3": 0.0,
    "volume_below_sea_m3": 0.0
  }
}
```

Minimum required metrics:

| Metric | Why it matters |
|---|---|
| min / max / relief | proves macro scale |
| p01 / p05 / p50 / p95 / p99 | prevents single-pixel mountain fraud |
| volume above sea | proves mass/landform change |
| land area | checks island mask vs sea-level reality |
| hypsometric curve | shows terrain distribution from coast to summit |
| elevation entropy / bin distribution | detects flat shelves or overdominant plateaus |

### Meaningful-change thresholds toward 1400 m volcanic island goal

The current observed +140 m maximum is about 10% of the 1400 m target. A major terrain implementation PR should be considered meaningful only if it passes at least one macro-relief threshold and all coastline/artifact gates.

#### Major PR meaningful-change gate

Pass if all hard gates pass and at least **two** of these are true compared with the previous accepted terrain baseline:

| Metric | Meaningful-change threshold |
|---|---:|
| Max elevation increase | >= +250 m |
| Relief increase | >= +300 m |
| p95 land elevation increase | >= +150 m |
| Volume above sea increase | >= +20% |
| Land area change | within config target ±5% while relief increases |
| p95 slope increase | >= +4 degrees, but p99 slope remains <= 55 degrees unless cliffs are masked |
| Terrain changed area | >= 20% of intended island area has abs(height_delta) >= 25 m |
| Summit/ridge system introduced | ridge length density increases >= 50% without square-edge artifacts |

#### Goal-ready volcanic island gate

A terrain iteration can claim it is approaching the 1400 m volcanic island goal only when:

| Metric | Goal-ready criterion |
|---|---:|
| Maximum elevation | 1300-1450 m, or config-documented target band |
| Relief | >= 1300 m |
| p99 land elevation | >= 900 m |
| p95 land elevation | >= 500 m |
| Land slope p95 | 15-45 degrees, depending on playable traversal goals |
| Beach slope p95 | <= 10 degrees |
| Ocean outside island | p99 <= -0.5 m |
| 0 m coastline ring | median radial error <= 10 m; p95 <= 30 m |
| Square-edge raised artifacts | 0 connected raised artifacts touching boundary |
| Major gullies | >= 80% terminate at beach/ocean or configured crater basin |

#### Hard fail conditions

Fail the terrain PR regardless of visual quality if any are true:

- `manifest.json` is missing or dirty state is unknown.
- Runtime/source export cannot be tied to exact config hash.
- 0 m contour is missing or not closed.
- Outside island/ocean mask contains significant raised land.
- Square-world edge contains raised terrain artifacts.
- Generated maps/CSVs/logs are staged.
- Comparison baseline is absent for a major terrain PR.
- Height units are ambiguous or mix centimetres/metres without metadata.
- Slope/hillshade generated with wrong z/horizontal scale.

### Required slope metrics

`metrics.json` must include:

```json
{
  "slope_deg": {
    "inside_island_mean": 0.0,
    "inside_island_median": 0.0,
    "inside_island_p90": 0.0,
    "inside_island_p95": 0.0,
    "inside_island_p99": 0.0,
    "inside_island_max": 0.0,
    "beach_median": 0.0,
    "beach_p95": 0.0,
    "percent_flat_0_2": 0.0,
    "percent_gentle_2_8": 0.0,
    "percent_rolling_8_15": 0.0,
    "percent_steep_15_30": 0.0,
    "percent_very_steep_30_45": 0.0,
    "percent_extreme_gt45": 0.0
  }
}
```

## Ridge / Gully / Catchment Metrics

### Flow/catchment metrics

Required `metrics.json` block:

```json
{
  "hydrology": {
    "algorithm": "D8",
    "filled_for_analysis": true,
    "total_fill_volume_m3": 0.0,
    "max_fill_depth_m": 0.0,
    "internal_pit_count": 0,
    "catchment_count": 0,
    "major_catchment_count": 0,
    "major_outlet_count": 0,
    "major_outlets_reaching_ocean_pct": 0.0,
    "stream_length_m": 0.0,
    "drainage_density_m_per_km2": 0.0,
    "radial_flow_alignment_mean": 0.0
  }
}
```

Recommended catchment thresholds:

| Metric | Pass criterion |
|---|---:|
| Major catchments | 6-24 for a medium/large volcanic island unless config says otherwise |
| Major outlets reaching beach/ocean | >= 90% |
| Internal pits excluding crater | <= 3 small pits or <= 0.1% land area |
| Total fill volume for flow analysis | <= 1% of terrain above-sea volume |
| Max fill depth outside crater | <= 5 m |
| Drainage density | non-zero and stable vs baseline; no hard absolute until island scale is fixed |

### Ridge/gully metrics

Required `metrics.json` block:

```json
{
  "ridge_gully": {
    "ridge_length_m": 0.0,
    "gully_length_m": 0.0,
    "ridge_density_m_per_km2": 0.0,
    "gully_density_m_per_km2": 0.0,
    "gully_to_ocean_termination_pct": 0.0,
    "ridge_crosses_outside_mask_pct": 0.0,
    "square_edge_feature_count": 0,
    "mean_radial_alignment_deg_error": 0.0
  }
}
```

Recommended pass criteria:

| Metric | Pass criterion |
|---|---:|
| Ridge length | increases materially vs flat baseline for mountain terrain PR |
| Gully length | increases materially vs flat baseline for erosion/drainage PR |
| Gully ocean termination | >= 80% |
| Ridge outside-mask crossing | <= 1% |
| Square-edge feature count | 0 |
| Radial alignment | report only until volcano morphology target is finalized |

### Practical algorithms

Recommended implementation options:

- D8 flow direction and accumulation for deterministic first implementation.
- Optional D∞/MFD later if D8 artifacts dominate.
- TPI at multiple radii for ridge/valley candidates.
- Profile/plan curvature for convex ridge and concave gully detection.
- Connected-component analysis for ridges, gullies, raised ocean artifacts, and stray islands.

Do not block PR5 Batch 003 on perfect geomorphology. Block it on deterministic export, reproducible metrics, coastline validation, and obvious artifact detection.

## Crater / Hazard Mask Metrics

Crater/hazard validation is required when the terrain config claims to generate a crater, volcanic summit, caldera, lava-risk zone, or steep hazard area.

Required `metrics.json` block:

```json
{
  "crater_hazard": {
    "enabled": true,
    "crater_area_m2": 0.0,
    "crater_centroid_x_m": 0.0,
    "crater_centroid_y_m": 0.0,
    "crater_floor_mean_m": 0.0,
    "crater_rim_mean_m": 0.0,
    "rim_floor_relief_m": 0.0,
    "rim_continuity_pct": 0.0,
    "crater_circularity": 0.0,
    "hazard_steep_area_m2": 0.0,
    "hazard_steep_area_pct_land": 0.0,
    "hazard_mask_overlaps_beach_pct": 0.0
  }
}
```

Pass criteria for a crater-enabled volcanic terrain PR:

| Metric | Pass criterion |
|---|---:|
| Crater mask exists | true |
| Crater centroid | within configured summit/crater tolerance |
| Rim continuity | >= 80% unless breached crater is configured |
| Rim-floor relief | >= configured minimum; recommend >= 50 m for visible macro crater |
| Hazard overlap with beach | <= 1% unless intentionally configured |
| Crater square-edge contact | 0 |
| Crater outside island mask overlap | 0 |

Stop condition:

- If crater/hazard is not in the current terrain config, report `crater_hazard.enabled=false` and do not invent crater requirements for that PR.

## Iteration Comparison Workflow

### Comparison identity

Every comparison must use two manifests:

```text
--base Saved/TerrainValidation/baselines/<baseline>/manifest.json
--head Saved/TerrainValidation/runs/<run_id>/manifest.json
```

Comparison is valid only when:

- both runs use the same terrain coordinate frame;
- both runs record sea level;
- both runs record world extent;
- both runs record sample spacing/resolution;
- both runs record generator/config identity;
- resampling method is recorded if dimensions differ.

### Numeric comparison outputs

Required file:

```text
comparison_vs_<baseline_run_id>.json
```

Required fields:

```json
{
  "base_run_id": "baseline",
  "head_run_id": "head",
  "height_delta_m": {
    "min": 0.0,
    "max": 0.0,
    "mean": 0.0,
    "median": 0.0,
    "mae": 0.0,
    "rmse": 0.0,
    "p95_abs": 0.0,
    "changed_area_abs_gt_5m_pct_land": 0.0,
    "changed_area_abs_gt_25m_pct_land": 0.0,
    "changed_area_abs_gt_100m_pct_land": 0.0
  },
  "coastline_delta": {
    "base_zero_contour_length_m": 0.0,
    "head_zero_contour_length_m": 0.0,
    "median_radial_error_change_m": 0.0,
    "p95_radial_error_change_m": 0.0,
    "land_area_change_pct": 0.0,
    "false_land_outside_change_pct": 0.0
  },
  "slope_delta": {
    "mean_change_deg": 0.0,
    "p95_change_deg": 0.0,
    "extreme_gt45_area_change_pct": 0.0
  },
  "hydrology_delta": {
    "major_catchment_count_change": 0,
    "gully_length_change_pct": 0.0,
    "outlet_to_ocean_pct_change": 0.0
  }
}
```

### Visual comparison outputs

Required maps:

```text
maps/diff_height_vs_baseline_4096.png
maps/diff_abs_height_vs_baseline_4096.png
maps/diff_contour_vs_baseline_4096.png
maps/diff_slope_vs_baseline_4096.png
maps/diff_coastline_error_vs_baseline_4096.png
maps/base_head_elevation_side_by_side_4096.png
maps/base_head_hillshade_side_by_side_4096.png
maps/base_head_slope_side_by_side_4096.png
```

Visual rules:

- Use fixed scales across base/head/diff.
- Do not auto-stretch each map independently for PR evidence.
- Use the same world extent and origin.
- Overlay previous/current 0 m contours in the coastline comparison.
- Include an explicit no-data/invalid-data color if needed.
- Store map rendering settings in `map_render_settings.json`.

### Profile comparison outputs

Required profile cuts:

- north-south through island center;
- east-west through island center;
- 8 radial spokes at 45 degree increments;
- optional crater/summit transect if crater is enabled.

Required profile metrics:

- max elevation along profile;
- beach crossing count;
- distance from intended coast to measured 0 m crossing;
- relief from coast to summit;
- slope p95 along profile;
- ridge/gully intersections along profile.

### Resampling policy

If base/head rasters differ in resolution:

| Data type | Resampling method |
|---|---|
| Height | bilinear for metrics, cubic only for visual maps if recorded |
| Slope/aspect | regenerate from resampled height, do not resample final slope unless only visual |
| Binary masks | nearest-neighbor |
| Labels/catchments | nearest-neighbor |
| Flow accumulation | regenerate after height resampling |
| Contours | regenerate from comparison grid |

Comparison must report:

```json
{
  "resampled": true,
  "resample_reason": "base/head dimensions differed",
  "base_original_resolution": [2049, 2049],
  "head_original_resolution": [4097, 4097],
  "comparison_resolution": [4097, 4097],
  "height_resampling": "bilinear",
  "mask_resampling": "nearest"
}
```

## Runtime Metadata And Log Tokens

### Manifest file

Every run must write:

```text
manifest.json
```

Required schema:

```json
{
  "schema_version": 1,
  "run_id": "pr5_batch003_abcdef1_seed123456",
  "run_label": "pr5_batch003_abcdef1",
  "created_utc": "2026-06-26T00:00:00Z",
  "project": "JungleGame",
  "platform": "Linux",
  "ue_version": "5.x",
  "build_config": "Development Editor",
  "git": {
    "commit": "abcdef1234567890",
    "branch": "feature/pr5-batch003-terrain-validation",
    "dirty": false,
    "tracked_diff_hash": "sha256:..."
  },
  "map": {
    "package": "/Game/Maps/JungleValidation",
    "world_partition": false,
    "world_origin_cm": [0.0, 0.0, 0.0]
  },
  "terrain": {
    "generator_class": "UJungleIslandTerrainGenerator",
    "generator_module": "JungleGame",
    "config_asset": "/Game/Terrain/Configs/IslandTerrainConfig",
    "config_export_json_sha256": "sha256:...",
    "seed": 123456,
    "source_type": "source_heightfield_and_runtime_mesh",
    "sea_level_m": 0.0,
    "island_center_m": [0.0, 0.0],
    "island_radius_m": 3000.0,
    "beach_band_m": [-2.0, 5.0],
    "coastal_shelf_band_m": [-20.0, 20.0],
    "target_peak_elevation_m": 1400.0
  },
  "grid": {
    "width": 4097,
    "height": 4097,
    "cell_size_m": 1.0,
    "extent_min_m": [-2048.0, -2048.0],
    "extent_max_m": [2048.0, 2048.0],
    "height_units": "metres_relative_to_sea_level",
    "horizontal_units": "metres"
  },
  "mesh": {
    "vertex_count": 0,
    "triangle_count": 0,
    "section_count": 0,
    "bounds_cm": {
      "min": [0.0, 0.0, 0.0],
      "max": [0.0, 0.0, 0.0]
    }
  },
  "artifacts": {
    "height_runtime_m_f32_tif_sha256": "sha256:...",
    "metrics_json_sha256": "sha256:...",
    "report_md_sha256": "sha256:..."
  },
  "commands": {
    "unreal_export": "...",
    "analysis": "..."
  }
}
```

### Log tokens

The exporter and analyzer must print grep-friendly tokens.

Required Unreal log tokens:

```text
JG_TERRAIN_EXPORT_BEGIN run_id=<run_id>
JG_TERRAIN_EXPORT_CONFIG config=<path> seed=<seed> config_sha256=<hash>
JG_TERRAIN_EXPORT_GRID width=<w> height=<h> cell_size_m=<m> sea_level_m=0
JG_TERRAIN_EXPORT_MESH vertices=<n> triangles=<n> sections=<n>
JG_TERRAIN_EXPORT_HEIGHT min_m=<min> max_m=<max> relief_m=<relief>
JG_TERRAIN_EXPORT_COAST median_error_m=<m> p95_error_m=<m> false_land_outside_pct=<pct>
JG_TERRAIN_EXPORT_ARTIFACTS out=<path>
JG_TERRAIN_EXPORT_PASS run_id=<run_id>
JG_TERRAIN_EXPORT_FAIL reason=<reason>
```

Required analyzer log tokens:

```text
JG_TERRAIN_ANALYSIS_BEGIN run_id=<run_id>
JG_TERRAIN_ANALYSIS_MAPS written=<n>
JG_TERRAIN_ANALYSIS_COMPARE base=<base_run_id> head=<head_run_id>
JG_TERRAIN_ANALYSIS_METRIC name=<metric> value=<value> threshold=<threshold> result=<PASS|FAIL>
JG_TERRAIN_ANALYSIS_ARTIFACT_HYGIENE result=<PASS|FAIL>
JG_TERRAIN_ANALYSIS_PASS run_id=<run_id>
JG_TERRAIN_ANALYSIS_FAIL reason=<reason>
```

### Provenance pass/fail

Fail validation if any required manifest field is missing:

- `git.commit`;
- `git.dirty`;
- `map.package`;
- `terrain.generator_class`;
- `terrain.config_asset`;
- `terrain.config_export_json_sha256`;
- `terrain.seed`;
- `terrain.sea_level_m`;
- `terrain.island_center_m`;
- `terrain.island_radius_m`;
- `grid.width`, `grid.height`, `grid.cell_size_m`;
- artifact hashes.

## Artifact Hygiene Rules

Generated validation artifacts must remain ignored and unstaged. Git's official documentation defines `.gitignore` as the mechanism for intentionally untracked files; files already tracked are not affected, so validation artifacts must never be introduced as tracked files in the first place. GitHub's documentation likewise recommends committing repository-level `.gitignore` rules to share ignored-file behavior with collaborators. [Git-gitignore] [GitHub-ignore]

### Required `.gitignore` additions

Add or verify:

```gitignore
# Jungle Game terrain validation generated artifacts
/Saved/TerrainValidation/
/Intermediate/TerrainValidation/
/DerivedDataCache/TerrainValidation/
/Artifacts/TerrainValidation/

# Terrain validation heavyweight/generated evidence
*.terrain-validation.tif
*.terrain-validation.tiff
*.terrain-validation.png
*.terrain-validation.csv
*.terrain-validation.csv.zst
*.terrain-validation.gpkg
*.terrain-validation.geojson
*.terrain-validation.jsonl
```

Prefer directory ignores over broad extension ignores. The broad artifact suffixes are useful only if tools may write into temporary external artifact directories.

### Allowed tracked files

Allowed in git:

```text
Tools/TerrainValidation/terrain_validate.py
Tools/TerrainValidation/terrain_maps.py
Tools/TerrainValidation/terrain_metrics.py
Tools/TerrainValidation/terrain_compare.py
Tools/TerrainValidation/README.md
Source/JungleGame/TerrainValidation/*
Config/DefaultTerrainValidation.ini
.gitignore
```

Not allowed in git:

```text
Saved/TerrainValidation/**
Intermediate/TerrainValidation/**
*.tif generated by validation
*.png generated by validation
*.csv.zst generated by validation
*.gpkg generated by validation
metrics.json generated by validation
manifest.json generated by validation
report.md generated by validation
```

### Staging guard command

Manual PR guard:

```bash
if git diff --cached --name-only | grep -E '(^Saved/TerrainValidation/|^Intermediate/TerrainValidation/|^Artifacts/TerrainValidation/|\.terrain-validation\.(tif|tiff|png|csv|csv\.zst|gpkg|geojson|jsonl)$)'; then
  echo 'BLOCKED: generated terrain validation artifacts are staged'
  exit 1
fi
```

### Status proof command

PR report must include this output:

```bash
git status --short --ignored Saved/TerrainValidation Intermediate/TerrainValidation Artifacts/TerrainValidation
```

Expected pattern:

```text
!! Saved/TerrainValidation/
```

No `A`, `M`, or `??` generated validation artifacts may appear in the normal staged/unstaged status.

## Evidence Report Template

Every major terrain PR must close with a compact evidence report in the PR body or a PR comment. The generated `report.md` remains ignored; paste the relevant summary manually or through the PR tool, but do not commit the generated report.

Template:

```markdown
## Terrain Validation Evidence

### Run Identity

- Run ID:
- Git commit:
- Branch:
- Dirty state: clean / dirty
- UE version:
- Platform:
- Map:
- Terrain config:
- Config SHA256:
- Seed:
- Export command:
- Analysis command:

### Export Provenance

| Check | Result |
|---|---|
| Runtime/source export present | PASS/FAIL |
| Config hash recorded | PASS/FAIL |
| Artifact hashes recorded | PASS/FAIL |
| Height units recorded | PASS/FAIL |
| Coordinate frame recorded | PASS/FAIL |
| Runtime/source parity | PASS/FAIL/NA |

### Core Terrain Metrics

| Metric | Baseline | Current | Delta | Gate |
|---|---:|---:|---:|---|
| Min elevation m | | | | report |
| Max elevation m | | | | >= meaningful threshold |
| Relief m | | | | >= meaningful threshold |
| p95 land elevation m | | | | report |
| Volume above sea m³ | | | | report |
| Land area m² | | | | within config |
| Mean slope deg | | | | report |
| p95 slope deg | | | | report |
| Beach p95 slope deg | | | | <= 10 |

### Coastline / Island Edge Gates

| Gate | Value | Threshold | Result |
|---|---:|---:|---|
| 0 m contour closed | | true | PASS/FAIL |
| 0 m contour median radial error | | <= 10 m | PASS/FAIL |
| 0 m contour p95 radial error | | <= 30 m | PASS/FAIL |
| Outside island p99 height | | <= -0.5 m | PASS/FAIL |
| Outside island max height | | <= +0.25 m nominal | PASS/FAIL |
| False land outside island | | <= 0.05% | PASS/FAIL |
| Square-edge raised components | | 0 | PASS/FAIL |
| Beach continuity | | >= 98% | PASS/FAIL |

### Hydrology / Ridge / Gully Gates

| Gate | Value | Threshold | Result |
|---|---:|---:|---|
| Major outlets reaching ocean | | >= 90% | PASS/FAIL |
| Major gullies ending in beach/ocean | | >= 80% | PASS/FAIL |
| Internal pits excluding crater | | <= threshold | PASS/FAIL |
| Ridge crossing outside mask | | <= 1% | PASS/FAIL |
| Square-edge ridge/gully artifacts | | 0 | PASS/FAIL |

### Crater / Hazard Gates

- Crater enabled: yes/no
- If enabled:
  - crater area:
  - rim continuity:
  - rim-floor relief:
  - hazard overlap with beach:
  - result:

### Required Maps Generated

- elevation:
- hillshade:
- contours:
- 0 m contour beach-ring overlay:
- slope:
- aspect:
- island/ocean mask:
- beach/shelf band:
- flow/catchment:
- ridge/gully:
- crater/hazard if applicable:
- height diff vs baseline:
- contour diff vs baseline:

### Artifact Hygiene

```bash
# paste status proof here
```

Result: PASS/FAIL

### Final Terrain Validation Decision

- PASS / FAIL:
- Blocking reasons if fail:
- Notes:
```

## Large PR Implementation Scope

### Scope summary

Implement the terrain validation loop as a developer tool, not gameplay.

Recommended large-PR scope:

1. Add Unreal terrain export command/subsystem.
2. Export manifest, source heightfield, runtime mesh vertices/triangles, and masks.
3. Add Python analysis scripts for DEM maps, metrics, and comparison.
4. Add coastline/edge/relief/slope/hydrology/mask gates.
5. Add `.gitignore` rules and staged-artifact guard.
6. Add a README documenting exact validation commands.
7. Add a small smoke test with synthetic terrain fixture if practical.

### Suggested implementation phases

#### Phase 1 — Exporter and manifest

Deliver:

- `manifest.json`;
- `height_source_m_f32.tif` or equivalent raw/npy fallback;
- `vertices_runtime.csv.zst` and `triangles_runtime.csv.zst` if mesh terrain exists;
- intended island/ocean/beach masks;
- log tokens;
- source/runtime parity checks.

Acceptance:

- exporter runs on Linux;
- exported files contain valid dimensions and non-empty data;
- manifest ties run to exact config/seed/commit;
- generated artifacts are ignored.

#### Phase 2 — Map generation

Deliver:

- elevation;
- hillshade;
- contours;
- 0 m contour beach-ring overlay;
- slope;
- aspect;
- island/ocean/beach maps.

Acceptance:

- maps generated from exported raster, not screenshots;
- map settings are fixed and recorded;
- 0 m contour is extracted as data and tested.

#### Phase 3 — Metrics and gates

Deliver:

- `metrics.json`;
- coastline gates;
- relief/slope gates;
- outside ocean/square-edge artifact gates;
- artifact hygiene gates.

Acceptance:

- validation fails on missing coastline;
- validation fails on raised square-edge terrain;
- validation fails on ambiguous metadata;
- validation reports baseline/current/delta.

#### Phase 4 — Hydrology/ridge/gully/crater

Deliver:

- flow accumulation;
- catchments;
- ridge/gully candidates;
- crater/hazard metrics when enabled.

Acceptance:

- major drainage outlets are counted;
- gullies/ridges are prevented from leaking into square-edge artifacts;
- crater metrics are skipped cleanly when crater is disabled.

#### Phase 5 — Comparison and PR evidence report

Deliver:

- baseline vs head numeric comparison;
- visual diff maps;
- report template generation;
- README commands.

Acceptance:

- major terrain PR can be judged from exported data and report;
- generated evidence remains unstaged.

## Files And Areas Likely Touched

Potential Unreal files:

```text
Source/JungleGame/TerrainValidation/TerrainValidationSubsystem.h
Source/JungleGame/TerrainValidation/TerrainValidationSubsystem.cpp
Source/JungleGame/TerrainValidation/TerrainValidationCommandlet.h
Source/JungleGame/TerrainValidation/TerrainValidationCommandlet.cpp
Source/JungleGame/TerrainValidation/TerrainValidationManifest.h
Source/JungleGame/TerrainValidation/TerrainValidationManifest.cpp
Source/JungleGame/TerrainValidation/TerrainValidationExportTypes.h
Source/JungleGame/Terrain/TerrainGenerator.*
Source/JungleGame/Terrain/IslandTerrainConfig.*
Config/DefaultTerrainValidation.ini
```

Potential tool files:

```text
Tools/TerrainValidation/terrain_validate.py
Tools/TerrainValidation/terrain_maps.py
Tools/TerrainValidation/terrain_metrics.py
Tools/TerrainValidation/terrain_compare.py
Tools/TerrainValidation/terrain_hydrology.py
Tools/TerrainValidation/terrain_report.py
Tools/TerrainValidation/README.md
```

Potential repo hygiene files:

```text
.gitignore
scripts/check_no_generated_terrain_artifacts_staged.sh
```

Generated but ignored files:

```text
Saved/TerrainValidation/**
Intermediate/TerrainValidation/**
Artifacts/TerrainValidation/**
```

## Acceptance Criteria

### Export acceptance

- [ ] Terrain validation command runs on Linux.
- [ ] `manifest.json` is written.
- [ ] Manifest records commit, dirty state, map, config path, config hash, seed, sea level, island center/radius, grid resolution, cell size, and artifact hashes.
- [ ] Source heightfield is exported when source grid exists.
- [ ] Runtime mesh vertices/triangles are exported when runtime mesh exists.
- [ ] Runtime/source parity is checked when both exist.
- [ ] Height units are metres relative to sea level.
- [ ] Ocean is real height data, not nodata.

### Map acceptance

- [ ] Elevation map generated.
- [ ] Hillshade map generated.
- [ ] Minor/major contours generated.
- [ ] 0 m contour generated separately.
- [ ] Slope map generated in degrees.
- [ ] Aspect map generated.
- [ ] Intended island mask generated.
- [ ] Measured land mask generated.
- [ ] Ocean mask generated.
- [ ] Beach/coastal shelf band generated.
- [ ] Flow/catchment maps generated or explicitly marked not implemented for current phase.
- [ ] Ridge/gully maps generated or explicitly marked not implemented for current phase.
- [ ] Crater/hazard map generated when crater is enabled.

### Coastline acceptance

- [ ] 0 m contour tracks intended beach ring.
- [ ] 0 m contour median radial error <= 10 m.
- [ ] 0 m contour p95 radial error <= 30 m.
- [ ] Outside island p99 height <= -0.5 m.
- [ ] False land outside island <= 0.05%.
- [ ] Beach band is measurable and continuous.
- [ ] Square-world edge has no raised terrain artifacts.
- [ ] Ridge/gully systems terminate coherently at coast.

### Terrain-scale acceptance

- [ ] Current iteration is compared against baseline.
- [ ] Meaningful-change gate reports pass/fail.
- [ ] If claiming 1400 m volcanic target progress, max elevation/relief/p95 elevation thresholds are reported.
- [ ] Slope distribution is reported.
- [ ] Extreme slopes are either justified by hazard/cliff mask or fail.

### Artifact hygiene acceptance

- [ ] `.gitignore` ignores terrain validation generated artifacts.
- [ ] Staging guard passes.
- [ ] Generated maps/CSVs/logs/rasters are not staged.
- [ ] PR evidence report includes status proof.

### Evidence acceptance

- [ ] PR includes terrain validation evidence report.
- [ ] Report contains run identity and provenance.
- [ ] Report contains metrics table.
- [ ] Report contains coastline gates.
- [ ] Report contains comparison against baseline.
- [ ] Report contains artifact hygiene proof.
- [ ] Any failed gate is explicitly called out.

## Risks And Stop Conditions

### Risks

| Risk | Mitigation |
|---|---|
| Runtime terrain differs from source grid | export both and add parity check |
| Mesh simplification hides true heightfield | record LOD/simplification and compare bounds/RMSE |
| Units mix Unreal cm and metres | store both raw cm and converted m; fail on missing units |
| Screenshots become evidence | forbid screenshots as primary evidence |
| Coastline looks okay but 0 m contour is broken | extract vector 0 m contour and test closure/radial error |
| Ocean outside island contains raised square-edge terrain | connected-component edge artifact test |
| Hydrology fill step masks real terrain pits | preserve fill-depth raster and report fill volume |
| Generated evidence gets committed | `.gitignore` + staging guard + PR status proof |
| Thresholds become too rigid before island scale is final | separate hard coastline/provenance gates from configurable morphology thresholds |

### Stop conditions

Stop and fail validation immediately when:

- no export can be produced from the actual runtime/source terrain;
- `manifest.json` cannot prove exact config/seed/commit;
- sea level is not defined;
- island center/radius is missing;
- 0 m contour cannot be extracted;
- outside island/ocean mask is undefined;
- square-world edge contains raised terrain artifacts;
- generated artifacts are staged;
- baseline comparison is required but baseline manifest is missing;
- metric generation uses screenshots instead of height/mesh data.

## Rejected Or Forbidden Approaches

Forbidden:

- Judging terrain quality by walking around in-game only.
- Using screenshots as the primary topographic evidence source.
- Hand-drawing contours.
- Hand-editing exported CSVs/maps.
- Exporting only source config and assuming runtime mesh matches.
- Exporting only runtime mesh while losing seed/config provenance.
- Using player-facing map/minimap/compass/GPS UI as validation.
- Committing generated maps, rasters, CSVs, logs, reports, or bulky evidence artifacts.
- Allowing square-world boundaries to pass because they are “outside play area.”
- Treating ocean as nodata.
- Changing map color scales per iteration in a way that hides real deltas.

Rejected for PR5 Batch 003 first implementation:

- Full erosion simulation correctness as a blocker.
- Perfect geomorphological classification.
- Remote GIS server dependency.
- Proprietary GIS requirement.
- Player-facing debug UI.

Allowed later:

- More advanced hydrology algorithms.
- Multi-resolution LOD validation.
- Automated PR artifact upload outside git.
- Richer crater/volcano morphology classification.
- Synthetic regression terrains for automated tests.

## Sources And References

[UE-Landscape]: https://dev.epicgames.com/documentation/unreal-engine/landscape-technical-guide-in-unreal-engine — Epic Games, *Landscape Technical Guide in Unreal Engine*. Used for Landscape dimensions, heightmap import formats, and height scaling context.

[UE-LandscapeEdit]: https://dev.epicgames.com/documentation/unreal-engine/API/Runtime/Landscape/FLandscapeEditDataInterface — Epic Games, *FLandscapeEditDataInterface API*. Used for Landscape height/weight data export approach.

[UE-ProceduralMesh]: https://dev.epicgames.com/documentation/unreal-engine/API/Plugins/ProceduralMeshComponent/UProceduralMeshComponent — Epic Games, *UProceduralMeshComponent API*. Used for generated runtime mesh context; note Epic marks the component experimental in current docs.

[UE-MeshDescription]: https://dev.epicgames.com/documentation/unreal-engine/API/Runtime/MeshDescription/FMeshDescription — Epic Games, *FMeshDescription API*. Used for editor/static mesh topology extraction context.

[UE-MeshDescriptionVertices]: https://dev.epicgames.com/documentation/unreal-engine/API/Runtime/MeshDescription/UMeshDescriptionBase/Vertices — Epic Games, *UMeshDescriptionBase::Vertices API*. Used for mesh vertex-array access context.

[GDAL-gdaldem]: https://gdal.org/en/stable/programs/gdaldem.html — GDAL, *gdaldem*. Used for hillshade, slope, aspect, and unit-scale guidance.

[GDAL-contour]: https://gdal.org/en/stable/programs/gdal_contour.html — GDAL, *gdal_contour*. Used for vector contour generation from DEM rasters.

[GDAL-DTM]: https://gdal.org/en/stable/tutorials/raster_dtm_tut.html — GDAL, *Working with a Digital Terrain Model*. Used for DEM terrain-processing workflow context.

[GRASS-watershed]: https://grass.osgeo.org/grass-stable/manuals/r.watershed.html — GRASS GIS, *r.watershed*. Used for flow accumulation, drainage direction, stream, and basin/catchment outputs.

[Git-gitignore]: https://git-scm.com/docs/gitignore — Git, *gitignore Documentation*. Used for ignored artifact rules and caveat that tracked files are not affected by ignore rules.

[GitHub-ignore]: https://docs.github.com/en/get-started/git-basics/ignoring-files — GitHub Docs, *Ignoring files*. Used for repository-level `.gitignore` guidance.
