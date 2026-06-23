# New Project Agent Rules

These rules apply to ChatGPT, Codex, and any repo worker operating in this folder.

## Authority and Memory Model

Authority order:

1. Current user instruction
2. This `AGENTS.md`
3. Confirmed stable `.mex/context/*` and `.mex/patterns/*`
4. `confirmed` entries in `.mex/context/active-memory.md`
5. `active-unreviewed` entries in `.mex/context/active-memory.md`

If the current user instruction conflicts with this file or `.mex/`, stop and resolve the conflict before making changes.

## Routed Project Memory

After reading this file, read `.mex/ROUTER.md` for current project state, routed context, and reusable task patterns.

Load `.mex/context/active-memory.md` first, then only the additional `.mex/context/*`, `.mex/patterns/*`, or `.agents/skills/*/SKILL.md` files relevant to the current task.

Before any PR lifecycle, load `.mex/context/pr-gates.md`, `docs/PR_WORKFLOW.md`, and the active gate file listed in `.mex/context/pr-gates.md`.

## Current Project State

This folder is the local workspace for Jungle Game. Git has been initialized and connected to `https://github.com/seth1295/Jungle-Game.git`.

Do not commit, push, open PRs, call GitHub/repo-host tools, or merge anything unless the user explicitly asks for that lifecycle.

## DevSpace Startup

For this project to advertise its own local skills, restart DevSpace with:

```bash
cd "$HOME/projects/new-project" && DEVSPACE_ALLOWED_ROOTS="$HOME/projects" DEVSPACE_SKILLS=1 DEVSPACE_AGENT_DIR="$HOME/.codex" DEVSPACE_SKILL_PATHS="$HOME/projects/new-project/.agents/skills" DEVSPACE_PUBLIC_BASE_URL="https://basket-enunciate-impotence.ngrok-free.dev" devspace serve
```

`DEVSPACE_ALLOWED_ROOTS` controls what folders ChatGPT can open. `DEVSPACE_SKILL_PATHS` controls which project-local skills are advertised.

## DevSpace Workflow

- Use DevSpace `read` for direct file inspection.
- Use DevSpace `edit` for targeted changes to existing files.
- Use DevSpace `write` for new files or full rewrites.
- Use DevSpace `bash` only for read-only inspection, tests, builds, package scripts, and directory discovery.
- Do not use bash redirection, heredocs, `tee`, `sed -i`, generated scripts, or inline language scripts to write project files through DevSpace.
- Keep secrets local. Do not print API keys, tokens, owner passwords, raw prompts, raw context, raw system prompts, or raw auth files.
- Inspect changed files before reporting completion.

## Local Skills

Project-local skills live in `.agents/skills/`:

- `devspace-local-workspace`
- `local-git-bootstrap`
- `standard-pr-workflow`
- `manual-coderabbit-loop`
- `project-memory-maintenance`

Load the matching skill before work that falls under it.

## Git Lifecycle

Only after explicit user approval:

1. Branch from clean updated `main` before modifying files when normal PR workflow is possible.
2. Identify and follow the active PR gate.
3. Keep each commit coherent and scoped.
4. Open ready PRs only; no draft PRs unless requested.
5. Add `@coderabbitai ignore` to PR bodies if using a manual CodeRabbit loop.
6. Do not merge unless explicitly authorized.

## Final Summary

Every completed workflow should report:

- Folder/workspace path
- Mode: read-only, local-edit, or git lifecycle
- Files changed
- Checks/tests run
- Final status
- Blockers
- Warnings
- Next action
