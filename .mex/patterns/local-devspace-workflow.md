---
name: local-devspace-workflow
description: Safe local DevSpace workflow for a non-git project folder.
---

# Local DevSpace Workflow

Use this pattern for local inspection and edits in `/home/seth/projects/new-project`.

## Steps

1. Verify the workspace path.
2. Read `AGENTS.md` and `.mex/ROUTER.md`.
3. Inspect relevant files with DevSpace `read` or read-only `bash` commands.
4. Make file changes with DevSpace `edit` or `write` only.
5. Do not use shell redirection, heredocs, `tee`, `sed -i`, or generated scripts to write files.
6. Run only relevant checks or smoke commands for the actual project contents.
7. Inspect changed file list and relevant content before reporting.
8. Do not initialize git, create commits, push, open PRs, or merge unless explicitly authorized.

## Output

Report:

- workspace path
- files changed
- checks run
- final status
- blockers
- warnings
- next action
