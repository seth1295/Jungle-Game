# PR5 Batch 003 Research Prompt 003 — Radial Ridges, Catchments, And Lahar Gully Terrain Implementation

You are an independent no-context researcher AI working from this prompt only.

Your job is to produce a Markdown research/spec document for **Jungle Game**, a UE5 Linux-first first-person project.

## Absolute final deliverable requirement

Your final answer must include a **downloadable Markdown file attachment** named exactly:

```text
003_radial_ridges_catchments_lahar_gullies_research.md
```

## Project context

Jungle Game’s current full-size terrain shell exists but is too smooth, too low, and not yet a realistic volcanic island. PR5 Batch 003 is a **large terrain-generation implementation batch**. This prompt focuses only on the terrain geometry of radial ridges, catchments, incised gullies, lahar channels, fans, saddles, and drainage logic.

The target island is circular/organic inside a 16.256 km square world, dominated by a roughly 1400 m active volcanic massif. The island perimeter must resolve to sea level and beach/coastal shelf terrain.

## Critical coastline requirement

All ridge, catchment, and gully systems must terminate coherently into a sea-level island perimeter:

```text
major gullies drain to coastal fans / creek mouths / beach breaks
ridge toes fade into low coastal terrain
outer circular perimeter is sea level / beach / shelf
outside the island mask is ocean or below sea level
no ridges continue as raised geometry to the square-world edge
no rivers/gullies end randomly in the middle of the beach ring
```

## Terrain-only scope boundary

Do not design foliage, ecology, weather, audio, survival mechanics, NPCs, story, final water rendering, final materials, VFX, or player UI. Water/lahar/creeks are discussed only as terrain incision, drainage paths, slope/catchment geometry, fans, and masks.

Do not recommend player-facing map/minimap/compass/GPS/objective/bearing/time/orientation UI.

## Research target

Produce a spec for a major implementation PR that builds the volcanic ridge/catchment terrain system.

Answer:

1. How many major radial ridges should descend from the 1400 m massif?
2. How should ridge arms, spurs, saddles, and shoulders be generated so they are terrain, not artificial roads?
3. How should catchment basins and creek/gully incision form between ridges?
4. How should lahar gullies, landslide scars, boulder/alluvial fans, and creek-mouth terrain be represented in the heightfield?
5. How should drainage lines terminate into sea-level beaches/coastal shelves?
6. Which ridge/gully slopes should be walkable, steep, dangerous, or hard-blocking terrain?
7. What developer-only flow/catchment/slope/topographic diagnostics prove the system is working?

## Required Markdown structure

```markdown
# Radial Ridges, Catchments, And Lahar Gully Terrain Research

## Executive Summary
## Terrain-Only Scope Boundary
## Current Terrain Baseline
## Major Implementation Goal
## Sea-Level Coast And Drainage Termination Requirements
## Volcanic Ridge And Drainage Principles
## Recommended Ridge Network
## Spur / Saddle / Shoulder Geometry
## Catchment And Creek-Incision Layout
## Lahar Gully And Landslide Scar Geometry
## Fan Deposit / Creek Mouth Terrain Geometry
## Traversable Versus Hard-Blocker Terrain Geometry
## Slope And Flow Diagnostic Requirements
## Developer-Only Topographic Validation Targets
## Large PR Implementation Scope
## Files And Areas Likely Touched
## Acceptance Criteria
## Risks And Stop Conditions
## Rejected Or Forbidden Approaches
## Sources And References
```

## Implementation usefulness requirements

Give concrete guidance for:

- ridge count and placement;
- deterministic ridge/gully generation strategy;
- drainage termination at sea-level beach ring;
- target gully depths/widths by elevation band;
- fan and creek-mouth geometry;
- slope thresholds;
- mask/channel outputs;
- validation map outputs.

This must solve terrain generation, not environment dressing.

## Source expectations

Use reliable sources where volcanic drainage, lahar channels, catchment geomorphology, landslide gullies, coastal fans, DEM/heightfield analysis, or UE5 terrain validation matter. Cite sources in the Markdown file.

## Final instruction

Your final output must be a downloadable Markdown file named exactly:

```text
003_radial_ridges_catchments_lahar_gullies_research.md
```
