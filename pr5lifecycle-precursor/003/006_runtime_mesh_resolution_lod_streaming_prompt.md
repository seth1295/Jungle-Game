# PR5 Batch 003 Research Prompt 006 — Runtime Mesh Resolution, LOD, And Terrain Scale Implementation

You are an independent no-context researcher AI working from this prompt only.

Your job is to produce a Markdown research/spec document for **Jungle Game**, a UE5 Linux-first first-person project.

## Absolute final deliverable requirement

Your final answer must include a **downloadable Markdown file attachment** named exactly:

```text
006_runtime_mesh_resolution_lod_streaming_research.md
```

## Project context

Jungle Game currently proves full-size terrain with a 129 x 129 procedural mesh over 16.256 km, about 127 m between vertices. That is enough to prove collision and macro scale, but it is too coarse for a realistic 1400 m active volcanic island with beaches, ridges, gullies, crater terrain, and slope readability.

PR5 Batch 003 is a large terrain-generation implementation batch. This prompt is about the implementation architecture for terrain resolution and scale, not a minimal patch.

## Critical coastline requirement

Any resolution/LOD/tiling strategy must preserve the island mask and sea-level perimeter:

```text
outer circular island edge = sea level beach/coastal shelf
outside island mask = ocean/below sea level
LOD seams must not break the 0 m shoreline
tiles must not create square-edge terrain artifacts
coastline, beaches, ridges, gullies, and crater masks must stay aligned across resolution changes
```

## Terrain-only scope boundary

Do not design foliage, ecology, weather, audio, survival mechanics, NPCs, VFX, final materials, world art dressing, or player UI. This is about terrain mesh/heightfield/landscape scale, resolution, LOD, streaming, collision, and validation only.

Do not recommend player-facing map/minimap/compass/GPS/objective/bearing/time/orientation UI.

## Research target

Produce a major-implementation spec for replacing or extending the current 129 x 129 runtime mesh with a terrain representation capable of realistic terrain generation at island scale.

Answer:

1. What resolution is needed for macro terrain, ridge/gully terrain, beach ring, and crater hazard terrain?
2. Should the project use a higher-resolution procedural mesh, tiled procedural meshes, UE Landscape, runtime-generated heightfield assets, or a staged bridge architecture?
3. How should collision be handled for large terrain without killing iteration speed?
4. How should LOD/streaming avoid gaps, seam artifacts, and coastline breakage?
5. What is the right next implementation step if the project cannot jump straight to final Landscape import?
6. How should actual runtime mesh/topographic exports prove the generated terrain matches the source?
7. What generated artifacts must stay ignored and unstaged?

## Required Markdown structure

```markdown
# Runtime Mesh Resolution, LOD, And Terrain Scale Research

## Executive Summary
## Terrain-Only Scope Boundary
## Current Runtime Mesh Baseline
## Major Implementation Goal
## Sea-Level Edge And Tile Seam Requirements
## Resolution Targets By Terrain Feature
## Procedural Mesh Versus Landscape Tradeoff
## Tiled Terrain Architecture Options
## Collision Strategy
## LOD / Streaming / Seam Strategy
## Heightfield Source And Runtime Mesh Consistency
## Developer-Only Validation Outputs
## Artifact Hygiene Rules
## Large PR Implementation Scope
## Files And Areas Likely Touched
## Acceptance Criteria
## Risks And Stop Conditions
## Rejected Or Forbidden Approaches
## Sources And References
```

## Implementation usefulness requirements

Give concrete guidance for:

- target vertex spacing options;
- suggested tile size/count;
- coastline seam preservation;
- collision cooking strategy;
- high-level class/module structure;
- validation maps and runtime logs;
- staged implementation path from current 129 x 129 mesh to better terrain.

This is terrain generation architecture only.

## Source expectations

Use reliable sources where UE5 procedural mesh, Landscape, World Partition, collision, LOD, heightfields, DEM resolution, or terrain validation matter. Cite sources in the Markdown file.

## Final instruction

Your final output must be a downloadable Markdown file named exactly:

```text
006_runtime_mesh_resolution_lod_streaming_research.md
```
