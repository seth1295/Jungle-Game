---
name: project
description: Current project identity and boundaries.
---

# Project Context

## Identity

- Name: Jungle Game
- Local path: `/home/seth/projects/new-project`
- Remote: `https://github.com/seth1295/Jungle-Game.git`
- Current form: git repository with project workflow scaffold and design brief
- Primary branch: `main`

## Purpose

This folder exists as a clean DevSpace-ready project scaffold for a serious UE5/Linux-first 3D jungle survival-simulation game.

Current lead concept:

- first-person spatial 3D
- dense tropical jungle, not generic pine woodland
- approximate setting: 1000 CE
- normal physical/ecological world state
- nonviolent relict hominin presence that observes and tests the player from distance
- survival-simulation first, light social/anthropological horror second

It carries the same relevant workflow habits from PrivateToolBridge:

- explicit authority order
- routed `.mex/` memory
- DevSpace-safe file operations
- no repo-host actions unless authorized
- clear final status reports

## Boundaries

- Do not create commits, branches, PRs, pushes, repo-host actions, or merges unless explicitly asked.
- Do not assume PrivateToolBridge code belongs here.
- Do not copy secrets or local auth material into this project.
- DevSpace is only the implementation mechanism, not the subject of the game.

## DevSpace Root Requirement

The active DevSpace server should be started with:

```bash
DEVSPACE_ALLOWED_ROOTS="$HOME/projects"
```

This lets ChatGPT open `/home/seth/projects/new-project` and other sibling repos under the same parent directory.
