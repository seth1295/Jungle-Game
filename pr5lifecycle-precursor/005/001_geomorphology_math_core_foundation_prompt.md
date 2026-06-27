# PR5 Batch 005 Research Prompt 001 — Geomorphology Math Core Foundation

You are an independent no-context researcher AI working from this prompt only.

Your job is to produce a Markdown research/spec document for **Jungle Game**, a UE5 Linux-first first-person survival-simulation project.

## Absolute final deliverable requirement

Your final answer must include a downloadable Markdown file attachment named exactly:

```text
001_geomorphology_math_core_foundation_research.md
```

## Project context

Jungle Game has a deterministic volcanic island terrain source and a preview exporter. The latest PR5 Batch 004 evidence proves the coast, beach, shelf, ocean, and 6x world scale gates can pass, but the interior terrain still fails visually.

The failure is structural, not cosmetic:

```text
Batch 004 terrain reads as a procedural volcano preview, not a real island.
The volcano is the only dominant feature.
The island is controlled by crater-centered math.
Ridge and gully masks still read as turbine / flower / fan-blade structures.
The terrain lacks independent macro landforms, basins, valleys, plateaus, coastal flats, saddles, and old volcanic history.
```

This prompt is for a **new mathematical foundation**, not a small patch. Treat Batch 004 as useful failure evidence, not as a base to tweak.

## Terrain-only scope boundary

This is terrain mathematics, terrain architecture, validation, source identity, and preview evidence only.

Do not design foliage, ecology, weather, audio, NPCs, survival gameplay, combat, buildings, quests, final materials, UI, map, compass, minimap, GPS, objective markers, or player-facing navigation overlays.

## Critical preservation requirement

The new terrain math must preserve the already-working coastal acceptance gates:

```text
world_size_m remains 97536.0 unless explicitly justified
linear scale vs Batch 003 remains 6.0
area scale vs Batch 003 remains 36.0
shoreline_error_max_m must remain 0 or effectively 0
beach_continuity_pct must remain 100%
ocean_below_sea_pct must remain 100%
square_edge_ocean_violations must remain 0
outer world edge remains ocean/container terrain
island perimeter resolves through beach/coastal shelf/sea-level transition
```

## Core research target

Design a first-principles terrain-generation foundation where the island is interesting **before** the volcano layer exists.

The final terrain model should be composed from a hierarchy like:

```text
island mass / coastline field
tectonic or basement macro relief
regional highland spine and upland blocks
catchment basins and ridge divides
valley floors, floodplains, coastal fans, saddles, terraces
old volcanic benches and secondary cones
compact active volcano subsystem
post-process coastal constraint and validation gates
```

Do not use crater-centered radial coordinates as the global organizing system.

## Required mathematical topics

Research and specify how to use or approximate these concepts in a deterministic game terrain generator:

- implicit scalar fields and signed distance fields for island mass, coastal envelope, terrain regions, and feature blending;
- hierarchical landform composition across island scale, regional scale, basin scale, and local scale;
- spectral terrain synthesis, fBm, multifractals, ridged multifractals, domain warping, and why noise alone is insufficient;
- anisotropic fields for ridge spines, escarpments, saddles, terraces, and valley floors;
- terrain graph or skeleton representation for basins, ridges, outlets, and landform regions;
- elevation distribution design: hypsometric curve, relief range, slope distribution, and lowland/highland proportions;
- physically motivated post-processing: diffusion, stream-power incision, slope failure relaxation, and coastal clamping;
- deterministic seed/config/fingerprint identity for reproducible evidence;
- unit-aware constants for a 97.536 km world.

## Questions the research must answer

1. What is the correct terrain authority hierarchy for a 97.536 km volcanic jungle island?
2. Which parts should be field-based, graph-based, particle/erosion-based, or feature-stamp-based?
3. How should the generator avoid one-feature terrain where the volcano controls the whole island?
4. How should non-volcanic terrain be evaluated with the volcano disabled?
5. How should macro relief, highlands, lowlands, and basin regions be distributed?
6. How should feature blending avoid circular falloff artifacts and obvious procedural seams?
7. How should terrain math remain deterministic and inspectable inside source code/tooling?
8. What files/classes/tools are likely touched in the existing Jungle Game terrain source and preview exporter?

## Required Markdown structure

```markdown
# Geomorphology Math Core Foundation Research

## Executive Summary
## Terrain-Only Scope Boundary
## Batch 004 Failure Evidence
## Required Terrain Authority Hierarchy
## Mathematical Foundation Overview
## Island Mass And Coast Envelope Fields
## Macro Relief And Regional Upland Design
## Basin / Ridge / Valley Graph Authority
## Scalar Field Composition And Blending Rules
## Noise, Domain Warping, And Multifractal Use
## Hypsometry, Relief, And Slope Distribution Targets
## Volcano-Disabled Island Acceptance Test
## Determinism, Seeds, Config, And Fingerprints
## Preview And Developer Evidence Requirements
## Files And Areas Likely Touched
## Large PR Implementation Scope
## Acceptance Criteria
## Risks And Stop Conditions
## Rejected Or Forbidden Approaches
## Sources And References
```

## Implementation usefulness requirements

Give concrete guidance for:

- proposed data structures for terrain regions, landform nodes, ridge/valley graph nodes, and feature stamps;
- exact metrics for lowland area, highland area, volcano footprint, relief distribution, and slope distribution;
- a `volcano_disabled_height_preview` and associated acceptance criteria;
- a `landform_region_id` map and named terrain regions;
- source-level version names for this rewrite, e.g. `JG_TERRAIN_MATH_CORE_005_001`;
- failure gates that reject radial/turbine terrain even if coast metrics pass.

## Source expectations

Use reliable sources where geomorphology, procedural terrain synthesis, hydrology, DEM analysis, erosion modeling, volcanic island morphology, and terrain validation matter. Cite sources in the Markdown file.

## Final instruction

Your final output must be a downloadable Markdown file named exactly:

```text
001_geomorphology_math_core_foundation_research.md
```
