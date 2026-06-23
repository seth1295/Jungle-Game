---
name: local-git-bootstrap
description: Use only when the user explicitly asks to initialize or connect git for this project.
---

# Local Git Bootstrap

## Scope

This project is now a git repository connected to the Jungle Game remote.

Use this skill only after the user explicitly authorizes remote setup changes or repo-host work.

## Boundaries

- Do not recreate or replace `.git` without explicit user approval.
- Do not create a remote repository without explicit user approval.
- Do not push, open PRs, or merge unless explicitly authorized.
- Do not import PrivateToolBridge history or sensitive local material.

## Inspect First

Before any git bootstrap or remote-change work:

1. Confirm current path is `/home/seth/projects/new-project`.
2. Inspect current git state.
3. Inspect file list for generated junk, caches, sensitive files, or unrelated material.
4. Ask only questions that change repo shape: public/private, remote owner/name, license, branch name, and whether a commit/PR is desired.

## If User Authorizes Local Git Changes

1. Inspect current branch and remote state.
2. Add an appropriate `.gitignore` if needed.
3. Stage intended project files only.
4. Commit a coherent checkpoint only if authorized.
5. Do not create a remote or PR unless requested.

## If User Authorizes Remote/PR Lifecycle

Follow `.agents/skills/standard-pr-workflow/SKILL.md` after git exists and a remote is configured.

## Output

Report:

- path
- git state before/after
- branch
- files staged/committed, if any
- remote state, if any
- blockers
- warnings
- next action
