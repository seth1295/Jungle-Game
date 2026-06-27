---
name: pr5-implementation-workflow
description: PR5 lifecycle execution rule for implementation batches.
---

# PR5 Implementation Workflow

## Rule

When the user gives PR5 workflow authority, merge authority, or says to run the workflow, treat the request as authorization to implement the actual approved game/source scope for that batch.

Do not convert the request into a docs-only, lifecycle-only, or prompt-file-only PR unless the user explicitly asks for only precursor/runtime Markdown deliverables.

## Required Behavior

1. Inspect current `.mex` state, branch, status, and relevant PR5 lifecycle files.
2. Identify the active implementation target from the latest user instruction and lifecycle files.
3. Implement the actual source/tool/game changes needed for that target.
4. Use lifecycle precursor/runtime files as inputs, not as the whole deliverable.
5. Include generated evidence only when it proves the implemented source/tool behavior.
6. Stage intended files only.
7. Run relevant checks.
8. Commit, push, open PR, and merge only when merge authority is active and gates are clean.

## Runtime File Granularity Rule

For PR5 runtime consumption, one canonical runtime file under `pr5lifecycle-runtimefiles/<batch>/` equals one sequential implementation PR unless the user explicitly authorizes combining specific runtime files.

Do not compress a multi-file runtime batch into a single implementation PR, umbrella PR, calibration suite, acceptance suite, or summary PR. A combined PR may only happen when the user explicitly says to combine files, and that combine/skip/defer decision must be recorded in `.mex/context/active-memory.md` before completion is claimed.

After each runtime-file PR lands, update `.mex/context/active-memory.md` with:

- consumed runtime file
- PR number, branch, and commit/merge state when known
- implemented source/tool/evidence scope
- validation results and blockers
- next runtime file or handoff requirement

A batch is complete only when every expected runtime file has either landed through its own PR lifecycle step or has an explicit user-approved combine, skip, or defer record in `.mex`. A merged PR proves that PR landed; it does not by itself prove the runtime batch is complete.

Agents must preserve the canonical runtime files as execution authority. Do not replace them with copied docs, summaries, handoff prose, or the agent's own compressed interpretation.

## PR5 Batch 004 Current Meaning

Batch 004 is not a docs packaging batch. It is terrain implementation work:

- 6x linear world/map scale in the terrain authority.
- Anti-radial volcanic catchment/ridge/gully rewrite.
- Regenerated terrain preview evidence under `Images/TerrainPreview/` proving the scale and morphology changes.

A valid Batch 004 implementation must touch game/source/tooling as needed, especially the canonical terrain model and preview exporter. A PR that only saves precursor/runtime Markdown is not sufficient unless the user explicitly asks for that only.

## Current User Correction

The user explicitly rejected hand-holding and docs-only merging. In this project context, profanity is not a blocker or a request to stop. Treat it as strong scope correction: build the actual game/source implementation.
