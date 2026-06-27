# PR5 Batch 005 Research Prompt 002 — Hydrology, Erosion, And Catchment Solver

You are an independent no-context researcher AI working from this prompt only.

Your job is to produce a Markdown research/spec document for **Jungle Game**, a UE5 Linux-first first-person survival-simulation project.

## Absolute final deliverable requirement

Your final answer must include a downloadable Markdown file attachment named exactly:

```text
002_hydrology_erosion_catchment_solver_research.md
```

## Project context

Jungle Game currently has deterministic terrain preview evidence for a 97.536 km volcanic island. The coast/beach/shelf gates pass, but the interior ridge/gully morphology still looks like crater-centered procedural spokes. The next terrain feature is a mathematical rewrite based on real drainage and erosion principles.

This prompt focuses on **watersheds, flow routing, erosion, deposition, and catchment competition**. The terrain must stop looking like a polar plot and start reading as a large island whose relief is organized by drainage basins, ridge divides, valley networks, fan deposition, and slope processes.

## Terrain-only scope boundary

This is terrain morphology and terrain validation only.

Do not design foliage, ecology, weather, audio, NPCs, gameplay systems, final materials, buildings, quests, player-facing UI, map, compass, minimap, GPS, objective markers, or navigation overlays.

## Critical preservation requirement

Hydrology and erosion work must not break the existing coast acceptance gates:

```text
world_size_m: 97536.0 target
shoreline_error_max_m: 0 or effectively 0
beach_continuity_pct: 100%
ocean_below_sea_pct: 100%
square_edge_ocean_violations: 0
outer square world edge: ocean/container terrain
beach/coastal shelf transition remains continuous
```

## Research target

Design an ambitious but implementable hydrology/erosion terrain solver or solver-inspired approximation for Jungle Game.

The output should tell developers how to generate:

- deterministic coastal outlets;
- major catchment basins;
- ridge divides between basins;
- valley heads and tributary trees;
- stream-power incision;
- diffusion/creep smoothing;
- sediment/deposition fans;
- lahar-compatible volcanic corridors;
- validation maps proving drainage is not radial.

The final design can be full simulation, hybrid graph-plus-field, or offline precompute plus runtime sampling, but it must be source-owned, deterministic, and practical for a UE5 project.

## Required mathematical topics

Research and explain how the terrain generator should use or approximate:

```text
flow routing: D8, D-infinity, multiple-flow direction
flow accumulation: contributing area A
stream power incision: E = K A^m S^n
slope / gradient: S = |grad h|
diffusion / hillslope creep: dh/dt = D laplacian(h)
sediment capacity and deposition
pit filling / depression handling
watershed labeling / basin segmentation
ridge extraction from watershed boundaries
Hack's law / drainage length-area scaling where useful
Strahler or Horton-style stream order for tributaries
alluvial/coastal fan deposition near outlets
```

Do not hand-wave these as vague “erosion noise.” Give implementable approximations.

## Questions the research must answer

1. Should Jungle Game use a true iterative erosion solver, a graph-based approximation, or a hybrid two-stage system?
2. How should coastal outlets be seeded irregularly without equal angular spacing?
3. How should catchment areas compete so basins have variable size, shape, curvature, and relief?
4. How should ridge divides be derived from catchment competition rather than drawn as radial spokes?
5. How should secondary and tertiary tributaries branch without becoming random noise mush?
6. How should erosion be constrained near the beach so coast gates still pass?
7. How should lava/lahar paths integrate with drainage without becoming clean radial splines?
8. What diagnostic maps and numeric metrics prove hydrology is working?

## Required Markdown structure

```markdown
# Hydrology, Erosion, And Catchment Solver Research

## Executive Summary
## Terrain-Only Scope Boundary
## Current Drainage Failure
## Hydrology Solver Goals
## Flow Routing Options
## Flow Accumulation And Stream Ordering
## Watershed / Catchment Segmentation
## Ridge Divide Extraction
## Valley And Tributary Graph Generation
## Stream-Power Incision Model
## Hillslope Diffusion And Slope Failure Relaxation
## Sediment Transport And Fan Deposition
## Volcanic Lahar And Lava Corridor Coupling
## Coast Preservation And Sea-Level Locking
## Deterministic Implementation Strategy
## Preview / Debug Maps
## Validation Metrics
## Files And Areas Likely Touched
## Large PR Implementation Scope
## Acceptance Criteria
## Risks And Stop Conditions
## Rejected Or Forbidden Approaches
## Sources And References
```

## Implementation usefulness requirements

Give concrete guidance for:

- source-owned structures such as `FTerrainOutlet`, `FCatchmentBasin`, `FDrainageNode`, `FDrainageEdge`, `FErosionPassConfig`, and `FHydrologyMetrics`;
- exact generated maps: flow direction, accumulation, catchment id, stream order, ridge divide, valley floor, outlet fan, erosion depth, deposition depth;
- anti-radial metrics: outlet spacing variance, catchment area variance, drainage direction entropy, radial alignment score, stream curvature score;
- performance strategy for preview grid vs full source grid;
- where iterative simulation can be approximated safely for 1024 px previews and 8129-class source evidence;
- how to preserve deterministic seeds and config fingerprints.

## Source expectations

Use reliable geomorphology, hydrology, terrain synthesis, erosion simulation, DEM analysis, and procedural terrain sources. Cite sources in the Markdown file.

## Final instruction

Your final output must be a downloadable Markdown file named exactly:

```text
002_hydrology_erosion_catchment_solver_research.md
```
