# PR5 Batch 006 Research Prompt 001 — DEM Benchmarking And Real Volcanic Island Calibration

You are an independent no-context researcher AI working from this prompt only.

Your job is to produce a Markdown research/spec document for **Jungle Game**, a UE5 Linux-first first-person survival-simulation project.

## Absolute final deliverable requirement

Your final answer must include a downloadable Markdown file attachment named exactly:

```text
001_dem_benchmarking_real_volcanic_islands_research.md
```

## Project context

Jungle Game needs a new terrain math foundation for a 97.536 km tropical volcanic island. Previous deterministic preview evidence proves coast metrics can pass, but the island interior still reads as a procedural radial volcano. Batch 005 asks for the new math core. Batch 006 expands that ambition by requiring the terrain to be benchmarked against real volcanic island DEM/topographic principles.

This prompt focuses on **real-world calibration and measurable terrain targets**.

## Terrain-only scope boundary

This is terrain research, DEM/topographic comparison, validation metrics, and calibration targets only.

Do not design foliage, ecology, weather, audio, NPCs, buildings, survival systems, final materials, UI, maps, compass, minimap, GPS, objective markers, or player-facing overlays.

## Critical preservation requirement

Any benchmarking recommendation must preserve Jungle Game's working constraints:

```text
world_size_m: 97536.0 target
shoreline_error_max_m: 0 or effectively 0
beach_continuity_pct: 100%
ocean_below_sea_pct: 100%
square_edge_ocean_violations: 0
outer world edge remains ocean/container terrain
```

## Research target

Create a calibration framework for comparing Jungle Game terrain against real volcanic island DEM patterns without copying any real island.

The research should identify useful reference families such as:

- shield volcanic islands;
- stratovolcanic islands;
- eroded tropical volcanic islands;
- oceanic island chains;
- islands with flank collapse scars;
- islands with major catchments, deep valleys, coastal fans, and old lava benches.

The output must turn real terrain observations into implementation targets: slope histograms, hypsometry, basin area distributions, drainage density, elevation-relief ratios, cone footprint ranges, coastal lowland proportions, and ridge/valley curvature statistics.

## Questions the research must answer

1. Which real volcanic island types are most relevant as calibration families for a large jungle island game world?
2. What DEM-derived metrics should Jungle Game emulate without copying a real location?
3. What ranges are plausible for peak height, relief, mean slope, coastal lowland width, basin size, and drainage density?
4. How should an active volcano be scaled relative to a 97.536 km island?
5. How can terrain previews be compared against DEM-style evidence at 1024 px and later 8129-class source resolution?
6. What visual and numeric signatures distinguish real eroded island terrain from procedural radial artifacts?
7. How should the preview manifest report calibration results?
8. What reference data cautions or licensing concerns should developers avoid?

## Required Markdown structure

```markdown
# DEM Benchmarking And Real Volcanic Island Calibration Research

## Executive Summary
## Terrain-Only Scope Boundary
## Why Batch 004 Failed The Real-Terrain Test
## Relevant Volcanic Island Families
## DEM Metrics Worth Emulating
## Hypsometric Curve Targets
## Slope Histogram Targets
## Drainage Density And Basin Statistics
## Ridge / Valley Curvature Statistics
## Coastal Lowland And Fan Metrics
## Volcano Footprint And Relief Calibration
## Flank Collapse And Old Lava Terrain Calibration
## Preview Comparison Workflow
## Manifest Fields And Debug Outputs
## Files And Areas Likely Touched
## Large PR Implementation Scope
## Acceptance Criteria
## Risks And Stop Conditions
## Rejected Or Forbidden Approaches
## Sources And References
```

## Implementation usefulness requirements

Give concrete guidance for:

- metrics structures such as `FTopographicBenchmarkMetrics`, `FDemComparisonTarget`, and `FVolcanicIslandCalibrationProfile`;
- target ranges for lowland/highland/volcano proportions;
- slope class and relief class bins suitable for preview manifests;
- basin area and drainage density metrics;
- volcano-disabled DEM comparison;
- preview outputs: hypsometry plot data, slope histogram data, catchment area histogram, ridge/valley curvature map, calibration report JSON.

## Source expectations

Use reliable sources on DEM analysis, volcanic island geomorphology, hydrology, slope analysis, drainage density, hypsometry, landslide/flank collapse morphology, and terrain validation. Cite sources in the Markdown file.

## Final instruction

Your final output must be a downloadable Markdown file named exactly:

```text
001_dem_benchmarking_real_volcanic_islands_research.md
```
