# PR5 Batch 002 Research Prompt 001 — Full-Size World Authority

You are an independent no-context researcher AI working from this prompt only.

Your job is to produce a Markdown research/spec document for **Jungle Game**, a UE5 Linux-first first-person jungle survival-simulation project.

## Absolute final deliverable requirement

Your final answer must include a **downloadable Markdown file attachment**.

The file must be named exactly:

```text
001_full_size_world_authority_research.md
```

This is not optional. A chat-only answer is incomplete. A summary without a downloadable `.md` file is incomplete. A pasted Markdown draft without a downloadable file is incomplete.

End your response by making the completed Markdown document available as a downloadable file named exactly `001_full_size_world_authority_research.md`.

## Project context

Jungle Game is a UE5/Linux-first first-person survival-simulation set around 1000 CE in a dense tropical island jungle. The world should be physically and ecologically normal. The core experience is terrain-first survival, navigation, ecology, weather, disorientation, and slow discovery. A quiet relict hominin presence may observe and test the player from distance, but this is not a combat/chase/gore game.

The project has already completed a PR5 lifecycle batch that created source-level contracts for:

- macro island terrain;
- terrain/biome channel vocabulary;
- biome masks and traversal classes;
- creek/riparian/wet-valley ecosystems;
- ridge/slope/canopy orientation;
- validation evidence.

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

Developer-only validation/debug facilities are allowed only when clearly isolated from player-facing UI.

## Research target

Research and specify how Jungle Game should establish **full-size world authority** before further full-world terrain work continues.

The implementation PR that consumes your research should be able to answer:

1. What is the authoritative project-owned full-size world/level/map asset or path?
2. What should packaged builds launch by default?
3. How should template `OpenWorld` usage be retired, wrapped, redirected, or explicitly treated?
4. How should Cell 0 be treated during the transition: old validation slice, future placed location, or temporary review anchor?
5. How should developer-only review spawn points be represented without becoming navigation UI?
6. What logs/evidence should prove the correct world loaded?
7. What criteria prove that full-size world authority is established?

## Required Markdown structure for the downloadable file

The downloadable file must use this exact top-level structure:

```markdown
# Full-Size World Authority Research

## Executive Summary
## Assumptions And Constraints
## UE5 World / Map Authority Concepts
## Recommended World Authority Model For Jungle Game
## Default Map And Packaged Launch Strategy
## Template OpenWorld / Legacy Slice Handling
## Cell 0 Transition Strategy
## Developer-Only Review Spawn Registry
## Logging And Validation Evidence
## Implementation-Sized PR Scope
## Files And Areas Likely Touched
## Acceptance Criteria
## Risks And Stop Conditions
## Rejected Or Forbidden Approaches
## Sources And References
```

## Implementation usefulness requirements

Make the document practical for one narrow implementation PR. Include concrete guidance for:

- recommended authoritative world naming/path convention;
- packaged startup behavior;
- review spawn naming and purpose;
- log lines or evidence fields that prove the correct world loaded;
- how to prevent Cell 0 from defining or faking the whole world;
- how to validate the right world in editor and packaged Linux builds.

Do not write production code. Short pseudocode, config examples, or schema sketches are allowed only if they clarify the spec.

## Source expectations

Use reliable sources where UE5 behavior matters. Prefer official Unreal Engine documentation, Epic docs, and stable UE5 references. Cite sources in the Markdown file. If a fact is version-dependent, label it clearly.

## Final instruction

Your final output must be a downloadable Markdown file named exactly:

```text
001_full_size_world_authority_research.md
```
