# PR5 Batch 006 Research Prompt 005 — Full Terrain Math Acceptance And Handoff

You are an independent no-context researcher AI working from this prompt only.

Produce a Markdown research/spec document for **Jungle Game**, a UE5 Linux-first first-person survival-simulation project.

## Deliverable

Return a downloadable Markdown file named exactly:

```text
005_full_terrain_math_acceptance_handoff_research.md
```

## Context

Jungle Game has a deterministic 97.536 km volcanic island preview. Coast and scale gates are working, but the interior terrain still fails because it is dominated by a procedural radial volcano. Batch 005 and Batch 006 define the replacement terrain math foundation.

## Scope

Terrain acceptance, validation evidence, project state, and handoff only. Do not design foliage, ecology, weather, audio, NPCs, gameplay systems, buildings, final materials, UI, map, compass, minimap, GPS, objective markers, or player-facing overlays.

## Hard gates

```text
world_size_m: 97536.0 target
shoreline_error_max_m: 0 or effectively 0
beach_continuity_pct: 100%
ocean_below_sea_pct: 100%
square_edge_ocean_violations: 0
outer world edge remains ocean/container terrain
```

Also prove the terrain is no longer one-feature radial volcano terrain.

## Research target

Define the final acceptance standard for the new terrain math lane, including source terrain identity, generator architecture evidence, hydrology/catchment evidence, ridge/valley graph evidence, volcanic subsystem evidence, feature grammar evidence, preview images, manifest metrics, volcano-disabled terrain proof, calibration summary, runtime terrain shell evidence, and handoff requirements.

## Required Markdown structure

```markdown
# Full Terrain Math Acceptance And Handoff Research

## Executive Summary
## Terrain-Only Scope Boundary
## Batch 004 Historical Failure Summary
## Batch 005 / 006 Replacement Goal
## Required Source Evidence
## Required Preview Evidence
## Required Manifest Metrics
## Coast And Scale Hard Gates
## Morphology Hard Gates
## Hydrology / Catchment Acceptance
## Ridge / Valley / Landform Acceptance
## Volcanic Subsystem Acceptance
## Feature Grammar Acceptance
## Calibration Acceptance
## Runtime Terrain Shell Acceptance
## Handoff File Requirements
## Recovery Instructions
## Files And Areas Likely Touched
## Acceptance Criteria
## Risks And Stop Conditions
## Rejected Or Forbidden Approaches
## Sources And References
```

## Implementation usefulness requirements

Give concrete guidance for final manifest schema, output folder under `Images/TerrainPreview/`, log tokens, handoff file naming, project state update requirements, language distinguishing completed Batch 004 from new Batch 005/006 work, and stop conditions if terrain still reads radial or volcano-owned.

## Source expectations

Use reliable sources on terrain validation, DEM analysis, procedural terrain QA, UE5 terrain evidence workflows, and reproducible generator pipelines. Cite sources in the Markdown file.
