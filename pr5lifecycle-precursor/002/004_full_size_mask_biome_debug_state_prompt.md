# PR5 Batch 002 Research Prompt 004 — Full-Size Mask And Biome Debug State

You are an independent no-context researcher AI working from this prompt only.

Your job is to produce a Markdown research/spec document for **Jungle Game**, a UE5 Linux-first first-person jungle survival-simulation project.

## Absolute final deliverable requirement

Your final answer must include a **downloadable Markdown file attachment**.

The file must be named exactly:

```text
004_full_size_mask_biome_debug_state_research.md
```

This is not optional. A chat-only answer is incomplete. A summary without a downloadable `.md` file is incomplete. A pasted Markdown draft without a downloadable file is incomplete.

End your response by making the completed Markdown document available as a downloadable file named exactly `004_full_size_mask_biome_debug_state_research.md`.

## Project context

Jungle Game is a UE5/Linux-first first-person survival-simulation set around 1000 CE in a dense tropical island jungle. The world must communicate navigation and survival evidence through terrain/ecology rather than UI aids. The player should read creek corridors, ridgelines, coast, canopy, slope, wetness, sound, sky windows, weather, and vegetation transitions to make movement decisions.

The project has already completed a PR5 lifecycle batch that created source-level contracts for:

- macro island terrain;
- biome mask vocabulary;
- traversal categories;
- creek/riparian/wet-valley systems;
- ridge/slope/canopy orientation;
- lowland/alluvial/gorge concepts;
- route evidence;
- world validation evidence.

The next batch must convert those contracts into a full-size world/map state that can be inspected in game.

In this project, **map state means the actual loaded UE world/level/landscape state**, not a player-facing map UI.

## Non-negotiable forbidden scope

Do not recommend, design, or imply any player-facing:

- map;
- minimap;
- compass;
- bearing number/readout;
- GPS marker;
- objective marker;
- objective arrow;
- HUD clock;
- explicit time display;
- orientation confidence meter;
- hidden orientation meter;
- character comments that tell the player whether they are oriented;
- modern navigation tools.

Developer-only debug views are allowed only when clearly isolated from player-facing UI and validation-only workflows.

## Research target

Research and specify how Jungle Game should represent, inspect, and validate full-size terrain-derived masks and biome/debug classification state over a rough island world.

The implementation PR that consumes your research should be able to add or define developer-only state for:

1. height bands;
2. slope classes;
3. aspect/shelter classes;
4. ridge/valley/spur/saddle/slope-break classification;
5. creek influence and wetness proxy;
6. coast/ocean distance and coastal transition classes;
7. lowland/alluvial/gorge candidates;
8. traversal hard blocker;
9. traversal soft blocker;
10. traversal affordance zones;
11. false affordance candidates;
12. biome placeholder zones;
13. review point classification reports.

## Required Markdown structure for the downloadable file

The downloadable file must use this exact top-level structure:

```markdown
# Full-Size Mask And Biome Debug State Research

## Executive Summary
## Assumptions And Constraints
## Terrain-Derived Mask Model
## Recommended Mask Set For Jungle Game
## Biome Placeholder Classification Model
## Traversal Classification Model
## False Affordance Classification
## Developer-Only Debug View Strategy
## Review Point Classification Reports
## World Partition / Boundary Continuity Checks
## Packaged-Build Safety For Debug Data
## Implementation-Sized PR Scope
## Files And Areas Likely Touched
## Acceptance Criteria
## Risks And Stop Conditions
## Rejected Or Forbidden Approaches
## Sources And References
```

## Implementation usefulness requirements

Make the document practical for one narrow implementation PR. Include concrete guidance for:

- canonical mask names;
- classification enums or data categories;
- representative review point outputs;
- how to prove masks align with terrain features;
- how to keep debug data developer-only;
- how to detect fake square/grid-aligned biome errors;
- how to validate classification continuity across large-world regions.

Do not write production code. Short pseudocode, config examples, or schema sketches are allowed only if they clarify the spec.

## Source expectations

Use reliable sources where UE5 debug visualization, Landscape layers, data assets, World Partition, terrain classification, hydrology proxying, or ecology zoning practices matter. Prefer official Unreal Engine documentation and stable technical references. Cite sources in the Markdown file. If a fact is version-dependent, label it clearly.

## Final instruction

Your final output must be a downloadable Markdown file named exactly:

```text
004_full_size_mask_biome_debug_state_research.md
```
