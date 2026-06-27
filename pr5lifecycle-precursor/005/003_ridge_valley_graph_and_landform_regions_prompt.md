# PR5 Batch 005 Research Prompt 003 — Ridge-Valley Graph And Landform Regions

You are an independent no-context researcher AI working from this prompt only.

Your job is to produce a Markdown research/spec document for **Jungle Game**, a UE5 Linux-first first-person survival-simulation project.

## Absolute final deliverable requirement

Your final answer must include a downloadable Markdown file attachment named exactly:

```text
003_ridge_valley_graph_and_landform_regions_research.md
```

## Project context

Jungle Game has a deterministic 97.536 km volcanic island terrain preview. The coast, beach, shelf, and ocean metrics pass. The interior terrain does not. It reads as one giant procedural volcano with radial fan-blade ridges and gullies.

This prompt focuses on a new intermediate representation: a **terrain graph and landform region system** that can own the island structure before local heightfield details are applied.

The goal is to make the island read as a believable, authored-yet-deterministic terrain composition:

```text
highland spine
upland blocks
saddles and passes
basin floors
asymmetric drainage regions
coastal flats
old terraces
scarps and benches
secondary hills
compact active volcano embedded into the terrain
```

## Terrain-only scope boundary

This is terrain graph, landform segmentation, heightfield construction, and debug validation only.

Do not design foliage, ecology, weather, audio, gameplay, NPCs, buildings, final materials, map, compass, minimap, GPS, objective markers, or player-facing navigation UI.

## Critical preservation requirement

The graph/region system must respect the existing world and coast gates:

```text
world_size_m: 97536.0 target
shoreline_error_max_m: 0 or effectively 0
beach_continuity_pct: 100%
ocean_below_sea_pct: 100%
square_edge_ocean_violations: 0
outer world edge remains ocean/container terrain
beach and shallow shelf remain continuous
```

## Research target

Design a deterministic ridge-valley graph and named landform-region system that sits between raw procedural fields and final sampled height.

The graph should answer questions like:

- where are the main uplands?
- where do valleys begin and exit?
- which ridges divide basins?
- where are passes/saddles?
- which areas are old volcanic benches vs eroded basement vs coastal plains?
- how does the active volcano attach to the rest of the island without owning it?

The generator should use the graph to create spatial identity before applying noise or erosion.

## Required mathematical / procedural topics

Research and specify:

- planar graph or constrained graph representation for terrain skeletons;
- basin graph topology and adjacency;
- Voronoi, power diagrams, weighted distance fields, and anisotropic distance for regions;
- skeleton curves, splines, and geodesic paths for ridges/valleys;
- ridge divide extraction from basin boundaries;
- saddle/pass detection and shaping;
- scalar field blending from graph features;
- region masks with soft transitions and hard semantic IDs;
- feature stamping for plateaus, benches, scarps, terraces, plains, and valleys;
- deterministic rejection sampling for nonuniform landform placement.

## Questions the research must answer

1. What graph data model should represent island-scale landforms?
2. How many major landform regions should a 97.536 km island expose at minimum?
3. How should region seeds be placed so they are deterministic, asymmetric, and nonuniform?
4. How should ridge and valley curves be generated from the graph instead of crater angle?
5. How should saddles, passes, terraces, and coastal flats be represented mathematically?
6. How should region IDs blend into continuous height without losing debug traceability?
7. How should volcano-specific regions plug into the graph as one subsystem?
8. What preview outputs prove the graph is working?

## Required Markdown structure

```markdown
# Ridge-Valley Graph And Landform Regions Research

## Executive Summary
## Terrain-Only Scope Boundary
## Current One-Feature Terrain Failure
## Terrain Graph Goals
## Landform Region Taxonomy
## Graph Data Model
## Deterministic Region Seeding
## Basin And Ridge Adjacency
## Valley Curves And Outlet Connections
## Ridge Spines, Saddles, Passes, And Scarps
## Terraces, Benches, Plateaus, And Coastal Flats
## Scalar Field Conversion From Graph Features
## Volcano Subsystem Attachment Points
## Debug Region Maps And Developer Review Points
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

- structures such as `FLandformRegion`, `FTerrainGraphNode`, `FTerrainGraphEdge`, `FRidgeSpine`, `FValleyAxis`, `FTerrainSaddle`, `FTerrainTerrace`, and `FLandformMetrics`;
- named region IDs and expected minimum count;
- graph constraints: no equal-angle distribution, no single central authority, no mirrored regions;
- metrics for region area diversity, adjacency count, graph branching, valley curvature, ridge-divide continuity, and volcano footprint;
- debug maps: region ID, graph overlay, ridge/valley skeleton, saddle/pass map, plateau/terrace map, volcano-disabled region map;
- how to export region metadata in the terrain preview manifest.

## Source expectations

Use reliable sources where terrain synthesis, computational geometry, hydrology, geomorphology, GIS/DEM analysis, graph-based procedural generation, and landform classification matter. Cite sources in the Markdown file.

## Final instruction

Your final output must be a downloadable Markdown file named exactly:

```text
003_ridge_valley_graph_and_landform_regions_research.md
```
