# Packaged Full-Size World Smoke Evidence Research

## Executive Summary

PR5 batch 002 should add a narrow packaged-build smoke path that proves Jungle Game launches the authoritative full-size UE world, moves through multiple developer-only review points, captures representative screenshots, writes structured evidence logs, and fails fast when the loaded world is empty, ocean-only, sky-only, or otherwise not the intended full-size landscape state.

The smoke path should validate **map state as loaded world/level/landscape state**, not as any player-facing navigation UI. The correct target is a repeatable Linux Development packaged build run that loads the authoritative full-size map directly, teleports a controlled review pawn or camera through fixed review points, waits for world streaming/settling, captures screenshots, writes parseable log lines, and produces a local evidence report. Generated screenshots, logs, profiling captures, cooked output, staged builds, and local runtime reports must remain ignored and unstaged.

The implementation should stay deliberately small: one developer-only full-size-world smoke mode, one review-point definition source, one launch/package helper path if needed, one evidence-report writer, ignore-rule hardening, one `.mex`/handoff update, and no player-facing navigation features.

## Assumptions And Constraints

- Target engine family: UE5. Use the project's pinned UE version where it differs from current UE documentation.
- Target platform: Linux packaged build, preferably Development configuration for this validation pass.
- Target map: the authoritative full-size jungle world asset, referred to here as `Jungle_FullSize_World` until the repo's actual asset name is confirmed.
- World system: likely World Partition or a large persistent world using equivalent streaming/lifecycle rules.
- Evidence target: visual smoke evidence and structured logs, not deterministic pixel comparison.
- This is not an editor-only validation. Editor PIE evidence may help during development, but PR acceptance requires a runnable packaged build.
- No generated artifact is source evidence by itself unless the implementation records its location and summary in tracked text, such as the PR evidence report or `.mex` checkpoint.
- No player-facing map, minimap, compass, bearing display, GPS marker, objective marker, objective arrow, HUD clock, explicit time display, orientation confidence meter, hidden orientation meter, or character orientation hint is allowed.
- Any console commands or command-line flags used for validation must be developer-only and must not expose navigation assistance to the player build.

Relevant UE basis: command-line arguments can choose maps and pass flags/key-value pairs to executables, but custom flags must be parsed by project code to have effect. [UE Command-Line Arguments](https://dev.epicgames.com/documentation/unreal-engine/command-line-arguments-in-unreal-engine)

## Packaged Build Validation Goal

The packaged smoke run passes only when it proves all of the following in one repeatable Linux run:

1. The packaged executable starts successfully.
2. The authoritative full-size map asset is loaded, not an old Cell 0 smoke map or empty fallback level.
3. The run executes under an explicit developer-only validation mode, for example `-JungleSmoke=FullSizeWorld`.
4. The smoke controller visits the minimum review-point set.
5. Each review point resolves to valid terrain/landscape ground and non-empty world context.
6. Each review point captures a screenshot after a bounded streaming/settling wait.
7. Each review point emits a structured log line containing world identity, coordinates, terrain class, biome/riparian/slope/canopy state where available, screenshot result, and pass/fail checks.
8. The run writes a local report summarizing pass/fail, screenshot paths, log evidence, and any warnings.
9. `git status --short --ignored` or equivalent proves generated artifacts are ignored or outside the repo and that intended tracked changes only are staged.
10. The batch ends with a `.mex` update and a handoff/checkpoint that records what was proven and what the next batch should consume.

UE packaging basis: Unreal Automation Tool can cook/package from the command line, and on Mac/Linux the UAT entrypoint is `RunUAT.sh` under `Engine/Build/BatchFiles`. [UE Build Operations](https://dev.epicgames.com/documentation/en-us/unreal-engine/build-operations-cooking-packaging-deploying-and-running-projects-in-unreal-engine)

## Recommended Multi-Point Smoke Workflow

Use a single explicit smoke path.

1. **Package or reuse a fresh Linux Development build**
   - Prefer a repo helper script or documented command that packages Linux Development into an ignored local directory.
   - The package command should specify the authoritative full-size map in the project's packaging settings or command path so the map is cooked.
   - Do not use a Shipping build for the first implementation PR unless the project already has a safe, non-console automation path in Shipping. UE documentation notes that Development builds expose the runtime console, which is useful for runtime commands and validation. [UE Packaging Your Project](https://dev.epicgames.com/documentation/unreal-engine/packaging-your-project)

2. **Launch the packaged executable with explicit smoke arguments**
   - Example shape, not a required exact command:

     ```bash
     ./Saved/Packaged/Linux/JungleGame.sh /Game/JungleGame/Maps/Jungle_FullSize_World \
       -JungleSmoke=FullSizeWorld \
       -JungleSmokeRunId=pr5b002_local_$(date -u +%Y%m%dT%H%M%SZ) \
       -windowed -ResX=1920 -ResY=1080 -stdout -FullStdOutLogOutput -unattended
     ```

   - Use the actual packaged executable path and map asset path from the repo.
   - Keep `-stdout`/stdout logging if supported by the project environment; UE's command-line reference includes `stdout` for log output and `statunit` for startup `stat unit`. [UE Command-Line Reference](https://dev.epicgames.com/documentation/unreal-engine/unreal-engine-command-line-arguments-reference)

3. **Smoke controller validates world identity**
   - Log package/build config, command-line smoke mode, map asset path, `UWorld` name, persistent level name, and whether World Partition/streaming is active.
   - If the loaded world is not the authoritative full-size world, fail before screenshots.

4. **Review-point traversal**
   - Load review points from one stable source: a developer-only data asset, config table, or hardcoded validation table if the PR must stay minimal.
   - For each point:
     - Move a dev-only pawn/camera to the point.
     - Wait for bounded world settling.
     - Perform ground/landscape and surroundings checks.
     - Capture screenshot.
     - Emit structured log line.
   - Do not provide any interactive navigation UI to the player.

5. **Evidence report**
   - Write a local Markdown or JSON+Markdown report under an ignored path, for example:

     ```text
     Saved/Automation/JungleSmoke/<run_id>/005_packaged_full_size_world_smoke_evidence_report.md
     ```

   - The tracked PR may include a short evidence summary in `.mex` or docs, but not the generated artifacts.

6. **Source-control hygiene gate**
   - Before commit, run:

     ```bash
     git status --short --ignored
     git check-ignore -v Saved/Automation/JungleSmoke/<run_id>/* || true
     ```

   - Pass requires no generated screenshots/logs/reports/cooked/staged output in staged files.

Git basis: `.gitignore` specifies intentionally untracked files that Git should ignore; it does not affect files already tracked. [Git gitignore](https://git-scm.com/docs/gitignore)

## Review Point Set

Minimum implementation set: **six review points**. Four is the absolute emergency minimum, but six gives enough coverage to prove the loaded map is not a single small slice or empty fallback.

| Index | Recommended Name | Purpose | Required Evidence |
|---:|---|---|---|
| 00 | `RP_WORLD_ORIGIN_BASELINE` | Confirms origin-adjacent area is valid without proving only origin works. | ground hit, non-empty landscape/foliage context, world identity |
| 01 | `RP_CREEK_RIPARIAN_LOWLAND` | Proves creek/riparian state from PR5 contracts exists in packaged world. | terrain class includes creek/riparian/lowland signal where available |
| 02 | `RP_DENSE_INTERIOR_UNDERSTORY` | Proves dense jungle interior/canopy state. | canopy/foliage density signal or equivalent state |
| 03 | `RP_RIDGE_SLOPE_TRANSITION` | Proves ridge/slope terrain, not flat placeholder. | slope class/elevation delta/terrain class |
| 04 | `RP_COASTAL_FOREST_EDGE` | Proves coastline/forest transition without ocean-only spawn. | land ground hit plus ocean/coast context, not water-only |
| 05 | `RP_FAR_WORLD_EXTENT_CHECK` | Proves full-size extent and streaming far from origin. | far coordinate, expected terrain class, streaming/loaded-cell evidence |

Optional later points:

- `RP_HIGH_CANOPY_VIEW`
- `RP_VALLEY_CREEK_BEND`
- `RP_STEEP_TRAVERSAL_LIMIT`
- `RP_DEEP_SHADE_NO_SKYBOX_ONLY`
- `RP_SECONDARY_RIDGE_LONG_VIEW`

Coordinate selection rules:

- Coordinates must be authored from known valid world-design locations, not random search.
- At least two points must be far enough from origin to prove full-size map extent.
- At least one point must be near water/creek, one in dense interior jungle, one on/near slope or ridge, and one at a coast/edge transition.
- Each point should store expected terrain class and optional expected state tags.
- Point coordinates must be world-space UE coordinates, not player-facing coordinates or navigation readouts.

Suggested review-point record shape:

```text
Name=RP_CREEK_RIPARIAN_LOWLAND
WorldLocation=(X=-184250.0,Y=93200.0,Z=ResolvedByGroundTrace)
Yaw=135.0
ExpectedTerrainClass=CreekRiparianLowland
ExpectedStateTags=creek,riparian,lowland,canopy_partial
GroundTraceMinZ=-200000.0
GroundTraceMaxZ=200000.0
MinNonSkyPixels=implementation-defined
MinLandscapeOrTerrainHit=true
```

## Screenshot Capture Strategy

Use screenshots as visual evidence, not as committed assets.

Recommended capture policy:

- Resolution: fixed `1920x1080` for first PR unless the existing smoke path already standardizes another resolution.
- Window mode: `-windowed -ResX=1920 -ResY=1080` for consistent local/CI capture.
- Settling delay: wait until world streaming/ground checks pass, then wait an additional short bounded delay such as 1–3 seconds or a fixed number of frames.
- Capture method: project-controlled call to screenshot capture or console `Shot` command where appropriate.
- Capture timing: one screenshot per review point after validation checks, plus optional initial world-loaded screenshot.
- Screenshot format: PNG preferred.
- No screenshot files committed.

UE basis: the standard screenshot feature is available through the `Shot` console command, and high-resolution screenshots are also supported through console-command tooling. [UE Taking Screenshots](https://dev.epicgames.com/documentation/unreal-engine/taking-screenshots-in-unreal-engine)

Recommended screenshot directory:

```text
Saved/Automation/JungleSmoke/<run_id>/Screenshots/
```

Recommended screenshot naming convention:

```text
005_pr5b002_fullsize_<run_id>_<index>_<review_point>_<map_slug>_x<X>_y<Y>_z<Z>.png
```

Example:

```text
005_pr5b002_fullsize_20260626T093015Z_02_RP_DENSE_INTERIOR_UNDERSTORY_JungleFullSize_x-84210_y153900_z1820.png
```

Filename rules:

- Sanitize names to ASCII letters, digits, hyphen, and underscore.
- Round coordinates to whole units or a consistent small precision.
- Include run ID and index for sorting.
- Do not include player-facing navigation data.
- The evidence report should reference screenshot paths relative to the ignored run directory.

Avoid relying on editor-only screenshot comparison for this PR. UE's screenshot comparison and automation tooling are useful, but the target here is packaged-build visual smoke evidence, not editor screenshot baselines. [UE Screenshot Comparison Tool](https://dev.epicgames.com/documentation/unreal-engine/screenshot-comparison-tool-in-unreal-engine)

## Log And Evidence Fields

Create a dedicated log category, for example:

```text
LogJungleFullSizeSmoke
```

UE basis: `UE_LOG` writes formatted messages into log files and supports custom categories and verbosity levels. [UE Logging](https://dev.epicgames.com/documentation/unreal-engine/logging-in-unreal-engine)

Use one parseable summary line per run and one parseable line per review point. Prefer single-line JSON or stable key-value format. JSON is safer for later tooling; key-value is easier to grep. Either is acceptable if stable.

Required run-start fields:

| Field | Required | Notes |
|---|---:|---|
| `event` | yes | `FullSizeSmokeRunStart` |
| `run_id` | yes | Stable run identifier from CLI or generated UTC timestamp |
| `project` | yes | `JungleGame` |
| `engine_version` | yes | Runtime engine version if available |
| `build_config` | yes | Development/Test/Shipping |
| `platform` | yes | Linux |
| `executable` | yes | Packaged executable name/path |
| `map_asset` | yes | Expected authoritative map path |
| `world_name` | yes | Actual loaded `UWorld` |
| `persistent_level` | yes | Actual persistent level |
| `world_partition_enabled` | yes | boolean if detectable |
| `smoke_mode` | yes | `FullSizeWorld` |
| `artifact_root` | yes | Ignored local evidence path |

Required per-review-point fields:

| Field | Required | Notes |
|---|---:|---|
| `event` | yes | `FullSizeSmokeReviewPoint` |
| `run_id` | yes | Join key |
| `index` | yes | Stable order |
| `review_point` | yes | Name from approved set |
| `world_name` | yes | Actual loaded world |
| `map_asset` | yes | Expected map asset |
| `location_x/y/z` | yes | Actual final world-space location |
| `rotation_pitch/yaw/roll` | yes | Capture orientation |
| `expected_terrain_class` | yes | From review-point definition |
| `actual_terrain_class` | yes | From terrain/biome classifier or `Unknown` with reason |
| `biome_mask` | recommended | If PR5 source contracts expose it |
| `slope_class` | recommended | If available |
| `riparian_state` | recommended | If available |
| `canopy_state` | recommended | If available |
| `landscape_hit` | yes | boolean |
| `ground_actor` | yes | Hit actor/component name or class |
| `ground_z` | yes | Resolved ground height |
| `height_above_ground` | yes | Final camera/pawn offset |
| `loaded_cell_or_streaming_state` | recommended | Especially for World Partition |
| `visible_level_count` | recommended | If available |
| `screenshot_path` | yes | Relative ignored path |
| `screenshot_ok` | yes | boolean |
| `empty_world_check` | yes | pass/fail |
| `ocean_only_check` | yes | pass/fail |
| `sky_only_check` | yes | pass/fail |
| `frame_ms_avg` | recommended | Small sample after settling |
| `warnings` | yes | Empty array/string if none |
| `result` | yes | `PASS` or `FAIL` |

Required run-end fields:

| Field | Required | Notes |
|---|---:|---|
| `event` | yes | `FullSizeSmokeRunEnd` |
| `run_id` | yes | Join key |
| `total_points` | yes | Count attempted |
| `passed_points` | yes | Count passed |
| `failed_points` | yes | Count failed |
| `screenshots_written` | yes | Count |
| `report_path` | yes | Ignored local report path |
| `overall_result` | yes | `PASS` or `FAIL` |
| `exit_code` | yes | Non-zero on fail if automated |

Example log line shape:

```json
{"event":"FullSizeSmokeReviewPoint","run_id":"20260626T093015Z","index":2,"review_point":"RP_DENSE_INTERIOR_UNDERSTORY","map_asset":"/Game/JungleGame/Maps/Jungle_FullSize_World","world_name":"Jungle_FullSize_World","location_x":-84210,"location_y":153900,"location_z":1820,"expected_terrain_class":"DenseInteriorUnderstory","actual_terrain_class":"DenseInteriorUnderstory","landscape_hit":true,"ground_z":1660,"height_above_ground":160,"screenshot_path":"Saved/Automation/JungleSmoke/20260626T093015Z/Screenshots/005_pr5b002_fullsize_20260626T093015Z_02_RP_DENSE_INTERIOR_UNDERSTORY_JungleFullSize_x-84210_y153900_z1820.png","empty_world_check":"PASS","ocean_only_check":"PASS","sky_only_check":"PASS","result":"PASS"}
```

## Empty-World / Bad-Spawn Regression Checks

The smoke path must fail if a review point can see only sky, only ocean, unloaded void, or placeholder/fallback state.

Required checks:

1. **World identity check**
   - Fail if `world_name`, persistent level, or map asset path does not match the authoritative full-size world.
   - Fail if the old Cell 0 smoke level loads instead.

2. **Ground trace check**
   - At each point, line trace downward and upward within a generous range.
   - Pass requires a valid hit against Landscape, terrain mesh, or approved terrain collision component.
   - Fail if no ground is found, if hit is only sky/void, or if the hit actor belongs to a fallback/debug-only plane.

3. **Height sanity check**
   - Camera/pawn must end within an expected offset above ground.
   - Fail if final Z is unbounded, NaN, below terrain, far above terrain, or exactly a known fallback spawn height.

4. **Ocean-only check**
   - At coastal/water-adjacent points, water may be visible but land/terrain must also be valid.
   - Fail if ground classifier returns only water/ocean where a land/coast transition is expected.

5. **Sky-only visual check**
   - Basic screenshot validation should reject trivially empty captures.
   - Minimum implementation can use non-black/non-sky pixel heuristics if already available, but terrain trace and world identity checks are more important for the first PR.
   - Do not overfit pixel thresholds to one GPU/driver.

6. **Streaming readiness check**
   - For World Partition, wait until required nearby cells/actors are loaded or until a bounded timeout fails.
   - Log loaded/visible state where available.
   - UE World Partition stores a world in a single persistent Level and subdivides it into streamable grid cells loaded/unloaded at runtime by streaming sources such as the player, so the smoke path must account for streaming settling before screenshots. [UE World Partition](https://dev.epicgames.com/documentation/unreal-engine/world-partition-in-unreal-engine)

7. **Fallback actor check**
   - Fail if the pawn starts at PlayerStart/default origin when the requested review point was not applied.
   - Fail if a review camera/pawn is missing and the system silently continues.

8. **Timeout behavior**
   - Each point gets a bounded timeout.
   - A timeout is a failure with log evidence, not a skipped point.

Pass/fail examples:

| Scenario | Result | Reason |
|---|---|---|
| Correct full-size world, six points visited, all terrain hits valid, screenshots written | PASS | Required evidence exists |
| Packaged build opens old Cell 0 map | FAIL | Wrong authoritative world |
| Pawn teleports but no landscape hit at far point | FAIL | Full-size extent not proven |
| Coast point captures ocean plus valid land terrain | PASS | Coastal transition evidence valid |
| Coast point captures only water/sky and no land hit | FAIL | Ocean-only regression |
| Screenshot command fails but logs are valid | FAIL | Visual smoke evidence incomplete |
| Screenshots exist but are staged for commit | FAIL | Artifact hygiene violation |

## Performance And Stability Evidence

This PR should not become a performance-optimization batch, but it should record enough stability evidence to detect catastrophic full-size-world failures.

Minimum evidence:

- Startup reached full-size world.
- No crash during traversal.
- No fatal/error log lines from smoke category.
- Each review point completed within a bounded timeout.
- Approximate frame timing or runtime responsiveness recorded after settling.
- Any severe hitch/streaming timeout noted as warning/fail depending on severity.

Recommended lightweight metrics:

| Metric | Source | Required For Pass |
|---|---|---:|
| `point_settle_ms` | smoke controller timing | yes |
| `point_total_ms` | smoke controller timing | yes |
| `frame_ms_avg` | sampled runtime frame time or `stat unit` equivalent | recommended |
| `visible_level_count` / streaming state | runtime query or `stat levels` support | recommended |
| `memory_warning` | log scan or platform stats if available | recommended |

UE basis: `stat unit` reports frame, game thread, draw thread, GPU, RHI thread, and dynamic-resolution timing; `stat levels` reports level streaming visibility/loading states. [UE Stat Commands](https://dev.epicgames.com/documentation/unreal-engine/stat-commands-in-unreal-engine)

Do not create large `*.uestats` captures by default. UE's stat capture commands can create profiling files and should be stopped explicitly to avoid bloated artifacts. [UE Stat Commands](https://dev.epicgames.com/documentation/unreal-engine/stat-commands-in-unreal-engine)

## Artifact Ignore / Source-Control Hygiene

Generated artifacts must be local evidence, not committed source.

Add or verify ignore patterns for:

```gitignore
# Jungle packaged/full-size smoke evidence
Saved/Automation/JungleSmoke/
Saved/Screenshots/
Saved/Logs/
Saved/Profiling/
Saved/StagedBuilds/
Saved/Cooked/
Saved/Sandboxes/
Build/
Binaries/
DerivedDataCache/
Intermediate/
```

Adjust patterns to match the repo's existing UE ignore policy. Do not duplicate broad rules if the repo already has them. If the project intentionally tracks any `Build/` or `Binaries/` subpaths, keep the existing policy and add narrower smoke-output rules instead.

Required hygiene commands before staging/commit:

```bash
git status --short
git status --short --ignored
git diff -- .gitignore
git check-ignore -v Saved/Automation/JungleSmoke/<run_id>/Screenshots/*.png || true
git diff --cached --stat
```

Pass conditions:

- No `Saved/Automation/JungleSmoke/**` files staged.
- No screenshots, videos, logs, cooked output, staged builds, caches, or packaged binaries staged.
- `.gitignore` changes are scoped and justified.
- Tracked evidence is text-only: implementation code/config, docs/spec, `.mex` checkpoint/handoff.

Fail conditions:

- Any generated PNG/video/log/cooked/staged/cache file appears in `git diff --cached --name-only`.
- Smoke report is generated under an unignored repo path.
- Ignore rules are too broad and hide source files that should remain visible.

Git basis: ignored files are intentionally untracked files; already tracked files are not affected by `.gitignore`, so the PR must inspect staged files directly rather than assuming ignore rules are enough. [Git gitignore](https://git-scm.com/docs/gitignore)

## Batch Completion Evidence Report

The local generated report should be ignored, but the implementation PR should also include a tracked concise evidence summary in the project's normal checkpoint mechanism.

Recommended local generated report path:

```text
Saved/Automation/JungleSmoke/<run_id>/005_packaged_full_size_world_smoke_evidence_report.md
```

Recommended generated report contents:

```markdown
# PR5 Batch 002 Full-Size World Smoke Evidence

- Run ID:
- Date UTC:
- Build configuration:
- Platform:
- Packaged executable:
- Expected map asset:
- Actual world:
- World Partition / streaming:
- Review points attempted:
- Review points passed:
- Screenshots written:
- Overall result:

## Review Point Results

| Index | Review Point | Location | Terrain | Screenshot | Result |
|---:|---|---|---|---|---|

## Failures / Warnings

## Artifact Hygiene

- `git status --short`:
- `git status --short --ignored`:
- Generated artifact root:
- Staged generated artifacts: none/FAIL

## Handoff

- What this batch proves:
- What remains for next batch:
```

Tracked `.mex`/checkpoint update should include:

- Batch name: `PR5 Batch 002 — Packaged Full-Size World Smoke Evidence`.
- Implementation branch/commit/PR reference if known.
- Build config and platform used.
- Run ID.
- Number of review points passed.
- Local artifact root path, but not artifact contents.
- Statement that generated artifacts are ignored and unstaged.
- Next batch feed:
  - Use this smoke path as baseline for future full-size traversal checks.
  - Expand terrain classifier coverage only after packaged smoke proves the authoritative world.
  - Add CI/headless/compare automation later if needed.

## Implementation-Sized PR Scope

Keep the implementation PR narrow and reviewable.

In scope:

- Add developer-only packaged full-size smoke mode.
- Add or refine review-point definitions.
- Add structured logging for run start, per-point evidence, and run end.
- Add screenshot capture at each review point.
- Add ground/world/empty/ocean/sky regression checks.
- Add local evidence report writer.
- Add or refine ignore rules for smoke artifacts.
- Add/update a short tracked doc or `.mex` checkpoint.
- Add minimal tests only for pure parsing/report-generation helpers if practical.

Out of scope:

- No production navigation UX.
- No player-facing map, minimap, compass, objective markers, bearing numbers, HUD clock, time display, orientation meter, or character orientation comments.
- No committing screenshots/videos/logs/cooked output/staged builds/caches.
- No broad performance optimization.
- No full automation framework migration unless the repo already has it and this PR only plugs into it.
- No large visual-baseline comparison suite.
- No rewriting terrain/world generation.

Suggested PR title:

```text
PR5 Batch 002: packaged full-size world smoke evidence
```

Suggested commit boundaries:

1. `Add full-size smoke review point definitions`
2. `Add packaged full-size smoke logging and screenshots`
3. `Add smoke evidence report and artifact ignore coverage`
4. `Update PR5 batch checkpoint`

If the repo prefers one commit per PR, combine only after the diff is inspected.

## Files And Areas Likely Touched

Likely touched areas, adapted to actual repo layout:

| Area | Likely Purpose |
|---|---|
| `Source/JungleGame/...Smoke...` | Developer-only smoke controller/subsystem |
| `Source/JungleGame/...WorldValidation...` | Reuse existing PR5 terrain/world validation contracts |
| `Content/.../Maps/...` | Only if adding developer-only placed review actors/data; avoid binary churn if possible |
| `Config/DefaultGame.ini` or packaging config | Ensure authoritative full-size map is cooked/launchable |
| `Config/DefaultEngine.ini` | Only if needed for logging/scalability/smoke config |
| `Scripts/` or `tools/` | Optional Linux package/launch helper |
| `.gitignore` | Ignore smoke artifacts and packaged output |
| `.mex` / checkpoint / handoff doc | Batch completion summary |
| `Docs/` | Optional concise tracked smoke instructions |

Prefer source/config review-point definitions over touching binary `.uasset` files unless the existing project pattern already uses data assets.

If a `.uasset` must be touched, the PR description should explain exactly why and list the asset name. Do not combine unrelated content edits.

## Acceptance Criteria

The implementation PR is acceptable only when all criteria pass:

1. **Packaged launch**
   - Linux packaged Development build launches from command line.
   - Launch uses explicit full-size smoke mode.
   - Launch loads the authoritative full-size world asset.

2. **Review points**
   - At least six named review points exist.
   - At least one point covers each of: origin/baseline, creek/riparian lowland, dense interior, ridge/slope transition, coastal/forest edge, far world extent.
   - Review points are developer-only and not exposed as player navigation.

3. **Screenshots**
   - One screenshot is captured per review point.
   - Screenshot paths follow the naming convention.
   - Screenshots are written under ignored local artifact root.
   - No screenshots are staged or committed.

4. **Logs**
   - Run-start, per-point, and run-end structured log lines exist.
   - Logs include world, review point, coordinates, terrain class, relevant state, screenshot path, and pass/fail.
   - Wrong-world, empty-world, ocean-only, sky-only, missing-ground, and screenshot-failure cases produce clear failures.

5. **Regression checks**
   - Empty sky/ocean-only spawn regressions are caught.
   - Missing or wrong map fails.
   - Missing review point fails.
   - Streaming/settling timeout fails.

6. **Performance/stability**
   - Run completes without crash.
   - Each point completes within bounded timeout.
   - Basic frame/settling evidence is logged.

7. **Artifact hygiene**
   - `.gitignore` or equivalent ignore policy covers generated outputs.
   - `git diff --cached --name-only` contains no generated artifacts.
   - `git status --short --ignored` confirms generated smoke outputs are ignored or outside repo.

8. **Batch close**
   - Evidence report is generated locally.
   - `.mex`/checkpoint is updated.
   - Handoff states what PR5 batch 002 now proves and what the next batch should consume.

## Risks And Stop Conditions

Stop and fix before merge if any of the following occur:

- Packaged build opens the wrong map.
- Packaged build works only in editor or PIE.
- Review points teleport into void, sky, or ocean-only state.
- Far-world review point fails while near-origin points pass.
- Screenshots are blank, sky-only, ocean-only, or not written.
- Structured logs omit map/world identity or coordinates.
- Smoke path exposes any player-facing navigation feature.
- Generated artifacts appear staged.
- `.gitignore` hides source files unintentionally.
- Evidence report claims pass while individual point logs contain failures.
- Build requires local machine paths that cannot be reproduced by another Linux developer.
- The implementation expands into unrelated terrain generation, UI, inventory, AI, or performance refactor work.

Mitigations:

- Keep smoke mode behind explicit command-line flag.
- Fail closed on missing data.
- Use bounded waits and explicit timeout messages.
- Prefer text/config source changes over binary content changes.
- Inspect staged files before commit.
- Record exact command used in the local report and `.mex` checkpoint.

## Rejected Or Forbidden Approaches

Forbidden:

- Player-facing map.
- Player-facing minimap.
- Player-facing compass.
- Bearing number/readout.
- GPS marker.
- Objective marker.
- Objective arrow.
- HUD clock.
- Explicit time display.
- Orientation confidence meter, visible or hidden.
- Character comments that tell the player whether they are oriented.
- Any modern navigation tool.
- Committing generated screenshots, videos, logs, cooked output, packaged build output, caches, or local runtime artifacts.

Rejected for this PR:

- Editor-only PIE smoke as final evidence.
- A single origin screenshot as proof of full-size world.
- Random teleport sampling with no expected terrain classes.
- Pixel-perfect screenshot baseline testing.
- Heavy Gauntlet/Horde infrastructure unless already present and trivial to reuse.
- Performance profiling captures by default.
- Broad world-generation rewrites.
- Any validation design that depends on player-visible UI.

Rationale:

- The batch goal is packaged full-size world proof, not player navigation and not visual QA infrastructure.
- Multi-point structured evidence is stronger than one screenshot because it proves identity, terrain variety, and world extent.
- Local generated artifacts are useful for review but must not become repository payload.

## Sources And References

- Epic Games, **Command-Line Arguments in Unreal Engine** — command-line syntax, map URL parameters, flags/key-value pairs, and custom argument parsing. <https://dev.epicgames.com/documentation/unreal-engine/command-line-arguments-in-unreal-engine>
- Epic Games, **Unreal Engine Command-Line Arguments Reference** — runtime flags including `stdout`, `statunit`, and screenshot-related flags. <https://dev.epicgames.com/documentation/unreal-engine/unreal-engine-command-line-arguments-reference>
- Epic Games, **Build Operations: Cooking, Packaging, Deploying, and Running Projects in Unreal Engine** — UAT command-line packaging and Linux `RunUAT.sh` location. <https://dev.epicgames.com/documentation/en-us/unreal-engine/build-operations-cooking-packaging-deploying-and-running-projects-in-unreal-engine>
- Epic Games, **Packaging Your Project** — packaged Development configuration and runtime console availability. <https://dev.epicgames.com/documentation/unreal-engine/packaging-your-project>
- Epic Games, **Taking Screenshots in Unreal Engine** — `Shot` console command and high-resolution screenshot tooling. <https://dev.epicgames.com/documentation/unreal-engine/taking-screenshots-in-unreal-engine>
- Epic Games, **Logging in Unreal Engine** — `UE_LOG`, log categories, and verbosity behavior. <https://dev.epicgames.com/documentation/unreal-engine/logging-in-unreal-engine>
- Epic Games, **World Partition in Unreal Engine** — single persistent level, streamable grid cells, runtime streaming sources. <https://dev.epicgames.com/documentation/unreal-engine/world-partition-in-unreal-engine>
- Epic Games, **Automation Test Framework in Unreal Engine** — automation scope and test hygiene guidance, including not assuming state and cleaning generated files. <https://dev.epicgames.com/documentation/unreal-engine/automation-test-framework-in-unreal-engine>
- Epic Games, **Functional Testing in Unreal Engine** — functional test structure and cleanup hooks. <https://dev.epicgames.com/documentation/unreal-engine/functional-testing-in-unreal-engine>
- Epic Games, **Screenshot Comparison Tool in Unreal Engine** — automation screenshot comparison context; useful later but not the primary target for this packaged smoke PR. <https://dev.epicgames.com/documentation/unreal-engine/screenshot-comparison-tool-in-unreal-engine>
- Epic Games, **Stat Commands in Unreal Engine** — `stat unit`, `stat levels`, stat capture behavior, and profiling artifact caution. <https://dev.epicgames.com/documentation/unreal-engine/stat-commands-in-unreal-engine>
- Epic Games, **Introduction to Performance Profiling and Configuration in Unreal Engine** — console usage in Development/Debug packaged builds and `Saved/Logs` location. <https://dev.epicgames.com/documentation/unreal-engine/introduction-to-performance-profiling-and-configuration-in-unreal-engine>
- Git, **gitignore Documentation** — ignore semantics and the fact that ignored patterns do not affect already tracked files. <https://git-scm.com/docs/gitignore>
