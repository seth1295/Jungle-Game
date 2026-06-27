# PR5 Batch 005 Research Prompt 005 — Multi-Scale Terrain Detail And Feature Grammar

You are an independent no-context researcher AI working from this prompt only.

Your job is to produce a Markdown research/spec document for **Jungle Game**, a UE5 Linux-first first-person survival-simulation project.

## Absolute final deliverable requirement

Your final answer must include a downloadable Markdown file attachment named exactly:

```text
005_multi_scale_noise_feature_grammar_research.md
```

## Project context

Jungle Game has deterministic terrain preview evidence for a 97.536 km volcanic island. The coast, beach, shelf, and ocean metrics pass, but the interior still reads too procedural: one dominant volcano, radial fan-blade ridges, repeated spoke gullies, and visible algorithmic authority.

This prompt focuses on the correct use of multi-scale terrain detail, domain warping, feature grammar, and deterministic terrain identity. Detail functions must support landforms. They must not replace landforms.

## Terrain-only scope boundary

This is terrain synthesis, feature grammar, heightfield/mask composition, deterministic identity, and preview validation only.

Do not design foliage, ecology, weather, audio, NPCs, survival systems, buildings, final materials, UI, map, compass, minimap, GPS, objective markers, or player-facing navigation overlays.

## Critical preservation requirement

The terrain identity system must preserve the working acceptance gates:

```text
world_size_m: 97536.0 target
shoreline_error_max_m: 0 or effectively 0
beach_continuity_pct: 100%
ocean_below_sea_pct: 100%
square_edge_ocean_violations: 0
outer world edge remains ocean/container terrain
coast/beach/shelf gates are final hard constraints after feature composition
```

## Research target

Design a deterministic terrain identity system that combines named landform grammar, multi-scale detail, domain warping without circular artifacts, and feature stamps for plateaus, benches, scarps, saddles, coastal flats, fans, floodplains, old lava fields, and secondary hills.

The outcome should feel like a specific place, not a generic procedural island.

## Required topics

Research and specify:

- octave budgeting by physical scale;
- ridged detail fields and when they are useful or dangerous;
- vector warp fields and how to avoid radial symmetry;
- anisotropic detail aligned to graph features, prevailing slope, or region axes;
- feature grammar and shape grammar for terrain regions;
- implicit feature stamps using distance fields and soft masks;
- field operations for benches, scarps, valleys, saddles, and terraces;
- deterministic placement for terrain features;
- config fingerprints and subsystem identity;
- preview evidence that separates real structure from decorative detail.

## Questions the research must answer

1. Which terrain details should come from graph, erosion, and volcanology, and which should come from detail fields?
2. What physical scale bands should detail operate at for a 97.536 km island?
3. How should warp fields be generated so they do not create circular artifacts?
4. How should feature grammar create recognizable landform identity?
5. How should feature stamps blend with hydrology and volcanology without overriding them?
6. How should the system avoid random detail mush while also avoiding sterile authored-looking blobs?
7. How should deterministic identity be recorded in manifest/debug output?
8. What preview maps prove that the terrain has multiple designed landform systems?

## Required Markdown structure

```markdown
# Multi-Scale Terrain Detail, Feature Grammar, And Terrain Identity Research

## Executive Summary
## Terrain-Only Scope Boundary
## Current Procedural Identity Failure
## Terrain Identity Goals
## Detail Is Not Terrain Authority
## Physical Scale Bands For Detail
## Domain Warping And Vector Fields
## Feature Grammar Overview
## Feature Stamp Data Model
## Plateaus, Benches, Scarps, Saddles, Terraces, And Flats
## Coastal Fans, Floodplains, And Valley Floor Features
## Old Lava Fields And Secondary Hill Features
## Deterministic Identity And Config Fingerprints
## Composition Order And Conflict Resolution
## Preview / Debug Outputs
## Validation Metrics
## Files And Areas Likely Touched
## Large PR Implementation Scope
## Acceptance Criteria
## Risks And Stop Conditions
## Rejected Or Forbidden Approaches
## Sources And References
```

## Implementation usefulness requirements

Give concrete guidance for structures such as `FTerrainFeatureGrammar`, `FTerrainFeatureStamp`, `FNoiseScaleBand`, `FDomainWarpField`, `FRegionFeatureRule`, and `FTerrainIdentityMetrics`.

Also define detail scale bands in meters, feature placement constraints, deterministic subsystem identity, metrics for feature count and terrain richness, and debug outputs including a feature stamp atlas, scale-band contribution atlas, warp-vector map, feature ownership map, and composition-stage previews.

## Source expectations

Use reliable sources on procedural terrain synthesis, noise functions, multifractals, domain warping, shape grammar, implicit modeling, geomorphology, and DEM validation. Cite sources in the Markdown file.

## Final instruction

Your final output must be a downloadable Markdown file named exactly:

```text
005_multi_scale_noise_feature_grammar_research.md
```
