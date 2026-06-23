# Gate 000 — Green-To-Go Workflow

## Status

passed

## Purpose

Install the PR-by-PR gate workflow that controls future validation and implementation work.

This gate does not make the project implementation-green by itself. It creates the process that future gates must follow. In the merged state of this PR, Gate 001 becomes the active next gate.

## Allowed scope

This gate may change only:

- `AGENTS.md`
- `.mex/ROUTER.md`
- `.mex/context/active-memory.md`
- `.mex/context/pr-gates.md`
- `docs/PR_WORKFLOW.md`
- `docs/GREEN_TO_GO.md`
- files under `docs/gates/`

## Forbidden scope

This gate must not:

- create a UE project;
- add game source code;
- add assets;
- install UE;
- run engine validation;
- change GitHub Actions;
- alter remote settings;
- change unrelated workflow rules.

## Required inputs

- Current `AGENTS.md`.
- Current `.mex/ROUTER.md`.
- Current project state on `main`.
- User approval to implement the workflow.

## Required validation

- Confirm the branch is based on clean updated `main`.
- Confirm the diff is docs/workflow only.
- Run `git diff --check`.
- Run CodeRabbit if the user asked for the normal review loop.

## Exit criteria

This gate passes when:

- the PR gate workflow exists;
- MEX routes PR work through the gate system;
- `AGENTS.md` requires the PR workflow and active gate before PR work;
- future gate files exist with clear allowed/forbidden scopes;
- CodeRabbit is clean or only non-blocking out-of-scope findings remain;
- the PR is merged to `main`.

## CodeRabbit handling

Fix valid in-scope findings only. Since this gate is workflow-only, code/game/engine findings are out of scope.

## Next gate

`docs/gates/001-ue5-linux-validation.md`

## Stop conditions

Stop and report if:

- the user asks to add UE/game files in this PR;
- the gate docs conflict with `AGENTS.md` authority rules;
- the diff includes generated output, secrets, binaries, or unrelated files;
- CodeRabbit reports a valid workflow issue that cannot be fixed inside this gate.
