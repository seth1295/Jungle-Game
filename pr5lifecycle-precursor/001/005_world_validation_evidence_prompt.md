# Research Prompt — PR5 Batch 001 / File 005

## Output File To Produce

Produce one Markdown document named:

```text
005_world_validation_evidence_research.md
```

## Role

You are an independent research/specification assistant. You do not have repo access and must not assume hidden project context. Your task is to produce a factual, implementation-useful Markdown research/spec document for a UE5 game project.

## Project Context

Project name: **Jungle Game**.

Jungle Game is a Linux-first Unreal Engine 5 first-person 3D survival-simulation set in a dense tropical jungle island world, approximately around 1000 CE. The world should feel physically and ecologically normal. The unsettling element is a nonviolent relict hominin presence that observes, tests, and changes the player's environment from distance. The player is not being hunted; the player is being evaluated.

The project is shifting toward world-first implementation: terrain, biome masks, traversal resistance, route classes, debug evidence, and packaged visual validation should prove the game world is playable and readable. Generated output and screenshots should be validation artifacts, not committed source bloat.

Hard design guard: no player-facing map, minimap, compass, bearing number/readout, GPS, objective marker navigation, HUD clock, explicit time readout, orientation confidence UI, hidden orientation meter, or character comments that tell the player whether they are oriented.

## Research Target

Research and specify a **world validation evidence workflow** for UE5 large-world terrain/biome/traversal systems.

Focus on what developer-only evidence should be captured to prove terrain, biome masks, traversal classes, route evidence, weather/night impacts, and no-map readability are working.

## Required Research Questions

Answer these questions with implementation-useful detail:

1. What evidence should prove terrain form, biome outputs, traversal resistance, route classes, and visual readability in a large UE5 world?
2. What developer-only debug views are needed for terrain channels, mask values, traversal classes, biome outputs, route evidence, and component/streaming continuity?
3. What first-person screenshot viewpoints should be captured for coast, creek, ridge, saddle, basin, gorge, lowland, canopy gap, slope break, and false affordance cases?
4. How should package/smoke validation differ from editor-only validation?
5. How should the project avoid committing generated output, cache files, screenshots, videos, packaged builds, logs, or other bloat while still recording validation evidence?
6. What minimum acceptance evidence should each world-system PR include?
7. How should CodeRabbit/review-friendly Markdown validation notes be structured?

## Expected Markdown Structure

Use this exact top-level structure:

```markdown
# World Validation Evidence Research

## Executive Summary
## Design Goals For Jungle Game
## Evidence Categories
## Developer-Only Debug Views
## First-Person Screenshot View Plan
## Package And Runtime Smoke Evidence
## Generated Output And Repo Hygiene Rules
## UE5 Implementation Guidance
## Per-PR Validation Checklist
## Review-Friendly Evidence Note Template
## Risks, Pitfalls, And Forbidden Drift
## PR-Ready Recommendations
## Sources
```

## Implementation Usefulness Requirements

The document must be usable later as one PR input. Include concrete recommendations for:

- evidence category names;
- debug view names;
- screenshot view names;
- validation checklist items;
- package/run smoke expectations;
- repo hygiene rules;
- Markdown validation note templates;
- data struct or enum naming suggestions where helpful;
- what should be implemented now versus deferred.

Do not write production C++ code. Pseudocode or enum examples are allowed only if they clarify the spec.

## Factuality Requirements

Use factual UE5 validation, build/package, World Partition, PCG/debug, QA, and version-control hygiene principles. Prefer official Unreal Engine documentation for UE-specific claims and reliable software QA/build/release references for validation workflow claims.

Include a `## Sources` section with source names and URLs. Do not fabricate sources. If uncertain, state uncertainty.

## Forbidden Scope

Do not recommend:

- player-facing debug views or navigation aids;
- committed packaged builds, screenshots, logs, cache folders, or generated editor junk;
- broad CI/CD redesign unless it directly supports the evidence workflow;
- modern UI navigation systems;
- combat, gore, chase, monster framing, or fantasy magic;
- validation that depends on private local paths only.

## Output Constraint

Return only the final Markdown document content for `005_world_validation_evidence_research.md`.
