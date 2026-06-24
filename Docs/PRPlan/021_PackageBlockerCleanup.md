# Package Blocker Cleanup And Packaged Launch Smoke

## Scope

This PR continues the PR #20 packaging lane without adding gameplay scope.

Allowed scope:

- package/cook/stage/pak blocker fixes;
- package preflight tooling;
- standalone game target/link fix;
- package smoke and headless launch evidence;
- blocker notes.

Forbidden scope:

- new major gameplay systems;
- final art expansion;
- player map, minimap, compass, bearing readout, GPS, objective marker, HUD clock, explicit time readout, orientation-confidence UI;
- generated package output in git;
- broad binary map/content commits.

## Code Fix

`Source/JungleGame/JungleGame.cpp` now uses:

```cpp
IMPLEMENT_PRIMARY_GAME_MODULE(FJungleGameModule, JungleGame, "JungleGame");
```

The previous `IMPLEMENT_GAME_MODULE` form worked for editor/module loading, but standalone `JungleGame Linux Development` link failed with undefined symbols from the primary game module boilerplate path:

```text
GInternalProjectName
GForeignEngineDir
GIsGameAgnosticExe
FMemory_Malloc
FMemory_Realloc
FMemory_Free
```

After switching to the primary game module macro, standalone `JungleGame` linked successfully and wrote:

```text
Binaries/Linux/JungleGame
Binaries/Linux/JungleGame.target
```

Generated binaries remain ignored and are not committed.

## Tooling Added

### `scripts/ue5-linux-package-preflight.sh`

This script validates and, when needed, builds the tool/target receipts required before package smoke:

- `ShaderCompileWorker.modules`
- `UnrealPak.modules`
- `Binaries/Linux/JungleGame.target`

Dry run:

```bash
bash scripts/ue5-linux-package-preflight.sh --dry-run
```

Build missing preflight targets:

```bash
bash scripts/ue5-linux-package-preflight.sh
```

### `scripts/ue5-linux-package-smoke.sh`

The existing package smoke script now warns about all known missing preflight artifacts:

- missing `ShaderCompileWorker.modules`;
- missing `UnrealPak.modules`;
- missing `JungleGame.target` receipt.

It points the user to `scripts/ue5-linux-package-preflight.sh` rather than failing later in opaque UAT stages.

## Validation Commands Run

### 1. Rebuilt ShaderCompileWorker

```bash
/run/media/seth/UE5_WORKSPACE/UnrealEngine/Engine/Build/BatchFiles/Linux/Build.sh ShaderCompileWorker Linux Development -WaitMutex
```

Result:

```text
Result: Succeeded
Output binary: /run/media/seth/UE5_WORKSPACE/UnrealEngine/Engine/Binaries/Linux/ShaderCompileWorker
```

Confirmed generated manifest:

```text
/run/media/seth/UE5_WORKSPACE/UnrealEngine/Engine/Binaries/Linux/ShaderCompileWorker.modules
```

### 2. Package smoke after ShaderCompileWorker fix

```bash
JUNGLE_SKIP_BUILD=1 bash scripts/ue5-linux-package-smoke.sh
```

Result:

- cook succeeded;
- staging failed because the standalone game target receipt was missing:

```text
Stage Failed. Missing receipt '/home/seth/projects/new-project/Binaries/Linux/JungleGame.target'. Check that this target has been built.
```

This was a new later-stage blocker after the ShaderCompileWorker blocker was fixed.

### 3. Built standalone game target

```bash
/run/media/seth/UE5_WORKSPACE/UnrealEngine/Engine/Build/BatchFiles/Linux/Build.sh JungleGame Linux Development /home/seth/projects/new-project/JungleGame.uproject -WaitMutex
```

Initial result before source fix:

- project source compiled;
- link failed because the game module was not declared as the primary game module.

After the `IMPLEMENT_PRIMARY_GAME_MODULE` fix:

```text
Result: Succeeded
Output binary: /home/seth/projects/new-project/Binaries/Linux/JungleGame
```

### 4. Package smoke after standalone game target fix

```bash
JUNGLE_SKIP_BUILD=1 bash scripts/ue5-linux-package-smoke.sh
```

Result:

- cook succeeded;
- stage started;
- pak failed because `UnrealPak.modules` was missing:

```text
Unable to read module manifest from '/run/media/seth/UE5_WORKSPACE/UnrealEngine/Engine/Binaries/Linux/UnrealPak.modules'. Module manifests are generated at build time, and must be present to locate modules at runtime.
```

### 5. Rebuilt UnrealPak

```bash
/run/media/seth/UE5_WORKSPACE/UnrealEngine/Engine/Build/BatchFiles/Linux/Build.sh UnrealPak Linux Development -WaitMutex
```

Result:

```text
Result: Succeeded
Output binary: /run/media/seth/UE5_WORKSPACE/UnrealEngine/Engine/Binaries/Linux/UnrealPak
```

Confirmed generated metadata:

```text
/run/media/seth/UE5_WORKSPACE/UnrealEngine/Engine/Binaries/Linux/UnrealPak.modules
```

### 6. Final package smoke

```bash
JUNGLE_SKIP_BUILD=1 bash scripts/ue5-linux-package-smoke.sh
```

Result:

```text
BUILD SUCCESSFUL
AutomationTool exiting with ExitCode=0 (Success)
```

Evidence:

- Linux SDK valid.
- Cook completed with `ExitCode=0`.
- Cooked `585` packages.
- Stage completed.
- Pak and IoStore completed.
- Archive completed to ignored package output under `PackagedBuilds/PR20Smoke`.
- `JungleGame-Linux.pak` was created.
- IoStore container summary reported `JungleGame-Linux` at approximately `175.02 MiB`, compressed.

### 7. Packaged headless launch smoke

```bash
timeout 45s PackagedBuilds/PR20Smoke/Linux/JungleGame.sh -nullrhi -nosound -unattended -ExecCmds='quit'
```

Result:

```text
exit_code=0
```

Observed runtime evidence:

```text
LogInit: Display: Game Engine Initialized.
LogInit: Display: Starting Game.
LogLoad: LoadMap: /Engine/Maps/Templates/OpenWorld?Name=Player
LogWorldPartition: UWorldPartition::Initialize : World = /Engine/Maps/Templates/OpenWorld.OpenWorld, World Type = Game
LogLoad: Game class is 'JungleGameMode'
LogJungleGame: Display: Player moved to Cell 0 large-world entry at X=204751.428 Y=-97017.881 Z=1200.000.
LogJungleGame: Display: Jungle Cell 0 integrated world location spawned at X=204800.000 Y=-96768.000 Z=1200.000 facing 34.0 degrees using placed-world anchor.
LogJungleGame: Display: Jungle Cell 0 fallback large-world director spawned at X=204800.000 Y=-96768.000 Z=1200.000.
LogJungleGame: Display: Rain pressure started.
```

## Current Status

Package status: **green for headless Linux package smoke**.

The packaged build launches with `-nullrhi -nosound -unattended`, loads the OpenWorld template map, initializes `JungleGameMode`, spawns the Cell 0 large-world fallback, moves the player to the Cell 0 entry point, starts rain pressure, and exits cleanly through `ExecCmds=quit`.

## Remaining Warnings

### No authored project `.umap` yet

The package still uses the engine OpenWorld template configured in `Config/DefaultEngine.ini`:

```ini
EditorStartupMap=/Engine/Maps/Templates/OpenWorld
GameDefaultMap=/Engine/Maps/Templates/OpenWorld
```

This is acceptable as a temporary package smoke because `JungleGameMode` spawns the current source-driven Cell 0 fallback inside that world, but it is not yet an authored project map.

Next map/content lane should create or commit a reviewed project-owned first playable map if binary asset policy allows it.

### Generated artifacts are not committed

The successful package and build products live under ignored paths:

- `Binaries/`
- `Intermediate/`
- `Saved/`
- `PackagedBuilds/`
- `DerivedDataCache/`

They are validation evidence only, not repo content.

### Android File Server token auto-write

UE editor/build/cook runs can re-add Android File Server config with a `SecurityToken` to `Config/DefaultEngine.ini`. That config is unrelated and must be reverted before commits.

## Acceptance Status

- Standalone game target linker blocker: **fixed**.
- ShaderCompileWorker metadata blocker: **documented/preflighted and fixed in local environment**.
- UnrealPak metadata blocker: **documented/preflighted and fixed in local environment**.
- Cook/stage/pak/archive smoke: **passed**.
- Packaged headless launch smoke: **passed**.
- Authored project map: **still deferred**.
