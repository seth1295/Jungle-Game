---
name: pr-gates
description: PR gate workflow routing and active gate pointer.
---

# PR Gates

## Active gate

PR5 Batch 006 terrain calibration suite is complete in PR #60; next implementation gate is unset until user review or explicit next-lane instruction.

## Completed setup gate chain

Historical setup gates were removed with the intentionally deleted broad `docs/` folder. Use `.mex` memory and PR5 lifecycle files as the active gate source.

## Active implementation plan

PR5 lifecycle state lives in `.mex`, `pr5lifecycle-precursor/**`, `pr5lifecycle-runtimefiles/**`, `handoffs/**`, `Source/**`, `scripts/**`, and `Images/**`.

Batch 005 is complete and must not be reopened unless explicitly requested. Batch 006 terrain calibration suite is implemented in PR #60 from runtime files 001 through 005. Do not reopen Batch 006 unless the user explicitly asks for a fix or follow-up review. The next implementation gate is intentionally unset after PR #60 until user review or a new explicit PR5 batch/lane instruction.

Active Batch 006 precursor files:

- `pr5lifecycle-precursor/006/001_dem_benchmarking_real_volcanic_islands_prompt.md`
- `pr5lifecycle-precursor/006/002_offline_generator_runtime_bridge_architecture_prompt.md`
- `pr5lifecycle-precursor/006/003_morphology_validation_preview_suite_prompt.md`
- `pr5lifecycle-precursor/006/004_terrain_math_implementation_pr_sequence_prompt.md`
- `pr5lifecycle-precursor/006/005_full_terrain_math_acceptance_handoff_prompt.md`

Active Batch 006 runtime files:

- `pr5lifecycle-runtimefiles/006/001_dem_benchmarking_real_volcanic_islands_research.md`
- `pr5lifecycle-runtimefiles/006/002_offline_generator_runtime_bridge_architecture_research.md`
- `pr5lifecycle-runtimefiles/006/003_morphology_validation_preview_suite_research.md`
- `pr5lifecycle-runtimefiles/006/004_terrain_math_implementation_pr_sequence_research.md`
- `pr5lifecycle-runtimefiles/006/005_full_terrain_math_acceptance_handoff_research.md`

Removed duplicate runtime file `pr5lifecycle-runtimefiles/006/001_geomorphology_math_core_foundation_research(1).md` must not be restored or treated as valid Batch 006 input.

Batch 006 output evidence is generated under `Images/TerrainPreview/003/`. Do not overwrite `Images/TerrainPreview/001/`, `Images/TerrainPreview/002/`, or `Images/TerrainPreview/003/` in later work; create the next numbered preview run folder when new accepted terrain evidence is required.

Broad `docs/**` and `Docs/**` folders were intentionally removed. Do not route future PR gates through deleted docs folders unless the user explicitly asks to recreate documentation structure.

## Routing rule

Before any PR lifecycle, load:

1. `AGENTS.md`
2. `.mex/ROUTER.md`
3. `.mex/context/active-memory.md`
4. `.mex/context/project.md`
5. `.mex/context/pr-gates.md`
6. `.mex/patterns/pr5-implementation-workflow.md`
7. relevant active PR5 lifecycle precursor/runtime files

## Enforcement rules

- Every PR must identify its active gate.
- If no active gate is defined, stop.
- If the requested work conflicts with the active gate, stop and ask whether to change gates.
- Do not skip gates unless the user explicitly says to skip or replace a gate.
- Do not expand a PR beyond its gate.
- Do not silently implement useful adjacent ideas; record them as next-gate notes instead.
- After a gate merges, read the next gate before starting the next PR.
- Only one gate should be active at a time.
- Execute PRs sequentially; do not run normal UE implementation PRs in parallel.
- Plan up to four PRs ahead, but implement and merge one PR at a time.
- Every completed PR must update `.mex/context/active-memory.md`; do not start the next implementation PR from stale `.mex` state.
- PR5 workflow authority means actual game/source implementation unless the user explicitly asks for Markdown-only lifecycle files.
- After the fourth PR in a sequential planning batch is complete, create a tracked fresh-chat handoff under `handoffs/` before starting the next batch.

## Completion policy

A gate is complete only when its exit criteria pass, CodeRabbit is clean or valid findings are resolved, `.mex/context/active-memory.md` is updated for the completed PR, and the PR is merged to `main` with local `main` synced cleanly.
