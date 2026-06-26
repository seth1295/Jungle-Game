# PR5 Batch 003 Research Prompt 008 — Heightfield Export And Topographic Comparison Implementation

You are an independent no-context researcher AI working from this prompt only.

Your job is to produce a Markdown research/spec document for **Jungle Game**, a UE5 Linux-first first-person project.

## Absolute final deliverable requirement

Your final answer must include a **downloadable Markdown file attachment** named exactly:

```text
008_heightfield_export_topographic_comparison_research.md
```

## Project context

Jungle Game’s terrain-generation work needs objective evidence. The current runtime mesh was mapped after commit and showed roughly -101 m to +140 m elevation and 0 to 7 degree slopes. PR5 Batch 003 must use actual runtime/source terrain exports to drive large implementation decisions instead of vibes-only ground inspection.

This prompt is terrain validation only: exporting heightfields/vertices/masks, generating topographic maps, comparing iterations, and gating terrain implementation quality.

## Critical coastline requirement

The validation workflow must explicitly test the circular island edge:

```text
0 m contour must track the intended beach ring
outside island mask must be ocean/below sea level
beach/coastal shelf band must be measurable
square-world edge must not contain raised terrain artifacts
ridge/gully systems must terminate coherently at coast
```

## Terrain-only scope boundary

Do not design player UI, final art, foliage, ecology, weather, audio, survival mechanics, NPCs, story, VFX, or materials. This is developer-only terrain export and comparison.

Do not recommend player-facing map/minimap/compass/GPS/objective/bearing/time/orientation UI.

## Research target

Produce a major-implementation spec for terrain export, topographic map generation, and iteration comparison.

Answer:

1. How should the project export actual runtime mesh vertices and/or source heightfield samples?
2. What topo maps are required: elevation, hillshade, contours, slope, aspect, island mask, beach ring, ocean mask, catchment/flow, ridge/gully, crater/hazard?
3. What metadata proves a map came from the exact runtime terrain config?
4. How should iterations be compared numerically and visually?
5. What thresholds prove the terrain changed meaningfully toward the 1400 m volcanic island goal?
6. How should generated maps/CSVs/logs remain ignored and unstaged?
7. What evidence report should close each major terrain PR?

## Required Markdown structure

```markdown
# Heightfield Export And Topographic Comparison Research

## Executive Summary
## Terrain-Only Scope Boundary
## Current Validation Baseline
## Major Implementation Goal
## Actual Runtime Mesh / Source Heightfield Export Requirements
## Required Topographic Map Outputs
## Sea-Level Beach Ring Validation
## Elevation / Relief / Slope Metrics
## Ridge / Gully / Catchment Metrics
## Crater / Hazard Mask Metrics
## Iteration Comparison Workflow
## Runtime Metadata And Log Tokens
## Artifact Hygiene Rules
## Evidence Report Template
## Large PR Implementation Scope
## Files And Areas Likely Touched
## Acceptance Criteria
## Risks And Stop Conditions
## Rejected Or Forbidden Approaches
## Sources And References
```

## Implementation usefulness requirements

Give concrete guidance for:

- export filenames;
- metadata fields;
- map resolutions;
- contour intervals;
- numeric metrics;
- pass/fail criteria;
- source-control ignore rules;
- CI/manual validation commands where useful;
- terrain PR evidence format.

This is terrain validation only.

## Source expectations

Use reliable sources where DEM analysis, slope/catchment/topographic mapping, heightfield export, UE mesh/Landscape data, artifact hygiene, or validation reporting matter. Cite sources in the Markdown file.

## Final instruction

Your final output must be a downloadable Markdown file named exactly:

```text
008_heightfield_export_topographic_comparison_research.md
```
