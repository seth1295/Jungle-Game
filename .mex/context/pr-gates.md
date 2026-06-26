---
name: pr-gates
description: PR gate workflow routing and active gate pointer.
---

# PR Gates

## Active gate

`Docs/PRPlan/006_ImplementationPRSequence.md`

## Completed setup gate chain

1. `docs/gates/000-green-to-go.md`
2. `docs/gates/001-ue5-linux-validation.md`
3. `docs/gates/002-engine-version-decision.md`
4. `docs/gates/003-ue-project-bootstrap.md`
5. `docs/gates/004-first-playable-slice-plan.md`
6. `docs/gates/005-jungle-cell-0-implementation.md`

## Active implementation plan

- `Docs/PRPlan/000_FirstPlayableImplementationRoadmap.md`
- `Docs/PRPlan/006_ImplementationPRSequence.md`

The active implementation plan defines the next sequential four-PR batch. The roadmap PR labels inside `Docs/PRPlan/` are planning labels; actual GitHub PR numbers may differ.

## Routing rule

Before any PR lifecycle, load:

1. `AGENTS.md`
2. `.mex/ROUTER.md`
3. `.mex/context/active-memory.md`
4. `.mex/context/project.md`
5. `.mex/context/pr-gates.md`
6. `docs/PR_WORKFLOW.md`
7. the active gate file listed above

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
- After the fourth PR in a sequential planning batch is complete, create a tracked fresh-chat handoff under `handoffs/` before starting the next batch.

## Completion policy

A gate is complete only when its exit criteria pass, CodeRabbit is clean or valid findings are resolved, `.mex/context/active-memory.md` is updated for the completed PR, and the PR is merged to `main` with local `main` synced cleanly.
