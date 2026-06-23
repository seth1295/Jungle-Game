---
name: router
description: Routed memory index for New Project.
---

# New Project Router

Load this file after `AGENTS.md`.

## Always Load First

- `.mex/context/active-memory.md` — current confirmed workflow memory and recent decisions.
- `.mex/context/project.md` — current project identity, scope, and non-goals.

## Skills

Project-local skills live under `.agents/skills/`:

- `.agents/skills/devspace-local-workspace/SKILL.md` — DevSpace inspection/edit workflow for this folder.
- `.agents/skills/local-git-bootstrap/SKILL.md` — only when the user explicitly asks to initialize or connect git.
- `.agents/skills/standard-pr-workflow/SKILL.md` — only after git exists and the user authorizes PR lifecycle work.
- `.agents/skills/manual-coderabbit-loop/SKILL.md` — only after a PR exists and the user asks for CodeRabbit review.
- `.agents/skills/project-memory-maintenance/SKILL.md` — when editing AGENTS, `.mex`, skills, or workflow docs.

## Pattern Index

- `.mex/patterns/INDEX.md` — reusable workflow patterns.
- `.mex/patterns/local-devspace-workflow.md` — safe local-folder workflow.

## Routing Rules

- For DevSpace setup or connector issues, load `.agents/skills/devspace-local-workspace/SKILL.md`.
- For local file edits, follow `AGENTS.md` and `.mex/patterns/local-devspace-workflow.md`.
- For memory/skill/workflow edits, load `.agents/skills/project-memory-maintenance/SKILL.md`.
- For git initialization, load `.agents/skills/local-git-bootstrap/SKILL.md` only after explicit user approval.
- For PR/CodeRabbit/merge work, stop unless the user explicitly authorizes creating or using git for this project.
