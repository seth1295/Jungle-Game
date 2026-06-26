# PR5 Batch 003 Research Prompt 004 — Active Crater, Lava, And Crust Terrain Hazard Implementation

You are an independent no-context researcher AI working from this prompt only.

Your job is to produce a Markdown research/spec document for **Jungle Game**, a UE5 Linux-first first-person project.

## Absolute final deliverable requirement

Your final answer must include a **downloadable Markdown file attachment** named exactly:

```text
004_active_crater_lava_crust_terrain_hazards_research.md
```

## Project context

Jungle Game’s PR5 Batch 003 is a major terrain-generation batch for a realistic active volcanic island. The island should sit inside a 16.256 km square world as a circular/organic sea-level landmass with a roughly 1400 m volcanic massif. This prompt focuses on terrain geometry for the active crater/vent/lava-crust hazard zone.

The desired result is not a tiny decorative volcano. The terrain should support a proper active-volcano feel through geometry: broken crater rim, active vent depression, lava/crust terrain surface, collapse shelves, ash/cinder slopes, fissures, hard blockers, and safe-ish overlook ridges.

## Critical coastline requirement

The active crater zone is inland/high-elevation, but all lava-flow scars, collapse channels, and lahar-related terrain must eventually respect the island-scale sea-level mask:

```text
outer island edge = sea level beach/coastal shelf
outside island mask = ocean/below sea level
volcanic flow or lahar channels taper into lower terrain/coastal fan geometry
no active terrain feature creates square-edge cliffs or raised land at world bounds
```

## Terrain-only scope boundary

Do not design VFX, smoke particles, heat shaders, audio, damage systems, survival mechanics, weather, foliage, ecology, NPCs, final materials, story, or player UI. Lava and crust are discussed here only as terrain geometry, collision/hard-blocker shape, masks, and terrain hazard zones.

Do not recommend player-facing map/minimap/compass/GPS/objective/bearing/time/orientation UI.

## Research target

Produce a major-implementation terrain spec for active crater, lava-crust, and volcanic hazard geometry.

Answer:

1. What active crater/vent terrain model best fits a 1400 m tropical volcanic massif: summit crater, breached crater, side vent, fissure field, lava lake, crusted lava shelf, or hybrid?
2. What crater/rim/breach dimensions and slope targets are appropriate at this island scale?
3. How should stable approach ridges, unsafe crust, fissures, collapse shelves, hard blockers, crater drops, ash/cinder slopes, and talus be represented in terrain geometry?
4. What masks/channels are needed for crater, rim, lava/crust, unstable crust, fissures, vents, hard blockers, and safe overlooks?
5. How can the terrain read as an active volcano before final VFX/materials exist?
6. How should this implementation avoid a theme-park crater while still being inspectable/playable around the edges?
7. What developer-only topo/slope/mask/collision diagnostics prove the terrain hazard zones work?

## Required Markdown structure

```markdown
# Active Crater, Lava, And Crust Terrain Hazards Research

## Executive Summary
## Terrain-Only Scope Boundary
## Current Terrain Baseline
## Major Implementation Goal
## Island Mask And Sea-Level Constraints
## Recommended Active Crater / Vent Terrain Model
## Crater Rim And Breach Geometry
## Lava / Crusted Surface Terrain Geometry
## Fissure / Vent / Collapse Terrain Geometry
## Stable Approach Versus Unstable Terrain
## Hard-Blocker Terrain Criteria
## Terrain Masks And Debug Channels
## Collision And Slope Validation Requirements
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

- crater/rim/breach dimensions;
- active vent placement;
- lava/crust terrain geometry independent of final visuals;
- unsafe crust/hard blocker geometry;
- masks/channels;
- slope thresholds;
- sea-level/coast termination constraints for volcanic scars;
- validation maps and pass/fail evidence.

This is terrain generation only.

## Source expectations

Use reliable sources where volcanic crater morphology, lava crust/collapse terrain, fissures, vent fields, cinder/ash slopes, DEM terrain analysis, or UE5 terrain/collision validation matter. Cite sources in the Markdown file.

## Final instruction

Your final output must be a downloadable Markdown file named exactly:

```text
004_active_crater_lava_crust_terrain_hazards_research.md
```
