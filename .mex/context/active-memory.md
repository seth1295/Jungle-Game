---
name: active-memory
description: Active workflow memory for New Project.
---

# Active Memory

## 2026-06-23-new-project-scaffold

status: confirmed
created: 2026-06-23
source: user
target: AGENTS.md
reason: User asked to create a simple new project folder under `~/projects` with relevant DevSpace workflow, without creating git.
review_by: 2026-07-23

`/home/seth/projects/new-project` began as a plain DevSpace-ready project folder. It should now be treated as the local workspace for Jungle Game. Git lifecycle actions remain explicit-request only.

## 2026-06-23-devspace-parent-root

status: confirmed
created: 2026-06-23
source: user
target: .agents/skills/devspace-local-workspace/SKILL.md
reason: DevSpace must open multiple project folders under `~/projects`.
review_by: 2026-07-23

DevSpace allowed roots are controlled by `DEVSPACE_ALLOWED_ROOTS`, not `WORKSPACE_PATH`. Start DevSpace with `DEVSPACE_ALLOWED_ROOTS="$HOME/projects"` so this folder and sibling repos can be opened from ChatGPT.

## 2026-06-23-jungle-game-git-remote

status: confirmed
created: 2026-06-23
source: user
target: .mex/context/project.md
reason: User provided GitHub remote and asked to connect it.
review_by: 2026-07-23

The local workspace is connected to `https://github.com/seth1295/Jungle-Game.git` as `origin`. The primary branch is `main`. Do not push, open PRs, run repo-host actions, or merge unless the user explicitly asks for that lifecycle.

## 2026-06-23-jungle-game-design-lane

status: active-unreviewed
created: 2026-06-23
source: user
target: README.md
reason: User selected the current game direction through discussion.
review_by: 2026-07-23

Current lead concept: a first-person UE5/Linux-first 3D survival-simulation set around 1000 CE in a dense tropical jungle. The world state should be physically/ecologically normal. The player gradually uncovers a quiet relict hominin presence that observes, tests, and changes the player's environment from distance. No 2D, no gore horror, no modern-current setting, no generic pine woodland.
