# PR5 Batch 006 Research Prompt 002 — Offline Generator And Runtime Bridge Architecture

You are an independent no-context researcher AI working from this prompt only.

Your job is to produce a Markdown research/spec document for **Jungle Game**, a UE5 Linux-first first-person survival-simulation project.

## Absolute final deliverable requirement

Your final answer must include a downloadable Markdown file attachment named exactly:

```text
002_offline_generator_runtime_bridge_architecture_research.md
```

## Project context

Jungle Game needs a serious terrain math rewrite for a 97.536 km island. The next generator may need hydrology, erosion, terrain graph construction, DEM-style metrics, multiple preview passes, and source-owned deterministic configuration. That may be too heavy to live only inside runtime actor sampling.

This prompt focuses on the architecture split between canonical terrain math source, offline or editor-side generator passes, small tracked preview evidence, large local generated artifacts, runtime sampling bridge, and UE5 mesh/LOD/collision/material channel consumers.

## Terrain-only scope boundary

This is terrain source architecture, generator tooling, runtime bridge, artifact policy, and validation only.

Do not design foliage, ecology, weather, audio, NPCs, buildings, gameplay systems, final materials, map, compass, minimap, GPS, objective markers, or player-facing overlays.

## Critical preservation requirement

Architecture changes must preserve the working terrain gates:

```text
world_size_m: 97536.0 target
shoreline_error_max_m: 0 or effectively 0
beach_continuity_pct: 100%
ocean_below_sea_pct: 100%
square_edge_ocean_violations: 0
outer world edge remains ocean/container terrain
```

## Research target

Design the proper source/tool/runtime architecture for an ambitious terrain generator that may include graph building, hydrology, erosion-style passes, volcanic subsystem composition, preview export, validation metrics, and UE runtime sampling.

The output should compare these options:

- pure C++ runtime sampling;
- Python preview/export mirror;
- UE editor-side generator;
- generated height and mask artifacts;
- config-driven generation;
- hybrid canonical C++ plus tooling mirror;
- staged migration from current source math to heavier generation.

## Questions the research must answer

1. What should be the canonical source of terrain truth?
2. Which passes must be runtime-sampleable, and which should be offline/editor generated?
3. How should deterministic config identity and fingerprints prevent tool/runtime drift?
4. How should large artifacts be handled without bloating the repository?
5. How should preview images remain small, tracked, and reviewable?
6. How should generated heightfields/masks feed UE5 runtime terrain shell, future landscape import, materials, collision, and debug views?
7. How should the pipeline support 1024 px preview, 8129-class source evidence, and runtime LOD sampling?
8. What implementation staging reduces risk while still allowing a big terrain rewrite?

## Required Markdown structure

```markdown
# Offline Generator And Runtime Bridge Architecture Research

## Executive Summary
## Terrain-Only Scope Boundary
## Current Architecture Limitations
## Canonical Terrain Authority Options
## Runtime Sampling Versus Offline Generation
## Recommended Architecture
## Config Identity, Versioning, And Fingerprints
## Generator Pass Pipeline
## Artifact Policy And Repository Hygiene
## Preview Export Pipeline
## Runtime Terrain Shell Bridge
## UE5 Landscape / Mesh / LOD / Collision Considerations
## Mask And Channel Data Flow
## Validation And Manifest Requirements
## Files And Areas Likely Touched
## Large PR Implementation Scope
## Acceptance Criteria
## Risks And Stop Conditions
## Rejected Or Forbidden Approaches
## Sources And References
```

## Implementation usefulness requirements

Give concrete guidance for possible classes/tools such as `FJungleTerrainGeneratorConfig`, `FJungleTerrainSourceGrid`, `FJungleTerrainGeneratedTile`, `UJungleTerrainGeneratorCommandlet`, and `scripts/terrain-preview-export.py`.

Also define config/version names for the new math pipeline, mirror-drift checks, generated artifact naming, minimal tracked preview evidence under `Images/TerrainPreview/<batch>/`, runtime log tokens, manifest fields, and staged PR boundaries that are large enough to matter but still reviewable.

## Source expectations

Use reliable sources on UE5 large worlds, terrain/heightfield generation, deterministic build pipelines, editor tooling, procedural terrain, DEM processing, and source-control artifact hygiene. Cite sources in the Markdown file.

## Final instruction

Your final output must be a downloadable Markdown file named exactly:

```text
002_offline_generator_runtime_bridge_architecture_research.md
```
