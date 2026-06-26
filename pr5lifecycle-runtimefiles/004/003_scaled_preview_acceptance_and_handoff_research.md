# Scaled Terrain Preview Acceptance And Handoff Research

## Executive Summary

Batch 004 PR 003 should land the scaled terrain-preview evidence layer for **Jungle Game**: a deterministic UE5/Linux-first terrain preview pipeline that proves the world is now **6x larger in linear size** while preserving the clean coast gates from Batch 003 and rejecting the visible radial/gear/fan-blade drainage artifact.

The PR should be treated as a terrain evidence and acceptance PR, not a gameplay, ecology, material, VFX, UI, or world-design PR. Its deliverable is a reproducible preview bundle under `Images/TerrainPreview/`, a manifest that proves the true terrain scale numerically, anti-radial diagnostic images/metrics, and a tracked handoff under `handoffs/` that later lifecycle work can trust.

The most important implementation principle is that a 6x preview must not be a zoomed image. The manifest, sampled grid metadata, preview pixel spacing, world bounds, transform scale, and comparison fields must prove that the generated terrain covers approximately:

```text
world_size_m: 97536.0
linear_scale_multiplier_vs_batch003: 6.0
area_scale_multiplier_vs_batch003: 36.0
```

The PR should stop if any of these are true:

- generated evidence lands in broad `docs/` or `Docs/` folders;
- `world_size_m` remains near `16256.0`;
- the image is only resampled/zoomed without updated physical spacing and bounds;
- coast gates regress;
- drainage still reads as radial spokes from a center point;
- the handoff cannot identify exactly which manifest/images prove the scale and drainage fix.

## Terrain-Only Scope Boundary

This PR is strictly about terrain preview/export/acceptance evidence.

Allowed:

- deterministic terrain preview export;
- manifest schema and metric validation;
- image outputs for height, color relief, slope, coast masks, basin IDs, outlet distribution, ridge/gully masks, and anti-radial diagnostics;
- artifact hygiene rules for generated evidence;
- developer-only validation workflow;
- handoff documentation for the next lifecycle stage.

Forbidden:

- foliage, ecology, biomes, plant placement, animals, NPCs, AI, survival mechanics, quests, buildings, POIs, weather, audio, player UI, final materials, VFX, minimaps, compasses, GPS, objective arrows, bearing indicators, or time/orientation UI;
- moving terrain preview evidence back into deleted broad documentation folders;
- accepting a visually bigger screenshot without physical-scale proof;
- hiding coast/radial failures behind subjective “looks better” language.

The PR may reference Unreal Engine landscape and large-world constraints only to make terrain evidence reproducible and safe for later UE integration. Epic’s landscape guide matters here because UE landscape sizing is controlled by heightmap dimensions, component structure, and scale relationships rather than by screenshots alone.[^epic-landscape] Large World Coordinates are relevant context because a ~97.5 km terrain has precision implications that must be surfaced before runtime lifecycle work resumes.[^epic-lwc]

## Current Batch 003 Preview Baseline

Batch 003 produced deterministic terrain-preview evidence under:

```text
Images/TerrainPreview/
```

The clean coast baseline was:

```yaml
resolution_px: 1024
world_size_m: 16256.0
pixel_spacing_m: 15.890518084066471
height_min_m: -145.0
height_max_m: 1750.4691
shoreline_error_max_m: 0.0
beach_continuity_pct: 100.0
ocean_below_sea_pct: 100.0
square_edge_ocean_violations: 0
```

Batch 003 is accepted as a coast baseline, not as an interior drainage baseline. Visual review identified the next terrain defects:

```text
map/world must become 6x bigger in linear size
interior ridge/gully drainage must stop reading as gear/fan-blade radial spokes
coast/beach/shelf acceptance must remain clean
```

The Batch 004 PR should preserve Batch 003 as a comparison input, but it should not overwrite Batch 003 evidence. The implementation should write a new `b004_pr003_*` preview set and a manifest that records Batch 003 values as explicit baseline fields.

## Batch 004 Acceptance Goals

Batch 004 PR 003 should be accepted only when the evidence bundle proves all of the following:

| Gate | Required Result |
|---|---:|
| True world size | `world_size_m == 97536.0` or equivalent 6x linear target within a tiny numeric tolerance |
| Linear multiplier | `linear_scale_multiplier_vs_batch003 == 6.0` |
| Area multiplier | `area_scale_multiplier_vs_batch003 == 36.0` |
| Preview proof | manifest shows updated physical bounds/pixel spacing, not just a zoomed image |
| Shoreline error | `shoreline_error_max_m <= 0.5 m`, preferred `0.0` |
| Beach continuity | `beach_continuity_pct == 100.0` |
| Ocean below sea | `ocean_below_sea_pct == 100.0` |
| Square-edge ocean | `square_edge_ocean_violations == 0` |
| Radial spokes | anti-radial metrics reject gear/fan-blade morphology |
| Evidence location | generated previews under `Images/TerrainPreview/` or explicit `Images/<terrain-evidence-subgroup>/` only |
| Docs hygiene | no broad `docs/` or `Docs/` dependency returns |
| Handoff | tracked handoff identifies accepted manifest/images, metrics, known limitations, and next lifecycle gate |

The world-size target follows directly from Batch 003:

```text
16256.0 m * 6 = 97536.0 m
97536.0 m / 1000 = 97.536 km
```

For a 1024 px preview that samples pixel centers across the same pixel count, the expected full-domain pixel spacing is:

```text
97536.0 / (1024 - 1) = 95.34213098729228 m
```

If the implementation uses a different sample convention, the manifest must record the convention explicitly and include both the formula and the resulting spacing. The acceptance decision should compare physical bounds and spacing, not visual footprint.

## Required Generated Images

All Batch 004 PR 003 image outputs should use an exact, grep-friendly prefix:

```text
Images/TerrainPreview/b004_pr003_scaled_preview_*
```

Recommended required files:

```text
Images/TerrainPreview/b004_pr003_scaled_preview_color_relief.png
Images/TerrainPreview/b004_pr003_scaled_preview_height_linear.png
Images/TerrainPreview/b004_pr003_scaled_preview_height_histogram.png
Images/TerrainPreview/b004_pr003_scaled_preview_slope.png
Images/TerrainPreview/b004_pr003_scaled_preview_slope_histogram.png
Images/TerrainPreview/b004_pr003_scaled_preview_mask_atlas.png
Images/TerrainPreview/b004_pr003_scaled_preview_coast_acceptance_overlay.png
Images/TerrainPreview/b004_pr003_scaled_preview_basin_ids.png
Images/TerrainPreview/b004_pr003_scaled_preview_basin_boundaries.png
Images/TerrainPreview/b004_pr003_scaled_preview_outlet_distribution.png
Images/TerrainPreview/b004_pr003_scaled_preview_flow_direction_hsv.png
Images/TerrainPreview/b004_pr003_scaled_preview_flow_accumulation_log.png
Images/TerrainPreview/b004_pr003_scaled_preview_ridge_gully_masks.png
Images/TerrainPreview/b004_pr003_scaled_preview_radial_diagnostics.png
Images/TerrainPreview/b004_pr003_scaled_preview_radial_angle_histogram.png
Images/TerrainPreview/b004_pr003_scaled_preview_radial_score_heatmap.png
Images/TerrainPreview/b004_pr003_scaled_preview_batch003_vs_batch004_contact_sheet.png
Images/TerrainPreview/b004_pr003_scaled_preview_manifest.json
Images/TerrainPreview/b004_pr003_scaled_preview_validation.txt
```

Optional, if cheap and deterministic:

```text
Images/TerrainPreview/b004_pr003_scaled_preview_contours_100m.png
Images/TerrainPreview/b004_pr003_scaled_preview_hillshade.png
Images/TerrainPreview/b004_pr003_scaled_preview_hypsometric_tint.png
Images/TerrainPreview/b004_pr003_scaled_preview_stream_candidates.png
Images/TerrainPreview/b004_pr003_scaled_preview_basin_area_rank.png
Images/TerrainPreview/b004_pr003_scaled_preview_outlet_rose.png
Images/TerrainPreview/b004_pr003_scaled_preview_edge_band_masks.png
```

The visual set should follow normal DEM-review practice: elevation/color relief, slope, aspect/flow direction, hillshade, contouring, and hydrology derivatives reveal different failure modes. GDAL’s `gdaldem` groups terrain visualization modes such as hillshade, slope, aspect, color-relief, TRI, TPI, and roughness, which is a useful reference model for what terrain preview evidence should expose.[^gdal-gdaldem] USGS DEM guidance likewise treats bare-earth elevation data as the base surface from which terrain derivatives and hydrologic structure can be analyzed.[^usgs-dem][^usgs-terrain]

## Manifest Schema

The manifest should be JSON, deterministic, and small enough to review in PR diffs.

Required file:

```text
Images/TerrainPreview/b004_pr003_scaled_preview_manifest.json
```

Recommended schema:

```json
{
  "schema_version": 1,
  "project": "Jungle Game",
  "batch": "004",
  "pr": "003",
  "artifact_prefix": "b004_pr003_scaled_preview",
  "terrain_scope": "terrain-preview-export-acceptance-only",
  "generated_utc": "2026-..-..T..:..:..Z",
  "generator": {
    "tool_name": "TerrainPreviewExporter",
    "tool_version": "...",
    "git_commit": "...",
    "git_branch": "...",
    "platform": "Linux",
    "engine_context": "UE5/Linux-first"
  },
  "determinism": {
    "seed": 0,
    "height_source_fingerprint": "sha256:...",
    "mask_source_fingerprint": "sha256:...",
    "algorithm_fingerprint": "sha256:...",
    "config_fingerprint": "sha256:..."
  },
  "batch003_baseline": {
    "world_size_m": 16256.0,
    "resolution_px": 1024,
    "pixel_spacing_m": 15.890518084066471,
    "shoreline_error_max_m": 0.0,
    "beach_continuity_pct": 100.0,
    "ocean_below_sea_pct": 100.0,
    "square_edge_ocean_violations": 0
  },
  "scale": {
    "target_linear_multiplier": 6.0,
    "target_area_multiplier": 36.0,
    "world_size_m": 97536.0,
    "world_size_km": 97.536,
    "world_area_km2": 9513.271296,
    "linear_scale_multiplier_vs_batch003": 6.0,
    "area_scale_multiplier_vs_batch003": 36.0,
    "resolution_px": 1024,
    "sample_convention": "pixel_centers_span_world_bounds",
    "pixel_spacing_m": 95.34213098729228,
    "bounds_m": {
      "min_x": -48768.0,
      "max_x": 48768.0,
      "min_y": -48768.0,
      "max_y": 48768.0
    },
    "not_zoomed_proof": {
      "physical_spacing_changed": true,
      "world_bounds_changed": true,
      "heightfield_resampled_only": false,
      "same_resolution_larger_spacing": true
    }
  },
  "height": {
    "height_min_m": -145.0,
    "height_max_m": 1750.4691,
    "height_mean_m": 0.0,
    "height_p01_m": 0.0,
    "height_p50_m": 0.0,
    "height_p99_m": 0.0,
    "sea_level_m": 0.0
  },
  "coast_acceptance": {
    "shoreline_error_max_m": 0.0,
    "shoreline_error_mean_m": 0.0,
    "shoreline_error_p95_m": 0.0,
    "beach_continuity_pct": 100.0,
    "ocean_below_sea_pct": 100.0,
    "square_edge_ocean_violations": 0,
    "coast_component_count": 1,
    "beach_gap_count": 0,
    "beach_gap_max_m": 0.0,
    "shelf_monotonicity_violations": 0
  },
  "hydrology": {
    "flow_model": "D8_or_Dinf_record_exactly",
    "pit_fill_or_breach_method": "record_exactly",
    "basin_count": 0,
    "basin_min_area_km2": 0.0,
    "basin_median_area_km2": 0.0,
    "basin_max_area_km2": 0.0,
    "outlet_count": 0,
    "outlet_edge_distribution": {
      "north": 0,
      "east": 0,
      "south": 0,
      "west": 0,
      "coast": 0,
      "interior_sink": 0
    }
  },
  "anti_radial_acceptance": {
    "candidate_center_m": [0.0, 0.0],
    "radial_angle_entropy_norm": 0.0,
    "radial_resultant_length": 0.0,
    "spoke_peak_count": 0,
    "spoke_peak_dominance_ratio": 0.0,
    "spoke_sector_max_fraction": 0.0,
    "ridge_radial_alignment_mean_abs_cos": 0.0,
    "gully_radial_alignment_mean_abs_cos": 0.0,
    "basin_outlet_angular_entropy_norm": 0.0,
    "basin_centroid_ray_alignment_p95": 0.0,
    "radial_score_max": 0.0,
    "radial_score_p95": 0.0,
    "gear_morphology_rejected": true
  },
  "outputs": {
    "images": [
      "Images/TerrainPreview/b004_pr003_scaled_preview_color_relief.png"
    ],
    "manifest": "Images/TerrainPreview/b004_pr003_scaled_preview_manifest.json",
    "validation_log": "Images/TerrainPreview/b004_pr003_scaled_preview_validation.txt",
    "handoff": "handoffs/2026-..-.._batch004_pr003_scaled_preview_acceptance_handoff.md"
  },
  "acceptance": {
    "accepted": true,
    "stop_conditions_triggered": [],
    "notes": []
  }
}
```

### Manifest review rules

A reviewer should be able to answer these questions from the manifest alone:

1. What Batch 003 terrain was used as the comparison baseline?
2. What is the Batch 004 world size in meters and kilometers?
3. Which formula produced `pixel_spacing_m`?
4. Did physical world bounds expand by 6x?
5. Did image resolution stay constant, increase, or change?
6. Did coast acceptance remain clean?
7. Which anti-radial metrics rejected gear/fan-blade morphology?
8. Which image files provide visual evidence?
9. Which handoff file records the lifecycle transition?
10. Are all outputs under `Images/TerrainPreview/` or approved `handoffs/` paths?

## 6x Scale Evidence Metrics

The scale proof must combine target values, computed values, and anti-fraud/anti-zoom fields.

Required fields:

```yaml
batch003_world_size_m: 16256.0
target_linear_multiplier: 6.0
target_world_size_m: 97536.0
actual_world_size_m: 97536.0
world_size_abs_error_m: 0.0
world_size_rel_error_pct: 0.0
actual_world_size_km: 97.536
actual_world_area_km2: 9513.271296
linear_scale_multiplier_vs_batch003: 6.0
area_scale_multiplier_vs_batch003: 36.0
resolution_px: 1024
sample_convention: pixel_centers_span_world_bounds
pixel_spacing_m: 95.34213098729228
batch003_pixel_spacing_m: 15.890518084066471
pixel_spacing_multiplier_vs_batch003: 6.0
bounds_min_x_m: -48768.0
bounds_max_x_m: 48768.0
bounds_min_y_m: -48768.0
bounds_max_y_m: 48768.0
not_zoomed_physical_bounds_changed: true
not_zoomed_pixel_spacing_changed: true
not_zoomed_manifest_formula_present: true
```

Recommended acceptance thresholds:

```yaml
world_size_abs_error_m_max: 0.001
linear_scale_multiplier_abs_error_max: 0.000001
area_scale_multiplier_abs_error_max: 0.0001
pixel_spacing_multiplier_abs_error_max: 0.000001
bounds_symmetry_error_m_max: 0.001
```

The validation script should fail if:

- `actual_world_size_m < 97000.0` or `actual_world_size_m > 98000.0`;
- `linear_scale_multiplier_vs_batch003` is not approximately `6.0`;
- `area_scale_multiplier_vs_batch003` is not approximately `36.0`;
- `pixel_spacing_m` is unchanged from Batch 003 while resolution is unchanged;
- only image dimensions changed but world bounds did not;
- output image metadata or manifest proves a 1024 px image with Batch 003 physical scale.

## Coast Preservation Metrics

The coast gates are preservation gates. Batch 004 is not allowed to use the 6x scale rewrite as an excuse to regress clean coastal behavior.

Required coast metrics:

```yaml
shoreline_error_max_m: 0.0
shoreline_error_mean_m: 0.0
shoreline_error_p95_m: 0.0
beach_continuity_pct: 100.0
ocean_below_sea_pct: 100.0
square_edge_ocean_violations: 0
beach_gap_count: 0
beach_gap_max_m: 0.0
coast_component_count: 1
coast_mask_connected_component_count: 1
beach_mask_connected_component_count: 1
shelf_monotonicity_violations: 0
sea_level_crossing_ambiguity_count: 0
```

Recommended thresholds:

```yaml
shoreline_error_max_m_max: 0.5
shoreline_error_p95_m_max: 0.25
beach_continuity_pct_min: 100.0
ocean_below_sea_pct_min: 100.0
square_edge_ocean_violations_max: 0
beach_gap_count_max: 0
shelf_monotonicity_violations_max: 0
```

Coast validation should include both numeric and image evidence:

- overlay expected shoreline against sampled zero/sea-level contour;
- mark beach mask continuity in a distinct overlay layer;
- show ocean-below-sea failures as high-contrast red pixels if any exist;
- show square-edge ocean violations in an edge-band diagnostic image;
- include connected-component counts for coast and beach masks.

The distance-based shoreline comparison pattern is consistent with established shoreline analysis practice: USGS DSAS uses shoreline positions and transects to compute shoreline/boundary change and related uncertainties.[^usgs-dsas] This project does not need full DSAS, but the same principle applies: a shoreline gate should be measured as a positional boundary error rather than judged only by image appearance.

Connected-component checks are appropriate for beach/coast continuity because region labeling can identify whether a binary mask is one continuous object or has fragmented gaps; scikit-image’s `measure.label` and `regionprops` are suitable reference APIs for this class of image measurement.[^skimage-measure]

## Anti-Radial Drainage Metrics

The anti-radial acceptance problem is not “does drainage exist?” It is “does the interior terrain stop reading like a gear/fan-blade with spokes radiating from a central generator?”

The PR should compute a small family of metrics rather than a single magic score.

### 1. Radial angle entropy

Compute angles from a candidate center to ridge/gully pixels or basin centroids. Bin angles into equal-width sectors, then compute normalized Shannon entropy:

```text
radial_angle_entropy_norm = H(angle_bins) / log(bin_count)
```

Interpretation:

- low entropy: pixels cluster into repeated spokes or sectors;
- high entropy: drainage/ridges distribute across angle space naturally.

Recommended acceptance:

```yaml
radial_angle_entropy_norm_min: 0.82
```

### 2. Circular resultant length

Use circular statistics on drainage/ridge/gully orientation angles. The mean resultant length is useful because a strong preferred direction produces a larger resultant; circular standard deviation is a standard way to summarize angular spread.[^scipy-circ]

Recommended acceptance:

```yaml
radial_resultant_length_max: 0.18
ridge_orientation_resultant_length_max: 0.22
gully_orientation_resultant_length_max: 0.22
```

### 3. Spoke peak count and dominance

Build an angular histogram around the suspected terrain center. Detect peaks that exceed a robust baseline, such as median plus `2.5 * MAD`.

Required fields:

```yaml
spoke_peak_count: 0
spoke_peak_angles_deg: []
spoke_peak_dominance_ratio: 0.0
spoke_sector_max_fraction: 0.0
```

Recommended acceptance:

```yaml
spoke_peak_count_max: 3
spoke_peak_dominance_ratio_max: 2.25
spoke_sector_max_fraction_max: 0.14
```

This does not require perfectly random terrain. It only rejects repeated blade-like sectors that dominate the interior.

### 4. Ridge/gully radial alignment

For each ridge/gully pixel, compute:

- `theta_radial`: angle from candidate center to pixel;
- `theta_feature`: local ridge/gully tangent or flow direction;
- alignment score: `abs(cos(theta_feature - theta_radial))`.

A gear/fan-blade terrain will show excessive alignment with radial rays.

Recommended fields:

```yaml
ridge_radial_alignment_mean_abs_cos: 0.0
ridge_radial_alignment_p95_abs_cos: 0.0
gully_radial_alignment_mean_abs_cos: 0.0
gully_radial_alignment_p95_abs_cos: 0.0
```

Recommended acceptance:

```yaml
ridge_radial_alignment_mean_abs_cos_max: 0.62
gully_radial_alignment_mean_abs_cos_max: 0.62
ridge_radial_alignment_p95_abs_cos_max: 0.92
gully_radial_alignment_p95_abs_cos_max: 0.92
```

### 5. Basin/outlet distribution

Drainage basins should terminate across plausible coastal/edge/outlet positions. They should not all look like center-to-edge spokes.

Required fields:

```yaml
basin_count: 0
basin_area_entropy_norm: 0.0
basin_centroid_angle_entropy_norm: 0.0
basin_outlet_angular_entropy_norm: 0.0
outlet_edge_distribution_north: 0
outlet_edge_distribution_east: 0
outlet_edge_distribution_south: 0
outlet_edge_distribution_west: 0
outlet_distribution_chi_square_p: 0.0
interior_sink_count: 0
```

Recommended acceptance:

```yaml
basin_centroid_angle_entropy_norm_min: 0.78
basin_outlet_angular_entropy_norm_min: 0.76
interior_sink_count_max: 0
largest_basin_area_fraction_max: 0.35
```

The exact hydrology model must be recorded. D8 is simple and deterministic; D-infinity can reduce grid-direction artifacts by allowing flow over triangular facets rather than only eight compass directions.[^taudem-dinf] GRASS `r.watershed` is a useful reference because it outputs flow accumulation, drainage direction, streams, and watershed basins from raster elevation data.[^grass-watershed] Jenson and Domingue’s classic DEM workflow is also directly relevant because it describes deriving topographic structure such as drainage and watershed information from DEMs for GIS analysis.[^jenson-domingue]

### 6. Visual anti-radial diagnostics

The following images must make the failure visible if it returns:

```text
b004_pr003_scaled_preview_radial_diagnostics.png
b004_pr003_scaled_preview_radial_angle_histogram.png
b004_pr003_scaled_preview_radial_score_heatmap.png
b004_pr003_scaled_preview_flow_direction_hsv.png
b004_pr003_scaled_preview_basin_ids.png
b004_pr003_scaled_preview_outlet_distribution.png
b004_pr003_scaled_preview_ridge_gully_masks.png
```

`radial_diagnostics.png` should include:

- suspected/candidate center marker;
- angle sectors;
- detected spoke peaks;
- ridge/gully pixels colored by radial-alignment score;
- coast outline for spatial context;
- text stamp with `world_size_m`, `pixel_spacing_m`, `radial_angle_entropy_norm`, `spoke_peak_count`, and `gear_morphology_rejected`.

### 7. Stop condition if it still looks like a gear

Even if metrics pass, the PR should stop if visual review clearly shows repeated radial spokes. The handoff should say:

```text
STOPPED: anti-radial metrics did not match visual review; terrain still reads as gear/fan-blade morphology.
```

Then include the failing image path and the metrics that need recalibration.

## Image Output Location Rules

The only approved generated terrain evidence root for this PR is:

```text
Images/TerrainPreview/
```

Allowed alternatives only if explicitly named as image/evidence subgroups:

```text
Images/TerrainPreview/
Images/TerrainEvidence/
Images/WorldEvidence/
```

Do not write generated terrain-preview evidence to:

```text
docs/
Docs/
docs/World/
Docs/World/
Documentation/
```

Required path policy:

```yaml
terrain_preview_root: Images/TerrainPreview
batch004_prefix: b004_pr003_scaled_preview
manifest_path: Images/TerrainPreview/b004_pr003_scaled_preview_manifest.json
validation_log_path: Images/TerrainPreview/b004_pr003_scaled_preview_validation.txt
handoff_root: handoffs
handoff_prefix: batch004_pr003_scaled_preview_acceptance_handoff
```

The exporter should fail fast if the configured output root resolves to a broad docs directory. This can be a simple guard:

```text
reject if normalized_output_path starts_with docs/ or Docs/
reject if path contains /docs/ or /Docs/ as the generated evidence root
```

## Artifact Hygiene Rules

### Track in git

Track the small deterministic evidence files needed for review and handoff:

```text
Images/TerrainPreview/b004_pr003_scaled_preview_manifest.json
Images/TerrainPreview/b004_pr003_scaled_preview_validation.txt
Images/TerrainPreview/b004_pr003_scaled_preview_color_relief.png
Images/TerrainPreview/b004_pr003_scaled_preview_height_linear.png
Images/TerrainPreview/b004_pr003_scaled_preview_slope.png
Images/TerrainPreview/b004_pr003_scaled_preview_mask_atlas.png
Images/TerrainPreview/b004_pr003_scaled_preview_coast_acceptance_overlay.png
Images/TerrainPreview/b004_pr003_scaled_preview_basin_ids.png
Images/TerrainPreview/b004_pr003_scaled_preview_outlet_distribution.png
Images/TerrainPreview/b004_pr003_scaled_preview_ridge_gully_masks.png
Images/TerrainPreview/b004_pr003_scaled_preview_radial_diagnostics.png
Images/TerrainPreview/b004_pr003_scaled_preview_radial_angle_histogram.png
Images/TerrainPreview/b004_pr003_scaled_preview_batch003_vs_batch004_contact_sheet.png
handoffs/2026-..-.._batch004_pr003_scaled_preview_acceptance_handoff.md
```

Track source changes that generate the evidence:

```text
Source/**/TerrainPreview*.h
Source/**/TerrainPreview*.cpp
Source/**/Terrain*.h
Source/**/Terrain*.cpp
Scripts/**/terrain_preview*.py
Scripts/**/validate_terrain_preview*.py
Config/**/TerrainPreview*.ini
```

### Ignore or do not commit

Do not track heavy raw exports, temporary arrays, local caches, or editor-generated junk:

```text
Saved/
Intermediate/
DerivedDataCache/
Binaries/
.vscode/
.idea/
*.tmp
*.cache
*.npy
*.npz
*.exr
*.tif
*.tiff
Images/TerrainPreview/tmp_*
Images/TerrainPreview/*_raw_*.bin
Images/TerrainPreview/*_debug_scratch_*
```

If high-bit-depth TIFF/EXR outputs are needed for local validation, keep them ignored and derive the tracked PNG/JSON evidence from them.

### Naming discipline

Every generated Batch 004 PR 003 artifact should share this prefix:

```text
b004_pr003_scaled_preview_
```

Every handoff should include a date prefix and batch/PR phrase:

```text
handoffs/YYYY-MM-DD_batch004_pr003_scaled_preview_acceptance_handoff.md
```

Do not use vague names such as:

```text
preview.png
terrain_new.png
fixed.png
final.png
map_big.png
handoff.md
```

## Developer-Only Validation Workflow

The validation flow should be one command, deterministic, and Linux-first.

Recommended command shape:

```bash
./Scripts/validate_terrain_preview.sh --batch 004 --pr 003 --output-root Images/TerrainPreview
```

The script should perform:

1. clean path guard;
2. deterministic export;
3. manifest validation;
4. image existence validation;
5. coast gate validation;
6. anti-radial metric validation;
7. artifact hygiene validation;
8. handoff existence validation.

Recommended validation output:

```text
[PASS] output root Images/TerrainPreview
[PASS] no docs/Docs terrain evidence output
[PASS] world_size_m 97536.0
[PASS] linear scale multiplier 6.0
[PASS] area scale multiplier 36.0
[PASS] pixel spacing proves physical scale change
[PASS] shoreline_error_max_m 0.0
[PASS] beach_continuity_pct 100.0
[PASS] ocean_below_sea_pct 100.0
[PASS] square_edge_ocean_violations 0
[PASS] radial_angle_entropy_norm >= threshold
[PASS] spoke_peak_count <= threshold
[PASS] gear_morphology_rejected true
[PASS] tracked handoff exists
```

If validation fails, the output should name the exact gate and file:

```text
[FAIL] world_size_m expected 97536.0 got 16256.0 in Images/TerrainPreview/b004_pr003_scaled_preview_manifest.json
```

Do not require player-facing UI or runtime map systems for validation. This is a developer evidence workflow only.

## Handoff Requirements

Required handoff path:

```text
handoffs/YYYY-MM-DD_batch004_pr003_scaled_preview_acceptance_handoff.md
```

Required handoff sections:

```markdown
# Batch 004 PR 003 Scaled Preview Acceptance Handoff

## Accepted Evidence Bundle
## Scale Proof
## Coast Preservation Result
## Anti-Radial Drainage Result
## Generated Files Tracked
## Generated Files Ignored
## Validation Command
## Known Limitations
## Stop Conditions Checked
## Next Lifecycle Gate
```

The handoff must record:

- exact manifest path;
- exact validation log path;
- exact image paths used for visual review;
- Batch 003 baseline fields;
- Batch 004 scale fields;
- coast preservation metrics;
- anti-radial metrics;
- whether visual review agrees with metrics;
- list of generated files committed;
- list of local/generated files intentionally ignored;
- explicit statement that no broad `docs/` or `Docs/` output dependency returned;
- exact next-stage boundary: later terrain/ecosystem work may resume only after this evidence bundle is accepted.

Suggested handoff acceptance text:

```text
Batch 004 PR 003 is accepted for scaled terrain-preview evidence only. The manifest proves the terrain world size is 97536.0 m, a 6x linear and 36x area increase over Batch 003. Coast preservation gates remain clean. Anti-radial diagnostics reject the previous gear/fan-blade drainage morphology. Evidence is stored under Images/TerrainPreview/. No broad docs/Docs terrain evidence dependency was restored.
```

Suggested stop text:

```text
Batch 004 PR 003 is not accepted. Do not resume later terrain/ecosystem work. The failing gate is: <gate>. Evidence path: <path>. Required next action: fix the terrain preview/export/metrics and regenerate the evidence bundle under Images/TerrainPreview/.
```

## Files And Areas Likely Touched

Likely implementation areas:

```text
Source/JungleGame/Private/Terrain/TerrainPreviewExporter.cpp
Source/JungleGame/Public/Terrain/TerrainPreviewExporter.h
Source/JungleGame/Private/Terrain/TerrainHeightfieldGenerator.cpp
Source/JungleGame/Public/Terrain/TerrainHeightfieldGenerator.h
Source/JungleGame/Private/Terrain/TerrainCoastValidator.cpp
Source/JungleGame/Public/Terrain/TerrainCoastValidator.h
Source/JungleGame/Private/Terrain/TerrainHydrologyAnalyzer.cpp
Source/JungleGame/Public/Terrain/TerrainHydrologyAnalyzer.h
Source/JungleGame/Private/Terrain/TerrainRadialDiagnostics.cpp
Source/JungleGame/Public/Terrain/TerrainRadialDiagnostics.h
Source/JungleGame/Private/Terrain/TerrainPreviewManifest.cpp
Source/JungleGame/Public/Terrain/TerrainPreviewManifest.h
```

Likely script/config areas:

```text
Scripts/export_terrain_preview.py
Scripts/validate_terrain_preview.py
Scripts/validate_terrain_preview.sh
Scripts/make_terrain_preview_contact_sheet.py
Config/TerrainPreview.ini
Config/TerrainAcceptance.ini
```

Likely output areas:

```text
Images/TerrainPreview/b004_pr003_scaled_preview_*.png
Images/TerrainPreview/b004_pr003_scaled_preview_manifest.json
Images/TerrainPreview/b004_pr003_scaled_preview_validation.txt
handoffs/YYYY-MM-DD_batch004_pr003_scaled_preview_acceptance_handoff.md
```

Likely tests:

```text
Tests/TerrainPreviewScaleTests.cpp
Tests/TerrainCoastAcceptanceTests.cpp
Tests/TerrainRadialDiagnosticsTests.cpp
Tests/TerrainPreviewManifestTests.cpp
```

If the project is currently Python-heavy for preview export, keep the change in scripts and do not invent C++ classes. If the exporter already lives in UE/C++, keep script additions as validators/wrappers. The PR should match existing architecture rather than introducing a parallel terrain stack.

## Large PR Implementation Scope

This is a large implementation PR because it couples generator changes, metrics, images, validation, and handoff. Keep it cohesive but constrained.

### In scope

- set scaled target to `97536.0 m`;
- update terrain sampling/scale config;
- update interior ridge/gully drainage generation to reduce radial regularity;
- add anti-radial diagnostics;
- preserve coast/beach/shelf acceptance;
- emit required PNGs, manifest JSON, and validation log;
- add a deterministic validation command;
- add handoff markdown;
- update `.gitignore` only for terrain-preview hygiene if necessary.

### Out of scope

- gameplay systems;
- runtime navigation UI;
- ecosystem/foliage generation;
- final material/shader work;
- player traversal tuning;
- POI/content design;
- weather/audio/VFX;
- world partition runtime streaming implementation, except for noting later risk.

### Suggested implementation order

1. Add scale constants and manifest fields.
2. Prove `world_size_m == 97536.0` with unchanged or explicitly recorded resolution.
3. Regenerate preview images under `Images/TerrainPreview/` with `b004_pr003_scaled_preview_` prefix.
4. Re-run coast metrics and compare against Batch 003.
5. Implement radial diagnostics from ridge/gully masks, flow directions, basin IDs, and outlets.
6. Generate radial diagnostic images.
7. Add validation script/log.
8. Add handoff.
9. Inspect diff/stat for artifact hygiene.

## Acceptance Criteria

The PR is acceptable when all criteria pass:

```yaml
scale:
  world_size_m: 97536.0
  world_size_abs_error_m_max: 0.001
  linear_scale_multiplier_vs_batch003: 6.0
  area_scale_multiplier_vs_batch003: 36.0
  pixel_spacing_changed_from_batch003: true
  manifest_proves_not_zoomed_image: true

coast:
  shoreline_error_max_m_max: 0.5
  preferred_shoreline_error_max_m: 0.0
  beach_continuity_pct: 100.0
  ocean_below_sea_pct: 100.0
  square_edge_ocean_violations: 0
  beach_gap_count: 0
  shelf_monotonicity_violations: 0

anti_radial:
  radial_angle_entropy_norm_min: 0.82
  radial_resultant_length_max: 0.18
  spoke_peak_count_max: 3
  spoke_peak_dominance_ratio_max: 2.25
  spoke_sector_max_fraction_max: 0.14
  ridge_radial_alignment_mean_abs_cos_max: 0.62
  gully_radial_alignment_mean_abs_cos_max: 0.62
  basin_centroid_angle_entropy_norm_min: 0.78
  basin_outlet_angular_entropy_norm_min: 0.76
  gear_morphology_rejected: true

artifacts:
  output_root: Images/TerrainPreview
  filename_prefix: b004_pr003_scaled_preview_
  manifest_exists: true
  validation_log_exists: true
  handoff_exists: true
  no_broad_docs_output: true
```

A reviewer should open these four files first:

```text
Images/TerrainPreview/b004_pr003_scaled_preview_manifest.json
Images/TerrainPreview/b004_pr003_scaled_preview_coast_acceptance_overlay.png
Images/TerrainPreview/b004_pr003_scaled_preview_radial_diagnostics.png
handoffs/YYYY-MM-DD_batch004_pr003_scaled_preview_acceptance_handoff.md
```

Then inspect the contact sheet:

```text
Images/TerrainPreview/b004_pr003_scaled_preview_batch003_vs_batch004_contact_sheet.png
```

The PR should not be accepted unless the manifest and images agree.

## Risks And Stop Conditions

### Scale risks

Risk: image is visually bigger but physical terrain is not.

Stop if:

```text
world_size_m is not 97536.0 or equivalent 6x target
pixel_spacing_m does not change appropriately
bounds_m still match Batch 003
manifest lacks formula/source fields
```

### Coast risks

Risk: larger terrain breaks coast mask, sea-level crossing, beach continuity, or square-edge ocean handling.

Stop if:

```text
shoreline_error_max_m > 0.5
beach_continuity_pct < 100.0
ocean_below_sea_pct < 100.0
square_edge_ocean_violations > 0
beach_gap_count > 0
```

### Radial morphology risks

Risk: metrics are too weak and approve a terrain that still looks like a gear.

Stop if:

```text
visual review shows radial/fan-blade spokes
spoke_peak_count > threshold
radial angle histogram has repeated dominant sectors
ridge/gully alignment heatmap shows center-out rays
basin/outlet distribution clusters unnaturally around repeated angles
```

### Artifact risks

Risk: generated evidence recreates deleted broad docs structure or commits heavy junk.

Stop if:

```text
docs/ or Docs/ contains generated terrain evidence
raw heavy rasters are staged without explicit reason
untracked generated files make reproduction ambiguous
handoff names files that are not committed
```

### UE integration risks

Risk: a ~97.5 km terrain has downstream precision, streaming, and landscape component implications.

Do not solve those in this PR. Record them in the handoff as next-stage concerns. UE Landscape sizing and component choices should follow Epic’s landscape technical constraints when runtime integration resumes.[^epic-landscape] Precision-sensitive runtime behavior should be evaluated against UE5 Large World Coordinates constraints before gameplay implementation proceeds.[^epic-lwc]

## Rejected Or Forbidden Approaches

Reject these approaches explicitly:

- “6x” by screenshot zoom only;
- resampling a 1024 px image without changing physical world bounds;
- accepting a larger terrain without a manifest formula;
- moving terrain evidence into `docs/`, `Docs/`, `docs/World/`, or `Docs/World/`;
- replacing coast metrics with subjective screenshot review;
- replacing anti-radial metrics with subjective screenshot review only;
- hiding radial spokes with color relief or hillshade settings;
- adding foliage/ecology/noise overlays to disguise terrain morphology;
- solving radial drainage by adding player-facing map UI;
- committing raw giant arrays or temporary scratch exports;
- accepting validation if the handoff does not name the exact evidence bundle.

## Sources And References

[^epic-landscape]: Epic Games. “Landscape Technical Guide in Unreal Engine.” The guide documents landscape components, performance considerations, and heightmap-dimension relationships that matter when proving terrain scale from actual landscape data rather than screenshots. <https://dev.epicgames.com/documentation/unreal-engine/landscape-technical-guide-in-unreal-engine>

[^epic-lwc]: Epic Games. “Large World Coordinates in Unreal Engine 5.” UE5 LWC introduces double-precision variants across engine systems to improve precision for massive world scales. <https://dev.epicgames.com/documentation/unreal-engine/large-world-coordinates-in-unreal-engine-5>

[^usgs-dem]: U.S. Geological Survey. “What is a digital elevation model (DEM)?” USGS defines DEMs as bare-earth topographic surface representations, which is the appropriate conceptual base for terrain-preview height evidence. <https://www.usgs.gov/faqs/what-a-digital-elevation-model-dem>

[^usgs-terrain]: U.S. Geological Survey. “Extraction of terrain features from digital elevation models.” USGS describes DEM use for slope, watershed shape, and stream/watershed delineation. <https://www.usgs.gov/publications/extraction-terrain-features-digital-elevation-models>

[^jenson-domingue]: Jenson, S. K., and Domingue, J. O. “Extracting topographic structure from digital elevation data for geographic information-system analysis.” USGS publication record, originally *Photogrammetric Engineering and Remote Sensing*, 1988. <https://pubs.usgs.gov/publication/70142175>

[^taudem-dinf]: TauDEM. “D-Infinity Contributing Area.” The documentation describes D-infinity flow direction as steepest downward slope on planar triangular facets and computes contributing area from upslope neighbors. <https://hydrology.usu.edu/taudem/taudem5/help53/DInfinityContributingArea.html>

[^grass-watershed]: GRASS GIS. “r.watershed.” The manual documents raster outputs for flow accumulation, drainage direction, stream locations, and watershed basins. <https://grass.osgeo.org/grass-stable/manuals/r.watershed.html>

[^gdal-gdaldem]: GDAL. “gdaldem.” The documentation covers DEM visualization and derivative modes such as hillshade, slope, aspect, color relief, TRI, TPI, and roughness. <https://gdal.org/en/stable/programs/gdaldem.html>

[^usgs-dsas]: U.S. Geological Survey. “Digital Shoreline Analysis System (DSAS).” DSAS provides methods for measuring shoreline or boundary change using shoreline positions, transects, statistics, and uncertainty. <https://www.usgs.gov/centers/whcmsc/science/digital-shoreline-analysis-system-dsas>

[^skimage-measure]: scikit-image. “skimage.measure.” The API includes connected-region labeling and region-property measurement, useful for coast/beach mask continuity checks. <https://scikit-image.org/docs/stable/api/skimage.measure.html>

[^scipy-circ]: SciPy. “circmean” and “circstd.” SciPy’s circular statistics functions define circular mean and circular standard deviation for angular observations, useful for radial/sector diagnostics. <https://docs.scipy.org/doc/scipy/reference/generated/scipy.stats.circmean.html> and <https://docs.scipy.org/doc/scipy/reference/generated/scipy.stats.circstd.html>
