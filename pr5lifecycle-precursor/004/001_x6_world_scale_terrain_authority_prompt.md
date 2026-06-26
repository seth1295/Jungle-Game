# PR5 Batch 004 Research Prompt 001 — 6x World Scale Terrain Authority

You are an independent no-context researcher AI working from this prompt only.

Your job is to produce a Markdown research/spec document for **Jungle Game**, a UE5 Linux-first first-person project.

## Absolute final deliverable requirement

Your final answer must include a **downloadable Markdown file attachment** named exactly:

```text
001_x6_world_scale_terrain_authority_research.md
```

## Project context

Jungle Game currently has a deterministic PR5 Batch 003 volcanic island terrain source and preview exporter. The current preview evidence reports:

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

The coastline, beach ring, shallow shelf, ocean-below-sea behavior, and square-edge ocean acceptance are working. The issue is scale and interior terrain morphology. The user now requires the map to be **6x bigger in linear world size**, not a minor upscale.

This means the implementation target should treat the current 16.256 km world as too small and move toward roughly:

```text
new_world_size_m: 97536.0
current_mean_island_radius_m: ~7000.0
new_mean_island_radius_m: ~42000.0
current_max_island_radius_m: ~7350.0
new_max_island_radius_m: ~44100.0
```

This is a 6x linear scale change and about 36x area. Do not frame it as a tiny tuning patch.

## Terrain-only scope boundary

This is terrain source, map scale, heightfield architecture, preview evidence, and validation only.

Do not design foliage, ecology, weather, audio, survival systems, NPCs, VFX, final materials, buildings, POIs, quests, or player UI.

Do not recommend player-facing map/minimap/compass/GPS/objective/bearing/time/orientation UI.

## Critical preservation requirement

The existing coast acceptance is valuable and must not be broken by the scale rewrite:

```text
shoreline_error_max_m must remain 0 or effectively 0 in preview evidence
beach_continuity_pct must remain 100%
ocean_below_sea_pct must remain 100%
square_edge_ocean_violations must remain 0
outer world edge must remain ocean/container terrain
island perimeter must remain beach/coastal shelf/sea-level transition
```

## Research target

Produce a major-implementation spec for scaling the canonical volcanic island terrain source from the current 16.256 km world to a 97.536 km-class world while keeping deterministic source identity, coast constraints, preview/export evidence, and future UE runtime feasibility.

Answer:

1. What constants and derived values must change for a 6x linear world scale?
2. Should vertical relief be kept near current height, scaled partially, or redesigned for a large-island volcanic profile?
3. What terrain features should scale linearly, sublinearly, or stay fixed-width?
4. How should beach, shelf, shore lock, and square-edge ocean constraints survive the larger world?
5. How should source resolution, preview resolution, and runtime mesh/LOD identity adapt to 97.536 km?
6. What validation metrics should prove the scale change is real and not just a preview zoom?
7. What implementation files/classes are likely touched?
8. What risks should block the PR?

## Required Markdown structure

```markdown
# 6x World Scale Terrain Authority Research

## Executive Summary
## Terrain-Only Scope Boundary
## Current Terrain Baseline
## Required 6x Scale Target
## World Constants And Derived Values
## Horizontal Scale Strategy
## Vertical Relief Strategy
## Coast / Beach / Shelf Preservation
## Mountain, Basin, Ridge, Gully, Crater Scale Rules
## Source Resolution And Runtime Mesh Consequences
## Preview Export Consequences
## Determinism And Config Identity
## Developer-Only Validation Metrics
## Files And Areas Likely Touched
## Large PR Implementation Scope
## Acceptance Criteria
## Risks And Stop Conditions
## Rejected Or Forbidden Approaches
## Sources And References
```

## Implementation usefulness requirements

Give concrete guidance for:

- exact proposed world-size constants;
- exact proposed island radius constants;
- which features scale by 6x and which should not;
- recommended vertical relief target for a 97.536 km volcanic island;
- how to avoid making the volcano look flat after the horizontal scale increase;
- preview exporter output naming and manifest fields;
- runtime logs needed to prove scale authority;
- source config/version/fingerprint changes.

## Source expectations

Use reliable sources where UE5 large worlds, World Partition, Large World Coordinates, Landscape sizing, DEM/terrain resolution, volcanic island morphology, coastline modelling, or heightfield validation matter. Cite sources in the Markdown file.

## Final instruction

Your final output must be a downloadable Markdown file named exactly:

```text
001_x6_world_scale_terrain_authority_research.md
```
