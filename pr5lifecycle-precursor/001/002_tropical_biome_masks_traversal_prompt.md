# Research Prompt — PR5 Batch 001 / File 002

## Output File To Produce

Produce one Markdown document named:

```text
002_tropical_biome_masks_traversal_research.md
```

## Role

You are an independent research/specification assistant. You do not have repo access and must not assume hidden project context. Your task is to produce a factual, implementation-useful Markdown research/spec document for a UE5 game project.

## Project Context

Project name: **Jungle Game**.

Jungle Game is a Linux-first Unreal Engine 5 first-person 3D survival-simulation set in a dense tropical jungle island world, approximately around 1000 CE. The world should feel physically and ecologically normal. The unsettling element is a nonviolent relict hominin presence that observes, tests, and changes the player's environment from distance. The player is not being hunted; the player is being evaluated.

Current direction is **world-first**. Terrain and biome data should drive traversal, visibility, sound, weather pressure, and environmental evidence. The project should avoid player-facing navigation UI and instead make the world readable through landform, water, canopy, light, sound, weather, and biome transitions.

Hard design guard: no player-facing map, minimap, compass, bearing number/readout, GPS, objective marker navigation, HUD clock, explicit time readout, orientation confidence UI, hidden orientation meter, or character comments that tell the player whether they are oriented.

## Research Target

Research and specify **tropical biome masks and traversal resistance** for a dense wet tropical island jungle.

Focus on mask families that can drive UE5 PCG/debug/validation systems later, while remaining developer-only and never becoming player navigation UI.

## Required Research Questions

Answer these questions with implementation-useful detail:

1. Which terrain-derived and ecology-derived mask families are useful for tropical rainforest traversal and readability?
2. How should masks such as elevation band, slope band, aspect exposure, wetness, ridge influence, valley influence, coast influence, canopy closure, disturbance gaps, visibility, sound propagation, sky windows, footing risk, ecosystem tells, and night danger be defined?
3. How can mask values classify hard blockers, soft blockers, affordances, false affordances, and neutral friction?
4. How should dense vegetation be represented as readable traversal friction rather than random collision noise?
5. What tropical vegetation or terrain conditions create movement resistance, misleading openings, route temptation, wet footing, sound masking, visibility loss, or sky-window orientation cues?
6. What mask value ranges, thresholds, or normalized contracts should a game use as placeholders before final data exists?
7. How should developer-only debug views expose mask logic without becoming player-facing navigation aids?

## Expected Markdown Structure

Use this exact top-level structure:

```markdown
# Tropical Biome Masks And Traversal Research

## Executive Summary
## Design Goals For Jungle Game
## Recommended Mask Families
## Traversal Resistance Model
## Tropical Vegetation And Terrain Inputs
## False Affordances And Readable Friction
## Developer-Only Debug View Requirements
## UE5 PCG And Data Contract Guidance
## Validation Points And Sample Tables
## Risks, Pitfalls, And Forbidden Drift
## PR-Ready Recommendations
## Sources
```

## Implementation Usefulness Requirements

The document must be usable later as one PR input. Include concrete recommendations for:

- mask names;
- normalized value contracts;
- placeholder thresholds;
- traversal category names;
- debug view names;
- data struct or enum naming suggestions where helpful;
- PCG input/output contracts;
- what should be implemented now versus deferred.

Do not write production C++ code. Pseudocode or enum examples are allowed only if they clarify the spec.

## Factuality Requirements

Use factual tropical rainforest ecology, terrain, hydrology, and movement/traversal principles. Prefer reliable sources such as ecology references, government environment agencies, academic/educational sources, land management references, and official Unreal Engine documentation for PCG/debug implementation notes.

Include a `## Sources` section with source names and URLs. Do not fabricate sources. If uncertain, state uncertainty.

## Forbidden Scope

Do not recommend:

- player-facing map, minimap, compass, bearing, GPS, objective marker, HUD clock, explicit time readout, or orientation confidence UI;
- modern survival tools;
- combat, gore, chase, monster framing, or fantasy magic;
- generic pine forest assumptions;
- final asset-pack foliage lists;
- full production biome simulation beyond research/spec needs;
- cultural stereotypes or protected-place replication.

## Output Constraint

Return only the final Markdown document content for `002_tropical_biome_masks_traversal_research.md`.
