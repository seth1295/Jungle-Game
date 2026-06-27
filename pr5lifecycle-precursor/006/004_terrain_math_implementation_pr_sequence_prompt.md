# PR5 Batch 006 Research Prompt 004 — Terrain Math Implementation PR Sequence

You are an independent no-context researcher AI working from this prompt only.

Your job is to produce a Markdown research/spec document for **Jungle Game**, a UE5 Linux-first first-person survival-simulation project.

## Absolute final deliverable requirement

Your final answer must include a downloadable Markdown file attachment named exactly:

```text
004_terrain_math_implementation_pr_sequence_research.md
```

## Project context

Jungle Game is moving beyond small terrain patches. The current 97.536 km terrain preview proves coastal gates can pass, but the interior still reads as procedural radial volcano terrain. The next work must be a serious terrain math rewrite built from geomorphology, hydrology, terrain graph structure, volcanic subsystem design, feature grammar, DEM-style calibration, architecture, and morphology validation.

This prompt converts that ambition into a sequenced implementation strategy.

## Terrain-only scope boundary

This is implementation sequencing for terrain source, generator tooling, preview evidence, validation, and runtime bridge only.

Do not design foliage, ecology, weather, audio, NPCs, gameplay systems, buildings, final materials, UI, map, compass, minimap, GPS, objective markers, or player-facing overlays.

## Critical preservation requirement

Every implementation slice must preserve or explicitly restore these hard gates:

```text
world_size_m: 97536.0 target
shoreline_error_max_m: 0 or effectively 0
beach_continuity_pct: 100%
ocean_below_sea_pct: 100%
square_edge_ocean_violations: 0
outer world edge remains ocean/container terrain
```

## Research target

Design a serious, ambitious implementation sequence for turning the new terrain math research into actual Jungle Game source/tooling changes.

The sequence should be bigger than a tiny patch but still ordered so each PR has clear source changes, preview evidence, validation, and rollback boundaries.

The output should define a likely PR order for:

- math core config and terrain authority replacement;
- landform region graph;
- hydrology/catchment/erosion passes;
- volcanic subsystem rework;
- feature grammar and multi-scale detail;
- preview exporter expansion;
- morphology validation gates;
- runtime terrain shell integration;
- final acceptance/handoff.

## Questions the research must answer

1. What is the safest ambitious PR sequence for the terrain math rewrite?
2. Which changes must land before hydrology, volcanology, preview validation, and runtime bridge work?
3. Which files/classes are likely touched per PR?
4. What validation should each PR run?
5. What generated preview evidence should each PR produce?
6. What stop conditions should prevent continuing to the next PR?
7. How should the implementation avoid docs-only or tiny fake changes?
8. How should Batch 005 and Batch 006 research files map into runtime implementation inputs?

## Required Markdown structure

```markdown
# Terrain Math Implementation PR Sequence Research

## Executive Summary
## Terrain-Only Scope Boundary
## Implementation Philosophy
## Dependency Graph
## Proposed PR Sequence Overview
## PR 1 — Terrain Math Core And Config Identity
## PR 2 — Landform Region Graph
## PR 3 — Hydrology / Catchment / Erosion Passes
## PR 4 — Volcanic Subsystem Rework
## PR 5 — Feature Grammar And Multi-Scale Detail
## PR 6 — Preview Exporter Expansion
## PR 7 — Morphology Validation Gates
## PR 8 — Runtime Terrain Shell Integration
## PR 9 — DEM Calibration / Benchmark Report
## PR 10 — Acceptance Handoff And Next Terrain Lane
## Validation Matrix
## Files And Areas Likely Touched
## Runtime Evidence Expectations
## Risks And Stop Conditions
## Rejected Or Forbidden Approaches
## Sources And References
```

## Implementation usefulness requirements

Give concrete guidance for source files, scripts, preview output folders, manifest fields, log tokens, and acceptance metrics per PR. The sequence must explicitly state that Batch 004 is historical failure evidence and new work begins from Batch 005/006 lifecycle inputs.

## Source expectations

Use reliable sources on software architecture for procedural terrain, UE5 terrain/runtime systems, validation pipelines, hydrology/erosion implementation strategy, and source-control artifact hygiene. Cite sources in the Markdown file.

## Final instruction

Your final output must be a downloadable Markdown file named exactly:

```text
004_terrain_math_implementation_pr_sequence_research.md
```
