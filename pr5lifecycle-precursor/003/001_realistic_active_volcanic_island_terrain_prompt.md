# PR5 Batch 003 Research Prompt 001 — Full-Scale Active Volcanic Island Terrain Implementation

You are an independent no-context researcher AI working from this prompt only.

Your job is to produce a Markdown research/spec document for **Jungle Game**, a UE5 Linux-first first-person project.

## Absolute final deliverable requirement

Your final answer must include a **downloadable Markdown file attachment**.

The file must be named exactly:

```text
001_realistic_active_volcanic_island_terrain_research.md
```

A chat-only answer is incomplete. A summary without a downloadable `.md` file is incomplete. A pasted Markdown draft without a downloadable file is incomplete.

## Project context

Jungle Game has a committed full-size terrain shell proof:

```text
world domain: 16.256 km x 16.256 km
runtime mesh: 129 x 129 vertices
runtime vertex spacing: about 127 m
runtime token: JG_FULL_TERRAIN_SHELL_003
current elevation range: about -101 m to +140 m
current slope range: about 0 to 7 degrees
```

That proof is too gentle and too provisional. PR5 Batch 003 is now a **major terrain-generation implementation batch**, not a minimal patch batch. Assume each PR may be a large, coherent implementation slice if needed. The goal is to solve the core terrain-generation problems before environment, traversal, ecology, weather, audio, story, or survival systems resume.

The target terrain identity is a **realistic active tropical volcanic island**: a circular/near-circular island within the 16.256 km square world, dominated by an approximately 1400 m volcanic massif, with radial ridges, catchments, gullies, crater/vent terrain, lava/crust terrain hazards, coast transitions, and developer-only topographic validation.

## Critical coastline requirement

The terrain must not use square-world edges as cliffs or arbitrary terrain. The playable island should be circular or organically rounded inside the square world domain.

The **outer circular island edge must resolve to sea level and beach/coastal shelf terrain**:

```text
outside island radius: ocean / below sea level
island edge: sea level / beach ring / coastal shelf
inside edge: low coastal terrain rising inland
no tall wall at the island perimeter
no random raised terrain at the circle edge
no square-edge landmass pretending to be terrain
```

The terrain generator should include explicit shoreline falloff, beach/shelf bands, coastal lowlands, and ocean-below-sea-level outside the island mask.

## Non-negotiable forbidden scope

This batch is terrain generation only. Do not recommend implementation plans for:

- foliage placement;
- ecology systems;
- weather systems;
- audio/ambience;
- survival mechanics;
- animals/NPCs/relict hominin content;
- story scripting;
- final VFX;
- final materials beyond debug readability;
- asset-pack dressing;
- player UI.

Do not recommend or imply any player-facing map, minimap, compass, bearing readout, GPS, objective marker, HUD clock, time readout, orientation meter, or modern navigation tool.

Developer-only topographic maps, heightmaps, terrain masks, slope maps, catchment maps, debug overlays, and validation images are allowed only as terrain-generation tools.

## Research target

Produce a terrain-generation research/spec document for a **large implementation PR**, not a small patch. The consuming implementation should be able to build a new terrain model that replaces the current low-relief shell with a realistic active volcanic island foundation.

Answer:

1. What volcanic island landform type should drive the whole terrain generator?
2. How should a 1400 m volcanic massif fit inside a 16.256 km square world while the actual island remains circular/organic?
3. How should the island mask, ocean, sea-level beach ring, coastal shelf, coastal flats, and inland rise be represented?
4. What macro terrain primitives are required in the first major implementation: massif, crater/rim, shoulders, radial ridges, gullies, catchments, coastal benches, beaches, fans, vents, hard blockers?
5. What elevation targets, slope targets, and relief targets should replace the current -101 m to +140 m low-relief proof?
6. What must be implemented as terrain now versus deferred to later non-terrain systems?
7. What diagnostic maps prove the terrain generator is correct at scale?

## Required Markdown structure for the downloadable file

```markdown
# Realistic Active Volcanic Island Terrain Research

## Executive Summary
## Terrain-Only Scope Boundary
## Current Runtime Terrain Baseline
## Major Implementation Goal
## Recommended Volcanic Island Landform Type
## Island Mask, Sea Level, And Beach Ring Requirements
## 16.256 km World Scale Model
## 1400 m Massif Placement And Proportions
## Macro Terrain Primitive Inventory
## Elevation, Relief, And Slope Targets
## Coastline / Beach / Shelf Terrain Geometry
## Inland Rise And Lowland Terrain Geometry
## Volcanic Ridge / Gully / Catchment Geometry
## Crater / Vent / Lava-Crust Terrain Geometry
## Terrain Generation Implementation Guidance
## Developer-Only Terrain Diagnostics
## Large PR Acceptance Criteria
## Risks And Stop Conditions
## Rejected Or Forbidden Approaches
## Sources And References
```

## Implementation usefulness requirements

This document must be useful for a major implementation PR. Include concrete guidance for:

- island-radius and sea-level handling;
- beach/coastal shelf bands;
- ocean/outside-island mask behavior;
- maximum elevation around 1400 m;
- slope bands for walkable, steep, dangerous, and hard-blocker terrain;
- deterministic terrain function or heightfield architecture;
- runtime mesh/heightfield resolution implications;
- developer-only map outputs and pass/fail thresholds.

Do not prescribe final environment art, foliage, ecology, audio, weather, or survival systems.

## Source expectations

Use reliable sources where volcanic geomorphology, island shorelines, DEM/heightfield generation, slope analysis, volcanic terrain hazards, or UE5 terrain/mesh implementation matter. Cite sources in the Markdown file.

## Final instruction

Your final output must be a downloadable Markdown file named exactly:

```text
001_realistic_active_volcanic_island_terrain_research.md
```
