# Research Prompt — PR5 Batch 001 / File 003

## Output File To Produce

Produce one Markdown document named:

```text
003_creek_riparian_wet_valley_research.md
```

## Role

You are an independent research/specification assistant. You do not have repo access and must not assume hidden project context. Your task is to produce a factual, implementation-useful Markdown research/spec document for a UE5 game project.

## Project Context

Project name: **Jungle Game**.

Jungle Game is a Linux-first Unreal Engine 5 first-person 3D survival-simulation set in a dense tropical jungle island world, approximately around 1000 CE. The world should feel physically and ecologically normal. The unsettling element is a nonviolent relict hominin presence that observes, tests, and changes the player's environment from distance. The player is not being hunted; the player is being evaluated.

The game is world-first: terrain, creeks, ridges, basins, coast, canopy, sound, light, and weather are the main game substrate. Creeks and wet valleys are expected to be primary movement evidence, survival pressure, sound anchors, and false-affordance systems.

Hard design guard: no player-facing map, minimap, compass, bearing number/readout, GPS, objective marker navigation, HUD clock, explicit time readout, orientation confidence UI, hidden orientation meter, or character comments that tell the player whether they are oriented.

## Research Target

Research and specify **creek, riparian, wet-valley, crossing, and water-sound traversal systems** for a dense wet tropical island jungle survival game.

Focus on how creeks can be useful but imperfect navigation structures without becoming painted route lines or UI aids.

## Required Research Questions

Answer these questions with implementation-useful detail:

1. How do wet tropical creek systems form readable movement structures through catchments, drainage corridors, banks, crossings, wet valleys, gullies, and creek mouths?
2. How can creek banks become affordance zones, soft blockers, hard blockers, false affordances, or route traps depending on slope, wetness, bank shape, vegetation, and flow scale?
3. What environmental cues can communicate water direction, crossing possibility, wet footing, water depth/risk, channel scale, and blocked routes from first person?
4. How should water sound, frogs/insects, waterfalls, rain, muffled audio, and valley acoustics support orientation without explicit UI?
5. What placeholder risk concepts are useful for leeches, ticks, mud, slippery rocks, deadfall, wait-a-while, bank collapse, flooding, or rain-swollen crossings without building a full health/disease system?
6. How should crossing candidates be represented for implementation and validation?
7. What developer-only debug/evidence views should later prove creek logic?

## Expected Markdown Structure

Use this exact top-level structure:

```markdown
# Creek Riparian Wet Valley Research

## Executive Summary
## Design Goals For Jungle Game
## Creek And Wet-Valley Landform Principles
## Creek Traversal Affordance Model
## Crossing Candidate Model
## Water-Sound And Acoustic Evidence
## Tropical Riparian Vegetation And Friction
## Weather And Night Interaction
## UE5 Implementation Guidance
## Validation Points And Evidence Plan
## Risks, Pitfalls, And Forbidden Drift
## PR-Ready Recommendations
## Sources
```

## Implementation Usefulness Requirements

The document must be usable later as one PR input. Include concrete recommendations for:

- creek/riparian mask names;
- crossing candidate fields;
- creek/wet-valley biome outputs;
- route evidence names;
- sound anchor categories;
- debug view names;
- validation point types;
- data struct or enum naming suggestions where helpful;
- what should be implemented now versus deferred.

Do not write production C++ code. Pseudocode or enum examples are allowed only if they clarify the spec.

## Factuality Requirements

Use factual hydrology, riparian ecology, tropical rainforest movement, and sound propagation principles. Prefer reliable sources such as hydrology references, government environment/geoscience agencies, ecology references, academic/educational sources, and official Unreal Engine documentation for UE-specific implementation notes.

Include a `## Sources` section with source names and URLs. Do not fabricate sources. If uncertain, state uncertainty.

## Forbidden Scope

Do not recommend:

- player-facing map, minimap, compass, bearing, GPS, objective marker, HUD clock, explicit time readout, or orientation confidence UI;
- modern survival tools;
- combat, gore, chase, monster framing, or fantasy magic;
- final water simulation or full hydrology simulation;
- full wildlife AI or disease system;
- exact protected-place replication;
- generic temperate creek assumptions without tropical adaptation.

## Output Constraint

Return only the final Markdown document content for `003_creek_riparian_wet_valley_research.md`.
