# PR5 Batch 003 Research Prompt 005 — Sea-Level Beach Ring And Coastal Shelf Implementation

You are an independent no-context researcher AI working from this prompt only.

Your job is to produce a Markdown research/spec document for **Jungle Game**, a UE5 Linux-first first-person project.

## Absolute final deliverable requirement

Your final answer must include a **downloadable Markdown file attachment** named exactly:

```text
005_sea_level_beach_ring_coastal_shelf_research.md
```

## Project context

Jungle Game has a 16.256 km square world container and a current runtime terrain shell. PR5 Batch 003 is now a large terrain-generation implementation batch for a realistic active volcanic island. The playable island should be circular/organic inside the square world, not square. The most important correction is that the **outer circular island edge must become sea level and beach/coastal shelf terrain**.

This prompt exists because the terrain generator must not create a raised island edge, square-world cliff, or infinite land sheet. A tropical volcanic island needs ocean outside the island mask, a sea-level shoreline, beaches/coastal shelves, low coastal flats, fan deposits, and inland rise toward volcanic terrain.

## Critical coastline requirement

This is the central requirement of this prompt:

```text
outer circular island edge = sea level
edge band = beach / coastal shelf / low coastal terrain
outside island radius = ocean or below sea level
inside island radius = terrain rises inland
square world edge = only container/ocean, never raised island terrain
```

Terrain generation must include explicit island mask, shoreline falloff, beach band, coastal shelf, lowland transition, and ocean-below-sea-level behavior.

## Terrain-only scope boundary

Do not design final water rendering, waves, foliage, ecology, weather, audio, survival mechanics, NPCs, story, VFX, final materials, or player UI. Beaches and ocean are discussed as terrain elevation/masks/collision/heightfield logic only, not art or water simulation.

Do not recommend player-facing map/minimap/compass/GPS/objective/bearing/time/orientation UI.

## Research target

Produce a major-implementation terrain spec for island masking, sea-level shoreline, beach ring, coastal shelf, ocean outside the island, and terrain blending into inland volcanic landforms.

Answer:

1. How should a circular/organic island mask be generated inside a 16.256 km square terrain domain?
2. How should the shoreline hit sea level reliably around the entire island edge?
3. What beach/coastal shelf width and elevation profile should be used at this scale?
4. How should ocean/outside-island heights be represented in the terrain function or mask outputs?
5. How should major ridges, gullies, and drainage systems terminate at the beach ring without breaking sea-level continuity?
6. How should cliffs, rocky headlands, creek mouths, and coastal fans vary the coastline while preserving sea-level correctness?
7. What developer-only diagnostics prove the coastline is correct?

## Required Markdown structure

```markdown
# Sea-Level Beach Ring And Coastal Shelf Research

## Executive Summary
## Terrain-Only Scope Boundary
## Current Terrain Baseline
## Major Implementation Goal
## Island Mask Requirements
## Sea-Level Shoreline Rules
## Beach Ring Elevation Profile
## Coastal Shelf And Ocean-Below-Sea-Level Model
## Coastal Lowland Transition Geometry
## Ridge / Gully / Fan Termination At Coast
## Cliffs, Headlands, Creek Mouths, And Beach Variation
## Terrain Masks And Debug Channels
## Developer-Only Coastline Validation Maps
## Large PR Implementation Scope
## Files And Areas Likely Touched
## Acceptance Criteria
## Risks And Stop Conditions
## Rejected Or Forbidden Approaches
## Sources And References
```

## Implementation usefulness requirements

Give concrete guidance for:

- island mask formula or workflow;
- radial/organic shoreline variation;
- beach-ring width in meters;
- coastal shelf depth/elevation profile;
- sea-level clamp or blend strategy;
- preserving sea level while allowing cliffs/headlands;
- drainage/fan termination;
- validation outputs such as elevation contour at 0 m, coastline mask, beach mask, ocean mask, and slope map.

This is terrain generation only. Do not prescribe final water, art, ecology, or player UI.

## Source expectations

Use reliable sources where island shoreline geomorphology, beach/coastal shelf profiles, DEM/heightfield masking, ocean/shoreline terrain representation, or UE5 terrain/mesh validation matter. Cite sources in the Markdown file.

## Final instruction

Your final output must be a downloadable Markdown file named exactly:

```text
005_sea_level_beach_ring_coastal_shelf_research.md
```
