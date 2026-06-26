# PR5 Batch 004 Research Prompt 003 — Scaled Terrain Preview Acceptance And Handoff

You are an independent no-context researcher AI working from this prompt only.

Your job is to produce a Markdown research/spec document for **Jungle Game**, a UE5 Linux-first first-person project.

## Absolute final deliverable requirement

Your final answer must include a **downloadable Markdown file attachment** named exactly:

```text
003_scaled_preview_acceptance_and_handoff_research.md
```

## Project context

Jungle Game has completed PR5 Batch 003 through terrain preview evidence. The latest deterministic preview exporter writes real sampled terrain evidence under:

```text
Images/TerrainPreview/
```

Do not route terrain preview evidence into deleted broad docs folders. Broad `docs/` and `Docs/` folders were intentionally removed. The terrain preview output is valuable, but it belongs in `Images/TerrainPreview/` or another explicit image/evidence subgroup, not `Docs/World`.

Current Batch 003 terrain preview metrics are clean for coast acceptance:

```text
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

Visual review of the exported images also showed the next required terrain fixes:

```text
map/world must become 6x bigger in linear size
interior ridge/gully drainage must stop reading as gear/fan-blade radial spokes
coast/beach/shelf acceptance must remain clean
```

## Terrain-only scope boundary

This is terrain preview/export/acceptance evidence only.

Do not design foliage, ecology, weather, audio, survival systems, NPCs, VFX, final materials, buildings, POIs, quests, or player UI.

Do not recommend player-facing map/minimap/compass/GPS/objective/bearing/time/orientation UI.

## Critical preservation requirement

The scaled preview and handoff must prove these acceptance gates:

```text
world_size_m is actually 97536.0 or equivalent 6x linear target
preview/manifest clearly proves the new scale, not just a zoomed image
shoreline_error_max_m remains 0 or effectively 0
beach_continuity_pct remains 100%
ocean_below_sea_pct remains 100%
square_edge_ocean_violations remains 0
ridge/gully radial regularity is reduced enough to reject gear/fan-blade morphology
terrain evidence is saved under Images/TerrainPreview or another explicit Images subgroup
no broad docs/Docs folder dependency returns
```

## Research target

Produce a major-implementation spec for the third PR in Batch 004: scaled preview evidence, anti-radial acceptance metrics, generated image/manifest naming, and the tracked handoff for the next lifecycle stage.

Answer:

1. What generated preview files should Batch 004 produce after the 6x scale and anti-radial rewrite?
2. What manifest fields should prove the terrain is truly 6x larger?
3. What metrics should prove coast acceptance did not regress?
4. What metrics should prove radial gear/fan-blade drainage was fixed?
5. What image outputs should expose color relief, height, slope, mask atlas, basin IDs, outlet distribution, and anti-radial diagnostics?
6. What files should be tracked versus ignored?
7. What should the Batch 004 handoff record before later terrain/ecosystem work resumes?
8. What implementation files/classes/scripts are likely touched?

## Required Markdown structure

```markdown
# Scaled Terrain Preview Acceptance And Handoff Research

## Executive Summary
## Terrain-Only Scope Boundary
## Current Batch 003 Preview Baseline
## Batch 004 Acceptance Goals
## Required Generated Images
## Manifest Schema
## 6x Scale Evidence Metrics
## Coast Preservation Metrics
## Anti-Radial Drainage Metrics
## Image Output Location Rules
## Artifact Hygiene Rules
## Developer-Only Validation Workflow
## Handoff Requirements
## Files And Areas Likely Touched
## Large PR Implementation Scope
## Acceptance Criteria
## Risks And Stop Conditions
## Rejected Or Forbidden Approaches
## Sources And References
```

## Implementation usefulness requirements

Give concrete guidance for:

- exact output path policy under `Images/TerrainPreview/`;
- output filename prefixes for Batch 004;
- manifest fields for 6x world scale;
- coast acceptance thresholds;
- anti-radial diagnostic metrics;
- preview overlays for basin IDs, outlet points, ridge/gully masks, slope, and hazard masks;
- how to compare Batch 003 and Batch 004 previews without using broad docs folders;
- handoff file naming under `handoffs/`;
- stop conditions if the preview still looks like a gear.

## Source expectations

Use reliable sources where terrain visualization, DEM validation, hydrology/drainage metrics, coastline validation, image-based terrain review, or UE5 large terrain evidence workflows matter. Cite sources in the Markdown file.

## Final instruction

Your final output must be a downloadable Markdown file named exactly:

```text
003_scaled_preview_acceptance_and_handoff_research.md
```
