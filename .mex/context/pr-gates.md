---
name: pr-gates
description: PR gate workflow routing and active gate pointer.
---

# PR Gates

## Active gate

PR5 Batch 005 terrain math implementation.

## Completed setup gate chain

Historical setup gates were removed with the intentionally deleted broad `docs/` folder. Use `.mex` memory and PR5 lifecycle files as the active gate source.

## Active implementation plan

PR5 lifecycle state lives in `.mex`, `pr5lifecycle-precursor/**`, `pr5lifecycle-runtimefiles/**`, `handoffs/**`, `Source/**`, `scripts/**`, and `Images/**`.

Active Batch 005 files:

- `pr5lifecycle-precursor/005/001_geomorphology_math_core_foundation_prompt.md`
- `pr5lifecycle-precursor/005/002_hydrology_erosion_catchment_solver_prompt.md`
- `pr5lifecycle-precursor/005/003_ridge_valley_graph_and_landform_regions_prompt.md`
- `pr5lifecycle-precursor/005/004_volcanic_subsystem_lava_collapse_rift_prompt.md`
- `pr5lifecycle-precursor/005/005_multi_scale_noise_feature_grammar_prompt.md`
- `pr5lifecycle-runtimefiles/005/001_geomorphology_math_core_foundation_research.md`
- `pr5lifecycle-runtimefiles/005/002_hydrology_erosion_catchment_solver_research.md`
- `pr5lifecycle-runtimefiles/005/003_ridge_valley_graph_and_landform_regions_research.md`
- `pr5lifecycle-runtimefiles/005/004_volcanic_subsystem_lava_collapse_rift_research.md`
- `pr5lifecycle-runtimefiles/005/005_multi_scale_noise_feature_grammar_research.md`

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
