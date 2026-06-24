# PR Workflow

This project uses a PR-by-PR gate workflow. Each implementation or validation PR must identify one active gate, follow that gate's allowed scope, pass its required validation, and stop at the gate boundary.

## Required reading order

Before starting any PR lifecycle:

1. Read `AGENTS.md`.
2. Read `.mex/ROUTER.md`.
3. Read `.mex/context/active-memory.md`.
4. Read `.mex/context/project.md`.
5. Read `.mex/context/pr-gates.md`.
6. Read this file.
7. Read the active file under `docs/gates/`.

If the active gate is unclear, missing, or conflicts with the user's current instruction, stop and report the conflict before changing files.

## Standard PR lifecycle

1. Start from clean, updated `main`.
2. Create a scoped branch named for the active gate.
3. Implement only the gate's allowed scope.
4. Avoid every item listed in the gate's forbidden scope.
5. Run the gate's required validation.
6. Inspect `git diff --stat` and the intended diff.
7. Stage intended files only.
8. Commit coherent changes.
9. Push the branch.
10. Open a ready PR against `main`; do not create a draft PR unless requested.
11. Add `@coderabbitai ignore` anywhere in the PR body when using the manual CodeRabbit loop.
12. Run CodeRabbit when requested or when the workflow requires it.
13. Fix only valid in-scope findings.
14. Rerun required validation and CodeRabbit after fixes.
15. Merge only when explicitly authorized and all gates pass.
16. After merge, sync local `main` and confirm clean status.
17. If this PR completes the fourth PR in the current sequential planning batch, create a fresh-chat handoff file under `handoffs/` before starting the next batch.
18. Read the next gate before starting the next PR.

## Gate states

Each gate has one status:

- `planned` — not active yet.
- `active` — current gate for the next PR.
- `passed` — completed and merged.
- `blocked` — cannot proceed without user decision or external validation.

Only one gate should be `active` at a time. This project uses sequential PR execution; do not run parallel implementation PRs.

## Sequential 4-PR planning batches

Plan work in batches of four upcoming PRs, but execute only one PR at a time:

1. Define the next four PRs at a high level.
2. Implement the first PR from clean updated `main`.
3. Validate, review, and merge that PR when authorized and clean.
4. Sync local `main`.
5. Implement the next PR in the batch.
6. Repeat until all four PRs in the batch are complete.
7. Create a handoff file under `handoffs/`.
8. Then reassess and define the next four-PR batch.

This is not parallel execution. Multiple active implementation branches should not be used for normal UE work because maps, config, World Partition assets, PCG graphs, and generated editor files are merge-conflict-prone.

Each PR in a four-PR batch must state:

- its batch position, for example `1/4`, `2/4`, `3/4`, or `4/4`;
- dependency on prior PRs in the same batch;
- allowed scope;
- forbidden scope;
- validation expectations;
- whether it must update the batch handoff.

## Fresh-chat handoff requirement

After completing the fourth PR in any sequential batch, create a tracked Markdown handoff under `handoffs/` before starting the next batch.

Use this naming pattern unless the user gives a better one:

```text
handoffs/YYYY-MM-DD-prNN-prMM-short-topic.md
```

The handoff must be written for a fresh DevSpace chat with no local conversation context. It must include:

- current repo/branch/PR state;
- the four PRs just completed;
- what was validated and how;
- known warnings/deferred risks;
- generated/cache/config files to avoid;
- current game/design direction needed for the next batch;
- next four proposed PRs, if known;
- exact next action for the next DevSpace chat.

If a batch stops early because of a blocker, external dependency, context reset, tool limit, or user pause, create an interim handoff under `handoffs/` with the same structure and clearly mark it as incomplete.

## Scope rules

A PR may not expand beyond its active gate. Useful ideas found during work should be written as notes or next-gate candidates, not silently implemented.

A PR must stop instead of guessing when:

- required environment access is missing;
- UE version, platform, or binary asset choices would change the architecture;
- the requested work conflicts with the active gate;
- validation fails and the fix is outside the gate;
- CodeRabbit reports a valid issue outside the current scope;
- the diff contains generated junk, cache files, secrets, or unrelated files.

## CodeRabbit handling

CodeRabbit findings are advisory until verified against current files. Fix valid in-scope findings only. Skip stale, duplicate, speculative, or out-of-scope findings with a short reason in the final report.

## Current gate chain

The planned gate chain is:

1. `docs/gates/000-green-to-go.md`
2. `docs/gates/001-ue5-linux-validation.md`
3. `docs/gates/002-engine-version-decision.md`
4. `docs/gates/003-ue-project-bootstrap.md`
5. `docs/gates/004-first-playable-slice-plan.md`
6. `docs/gates/005-jungle-cell-0-implementation.md`

The active gate pointer is maintained in `.mex/context/pr-gates.md`.
