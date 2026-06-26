# Research Prompt — PR5 Batch 001 / File 001

## Output File To Produce

Produce one Markdown document named:

```text
001_macro_island_terrain_navigation_research.md
```

## Role

You are an independent research/specification assistant. You do not have repo access and must not assume hidden project context. Your task is to produce a factual, implementation-useful Markdown research/spec document for a UE5 game project.

## Project Context

Project name: **Jungle Game**.

Jungle Game is a Linux-first Unreal Engine 5 first-person 3D survival-simulation set in a dense tropical jungle island world, approximately around 1000 CE. The world should feel physically and ecologically normal. The unsettling element is a nonviolent relict hominin presence that observes, tests, and changes the player's environment from distance. The player is not being hunted; the player is being evaluated.

Current game direction is **world-first**: the island terrain, creek systems, ridges, saddles, gullies, basins, coastlines, canopy, sound, light, and weather are the main game substrate. Authored places like Cell 0 are placed inside the world later; they must not fake the whole world.

Hard design guard: no player-facing map, minimap, compass, bearing number/readout, GPS, objective marker navigation, HUD clock, explicit time readout, orientation confidence UI, hidden orientation meter, or character comments that tell the player whether they are oriented.

## Research Target

Research and specify **macro island terrain and landform navigation** for a dense tropical FNQ-inspired island jungle survival game.

Focus on how real landforms can become first-person navigation evidence and traversal pressure without adding UI navigation aids.

Include a focused comparison of classical procedural terrain/noise approaches versus emerging learned terrain generation approaches. Specifically review **InfiniteDiffusion / Terrain Diffusion** as a research reference, using this project page as a required source: `https://xandergos.github.io/terrain-diffusion/`. Treat it as a reference for future terrain-generation architecture, not as an immediate dependency recommendation.

## Required Research Questions

Answer these questions with implementation-useful detail:

1. What macro landforms should a wet tropical island world include so navigation emerges naturally?
2. How should a mountain or upland spine create catchments, creek heads, ridges, spurs, saddles, basins, slope breaks, and skyline landmarks?
3. How can ridges, spurs, saddles, valleys, gullies, basins, creek corridors, coastlines, mangrove transitions, beaches, cliff coasts, and rocky headlands support no-map first-person orientation?
4. What terrain features should become hard blockers, soft blockers, affordances, false affordances, or neutral friction?
5. How should the terrain remain readable from first person rather than only from a top-down map/debug view?
6. What validation points should the game capture later for terrain evidence?
7. What are common pitfalls when designing jungle terrain for gameplay, such as arbitrary steep noise, too-flat arenas, uniform foliage, clean circular coast paths, or invisible-wall boundaries?
8. What terrain-generation lessons can be taken from InfiniteDiffusion / Terrain Diffusion, especially around seed consistency, lazy/unbounded generation, random access, learned terrain fidelity, and whether those ideas are practical for a UE5 island-first workflow?

## Expected Markdown Structure

Use this exact top-level structure:

```markdown
# Macro Island Terrain Navigation Research

## Executive Summary
## Design Goals For Jungle Game
## Real-World Landform Principles
## Recommended Island Landform Set
## Terrain Navigation Evidence
## Traversal Category Mapping
## First-Person Readability Rules
## Terrain Generation Architecture Notes
## UE5 Implementation Guidance
## Validation Points And Evidence Plan
## Risks, Pitfalls, And Forbidden Drift
## PR-Ready Recommendations
## Sources
```

## Implementation Usefulness Requirements

The document must be usable later as one PR input. Include concrete recommendations for:

- terrain channel names;
- landform category names;
- terrain generation architecture options and tradeoffs;
- sample validation point types;
- first-person screenshot/evidence points;
- C++/data enum or struct naming suggestions where helpful;
- debug-only evidence concepts;
- what should be implemented now versus deferred.

Do not write production C++ code. Pseudocode or enum examples are allowed only if they clarify the spec.

## Factuality Requirements

Use factual geomorphology and tropical terrain principles. Prefer reliable sources such as government/geoscience agencies, hydrology/geomorphology references, ecology references, academic or educational sources, and official Unreal Engine documentation for UE-specific implementation notes. Include InfiniteDiffusion / Terrain Diffusion as an emerging research reference, and clearly distinguish claims from the project page/paper from production-ready recommendations.

Include a `## Sources` section with source names and URLs. Do not fabricate sources. If uncertain, state uncertainty.

## Forbidden Scope

Do not recommend:

- player-facing map, minimap, compass, bearing, GPS, objective marker, HUD clock, explicit time readout, or orientation confidence UI;
- modern survival tools;
- combat, gore, chase, monster framing, or fantasy magic;
- generic pine forest assumptions;
- asset pack dumps;
- full production terrain pipeline beyond the research/spec needs;
- exact replication of protected or culturally sensitive real places.

## Output Constraint

Return only the final Markdown document content for `001_macro_island_terrain_navigation_research.md`.
