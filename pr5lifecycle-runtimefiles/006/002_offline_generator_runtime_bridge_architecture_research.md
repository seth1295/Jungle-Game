# Offline Generator And Runtime Bridge Architecture Research

## Executive Summary

The recommended architecture is a **hybrid canonical C++ terrain authority plus offline/editor-generated artifacts**.

Jungle Game should not move its terrain truth into a Python-only generator, nor should it keep all future terrain work inside runtime actor sampling. The 97.536 km island target, future hydrology, erosion-style passes, terrain graph construction, DEM-style metrics, multi-resolution preview evidence, and deterministic validation gates are too broad for runtime-only sampling. The source of truth should be a deterministic C++ terrain core used by:

1. runtime terrain sampling;
2. UE editor commandlets;
3. validation tests;
4. local artifact generation;
5. preview export tooling.

Python should remain a **tooling wrapper and review/export layer**, not the canonical math authority. It can read manifests, invoke the UE commandlet, downsample generated grids, export PNG previews, and compare fingerprints, but it should not own terrain logic unless it is explicitly marked as a non-authoritative mirror.

The core pipeline should produce:

- small, tracked source config;
- small, tracked preview evidence;
- small, tracked validation manifests;
- large, local generated artifacts ignored by Git by default;
- optional curated binary golden samples only through Git LFS or external artifact storage;
- a runtime bridge that loads or samples generated height/mask tiles without re-running expensive global passes.

The architecture must preserve these existing terrain gates:

```text
world_size_m: 97536.0 target
shoreline_error_max_m: 0 or effectively 0
beach_continuity_pct: 100%
ocean_below_sea_pct: 100%
square_edge_ocean_violations: 0
outer world edge remains ocean/container terrain
```

The target migration should be staged: first formalize config and fingerprints, then add commandlet generation and manifests, then add preview export and validation, then bridge runtime consumers to generated tiles, and only then migrate heavier terrain math.

## Terrain-Only Scope Boundary

This document covers only:

- terrain source architecture;
- generator tooling;
- deterministic configuration identity;
- artifact policy;
- preview export;
- runtime height/mask bridge;
- UE5 terrain shell, mesh, LOD, collision, material-channel consumers;
- validation manifests and gates.

It deliberately excludes:

- foliage;
- ecology;
- weather;
- audio;
- NPCs;
- buildings;
- gameplay systems;
- final player-facing materials;
- map, compass, minimap, GPS, objective markers, or player-facing overlays.

Material-channel discussion is limited to terrain data flow: masks, debug layers, and future material inputs. It does not specify final art direction or biome content.

## Current Architecture Limitations

A runtime-only actor sampling approach is acceptable for early procedural terrain, but it becomes a liability once the generator needs global terrain reasoning.

Likely current limitations:

1. **Global passes are too expensive for runtime.** Hydrology, erosion-like relaxation, flow accumulation, basin assignment, coastline enforcement, and graph construction require neighborhood-wide or island-wide context. Recomputing that during gameplay startup or per-tile runtime generation risks long stalls and nondeterministic behavior.

2. **Preview evidence is weak if it is not tied to source identity.** A 1024 px preview image is useful for review only if it records the exact config, source version, generator version, and artifact hash that produced it.

3. **Runtime sampling cannot be the only validator.** Shoreline error, beach continuity, ocean-below-sea, square-edge ocean violations, NaN checks, seam deltas, and monotonic mask invariants should be evaluated in generator/validation passes before runtime assets are consumed.

4. **Python-only preview mirrors drift.** Python is excellent for export, images, charts, and batch workflows, but if it owns a second implementation of terrain math, it can silently diverge from UE runtime C++.

5. **Large artifacts can bloat the repository.** 8129-class heightfields, mask stacks, tile mips, debug rasters, and intermediate hydrology grids should not be casually committed to normal Git history.

6. **UE5 consumers need explicit data contracts.** Runtime procedural mesh, future Landscape import, material masks, collision, debug views, and LOD sampling all need the same coordinate origin, scale, sea level, tile addressing, border policy, and height encoding.

## Canonical Terrain Authority Options

| Option | Strengths | Weaknesses | Recommendation |
|---|---|---|---|
| Pure C++ runtime sampling | One implementation; no tool/runtime drift; direct gameplay compatibility. | Bad fit for global hydrology/erosion/graph passes; slow previews; hard to review at 8129-class resolution; runtime stalls. | Keep only for analytic base functions and lightweight runtime queries. Do not make it the whole pipeline. |
| Python preview/export mirror | Fast iteration; easy image export; strong scientific tooling; easy raster inspection. | High drift risk; not cooked runtime code; floating-point and noise differences can invalidate gates. | Use as wrapper/exporter only. Treat any math mirror as non-authoritative unless parity-tested. |
| UE editor-side generator | Uses project code, UE types, content paths, commandlets, and editor integration. | Editor-only tooling is not cooked runtime; can be slower to run headless; platform path issues need discipline. | Strong fit for generation and validation commandlets. |
| Generated height/mask artifacts | Fast runtime; stable review target; enables heavy offline passes. | Artifact invalidation complexity; storage policy needed; stale artifacts can be dangerous. | Required for heavy generator stages. Must be fingerprinted and manifest-driven. |
| Config-driven generation | Deterministic identity; source-owned parameters; reproducible artifacts. | Requires strict schema migration and canonical serialization. | Mandatory. |
| Hybrid canonical C++ plus tooling mirror | Runtime and commandlets use same source; Python handles automation/export; artifacts serve heavy passes. | More plumbing up front. | Recommended architecture. |
| Staged migration from current source math | Preserves existing gates while introducing infrastructure. | Slower than a rewrite spike. | Required to reduce regression risk. |

## Runtime Sampling Versus Offline Generation

### Runtime-sampleable passes

Runtime should be able to query terrain answers quickly and deterministically at arbitrary world coordinates or tile-local coordinates:

- island/container classification;
- outer ocean/container terrain classification;
- sea-level relation;
- final terrain height in meters;
- surface normal or gradient estimate;
- shoreline/beach classification;
- terrain mask values needed by mesh/material/collision consumers;
- tile availability and fallback status;
- low-order analytic base terrain, if generated artifacts are missing in editor/development builds;
- LOD/mip sampling for distant mesh generation.

Runtime sampling must **not** require whole-island graph rebuilding.

Suggested runtime API:

```cpp
struct FJungleTerrainSample
{
    double WorldX_M;
    double WorldY_M;
    float Height_M;
    float SeaLevel_M;
    FVector3f Normal;
    uint8 LandMask;
    uint8 OceanMask;
    uint8 BeachMask;
    uint8 CliffMask;
    uint8 RuntimeLodLevel;
    FName SourceFingerprint;
};

class FJungleTerrainRuntimeBridge
{
public:
    bool InitializeFromManifest(const FString& ManifestPath);
    FJungleTerrainSample Sample(double WorldX_M, double WorldY_M, int32 LodHint) const;
    bool TryGetTileMetadata(FIntPoint TileCoord, FJungleTerrainGeneratedTile& OutTile) const;
    const FJungleTerrainManifest& GetManifest() const;
};
```

### Offline/editor-generated passes

These should run in a commandlet/editor pipeline, not gameplay runtime:

- full island source grid construction;
- hydrology graph building;
- flow direction;
- flow accumulation;
- basin and catchment labeling;
- erosion-style iterative passes;
- volcanic subsystem composition;
- DEM-style slope, curvature, roughness, and relief metrics;
- coastline/beach continuity enforcement;
- seam validation;
- 8129-class source evidence generation;
- multi-resolution mip/tile export;
- PNG preview export;
- manifest and validation report generation.

Heavy passes may still be deterministic C++, but they should execute through tools such as `UJungleTerrainGeneratorCommandlet`, not through gameplay actors.

## Recommended Architecture

### Core recommendation

Use a **source-owned deterministic terrain core** compiled into the project and shared by commandlets, tests, and runtime.

Recommended module split:

```text
Source/JungleGame/Terrain/
  JungleTerrainCore/
    FJungleTerrainGeneratorConfig
    FJungleTerrainConfigFingerprint
    FJungleTerrainCanonicalRng
    FJungleTerrainMathVersion
    FJungleTerrainSourceGrid
    FJungleTerrainSourceSample
    FJungleTerrainMaskChannels
    FJungleTerrainValidationMetrics

  JungleTerrainGenerator/
    FJungleTerrainGraphBuilder
    FJungleTerrainHydrologyPass
    FJungleTerrainErosionPass
    FJungleTerrainVolcanicPass
    FJungleTerrainArtifactWriter
    FJungleTerrainManifestWriter

  JungleTerrainRuntime/
    FJungleTerrainRuntimeBridge
    FJungleTerrainGeneratedTile
    FJungleTerrainTileCache
    UJungleRuntimeTerrainShellComponent

  JungleTerrainEditor/
    UJungleTerrainGeneratorCommandlet
    UJungleTerrainPreviewExporter
```

Suggested script/tooling layer:

```text
scripts/
  terrain-preview-export.py
  terrain-validate-manifest.py
  terrain-clean-generated.py
  terrain-compare-fingerprints.py
```

Suggested config/artifact layout:

```text
Config/Terrain/
  JungleTerrainMath_002_OfflineBridge_v1.json
  JungleTerrainMath_002_OfflineBridge_v1.schema.json

Generated/Terrain/                         # ignored by Git
  <terrain_config_fingerprint>/
    manifest.json
    source/
      height_8129_f32.tilepack
      masks_8129_u8.tilepack
    tiles/
      lod0/
      lod1/
      lod2/
    debug/
      flow_accum_f32.tilepack
      slope_f16.tilepack

Images/TerrainPreview/
  Batch006_OfflineBridge/
    002_height_1024.png
    002_sea_mask_1024.png
    002_beach_mask_1024.png
    002_slope_1024.png
    002_flow_accum_log_1024.png
    002_validation_overlay_1024.png
    002_manifest_summary.json
    002_validation_report.md
```

### Authority rule

The canonical terrain truth is:

```text
canonical C++ terrain core + canonical normalized config + generator source version
```

Generated artifacts are not the truth by themselves. They are materialized outputs of a known truth identity.

Runtime must reject or warn on artifacts whose manifest does not match the expected config fingerprint and generator version.

### Why this fits UE5

UE5 World Partition is designed for large world management through distance-based level streaming and grid cells, which aligns with terrain tile artifacts and runtime terrain shell streaming. Large World Coordinates improve precision for massive worlds by moving core data types to double precision. UE commandlets run in a raw non-game environment, making them appropriate for headless generation/validation passes. UE Python is explicitly an editor automation/pipeline facility, not gameplay runtime scripting, which supports keeping Python as tooling rather than canonical runtime terrain logic. UE Landscape has specific heightmap dimensions, import formats, component costs, and debug-layer behavior, so generated height/mask outputs should be designed to feed both immediate runtime mesh shells and possible later Landscape import.

## Config Identity, Versioning, And Fingerprints

### Config class

Define:

```cpp
USTRUCT(BlueprintType)
struct FJungleTerrainGeneratorConfig
{
    GENERATED_BODY();

    UPROPERTY(EditAnywhere)
    FString SchemaVersion = TEXT("2");

    UPROPERTY(EditAnywhere)
    FString MathPipelineId = TEXT("JungleTerrainMath_002_OfflineBridge_v1");

    UPROPERTY(EditAnywhere)
    double WorldSizeM = 97536.0;

    UPROPERTY(EditAnywhere)
    double SeaLevelM = 0.0;

    UPROPERTY(EditAnywhere)
    int32 SourceResolution = 8129;

    UPROPERTY(EditAnywhere)
    int32 PreviewResolution = 1024;

    UPROPERTY(EditAnywhere)
    int32 TileSizeVertices = 257;

    UPROPERTY(EditAnywhere)
    int32 TileBorderVertices = 1;

    UPROPERTY(EditAnywhere)
    int64 Seed = 97536002;

    UPROPERTY(EditAnywhere)
    bool bForceOuterEdgeOcean = true;

    UPROPERTY(EditAnywhere)
    bool bEnableHydrologyPass = false;

    UPROPERTY(EditAnywhere)
    bool bEnableErosionPass = false;

    UPROPERTY(EditAnywhere)
    bool bEnableVolcanicPass = false;
};
```

Keep the authoritative serialized config as JSON in `Config/Terrain/`.

### Version names

Use these exact initial names for the new pipeline:

```text
MathPipelineId: JungleTerrainMath_002_OfflineBridge_v1
ConfigSchemaId: JungleTerrainGeneratorConfigSchema_002
ManifestSchemaId: JungleTerrainGeneratedManifest_002
PreviewBatchId: Batch006_OfflineBridge
RuntimeBridgeId: JungleTerrainRuntimeBridge_002
```

### Fingerprint names

Use three separate hashes:

```text
TerrainConfigFingerprint
TerrainSourceCodeFingerprint
TerrainArtifactFingerprint
```

Recommended fingerprint construction:

1. `TerrainConfigFingerprint`
   - canonical JSON serialization;
   - UTF-8;
   - sorted keys;
   - stable float formatting;
   - no timestamps;
   - no local file paths.

2. `TerrainSourceCodeFingerprint`
   - git commit SHA if available;
   - plus generator module file hashes if working tree is dirty;
   - plus `MathPipelineId`.

3. `TerrainArtifactFingerprint`
   - SHA-256 over manifest-declared artifact files;
   - include file size, relative path, and content hash;
   - exclude generated-at timestamps.

Composite identity:

```text
TerrainBuildFingerprint =
sha256(
  MathPipelineId +
  TerrainConfigFingerprint +
  TerrainSourceCodeFingerprint +
  GeneratorExecutableFingerprint
)
```

### Runtime log tokens

Add stable log tokens so generated artifacts can be audited in logs:

```text
JTERRAIN_CONFIG_FP=<sha256>
JTERRAIN_SOURCE_FP=<sha256>
JTERRAIN_ARTIFACT_FP=<sha256>
JTERRAIN_PIPELINE=JungleTerrainMath_002_OfflineBridge_v1
JTERRAIN_WORLD_SIZE_M=97536.0
JTERRAIN_SOURCE_RES=8129
JTERRAIN_PREVIEW_RES=1024
JTERRAIN_GATE_STATUS=PASS
```

Example runtime startup log:

```text
LogJungleTerrain: Display: JTERRAIN_PIPELINE=JungleTerrainMath_002_OfflineBridge_v1 JTERRAIN_CONFIG_FP=9e... JTERRAIN_ARTIFACT_FP=3b... JTERRAIN_WORLD_SIZE_M=97536.0 JTERRAIN_GATE_STATUS=PASS
```

### Drift prevention

Hard rules:

- Runtime must load the manifest before using generated tiles.
- Runtime must compare manifest `TerrainConfigFingerprint` against the expected project config.
- Editor preview export must write the same fingerprint into PNG metadata or sidecar JSON.
- Python preview scripts must fail if the commandlet manifest and source config disagree.
- CI should run at least a low-resolution deterministic validation pass.
- Any artifact generated from a dirty source tree must mark `source_tree_dirty: true` in the manifest.

## Generator Pass Pipeline

Recommended commandlet:

```cpp
UCLASS()
class UJungleTerrainGeneratorCommandlet : public UCommandlet
{
    GENERATED_BODY();

public:
    virtual int32 Main(const FString& Params) override;
};
```

Suggested invocation:

```bash
UnrealEditor-Cmd JungleGame.uproject \
  -run=JungleTerrainGenerator \
  -Config=Config/Terrain/JungleTerrainMath_002_OfflineBridge_v1.json \
  -Output=Generated/Terrain \
  -PreviewOut=Images/TerrainPreview/Batch006_OfflineBridge \
  -Resolution=8129 \
  -PreviewResolution=1024 \
  -Validate \
  -FailOnGateRegression
```

### Pass sequence

1. **Config load and canonicalization**
   - Parse `FJungleTerrainGeneratorConfig`.
   - Validate schema.
   - Emit `TerrainConfigFingerprint`.

2. **World-domain construction**
   - Define world domain as exactly `97536.0 m`.
   - Define origin convention.
   - Define sea level.
   - Define source pixel spacing.
   - Define edge policy.

3. **Base analytic terrain source**
   - Deterministic island/container field.
   - Deterministic elevation base.
   - Deterministic shoreline/beach basis.
   - Enforce outer edge as ocean/container terrain.

4. **Optional volcanic subsystem pass**
   - Rift/collapse masks.
   - Volcanic influence channel.
   - Height deltas.
   - Must not break shoreline/ocean gates.

5. **Hydrology graph pass**
   - Flow direction.
   - Flow accumulation.
   - Catchments/basins.
   - Drainage enforcement.
   - Ocean outlet validation.

6. **Erosion-style pass**
   - Iterative but deterministic.
   - Bounded delta per pass.
   - Fixed iteration count.
   - No wall-clock-dependent stopping condition.
   - Outputs sediment/erosion debug masks.

7. **Mask synthesis**
   - Ocean.
   - Land.
   - Beach.
   - Cliff/slope class.
   - Shore distance.
   - Flow accumulation.
   - Basin id.
   - Volcanic influence.
   - Erosion/sediment.

8. **Validation**
   - Existing gates.
   - Seam deltas.
   - NaN/Inf checks.
   - Range checks.
   - Tile-border consistency.
   - Manifest completeness.

9. **Artifact write**
   - Source grid tilepack.
   - Runtime tile mips.
   - Mask channels.
   - Manifest.
   - Validation report.
   - Preview images.

10. **Runtime bridge smoke test**
    - Load manifest.
    - Sample known points.
    - Verify gate summary.
    - Verify tile lookup and fallback behavior.

## Artifact Policy And Repository Hygiene

### Track in Git

Track:

```text
Config/Terrain/*.json
Config/Terrain/*.schema.json
Source/JungleGame/Terrain/**
scripts/terrain-preview-export.py
scripts/terrain-validate-manifest.py
Images/TerrainPreview/Batch006_OfflineBridge/*_1024.png
Images/TerrainPreview/Batch006_OfflineBridge/*_summary.json
Images/TerrainPreview/Batch006_OfflineBridge/*_validation_report.md
Docs/Terrain/*
```

Tracked preview evidence should remain small enough for ordinary code review.

### Do not track in normal Git

Ignore:

```gitignore
Generated/Terrain/**
Saved/Terrain/**
Intermediate/Terrain/**
DerivedDataCache/**
*.tilepack
*.height.f32
*.height.r16
*.mask.u8
*.flow.f32
*.basin.u16
```

### Optional Git LFS policy

Use Git LFS only for curated binary evidence that must be preserved across machines, such as one tiny golden tile or one compressed reference artifact. Do not use LFS as an excuse to commit every generated 8129-class output.

Possible `.gitattributes`:

```gitattributes
*.tilepack filter=lfs diff=lfs merge=lfs -text
*.height.r16 filter=lfs diff=lfs merge=lfs -text
*.mask.u8 filter=lfs diff=lfs merge=lfs -text
```

Preferred rule:

```text
Normal Git: source, configs, manifests, 1024 previews.
Git LFS: rare golden binary samples only.
External/local artifact cache: full generated terrain.
```

### Artifact root

Use:

```text
Generated/Terrain/<TerrainBuildFingerprint>/
```

This prevents stale artifacts from overwriting newer outputs and makes local cleanup safe.

### Artifact naming

Use deterministic names:

```text
height_8129_f32_<TerrainBuildFingerprint>.tilepack
height_8129_r16_landscape_<TerrainBuildFingerprint>.r16
masks_8129_rgba8_<TerrainBuildFingerprint>.tilepack
flow_accum_8129_f32_<TerrainBuildFingerprint>.tilepack
basins_8129_u16_<TerrainBuildFingerprint>.tilepack
runtime_tiles_lod0_<TerrainBuildFingerprint>.tilepack
runtime_tiles_lod1_<TerrainBuildFingerprint>.tilepack
manifest_<TerrainBuildFingerprint>.json
validation_<TerrainBuildFingerprint>.md
```

### Cache/security note

If using Unreal Zen Storage Server or a shared DDC-like cache, keep it on a trusted LAN/VPN. Do not expose unauthenticated cache services to the public internet.

## Preview Export Pipeline

### Goals

Preview images must be:

- small;
- deterministic;
- tracked;
- reviewable in PRs;
- tied to exact config/source/artifact fingerprints;
- sufficient to detect major terrain regressions without committing full artifacts.

### Required tracked previews

Under:

```text
Images/TerrainPreview/Batch006_OfflineBridge/
```

Create:

```text
002_height_1024.png
002_height_relief_1024.png
002_sea_mask_1024.png
002_beach_mask_1024.png
002_shore_distance_1024.png
002_slope_1024.png
002_flow_accum_log_1024.png
002_basin_id_debug_1024.png
002_volcanic_influence_1024.png
002_validation_overlay_1024.png
002_manifest_summary.json
002_validation_report.md
```

### Preview script

`scripts/terrain-preview-export.py` should be a wrapper, not the terrain authority.

Responsibilities:

- invoke or read output from `UJungleTerrainGeneratorCommandlet`;
- locate manifest by fingerprint;
- downsample 8129-class artifacts to 1024 previews;
- use consistent min/max and color ramps;
- write sidecar JSON;
- fail on fingerprint mismatch;
- fail if validation gates are absent;
- optionally compare current preview summary against a previous batch.

Suggested CLI:

```bash
python3 scripts/terrain-preview-export.py \
  --config Config/Terrain/JungleTerrainMath_002_OfflineBridge_v1.json \
  --manifest Generated/Terrain/<fp>/manifest.json \
  --out Images/TerrainPreview/Batch006_OfflineBridge \
  --preview-size 1024 \
  --require-gates-pass
```

### Preview sidecar fields

`002_manifest_summary.json`:

```json
{
  "preview_batch_id": "Batch006_OfflineBridge",
  "math_pipeline_id": "JungleTerrainMath_002_OfflineBridge_v1",
  "terrain_config_fingerprint": "sha256:...",
  "terrain_source_code_fingerprint": "sha256:...",
  "terrain_artifact_fingerprint": "sha256:...",
  "world_size_m": 97536.0,
  "source_resolution": 8129,
  "preview_resolution": 1024,
  "sea_level_m": 0.0,
  "shoreline_error_max_m": 0.0,
  "beach_continuity_pct": 100.0,
  "ocean_below_sea_pct": 100.0,
  "square_edge_ocean_violations": 0,
  "outer_edge_ocean_pass": true
}
```

## Runtime Terrain Shell Bridge

### Purpose

The runtime terrain shell bridge should let UE runtime consumers query generated terrain without knowing whether the underlying data came from analytic math, generated tiles, future Landscape import, or editor-only validation grids.

### Required runtime classes

```text
FJungleTerrainSourceGrid
FJungleTerrainGeneratedTile
FJungleTerrainRuntimeBridge
FJungleTerrainTileCache
UJungleRuntimeTerrainShellComponent
UJungleTerrainDebugSubsystem
```

### `FJungleTerrainSourceGrid`

Represents the full-resolution generated source domain.

Responsibilities:

- store grid resolution;
- store world size;
- store origin convention;
- convert world XY to grid UV;
- sample height/masks at source resolution;
- sample lower mip/LOD where requested;
- expose border policy.

### `FJungleTerrainGeneratedTile`

Represents one tile/mip of generated terrain.

Fields:

```cpp
struct FJungleTerrainGeneratedTile
{
    FIntPoint TileCoord;
    int32 LodLevel;
    int32 SizeVertices;
    int32 BorderVertices;
    FBox2d WorldBoundsM;
    FString HeightPayloadPath;
    FString MaskPayloadPath;
    FString TileFingerprint;
    bool bHasCollisionData;
    bool bHasMaterialMasks;
};
```

### Runtime fallback policy

In development builds:

1. If matching generated artifacts exist, load them.
2. If missing but analytic source math can provide safe fallback, log warning and sample low-order canonical C++ terrain.
3. If artifacts are required by config, fail fast.

In shipping builds:

- do not silently regenerate global terrain;
- do not fall back to stale artifacts;
- reject mismatched fingerprints.

### Runtime mesh/LOD sampling

The terrain shell should request samples by:

```text
world bounds + desired vertex spacing + LOD hint
```

The bridge should resolve:

- correct tile;
- mip level;
- border samples;
- height;
- masks;
- normals;
- collision simplification.

LOD sampling should be deterministic and tile-border-safe. Adjacent tiles must agree at shared borders.

## UE5 Landscape / Mesh / LOD / Collision Considerations

### Large world scale

The island target is 97.536 km across. UE5 Large World Coordinates are relevant because they improve world precision through double-precision core data types. Runtime terrain code should use `double` for world XY and meters-space domain conversion, then convert to `float` only at mesh-buffer boundaries.

### World Partition

World Partition should remain the likely large-world level-management layer. Its distance-based streaming grid maps naturally to generated terrain tile metadata and runtime shell tile loading. Terrain artifacts should be spatially chunked so they can align with future World Partition cell strategy rather than requiring one monolithic runtime payload.

### Landscape import compatibility

Future Landscape import should remain possible. UE Landscape’s technical constraints matter:

- components are the base unit for rendering, visibility, and collision;
- Epic’s guide lists 8129 × 8129 as a recommended Landscape size with 1024 components;
- 16-bit grayscale PNG, 8-bit `r8`, and 16-bit `r16` are supported heightmap import formats;
- Landscape debug visualizers can inspect layer channels.

Therefore, the generator should emit both runtime tile artifacts and optional Landscape-compatible `r16`/PNG heightmaps with sidecar metadata.

### Mesh shell compatibility

For a Linux-first UE5 runtime shell:

- generate mesh chunks from `FJungleTerrainRuntimeBridge`;
- keep chunk bounds in meters;
- include skirt or shared-border policy to prevent cracks;
- produce simplified collision meshes from lower LOD height samples;
- use async loading for tile payloads;
- keep expensive global data out of runtime mesh build paths.

### Collision

Collision should not require full-resolution 8129 sampling everywhere. Use:

- near-player collision LOD;
- simplified far collision;
- cached per-tile collision payloads if needed;
- deterministic resampling from generated source grid.

Collision build should log the same terrain fingerprints as rendering.

### Material/debug consumers

Material and debug views should consume named terrain channels, not ad hoc texture assumptions.

Debug views should include:

- height;
- slope;
- sea/ocean;
- beach;
- shore distance;
- flow accumulation;
- basin id;
- tile seam error;
- validation overlay.

Do not build player-facing overlays from these debug views.

## Mask And Channel Data Flow

### Channel model

Define:

```cpp
enum class EJungleTerrainChannel : uint8
{
    HeightM,
    OceanMask,
    LandMask,
    BeachMask,
    ShoreDistanceM,
    Slope,
    Curvature,
    FlowDirection,
    FlowAccumulation,
    BasinId,
    VolcanicInfluence,
    ErosionDelta,
    Sediment,
    ValidationOverlay
};
```

### Artifact channel formats

| Channel | Suggested format | Runtime use | Preview use |
|---|---:|---|---|
| HeightM | F32 source, optional R16 Landscape export | mesh/collision/LOD | height/relief |
| OceanMask | U8 | ocean/container classification | sea mask |
| LandMask | U8 | terrain shell inclusion | land mask |
| BeachMask | U8 | beach debug/material input | beach mask |
| ShoreDistanceM | F16/F32 | shoreline logic/debug | shore-distance gradient |
| Slope | F16 or U16 normalized | material/debug/collision hints | slope map |
| FlowDirection | U8 | debug/offline only | arrows/debug |
| FlowAccumulation | F32 or log F16 | debug/future terrain shaping | log flow map |
| BasinId | U16/U32 | debug/offline | basin color map |
| VolcanicInfluence | U8/F16 | terrain material/debug input | volcanic map |
| ErosionDelta | F16/F32 | debug/source evidence | erosion delta |
| Sediment | F16/F32 | debug/source evidence | sediment map |
| ValidationOverlay | RGBA8 | debug only | PR review overlay |

### RGBA preview packing

Do not overload final runtime mask semantics into arbitrary RGBA names. Use explicit metadata:

```json
{
  "file": "masks_8129_rgba8_<fp>.tilepack",
  "channels": {
    "R": "BeachMask",
    "G": "SlopeNormalized",
    "B": "VolcanicInfluence",
    "A": "OceanMask"
  }
}
```

### Material bridge

Future materials should bind by channel name, not by hardcoded texture slot alone.

Suggested material parameter naming:

```text
Terrain_HeightM
Terrain_OceanMask
Terrain_BeachMask
Terrain_ShoreDistanceM
Terrain_Slope
Terrain_FlowAccumulationLog
Terrain_VolcanicInfluence
```

Again, this is data flow only, not final material design.

## Validation And Manifest Requirements

### Manifest file

`manifest_<TerrainBuildFingerprint>.json` should include:

```json
{
  "manifest_schema_id": "JungleTerrainGeneratedManifest_002",
  "math_pipeline_id": "JungleTerrainMath_002_OfflineBridge_v1",
  "terrain_config_fingerprint": "sha256:...",
  "terrain_source_code_fingerprint": "sha256:...",
  "terrain_artifact_fingerprint": "sha256:...",
  "terrain_build_fingerprint": "sha256:...",
  "generator": {
    "tool": "UJungleTerrainGeneratorCommandlet",
    "tool_version": "002",
    "ue_version": "5.x",
    "platform": "Linux",
    "source_tree_dirty": false
  },
  "domain": {
    "world_size_m": 97536.0,
    "origin": "centered",
    "sea_level_m": 0.0,
    "source_resolution": 8129,
    "pixel_spacing_m": 12.0,
    "outer_edge_policy": "force_ocean_container"
  },
  "tiles": {
    "tile_size_vertices": 257,
    "tile_border_vertices": 1,
    "lod_count": 4,
    "tile_count_x": 32,
    "tile_count_y": 32
  },
  "artifacts": [
    {
      "role": "height_source",
      "path": "source/height_8129_f32_<fp>.tilepack",
      "format": "f32",
      "sha256": "...",
      "bytes": 0
    }
  ],
  "validation": {
    "gate_status": "PASS",
    "world_size_m": 97536.0,
    "shoreline_error_max_m": 0.0,
    "beach_continuity_pct": 100.0,
    "ocean_below_sea_pct": 100.0,
    "square_edge_ocean_violations": 0,
    "outer_edge_ocean_pass": true,
    "nan_count": 0,
    "inf_count": 0,
    "max_tile_border_height_delta_m": 0.0,
    "max_tile_border_mask_delta": 0
  }
}
```

### Hard validation gates

These are non-negotiable:

```text
world_size_m == 97536.0 target
shoreline_error_max_m <= epsilon
beach_continuity_pct == 100%
ocean_below_sea_pct == 100%
square_edge_ocean_violations == 0
outer_edge_ocean_pass == true
nan_count == 0
inf_count == 0
```

Recommended epsilon:

```text
shoreline_error_epsilon_m = 0.01
tile_border_height_delta_epsilon_m = 0.001 for generated same-LOD borders
```

If the existing system already achieves exact `0`, preserve exact `0` in integer/topological gates and only use epsilon where floating-point conversion makes exact equality inappropriate.

### Validation report

`002_validation_report.md` should include:

- config fingerprint;
- source fingerprint;
- artifact fingerprint;
- source resolution;
- preview resolution;
- pass/fail table;
- max/min heights;
- sea/ocean stats;
- beach continuity stats;
- edge policy confirmation;
- tile seam stats;
- generated file list;
- known limitations.

## Files And Areas Likely Touched

Likely new or modified areas:

```text
Source/JungleGame/Terrain/JungleTerrainCore/
Source/JungleGame/Terrain/JungleTerrainGenerator/
Source/JungleGame/Terrain/JungleTerrainRuntime/
Source/JungleGame/Terrain/JungleTerrainEditor/

Config/Terrain/
  JungleTerrainMath_002_OfflineBridge_v1.json
  JungleTerrainMath_002_OfflineBridge_v1.schema.json

scripts/
  terrain-preview-export.py
  terrain-validate-manifest.py
  terrain-compare-fingerprints.py
  terrain-clean-generated.py

Images/TerrainPreview/Batch006_OfflineBridge/
  002_*_1024.png
  002_manifest_summary.json
  002_validation_report.md

Docs/Terrain/
  002_offline_generator_runtime_bridge_architecture_research.md

.gitignore
.gitattributes
```

Possible test areas:

```text
Source/JungleGameTests/Terrain/
  JungleTerrainConfigFingerprintTests.cpp
  JungleTerrainRuntimeBridgeTests.cpp
  JungleTerrainValidationGateTests.cpp
```

## Large PR Implementation Scope

The implementation should be large enough to matter but split into reviewable PRs.

### PR A — Config identity and no-behavior-change terrain core

Scope:

- add `FJungleTerrainGeneratorConfig`;
- add canonical JSON load/normalize/hash path;
- add `MathPipelineId`;
- add source/version fingerprint helpers;
- add tests for stable hash output;
- do not rewrite terrain shape yet.

Acceptance:

- same config produces same hash on repeated runs;
- changing seed/world size/schema changes hash;
- existing runtime terrain gates still pass.

### PR B — Commandlet skeleton and manifest writer

Scope:

- add `UJungleTerrainGeneratorCommandlet`;
- generate manifest with config/source fingerprints;
- emit validation report stub;
- create ignored `Generated/Terrain/<fp>/`;
- no heavy hydrology/erosion yet.

Acceptance:

- commandlet runs headless on Linux;
- manifest contains required fields;
- missing/mismatched config fails cleanly;
- no large artifacts tracked.

### PR C — 1024 preview export and tracked evidence

Scope:

- add `scripts/terrain-preview-export.py`;
- commandlet exports 1024 previews;
- write `Images/TerrainPreview/Batch006_OfflineBridge/`;
- add validation overlay.

Acceptance:

- preview sidecar includes fingerprints;
- PR diff shows small PNGs and summary JSON only;
- existing gates are visible in validation report.

### PR D — Source grid and generated tile format

Scope:

- add `FJungleTerrainSourceGrid`;
- add `FJungleTerrainGeneratedTile`;
- write height/mask tilepack format;
- add tile seam validation;
- add local cleanup script.

Acceptance:

- generated artifacts land under ignored `Generated/Terrain/<fp>/`;
- manifest file hashes match payloads;
- tile borders are consistent;
- 8129-class generation is reproducible locally.

### PR E — Runtime bridge integration

Scope:

- add `FJungleTerrainRuntimeBridge`;
- load manifest;
- reject mismatched artifacts;
- expose height/mask sampling;
- add dev fallback only where safe;
- add runtime log tokens.

Acceptance:

- runtime loads correct artifact set;
- stale artifacts are rejected;
- bridge samples known coordinates;
- no global hydrology/erosion runs in gameplay.

### PR F — Heavy pass migration

Scope:

- migrate/introduce hydrology/erosion/volcanic passes one at a time;
- each pass must emit debug channels and validation deltas;
- preserve existing gates after each pass.

Acceptance:

- each new pass can be disabled;
- default config remains gate-clean;
- previews show before/after channel evidence;
- runtime bridge API does not change for every pass.

## Acceptance Criteria

The architecture is accepted when:

1. `FJungleTerrainGeneratorConfig` exists and owns deterministic terrain identity.
2. `JungleTerrainMath_002_OfflineBridge_v1.json` is tracked.
3. Terrain config hashing is stable and tested.
4. `UJungleTerrainGeneratorCommandlet` can run headless on Linux.
5. Generated outputs land under `Generated/Terrain/<TerrainBuildFingerprint>/`.
6. Large generated artifacts are ignored by normal Git.
7. 1024 preview images are tracked under `Images/TerrainPreview/Batch006_OfflineBridge/`.
8. Preview sidecar JSON records config/source/artifact fingerprints.
9. Manifest records domain, resolution, tile layout, artifact hashes, and validation gates.
10. Runtime bridge refuses mismatched artifacts.
11. Runtime logs stable `JTERRAIN_*` tokens.
12. Existing terrain gates remain passing:
    - `world_size_m: 97536.0 target`;
    - `shoreline_error_max_m: 0 or effectively 0`;
    - `beach_continuity_pct: 100%`;
    - `ocean_below_sea_pct: 100%`;
    - `square_edge_ocean_violations: 0`;
    - outer world edge remains ocean/container terrain.
13. 8129-class evidence can be generated locally.
14. 8129-class evidence is represented in Git by manifest, validation metrics, and small previews, not by unbounded binary commits.
15. Runtime mesh/LOD/collision consumers query terrain through the bridge, not by duplicating file parsing.

## Risks And Stop Conditions

### Risks

| Risk | Consequence | Mitigation |
|---|---|---|
| Python mirror becomes de facto authority | Runtime/tool drift | C++ owns math; Python only invokes/exports; parity checks if mirror exists. |
| Large artifacts enter Git history | Repository bloat | `.gitignore`, `.gitattributes`, review checks, manifest-only tracking. |
| Fingerprints include timestamps/local paths | Non-reproducible builds | Canonical config excludes volatile fields. |
| Runtime silently uses stale artifacts | Wrong terrain in-game | Manifest fingerprint checks and fail-fast policy. |
| Heavy passes alter shoreline gates | Regresses proven terrain constraints | Gate checks after every pass; pass toggles; stop on regression. |
| Tile seams crack | Visual/collision artifacts | Shared border samples and seam validation. |
| Landscape assumptions leak into runtime mesh design | Architecture lock-in | Runtime bridge abstracts source grid and artifact format. |
| UE editor-only Python used as runtime dependency | Shipping failure | Python remains editor/tooling only. |

### Stop conditions

Stop implementation and report the blocker if:

- shoreline error increases above allowed epsilon;
- beach continuity drops below 100%;
- ocean-below-sea drops below 100%;
- square-edge ocean violations become nonzero;
- outer world edge is no longer ocean/container terrain;
- generated artifacts cannot be tied to a manifest fingerprint;
- commandlet output is nondeterministic across repeated runs with same config/source;
- runtime bridge loads artifacts whose fingerprints do not match expected config;
- PR includes large generated binaries outside approved LFS/external policy;
- Python output disagrees with commandlet/C++ output and the difference is unexplained.

## Rejected Or Forbidden Approaches

Do not use these approaches:

1. **Python-only terrain authority.**
   - Rejected because runtime C++ and editor tooling will drift.

2. **Runtime global hydrology or erosion rebuilds.**
   - Rejected because expensive global passes should not run during gameplay.

3. **Committing full generated 8129-class artifacts to normal Git.**
   - Rejected because it bloats history and makes review harder.

4. **Unfingerprinted generated assets.**
   - Rejected because stale or mismatched terrain can silently load.

5. **Timestamp/path-dependent fingerprints.**
   - Rejected because they break reproducibility.

6. **Changing terrain gates during architecture migration.**
   - Rejected because the existing world-size, shoreline, beach, ocean, and edge guarantees are preservation requirements.

7. **Ad hoc material channel packing with no metadata.**
   - Rejected because future material/debug consumers need explicit channel semantics.

8. **Player-facing overlays as part of this work.**
   - Rejected by terrain-only scope.

9. **Treating Landscape import as the only runtime path.**
   - Rejected because the project may need a custom runtime terrain shell before or instead of full Landscape import.

10. **Treating generated artifacts as source truth.**
    - Rejected because artifacts are outputs. The source truth is canonical C++ terrain core plus canonical config and source identity.

## Sources And References

- Epic Games, **World Partition in Unreal Engine**. World Partition is an automatic data-management and distance-based streaming system for large worlds, which supports the recommendation to align generated terrain tiles with streaming/grid boundaries.  
  <https://dev.epicgames.com/documentation/unreal-engine/world-partition-in-unreal-engine>

- Epic Games, **Large World Coordinates in Unreal Engine 5**. UE5 Large World Coordinates introduce double-precision core data variants for massive world scale and precision-sensitive placement, supporting `double` world-domain math in the terrain bridge.  
  <https://dev.epicgames.com/documentation/unreal-engine/large-world-coordinates-in-unreal-engine-5>

- Epic Games, **Landscape Technical Guide in Unreal Engine**. UE Landscape components drive rendering, visibility, and collision; the guide lists 8129 × 8129 as a recommended Landscape size and documents 16-bit PNG / `r16` heightmap import and layer debug visualization.  
  <https://dev.epicgames.com/documentation/unreal-engine/landscape-technical-guide-in-unreal-engine>

- Epic Games, **UCommandlet API Reference**. Commandlets run in a raw environment with no loaded game/client/actors, supporting their use for headless terrain generation and validation tools.  
  <https://dev.epicgames.com/documentation/unreal-engine/API/Runtime/Engine/UCommandlet>

- Epic Games, **Scripting the Unreal Editor Using Python**. UE Python is designed for editor automation and production pipelines, and is not available as gameplay scripting in cooked runtime, supporting Python as tooling/export rather than terrain authority.  
  <https://dev.epicgames.com/documentation/unreal-engine/scripting-the-unreal-editor-using-python>

- Epic Games, **One File Per Actor in Unreal Engine**. OFPA reduces source-control overlap by storing actor instances externally and is enabled by default with World Partition, relevant to repository hygiene around large-world UE content.  
  <https://dev.epicgames.com/documentation/unreal-engine/one-file-per-actor-in-unreal-engine>

- Epic Games, **Set up Zen Storage Server as Shared DDC for Unreal Engine**. Epic documents Zenserver as an unauthenticated shared DDC intended for trusted LAN/VPN-like environments, supporting the warning not to expose cache/artifact services publicly.  
  <https://dev.epicgames.com/documentation/unreal-engine/set-up-zen-storage-server-as-shared-ddc-for-unreal-engine>

- Git LFS, **Git Large File Storage**. Git LFS replaces large file contents with text pointers in Git and stores file contents on a remote server, supporting optional LFS use for curated binary artifacts rather than normal Git commits.  
  <https://git-lfs.com/>

- GitHub Docs, **About Git Large File Storage**. GitHub documents LFS as the mechanism for tracking files beyond normal repository size limits.  
  <https://docs.github.com/repositories/working-with-files/managing-large-files/about-git-large-file-storage>

- GDAL, **Raster Data Model**. GDAL’s dataset/raster-band model formalizes raster size, bands, metadata, georeferencing transform, and coordinate-system concepts, supporting manifest fields for height/mask raster artifacts.  
  <https://gdal.org/en/stable/user/raster_data_model.html>

- GDAL, **Geotransform Tutorial**. GDAL geotransforms define affine conversion from pixel/line coordinates to georeferenced space, supporting explicit world/grid coordinate metadata for source heightfields and masks.  
  <https://gdal.org/en/stable/tutorials/geotransforms_tut.html>
