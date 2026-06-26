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

The active implementation source of truth is `Docs/PRPlan/006_ImplementationPRSequence.md`, supported by the large-world design docs under `Docs/PRPlan/`. Roadmap PR labels inside those docs are planning labels; actual GitHub PR numbers may differ. Preserve island-first world framing, ocean boundary, terrain-first traversal, and hard no-map/no-compass/no-clock navigation rules across every roadmap PR.

## 2026-06-24-large-world-batch-1-complete

status: confirmed
created: 2026-06-24
source: user
target: handoffs/2026-06-24-large-world-batch-1-complete.md
reason: User clarified that `.mex` is live project memory and the handoff is only backup; record completed large-world batch before continuing PR #12.
review_by: 2026-07-24

Large-world batch 1 is complete and merged. Completed GitHub PRs: #10 roadmap PR #7 `16x16 World Partition landscape foundation`, #11 roadmap PR #8 `Landscape generation/import/tooling scaffold`, #12 roadmap PR #9 `Macro terrain island landform scaffold`, #13 roadmap PR #10 `Ecosystem mask system`, #14 roadmap PR #11 `Ecosystem debug visualisation and validation`, and #15 tracked handoff document. Completed branches were cleaned. Do not restart or redo this batch.

Historical note at the time of this entry: the next sequential implementation target was roadmap PR #12 `PCG biome framework`, branch `pr12-pcg-biome-framework`. That target is now complete and superseded by later entries below; do not use this paragraph as the current next action.

## 2026-06-25-per-pr-mex-update-gate

status: confirmed
created: 2026-06-25
source: user
target: AGENTS.md, docs/PR_WORKFLOW.md, .mex/context/pr-gates.md, .agents/skills/project-memory-maintenance/SKILL.md
reason: User explicitly required every completed PR to update `.mex` because stale active-memory caused fresh chats to boot from old PR #12 context.
review_by: 2026-07-25

Every completed implementation or validation PR must update `.mex/context/active-memory.md` before starting the next PR. This is a hard lifecycle gate, not optional cleanup. The update must record completed PR state, validation result, landed branch/commit where known, the next target, active blockers/pauses, and whether a four-PR handoff is due. If a PR lands through a direct cherry-pick, direct push, tool-blocked PR merge workaround, or other unusual path, `.mex` still must be updated before further implementation.

## 2026-06-25-large-world-pcg-complete

status: confirmed
created: 2026-06-25
source: repo-history
target: .mex/context/active-memory.md
reason: Catch up stale active memory after PCG biome framework completion.
review_by: 2026-07-25

Roadmap PR #12 `PCG biome framework` completed and landed as GitHub PR #16, commit `fd2441b`. Do not treat PCG biome framework as the next target anymore. Future work should continue from the active large-world implementation sequence and current project state, not from stale `pr12-pcg-biome-framework` instructions.

## 2026-06-25-visible-spawn-visual-capture-complete

status: confirmed
created: 2026-06-25
source: repo-history
target: .mex/context/active-memory.md
reason: Catch up active memory after visible playable spawn and packaged visual capture work landed.
review_by: 2026-07-25

Visible playable spawn/package validation work landed on `main` as commit `c0cd361` after PR #29 was closed. The game now has a source-authored packaged Cell 0 spawn slice, radial visibility diagnostics, and UE-native visual capture via `CAPTURE_VIEW=1` / `-JungleVisualSmoke`. The packaged visual capture workflow is now normal validation evidence for spawn/first-playable work. Do not rely only on logs when visual state matters; capture images and inspect them.

## 2026-06-25-codex-devspace-worker-model-blocked

status: active-unreviewed
created: 2026-06-25
source: user
target: docs/CODEX_SCOPED_WORKER.md
reason: User wants Codex used as a real scoped repo implementation worker, not a docs-only advisor, but ChatGPT-through-DevSpace execution policy is unresolved.
review_by: 2026-07-25

Codex should be treated as a capable scoped repo worker/code editor for implementation PRs. However, running Codex as a file-mutating subprocess through DevSpace `bash` conflicts with the current DevSpace rule that bash must not create or modify project files. Before using ChatGPT to invoke Codex directly for repo-writing work, resolve the compliant execution architecture. Do not keep handing the user manual Bash commands as the answer; determine whether a first-class Codex tool/connector or patch-only workflow can satisfy both the user goal and DevSpace tool restrictions.

## 2026-06-25-pr30-paused-environment-shell

status: active-unreviewed
created: 2026-06-25
source: user
target: Docs/PRPlan/006_ImplementationPRSequence.md
reason: User paused PR30 implementation to resolve Codex/DevSpace workflow and `.mex` maintenance first.
review_by: 2026-07-25

Current branch may be `pr30-cell0-environment-shell`; verify before editing. Intended next implementation remains Cell 0 environment shell plus multi-angle packaged visual capture: larger surrounding land mass, edge banks/ridges, trunk clusters, canopy/crown slabs, side/rear sightline blockers, creek/ridge framing, and multi-angle screenshot validation. Do not continue PR30 until the Codex/DevSpace worker model is resolved or the user explicitly tells the assistant to proceed without Codex.

## 2026-06-26-pr5-lifecycle-setup-batch-001

status: active-unreviewed
created: 2026-06-26
source: user
target: docs/PR5_LIFECYCLE.md, pr5lifecycle-precursor/001, pr5lifecycle-runtimefiles/001
reason: User defined PR5 lifecycle as a repeatable workflow where setup creates five independent researcher prompts, external researcher chats produce Markdown runtime files, and runtime files are consumed as five sequential PR inputs.
review_by: 2026-07-26

PR5 lifecycle is now a project workflow. `pr5lifecycle-setup` creates five standalone researcher prompt files under `pr5lifecycle-precursor/<batch>/`. The user sends those prompts to independent no-context researcher AIs. The returned Markdown research/spec files are placed under `pr5lifecycle-runtimefiles/<batch>/`. `pr5lifecycle-run` consumes runtime files 001 through 005 as five sequential implementation PRs with validation, `.mex` updates, CodeRabbit, merge, and main sync between each PR. Batch 001 setup prompts target the world-first terrain/biome lane. Batch 001 runtime files are present and staged in branch `pr5-setup-batch-001` with world-first terrain/biome source bridge scaffolding. Validation: `git diff --cached --check` passes after trimming generated Markdown trailing whitespace; UE compile validation is blocked in the current shell because both documented build paths `/mnt/ue5/UnrealEngine/.../Build.sh` and `/run/media/seth/UE5_WORKSPACE/UnrealEngine/.../Build.sh` are missing. Treat this as an environment path blocker, not a source compile failure.

## 2026-06-26-pr5-run-001-macro-terrain

status: active-unreviewed
created: 2026-06-26
source: pr5lifecycle-runtimefiles/001/001_macro_island_terrain_navigation_research.md
target: Source/JungleGame/JungleWorldTerrainBiomeRunway.h, Source/JungleGame/JungleWorldTerrainBiomeRunway.cpp
reason: PR5 runtime file 001 is being consumed as the first sequential implementation PR input.
review_by: 2026-07-26

PR5-run 001 implements the accepted macro island terrain research as source-level contracts: expanded terrain channel schema, landform taxonomy, traversal categories, terrain evidence types, channel contracts, landform traversal rules, and default developer-only terrain review points. Deferred items remain runtime learned terrain generation, InfiniteDiffusion/Terrain Diffusion integration, full erosion/hydrology simulation, final rainforest biome generation, and production PCG. No player-facing map, compass, GPS, objective marker, HUD clock, explicit time readout, or orientation confidence UI is introduced.
