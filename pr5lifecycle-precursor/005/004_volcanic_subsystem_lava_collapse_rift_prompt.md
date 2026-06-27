# PR5 Batch 005 Research Prompt 004 — Volcanic Subsystem, Lava, Collapse, And Rift Math

You are an independent no-context researcher AI working from this prompt only.

Your job is to produce a Markdown research/spec document for **Jungle Game**, a UE5 Linux-first first-person survival-simulation project.

## Absolute final deliverable requirement

Your final answer must include a downloadable Markdown file attachment named exactly:

```text
004_volcanic_subsystem_lava_collapse_rift_research.md
```

## Project context

Jungle Game has a deterministic 97.536 km volcanic island terrain preview. The coastline and x6 scale gates are working. The main interior failure is that the volcano controls nearly the whole island and the ridge/gully system still reads as procedural radial spokes.

This prompt focuses on rebuilding the volcano as a **local geological subsystem embedded inside a larger island**, not as the global coordinate authority.

The volcano should remain a powerful landmark, but it must not be the only feature. The island should still have interesting terrain with the volcano disabled.

## Terrain-only scope boundary

This is volcanic terrain morphology only.

Do not design VFX, smoke particles, audio, damage systems, NPCs, combat, survival mechanics, final materials, buildings, quests, UI, map, compass, GPS, objective markers, or player-facing hazard overlays.

Lava, crust, hazards, vents, and blockers in this prompt mean terrain masks and heightfield geometry only.

## Critical preservation requirement

The volcanic subsystem must not break existing terrain acceptance gates:

```text
world_size_m: 97536.0 target
shoreline_error_max_m: 0 or effectively 0
beach_continuity_pct: 100%
ocean_below_sea_pct: 100%
square_edge_ocean_violations: 0
outer world edge remains ocean/container terrain
volcanic terrain contributions must be coast-clamped after composition
```

## Research target

Design a serious volcanic terrain subsystem for a large tropical island that includes:

- compact active cone and summit crater;
- nested vent / crater floor / broken rim;
- breached crater sector;
- flank collapse amphitheater or horseshoe scar;
- rift or fissure alignment with secondary cones;
- older lava benches and flow fields;
- lava channels that follow real terrain gullies;
- lahar corridors coupled to catchments;
- unstable crust / hard blocker terrain masks;
- local radial detail only inside the volcano subsystem.

The design must explicitly ban crater-centered radial math from owning the whole island.

## Required mathematical / geological topics

Research and specify how to model or approximate:

- volcanic construct geometry: shield-like slopes, stratovolcano slopes, cones, crater rims, vents;
- crater depression and rim asymmetry;
- sector collapse / edifice failure / horseshoe scars;
- rift-zone or fissure-line placement;
- lava flow path routing using slope descent and channel confinement;
- lava lobe deposition, levees, crusted flow fields, and stopped flows;
- lahar pathways using drainage networks;
- tephra/ash/scree aprons and slope relaxation;
- secondary cones and parasitic vents;
- older eroded volcanic benches integrated with non-volcanic terrain.

## Questions the research must answer

1. What is a correct footprint target for active cone, broader volcanic highland, and old volcanic terrain on a 97.536 km island?
2. How should the volcano be offset and attached to regional terrain without owning the whole island?
3. What local radial math is acceptable, and where must it be forbidden?
4. How should crater breach, collapse scar, and lava channels choose directions from terrain context rather than equal-angle sectors?
5. How should lava and lahar masks follow hydrology/catchment data?
6. How should old lava benches differ from active cone terrain in height, slope, and mask behavior?
7. How should volcanic blockers and unstable crust be terrain-derived, not gameplay/VFX-derived?
8. What preview maps prove the volcano is compact, embedded, and terrain-owned?

## Required Markdown structure

```markdown
# Volcanic Subsystem, Lava, Collapse, And Rift Math Research

## Executive Summary
## Terrain-Only Scope Boundary
## Current Volcano Failure
## Volcanic Subsystem Goals
## Volcano Footprint And Relief Targets
## Active Cone And Crater Geometry
## Nested Vent, Broken Rim, And Breach Model
## Flank Collapse / Horseshoe Scar Model
## Rift Zones, Fissures, And Secondary Cones
## Lava Flow Routing And Lobe Deposition
## Lahar Corridor Coupling To Drainage
## Old Lava Benches And Eroded Volcanic Terrain
## Scree, Ash, Crust, And Hard-Blocker Terrain Masks
## Local Radial Math Limits
## Integration With Island Graph And Hydrology
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

Give concrete guidance for:

- structures such as `FVolcanicConstruct`, `FCraterRim`, `FCollapseScar`, `FRiftLine`, `FSecondaryCone`, `FLavaFlowPath`, `FLavaLobe`, `FLaharCorridor`, and `FVolcanicMetrics`;
- maximum active-cone footprint and broader highland footprint gates;
- local-only polar coordinate windows and explicit global radial bans;
- terrain-derived lava path algorithm using flow routing / slope descent;
- preview outputs: volcano footprint mask, old lava bench mask, collapse scar map, lava path graph, lahar catchment coupling map, volcano-disabled preview;
- acceptance gates proving the island remains interesting without the volcano.

## Source expectations

Use reliable sources on volcanic geomorphology, lava flow modeling, lahar routing, edifice collapse, DEM analysis, terrain synthesis, and procedural terrain generation. Cite sources in the Markdown file.

## Final instruction

Your final output must be a downloadable Markdown file named exactly:

```text
004_volcanic_subsystem_lava_collapse_rift_research.md
```
