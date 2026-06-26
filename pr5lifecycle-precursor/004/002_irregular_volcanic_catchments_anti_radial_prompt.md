# PR5 Batch 004 Research Prompt 002 — Irregular Volcanic Catchments And Anti-Radial Drainage

You are an independent no-context researcher AI working from this prompt only.

Your job is to produce a Markdown research/spec document for **Jungle Game**, a UE5 Linux-first first-person project.

## Absolute final deliverable requirement

Your final answer must include a **downloadable Markdown file attachment** named exactly:

```text
002_irregular_volcanic_catchments_anti_radial_research.md
```

## Project context

Jungle Game has a deterministic PR5 Batch 003 volcanic island terrain model and a real terrain preview exporter. The latest preview proves the coastline system works, but it also exposes a serious interior terrain problem.

The terrain preview images show:

```text
coast / beach / shelf: good
outer world edge: good, ocean/container terrain
volcano presence: good enough to continue
ridge/gully drainage morphology: bad
main visual failure: procedural-radial / gear-shaped / fan-blade spokes
```

The mask atlas is especially clear: the ridge/gully channel is too evenly angular. The terrain currently reads like equal-angle radial catchments around a central peak, not like a large natural volcanic island shaped by irregular drainage, erosion, landslides, lava history, and asymmetric catchment competition.

## Terrain-only scope boundary

This is terrain morphology only.

Do not design foliage, ecology, weather, audio, survival systems, NPCs, VFX, final materials, buildings, POIs, quests, or player UI.

Do not recommend player-facing map/minimap/compass/GPS/objective/bearing/time/orientation UI.

## Critical preservation requirement

Do not break the working coast acceptance while fixing the interior drainage morphology:

```text
shoreline_error_max_m must remain 0 or effectively 0 in preview evidence
beach_continuity_pct must remain 100%
ocean_below_sea_pct must remain 100%
square_edge_ocean_violations must remain 0
outer world edge must remain ocean/container terrain
island perimeter must remain beach/coastal shelf/sea-level transition
```

## Hard visual rejection criteria

The next generator must reject:

```text
equal-angle radial catchments
perfectly repeated fan blades
gear teeth around the volcano
uniform spoke gullies
uniform spoke ridges
single-ring crater with symmetric drainage
ridge/gully masks that look like a polar plot
```

## Research target

Produce a major-implementation spec for replacing the current radial catchment/ridge/gully math with irregular volcanic island drainage morphology.

Answer:

1. How should catchment centers, ridge divides, valley heads, and outlets be seeded so they are deterministic but not evenly spaced?
2. How should primary basins vary in width, length, curvature, elevation head, and outlet shape?
3. How should secondary and tertiary gullies branch without becoming noise mush?
4. How should ridge masks and gully masks compete so the result reads like erosion/drainage instead of spokes?
5. How should lava flows, lahar corridors, crater breach, landslide scars, and coastal fans interact with drainage without becoming repeated radial wedges?
6. How should the model preserve sea-level coast/beach/shelf constraints after interior terrain contributions?
7. What preview and mask metrics should prove the anti-radial rewrite worked?
8. What implementation files/classes are likely touched?

## Required Markdown structure

```markdown
# Irregular Volcanic Catchments And Anti-Radial Drainage Research

## Executive Summary
## Terrain-Only Scope Boundary
## Current Preview Failure
## Hard Anti-Radial Requirements
## Natural Volcanic Drainage Goals
## Deterministic Irregular Basin Seeding
## Variable Catchment Width And Curvature
## Ridge Divide Model
## Gully And Valley Incision Model
## Secondary / Tertiary Drainage Branching
## Lahar, Lava, Breach, And Landslide Interaction
## Coastal Outlet And Fan Treatment
## Coast Preservation Rules
## Preview / Mask Validation Metrics
## Developer-Only Debug Outputs
## Files And Areas Likely Touched
## Large PR Implementation Scope
## Acceptance Criteria
## Risks And Stop Conditions
## Rejected Or Forbidden Approaches
## Sources And References
```

## Implementation usefulness requirements

Give concrete guidance for:

- deterministic basin seed data structures;
- nonuniform angle spacing;
- basin anchor warping;
- outlet placement and coastal fan variation;
- valley-head offsets;
- ridge/gully strength curves;
- secondary branch logic;
- noise/fBm/domain-warp use without losing deterministic validation;
- anti-radial metrics such as angular regularity score, basin-width variance, outlet-spacing variance, ridge/gully symmetry score, and fan-blade rejection gates;
- preview exporter overlays that make the issue obvious.

## Scale interaction

This prompt is part of PR5 Batch 004. Batch 004 also requires a 6x linear world/map size increase. Make sure the drainage rewrite can work on a 97.536 km-class island and does not assume the old 16.256 km world scale.

## Source expectations

Use reliable sources where volcanic island drainage, fluvial geomorphology, erosion terrain generation, procedural terrain synthesis, hydrology, domain warping, DEM analysis, or terrain validation matter. Cite sources in the Markdown file.

## Final instruction

Your final output must be a downloadable Markdown file named exactly:

```text
002_irregular_volcanic_catchments_anti_radial_research.md
```
