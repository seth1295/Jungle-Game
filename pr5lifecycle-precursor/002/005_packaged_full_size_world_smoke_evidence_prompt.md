# PR5 Batch 002 Research Prompt 005 — Packaged Full-Size World Smoke Evidence

You are an independent no-context researcher AI working from this prompt only.

Your job is to produce a Markdown research/spec document for **Jungle Game**, a UE5 Linux-first first-person jungle survival-simulation project.

## Absolute final deliverable requirement

Your final answer must include a **downloadable Markdown file attachment**.

The file must be named exactly:

```text
005_packaged_full_size_world_smoke_evidence_research.md
```

This is not optional. A chat-only answer is incomplete. A summary without a downloadable `.md` file is incomplete. A pasted Markdown draft without a downloadable file is incomplete.

End your response by making the completed Markdown document available as a downloadable file named exactly `005_packaged_full_size_world_smoke_evidence_research.md`.

## Project context

Jungle Game is a UE5/Linux-first first-person survival-simulation set around 1000 CE in a dense tropical island jungle. The game must be validated as an actual runnable packaged build, not just as source contracts or editor-only plans.

The project already has a packaged visual smoke concept for the older Cell 0 slice. Recent PR5 lifecycle work added source-level contracts for terrain, biome masks, traversal, creek/riparian systems, ridge/slope/canopy orientation, and world validation evidence. The next batch must make the packaged game prove a **full-size world/map state**.

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

Do not recommend committing generated screenshots, videos, logs, cooked output, packaged build output, caches, or local runtime artifacts.

## Research target

Research and specify how Jungle Game should validate a packaged full-size world state with multi-point visual smoke evidence.

The implementation PR that consumes your research should be able to create or refine a validation path that proves:

1. the packaged game launches the authoritative full-size world;
2. the game can move to multiple developer-only review points;
3. screenshots can be captured from representative world locations;
4. logs identify world, review point, coordinates, terrain class, and relevant state;
5. empty sky/ocean-only spawn regressions are caught;
6. generated artifacts remain ignored and unstaged;
7. the batch ends with a clear evidence report, `.mex` update, and handoff/checkpoint.

## Required Markdown structure for the downloadable file

The downloadable file must use this exact top-level structure:

```markdown
# Packaged Full-Size World Smoke Evidence Research

## Executive Summary
## Assumptions And Constraints
## Packaged Build Validation Goal
## Recommended Multi-Point Smoke Workflow
## Review Point Set
## Screenshot Capture Strategy
## Log And Evidence Fields
## Empty-World / Bad-Spawn Regression Checks
## Performance And Stability Evidence
## Artifact Ignore / Source-Control Hygiene
## Batch Completion Evidence Report
## Implementation-Sized PR Scope
## Files And Areas Likely Touched
## Acceptance Criteria
## Risks And Stop Conditions
## Rejected Or Forbidden Approaches
## Sources And References
```

## Implementation usefulness requirements

Make the document practical for one narrow implementation PR. Include concrete guidance for:

- recommended review point names and minimum set;
- required log line fields;
- screenshot naming conventions;
- what constitutes pass/fail evidence;
- how to keep generated artifacts out of source control;
- how to package or launch under Linux for repeatable visual smoke;
- how this evidence should close PR5 batch 002 and feed the next batch.

Do not write production code. Short pseudocode, command examples, or schema sketches are allowed only if they clarify the spec.

## Source expectations

Use reliable sources where UE5 packaging, screenshot automation, console commands, logging, Linux packaged builds, World Partition, or validation practices matter. Prefer official Unreal Engine documentation and stable UE5 references. Cite sources in the Markdown file. If a fact is version-dependent, label it clearly.

## Final instruction

Your final output must be a downloadable Markdown file named exactly:

```text
005_packaged_full_size_world_smoke_evidence_research.md
```
