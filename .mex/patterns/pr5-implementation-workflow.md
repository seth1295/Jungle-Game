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

## PR5 Batch 004 Current Meaning

Batch 004 is not a docs packaging batch. It is terrain implementation work:

- 6x linear world/map scale in the terrain authority.
- Anti-radial volcanic catchment/ridge/gully rewrite.
- Regenerated terrain preview evidence under `Images/TerrainPreview/` proving the scale and morphology changes.

A valid Batch 004 implementation must touch game/source/tooling as needed, especially the canonical terrain model and preview exporter. A PR that only saves precursor/runtime Markdown is not sufficient unless the user explicitly asks for that only.

## Current User Correction

The user explicitly rejected hand-holding and docs-only merging. In this project context, profanity is not a blocker or a request to stop. Treat it as strong scope correction: build the actual game/source implementation.
