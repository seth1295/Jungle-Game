# Packaged Large-World First Playable Smoke, Performance Checks, And Release Notes

## Roadmap Item

Roadmap PR #20 — `Packaged large-world first playable smoke, performance checks, and release notes`

## Goal

Record the first packaging smoke attempt for the large-world first playable lane, make the smoke command reproducible, and document the actual blockers without adding new gameplay, final art, or forbidden navigation UI.

This pass is intentionally honest: packaging was attempted and reached real UE build/cook machinery, but the package is not yet green because local cook/toolchain blockers remain.

## Added Smoke Tooling

`/scripts/ue5-linux-package-smoke.sh`

The script:

- locates the project root and `JungleGame.uproject`;
- uses `UE5_ROOT` if provided, with the current Linux default fallback;
- runs UAT `BuildCookRun` for Linux Development;
- builds by default so the smoke path is valid from a clean checkout;
- supports `JUNGLE_SKIP_BUILD=1` only for already-built local workspaces;
- uses headless-safe smoke flags: `-unattended`, `-utf8output`, and `-nullrhi`;
- routes output to ignored `PackagedBuilds/PR20Smoke` by default;
- uses `-ddc=InstalledNoZenLocalFallback` and forwards `-skipzenstore` through additional cooker options based on PR #20 validation findings;
- warns when no project `.umap` exists under `Content/`;
- warns when `ShaderCompileWorker.modules` is missing;
- supports `--dry-run` for command validation without generating package output.

Run dry validation:

```bash
bash scripts/ue5-linux-package-smoke.sh --dry-run
```

Run package smoke:

```bash
bash scripts/ue5-linux-package-smoke.sh
```

Optional environment overrides:

```bash
UE5_ROOT=/path/to/UnrealEngine ARCHIVE_DIR=/tmp/JungleGameSmoke bash scripts/ue5-linux-package-smoke.sh
```

Already-built local workspace smoke:

```bash
JUNGLE_SKIP_BUILD=1 bash scripts/ue5-linux-package-smoke.sh
```

## Validation Attempts

### 1. Full BuildCookRun

Command shape:

```bash
RunUAT.sh BuildCookRun \
  -project=/home/seth/projects/new-project/JungleGame.uproject \
  -noP4 \
  -platform=Linux \
  -clientconfig=Development \
  -serverconfig=Development \
  -build -cook -stage -pak -archive \
  -archivedirectory=/home/seth/projects/new-project/PackagedBuilds/PR20Smoke \
  -unattended -utf8output -nullrhi
```

Result:

- DevSpace timed out before the command completed.
- UAT/UBT reached real build machinery.
- UBT started building `JungleGame`, `UnrealPak`, `ShaderCompileWorker`, and `JungleGameEditor`.
- Linux SDK/toolchain validation reached UE's bundled Clang 20.1.8 path.
- No project source compile error was surfaced in the captured tail.

### 2. No-Build Cook/Stage/Pak

Command shape:

```bash
RunUAT.sh BuildCookRun \
  -project=/home/seth/projects/new-project/JungleGame.uproject \
  -noP4 \
  -platform=Linux \
  -clientconfig=Development \
  -cook -stage -pak -archive \
  -archivedirectory=/home/seth/projects/new-project/PackagedBuilds/PR20Smoke \
  -skipbuild -nocompile -nocompileeditor \
  -unattended -utf8output -nullrhi
```

Result:

- Failed with AutomationTool exit code `25` / `Error_UnknownCookFailure`.
- Linux target platform validated:
  - `Linux VALID v26_clang-20.1.8-rockylinux8`
  - `LinuxArm64 VALID v26_clang-20.1.8-rockylinux8`
- `NullRHI` initialized for commandlet cook.
- Cook failed in `ZenStoreWriter` after Local ZenServer/Zen Store connection failures:

```text
Fatal error: [File:./Developer/IoStoreUtilities/Private/ZenStoreWriter.cpp] [Line: 497]
Failed to delete oplog on the ZenServer
```

Also observed:

```text
CrashReportClientEditor ... does not exist
```

This affects crash handling/reporting after the cook crash; it was not the first failure.

### 3. No-Zen DDC Fallback

Command shape:

```bash
RunUAT.sh BuildCookRun \
  -project=/home/seth/projects/new-project/JungleGame.uproject \
  -noP4 \
  -platform=Linux \
  -clientconfig=Development \
  -cook -stage -pak -archive \
  -archivedirectory=/home/seth/projects/new-project/PackagedBuilds/PR20SmokeNoZen \
  -skipbuild -nocompile -nocompileeditor \
  -unattended -utf8output -nullrhi \
  -ddc=InstalledNoZenLocalFallback -NoZenAutoLaunch
```

Result:

- DDC fallback switched to local installed cache successfully.
- Cook still entered the Zen package writer/storage-server path through generated cook project-store state.
- Failure remained `ZenStoreWriter.cpp` line `497`, `Failed to delete oplog on the ZenServer`.

### 4. Skip-ZenStore Cooker Attempt

Command shape:

```bash
RunUAT.sh BuildCookRun \
  -project=/home/seth/projects/new-project/JungleGame.uproject \
  -noP4 \
  -platform=Linux \
  -clientconfig=Development \
  -cook -stage -pak -archive \
  -archivedirectory=/home/seth/projects/new-project/PackagedBuilds/PR20SmokeSkipZen \
  -skipbuild -nocompile -nocompileeditor \
  -unattended -utf8output -nullrhi \
  -ddc=InstalledNoZenLocalFallback \
  -AdditionalCookerOptions="-skipzenstore"
```

Result:

- DevSpace timed out while the attempt was still active.
- Latest observed log showed the cook progressed farther than the previous Zen crashes.
- Cook reached at least:

```text
Cooked packages 154 Packages Remain 430 Total 584
```

- The next blocker was missing shader worker module manifest:

```text
Unable to read module manifest from '/run/media/seth/UE5_WORKSPACE/UnrealEngine/Engine/Binaries/Linux/ShaderCompileWorker.modules'. Module manifests are generated at build time, and must be present to locate modules at runtime.
ShaderCompileWorker terminated unexpectedly, return code 1.
```

## Current Packaging Status

Status: **not packaged yet**.

This PR produced a repeatable smoke command and identified concrete blockers. It did not produce a successful packaged first playable.

## Current Blockers

### Blocker 1 — No tracked project map

Current `Config/DefaultEngine.ini` points to engine template maps:

```ini
EditorStartupMap=/Engine/Maps/Templates/OpenWorld
GameDefaultMap=/Engine/Maps/Templates/OpenWorld
```

No project `.umap` exists under tracked `Content/` at this point. That means a package smoke currently targets engine template content plus runtime-spawned game code, not an authored project large-world map package.

### Blocker 2 — Zen Store / cook project-store instability

Cook repeatedly failed when the Zen Store package writer attempted to delete or open an oplog against an unhealthy local Zen server/project store.

Observed symptoms:

- Local ZenServer startup/readiness timeouts on port `8558`.
- Generated `ue.projectstore` state caused storage-server connection attempts.
- `ZenStoreWriter.cpp` fatal on oplog delete.

### Blocker 3 — Missing `ShaderCompileWorker.modules`

The skip-Zen attempt progressed into shader work but failed because the engine binary directory lacked:

```text
Engine/Binaries/Linux/ShaderCompileWorker.modules
```

UE reports that module manifests are generated at build time and must exist for runtime module lookup.

### Blocker 4 — Full engine/tool build exceeds current DevSpace timeout

A full `-build -cook -stage -pak` run reached UBT and began building project/editor/tool targets, but exceeded the DevSpace command timeout. This is likely a warm-up/build-duration problem rather than direct project source failure based on the captured log, but it is still a PR #20 packaging gate blocker.

### Blocker 5 — CrashReportClientEditor missing

After the cook crash, UE attempted to invoke `CrashReportClientEditor`, but the file was missing from the engine binary directory. This did not cause the original cook failure, but it degrades crash reporting and diagnostics for package validation.

## Performance / Smoke Notes

No packaged runtime performance result exists yet.

Validated so far:

- Linux SDK was valid.
- `NullRHI` commandlet cook path started.
- UAT/UBT reached real Linux build/cook tooling.
- Cook progressed into engine/template content and shader compilation.
- The machine reported a 16-core / 32-thread Ryzen 9 5950X and roughly 31 GB RAM available in logs.

Not validated yet:

- packaged executable launch;
- packaged Cell 0 entry spawn;
- runtime FPS/memory;
- traversal smoke in packaged build;
- large-world streaming smoke in packaged build.

## Security / Generated Output Notes

UE editor/cook runs can auto-write Android File Server config containing a `SecurityToken` into `Config/DefaultEngine.ini`. That config is unrelated to PR #20 and must not be committed.

Generated output paths remain ignored and should not be staged casually:

- `Binaries/`
- `Build/`
- `DerivedDataCache/`
- `Intermediate/`
- `Saved/`
- `PackagedBuilds/`
- `Releases/`

## Acceptance Status

- Reproducible smoke command: **added**.
- Packaging blockers documented: **done**.
- Linux SDK validation captured: **done**.
- Successful package: **blocked**.
- Runtime packaged performance evidence: **blocked**.
- Release notes: **included in this document**.

## Recommended Next Fix Lane

Before another packaging pass, fix or validate these in order:

1. Produce or commit an authored project map for the first playable, or intentionally document engine-template-map packaging as a temporary smoke mode.
2. Build/regenerate `ShaderCompileWorker` and confirm `Engine/Binaries/Linux/ShaderCompileWorker.modules` exists.
3. Reset or repair local Zen project-store state, or confirm a fully supported non-Zen cook path for UE 5.8 on this install.
4. Rerun `bash scripts/ue5-linux-package-smoke.sh` for a clean reproducible smoke, or `JUNGLE_SKIP_BUILD=1 bash scripts/ue5-linux-package-smoke.sh` only after local build products are known-good.
5. Only after a package exists, run packaged executable smoke and record FPS/memory/load behavior.
