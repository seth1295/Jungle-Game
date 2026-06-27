# Morphology Validation And Preview Evidence Suite Research

## Executive Summary

The current Jungle Game terrain pipeline has a known validation gap: the shoreline can pass every coast gate while the island interior still reads as a single procedural volcano with radial ridge/gully spokes. This suite closes that gap by adding developer-only morphology metrics, visual evidence outputs, and a review manifest that can block acceptance even when the coast is perfect.

The proposed suite treats terrain quality as a measurable build artifact. Every batch must emit a coherent preview evidence set under `Images/TerrainPreview/`, plus a JSON manifest that states which morphology gates passed, failed, or need review. The review goal is simple: a fresh-chat reviewer must be able to open the manifest and preview images and understand why the terrain is accepted or rejected without needing prior context.

The suite preserves the current hard coast gates exactly:

```text
shoreline_error_max_m: 0 or effectively 0
beach_continuity_pct: 100%
ocean_below_sea_pct: 100%
square_edge_ocean_violations: 0
world_size_m: 97536.0 target
```

New morphology gates should catch:

- one-feature terrain where a single volcano or highland mass dominates the island;
- radial ridge/gully spokes aligned to the volcano center or map center;
- circular falloff rings caused by synthetic radial masks;
- weak catchment, ridge, valley, fan, and region diversity;
- an oversized volcano footprint;
- terrain that becomes boring when volcano generation is disabled.

The suite should be implemented as a large, terrain-only developer PR touching preview generation, metric computation, manifest schema, and automated acceptance gates. It must not introduce gameplay, UI, foliage, ecology, weather, buildings, minimaps, compass/GPS overlays, objective markers, final materials, or player-facing map systems.

## Terrain-Only Scope Boundary

This research/spec is intentionally narrow. It covers only deterministic terrain validation and preview evidence for a UE5 Linux-first first-person survival-simulation project.

**Allowed:**

- offline/developer preview rendering;
- deterministic height, slope, mask, region, catchment, and flow rasters;
- diagnostic overlays;
- JSON manifest schema;
- terrain-only C++/automation structures;
- morphology pass/fail logic;
- review warnings for human QA;
- batch-level file naming and grouping under `Images/TerrainPreview/`.

**Forbidden in this scope:**

- foliage, vegetation placement, ecology, animals, NPCs, weather, audio, buildings, final materials, gameplay loops, survival systems, map UI, compass UI, minimap, GPS, objective markers, quest markers, player-facing overlays, POI gameplay, or worldbuilding text;
- replacing visual terrain QA with screenshots from a player HUD;
- accepting subjective beauty screenshots without numeric gates;
- weakening coast gates because interior morphology is now being checked.

The suite is a terrain QA and evidence layer, not a player feature.

## Why Coast Metrics Are Not Enough

The existing coast metrics prove that the island boundary is geometrically correct. They do not prove the interior is believable, diverse, or free of procedural artifacts.

A coast-valid island can still fail because:

1. **Interior composition can collapse to one feature.** A volcano can occupy too much land area or dominate relief so strongly that all other features become subordinate.
2. **Drainage can be mathematically connected but visually fake.** Radial gullies may drain, but if they point away from a central volcano like spokes, the result reads as generator math rather than geomorphology.
3. **Hydrology can pass basic flow checks while catchments are repetitive.** Many equally spaced outlets with straight, equal-length channels suggest procedural radial partitioning.
4. **Slope and curvature can reveal synthetic masks.** Circular rings, isotropic falloff, or repeated annuli are invisible to shoreline gates.
5. **Volcano-disabled terrain can expose weak base terrain.** If disabling the volcano leaves a flat or boring island, the volcano is masking a failed terrain foundation.

Hydrologic and geomorphometric terrain workflows commonly derive flow direction, flow accumulation, watersheds, slope, curvature, and related layers from DEMs or height rasters. This suite adapts those terrain-analysis concepts to procedural terrain acceptance rather than real-world survey validation.

## Batch 004 Failure Modes To Detect

The suite should explicitly encode the known Batch 004 visual failures as named failure modes.

| Failure mode | Symptom | Required detection |
|---|---:|---|
| `OneFeatureTerrain` | Island reads as one volcano plus coast | Dominant highland/volcano footprint, landform entropy, named region count |
| `RadialSpokeRidges` | Ridges and gullies radiate from volcano center | Skeleton orientation vs radial vector, annular radial alignment score |
| `CircularFalloffArtifact` | Terrain has visible rings or circular masks | radial height profile fit, circular Hough/ring edge accumulator, low azimuthal variance by radius |
| `WeakBaseTerrain` | Volcano-disabled terrain is flat/boring | no-volcano relief, slope distribution, landform diversity, catchment count |
| `OversizedVolcanoFootprint` | Volcano masks the island foundation | volcano footprint fraction, contiguous highland dominance, radial-drainage capture |
| `HydrologyTooUniform` | Catchments look evenly sliced around island | catchment count, outlet spacing variance, drainage curvature, basin area entropy |
| `RegionMonotony` | Lowlands/interior lack distinct landform regions | landform region count, area-balanced entropy, adjacency diversity |

The manifest should preserve these names as stable machine-readable `failure_modes[]` values so a reviewer can compare batches over time.

## Required Preview Outputs

Every accepted batch must produce the following preview outputs. Missing required previews should fail the batch because the evidence set is incomplete.

| Output | Purpose | Hard requirement |
|---|---|---|
| `color_relief` | Human-readable terrain overview with shaded relief and hypsometric tint | Required |
| `height` | Raw normalized height raster | Required |
| `slope` | Slope magnitude raster | Required |
| `terrain_mask_atlas` | Debug atlas for terrain masks/classes | Required |
| `volcano_disabled_height` | Height preview with volcano subsystem disabled or zero-weighted | Required |
| `landform_region_map` | Connected/named morphology regions | Required |
| `catchment_id_map` | Drainage basin IDs and outlets | Required |
| `flow_accumulation_map` | Accumulated upstream area/flow raster | Required |
| `ridge_valley_skeleton_overlay` | Extracted ridge/valley centerlines over relief | Required |
| `radial_alignment_diagnostic` | Orientation/radial artifact heatmap | Required |
| `volcano_footprint_mask` | Binary/graded volcano influence mask | Required |
| `json_manifest` | Machine-readable metrics, paths, pass/fail reasons | Required |

### Preview Naming And Grouping

Use a deterministic, batch-scoped directory. Recommended layout:

```text
Images/TerrainPreview/
  Batch_006/
    World_97536m/
      Seed_<SeedHexOrDecimal>/
        00_manifest/
          B006_S<Seed>_terrain_preview_manifest.json
        01_overview/
          B006_S<Seed>_color_relief.png
          B006_S<Seed>_height.png
          B006_S<Seed>_slope.png
        02_masks/
          B006_S<Seed>_terrain_mask_atlas.png
          B006_S<Seed>_volcano_footprint_mask.png
        03_hydrology/
          B006_S<Seed>_catchment_id_map.png
          B006_S<Seed>_flow_accumulation_map.png
          B006_S<Seed>_ridge_valley_skeleton_overlay.png
        04_morphology/
          B006_S<Seed>_landform_region_map.png
          B006_S<Seed>_radial_alignment_diagnostic.png
          B006_S<Seed>_volcano_disabled_height.png
        05_review_contact_sheet/
          B006_S<Seed>_morphology_review_contact_sheet.png
```

Rules:

- Keep the manifest in `00_manifest/` and use paths relative to `Images/TerrainPreview/`.
- Prefix every image with batch and seed.
- Do not overwrite earlier batches.
- Use lowercase metric keys in JSON and stable output IDs in code.
- Include pixel size, world size, sea level, height scale, seed, generator version, and git revision in the manifest.
- The contact sheet is optional as a convenience image, but the required individual preview outputs remain mandatory.

### Preview Resolution

Use at least two resolutions:

```text
evidence_resolution_px: 4096 x 4096 preferred
quick_preview_resolution_px: 2048 x 2048 acceptable for local iteration
```

A lower quick preview may be used during development, but acceptance evidence should be generated at the full evidence resolution unless memory constraints are explicitly recorded in the manifest.

## Manifest Schema Additions

The manifest is the primary fresh-chat review artifact. It must explain what was tested, where the images are, and why the batch passed or failed.

### `FPreviewEvidenceManifest`

Recommended C++/JSON structure:

```cpp
struct FTerrainPreviewOutputSet
{
    FString OutputRoot;
    FString ColorReliefPath;
    FString HeightPath;
    FString SlopePath;
    FString TerrainMaskAtlasPath;
    FString VolcanoDisabledHeightPath;
    FString LandformRegionMapPath;
    FString CatchmentIdMapPath;
    FString FlowAccumulationMapPath;
    FString RidgeValleySkeletonOverlayPath;
    FString RadialAlignmentDiagnosticPath;
    FString VolcanoFootprintMaskPath;
    FString ManifestPath;
    FString ContactSheetPath;
};

struct FCoastValidationMetrics
{
    double ShorelineErrorMaxM;
    double BeachContinuityPct;
    double OceanBelowSeaPct;
    int32 SquareEdgeOceanViolations;
    double WorldSizeM;
};

struct FTerrainMorphologyMetrics
{
    int32 EvidenceResolutionPx;
    double LandAreaFraction;
    double LowlandFraction;
    double HighlandFraction;
    double ReliefP95P05M;
    double SlopeMedianDeg;
    double SlopeP75Deg;
    double LandformEntropy01;
    double DominantLandformFraction;
    int32 NamedRegionCount;
    int32 MajorRegionCount;
    int32 FailureCount;
    TArray<FString> FailureModes;
    TArray<FString> WarningModes;
};

struct FVolcanoDominanceMetrics
{
    double VolcanoFootprintLandFraction;
    double VolcanoCoreLandFraction;
    double VolcanoInfluenceMean;
    double VolcanoReliefShare;
    double ContiguousHighlandMaxLandFraction;
    double PeakProminenceM;
    double RadialDrainageCaptureFraction;
    double NonVolcanicReliefP95P05M;
};

struct FRadialArtifactMetrics
{
    FVector2D ReferenceCenterWorldXY;
    FString ReferenceCenterMode; // VolcanoCenter, IslandCentroid, MapCenter
    double SkeletonRadialAligned15Fraction;
    double SkeletonRadialAligned225Fraction;
    double AnnularMaxRadialAligned15Fraction;
    double MeanResultantLength01;
    double RayleighPApprox;
    double RadialHeightProfileR2;
    double CircularRingAccumulatorPeak01;
    double RadialDiagnosticScore01;
};

struct FCatchmentDiversityMetrics
{
    int32 MajorCatchmentCount;
    int32 TotalCatchmentCount;
    double CatchmentAreaEntropy01;
    double DominantCatchmentLandFraction;
    double OutletSpacingCV;
    double OutletSpacingMinM;
    double OutletSpacingMaxM;
    double MedianMainstemSinuosity;
    double MedianDrainageCurvatureDegPerKm;
    double BasinShapeCompactnessMedian;
    double StreamOrderMax;
    double FlowAccumulationGini;
};

struct FRidgeValleyFlowMetrics
{
    double RidgeSkeletonLengthKm;
    double ValleySkeletonLengthKm;
    double RidgeValleyLengthRatio;
    double BranchNodeDensityPer100Km2;
    double DeadStraightSegmentFraction;
    double ValleyFollowsFlowFraction;
    double RidgeValleyCrossingErrorCount;
};

struct FPreviewEvidenceManifest
{
    FString SchemaVersion;
    FString ProjectName;
    FString BatchId;
    FString Seed;
    FString GeneratorVersion;
    FString GitCommit;
    FString CreatedUtc;
    FCoastValidationMetrics Coast;
    FTerrainMorphologyMetrics Morphology;
    FVolcanoDominanceMetrics Volcano;
    FRadialArtifactMetrics Radial;
    FCatchmentDiversityMetrics Catchments;
    FRidgeValleyFlowMetrics RidgeValleyFlow;
    FTerrainPreviewOutputSet Outputs;
    TArray<FString> HardGateFailures;
    TArray<FString> ReviewWarnings;
    FString ReviewSummary;
    bool bAccepted;
};
```

### JSON Manifest Shape

Example shape:

```json
{
  "schema_version": "terrain_preview_evidence.v1",
  "project_name": "Jungle Game",
  "batch_id": "Batch_006",
  "seed": "123456789",
  "world": {
    "world_size_m": 97536.0,
    "sea_level_m": 0.0,
    "height_scale_m": 1.0,
    "evidence_resolution_px": 4096
  },
  "coast": {
    "shoreline_error_max_m": 0.0,
    "beach_continuity_pct": 100.0,
    "ocean_below_sea_pct": 100.0,
    "square_edge_ocean_violations": 0,
    "pass": true
  },
  "morphology": {
    "lowland_fraction": 0.48,
    "relief_p95_p05_m": 420.0,
    "landform_entropy_01": 0.72,
    "dominant_landform_fraction": 0.27,
    "named_region_count": 14,
    "major_region_count": 9,
    "pass": true
  },
  "volcano": {
    "volcano_footprint_land_fraction": 0.105,
    "volcano_core_land_fraction": 0.042,
    "volcano_relief_share": 0.31,
    "contiguous_highland_max_land_fraction": 0.22,
    "radial_drainage_capture_fraction": 0.29,
    "pass": true
  },
  "radial_artifacts": {
    "reference_center_mode": "VolcanoCenter",
    "skeleton_radial_aligned_15_fraction": 0.21,
    "skeleton_radial_aligned_22_5_fraction": 0.33,
    "annular_max_radial_aligned_15_fraction": 0.31,
    "mean_resultant_length_01": 0.18,
    "radial_height_profile_r2": 0.28,
    "circular_ring_accumulator_peak_01": 0.16,
    "pass": true
  },
  "catchments": {
    "major_catchment_count": 18,
    "catchment_area_entropy_01": 0.78,
    "dominant_catchment_land_fraction": 0.11,
    "outlet_spacing_cv": 0.62,
    "median_mainstem_sinuosity": 1.18,
    "median_drainage_curvature_deg_per_km": 9.4,
    "pass": true
  },
  "outputs": {
    "color_relief": "Batch_006/World_97536m/Seed_123456789/01_overview/B006_S123456789_color_relief.png",
    "height": "Batch_006/World_97536m/Seed_123456789/01_overview/B006_S123456789_height.png",
    "slope": "Batch_006/World_97536m/Seed_123456789/01_overview/B006_S123456789_slope.png",
    "terrain_mask_atlas": "Batch_006/World_97536m/Seed_123456789/02_masks/B006_S123456789_terrain_mask_atlas.png",
    "volcano_disabled_height": "Batch_006/World_97536m/Seed_123456789/04_morphology/B006_S123456789_volcano_disabled_height.png",
    "landform_region_map": "Batch_006/World_97536m/Seed_123456789/04_morphology/B006_S123456789_landform_region_map.png",
    "catchment_id_map": "Batch_006/World_97536m/Seed_123456789/03_hydrology/B006_S123456789_catchment_id_map.png",
    "flow_accumulation_map": "Batch_006/World_97536m/Seed_123456789/03_hydrology/B006_S123456789_flow_accumulation_map.png",
    "ridge_valley_skeleton_overlay": "Batch_006/World_97536m/Seed_123456789/03_hydrology/B006_S123456789_ridge_valley_skeleton_overlay.png",
    "radial_alignment_diagnostic": "Batch_006/World_97536m/Seed_123456789/04_morphology/B006_S123456789_radial_alignment_diagnostic.png",
    "volcano_footprint_mask": "Batch_006/World_97536m/Seed_123456789/02_masks/B006_S123456789_volcano_footprint_mask.png"
  },
  "hard_gate_failures": [],
  "review_warnings": [],
  "review_summary": "Coast gates pass and morphology evidence shows balanced volcanic, lowland, catchment, ridge, valley, and region diversity without radial spoke artifacts.",
  "accepted": true
}
```

### Fresh-Chat Review Requirements

The manifest must include:

- `hard_gate_failures[]` with stable failure IDs and human-readable reasons;
- `review_warnings[]` with stable warning IDs and suggested images to inspect;
- one `review_summary` paragraph that states whether the batch is acceptable;
- relative image paths for every required preview;
- all threshold values used to make pass/fail decisions;
- generator seed and version;
- explicit coast-gate preservation fields.

A fresh-chat reviewer should not need to infer why a metric failed. Example failure entry:

```json
{
  "id": "RadialSpokeRidges",
  "severity": "hard_fail",
  "metric": "skeleton_radial_aligned_15_fraction",
  "value": 0.43,
  "threshold": "<= 0.34",
  "reason": "Too many ridge/valley skeleton pixels align within 15 degrees of the volcano radial vector.",
  "inspect": [
    "radial_alignment_diagnostic",
    "ridge_valley_skeleton_overlay",
    "catchment_id_map"
  ]
}
```

## Volcano-Disabled Terrain Validation

The volcano-disabled preview is mandatory because the current volcano can hide a weak base terrain. The correct test is not simply “does the terrain still have height variation?” The test is: **does the island retain multiple readable non-volcanic landforms after the volcano subsystem is removed or set to zero influence?**

### Generation Modes

Use two deterministic modes for the same seed:

1. `FullTerrain`: all terrain modules enabled.
2. `VolcanoDisabledTerrain`: volcano subsystem disabled, volcano masks zero-weighted, and no post-pass allowed to fake replacement relief unless that pass is part of the base terrain generator.

The disabled preview must share the same coastline and world frame as the full terrain. Coast gates should still be checked, but a volcano-disabled coast mismatch should be treated as a generator dependency bug rather than an excuse to skip morphology QA.

### Required No-Volcano Metrics

| Metric | Target | Warning | Hard fail |
|---|---:|---:|---:|
| `non_volcanic_relief_p95_p05_m` | `>= 180 m` | `< 180 m` | `< 120 m` |
| `no_volcano_slope_p75_deg` | `>= 4.0°` | `< 4.0°` | `< 2.5°` |
| `no_volcano_named_region_count` | `>= 8` | `6-7` | `< 6` |
| `no_volcano_major_catchment_count` | `>= 10` | `8-9` | `< 8` |
| `no_volcano_landform_entropy_01` | `>= 0.55` | `0.45-0.55` | `< 0.45` |
| `no_volcano_dominant_landform_fraction` | `<= 0.38` | `0.38-0.48` | `> 0.48` |

These ranges are initial acceptance thresholds for a 97.536 km island, not universal geomorphology laws. They should be recorded in the manifest and refined once multiple seeds and hand-reviewed batches establish project-specific baselines.

### What The Suite Must Prove

The volcano-disabled evidence must show:

- multiple catchments that do not collapse into flat coastal runoff;
- at least several internally distinct upland/lowland/valley/fan regions;
- non-trivial slope distribution outside the coast and beach bands;
- no hidden radial alignment caused by base terrain masks;
- no massive feature void where the volcano was removed.

A batch should fail if volcano-disabled terrain is boring even when the full volcano render looks dramatic.

## Volcano Footprint And Feature Dominance Metrics

The volcano should be a major landform, not the island’s only identity. Measure its footprint, influence, and dominance separately.

### Volcano Footprint Mask

Build a graded `volcano_influence` raster from the volcano subsystem itself when possible. If direct subsystem contribution is unavailable, estimate influence from one or more of:

- delta between `FullTerrain` and `VolcanoDisabledTerrain`;
- crater/cone/lava masks;
- high radial-gradient contribution around the volcano center;
- volcanic material/terrain-class masks.

Recommended mask bands:

```text
volcano_core: influence >= 0.75
volcano_footprint: influence >= 0.25
volcano_transition: 0.10 <= influence < 0.25
non_volcanic_land: influence < 0.10
```

### Volcano Dominance Targets

| Metric | Target | Warning | Hard fail |
|---|---:|---:|---:|
| `volcano_footprint_land_fraction` | `0.04-0.12` | `0.12-0.18` or `< 0.025` | `> 0.18` or `< 0.015` |
| `volcano_core_land_fraction` | `0.01-0.06` | `0.06-0.09` | `> 0.09` |
| `volcano_relief_share` | `0.18-0.42` | `0.42-0.55` | `> 0.55` |
| `contiguous_highland_max_land_fraction` | `<= 0.28` | `0.28-0.38` | `> 0.38` |
| `radial_drainage_capture_fraction` | `<= 0.35` | `0.35-0.45` | `> 0.45` |
| `lowland_fraction` | `0.35-0.70` | `0.25-0.35` or `0.70-0.80` | `< 0.25` or `> 0.80` |
| `highland_fraction` | `0.08-0.35` | `0.35-0.45` | `> 0.45` |

Definitions:

- `volcano_footprint_land_fraction`: land pixels where volcano influence is at least 0.25.
- `volcano_core_land_fraction`: land pixels where volcano influence is at least 0.75.
- `volcano_relief_share`: fraction of total positive island relief attributable to volcano contribution.
- `contiguous_highland_max_land_fraction`: largest connected component of highland pixels divided by land pixels.
- `radial_drainage_capture_fraction`: fraction of major channels whose upstream head or dominant descent vector begins in the volcano footprint and drains radially outward.
- `lowland_fraction`: land below the project lowland elevation threshold or below the lower terrain-zone percentile, excluding beach/ocean.
- `highland_fraction`: land above the project highland threshold or top elevation zone percentile, excluding isolated noise peaks.

### Dominance Blocking Rule

A batch must fail if the volcano footprint and radial artifact metrics fail together, even if each individual value is near only a warning threshold. The combined visual result is the known bad case: a large volcano whose ridges and gullies point outward like spokes.

Suggested combined fail:

```text
if volcano_footprint_land_fraction > 0.14
and skeleton_radial_aligned_15_fraction > 0.30
and radial_height_profile_r2 > 0.45:
    hard_fail: VolcanoRadialDominance
```

## Radial Alignment Artifact Metrics

Radial terrain is the most important numerical artifact to catch. The suite should measure whether ridges, valleys, slope/aspect gradients, flow paths, and height contours align too strongly with vectors from a reference center.

### Reference Centers

Run radial metrics against three centers:

1. `VolcanoCenter`: required when a volcano exists.
2. `IslandCentroid`: required for whole-island morphology.
3. `MapCenter`: required to detect accidental square/world-coordinate falloff.

Use the worst score as the batch score, but record all three in the manifest.

### Skeleton Orientation Method

Extract ridge and valley skeletons from curvature, slope, and flow accumulation masks. For each skeleton pixel:

1. Estimate local line orientation `theta_skeleton` using local PCA, structure tensor, or neighbor-chain tangent.
2. Compute vector from reference center to pixel: `theta_radial = atan2(y - cy, x - cx)`.
3. Because ridges/valleys are axial features, compute angular difference as:

```text
delta = min(abs(theta_skeleton - theta_radial), pi - abs(theta_skeleton - theta_radial))
```

4. Weight each sample by skeleton confidence, local relief, or flow accumulation.
5. Count fractions within angular windows:

```text
aligned_15 = fraction(delta <= 15°)
aligned_22_5 = fraction(delta <= 22.5°)
aligned_30 = fraction(delta <= 30°)
```

### Circular Statistics Method

Use circular statistics on doubled axial angles:

```text
alpha = 2 * (theta_skeleton - theta_radial)
R = length(mean(exp(i * alpha)))
circular_variance = 1 - R
```

High `R` means directional concentration around radial alignment. Record an approximate Rayleigh-style uniformity score, but use it as a warning/review statistic unless calibrated against project seeds. Mean resultant length is useful because it is scale-free and easy to explain in the manifest.

### Annular Radial Spike Method

Radial artifacts often concentrate in bands. Divide land into annuli from the reference center:

```text
annulus_width_m: 1500-3000 m
ignore_radius_m: first 1500 m around reference center
ignore_coast_buffer_m: 1000 m from coast/beach
```

For each annulus, compute radial alignment fractions. Record the maximum annular score. This catches cases where the global average looks acceptable but a mid-island ring is full of spokes.

### Height Profile And Ring Method

Radial falloff artifacts appear when height is too well explained by distance from a center.

Compute:

```text
height_vs_radius_r2
slope_vs_radius_r2
curvature_ring_peak
circular_edge_hough_peak
azimuthal_variance_by_radius
```

A natural island can have broad elevation gradients. It should not have a strong circular height profile with low azimuthal variance across large annuli.

### Radial Alignment Targets

| Metric | Target | Warning | Hard fail |
|---|---:|---:|---:|
| `skeleton_radial_aligned_15_fraction` | `<= 0.27` | `0.27-0.34` | `> 0.34` |
| `skeleton_radial_aligned_22_5_fraction` | `<= 0.42` | `0.42-0.50` | `> 0.50` |
| `annular_max_radial_aligned_15_fraction` | `<= 0.38` | `0.38-0.48` | `> 0.48` |
| `mean_resultant_length_01` | `<= 0.25` | `0.25-0.35` | `> 0.35` |
| `radial_height_profile_r2` | `<= 0.40` | `0.40-0.55` | `> 0.55` |
| `circular_ring_accumulator_peak_01` | `<= 0.30` | `0.30-0.45` | `> 0.45` |

### Radial Diagnostic Image

The radial alignment preview should show:

- reference center marker;
- annular bands;
- skeleton pixels colored by angular difference from radial;
- red/orange for strong radial alignment;
- separate volcano-center, island-centroid, and map-center panels if practical;
- a text legend with the key failing metric values.

This image is developer evidence only and must not become a player overlay.

## Catchment Diversity Metrics

Catchments should prove that the island has a readable drainage system with diverse basins, outlets, shapes, and flow paths.

### Catchment Extraction

Recommended pipeline:

1. Use land-only height raster with ocean masked.
2. Hydrologically condition the height field for analysis by filling or breaching tiny numerical pits while recording intervention amount.
3. Compute flow direction. D8 is acceptable for deterministic grid QA; MFD or D-Infinity can be added later for smoother diagnostic maps.
4. Compute flow accumulation.
5. Define channels using an accumulation threshold calibrated to island size.
6. Delineate catchments to ocean outlets.
7. Merge trivial coastal slivers below area threshold.
8. Identify major catchments.

Hydrologic conditioning must be analysis-only unless the terrain generator already performs equivalent terrain shaping. Do not silently modify accepted terrain based on the diagnostic pass.

### Major Catchment Criteria

A `major_catchment` should satisfy at least one area criterion and one flow criterion:

```text
area_fraction_of_land >= 0.0075
or area_km2 >= 1.5

and

max_flow_accumulation_quantile >= 0.92
or mainstem_length_km >= 2.5
```

For a 97.536 km island, the exact expected count depends on island shape, but a healthy procedural terrain should not produce only a few giant radial basins.

### Catchment Diversity Targets

| Metric | Target | Warning | Hard fail |
|---|---:|---:|---:|
| `major_catchment_count` | `12-45` | `8-11` or `46-65` | `< 8` or `> 80` |
| `catchment_area_entropy_01` | `>= 0.60` | `0.45-0.60` | `< 0.45` |
| `dominant_catchment_land_fraction` | `<= 0.18` | `0.18-0.28` | `> 0.28` |
| `outlet_spacing_cv` | `0.35-1.25` | `0.20-0.35` or `1.25-1.80` | `< 0.20` or `> 1.80` |
| `median_mainstem_sinuosity` | `>= 1.12` | `1.05-1.12` | `< 1.05` |
| `median_drainage_curvature_deg_per_km` | `>= 5.0` | `2.5-5.0` | `< 2.5` |
| `flow_accumulation_gini` | `0.45-0.80` | `0.30-0.45` or `0.80-0.90` | `< 0.30` or `> 0.90` |

Interpretation:

- Very low outlet spacing variance suggests evenly spaced procedural slices.
- Very low mainstem sinuosity suggests straight radial spokes.
- Very high dominant catchment fraction suggests the terrain funnels too much of the island into one basin.
- Very low entropy suggests repetitive basin sizes or too few meaningful catchments.
- Extremely high total catchment count may mean noisy fragmentation rather than good diversity.

### Catchment ID Preview

The catchment map should:

- assign unique colors to major catchments;
- use muted or grouped color for minor slivers;
- mark ocean outlets;
- show catchment boundaries;
- optionally label major catchment IDs used in the manifest.

## Ridge / Valley / Flow Metrics

Ridges and valleys are the visual language of interior terrain. The suite needs to verify they are branched, curved, terrain-following, and not mechanically radial.

### Ridge Extraction

Potential ridge masks:

- positive plan/profile curvature ridgelines;
- local maxima in height across slope-normal direction;
- watershed divide boundaries between catchments;
- high TPI at medium/large windows.

### Valley Extraction

Potential valley masks:

- high flow accumulation channels;
- negative curvature valley bottoms;
- low TPI drainage positions;
- flow convergence zones.

### Skeleton Metrics

| Metric | Target | Warning | Hard fail |
|---|---:|---:|---:|
| `ridge_skeleton_length_density_km_per_100km2` | `12-45` | `< 12` or `> 60` | `< 6` or `> 90` |
| `valley_skeleton_length_density_km_per_100km2` | `15-55` | `< 15` or `> 75` | `< 8` or `> 100` |
| `ridge_valley_length_ratio` | `0.55-1.75` | `0.35-0.55` or `1.75-2.50` | `< 0.35` or `> 2.50` |
| `branch_node_density_per_100km2` | `8-45` | `< 8` | `< 4` |
| `dead_straight_segment_fraction` | `<= 0.22` | `0.22-0.35` | `> 0.35` |
| `valley_follows_flow_fraction` | `>= 0.80` | `0.65-0.80` | `< 0.65` |
| `ridge_valley_crossing_error_count` | `0` | `1-3` | `> 3` |

### Ridge / Valley Overlay

The overlay must use the color relief or height raster as a base and draw:

- ridges;
- valleys/channels;
- major catchment boundaries;
- volcano footprint outline;
- rejected/low-confidence skeleton segments if useful.

This image is the fastest way to catch “spoke terrain” visually.

## Slope And Circular Artifact Detection

Slope and curvature should be used both as images and metrics.

### Slope Metrics

Compute land-only slope in degrees or percent grade. Record:

```text
slope_median_deg
slope_p75_deg
slope_p95_deg
flat_interior_fraction
steep_area_fraction
slope_entropy_01
```

Suggested targets:

| Metric | Target | Warning | Hard fail |
|---|---:|---:|---:|
| `flat_interior_fraction` | `<= 0.35` | `0.35-0.50` | `> 0.50` |
| `slope_p75_deg` | `>= 4.0°` | `2.5-4.0°` | `< 2.5°` |
| `slope_p95_deg` | `<= 45°` unless cliffs intended | `45-55°` | `> 55° widespread` |
| `slope_entropy_01` | `>= 0.50` | `0.35-0.50` | `< 0.35` |

Exclude beach/ocean bands from interior slope metrics. Otherwise a wide valid beach can falsely lower slope diversity.

### Circular Artifact Metrics

Detect circular artifacts with three complementary methods:

1. **Radial profile fit:** regress height, slope, and curvature against radius from the volcano, island centroid, and map center.
2. **Ring edge accumulator:** run circle/ring detection over curvature or slope discontinuity edges at plausible radii.
3. **Annular variance check:** measure whether pixels at the same radius are too similar compared with nearby radii.

Suggested hard fail:

```text
if radial_height_profile_r2 > 0.55
or circular_ring_accumulator_peak_01 > 0.45
or (radial_height_profile_r2 > 0.45 and annular_max_radial_aligned_15_fraction > 0.42):
    hard_fail: CircularFalloffArtifact
```

Reviewers should inspect `slope`, `radial_alignment_diagnostic`, and `color_relief` together when these metrics warn.

## Lava / Lahar Terrain-Following Metrics

This section remains terrain-only. It validates that lava/lahar terrain features follow plausible topography instead of drawing arbitrary radial stripes.

### Terrain-Following Requirements

For every lava or lahar channel/fan mask:

- channel centerline should trend downslope except for controlled local flats;
- channel should preferentially occupy valleys or flow-accumulation corridors;
- channel should not cross major ridges without a breach/pass;
- fan deposits should appear where slope decreases, not on steep upper cone walls;
- lahar routes should be hydrologically consistent with downstream drainage.

### Metrics

| Metric | Target | Warning | Hard fail |
|---|---:|---:|---:|
| `lava_downslope_step_fraction` | `>= 0.85` | `0.70-0.85` | `< 0.70` |
| `lahar_flow_accumulation_overlap` | `>= 0.65` | `0.50-0.65` | `< 0.50` |
| `ridge_crossing_unexplained_count` | `0` | `1-2` | `> 2` |
| `fan_slope_drop_fraction` | `>= 0.70` | `0.55-0.70` | `< 0.55` |
| `deposit_on_excessive_slope_fraction` | `<= 0.15` | `0.15-0.25` | `> 0.25` |
| `radial_lava_straightness_fraction` | `<= 0.30` | `0.30-0.45` | `> 0.45` |

These metrics should not force real-world volcanology simulation. Their purpose is to catch obvious terrain-following failures and radial stripe artifacts.

## Landform Region Diversity Metrics

Landform regions should explain the island at a medium scale: coastal plains, interior ridges, basin floors, uplands, valley corridors, fans, saddles, scarps, and volcanic zones. The suite should not require every named class in every seed, but it should require enough region diversity that the island does not read as one monotone mass.

### Region Classification Inputs

Use multiple terrain derivatives:

- elevation bands;
- slope bands;
- TPI at small/medium/large windows;
- curvature;
- flow accumulation;
- catchment boundaries;
- volcano influence;
- distance to coast;
- fan/lahar/lava terrain masks where present.

### Region Counting Rules

A `named_region` should be connected or spatially coherent and exceed minimum size:

```text
min_named_region_area_fraction: 0.005 of land
min_named_region_area_km2: 0.75 km2
major_region_area_fraction: 0.01 of land
```

Do not count tiny speckle regions. Apply morphological cleanup and report the amount removed.

### Region Diversity Targets

| Metric | Target | Warning | Hard fail |
|---|---:|---:|---:|
| `named_region_count` | `8-24` | `6-7` or `25-36` | `< 6` or `> 40` |
| `major_region_count` | `6-18` | `4-5` | `< 4` |
| `landform_entropy_01` | `>= 0.58` | `0.45-0.58` | `< 0.45` |
| `dominant_landform_fraction` | `<= 0.35` | `0.35-0.50` | `> 0.50` |
| `lowland_fraction` | `0.35-0.70` | `0.25-0.35` or `0.70-0.80` | `< 0.25` or `> 0.80` |
| `region_adjacency_diversity_01` | `>= 0.45` | `0.30-0.45` | `< 0.30` |

### Landform Region Map

The `landform_region_map` should be legible without gameplay labels. Use a developer legend in the image margin or metadata. It may include classes such as:

```text
CoastalPlain
BeachBackshore
LowlandBasin
AlluvialFan
InteriorValley
DissectedUpland
RidgeComplex
HighlandMassif
VolcanicCone
CraterRim
LavaApron
LaharCorridor
SaddlePass
ScarpOrCliff
```

These are terrain morphology classes only; they are not biomes or gameplay regions.

## Hard Gates Versus Review Warnings

Hard gates should block acceptance automatically. Review warnings should require human inspection but not necessarily fail the batch.

### Hard Gates

| Gate ID | Blocks when |
|---|---|
| `CoastValidationFailed` | Any preserved coast gate fails |
| `MissingPreviewEvidence` | Any required preview image or manifest field is missing |
| `WorldSizeMismatch` | `world_size_m` differs from `97536.0` beyond accepted floating tolerance |
| `OneFeatureTerrain` | dominant landform, highland, catchment, or volcano footprint exceeds hard threshold |
| `OversizedVolcanoFootprint` | volcano footprint/core/dominance exceeds hard threshold |
| `WeakBaseTerrain` | volcano-disabled terrain fails relief/diversity/catchment thresholds |
| `RadialSpokeRidges` | skeleton radial alignment exceeds hard threshold |
| `CircularFalloffArtifact` | radial height/ring artifact metrics exceed hard threshold |
| `HydrologyTooUniform` | catchments/outlets are too uniform, too few, or too straight |
| `CatchmentCollapse` | too few major catchments or one catchment dominates |
| `RegionMonotony` | named regions/entropy below hard threshold |
| `TerrainFollowingFailed` | lava/lahar masks violate terrain-following hard thresholds |
| `MetricComputationInvalid` | metrics contain NaN, invalid masks, empty land set, or inconsistent units |

### Review Warnings

| Warning ID | Meaning |
|---|---|
| `VolcanoNearDominanceLimit` | volcano is large but not an automatic fail |
| `RadialAlignmentNeedsReview` | radial score is high enough to inspect overlays |
| `CatchmentUniformityNeedsReview` | outlets/basins may be too regular |
| `NoVolcanoTerrainWeakButNotFailing` | base terrain is acceptable but marginal |
| `SlopeDistributionNeedsReview` | terrain may be too flat or too steep |
| `RegionOverFragmentation` | too many tiny morphology regions after cleanup |
| `CircularSignalNeedsReview` | radial/ring signal is present but below hard fail |
| `EvidenceResolutionReduced` | evidence rendered below preferred resolution due to resource constraints |

### Metrics That Must Block Acceptance Even If Coast Metrics Pass

The following should fail the batch regardless of shoreline quality:

- `skeleton_radial_aligned_15_fraction > 0.34`;
- `annular_max_radial_aligned_15_fraction > 0.48`;
- `radial_height_profile_r2 > 0.55`;
- `circular_ring_accumulator_peak_01 > 0.45`;
- `volcano_footprint_land_fraction > 0.18`;
- `volcano_core_land_fraction > 0.09`;
- `contiguous_highland_max_land_fraction > 0.38`;
- `non_volcanic_relief_p95_p05_m < 120 m`;
- `no_volcano_named_region_count < 6`;
- `major_catchment_count < 8`;
- `dominant_catchment_land_fraction > 0.28`;
- `median_mainstem_sinuosity < 1.05`;
- `named_region_count < 6`;
- `dominant_landform_fraction > 0.50`;
- any missing required preview output;
- any NaN/invalid metric used by a hard gate.

## Files And Areas Likely Touched

Likely implementation areas in a UE5/Linux-first project:

```text
Source/JungleGame/
  Terrain/
    TerrainPreviewRenderer.*
    TerrainValidationMetrics.*
    TerrainMorphologyMetrics.*
    TerrainHydrologyAnalysis.*
    TerrainRegionClassifier.*
    TerrainPreviewManifest.*
    TerrainMaskAtlas.*
    TerrainSkeletonization.*
    TerrainVolcanoDiagnostics.*
  Automation/
    TerrainPreviewCommandlet.*
    TerrainValidationCommandlet.*
  Tests/
    TerrainMorphologyValidationTests.*
    TerrainPreviewManifestTests.*
    TerrainHydrologyAnalysisTests.*
Config/
  TerrainValidation.ini
  TerrainPreviewEvidence.ini
Images/
  TerrainPreview/
Docs/
  Terrain/
    MorphologyValidation.md
```

The actual project names should match existing source layout. The PR should avoid broad architecture rewrites unless the current terrain preview system cannot expose height/mask buffers cleanly.

### Config Fields

Recommended config:

```ini
[TerrainValidation.Morphology]
EvidenceResolutionPx=4096
QuickPreviewResolutionPx=2048
WorldSizeM=97536.0
MinMajorCatchmentAreaFraction=0.0075
MinNamedRegionAreaFraction=0.005
VolcanoFootprintInfluenceThreshold=0.25
VolcanoCoreInfluenceThreshold=0.75
RadialAligned15Fail=0.34
RadialAligned225Fail=0.50
VolcanoFootprintFail=0.18
NoVolcanoReliefFailM=120.0
MajorCatchmentCountMin=8
NamedRegionCountMin=6
```

## Large PR Implementation Scope

This is a large but coherent PR. It should be implemented as an evidence/validation feature, not as a visual terrain redesign PR.

### In-Scope Work

1. Add terrain analysis buffers:
   - land mask;
   - ocean/beach masks;
   - height;
   - slope;
   - curvature/TPI if not already available;
   - volcano contribution/influence;
   - volcano-disabled height.

2. Add hydrology analysis:
   - pit filling/breaching for analysis;
   - flow direction;
   - flow accumulation;
   - catchment delineation;
   - outlet extraction;
   - mainstem extraction.

3. Add morphology analysis:
   - landform region classification;
   - ridge/valley skeleton extraction;
   - radial alignment diagnostics;
   - circular falloff/ring diagnostics;
   - volcano dominance metrics.

4. Add preview rendering:
   - required PNG outputs;
   - deterministic naming;
   - optional contact sheet.

5. Add manifest:
   - schema version;
   - output paths;
   - metrics;
   - thresholds;
   - pass/fail reasons;
   - review summary.

6. Add gates:
   - preserve current coast gates;
   - add hard morphology gates;
   - add review warnings.

7. Add tests:
   - manifest schema serialization;
   - threshold pass/fail classification;
   - synthetic radial-spoke terrain fails;
   - synthetic diverse terrain passes basic morphology gates;
   - volcano-disabled flat terrain fails;
   - missing preview output fails.

### Out-of-Scope Work

- redesigning terrain generation itself beyond exposing diagnostic buffers;
- adding gameplay-driven regions;
- adding player-visible debug UI;
- adding ecology/foliage/material systems;
- replacing existing coast validation;
- accepting “looks good in one screenshot” as a substitute for metrics.

## Acceptance Criteria

A batch is acceptable only when all criteria below are met.

### Evidence Completeness

- All required preview outputs exist.
- Manifest exists and parses successfully.
- Manifest schema version is present.
- Every image path in the manifest points to a generated file.
- Preview images use deterministic batch/seed naming.
- Evidence resolution is recorded.

### Coast Preservation

- `shoreline_error_max_m` is `0` or effectively `0`.
- `beach_continuity_pct` is `100%`.
- `ocean_below_sea_pct` is `100%`.
- `square_edge_ocean_violations` is `0`.
- `world_size_m` targets `97536.0`.

### Morphology

- Volcano footprint and dominance are within target or acceptable warning ranges.
- Radial alignment metrics do not hit hard-fail thresholds.
- Circular falloff metrics do not hit hard-fail thresholds.
- Volcano-disabled terrain remains interesting by relief, slope, regions, and catchments.
- Major catchment count and entropy are acceptable.
- Outlet spacing is not suspiciously uniform.
- Drainage curvature/sinuosity is not too straight.
- Landform region count and entropy are acceptable.
- No single landform/catchment/highland component dominates the island.

### Review

- `hard_gate_failures[]` is empty.
- Any `review_warnings[]` are inspectable with named preview images.
- `review_summary` clearly states acceptance rationale.
- If the batch fails, failure reasons are specific enough for the terrain developer to act.

## Risks And Stop Conditions

### Risks

1. **Overfitting thresholds too early.** Initial thresholds should catch known bad terrain but may need calibration over multiple seeds.
2. **False positives from real volcanic radial drainage.** Some radial drainage near a volcanic cone is plausible. The gate should fail island-wide spokes and annular/mechanical alignment, not every local volcanic channel.
3. **Hydrology artifacts from analysis preprocessing.** Depression filling can create artificial flow if over-applied. Record conditioning deltas.
4. **Resolution sensitivity.** Skeleton and catchment metrics can shift with raster resolution. Record resolution and keep acceptance evidence consistent.
5. **Mask dependency bugs.** Volcano-disabled terrain must not accidentally disable unrelated base terrain.
6. **Metric-only blindness.** Hard gates catch known failures, but review images remain important for edge cases.

### Stop Conditions

Stop and report a blocked validation state when:

- land/ocean masks are inconsistent with coast validation;
- height units are unknown or inconsistent;
- volcano contribution cannot be isolated and no reliable delta estimate can be generated;
- flow accumulation produces invalid/NaN output;
- catchments cannot be delineated to ocean outlets;
- required preview images cannot be written;
- manifest paths cannot be made deterministic;
- any hard gate relies on an empty sample set.

A blocked validation state is not a pass. It is a failed evidence run that must be fixed before terrain acceptance.

## Rejected Or Forbidden Approaches

Do not accept any of the following:

- lowering coast gates to make morphology pass;
- replacing hard morphology gates with only subjective screenshots;
- accepting the full volcano render without checking volcano-disabled terrain;
- checking radial alignment only by eye;
- using a single color relief image as proof of catchment diversity;
- counting tiny noisy mask fragments as named regions;
- allowing a large volcano footprint because “volcanoes are radial”;
- using player-facing map/compass/minimap/GPS/objective marker overlays as evidence;
- adding foliage or materials to hide terrain monotony;
- post-processing preview images to make terrain look better than the raw height data;
- hiding failed metrics from the manifest;
- treating missing metrics as warnings;
- accepting NaN/invalid metrics as zero.

## Sources And References

These references support the proposed diagnostic categories and analysis methods. The suite adapts established terrain-analysis concepts to procedural terrain QA; the exact thresholds above are project-specific acceptance gates that should be calibrated against Jungle Game seeds.

1. USGS, **Hydrologically Conditioned and Enforced DEM**. Notes that hydro-conditioned DEMs support watershed boundaries, flow direction, and flow accumulation derivatives, and that DEM cells should flow to an outlet point or true sink.  
   <https://www.usgs.gov/ngp-standards-and-specifications/elevation-derived-hydrography-data-acquisition-specifications-15>

2. USGS, **Hydrologic Derivatives for Modeling and Analysis**. Describes hydrologic derivative layers including DEM, flow direction, flow accumulation, slope, compound topographic index, streams, and catchment boundaries.  
   <https://www.usgs.gov/publications/hydrologic-derivatives-modeling-and-analysis-a-new-global-high-resolution-database>

3. Esri ArcGIS Pro Documentation, **Flow Accumulation**. Describes accumulated flow rasters and D8, Multiple Flow Direction, and D-Infinity flow methods.  
   <https://pro.arcgis.com/en/pro-app/3.4/tool-reference/spatial-analyst/flow-accumulation.htm>

4. Esri ArcGIS Pro Documentation, **Hydrologic analysis sample applications**. Summarizes hydrologic tools for sinks, flow direction, flow accumulation, watersheds, and stream networks.  
   <https://pro.arcgis.com/en/pro-app/3.4/tool-reference/spatial-analyst/hydrologic-analysis-sample-applications.htm>

5. GRASS GIS Manual, **r.watershed**. Documents generation of flow accumulation, drainage direction, stream locations, watershed basins, and related terrain factors.  
   <https://grass.osgeo.org/grass-stable/manuals/r.watershed.html>

6. Pike, R. J., Evans, I. S., and Hengl, T., **Geomorphometry: A Brief Guide**, in *Geomorphometry: Concepts, Software, Applications*, 2008. Establishes DEM-based geomorphometric terrain analysis as a basis for deriving terrain variables.  
   <https://www.geomorphometry.org/uploads/pdf/Pike_2008_Geomorphometry_ch1.pdf>

7. SAGA GIS Tool Library, **Slope, Aspect, Curvature**. Documents local morphometric parameters including slope, aspect, and curvature components.  
   <https://saga-gis.sourceforge.io/saga_tool_doc/7.6.3/ta_morphometry_0.html>

8. Weiss, A. D., **Topographic Position and Landforms Analysis**, 2001. Presents multi-scale Topographic Position Index concepts for landform classification.  
   <https://www.jennessent.com/downloads/TPI-poster-TNC_18x22.pdf>

9. Berens, P., **CircStat: A MATLAB Toolbox for Circular Statistics**, *Journal of Statistical Software*, 2009. Supports mean resultant length and circular spread/statistical tests for directional data.  
   <https://www.jstatsoft.org/article/view/v031i10/335>

10. Duda, R. O. and Hart, P. E., **Use of the Hough Transformation to Detect Lines and Curves in Pictures**, *Communications of the ACM*, 1972. Foundational reference for Hough-style line/curve detection in images.  
    <https://dl.acm.org/doi/10.1145/361237.361242>

11. scikit-image Documentation, **Circular and Elliptical Hough Transforms**. Practical reference for detecting circular or elliptical structures from edge evidence using Hough transforms.  
    <https://scikit-image.org/docs/0.25.x/auto_examples/edges/plot_circular_elliptical_hough_transform.html>

12. Barnes, R., Lehman, C., and Mulla, D., **Priority-Flood: An Optimal Depression-Filling and Watershed-Labeling Algorithm for Digital Elevation Models**. Relevant for efficient DEM depression filling and watershed labeling concepts.  
    <https://arxiv.org/abs/1511.04463>
