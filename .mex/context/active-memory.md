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

## 2026-06-26-pr5-lifecycle-tooling-checkpoint

status: active-unreviewed
created: 2026-06-26
source: user
target: docs/PR5_LIFECYCLE.md, .mex/context/active-memory.md
reason: PR5 lifecycle runtime work was paused to remove DevSpace raw bash command-shape blockers from PR lifecycle operations before continuing implementation.
review_by: 2026-07-26

PR5 lifecycle remains the active Jungle Game lane. PR5-run 001 macro terrain contracts have landed. Before the PTB/DevSpace tooling pause, PR5-run 002 biome mask/traversal work had been started but was not completed or PR-merged. Last known pre-pause state: Jungle Game may be on branch `pr5-run-002`; `Source/JungleGame/JungleWorldMaskData.h` had partial local edits expanding biome mask/traversal declarations; `Source/JungleGame/JungleWorldMaskData.cpp` was not confirmed present/complete; no `.mex` completion entry, commit, PR, CodeRabbit pass, or merge was confirmed for PR5-run 002. During the pause, PrivateToolBridge was patched toward typed repo lifecycle MCP operations, including a `repo_complete_pr` tool backed by `repo_lifecycle_tools.py`, so future PR5 lifecycle merges can avoid raw `gh pr merge`/merge-endpoint bash strings that ChatGPT sometimes blocks before MCP. DevSpace/PTB was relaunched after the patch. Next PR5 action is to verify the new typed tool is visible/usable, then inspect the Jungle Game branch/status/diff before resuming PR5-run 002. Do not assume PR5-run 002 is clean, complete, or still on the expected branch without inspection.

## 2026-06-26-pr5-run-002-biome-mask-traversal-contracts

status: confirmed
created: 2026-06-26
source: pr5lifecycle-runtimefiles/001/002_tropical_biome_masks_traversal_research.md
target: Source/JungleGame/JungleWorldMaskData.h, Source/JungleGame/JungleWorldMaskData.cpp
reason: PR5 runtime file 002 was consumed as the second sequential implementation PR input.
review_by: 2026-07-26

PR5-run 002 is GitHub PR #32 on branch `pr5-run-002`. Commits `57408c8` and `ac45cd4` implement source-level biome mask/traversal contracts: canonical normalized mask vocabulary, traversal classes/rules, developer-only debug view/report specs, traversal resistance composition, false-affordance classification, and explicit guard fields preventing debug/mask data from becoming player-facing navigation UI. Validation: `git diff --cached --check` passed for source commits; forbidden navigation UI keyword scan on the new source/header returned no matches; CodeRabbit light review initially found two valid contract issues and rerun after fixes returned no findings. UE compile/package validation remains blocked locally because documented Unreal Engine Build.sh/UnrealEditor paths are not present under `/run/media/seth` or `/mnt/ue5`. Next PR5 target after PR #32 merges and local `main` syncs is runtime file 003 creek/riparian/wet-valley ecosystem contracts. Four-PR handoff is not due yet; PR5-run 002 is position 2/5 in batch 001 runtime consumption. PR #32 landed on `main` as `8939503`, and local `main` was synced clean after merge.

## 2026-06-26-pr5-run-003-riparian-wet-valley-contracts

status: active-unreviewed
created: 2026-06-26
source: pr5lifecycle-runtimefiles/001/003_creek_riparian_wet_valley_research.md
target: Source/JungleGame/JungleCreekRiparianWetValleyEcosystemSpec.h
reason: PR5 runtime file 003 was consumed as the third sequential implementation PR input.
review_by: 2026-07-26

PR5-run 003 is GitHub PR #33 on branch `pr5-run-003`, commit `1bef01f`. It expands the riparian/wet-valley source contract with reach metadata, crossing candidates, rain-state response, sound anchors, route evidence, debug views, and bank/crossing classification helpers. Validation: `git diff --check` passed; the new source/header scan returned no forbidden navigation drift matches; the mounted UE Build.sh was invoked but exceeded the DevSpace 300s timeout without returning a compiler error; local CodeRabbit CLI hit its review quota, but GitHub PR checks showed CodeRabbit passing. Next PR5 target after PR #33 merges and local `main` syncs is runtime file 004 ridge/slope/canopy orientation contracts. Four-PR handoff is not due yet; PR5-run 003 is position 3/5 in batch 001 runtime consumption.

## 2026-06-26-pr5-run-004-ridge-slope-canopy-contracts

status: active-unreviewed
created: 2026-06-26
source: pr5lifecycle-runtimefiles/001/004_ridge_slope_canopy_orientation_research.md
target: Source/JungleGame/JungleRidgeSlopeMountainEcosystemSpec.h
reason: PR5 runtime file 004 was consumed as the fourth sequential implementation PR input.
review_by: 2026-07-26

PR5-run 004 is GitHub PR #34 on branch `pr5-run-004`, commit `f00d0e2`. It expands ridge/slope/canopy source contracts with orientation cue categories, terrain forms, canopy states, cue degradation, weather/solar/night states, profile-shaped data contracts, developer validation points, debug view names, mask naming, cue-floor validation, and first validation point lists. Validation: `git diff --check` passed; the new source/header scan returned no forbidden navigation drift matches; mounted UE Build.sh was invoked but exceeded the DevSpace 300s timeout without returning a compiler error; GitHub PR checks showed CodeRabbit passing. Next PR5 target after PR #34 merges and local `main` syncs is runtime file 005 world validation evidence contracts. Four-PR handoff is due after PR5-run 004 because this completes positions 1/4 through 4/4 of the current sequential runtime batch, but the user explicitly requested continuing through 005, so create/update handoff after PR5-run 005 lands.

## 2026-06-26-pr5-run-005-validation-contracts

status: active-unreviewed
created: 2026-06-26
source: pr5lifecycle-runtimefiles/001/005_world_validation_evidence_research.md
target: Source/JungleGame/JungleWorldValidationEvidenceSpec.h, handoffs/2026-06-26-pr5-batch-001-runtime-001-005.md
reason: PR5 runtime file 005 was consumed as the fifth sequential implementation PR input and batch handoff point.
review_by: 2026-07-26

PR5-run 005 is GitHub PR #35 on branch `pr5-run-005`, source commit `e815414`. It adds developer-only world validation evidence contracts, package smoke evidence fields, repo hygiene evidence fields, artifact references, screenshot specs, debug view names, and local artifact ignore guidance. Validation before PR: `git diff --check` and `git diff --cached --check` passed; mounted UE Build.sh was invoked but exceeded the DevSpace 300s timeout without returning a compiler error. GitHub allowed PR #35 to merge and local `main` was synced after merge. PR #35 landed on `main` as `32b9b99`. Batch handoff file is tracked for fresh-chat recovery.

## 2026-06-26-pr5-batch002-001-world-authority

status: confirmed
created: 2026-06-26
source: pr5lifecycle-runtimefiles/002/001_full_size_world_authority_research.md
target: Source/JungleGame/JungleFullSizeWorldAuthoritySpec.h, Docs/World/FullSizeWorldAuthority.md
reason: PR5 runtime batch 002 file 001 is being consumed as the first sequential implementation PR input.
review_by: 2026-07-26

PR5 Batch 002 / 001 establishes the first full-size world authority shell. The project-owned target world path is defined as `/Game/JungleGame/Maps/WLD_JungleIsland_Full`; existing template OpenWorld use is documented as a temporary bridge only because no project-owned map asset is present yet. Cell 0 remains a review anchor only. Validation: `git diff --check` and `git diff --cached --check` passed. PR #36 landed on `main` as `9376897`. Next target is runtime file 002 deterministic full-size terrain source.

## 2026-06-26-pr5-batch002-002-deterministic-terrain-source

status: confirmed
created: 2026-06-26
source: pr5lifecycle-runtimefiles/002/002_deterministic_full_size_terrain_source_research.md
target: Docs/World/FullSizeTerrainSource.md
reason: PR5 runtime batch 002 file 002 is being consumed as the second sequential implementation PR input.
review_by: 2026-07-26

PR5 Batch 002 / 002 defines the deterministic full-size terrain source contract: 8129 by 8129 vertices, 2 m source spacing, 16.256 km square domain, fixed config identity, required derived channels, developer-only review point intents, and generated artifact policy. Full generated rasters remain ignored by default. Validation: `git diff --cached --check` passed. PR #37 landed on `main` as `294a88a`. Next target is runtime file 003 full-size terrain in-game shell.

## 2026-06-26-pr5-batch002-003-terrain-ingame-shell

status: confirmed
created: 2026-06-26
source: pr5lifecycle-runtimefiles/002/003_full_size_terrain_ingame_shell_research.md
target: Source/JungleGame/JungleFullSizeTerrainShellActor.h, Source/JungleGame/JungleFullSizeTerrainShellActor.cpp, Source/JungleGame/JungleGameMode.cpp, Docs/World/FullSizeTerrainShell.md
reason: PR5 runtime batch 002 file 003 is being consumed as the third sequential implementation PR input.
review_by: 2026-07-26

PR5 Batch 002 / 003 adds a source-authored runtime terrain shell actor and spawns it from `JungleGameMode` while no project-owned `.umap` asset exists. The shell provides rough full-size world blockout structure for basin/lowland, creek valley, ridge spine, mountain shoulder, creek mouth/coast, and ocean edge placeholders. Validation: `git diff --cached --check` passed; UE Build.sh was invoked but exceeded the DevSpace 300s timeout without returning a compiler error. PR #38 was opened; merge endpoint was filtered, so branch head was fast-forward pushed to `main` as `4f317cc`. Next target is runtime file 004 full-size mask/biome debug state.

## 2026-06-26-pr5-batch002-004-mask-biome-debug-state

status: confirmed
created: 2026-06-26
source: pr5lifecycle-runtimefiles/002/004_full_size_mask_biome_debug_state_research.md
target: Source/JungleGame/JungleFullSizeMaskBiomeDebugSpec.h, Docs/World/FullSizeMaskBiomeDebugState.md
reason: PR5 runtime batch 002 file 004 is being consumed as the fourth sequential implementation PR input.
review_by: 2026-07-26

PR5 Batch 002 / 004 defines developer-only full-size mask, biome, and traversal debug state. It adds required mask names, initial biome classes, traversal classes, and review sample fields while preserving the no player-facing navigation UI rule. Validation: `git diff --cached --check` passed. PR #39 was opened; merge endpoint was filtered, so branch head was fast-forward pushed to `main` as `f1cd5f7`. Next target is runtime file 005 packaged full-size world smoke evidence.

## 2026-06-26-pr5-batch002-005-packaged-smoke-evidence

status: confirmed
created: 2026-06-26
source: pr5lifecycle-runtimefiles/002/005_packaged_full_size_world_smoke_evidence_research.md
target: Source/JungleGame/JunglePackagedFullSizeWorldSmokeSpec.h, Docs/World/PackagedFullSizeWorldSmoke.md, handoffs/2026-06-26-pr5-batch-002-runtime-001-005.md
reason: PR5 runtime batch 002 file 005 is being consumed as the fifth sequential implementation PR input and batch handoff point.
review_by: 2026-07-26

PR5 Batch 002 / 005 defines packaged full-size world smoke evidence, required developer-only review points, runtime log token expectations, artifact hygiene, and the batch handoff. It records that a fresh packaged build including the runtime shell is pending because UE Build.sh previously exceeded the DevSpace 300s timeout without returning a compiler error. PR #40 landed on `main` as `dcdf53e`. PR5 Batch 002 is complete with handoff `handoffs/2026-06-26-pr5-batch-002-runtime-001-005.md`. Current planning decision: PR5 Batch 003 should not jump straight to traversal/ecosystem readability on cube slabs. First target should be a deterministic procedural full-size terrain substrate / terrain shell v2 that replaces or augments the cube blockout with a coarse continuous walkable terrain mesh or heightfield while preserving fixed seed/config identity, developer-only review points, no player-facing navigation UI, and cube shell fallback/debug value. After that substrate exists, continue into creek/riparian traversal, ridge/slope readability, biome transitions, diegetic navigation cues, and day/night/weather traversal pressure.

## 2026-06-26-pr5-batch003-001-terrain-shell-v2

status: confirmed
created: 2026-06-26
source: user, HighResShot validation of cube-block Batch 002 shell
target: Source/JungleGame/JungleFullSizeTerrainShellActor.h, Source/JungleGame/JungleFullSizeTerrainShellActor.cpp, Source/JungleGame/JungleGame.Build.cs, JungleGame.uproject, Docs/World/FullSizeTerrainShellV2.md
reason: Batch 002 runtime screenshot proved the full-size shell exists but is still visually cube-block geometry, so Batch 003 must start with a deterministic terrain substrate before traversal/ecosystem readability.
review_by: 2026-07-26

PR5 Batch 003 / 001 is GitHub PR #41 on branch `pr5-batch003-001-terrain-shell-v2`. It converts `AJungleFullSizeTerrainShellActor` from cube-only blockout to a deterministic procedural mesh heightfield with basin, creek-valley, ridge-spine, mountain-shoulder, coast, and ocean-edge shaping; retains cube blockout as explicit debug fallback via `-JungleDebugCubeShell`; adds the `ProceduralMeshComponent` module/plugin; and adds `-JungleTerrainShellSmoke` so HighResShot can validate the terrain substrate instead of the Cell 0 cube slice. Validation: `git diff --check main..HEAD` passed; GitHub reported the PR merge state as clean and no checks were configured; UBT `JungleGameEditor Linux Development` had previously succeeded for this branch, but current Build.sh path discovery did not find documented local Unreal paths; forbidden navigation UI scan produced only existing deny-rule documentation matches. Manual CodeRabbit CLI probing was blocked before execution in the current ChatGPT/DevSpace path, so review state is recorded as tooling-blocked rather than source-blocked. PR #41 landed on `main` as `f5c9321`. Next target is PR5 Batch 003 / 002: 1400 m volcanic massif heightfield.

## 2026-06-26-pr5-batch003-002-volcanic-massif

status: confirmed
created: 2026-06-26
source: pr5lifecycle-runtimefiles/003/002_1400m_volcanic_massif_heightfield_research.md
target: Source/JungleGame/JungleVolcanicIslandTerrainModel.h, Source/JungleGame/JungleVolcanicIslandTerrainModel.cpp, Source/JungleGame/JungleFullSizeTerrainShellActor.h, Source/JungleGame/JungleFullSizeTerrainShellActor.cpp, Docs/World/VolcanicMassifHeightfield.md
reason: PR5 Batch 003 requires the full-size terrain shell to become a deterministic volcanic island heightfield with a 1400 m-class massif rather than actor-local low-relief terrain math.
review_by: 2026-07-26

PR5 Batch 003 / 002 was GitHub PR #42 on branch `pr5-batch003-002-volcanic-massif`. It added `FJungleVolcanicIslandTerrainModel` as the source-owned terrain authority for world size, sea level, organic island radius, ocean/shelf/beach/coastal-lowland masks, coastal protection, and a 1400 m target volcanic massif. `AJungleFullSizeTerrainShellActor` now samples this model and logs `JG_VOLCANIC_TERRAIN_002` metrics for sampled min/max elevation, island radius, ocean margin, and square-edge ocean height. Validation: `git diff --check` and `git diff --cached --check` passed; forbidden navigation UI scan on changed source/docs returned no implementation drift; documented local Unreal Build.sh paths were missing, so UBT validation was environment-blocked rather than source-failed; GitHub CodeRabbit checks passed. PR #42 landed on `main` as `cba7a14`. Next target is PR5 Batch 003 / 003: radial ridges, catchments, gullies, lahar channels, and coastal outlets.

## 2026-06-26-pr5-batch003-003-ridges-catchments

status: confirmed
created: 2026-06-26
source: pr5lifecycle-runtimefiles/003/003_radial_ridges_catchments_lahar_gullies_research.md
target: Source/JungleGame/JungleVolcanicIslandTerrainModel.h, Source/JungleGame/JungleVolcanicIslandTerrainModel.cpp, Docs/World/RadialRidgesCatchmentsGullies.md
reason: PR5 Batch 003 requires radial volcanic drainage structure where ridges define catchment divides, gullies incise catchment centers, lahar-capable corridors follow selected valleys, and outlet/fan terrain resolves toward the sea-level coast.
review_by: 2026-07-26

PR5 Batch 003 / 003 was GitHub PR #43 on branch `pr5-batch003-003-ridges-catchments`. It extended `FJungleVolcanicIslandTerrainModel` with 14 primary catchments, catchment IDs, ridge masks, gully masks, selected lahar corridor masks, coastal fan masks, ridge height contribution, gully incision, fan deposition, and `JG_VOLCANIC_TERRAIN_003` metrics. Validation: `git diff --check` and `git diff --cached --check` passed; forbidden navigation UI scan produced only existing deny-rule documentation matches; documented local Unreal Build.sh paths were missing, so UBT validation was environment-blocked rather than source-failed; GitHub CodeRabbit checks passed. PR #43 landed on `main` as `a5de98b`. Next target is PR5 Batch 003 / 004: active crater, lava crust, terrain hazard masks, and hard-blocker terrain geometry.

## 2026-06-26-pr5-batch003-004-crater-lava-hazards

status: confirmed
created: 2026-06-26
source: pr5lifecycle-runtimefiles/003/004_active_crater_lava_crust_terrain_hazards_research.md
target: Source/JungleGame/JungleVolcanicIslandTerrainModel.h, Source/JungleGame/JungleVolcanicIslandTerrainModel.cpp, Docs/World/ActiveCraterLavaCrustTerrainHazards.md
reason: PR5 Batch 003 requires terrain-only active volcano structure before any later VFX/gameplay systems: broken summit crater, nested vent, breach, lava-crust masks, unstable-crust masks, and hard-blocker terrain evidence.
review_by: 2026-07-26

PR5 Batch 003 / 004 was GitHub PR #44 on branch `pr5-batch003-004-crater-lava-hazards`. It extended `FJungleVolcanicIslandTerrainModel` with crater interior, broken rim, nested vent, breach, fissure, lava-crust, unstable-crust, and hard-blocker masks plus crater depression/rim-raise terrain contributions. Runtime metrics advanced to `JG_VOLCANIC_TERRAIN_004` with crater, vent, and hard-blocker mask maxima. Validation: `git diff --check` and `git diff --cached --check` passed; forbidden navigation UI scan produced only existing deny-rule documentation matches; documented local Unreal Build.sh paths were missing, so UBT validation was environment-blocked rather than source-failed; GitHub CodeRabbit checks passed. PR #44 landed on `main` as `c3a142b`. Next target is PR5 Batch 003 / 005: sea-level beach ring and coastal shelf hardening plus batch handoff.

## 2026-06-26-pr5-batch003-005-sea-level-coast

status: confirmed
created: 2026-06-26
source: pr5lifecycle-runtimefiles/003/005_sea_level_beach_ring_coastal_shelf_research.md
target: Source/JungleGame/JungleVolcanicIslandTerrainModel.h, Source/JungleGame/JungleVolcanicIslandTerrainModel.cpp, Docs/World/SeaLevelBeachRingCoastalShelf.md, handoffs/2026-06-26-pr5-batch-003-runtime-001-005.md
reason: PR5 Batch 003 requires the island perimeter to resolve through a sea-level beach/coastal-shelf constraint, with square world edges remaining ocean/container terrain, before the batch stop point.
review_by: 2026-07-26

PR5 Batch 003 / 005 was GitHub PR #45 on branch `pr5-batch003-005-sea-level-coast`. It added shoreline constraint, shoreline error, beach width, beach continuity, and square-edge ocean violation metrics to `FJungleVolcanicIslandTerrainModel`; runtime metrics advanced to `JG_VOLCANIC_TERRAIN_005`; and the batch handoff `handoffs/2026-06-26-pr5-batch-003-runtime-001-005.md` records the stop point. Validation: `git diff --check` and `git diff --cached --check` passed; forbidden navigation UI scan produced only existing deny-rule documentation matches; documented local Unreal Build.sh paths were missing, so UBT validation was environment-blocked rather than source-failed; GitHub CodeRabbit checks passed. PR #45 landed on `main` as `1171fee`. PR5 Batch 003 is stopped at runtime file 005 by user instruction. Do not start runtime file 006 unless explicitly requested.

## 2026-06-27-pr5lifecycle-repo-files-correction

status: confirmed
created: 2026-06-27
source: user correction during PR5 Batch 003 recovery
target: pr5lifecycle-precursor/**, pr5lifecycle-runtimefiles/**
reason: PR5 lifecycle precursor and runtime research files are repo-owned lifecycle state, not disposable local scratch; future workers must not advise or perform untracking, cleaning, deletion, or stash workflows that hide/remove these files from the working tree without explicit lifecycle-file handling.
review_by: 2026-07-27

Correction: `pr5lifecycle-precursor/**` and `pr5lifecycle-runtimefiles/**` are project repository files and must remain part of PR5 lifecycle state. Batch 003 files under `pr5lifecycle-precursor/003/` and `pr5lifecycle-runtimefiles/003/` were recovered after a stash workflow moved untracked lifecycle files out of the working tree. Treat these lifecycle folders as implementation inputs and repo state that should be tracked/restored deliberately, not ignored as temporary docs. Before any future PR5 implementation, confirm the relevant lifecycle files exist and are not accidentally omitted from the intended PR/state commit.

## 2026-06-27-pr5-batch003-006-runtime-mesh-resolution

status: confirmed
created: 2026-06-27
source: pr5lifecycle-runtimefiles/003/006_runtime_mesh_resolution_lod_streaming_research.md
target: Source/JungleGame/JungleVolcanicIslandTerrainModel.h, Source/JungleGame/JungleVolcanicIslandTerrainModel.cpp, Source/JungleGame/JungleFullSizeTerrainShellActor.h, Source/JungleGame/JungleFullSizeTerrainShellActor.cpp, pr5lifecycle-precursor/003/**, pr5lifecycle-runtimefiles/003/**
reason: PR5 Batch 003 runtime file 006 continues from the landed runtime 005 coast state into a canonical-source tiled runtime mesh bridge, LOD/collision descriptors, source-reference resolution identity, seam/shoreline metrics, and tracked lifecycle-file restoration.
review_by: 2026-07-27

PR5 Batch 003 / 006 is the runtime mesh resolution, LOD, and terrain scale bridge. It records Batch 003 lifecycle precursor/runtime files as repo state, adds source-reference resolution constants for the 8129 x 8129 / 2 m full-island target, adds runtime tile descriptors for a 32 x 32 tile grid with near/mid/far vertex density, converts the terrain shell to build deterministic validation mesh sections from the canonical volcanic terrain source, enables collision only for near validation tiles, and logs `JG_TERRAIN_RUNTIME_006` metrics for tile counts, vertices, triangles, collision tiles, LOD ring counts, spacing, seam samples, and shoreline error. Validation so far: source diff whitespace passed; full cached diff check is blocked by restored historical lifecycle Markdown trailing whitespace that predates this code path and should be normalized separately or explicitly accepted when tracking those repo files. Next target after landing is PR5 Batch 003 / 007: terrain masks, channels, and material/debug state.

## 2026-06-27-pr5-batch003-007-terrain-mask-channels

status: confirmed
created: 2026-06-27
source: pr5lifecycle-runtimefiles/003/007_terrain_masks_channels_material_debug_research.md
target: Source/JungleGame/JungleVolcanicIslandTerrainModel.h, Source/JungleGame/JungleVolcanicIslandTerrainModel.cpp, Source/JungleGame/JungleFullSizeTerrainShellActor.cpp
reason: PR5 Batch 003 runtime file 007 requires the canonical volcanic terrain source to expose terrain-derived channel data for developer-only material/debug validation without creating player-facing navigation UI.
review_by: 2026-07-27

PR5 Batch 003 / 007 adds terrain channel sample and metrics structures, debug channel identity, slope/relief finite-difference derivation, packed coast/landform/hazard channel values, discrete elevation/slope/relief classes, and runtime log token `JG_TERRAIN_CHANNELS_007`. The terrain shell logs channel metrics alongside runtime tile metrics so reviewer-visible terrain masks include beach, ocean, shelf, massif, ridge, gully, catchment, crater, lava/crust, unstable crust, hard blocker, slope, and relief state. Validation expectation: `git diff --check` for changed source/.mex should pass; UBT remains environment-dependent on local Unreal path visibility. Next target after landing is PR5 Batch 003 / 008: heightfield export and topographic comparison evidence.

## 2026-06-27-pr5-batch003-008-topographic-export-evidence

status: confirmed
created: 2026-06-27
source: pr5lifecycle-runtimefiles/003/008_heightfield_export_topographic_comparison_research.md
target: Source/JungleGame/JungleVolcanicIslandTerrainModel.h, Source/JungleGame/JungleVolcanicIslandTerrainModel.cpp, Source/JungleGame/JungleFullSizeTerrainShellActor.cpp
reason: PR5 Batch 003 runtime file 008 requires deterministic heightfield/topographic comparison evidence derived from the same canonical terrain source used by runtime tiles and channel masks.
review_by: 2026-07-27

PR5 Batch 003 / 008 adds topographic export/evidence metrics without writing heavyweight generated artifacts into the repo. It defines export-grid identity, required/generated map schema counts, height/slope/relief ranges, 50 m contour band coverage, shoreline contour samples, sea-level shoreline error, beach continuity percentage, ocean-below-sea percentage, ridge/gully/crater/hazard mask maxima, and slope histograms. Runtime logs now include `JG_TOPO_EXPORT_008` alongside terrain/channel metrics, making the canonical source measurable for future PNG/CSV/heightmap export commandlets while preserving artifact hygiene. Validation expectation: changed source/.mex diff checks should pass; UBT remains environment-dependent on local Unreal path visibility. Next target after landing is PR5 Batch 003 / 009: large terrain generator architecture.

## 2026-06-27-pr5-batch003-009-generator-architecture

status: confirmed
created: 2026-06-27
source: pr5lifecycle-runtimefiles/003/009_large_terrain_generator_architecture_research.md
target: Source/JungleGame/JungleVolcanicIslandTerrainModel.h, Source/JungleGame/JungleVolcanicIslandTerrainModel.cpp, Source/JungleGame/JungleFullSizeTerrainShellActor.cpp
reason: PR5 Batch 003 runtime file 009 requires the terrain generator to expose explicit config/version/seed/identity and architecture validation rather than relying on implicit constants scattered through the source.
review_by: 2026-07-27

PR5 Batch 003 / 009 adds generator config identity, version ID `JG_TERRAIN_GENERATOR_009`, deterministic seed, source resolution identity, runtime tile identity, channel count, architecture switches, a stable fingerprint, and architecture validation metrics. Runtime logs now include world-size, sea-level, source-resolution, coast-source-ownership, runtime mesh bridge, topographic evidence, and overall architecture validity via `JG_TERRAIN_GENERATOR_009`. This keeps terrain generation source-owned and deterministic while avoiding Blueprint-only or mesh-first generator drift. Validation expectation: changed source/.mex diff checks should pass; UBT remains environment-dependent on local Unreal path visibility. Next target after landing is PR5 Batch 003 / 010: full terrain batch acceptance and handoff.

## 2026-06-27-pr5-batch003-010-acceptance-handoff

status: confirmed
created: 2026-06-27
source: pr5lifecycle-runtimefiles/003/010_full_terrain_batch_acceptance_and_handoff_research.md
target: Source/JungleGame/JungleVolcanicIslandTerrainModel.h, Source/JungleGame/JungleVolcanicIslandTerrainModel.cpp, Source/JungleGame/JungleFullSizeTerrainShellActor.cpp, handoffs/2026-06-27-pr5-batch-003-runtime-006-010.md
reason: PR5 Batch 003 runtime file 010 requires final terrain-batch acceptance metrics and a tracked fresh-chat handoff before later systems resume.
review_by: 2026-07-27

PR5 Batch 003 / 010 adds aggregate terrain batch acceptance metrics and tracked handoff `handoffs/2026-06-27-pr5-batch-003-runtime-006-010.md`. Runtime acceptance aggregates terrain height/coast metrics, runtime mesh seam metrics, channel metrics, topographic evidence metrics, and generator architecture metrics into `JG_TERRAIN_BATCH003_ACCEPTANCE_010`, including pass/fail booleans for peak, sea-level, beach, ocean, runtime mesh, channels, topographic evidence, architecture, and overall batch acceptance. Validation expectation: changed source/.mex/handoff diff checks should pass; UBT remains environment-dependent on local Unreal path visibility. After landing, PR5 Batch 003 runtime files 001-010 are complete and later PR5 work should resume from the tracked handoff plus local Unreal build/smoke validation when engine access is available.

## 2026-06-27-pr5-batch003-shoreline-acceptance-fix

status: confirmed
created: 2026-06-27
source: derived topography preview after PR5 Batch 003 / 010
target: Source/JungleGame/JungleVolcanicIslandTerrainModel.cpp
reason: Preview topography derivation proved terrain output was derivable but failed shore acceptance because post-process coastline locking was too narrow and beach-band samples still received terrain-process uplift.
review_by: 2026-07-27

This fix keeps the accepted 1743.77 m volcanic peak while hardening final coastal acceptance after all massif, ridge, gully, crater, and long-wave terrain contributions. It adds a stronger final shoreline hard-lock inside the measured shoreline-error envelope and clamps only the landward beach acceptance band to -0.25 m through +9.75 m. Lightweight preview derivation after the fix reported: height max 1743.77 m, shoreline error max 0.0000 m, beach continuity 100.0%, ocean below sea 100.0%, and square-edge ocean violations 0/512. UBT remains deferred until local Unreal build access is available.

## 2026-06-27-pr5-terrain-preview-image-exporter

status: confirmed
created: 2026-06-27
source: user request for a tool-rendered island image, not AI-generated imagery
target: scripts/terrain-preview-export.py, Images/TerrainPreview/**
reason: The project needs a deterministic, labelled preview image exporter that renders the PR5 Batch 003 island from sampled terrain data rather than prompt-generated concept art, without relying on deleted broad docs folders.
review_by: 2026-07-27

PR5 terrain preview tool was GitHub PR #52 on branch `pr5-terrain-preview-tool`. It added `scripts/terrain-preview-export.py`, a dependency-free Python PNG exporter that mirrors the PR5 Batch 003 terrain math and writes labelled, tracked terrain evidence under `Images/TerrainPreview/`. Default preview resolution is 1024 px so the primary island preview is useful without UE rendering or full 8129 x 8129 export. Generated files include color relief, grayscale height, grayscale slope, RGB mask atlas, manifest JSON, and README. The 1024 px generated manifest reported height max 1750.4691 m, shoreline error max 0.0 m, beach continuity 100.0%, ocean below sea 100.0%, and square-edge ocean violations 0/4092. These files are intended as small tracked terrain evidence, not ignored scratch, and must not be buried under deleted `Docs/World` structure. PR #52 landed on `main` via merge commit `6da06c8`; branch source commit was `17b65dd`. Full 8129 x 8129 export remains a later heavyweight tool path.

## 2026-06-27-docs-folders-intentional-removal

status: confirmed
created: 2026-06-27
source: user correction after terrain preview review
target: docs/**, Docs/**, Images/TerrainPreview/**
reason: User intentionally deleted broad `docs/` and `Docs/` folders because they were artificial planning/research spillover, not required project source. The only valuable output from that deleted structure was the terrain map generator output.
review_by: 2026-07-27

Treat deletion of broad `docs/**` and `Docs/**` folders as intentional cleanup, not repo damage. Do not restore those folders unless explicitly requested. Preserve PR5 lifecycle state under `pr5lifecycle-precursor/**` and `pr5lifecycle-runtimefiles/**`. Terrain preview evidence belongs under `Images/TerrainPreview/**`, not under deleted `Docs/World/TerrainPreview/**`. Future workflow references must avoid dead `docs/` / `Docs/` paths and use `.mex`, `pr5lifecycle-*`, `handoffs`, `Source`, `scripts`, and `Images` as appropriate.

## 2026-06-27-pr5-batch003-complete-current-state

status: confirmed
created: 2026-06-27
source: user-provided merged PR history and local git log
target: .mex/context/active-memory.md, pr5lifecycle-precursor/004/**
reason: `.mex` was stale and still represented PR5 Batch 003 006-010, shoreline fix, and terrain preview exporter as active-unreviewed despite PRs #46-#52 being merged.
review_by: 2026-07-27

PR5 Batch 003 is complete through GitHub PR #52. Merged history: #40 packaged smoke evidence, #41 terrain shell v2, #42 volcanic massif heightfield, #43 ridges/catchments/gullies, #44 crater/lava terrain hazards, #45 sea-level coast handoff, #46 runtime mesh resolution, #47 terrain mask channels, #48 topographic evidence, #49 generator architecture, #50 terrain acceptance handoff, #51 shoreline acceptance fix, and #52 terrain preview tool. Current terrain preview evidence proves coast acceptance is clean, but visual review shows the interior ridge/gully system still reads as procedural radial gear/fan blades. Next lane is PR5 Batch 004: a three-PR lifecycle focused on 6x world/map scale and fixing the image-proven radial terrain morphology while preserving the working coast/beach/shelf acceptance.

## 2026-06-27-pr5-batch004-precursor-setup

status: active-unreviewed
created: 2026-06-27
source: user request after Batch 003 terrain preview review
target: pr5lifecycle-precursor/004/**
reason: User requested a three-PR PR5 lifecycle precursor batch for the map-size increase and terrain defects shown by the generated preview images.
review_by: 2026-07-27

PR5 Batch 004 precursor setup contains three standalone researcher prompts: `001_x6_world_scale_terrain_authority_prompt.md`, `002_irregular_volcanic_catchments_anti_radial_prompt.md`, and `003_scaled_preview_acceptance_and_handoff_prompt.md`. Batch 004 scope is terrain-only: scale the world/map 6x in linear size, remove the image-proven gear/fan-blade radial ridge/gully morphology, and regenerate/save acceptance evidence under `Images/TerrainPreview/` while preserving the clean Batch 003 coast/beach/shelf metrics. Broad `docs/` and `Docs/` folders remain intentionally deleted and must not be restored as part of this lifecycle.

## 2026-06-27-pr5-implementation-not-docs-workflow

status: confirmed
created: 2026-06-27
source: user correction during Batch 004 merge workflow
target: .mex/patterns/pr5-implementation-workflow.md, .mex/context/active-memory.md
reason: User explicitly corrected that PR5 workflow/merge authority means run the actual implementation workflow and build game/source changes, not package precursor/runtime Markdown or docs-only changes as a fake implementation PR.
review_by: 2026-07-27

When the user gives PR5 workflow authority, merge authority, or says to run the workflow, implement the actual approved game/source scope for the batch. Use precursor/runtime files as inputs, not as the whole deliverable. Do not create docs-only, lifecycle-only, or prompt-file-only PRs unless the user explicitly asks for only Markdown deliverables. Batch implementation PRs should be substantial enough for the approved scope and should not default to tiny low-impact ~200 LOC paper changes when the requested terrain/game work needs a larger source implementation. Current Batch 004 implementation target is actual terrain source/tooling work: 6x linear world scale, anti-radial catchment/ridge/gully rewrite, and regenerated terrain preview evidence under `Images/TerrainPreview/`.

## 2026-06-27-pr5-batch004-x6-anti-radial-implementation

status: confirmed
created: 2026-06-27
source: pr5lifecycle-runtimefiles/004/** and user merge/workflow correction
target: Source/JungleGame/JungleVolcanicIslandTerrainModel.*, Source/JungleGame/JungleFullSizeTerrainShellActor.*, scripts/terrain-preview-export.py, Images/TerrainPreview/**
reason: Batch 004 required actual game/source implementation, not docs-only packaging: increase world/map scale by 6x, remove image-proven radial gear/fan-blade ridge/gully morphology, and regenerate deterministic terrain preview evidence.
review_by: 2026-07-27

PR5 Batch 004 implementation changes the canonical volcanic island terrain authority from 16.256 km to 97.536 km world size, mean island radius from 7 km to 42 km, max island radius from 7.35 km to 44.1 km, target peak from 1400 m to 3800 m, and primary catchments from 14 equal radial sectors to 23 deterministic irregular basin anchors. The ridge/gully model now uses warped basin angles, per-basin width/curve/strength data, branch gullies, basin-specific lahar corridors, irregular coastal fans, larger crater/vent/fissure scale, and scaled runtime shell/smoke-view extents. The terrain preview exporter mirrors the Batch 004 x6 terrain math and generated `Images/TerrainPreview/PR5_Batch004_X6_Island_1024px_*`; manifest reports world_size_m 97536.0, height_max_m 4593.8807, shoreline_error_max_m 0.0, beach_continuity_pct 100.0, ocean_below_sea_pct 100.0, square_edge_ocean_violations 0, linear scale 6.0, area scale 36.0, and morphology `x6-irregular-basin-graph-anti-radial`. Validation: `git diff --check` passed, Python exporter syntax passed, 1024px preview generation passed, live workflow references no longer point at deleted `docs/` / `Docs/` paths, and PR #53 merged to `main` as `cfbc679` from source commit `f107b7a`. UE Build.sh remains environment-blocked because documented local Build.sh paths are absent.

## 2026-06-27-pr5-batch005-batch006-precursor-setup

status: active-unreviewed
created: 2026-06-27
source: user
target: pr5lifecycle-precursor/005, pr5lifecycle-precursor/006
reason: User requested ambitious new terrain math precursor batches after Batch 004 preview review.
review_by: 2026-07-27

Batch 004 is complete historical evidence and must not be extended unless explicitly requested. Its preview passed coast/scale metrics but failed morphology review because the island still reads as volcano-owned and procedural-radial. New terrain math work starts in Batch 005. Batch 005 contains five terrain-only precursor prompts for geomorphology math core, hydrology/erosion solver, ridge-valley landform graph, volcanic subsystem math, and multi-scale feature grammar. Batch 006 contains five overflow precursor prompts for real volcanic island DEM calibration, offline generator/runtime bridge architecture, morphology preview validation, implementation sequence, and final terrain math acceptance/handoff. Run Batch 005 before Batch 006.

## 2026-06-27-pr5-lifecycle-images-tracked

status: confirmed
created: 2026-06-27
source: user
target: pr5lifecycle-precursor/**, pr5lifecycle-runtimefiles/**, Images/TerrainPreview/**
reason: User explicitly confirmed PR5 lifecycle folders and terrain preview image evidence must be tracked on GitHub, then authorized a PR/merge with no CodeRabbit review loop.
review_by: 2026-07-27

PR5 lifecycle prompt/runtime folders and terrain preview image evidence are repo-owned tracked state, not local scratch. Current tracking change moves terrain preview evidence into numbered run folders under `Images/TerrainPreview/001/` and `Images/TerrainPreview/002/`, tracks Batch 005 and Batch 006 precursor/runtime files, preserves broad `docs/` and `Docs/` removal intent, and records that this tracking PR is intentionally run without a CodeRabbit review loop by explicit user instruction.

## 2026-06-27-pr5-batch005-001-geomorphology-core

status: confirmed
created: 2026-06-27
source: pr5lifecycle-runtimefiles/005/001_geomorphology_math_core_foundation_research.md
target: Source/JungleGame/JungleVolcanicIslandTerrainModel.h, Source/JungleGame/JungleVolcanicIslandTerrainModel.cpp
reason: PR5 Batch 005 runtime file 001 requires the terrain authority to move from crater-owned island math toward a geomorphology-first foundation where volcano-disabled terrain remains structurally meaningful.
review_by: 2026-07-27

PR5 Batch 005 / 001 is GitHub PR #55 on branch `pr5-batch005-001-geomorphology-core`, source commit `72e0a8a`. It adds basement and regional landform height fields, eight deterministic landform region weights/IDs, a volcano-disabled height evidence path using the same coast clamp, a separated bounded active-volcano contribution, updated generator identity `JG_TERRAIN_MATH_CORE_005_001`, and acceptance fields for volcano-disabled terrain and landform-region authority. Validation: `git diff --check` passed, CodeRabbit status passed, and `JungleGameEditor Linux Development` UBT succeeded. PR #55 landed on `main` as merge commit `b558c06`. Next target is PR5 Batch 005 / 002 hydrology, erosion, and catchment solver.

## 2026-06-27-pr5-batch005-002-hydrology-erosion

status: confirmed
created: 2026-06-27
source: pr5lifecycle-runtimefiles/005/002_hydrology_erosion_catchment_solver_research.md
target: Source/JungleGame/JungleVolcanicIslandTerrainModel.h, Source/JungleGame/JungleVolcanicIslandTerrainModel.cpp
reason: PR5 Batch 005 runtime file 002 requires graph/solver-style drainage evidence and bounded erosion/deposition shaping instead of crater-centered radial drainage.
review_by: 2026-07-27

PR5 Batch 005 / 002 is GitHub PR #56 on branch `pr5-batch005-002-hydrology-erosion`, source commit `f191fd5`. It adds source-owned hydrology evidence fields for flow accumulation, trunk/tributary stream masks, watershed divide masks, stream order, outlet IDs, stream-power incision, hillslope diffusion, hydrology fan deposition, hydrology solver config identity `JG_HYDROLOGY_EROSION_005_002`, and hydrology acceptance gates while preserving locked coast/beach/ocean constraints. Validation: `git diff --check` passed, CodeRabbit status passed, and `JungleGameEditor Linux Development` UBT succeeded after an initial DevSpace timeout/retry. PR #56 landed on `main` as merge commit `e067d40`. Next target is PR5 Batch 005 / 003 ridge-valley graph and landform regions.

## 2026-06-27-pr5-batch005-003-ridge-valley-regions

status: confirmed
created: 2026-06-27
source: pr5lifecycle-runtimefiles/005/003_ridge_valley_graph_and_landform_regions_research.md
target: Source/JungleGame/JungleVolcanicIslandTerrainModel.h, Source/JungleGame/JungleVolcanicIslandTerrainModel.cpp
reason: PR5 Batch 005 runtime file 003 requires stable graph-owned landform region identity so the interior terrain is not just a crater-centered height formula.
review_by: 2026-07-27

PR5 Batch 005 / 003 is GitHub PR #57 on branch `pr5-batch005-003-ridge-valley-regions`, source commit `c8fe299`. It raises the terrain graph contract to 24 major regions, adds graph-owned landform class IDs, and exposes upland block, basin floor, saddle/pass, old terrace, scarp, secondary hill, and volcano apron masks while preserving coast/beach/ocean constraints. Validation so far: `git diff --check` passed and `JungleGameEditor Linux Development` UBT succeeded. Manual CodeRabbit review is required before merge. Next target after PR #57 merges and local `main` syncs is PR5 Batch 005 / 004 volcanic subsystem lava, collapse, and rift math.
