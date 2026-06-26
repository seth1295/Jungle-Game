# PR5 Batch 003 Research Prompt 007 — Terrain Masks, Channels, And Debug Material Implementation

You are an independent no-context researcher AI working from this prompt only.

Your job is to produce a Markdown research/spec document for **Jungle Game**, a UE5 Linux-first first-person project.

## Absolute final deliverable requirement

Your final answer must include a **downloadable Markdown file attachment** named exactly:

```text
007_terrain_masks_channels_material_debug_research.md
```

## Project context

Jungle Game’s PR5 Batch 003 is a major terrain-generation batch. It must produce realistic active volcanic island terrain and prove it with developer-only diagnostics. The project needs source-owned terrain masks/channels so each major landform can be validated and later consumed by systems without guessing.

This is not about final materials or environment art. It is about terrain-generation channels: island mask, ocean mask, beach ring, coastal shelf, lowlands, massif, ridges, gullies, catchments, crater, lava/crust, unsafe crust, hard blockers, slope classes, and debug visualization.

## Critical coastline requirement

Masks must enforce and prove the sea-level edge:

```text
island mask defines circular/organic island inside square world
ocean mask outside island radius is below sea level
beach ring mask follows 0 m shoreline
coastal shelf mask surrounds beach ring
no terrain channel should raise the outer circular island edge above sea level unless it is explicitly a rocky/cliff coast variant that still intersects sea level correctly
```

## Terrain-only scope boundary

Do not design foliage, ecology, weather, audio, survival mechanics, NPCs, story, final VFX, final art materials, or player UI. Debug materials are allowed only to inspect terrain generation channels.

Do not recommend player-facing map/minimap/compass/GPS/objective/bearing/time/orientation UI.

## Research target

Produce a major-implementation spec for terrain masks/channels and developer-only debug visualization.

Answer:

1. What terrain masks/channels are needed for the active volcanic island generator?
2. Which channels are continuous scalar fields versus discrete class masks?
3. How should sea level, beach ring, coast shelf, island mask, and ocean mask be represented?
4. How should ridges, gullies, catchments, crater, lava/crust, unsafe crust, hard blockers, and slope classes be represented?
5. How should debug material colors or overlay modes be defined for developer validation without becoming player UI?
6. How should mask metadata/log tokens identify terrain config identity?
7. What acceptance criteria prove terrain channels are correct enough for the next implementation stage?

## Required Markdown structure

```markdown
# Terrain Masks, Channels, And Debug Material Research

## Executive Summary
## Terrain-Only Scope Boundary
## Current Terrain Baseline
## Major Implementation Goal
## Required Terrain Channels
## Sea-Level / Island / Beach / Ocean Masks
## Massif / Ridge / Gully / Catchment Masks
## Crater / Lava-Crust / Hard-Blocker Masks
## Slope / Elevation / Relief Classes
## Continuous Versus Discrete Channel Rules
## Developer-Only Debug Material Modes
## Runtime Metadata And Log Tokens
## Validation Map Requirements
## Large PR Implementation Scope
## Files And Areas Likely Touched
## Acceptance Criteria
## Risks And Stop Conditions
## Rejected Or Forbidden Approaches
## Sources And References
```

## Implementation usefulness requirements

Give concrete guidance for:

- channel names;
- value ranges;
- generated mask ownership;
- debug colors/overlays;
- log token fields;
- validation screenshots/maps;
- source-control handling of generated artifacts;
- how masks support terrain generation without implementing environment systems.

This is terrain generation/debug only.

## Source expectations

Use reliable sources where terrain masks, heightfield channels, slope classification, UE materials/debug visualization, or terrain validation matter. Cite sources in the Markdown file.

## Final instruction

Your final output must be a downloadable Markdown file named exactly:

```text
007_terrain_masks_channels_material_debug_research.md
```
