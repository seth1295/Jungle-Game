---
name: pr-gates
description: PR gate workflow routing and active gate pointer.
---

# PR Gates

## Active gate

Self-iterative runtime 010 is in final merge gate on PR #71. Runtime 010 acceptance is limited to visible terrain with the player/camera on the playable surface. UBT, offscreen visual smoke, tracked evidence under `Images/InGameEvidence/010/`, repo image inspection, and CodeRabbit rerun are clean. Merge PR #71 when repository checks allow, then sync local `main` and wait for explicit user instruction before starting another runtime.

## Completed setup gate chain

Historical setup gates were removed with the intentionally deleted broad `docs/` folder. Use `.mex` memory and PR5 lifecycle files as the active gate source.

## Active implementation plan

PR5 lifecycle state lives in `.mex`, `pr5lifecycle-precursor/**`, `pr5lifecycle-runtimefiles/**`, `handoffs/**`, `Source/**`, `scripts/**`, and `Images/**`.

Self-iterative lifecycle state lives in `.mex/patterns/self-iterative-lifecycle.md` and `SELF-ITERATIVE/**`. It is repo-owned lifecycle state like PR5 lifecycle. When active, the agent diagnoses with available tools, writes numbered scoped runtime files into `SELF-ITERATIVE/`, implements one runtime file at a time, inspects what changed, updates `.mex`, then runs PR review and merge when authorized.

Batch 005 is complete and must not be reopened unless explicitly requested. PR #60 landed a combined Batch 006 terrain calibration implementation. Future PR5 runtime work must not use a combined PR as the default execution shape. The active follow-up lane is the extended self-iterative Batch006 morphology loop. Runtime 009 is complete on PR #70 with tracked evidence under `Images/TerrainPreview/009/`; wait for explicit user instruction before extending the terrain loop again.

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
7. `.mex/patterns/self-iterative-lifecycle.md` when self-iterative lifecycle is active
8. relevant active PR5 lifecycle precursor/runtime files
9. relevant `SELF-ITERATIVE/**` runtime files when self-iterative lifecycle is active

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
- For PR5 runtime work, one canonical runtime file equals one sequential implementation PR unless the user explicitly approves a combine, skip, or defer.
- Do not compress multiple runtime files into one umbrella implementation PR.
- For self-iterative lifecycle work, one `SELF-ITERATIVE/<number>_*.md` runtime file equals one scoped diagnose/implement/inspect/PR iteration unless the user explicitly approves combining, skipping, or deferring runtime files.
- Plan up to four PRs ahead, but implement and merge one PR at a time.
- Every completed PR must update `.mex/context/active-memory.md`; do not start the next implementation PR from stale `.mex` state.
- PR5 workflow authority means actual game/source implementation unless the user explicitly asks for Markdown-only lifecycle files.
- After the fourth PR in a sequential planning batch is complete, create a tracked fresh-chat handoff under `handoffs/` before starting the next batch.

## Completion policy

A gate is complete only when its exit criteria pass, CodeRabbit is clean or valid findings are resolved, `.mex/context/active-memory.md` is updated for the completed PR, and the PR is merged to `main` with local `main` synced cleanly.

For PR5 runtime batches, completion also requires every expected runtime file to have a landed sequential PR or a recorded user-approved combine/skip/defer state in `.mex`.

For self-iterative lifecycle runs, completion also requires the numbered `SELF-ITERATIVE/**` runtime file state, tool evidence, generated-result inspection, PR/review/merge state, and next-runtime decision to be recorded in `.mex/context/active-memory.md`.
