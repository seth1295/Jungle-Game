---
name: devspace-local-workspace
description: Use when ChatGPT/DevSpace is operating on this local project folder for inspection, targeted edits, or environment diagnostics.
---

# DevSpace Local Workspace

## Scope

This skill applies to `/home/seth/projects/new-project`.

The folder is now a git repo connected to the Jungle Game remote. Do not commit, push, open PRs, use GitHub, or merge unless the user explicitly asks for that lifecycle.

## Required DevSpace Startup

DevSpace must allow the parent projects directory:

```bash
DEVSPACE_ALLOWED_ROOTS="$HOME/projects"
```

The active public endpoint is normally:

```text
https://basket-enunciate-impotence.ngrok-free.dev/mcp
```

## Workflow

1. Open `/home/seth/projects/new-project` with DevSpace `open_workspace`.
2. Read `AGENTS.md` and `.mex/ROUTER.md`.
3. Use `read` for direct file inspection.
4. Use `bash` only for read-only inspection, tests, builds, package scripts, search, file discovery, and directory inspection.
5. Use `edit` for targeted file changes.
6. Use `write` for new files or full rewrites.
7. Do not use bash redirection, heredocs, `tee`, `sed -i`, generated scripts, or inline language scripts to modify files.
8. Keep secrets local; do not print tokens, owner passwords, raw auth files, or raw prompts.
9. Inspect changed files before reporting completion.

## Classification Rules

- `Path is outside allowed roots` means DevSpace was not started with `DEVSPACE_ALLOWED_ROOTS="$HOME/projects"`.
- Missing `AGENTS.md` means the folder is not using this scaffold or the wrong workspace was opened.
- Any request to commit, push, open a PR, or merge crosses from local-folder workflow into a git lifecycle and needs explicit authorization.

## Output

Report:

- workspace path
- mode
- files changed
- checks run
- final status
- blockers
- warnings
- next action
