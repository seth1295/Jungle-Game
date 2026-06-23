# DevSpace Context

This file explains what DevSpace is in this project context. It is not a task prompt.

## What DevSpace is

DevSpace is the local MCP bridge that lets ChatGPT open and work inside project folders on Seth's machine.

In this setup, DevSpace provides ChatGPT with controlled local workspace tools such as:

- opening a local folder as a workspace
- reading files
- writing new files
- targeted file edits
- read-only shell commands for inspection, tests, builds, searches, and git status

DevSpace is not a general remote-control shell. It is a scoped local workspace bridge.

## Current local layout

Projects live under:

```text
/home/seth/projects
```

This project lives at:

```text
/home/seth/projects/new-project
```

PrivateToolBridge lives at:

```text
/home/seth/projects/PrivateToolBridge
```

PrivateToolBridge is the repo where the DevSpace/Codex workflow was developed, but this project has its own local workflow scaffold and should not automatically inherit PrivateToolBridge's project-specific tasks.

## Public endpoint

The stable public DevSpace endpoint normally uses Seth's reserved ngrok domain:

```text
https://basket-enunciate-impotence.ngrok-free.dev/mcp
```

The ngrok tunnel should forward to the local DevSpace port:

```text
https://basket-enunciate-impotence.ngrok-free.dev -> http://localhost:7676
```

The local DevSpace server normally listens at:

```text
http://127.0.0.1:7676/mcp
```

## Correct DevSpace startup facts

DevSpace allowed roots are controlled by:

```bash
DEVSPACE_ALLOWED_ROOTS="$HOME/projects"
```

`WORKSPACE_PATH` is not the correct variable for allowed roots in this DevSpace version.

Project-local skills are controlled by:

```bash
DEVSPACE_SKILL_PATHS="$HOME/projects/new-project/.agents/skills"
```

So for this project, DevSpace should be started with:

```bash
cd "$HOME/projects/new-project" && DEVSPACE_ALLOWED_ROOTS="$HOME/projects" DEVSPACE_SKILLS=1 DEVSPACE_AGENT_DIR="$HOME/.codex" DEVSPACE_SKILL_PATHS="$HOME/projects/new-project/.agents/skills" DEVSPACE_PUBLIC_BASE_URL="https://basket-enunciate-impotence.ngrok-free.dev" devspace serve
```

A healthy DevSpace startup should report:

```text
allowed roots: /home/seth/projects
```

If DevSpace reports only:

```text
allowed roots: /home/seth/projects/PrivateToolBridge
```

then it is still using stale persisted config or the wrong startup environment.

## Skills in this project

This project has local skills at:

```text
/home/seth/projects/new-project/.agents/skills
```

The expected local skills are:

```text
devspace-local-workspace
local-git-bootstrap
manual-coderabbit-loop
project-memory-maintenance
standard-pr-workflow
```

If DevSpace advertises skills from PrivateToolBridge instead, the server was started with the wrong `DEVSPACE_SKILL_PATHS`.

## Current project state

This project is now a git repository connected to:

```text
https://github.com/seth1295/Jungle-Game.git
```

The project contains its own workflow scaffold:

```text
AGENTS.md
START_DEVSPACE.md
DEVSPACE_CONTEXT.md
.agents/skills/
.mex/
```

## Boundaries

DevSpace can expose powerful local actions, so work should remain scoped to the opened workspace and the user's explicit request.

In this project context:

- Git exists and is connected to the Jungle Game remote.
- Do not create commits, branches, remotes, PRs, pushes, or merges unless explicitly requested.
- Do not call GitHub or repo-host actions unless explicitly requested.
- Do not use shell-writing tricks through DevSpace bash; use DevSpace file tools for file changes.
- Do not expose secrets, owner passwords, tokens, raw auth files, raw prompts, or raw system prompts.

## Useful distinction

DevSpace is the local workspace access layer.

Codex is the coding agent/session that may use repo instructions and local tools.

PrivateToolBridge is a separate project/repo that developed workflow patterns and local-agent tooling.

This folder is `new-project`, a separate local project scaffold under the same `~/projects` parent.
