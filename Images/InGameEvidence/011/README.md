# In-Game Evidence 011 - Jungle Floor Runtime Terrain Material

This run verifies that the runtime full-size procedural terrain shell uses the
project jungle-floor material instead of the engine default/debug material.

Capture command:

```bash
<UE5_ROOT>/Engine/Binaries/Linux/UnrealEditor <PROJECT_ROOT>/JungleGame.uproject -game -RenderOffscreen -JungleTerrainShellSmoke -JungleTrackedVisualEvidence=Images/InGameEvidence/011/JungleFloorTerrain_Runtime_TerrainShell.png -ResX=1920 -ResY=1080 -windowed -nosplash -unattended -ddc=NoZenLocalFallback
```

Runtime proof from `Saved/Logs/JungleGame.log`:

```text
Full-size terrain shell material: /Game/JungleGame/Materials/Ground/M_JungleFloor_LeafMud_Test.M_JungleFloor_LeafMud_Test
right_hit=true right_actor=JungleFullSizeTerrainShellActor_0
back_hit=true back_actor=JungleFullSizeTerrainShellActor_0
Full-size terrain shell tracked smoke shot requested: .../Images/InGameEvidence/011/JungleFloorTerrain_Runtime_TerrainShell.png.
```

Captured image:

- `JungleFloorTerrain_Runtime_TerrainShell.png` - runtime `-game` screenshot from the procedural terrain shell smoke path.

Visual note: the material is wired to the runtime shell, but the current terrain
UV/camera scale makes the leaf-mud texture read mostly as a muted brown terrain
surface at runtime. Texture scale/readability tuning is intentionally deferred.
