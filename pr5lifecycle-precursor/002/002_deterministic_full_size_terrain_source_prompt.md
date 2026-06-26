# PR5 Batch 002 Research Prompt 002 — Deterministic Full-Size Terrain Source

You are an independent no-context researcher AI working from this prompt only.

Your job is to produce a Markdown research/spec document for **Jungle Game**, a UE5 Linux-first first-person jungle survival-simulation project.

## Absolute final deliverable requirement

Your final answer must include a **downloadable Markdown file attachment**.

The file must be named exactly:

```text
002_deterministic_full_size_terrain_source_research.md
```

This is not optional. A chat-only answer is incomplete. A summary without a downloadable `.md` file is incomplete. A pasted Markdown draft without a downloadable file is incomplete.

End your response by making the completed Markdown document available as a downloadable file named exactly `002_deterministic_full_size_terrain_source_research.md`.

## Project context

Jungle Game is a UE5/Linux-first first-person survival-simulation set around 1000 CE in a dense tropical island jungle. The player should navigate from diegetic environmental evidence: ridge lines, creek flow, slope, coast, sky windows, sound, weather, vegetation transition, and remembered landmarks.

The project has already completed a PR5 lifecycle batch that created source-level contracts for macro island terrain, biome masks, traversal categories, creek/riparian/wet-valley ecosystems, ridge/slope/canopy orientation, and validation evidence.

The visible packaged game still resembles an older Cell 0 playable slice. The next batch must move directly toward a **full-size world/map state**.

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

Also do not recommend:

- random noise-only terrain;
- paid/proprietary terrain tools as the required path;
- huge unexplained binary dumps;
- terrain that exists only as vibes without reproducible source parameters.

## Research target

Research and specify a deterministic terrain source path for a 16 km-class tropical island terrain suitable for UE5 World Partition / Landscape work.

The implementation PR that consumes your research should be able to define or produce a reproducible source terrain pipeline with:

1. island outline and coastline;
2. ocean boundary assumption;
3. mountain spine and highland massing;
4. ridges, spurs, saddles, slope breaks;
5. creek heads and flow structure;
6. valleys, basins, gullies, alluvial areas, gorge candidates;
7. coast types: beach, rocky shore, mangrove, creek mouth, cliff coast;
8. derived terrain channels: height, slope, aspect, flow, ridge/valley, wetness proxy, coast distance, creek influence;
9. review samples that prove the terrain is navigable and not arbitrary noise.

## Required Markdown structure for the downloadable file

The downloadable file must use this exact top-level structure:

```markdown
# Deterministic Full-Size Terrain Source Research

## Executive Summary
## Assumptions And Constraints
## Target Terrain Scale And Resolution
## Deterministic Source Pipeline Options
## Recommended Pipeline For Jungle Game
## Island Outline And Coastline Model
## Mountain Spine / Ridge / Spur / Saddle Model
## Creek Head / Flow / Basin / Valley Model
## Derived Terrain Channels
## Review Point And Sample Strategy
## Artifact And Source-Control Strategy
## Implementation-Sized PR Scope
## Files And Areas Likely Touched
## Acceptance Criteria
## Risks And Stop Conditions
## Rejected Or Forbidden Approaches
## Sources And References
```

## Implementation usefulness requirements

Make the document practical for one narrow implementation PR. Include concrete guidance for:

- deterministic seed/config shape;
- heightfield dimensions and scale assumptions;
- source data representation;
- whether generated outputs should be committed or ignored;
- what small reviewable artifacts are acceptable;
- what debug/sample tables should be produced;
- how to validate terrain channels before UE import/runtime use.

Do not write production code. Short pseudocode, config examples, or schema sketches are allowed only if they clarify the spec.

## Source expectations

Use reliable sources where UE5 Landscape dimensions, World Partition, heightmap import, terrain generation, hydrology, or terrain-analysis facts matter. Prefer official Unreal Engine documentation and stable technical references. Cite sources in the Markdown file. If a fact is version-dependent, label it clearly.

## Final instruction

Your final output must be a downloadable Markdown file named exactly:

```text
002_deterministic_full_size_terrain_source_research.md
```
