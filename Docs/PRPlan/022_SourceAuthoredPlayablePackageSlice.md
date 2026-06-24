# Source-Authored Playable Package Slice

## Decision

Make the current packaged game playable by expanding the source-authored Cell 0 slice, not by committing an opaque binary `.umap` in this PR.

Binary map policy for this lane:

- do not casually commit generated map/content churn;
- keep the first playable reproducible from source while the map asset policy is still being decided;
- allow a future reviewed binary-map PR only when its entire purpose is a project-owned first playable map, with generated/cache files excluded and package smoke repeated;
- until then, `JungleGameMode` plus `AJungleCell0Director` own the first playable package slice.

This is the practical choice because PR #21 proved the packaged build is green with the engine OpenWorld template plus the runtime Cell 0 fallback. The fastest safe route to a playable game is to make that runtime fallback an intentional, physical, traversable first-playable area.

## Scope

Allowed scope:

- source-authored Cell 0 terrain/readability expansion;
- package-launch convenience script;
- package and launch smoke validation;
- documentation of the binary-map decision.

Forbidden scope:

- player map;
- minimap;
- compass;
- bearing readout;
- GPS marker;
- objective marker or objective arrow;
- HUD clock or explicit time readout;
- orientation-confidence UI or hidden orientation meter;
- final art pass;
- full story expansion;
- broad binary map/content commits;
- generated package output in git.

## Gameplay Result

`AJungleCell0Director` now builds a larger source-authored first-playable loop:

```text
shelter / embers
  -> creek approach
  -> crossing
  -> arranged marker evidence
  -> watcher sightline break
  -> distant watcher cue
  -> exit ridge / landmark trunk
```

The area is intentionally made from simple engine primitives. It is not final art. It exists so the packaged build gives the player a physical space to walk, inspect, and read without relying on UI navigation aids.

## Files

- `Source/JungleGame/JungleCell0Director.h`
- `Source/JungleGame/JungleCell0Director.cpp`
- `scripts/ue5-linux-run-packaged-playable.sh`

## Player Launch

Build/package through the existing package scripts:

```bash
bash scripts/ue5-linux-package-preflight.sh
JUNGLE_SKIP_BUILD=1 bash scripts/ue5-linux-package-smoke.sh
```

Launch the packaged game interactively:

```bash
bash scripts/ue5-linux-run-packaged-playable.sh
```

Headless launch smoke:

```bash
HEADLESS=1 bash scripts/ue5-linux-run-packaged-playable.sh
```

## Validation Results

Standalone game target build:

```bash
/run/media/seth/UE5_WORKSPACE/UnrealEngine/Engine/Build/BatchFiles/Linux/Build.sh JungleGame Linux Development /home/seth/projects/new-project/JungleGame.uproject -WaitMutex
```

Result:

```text
Result: Succeeded
Output binary: /home/seth/projects/new-project/Binaries/Linux/JungleGame
```

Package smoke:

```bash
JUNGLE_SKIP_BUILD=1 bash scripts/ue5-linux-package-smoke.sh
```

Result:

```text
BUILD SUCCESSFUL
AutomationTool exiting with ExitCode=0 (Success)
```

Packaged headless launch smoke:

```bash
HEADLESS=1 bash scripts/ue5-linux-run-packaged-playable.sh
```

Result:

```text
Jungle Cell 0 playable source-authored slice spawned at X=204800.000 Y=-96768.000 Z=1200.000 facing 34.0 degrees using placed-world anchor.
Rain pressure started.
exit_code=0
```

Validated:

- standalone `JungleGame` target builds;
- package smoke succeeds;
- packaged headless launch exits with code `0`;
- launch logs show `JungleGameMode` loads, Cell 0 source-authored playable slice spawns, the player is moved to the entry point, and rain pressure starts;
- no generated/cache/package output is staged;
- no forbidden navigation/time/orientation UI is introduced.

## Remaining Deferred Asset Lane

A future project-map PR can create a project-owned `.umap`, but it must be explicit and reviewed as an asset PR. It should not be mixed with code gameplay changes or broad generated editor churn.
