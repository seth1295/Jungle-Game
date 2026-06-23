---
name: standard-pr-workflow
description: Use only after this project has git/remotes and the user explicitly authorizes an implementation PR lifecycle.
---

# Standard PR Workflow

## Preconditions

This skill is active only when the project is a git repository with an intended remote and the user explicitly authorizes an implementation PR lifecycle.

## Full Lifecycle Contract

When the user authorizes an implementation PR lifecycle:

1. Start from clean, updated `main`.
2. Create a scoped branch.
3. Implement only the approved scope.
4. Run relevant tests/checks.
5. Inspect `git diff --stat` and the full intended diff.
6. Stage intended files only.
7. Commit coherent changes.
8. Push branch.
9. Open a ready PR against `main`; do not create a draft PR unless requested.
10. Add `@coderabbitai ignore` anywhere in the PR body if using manual CodeRabbit review.
11. Run CodeRabbit only when the user asks for CodeRabbit or the workflow authorizes the review loop.
12. Fix valid in-scope findings only.
13. Rerun tests/checks after fixes.
14. Commit/push focused fixes.
15. Rerun review until clean or blocked.
16. Merge only when explicitly authorized.
17. After merge, sync local `main` and confirm clean status.

## Review-Only Mode

If the user asks only to review, inspect, or classify:

- Do not edit files.
- Do not commit.
- Do not push.
- Do not merge.
- Return findings and stop.

Switch from review-only to review-and-fix only when the user authorizes fixing, cleaning, shipping, or the full review loop.

## Output

Report:

- branch
- commit(s)
- PR URL, if any
- merge state
- tests/checks run
- review state
- findings fixed/skipped
- files changed
- final status
- blockers
- warnings
- next action
