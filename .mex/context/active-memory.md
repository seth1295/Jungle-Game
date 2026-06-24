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

## 2026-06-23-pr-gate-workflow

status: active-unreviewed
created: 2026-06-23
source: user
target: .mex/context/pr-gates.md
reason: User asked to make planning gates into a PR-by-PR workflow where each completed PR points agents to the next documented gate.
review_by: 2026-07-23

Future PR work must load `.mex/context/pr-gates.md`, `docs/PR_WORKFLOW.md`, and the active `docs/gates/*.md` file before changing files. Each PR must identify its active gate, stay inside that gate's allowed scope, run the gate's validation, and stop if the requested work conflicts with the active gate. After a gate merges, read the next gate before starting more work.

## 2026-06-24-sequential-four-pr-handoffs

status: confirmed
created: 2026-06-24
source: user
target: docs/PR_WORKFLOW.md
reason: User clarified that future work should plan four PRs ahead but execute sequentially, and that fresh DevSpace chats need handoff files after each four-PR batch.
review_by: 2026-07-24

Plan up to four PRs ahead, but execute one PR at a time. Do not run normal UE implementation PRs in parallel. After completing the fourth PR in a sequential batch, create a tracked Markdown handoff under `handoffs/` for the next fresh DevSpace chat before starting the next batch. If work stops early because of a blocker, context reset, tool limit, or user pause, create an interim handoff under `handoffs/` and mark it incomplete.

## 2026-06-24-large-world-plan-active

status: confirmed
created: 2026-06-24
source: user
target: Docs/PRPlan/006_ImplementationPRSequence.md
reason: User approved moving from Cell 0 visibility/docs into the large-world implementation lane.
review_by: 2026-07-24

The active implementation source of truth is `Docs/PRPlan/006_ImplementationPRSequence.md`, supported by the large-world design docs under `Docs/PRPlan/`. Roadmap PR labels inside those docs are planning labels; actual GitHub PR numbers may differ. The next implementation target is the first roadmap implementation item: `16x16 World Partition landscape foundation`, preserving island-first world framing, ocean boundary, terrain-first traversal, and hard no-map/no-compass/no-clock navigation rules.
