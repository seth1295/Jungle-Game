---
name: self-iterative-lifecycle
description: Tool-driven lifecycle for diagnosing, scoping, implementing, inspecting, and iterating fixes from evidence.
---

# Self-Iterative Lifecycle

## Purpose

`self-iterative-lifecycle` is a repo-owned workflow for issues where the agent must use the available project tools to discover the actual fault, write scoped runtime files, implement fixes, inspect the results, and iterate until the evidence proves the issue is resolved.

This workflow is saved project state, like PR5 lifecycle. It is not a chat-only instruction, a temporary prompt note, or a docs-only planning lane.

## Runtime Folder

Runtime files live under:

```text
SELF-ITERATIVE/
```

Runtime files are numbered sequentially:

```text
SELF-ITERATIVE/001_<scope>.md
SELF-ITERATIVE/002_<scope>.md
SELF-ITERATIVE/003_<scope>.md
```

Each runtime file is a scoped implementation unit. Do not compress multiple runtime files into one PR unless the user explicitly approves combining them.

## Authority Model

When the user invokes this lifecycle, the agent must:

1. Treat the current user instruction as the active goal.
2. Load `AGENTS.md`, `.mex/ROUTER.md`, `.mex/context/active-memory.md`, `.mex/context/project.md`, `.mex/context/pr-gates.md`, `.mex/patterns/pr5-implementation-workflow.md`, and this file.
3. Inspect current repo state and classify it in project terms before mutation.
4. Preserve valid existing repo work; do not reset, revert, delete, stash, or branch around meaningful dirty state without explicit user instruction.
5. Use available tools to diagnose the issue, not only screenshots or image review.
6. Write scoped numbered runtime files into `SELF-ITERATIVE/` that describe what will be changed and how success will be inspected.
7. Implement one runtime file at a time.
8. Inspect what the changes did using the strongest available validation evidence.
9. Update `.mex` with results, blockers, next runtime file, and evidence.
10. Run PR lifecycle: branch/commit/push/PR, CodeRabbit, valid fixes only, merge, and local sync when tool access allows and user authority is active.

## Diagnostic Requirement

The agent must find what is wrong using every relevant available tool for the issue class. For terrain/image-driven work this can include:

- source inspection
- script inspection
- manifest metrics
- generated preview images through `repo_image_view`
- generated numeric diagnostics
- exporter runs
- source-level metrics/log tokens
- git diff/stat inspection
- build/UBT where available
- CodeRabbit review

Image review alone is not sufficient. Image review is evidence, not the only diagnosis path.

## Iteration Contract

For every runtime file:

1. State the suspected cause.
2. State the scoped implementation target.
3. State the evidence that will prove the change worked.
4. Make the code/tool/repo changes.
5. Run the relevant generator/checks.
6. Inspect the generated results.
7. If the result is still wrong, write the next `SELF-ITERATIVE/<next>.md` runtime file based on the new evidence.
8. Continue until the issue is fixed or a real blocker is recorded.

## PR Contract

For each scoped runtime implementation PR:

1. Stage intended files only.
2. Run relevant checks.
3. Commit coherent changes.
4. Push a ready PR against `main`.
5. Include `@coderabbitai ignore` in the PR body when using the manual CodeRabbit loop.
6. Run CodeRabbit.
7. Fix only valid in-scope findings.
8. Rerun checks.
9. Merge only when the user has granted merge authority and gates are clean.
10. Sync local `main` after merge.
11. Update `.mex/context/active-memory.md` before moving to the next runtime file.

## Terrain Morphology Use Case

For the current Batch006 terrain morphology problem, this lifecycle means:

- do not rely only on the `Images/TerrainPreview/003/` visual failure;
- inspect terrain source and preview exporter math;
- identify why ridge/gully/catchment morphology remains radial;
- write `SELF-ITERATIVE/001_*` as the first scoped runtime file for diagnostic gates or source correction;
- implement it;
- regenerate preview evidence into the next numbered `Images/TerrainPreview/` folder without overwriting existing folders;
- inspect the generated images through `repo_image_view`;
- write the next runtime file from the evidence if more iteration is needed.

## Non-Negotiables

- Do not treat dirty repo state as junk by default.
- Do not delete or revert meaningful workflow/project state without explicit instruction.
- Do not call a lifecycle complete because one combined PR landed unless the runtime files and `.mex` state prove completion.
- Do not make docs-only PRs unless the user explicitly asks for docs-only work.
- Do not overwrite existing terrain preview evidence folders.
- Do not hide tool blockers; classify them as tool/read/validation blockers and continue with available safe tools where possible.
