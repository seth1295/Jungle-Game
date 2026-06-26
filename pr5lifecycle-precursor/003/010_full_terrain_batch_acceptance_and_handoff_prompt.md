# PR5 Batch 003 Research Prompt 010 — Full Terrain Batch Acceptance And Handoff

You are an independent no-context researcher AI working from this prompt only.

Your job is to produce a Markdown research/spec document for **Jungle Game**, a UE5 Linux-first first-person project.

## Absolute final deliverable requirement

Your final answer must include a **downloadable Markdown file attachment** named exactly:

```text
010_full_terrain_batch_acceptance_and_handoff_research.md
```

## Project context

PR5 Batch 003 is now a large terrain-generation implementation batch, potentially up to 10 implementation PRs. The batch goal is not minimal cleanup. It is to solve the core terrain-generation foundation for Jungle Game before non-terrain systems resume.

The terrain target is a realistic active volcanic island inside a 16.256 km square world container:

```text
actual island shape: circular/organic, not square
outer circular island edge: sea level beach/coastal shelf
outside island: ocean/below sea level
main landform: about 1400 m active volcanic massif
terrain systems: ridges, catchments, gullies, crater, lava/crust terrain hazards, masks/channels, resolution/LOD, topographic validation
```

This prompt defines how the whole batch should be accepted, validated, and handed off after major terrain implementations.

## Terrain-only scope boundary

Do not design foliage, ecology, weather, audio, survival mechanics, NPCs, story, VFX, final materials, asset packs, or player UI. This is terrain-generation acceptance, validation, sequencing, and handoff only.

Do not recommend player-facing map/minimap/compass/GPS/objective/bearing/time/orientation UI.

## Research target

Produce a batch-level acceptance and handoff spec for a major terrain-generation lifecycle batch.

Answer:

1. What must be true before PR5 Batch 003 can be considered successful?
2. What terrain-generation issues must be solved in this batch rather than deferred?
3. What terrain outputs must exist: heightfield/mesh, island mask, beach ring, ocean mask, massif, ridges, gullies, crater/hazard masks, topo exports, slope/catchment maps?
4. What numeric targets should be met: max elevation, sea-level edge correctness, slope distribution, ridge/gully coverage, beach band continuity, hard-blocker masks?
5. What runtime evidence and developer-only maps must be produced?
6. What should the handoff say before later traversal/environment systems resume?
7. What should be rejected as incomplete terrain work?

## Required Markdown structure

```markdown
# Full Terrain Batch Acceptance And Handoff Research

## Executive Summary
## Terrain-Only Scope Boundary
## Batch 003 Terrain Goal
## Required Major Implementation Outcomes
## Sea-Level Beach Ring Acceptance Criteria
## 1400 m Volcanic Massif Acceptance Criteria
## Ridge / Gully / Catchment Acceptance Criteria
## Crater / Lava-Crust Terrain Hazard Acceptance Criteria
## Runtime Mesh / Resolution Acceptance Criteria
## Terrain Masks And Channel Acceptance Criteria
## Topographic Validation Evidence Requirements
## Numeric Pass / Warning / Fail Thresholds
## Required Handoff Contents
## Later-System Resume Gate
## Large PR Sequencing Recommendations
## Risks And Stop Conditions
## Rejected Or Forbidden Approaches
## Sources And References
```

## Implementation usefulness requirements

Give concrete guidance for:

- final batch acceptance checklist;
- terrain-only handoff template;
- required generated evidence, with source-control hygiene;
- numeric targets and tolerances;
- how to prevent premature drift into foliage/ecology/weather/gameplay;
- how to decide whether Batch 003 needs 5, 10, or more implementation PRs.

This is terrain generation acceptance only.

## Source expectations

Use reliable sources where terrain QA, DEM/topographic metrics, slope/catchment analysis, volcanic geomorphology, UE5 validation, or batch handoff practices matter. Cite sources in the Markdown file.

## Final instruction

Your final output must be a downloadable Markdown file named exactly:

```text
010_full_terrain_batch_acceptance_and_handoff_research.md
```
