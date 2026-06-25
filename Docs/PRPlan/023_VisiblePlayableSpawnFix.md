# Visible Playable Spawn Fix

## Problem

The packaged build launched and reported Cell 0 spawn logs, but the player could see only sky/empty world after looking around.

This was not a camera-angle problem. Runtime diagnostics showed the player was falling away from the source-authored Cell 0 geometry.

## Confirmed Root Cause

`AJungleCell0Director` moved the player before runtime terrain was built, and the configured entry point was just outside the camp ground footprint.

Before this fix:

```cpp
FVector PlayerEntryLocalLocation = FVector(-180.0f, -180.0f, 0.0f);
```

The source-authored camp ground is centered at local `X=320` with scale `X=9`, so the visible/collidable platform starts at roughly local `X=-130`. The old entry X of `-180` placed the player just off the platform edge.

The first diagnostic run confirmed that failure:

```text
Cell 0 visibility diagnostic: pawn=X=204751.428 Y=-97017.881 Z=709.760 view=X=204751.428 Y=-97017.881 Z=774.250 rot=P=0.000000 Y=34.000000 R=-0.000000 down_hit=false down_actor=none down_loc=X=0.000 Y=0.000 Z=0.000 forward_hit=false forward_actor=none forward_loc=X=0.000 Y=0.000 Z=0.000
```

The pawn had already fallen below the intended Cell 0 elevation, and both the downward and forward traces hit nothing.

## Fix

`AJungleCell0Director::BuildCell()` now builds the source-authored terrain before moving the player to the entry point.

The entry point is now inside the camp ground footprint:

```cpp
FVector PlayerEntryLocalLocation = FVector(160.0f, 0.0f, 0.0f);
```

This puts the player on the camp ground instead of just outside its edge.

## Runtime Diagnostic Guard

A one-shot startup diagnostic logs the player position, view point, downward trace, and radial visibility traces after spawn. This is not player-facing UI. It exists to catch exactly this class of packaged-spawn regression, including the user looking around rather than only facing the initial forward direction.

## Validation

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

After the initial spawn fix, the packaged diagnostic run reported valid floor and forward hits:

```text
Player moved to Cell 0 large-world entry at X=204932.646 Y=-96678.529 Z=1200.000.
Cell 0 visibility diagnostic: pawn=X=204932.646 Y=-96678.529 Z=1200.150 view=X=204932.646 Y=-96678.529 Z=1264.150 rot=P=0.000000 Y=34.000000 R=-0.000000 down_hit=true down_actor=StaticMeshActor_2147482345 down_loc=X=204932.646 Y=-96678.529 Z=1110.000 forward_hit=true forward_actor=StaticMeshActor_2147482334 forward_loc=X=205369.963 Y=-96383.555 Z=1264.150
```

Meaning:

- the pawn stayed at the intended Cell 0 elevation;
- a downward trace hit the camp ground actor;
- a forward trace hit visible Cell 0 geometry;
- Cell 0 is loaded and physically present around the player.

A rendered packaged screenshot was also captured to the ignored package output path:

```text
PackagedBuilds/PR20Smoke/Linux/JungleGame/Saved/Screenshots/Linux/HighresScreenshot00000.png
```

That generated screenshot is validation evidence only and is not committed.

## User Screenshot Follow-up

The first follow-up screenshot showed the spawn was no longer sky-only, but the second screenshot showed the player could still turn toward a thin side/rear edge and see mostly ocean/sky with floating blockout silhouettes.

Follow-up fix:

- widened the source-authored camp ground and connected approach/far-bank/exit-ridge ground plates;
- added left, right, and rear jungle-edge blockout silhouettes around the entry area;
- added rear tree silhouettes so turning around from spawn still shows nearby source-authored Cell 0 geometry;
- expanded the diagnostic guard from forward-only visibility to forward/right/back/left traces.

## Generated Output Notes

Generated build/package output remains ignored and unstaged.

## Scope Guard

This fix does not add:

- player map;
- minimap;
- compass;
- bearing readout;
- GPS marker;
- objective marker or objective arrow;
- HUD clock or explicit time readout;
- orientation-confidence UI;
- binary `.umap` content;
- unrelated gameplay systems.
