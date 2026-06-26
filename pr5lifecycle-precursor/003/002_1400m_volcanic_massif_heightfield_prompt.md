# PR5 Batch 003 Research Prompt 002 — 1400 m Volcanic Massif Heightfield Implementation

You are an independent no-context researcher AI working from this prompt only.

Your job is to produce a Markdown research/spec document for **Jungle Game**, a UE5 Linux-first first-person project.

## Absolute final deliverable requirement

Your final answer must include a **downloadable Markdown file attachment** named exactly:

```text
002_1400m_volcanic_massif_heightfield_research.md
```

## Project context

Jungle Game currently has a 16.256 km square runtime terrain shell with a 129 x 129 procedural mesh. It works, but it only reaches about +140 m and slopes are mostly below 7 degrees. PR5 Batch 003 is a **big terrain implementation batch**, not a minimal-patch batch.

This prompt focuses on the deterministic heightfield/math needed to create the central landform: an approximately **1400 m active volcanic massif** inside a circular/organic island. The result should be suitable for a large implementation PR that can replace the weak provisional massif logic.

## Critical coastline and island-mask requirement

The 16.256 km world is a square container, but the island must not fill the square as land. The island should be circular or organically rounded. The outer island edge must resolve to sea level and beach/coastal shelf terrain:

```text
square world edge: container only, not terrain identity
outside island radius: ocean / below sea level
island perimeter: sea level / beach ring / coastal shelf
coast interior: low coastal plain rising inland
main massif: rises from inland terrain, not from a square edge
```

Any massif formula must blend cleanly into this sea-level island mask. No high mountain shoulder should leak into the beach ring or square-world edge.

## Terrain-only scope boundary

Do not design foliage, ecology, weather, audio, story, NPCs, VFX, survival mechanics, final art materials, or player UI. This is terrain generation only: elevation, slopes, forms, masks, collision/hard-blocker geometry, and developer-only validation maps.

Do not recommend player-facing map/minimap/compass/GPS/objective/bearing/time/orientation UI.

## Research target

Produce a major-implementation terrain spec for the volcanic massif heightfield.

Answer:

1. How should the 1400 m volcanic massif be mathematically shaped inside a 16.256 km world?
2. What base radius, shoulder radius, summit radius, crater/rim radius, flank slope, and saddle geometry should it target?
3. How should the massif blend into the circular sea-level beach-ring island mask?
4. How should the generator avoid a perfect cone while still making the mountain read as volcanic?
5. How should secondary shoulders, breached rim, flank benches, and slope breaks be represented?
6. What slope bands should be walkable, steep, dangerous, or hard-blocking?
7. What current runtime mesh limitations must be fixed for a 1400 m massif to look correct?

## Required Markdown structure

```markdown
# 1400 m Volcanic Massif Heightfield Research

## Executive Summary
## Terrain-Only Scope Boundary
## Current Heightfield Baseline
## Major Implementation Goal
## Island Mask And Sea-Level Blend Requirements
## Massif Scale And Proportion Targets
## Heightfield Shape Model
## Slope Profile Targets
## Summit / Crater / Rim Geometry
## Shoulder / Saddle / Spur Geometry
## Anti-Cone Naturalization Strategy
## Beach Ring Protection Rules
## Runtime Mesh Resolution Implications
## Developer-Only Topographic Validation Targets
## Large PR Implementation Scope
## Files And Areas Likely Touched
## Acceptance Criteria
## Risks And Stop Conditions
## Rejected Or Forbidden Approaches
## Sources And References
```

## Implementation usefulness requirements

Give concrete implementation guidance for a major terrain PR:

- target max elevation near 1400 m;
- sea-level perimeter preservation;
- base/shoulder/summit/crater dimensions;
- formulas or pseudocode for massif shaping;
- blend functions for beach ring and coastal shelf;
- slope thresholds;
- runtime mesh/heightfield resolution changes;
- validation maps and numeric pass/fail metrics.

Do not prescribe final visuals, foliage, weather, ecology, audio, or survival systems.

## Source expectations

Use reliable sources where volcanic morphology, stratovolcano profiles, DEM/heightfield generation, crater geometry, slope analysis, shoreline terrain, or UE5 terrain implementation matters. Cite sources in the Markdown file.

## Final instruction

Your final output must be a downloadable Markdown file named exactly:

```text
002_1400m_volcanic_massif_heightfield_research.md
```
