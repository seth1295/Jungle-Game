# PR5 Batch 002 Research Prompt 003 — Full-Size Terrain In-Game Shell

You are an independent no-context researcher AI working from this prompt only.

Your job is to produce a Markdown research/spec document for **Jungle Game**, a UE5 Linux-first first-person jungle survival-simulation project.

## Absolute final deliverable requirement

Your final answer must include a **downloadable Markdown file attachment**.

The file must be named exactly:

```text
003_full_size_terrain_ingame_shell_research.md
```

This is not optional. A chat-only answer is incomplete. A summary without a downloadable `.md` file is incomplete. A pasted Markdown draft without a downloadable file is incomplete.

End your response by making the completed Markdown document available as a downloadable file named exactly `003_full_size_terrain_ingame_shell_research.md`.

## Project context

Jungle Game is a UE5/Linux-first first-person survival-simulation set around 1000 CE in a dense tropical island jungle. The game is terrain-first: creeks, ridges, slope, coast, sky windows, weather, sound, vegetation transitions, and remembered landmarks should support navigation without map UI.

The project has already completed a PR5 lifecycle batch that created source-level contracts for terrain, biome masks, traversal categories, creek/riparian systems, ridge/slope/canopy orientation, and validation evidence. The visible packaged build still resembles an older Cell 0 playable slice. The next batch must make a full-size world/map state visible in game.

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

- final art/content density;
- asset-pack dumping;
- broad unreviewable binary churn;
- a world shell that is visually indistinguishable from the old Cell 0 slice.

## Research target

Research and specify how Jungle Game should turn deterministic full-size terrain source into a rough but visible **full-size in-game terrain shell** in UE5.

The implementation PR that consumes your research should make the packaged game clearly stop looking like only the old Cell 0 slice and instead load a rough full-size island/world shell.

The shell should support:

1. visible large terrain extent;
2. ocean or natural island boundary placeholder;
3. multiple developer-only review spawn points;
4. representative coast, ridge, creek/valley, basin/lowland, and highland/mountain review locations;
5. safe player start placement;
6. rough traversal sanity without final collision polish;
7. screenshot validation from multiple points;
8. clear separation between the full-size world and future Cell 0 placement.

## Required Markdown structure for the downloadable file

The downloadable file must use this exact top-level structure:

```markdown
# Full-Size Terrain In-Game Shell Research

## Executive Summary
## Assumptions And Constraints
## UE5 Landscape / World Partition Realization Strategy
## Recommended In-Game Shell For Jungle Game
## Terrain Import Or Runtime Realization Path
## Ocean / Island Boundary Placeholder Strategy
## Developer-Only Review Spawn Points
## Safe Spawn And Traversal Sanity Rules
## Cell 0 Separation / Future Placement Notes
## Multi-Point Screenshot Validation
## Performance And Streaming Sanity Checks
## Implementation-Sized PR Scope
## Files And Areas Likely Touched
## Acceptance Criteria
## Risks And Stop Conditions
## Rejected Or Forbidden Approaches
## Sources And References
```

## Implementation usefulness requirements

Make the document practical for one narrow implementation PR. Include concrete guidance for:

- how many review points to start with;
- review point names and purpose;
- how to prove the shell is full-size in logs/screenshots;
- how to prevent empty sky/ocean-only spawn regressions;
- how to avoid unreviewable generated asset churn;
- how to validate in packaged Linux builds.

Do not write production code. Short pseudocode, config examples, or schema sketches are allowed only if they clarify the spec.

## Source expectations

Use reliable sources where UE5 Landscape, World Partition, packaging, Linux packaged launch, screenshot automation, or performance behavior matters. Prefer official Unreal Engine documentation and stable UE5 references. Cite sources in the Markdown file. If a fact is version-dependent, label it clearly.

## Final instruction

Your final output must be a downloadable Markdown file named exactly:

```text
003_full_size_terrain_ingame_shell_research.md
```
