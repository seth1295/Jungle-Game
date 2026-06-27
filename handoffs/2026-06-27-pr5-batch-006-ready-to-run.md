# PR5 Batch 006 Ready-To-Run Handoff

Created: 2026-06-27
Status: ready for later explicit `@devspace chatgpt GO`; implementation not started in the preparation chat.

## Workspace

- Path: `/home/seth/projects/new-project`
- Mode: checkout
- Base branch: `main`, currently synced with `origin/main` before preparation changes
- Project: Jungle Game / UE5 Linux-first terrain workflow

## Fresh Chat Startup

When the user starts a fresh chat and says `@devspace chatgpt GO` or equivalent, treat that as explicit authorization to run the PR5 Batch 006 implementation lifecycle for this repo.

Startup sequence:

1. Open `/home/seth/projects/new-project` with DevSpace in checkout mode.
2. Follow `AGENTS.md` exactly.
3. Load `.mex/ROUTER.md`.
4. Load `.mex/context/active-memory.md`.
5. Load `.mex/context/project.md`.
6. Load `.mex/context/pr-gates.md`.
7. Load `.mex/patterns/pr5-implementation-workflow.md`.
8. Load relevant PR workflow/CodeRabbit skills before PR lifecycle work.
9. Inspect branch/status/diff. The preparation diff listed below is expected and must be carried/staged deliberately on the first Batch 006 lifecycle branch, not discarded.
10. Load the Batch 006 precursor/runtime files below.

## Do Not Start From Batch 005

Batch 005 is complete through PR #59. Do not reopen Batch 005 unless the user explicitly says to.

The removed duplicate runtime file must stay removed:

- `pr5lifecycle-runtimefiles/006/001_geomorphology_math_core_foundation_research(1).md`

Do not restore it or treat it as valid Batch 006 input.

## Valid Batch 006 Inputs

Precursor files:

- `pr5lifecycle-precursor/006/001_dem_benchmarking_real_volcanic_islands_prompt.md`
- `pr5lifecycle-precursor/006/002_offline_generator_runtime_bridge_architecture_prompt.md`
- `pr5lifecycle-precursor/006/003_morphology_validation_preview_suite_prompt.md`
- `pr5lifecycle-precursor/006/004_terrain_math_implementation_pr_sequence_prompt.md`
- `pr5lifecycle-precursor/006/005_full_terrain_math_acceptance_handoff_prompt.md`

Runtime files:

- `pr5lifecycle-runtimefiles/006/001_dem_benchmarking_real_volcanic_islands_research.md`
- `pr5lifecycle-runtimefiles/006/002_offline_generator_runtime_bridge_architecture_research.md`
- `pr5lifecycle-runtimefiles/006/003_morphology_validation_preview_suite_research.md`
- `pr5lifecycle-runtimefiles/006/004_terrain_math_implementation_pr_sequence_research.md`
- `pr5lifecycle-runtimefiles/006/005_full_terrain_math_acceptance_handoff_research.md`

## Batch 006 Implementation Intent

Run Batch 006 as actual source/tool/evidence implementation, not docs-only packaging.

The lane is terrain-only and should replace the remaining radial/procedural-volcano feel with a measurable terrain system:

- DEM-style volcanic island calibration and morphology targets.
- Offline generator/runtime bridge architecture.
- Morphology preview validation suite.
- Terrain math implementation PR sequence from runtime file 004.
- Full terrain acceptance and handoff from runtime file 005.

Respect the locked terrain constraints:

```text
world_size_m: 97536.0 target
shoreline_error_max_m: 0 or effectively 0
beach_continuity_pct: 100%
ocean_below_sea_pct: 100%
square_edge_ocean_violations: 0
outer world edge remains ocean/container terrain
```

Forbidden scope remains: foliage, ecology, weather, audio, NPCs, buildings, survival systems, final materials, UI, maps, minimaps, compass, GPS, objective markers, player-facing overlays, or player-facing navigation systems.

## PR Lifecycle Expectations

- Create scoped implementation branches from clean synced `main`.
- Execute sequentially; no normal UE implementation PRs in parallel.
- Plan up to four PRs ahead, but implement and merge one PR at a time.
- Keep each PR substantial and source/evidence producing.
- Run relevant validation, including `git diff --check`, source scans, UBT when available, preview/export validation when relevant, and CodeRabbit unless the user explicitly disables it.
- Fix valid in-scope CodeRabbit findings.
- Update `.mex/context/active-memory.md` for every completed PR before starting the next PR.
- Merge only when gates are clean and the user instruction provides merge authority.
- Sync local `main` after each merge.

Runtime file 004 proposes a ten-PR terrain sequence. Follow that sequence unless direct inspection of Batch 006 files and current source state clearly supports a smaller equivalent sequence. Do not collapse the lane into tiny docs-only or placeholder commits.

## Terrain Preview Evidence Output

At Batch 006 completion, generate accepted terrain preview/manifest evidence under:

```text
Images/TerrainPreview/003/
```

Rules:

- `Images/TerrainPreview/003/` is the next terrain preview run folder after existing `001/` and `002/`.
- Do not overwrite `Images/TerrainPreview/001/` or `Images/TerrainPreview/002/`.
- If `003/` contains only preparation placeholders, replace/add the actual Batch 006 generated evidence there.
- If `003/` is no longer empty with real generated evidence when the future run starts, create the next unused numeric folder instead and record why in `.mex`.
- Evidence should include the required Batch 006 morphology suite outputs and manifest, not only the old Batch 004 five-image set.

## Current Preparation State

This handoff was created without starting Batch 006 implementation.

Expected dirty working tree from preparation:

- `.mex/context/pr-gates.md` modified to point at Batch 006 ready state.
- `.mex/context/active-memory.md` modified with Batch 006 ready-to-run entry.
- this handoff file added.
- duplicate runtime file 001 deleted.
- corrected runtime file 001 added.

The next worker should inspect status before beginning, create the first scoped Batch 006 lifecycle branch from `main`, carry/stage this preparation state deliberately, and commit it as part of the first valid lifecycle commit or an initial Batch 006 setup commit before implementation changes continue. Do not discard these preparation changes as junk.
