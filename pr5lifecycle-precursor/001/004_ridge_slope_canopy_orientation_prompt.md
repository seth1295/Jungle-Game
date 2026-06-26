# Research Prompt — PR5 Batch 001 / File 004

## Output File To Produce

Produce one Markdown document named:

```text
004_ridge_slope_canopy_orientation_research.md
```

## Role

You are an independent research/specification assistant. You do not have repo access and must not assume hidden project context. Your task is to produce a factual, implementation-useful Markdown research/spec document for a UE5 game project.

## Project Context

Project name: **Jungle Game**.

Jungle Game is a Linux-first Unreal Engine 5 first-person 3D survival-simulation set in a dense tropical jungle island world, approximately around 1000 CE. The world should feel physically and ecologically normal. The unsettling element is a nonviolent relict hominin presence that observes, tests, and changes the player's environment from distance. The player is not being hunted; the player is being evaluated.

The game is world-first and no-map: players should infer orientation from ridges, spurs, saddles, slope, creek flow, coast/ocean sound, canopy gaps, sky windows, sun/shadow, stars/moon where visible, weather, vegetation transitions, and remembered terrain. The UI must not rescue them.

Hard design guard: no player-facing map, minimap, compass, bearing number/readout, GPS, objective marker navigation, HUD clock, explicit time readout, orientation confidence UI, hidden orientation meter, or character comments that tell the player whether they are oriented.

## Research Target

Research and specify **ridge, slope, canopy-gap, sky-window, sun/shadow, night, and weather orientation systems** for a dense tropical jungle game without map or compass UI.

Focus on diegetic orientation evidence and how canopy/weather can degrade or reveal it.

## Required Research Questions

Answer these questions with implementation-useful detail:

1. How can ridges, spurs, saddles, mountain silhouettes, slope breaks, and skyline windows support no-map first-person orientation?
2. How can canopy closure, canopy gaps, sky windows, visible sun/shadow, stars/moon, and horizon/silhouette glimpses support imperfect orientation?
3. How should dense canopy, gorges, gullies, rain, fog, cloud, night, wet ground, and terrain occlusion degrade orientation cues without making the world unreadable?
4. What role should slope feel, fatigue pressure, footing risk, wind exposure, sound, and vegetation transition play in ridge/slope movement decisions?
5. How can the design avoid hidden orientation meters, explicit feedback, or character comments while still making recovery possible?
6. What developer-only debug/evidence views should later prove orientation cues are present and degraded correctly?
7. What validation points and screenshot angles should be captured later?

## Expected Markdown Structure

Use this exact top-level structure:

```markdown
# Ridge Slope Canopy Orientation Research

## Executive Summary
## Design Goals For Jungle Game
## Ridge And Slope Orientation Principles
## Canopy Gap And Sky Window Evidence
## Sun, Shadow, Night, And Weather Cue Model
## Cue Degradation Rules
## Traversal Pressure And Recovery Design
## UE5 Implementation Guidance
## Developer-Only Debug And Evidence Views
## Validation Points And Screenshot Plan
## Risks, Pitfalls, And Forbidden Drift
## PR-Ready Recommendations
## Sources
```

## Implementation Usefulness Requirements

The document must be usable later as one PR input. Include concrete recommendations for:

- ridge/slope/canopy mask names;
- orientation cue categories;
- cue degradation states;
- debug view names;
- validation point types;
- screenshot/evidence view names;
- data struct or enum naming suggestions where helpful;
- what should be implemented now versus deferred.

Do not write production C++ code. Pseudocode or enum examples are allowed only if they clarify the spec.

## Factuality Requirements

Use factual navigation/orientation, rainforest canopy, lighting, weather, terrain, and perception principles. Prefer reliable sources such as ecology references, meteorology/astronomy/navigation education sources, geoscience/terrain references, government environment agencies, and official Unreal Engine documentation for lighting/weather/debug implementation notes.

Include a `## Sources` section with source names and URLs. Do not fabricate sources. If uncertain, state uncertainty.

## Forbidden Scope

Do not recommend:

- player-facing map, minimap, compass, bearing, GPS, objective marker, HUD clock, explicit time readout, or orientation confidence UI;
- character lines that tell the player whether they are oriented;
- modern survival tools;
- combat, gore, chase, monster framing, or fantasy magic;
- a perfectly reliable navigation system;
- complete astronomy/weather simulation beyond research/spec needs;
- generic temperate forest assumptions without tropical adaptation.

## Output Constraint

Return only the final Markdown document content for `004_ridge_slope_canopy_orientation_research.md`.
