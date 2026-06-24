# Jungle Cell 0 Validation

## Summary

Gate 005 has an initial C++-owned playable prototype for `Jungle Cell 0: Embers`.

The implementation is deliberately prototype-grade and avoids binary asset bloat:

- no committed `Content/` directory;
- no committed `.umap`;
- no paid assets/plugins;
- no combat, gore, VR, multiplayer, modern equipment, or heavy Blueprint architecture;
- runtime blockout actors are spawned from C++ using engine basic shapes.

## Implemented Prototype Scope

Implemented C++ areas:

| Area | File(s) | Purpose |
|---|---|---|
| Game mode | `JungleGameMode.*` | Sets player class and spawns the Cell 0 director |
| Player shell | `JunglePlayerCharacter.*` | First-person move/look/interact baseline |
| Interaction base | `JungleInteractableActor.*` | Small shared interaction base |
| Fire/embers | `JungleFireActor.*` | Ember decay, tending interaction, rain pressure hook, light placeholder |
| Crossing | `JungleCrossingActor.*` | Crossing state that can be changed and inspected |
| Marker clue | `JungleMarkerActor.*` | Revealed arranged marker placeholder |
| Watcher cue | `JungleWatcherCueActor.*` | Nonviolent distant cue placeholder |
| Slice director | `JungleCell0Director.*` | Runtime blockout, timed rain/crossing/cue sequence |

Config added:

| File | Purpose |
|---|---|
| `Config/DefaultEngine.ini` | Default map and `JungleGameMode` |
| `Config/DefaultGame.ini` | Project metadata |
| `Config/DefaultInput.ini` | Move/look/interact mappings |

## Build Validation

Command:

```bash
/mnt/ue5/UnrealEngine/Engine/Build/BatchFiles/Linux/Build.sh JungleGameEditor Linux Development -Project=/home/seth/projects/new-project/JungleGame.uproject -NoHotReload
```

Result:

```text
UHT processed JungleGameEditor in 2.5885219 seconds (18 generated files written)
[12/12] Link libUnrealEditor-JungleGame.so
Result: Succeeded
Total execution time: 7.66 seconds
```

Classification: **pass**.

## Commandlet Validation

Command:

```bash
/mnt/ue5/UnrealEngine/Engine/Binaries/Linux/UnrealEditor-Cmd /home/seth/projects/new-project/JungleGame.uproject -run=CompileAllBlueprints -NullRHI -NoSound -nop4 -unattended -stdout -FullStdOutLogOutput
```

Result:

```text
Compiling Completed with 0 errors and 0 warnings and 0 blueprints that failed to load.
Success - 0 error(s), 1 warning(s)
```

The one warning is the already-known local ZenServer auto-launch timeout.

Classification: **pass with known Zen warning**.

## Headless Game-Run Smoke

Command:

```bash
/mnt/ue5/UnrealEngine/Engine/Binaries/Linux/UnrealEditor-Cmd /home/seth/projects/new-project/JungleGame.uproject /Engine/Maps/Templates/OpenWorld -game -NullRHI -NoSound -nop4 -unattended -stdout -FullStdOutLogOutput -ExecCmds='quit'
```

Key result lines:

```text
LogLoad: Game class is 'JungleGameMode'
LogJungleGame: Display: Jungle Cell 0 runtime blockout spawned.
LogJungleGame: Display: Jungle Cell 0 GameMode started.
LogJungleGame: Display: Rain pressure started.
```

Classification: **pass for C++ runtime initialization**.

## Known Warnings

| Warning | Classification | Notes |
|---|---|---|
| Local ZenServer auto-launch timeout | Warning | Cache-service fallback; not blocking build or commandlet validation |
| `Failed to find game directory: /mnt/ue5/UnrealEngine/JungleGame/Binaries` | Warning | Appears during commandlet/game-run path but does not prevent successful validation |
| Wayland query before SDL init | Warning | Headless commandlet/game-run warning |
| EditorDataStorageUI widget factory warnings | Warning | Editor UI registration noise in headless run |
| EnhancedInput user settings warning | Warning | Headless/local player settings warning; input mappings are still configured |
| Full Linux package/cook smoke | Deferred warning | Earlier timeboxed DevSpace `BuildCookRun` attempt exceeded 300 seconds and produced no package output |

## Manual Visual Validation Still Useful

The prototype is C++-owned and headless-validated, but a human visual pass in the editor is still useful to confirm:

- first-person control feel;
- runtime blockout visibility/scale;
- fire light/scale behavior;
- crossing and marker readability;
- watcher cue visibility;
- whether OpenWorld template is acceptable as temporary host map before a dedicated map is added.

## Gate 005 Status

Gate 005 is **implemented as an initial prototype** and has passed C++ build, commandlet validation, and headless runtime initialization.

It is not yet a polished playable slice. The next improvement should be either:

1. a manual editor/PIE visual validation pass; or
2. a follow-up scoped map/content pass that adds one minimal dedicated `Content/JungleCell0` map with reviewed binary assets only.
