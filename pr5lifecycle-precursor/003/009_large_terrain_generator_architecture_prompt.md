# PR5 Batch 003 Research Prompt 009 — Large Terrain Generator Architecture Implementation

You are an independent no-context researcher AI working from this prompt only.

Your job is to produce a Markdown research/spec document for **Jungle Game**, a UE5 Linux-first first-person project.

## Absolute final deliverable requirement

Your final answer must include a **downloadable Markdown file attachment** named exactly:

```text
009_large_terrain_generator_architecture_research.md
```

## Project context

Jungle Game currently has terrain generation embedded in a runtime shell actor. That proved the concept, but PR5 Batch 003 is now a large implementation batch for solving terrain generation at scale. The project needs a cleaner terrain-generator architecture that can support island masks, sea-level beaches, 1400 m volcanic massif, ridges, catchments, crater hazards, masks/channels, exports, and validation.

This prompt is about source architecture for terrain generation only. It is not about gameplay systems or environment art.

## Critical coastline requirement

The architecture must make sea-level island perimeter behavior a first-class invariant:

```text
island mask and sea-level beach ring are not optional post-effects
all terrain features blend against island/coast constraints
square-world edge remains ocean/container, not terrain identity
all exports and runtime logs include sea-level/coast metadata
```

## Terrain-only scope boundary

Do not design foliage, ecology, weather, audio, survival mechanics, NPCs, story, VFX, final materials, asset packs, or player UI. This is deterministic terrain-generator source architecture, runtime mesh/heightfield build, masks, validation, and docs.

Do not recommend player-facing map/minimap/compass/GPS/objective/bearing/time/orientation UI.

## Research target

Produce a major-implementation architecture spec for the terrain generator.

Answer:

1. How should the terrain generator be split into source-owned config, deterministic sampling, feature functions, masks/channels, mesh/heightfield output, and validation export?
2. What classes/files/modules would likely be touched in UE5 C++?
3. How should config identity and seed/version metadata be handled?
4. How should island mask, beach ring, massif, ridges, gullies, crater, lava/crust, and hard blockers be composed without spaghetti?
5. How should the current terrain shell actor evolve into a generator-backed actor or subsystem?
6. How should large implementation PRs be sequenced without minimalizing the terrain work into tiny ineffective patches?
7. What stop conditions should prevent terrain architecture from sprawling into environment/gameplay systems?

## Required Markdown structure

```markdown
# Large Terrain Generator Architecture Research

## Executive Summary
## Terrain-Only Scope Boundary
## Current Source Baseline
## Major Implementation Goal
## Terrain Generator Ownership Model
## Config / Seed / Version Identity
## Feature Function Composition Strategy
## Island Mask And Sea-Level Invariant
## Heightfield / Runtime Mesh Output Strategy
## Terrain Masks And Channel Architecture
## Export / Topographic Validation Architecture
## UE5 Files And Classes Likely Touched
## Large PR Sequencing Guidance
## Acceptance Criteria
## Risks And Stop Conditions
## Rejected Or Forbidden Approaches
## Sources And References
```

## Implementation usefulness requirements

Give concrete guidance for:

- suggested class names and responsibilities;
- config structs/data assets or source constants;
- deterministic function composition;
- mask/channel ownership;
- runtime log token format;
- validation/export API shape;
- what can be large in this implementation batch;
- what must be rejected as scope creep.

This is terrain generator architecture only.

## Source expectations

Use reliable sources where UE5 C++ architecture, procedural mesh/Landscape generation, deterministic terrain generation, config/data assets, validation/export workflows, or source-control hygiene matter. Cite sources in the Markdown file.

## Final instruction

Your final output must be a downloadable Markdown file named exactly:

```text
009_large_terrain_generator_architecture_research.md
```
