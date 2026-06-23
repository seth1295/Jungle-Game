---
name: pr-gates
description: PR gate workflow routing and active gate pointer.
---

# PR Gates

## Active gate

`docs/gates/001-ue5-linux-validation.md`

## Gate chain

1. `docs/gates/000-green-to-go.md`
2. `docs/gates/001-ue5-linux-validation.md`
3. `docs/gates/002-engine-version-decision.md`
4. `docs/gates/003-ue-project-bootstrap.md`
5. `docs/gates/004-first-playable-slice-plan.md`
6. `docs/gates/005-jungle-cell-0-implementation.md`

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
- Only one gate should be active at a time unless the user explicitly approves parallel work.

## Completion policy

A gate is complete only when its exit criteria pass, CodeRabbit is clean or valid findings are resolved, and the PR is merged to `main` with local `main` synced cleanly.
