# PR5 Batch 006 Research Prompt 003 — Morphology Validation And Preview Evidence Suite

You are an independent no-context researcher AI working from this prompt only.

Your job is to produce a Markdown research/spec document for **Jungle Game**, a UE5 Linux-first first-person survival-simulation project.

## Absolute final deliverable requirement

Your final answer must include a downloadable Markdown file attachment named exactly:

```text
003_morphology_validation_preview_suite_research.md
```

## Project context

Jungle Game has deterministic terrain preview evidence for a 97.536 km island. Current hard coast gates pass, but the interior still fails visually because the volcano dominates and the ridge/gully system reads as radial procedural spokes.

This prompt focuses on developer-only terrain morphology validation and preview evidence. The suite must prove terrain quality, not just shoreline correctness.

## Terrain-only scope boundary

This is validation, preview output, diagnostic metrics, manifest schema, and terrain QA only.

Do not design foliage, ecology, weather, audio, NPCs, gameplay systems, buildings, final materials, UI, map, compass, minimap, GPS, objective markers, or player-facing overlays.

## Critical preservation requirement

Keep existing coast gates as hard requirements:

```text
shoreline_error_max_m: 0 or effectively 0
beach_continuity_pct: 100%
ocean_below_sea_pct: 100%
square_edge_ocean_violations: 0
world_size_m: 97536.0 target
```

Add morphology gates that catch one-feature terrain, radial ridge/gully structure, circular falloff artifacts, weak landform diversity, oversized volcano footprint, and boring volcano-disabled terrain.

## Required preview outputs

Define these outputs:

- color relief;
- height;
- slope;
- terrain mask atlas;
- volcano-disabled height;
- landform region map;
- catchment ID map;
- flow accumulation map;
- ridge/valley skeleton overlay;
- radial alignment diagnostic;
- volcano footprint mask;
- JSON manifest.

## Questions the research must answer

1. Which morphology metrics should be hard gates vs review warnings?
2. How can radial terrain be detected numerically?
3. How can the suite prove terrain remains interesting with the volcano disabled?
4. How should volcano footprint and highland dominance be measured?
5. How should catchment, ridge, valley, fan, and region diversity be measured?
6. How should preview images be named and grouped per batch under `Images/TerrainPreview/`?
7. How should the manifest explain pass/fail reasons in a fresh-chat review?
8. What metrics should block acceptance even if coast metrics pass?

## Required Markdown structure

```markdown
# Morphology Validation And Preview Evidence Suite Research

## Executive Summary
## Terrain-Only Scope Boundary
## Why Coast Metrics Are Not Enough
## Batch 004 Failure Modes To Detect
## Required Preview Outputs
## Manifest Schema Additions
## Volcano-Disabled Terrain Validation
## Volcano Footprint And Feature Dominance Metrics
## Radial Alignment Artifact Metrics
## Catchment Diversity Metrics
## Ridge / Valley / Flow Metrics
## Slope And Circular Artifact Detection
## Lava / Lahar Terrain-Following Metrics
## Landform Region Diversity Metrics
## Hard Gates Versus Review Warnings
## Files And Areas Likely Touched
## Large PR Implementation Scope
## Acceptance Criteria
## Risks And Stop Conditions
## Rejected Or Forbidden Approaches
## Sources And References
```

## Implementation usefulness requirements

Give concrete guidance for metric structures such as `FTerrainMorphologyMetrics`, `FVolcanoDominanceMetrics`, `FRadialArtifactMetrics`, `FCatchmentDiversityMetrics`, `FPreviewEvidenceManifest`, and `FTerrainPreviewOutputSet`.

Define target ranges and fail conditions for volcano footprint, radial alignment, lowland fraction, named region count, catchment count, outlet spacing variance, drainage curvature, and volcano-disabled relief.

## Source expectations

Use reliable sources on DEM validation, hydrology metrics, terrain analysis, slope analysis, geomorphology, and procedural terrain validation. Cite sources in the Markdown file.

## Final instruction

Your final output must be a downloadable Markdown file named exactly:

```text
003_morphology_validation_preview_suite_research.md
```
