---
name: project-memory-maintenance
description: Use when updating AGENTS.md, .mex memory, or local workflow skills for this project.
---

# Project Memory Maintenance

## Scope

Use this skill for edits to:

- `AGENTS.md`
- `.mex/`
- `.agents/skills/`
- workflow documentation

## Rules

- Keep `AGENTS.md` concise and authoritative.
- Put evolving workflow detail in `.mex/` or `.agents/skills/`.
- Do not store secrets, tokens, owner passwords, raw prompts, raw context dumps, or raw model outputs.
- Do not copy PrivateToolBridge project-specific implementation details unless they are generic workflow rules needed here.
- Memory entries should have status, created date, source, target, reason, and review date.

## Validation

This project does not currently have a memory-check command. Validate by direct inspection:

1. Confirm referenced files exist.
2. Confirm router paths stay inside `.mex/` or local `.agents/skills/`.
3. Confirm active-memory entries do not contain secrets or raw auth material.
4. Confirm any git lifecycle action was explicitly authorized.

## Output

Report:

- files changed
- memory/routes updated
- validation performed
- blockers
- warnings
- next action
