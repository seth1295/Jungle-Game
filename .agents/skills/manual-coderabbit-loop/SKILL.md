---
name: manual-coderabbit-loop
description: Use only when this project has a PR and the user explicitly asks for CodeRabbit review or a CodeRabbit review loop.
---

# Manual CodeRabbit Loop

## Preconditions

- Project is a git repository.
- A PR exists or the current branch is intended for a PR.
- CodeRabbit CLI is available.
- User explicitly asked for CodeRabbit review, CodeRabbit subagent review, or a review-and-fix loop.

## Terminology

`coderabbit review --agent` is CodeRabbit structured output mode. It is not a local subagent by itself.

If a future local-agent bridge is added to this project, keep the distinction explicit:

```text
CodeRabbit structured output
→ local/project agent triage
→ repo worker verifies findings
```

## Review-Only Mode

If the user asks only for review, do not edit files, commit, push, or merge. Report findings and stop.

## Review-And-Fix Mode

When the user authorizes fixing valid findings:

1. Verify branch and clean/dirty state.
2. Run CodeRabbit directly from PATH unless another command is explicitly required.
3. Inspect every finding against current code.
4. Fix valid in-scope findings only.
5. Skip invalid, stale, duplicate, speculative, or out-of-scope findings with a short reason.
6. Run relevant tests/checks after fixes.
7. Inspect the focused diff.
8. Commit and push focused fixes only when the workflow authorizes it.
9. Rerun CodeRabbit until clean or blocked.
10. Do not merge unless explicitly authorized.

## Output

Report:

- CodeRabbit run count
- findings fixed
- findings skipped with reason
- tests/checks run
- commit SHA(s), if any
- final CodeRabbit state
- final git status
- blockers
- warnings
- next action
