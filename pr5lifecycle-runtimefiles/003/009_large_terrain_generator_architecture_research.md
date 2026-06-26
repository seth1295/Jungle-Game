# Large Terrain Generator Architecture Research

## Executive Summary

PR5 Batch 003 should convert Jungle Game terrain generation from a proof-of-concept runtime shell actor into a source-owned, deterministic terrain generation stack. The shell actor should stop being the architecture. It should become a host that selects a terrain config, requests generation, receives a canonical heightfield/channel result, and builds runtime render output.

The generator architecture should be organized around six owned layers:

1. **Config and identity** — source-controlled terrain config structs/data assets, algorithm versions, seed derivation, schema versioning, canonical config hashing, and build metadata.
2. **Deterministic sampling** — coordinate conversion, stable noise/sampling primitives, named seed streams, quantization policy, and no global RNG.
3. **Feature functions** — island/coast invariants, massif, ridges, gullies/catchments, crater, lava/crust, and hard blockers as explicit composable terrain layers.
4. **Masks/channels** — scalar grids and compact per-sample metadata that every feature can read and write through typed channels, not ad hoc booleans scattered through actor code.
5. **Heightfield/runtime output** — canonical heightfield first, then mesh/Landscape/runtime component output as consumers.
6. **Validation/export** — deterministic report generation, topographic stats, coast metadata, validation failures, artifact manifests, and runtime log tokens.

The critical architecture invariant is that **island mask and sea-level beach ring are not optional post-effects**. They are first-class terrain fields generated before major feature composition. Every elevation feature must blend against island/coast constraints, and every export/log/report must carry coast and sea-level metadata.

The correct implementation shape is a terrain domain module with pure C++ generator classes, typed result structs, and a thin actor/component adapter. It should explicitly avoid foliage, ecology, weather, survival mechanics, NPCs, story, VFX, final materials, asset packs, and player-facing UI.

## Terrain-Only Scope Boundary

This batch is terrain-generator source architecture only. It may implement or refactor:

- deterministic heightfield sampling;
- terrain config and version identity;
- island mask, beach ring, sea-level metadata, and square-ocean containment;
- massif, ridge, gully/catchment, crater, lava/crust, and hard-blocker terrain feature functions;
- typed mask/channel grids;
- runtime mesh or Landscape-compatible heightfield output;
- validation reports, debug exports, logs, and tests;
- editor-only debug commands that export terrain artifacts.

It must not design or implement:

- foliage placement, ecology, biomes, animal systems, weather, audio, survival mechanics, NPCs, story, VFX, final art direction, asset packs, or player UI;
- player-facing map/minimap/compass/GPS/objective/bearing/time/orientation UI;
- gameplay damage, navigation, encounter spawning, inventory, crafting, questing, hazard effects, or NPC behavior;
- final material/shader work beyond simple debug visualization channels needed to validate terrain.

A good test for scope: if the change is needed to deterministically generate, display, validate, or export terrain geometry/masks, it belongs. If it answers “what lives there,” “what the player does there,” or “how the player navigates there,” it does not.

## Current Source Baseline

The prompt baseline states that Jungle Game currently has terrain generation embedded in a runtime shell actor. That actor proved the concept but is now the wrong ownership boundary for a scaled terrain generator.

Expected current problems:

- actor lifecycle, rendering, generation logic, config defaults, and debug output are likely coupled;
- terrain feature order is probably implicit in function order rather than declared as a composition graph;
- island/coast behavior risks being a late clamp or cosmetic mask instead of a core invariant;
- seed/config identity may not be reproducible across builds;
- terrain channels may be implicit temporary arrays, debug colors, or local booleans rather than named outputs;
- validation probably depends on visual inspection rather than exported evidence.

The implementation should first identify the existing shell actor and preserve its external role while moving generation responsibilities out of it. The actor should become an adapter, not the terrain source of truth.

Suggested transition names, adapted to the project’s existing naming style:

| Current responsibility | Target owner | Notes |
|---|---|---|
| Runtime actor owns generation parameters | `UJGTerrainGeneratorConfig` plus `FJGTerrainConfig` | Config is source-controlled and hashable. |
| Runtime actor samples noise directly | `FJGTerrainGenerator` | Pure C++; no `UWorld` dependency. |
| Runtime actor composes features | `FJGTerrainFeatureComposer` | Explicit pass graph and channel reads/writes. |
| Runtime actor builds mesh | `UJGTerrainRuntimeComponent` or `FJGTerrainMeshBuilder` | Rendering output consumes canonical heightfield. |
| Runtime actor logs debug values | `FJGTerrainValidationReport` / `FJGTerrainExportService` | Logs and exports use shared metadata. |

## Major Implementation Goal

The major implementation goal is to create a deterministic terrain generator that can support a large island terrain with sea-level coast behavior, a 1400 m volcanic massif, ridges, catchments, crater hazards, masks/channels, exports, and validation without turning into a giant actor or a pile of special cases.

The architecture should establish these durable guarantees:

- **Canonical source**: the generated heightfield and channel set are the truth; runtime mesh and editor exports are derived outputs.
- **Deterministic identity**: terrain builds are reproducible from config schema version, algorithm version, seed, source revision, and canonical config hash.
- **Coast-first composition**: island mask, sea level, coast distance, and beach ring are computed before terrain features and constrain all feature blending.
- **Typed channels**: masks and channels are named data products with ownership and export rules.
- **Composable features**: massif/ridge/gully/crater/lava/blocker features return typed contributions; they do not mutate actor fields directly.
- **Validation as a product**: every build can produce JSON/Markdown/CSV summaries and optional raster outputs for review.
- **Runtime thinness**: the shell actor requests and displays terrain; it does not own terrain math.

A successful batch may be large. It should not be reduced to tiny cosmetic patches that merely rearrange code while leaving the generator embedded in the shell actor.

## Terrain Generator Ownership Model

### Recommended module layout

Use a dedicated terrain source area under the main game module unless the project already has plugin/module boundaries.

```text
Source/JungleGame/
  Public/Terrain/
    JGTerrainTypes.h
    JGTerrainConfig.h
    JGTerrainBuildMetadata.h
    JGTerrainChannels.h
    JGTerrainSampling.h
    JGTerrainFeatures.h
    JGTerrainGenerator.h
    JGTerrainMeshBuilder.h
    JGTerrainValidation.h
    JGTerrainExport.h
    JGTerrainRuntimeComponent.h
    JGTerrainGeneratorActor.h

  Private/Terrain/
    JGTerrainConfig.cpp
    JGTerrainBuildMetadata.cpp
    JGTerrainChannels.cpp
    JGTerrainSampling.cpp
    JGTerrainFeatures.cpp
    JGTerrainGenerator.cpp
    JGTerrainMeshBuilder.cpp
    JGTerrainValidation.cpp
    JGTerrainExport.cpp
    JGTerrainRuntimeComponent.cpp
    JGTerrainGeneratorActor.cpp

  Private/Tests/
    JGTerrainDeterminismTests.cpp
    JGTerrainCoastInvariantTests.cpp
    JGTerrainValidationTests.cpp
```

If the project uses plugins, the cleaner long-term split is:

```text
Plugins/JungleTerrain/Source/JungleTerrainRuntime/
Plugins/JungleTerrain/Source/JungleTerrainEditor/
```

For PR5 Batch 003, a game-module terrain folder is acceptable if plugin extraction would slow the terrain architecture work. Do not block the batch on plugin purity.

### Core ownership rules

| Layer | Owner | Allowed dependencies | Forbidden dependencies |
|---|---|---|---|
| Config structs | `FJGTerrainConfig`, `UJGTerrainGeneratorConfig` | Core types, DataAsset if needed | Actor lifecycle, player systems |
| Sampling | `FJGTerrainSampler`, `FJGTerrainNoise` | Math/Core only | `UWorld`, actor state, random globals |
| Feature functions | `FJGTerrainFeatureComposer` and feature structs | Config, sampling, channels | Mesh component, materials, gameplay |
| Heightfield result | `FJGTerrainHeightfield`, `FJGTerrainChannelSet` | Core containers | Actor, world, UI |
| Runtime mesh | `FJGTerrainMeshBuilder`, `UJGTerrainRuntimeComponent` | Mesh component APIs | Terrain feature ownership |
| Validation/export | `FJGTerrainValidator`, `FJGTerrainExportService` | File helpers, JSON, automation | Gameplay/environment systems |
| Actor | `AJGTerrainGeneratorActor` | Runtime component, config asset | Direct feature math |

### Recommended class responsibilities

#### `FJGTerrainBuildRequest`

Immutable request passed to the generator.

```cpp
struct FJGTerrainBuildRequest
{
    FJGTerrainConfig Config;
    FJGTerrainBuildMetadata Metadata;
    FIntPoint Resolution;
    double WorldSizeMeters = 8192.0;
    double SeaLevelMeters = 0.0;
    bool bGenerateValidation = true;
    bool bGenerateExportArtifacts = false;
};
```

#### `FJGTerrainBuildResult`

Canonical result returned by the generator.

```cpp
struct FJGTerrainBuildResult
{
    FJGTerrainBuildMetadata Metadata;
    FJGTerrainHeightfield Heightfield;
    FJGTerrainChannelSet Channels;
    FJGTerrainValidationReport Validation;
    TArray<FJGTerrainFeatureContributionSummary> FeatureSummaries;
};
```

#### `FJGTerrainGenerator`

Pure generator. It should not derive from `UObject` unless there is a strong engine integration reason. A simple C++ class is easier to test.

```cpp
class FJGTerrainGenerator
{
public:
    FJGTerrainBuildResult Generate(const FJGTerrainBuildRequest& Request) const;
};
```

#### `UJGTerrainGeneratorConfig`

A Data Asset wrapper around source-owned config. Unreal’s asset management model distinguishes Primary and Secondary Assets and allows Primary Assets to be addressed by ID, which is useful for stable config identity and editor selection. Keep the canonical hash based on the normalized config fields, not object pointer identity.

```cpp
UCLASS(BlueprintType)
class UJGTerrainGeneratorConfig : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, Category="Terrain")
    FJGTerrainConfig Config;

    virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};
```

Use this only as an editor/source-controlled config surface. Do not let runtime mutation silently change the build identity.

#### `AJGTerrainGeneratorActor`

Thin world host.

```cpp
UCLASS()
class AJGTerrainGeneratorActor : public AActor
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, Category="Terrain")
    TObjectPtr<UJGTerrainGeneratorConfig> TerrainConfig;

    UPROPERTY(VisibleAnywhere, Category="Terrain")
    TObjectPtr<UJGTerrainRuntimeComponent> TerrainRuntimeComponent;

    UFUNCTION(CallInEditor, Category="Terrain")
    void RebuildTerrain();
};
```

The actor may:

- select a config;
- request generation;
- send the result to runtime mesh/Landscape output;
- write a single build log token;
- expose editor rebuild buttons.

The actor must not:

- sample terrain noise directly;
- own island/coast math;
- store hidden feature state;
- decide terrain pass order inline;
- implement validation rules directly.

## Config / Seed / Version Identity

### Config ownership

Use a two-level config model:

1. **Canonical source struct**: `FJGTerrainConfig` and nested structs live in C++ and are serializable in a stable order.
2. **Editor config asset**: `UJGTerrainGeneratorConfig` wraps the struct for selecting variants in the editor.

Suggested config structs:

```cpp
struct FJGTerrainConfig
{
    FName ConfigId = "JungleTerrain.MainIsland";
    int32 ConfigVersionMajor = 0;
    int32 ConfigVersionMinor = 3;

    FJGTerrainDomainConfig Domain;
    FJGIslandCoastConfig IslandCoast;
    FJGMassifConfig Massif;
    FJGRidgeConfig Ridges;
    FJGCatchmentConfig Catchments;
    FJGCraterConfig Crater;
    FJGLavaCrustConfig LavaCrust;
    FJGHardBlockerConfig HardBlockers;
    FJGTerrainOutputConfig Output;
    FJGTerrainValidationConfig Validation;
};
```

Config fields should use meters, normalized 0..1 masks, explicit enum modes, and named defaults. Avoid magic numbers inside feature functions.

### Identity fields

Every terrain build should carry:

| Field | Example | Purpose |
|---|---:|---|
| `terrain_schema` | `terrain.v3` | Breaks compatibility intentionally. |
| `algorithm_version` | `JG-TerrainGen-0.3.0` | Identifies terrain math revision. |
| `config_id` | `JungleTerrain.MainIsland` | Human-readable config identity. |
| `config_version` | `0.3` | Config semantic version. |
| `config_hash` | `sha256:9e4f...` | Rebuild identity from canonical config serialization. |
| `root_seed` | `0x53A7C041` | Seed for named stream derivation. |
| `source_revision` | Git commit or `unknown` | Debug reproducibility. |
| `world_size_m` | `8192` | Physical domain size. |
| `resolution` | `4097x4097` | Heightfield grid identity. |
| `sea_level_m` | `0.0` | Coast invariant metadata. |
| `coast_schema` | `island-mask.v1/beach-ring.v1` | Coast math identity. |
| `export_timestamp_utc` | ISO-8601 | Artifact timestamp, not deterministic identity. |

### Canonical config hash

The config hash should be computed from a normalized, sorted, stable representation. Avoid raw memory hashing because padding, platform endianness, or float representation details may change.

Recommended policy:

- serialize to canonical JSON with stable key order;
- convert floats to fixed decimal precision or quantized integer millimeters where appropriate;
- include schema and algorithm version;
- exclude editor-only object path unless intentionally part of identity;
- include named feature versions;
- use SHA-256 or another stable project-approved hash.

Example canonical identity object:

```json
{
  "terrain_schema": "terrain.v3",
  "algorithm_version": "JG-TerrainGen-0.3.0",
  "config_id": "JungleTerrain.MainIsland",
  "config_version": "0.3",
  "root_seed": 1403506753,
  "domain": { "world_size_m": 8192, "resolution_x": 4097, "resolution_y": 4097 },
  "sea_level_m": 0,
  "coast_schema": "island-mask.v1/beach-ring.v1",
  "feature_versions": {
    "massif": "v1",
    "ridges": "v1",
    "catchments": "v1",
    "crater": "v1",
    "lava_crust": "v1",
    "hard_blockers": "v1"
  }
}
```

### Seed handling

Unreal has `FRandomStream` for repeatable random streams, including initialization from a seed/name, and Random Streams are documented as a repeatable random mechanism for procedural distributions. That is appropriate for named stream derivation and editor-facing deterministic knobs, but the terrain result should not depend on actor construction order or global random state.

Recommended seed rules:

- one root seed in `FJGTerrainConfig`;
- derive feature seeds by hashing `root_seed + feature_name + feature_version`;
- never use `FMath::Rand`, `FMath::FRand`, wall-clock time, object pointer addresses, or unordered map iteration order;
- do not let thread scheduling change results;
- if parallel generation is used, tile/thread work must derive seeds from stable tile coordinates, not task order;
- quantize final exported height values deterministically;
- use tests that compare hash of generated heightfield/channel outputs.

Example named stream derivation:

```cpp
uint32 FJGTerrainSeed::Derive(uint32 RootSeed, FStringView FeatureName, FStringView FeatureVersion)
{
    return CityHash32OrProjectHash(RootSeed, FeatureName, FeatureVersion);
}
```

Use the project’s existing hash utility if available. Do not invent a new crypto dependency unless needed.

## Feature Function Composition Strategy

### Composition principle

Terrain features should be pure functions over a shared terrain domain. Each feature receives:

- immutable config;
- sample coordinates;
- deterministic sampler/noise state;
- read-only access to previously declared invariant channels;
- optional read-only access to earlier terrain elevation where the feature logically depends on it;
- explicit writable contribution fields.

Features should return a contribution, not mutate actor state.

Suggested per-sample contribution:

```cpp
struct FJGTerrainSampleContribution
{
    double ElevationDeltaMeters = 0.0;
    double ElevationOverrideMeters = TNumericLimits<double>::QuietNaN();
    double Weight = 1.0;
    TStaticArray<float, static_cast<int32>(EJGTerrainChannel::Count)> ChannelWrites;
};
```

For grid-level passes, use typed arrays instead of per-sample virtual dispatch if performance matters. The abstraction is conceptual; hot loops can be plain functions.

### Recommended pass order

1. **Domain setup**
   - world coordinates;
   - normalized coordinates;
   - tile index;
   - radial distance from island center;
   - square-edge ocean containment distance.

2. **Island/coast invariant pass**
   - island mask;
   - coast distance;
   - beach ring weight;
   - ocean/container mask;
   - sea-level clamp metadata.

3. **Base landform pass**
   - interior base elevation envelope;
   - offshore bathymetry envelope;
   - broad-scale slope toward coast;
   - no detailed features yet.

4. **Massif pass**
   - 1400 m volcanic massif target envelope;
   - summit/crater relationship;
   - massif radial falloff;
   - cliff/slope budget channels.

5. **Ridge pass**
   - ridge fields anchored to massif/interior;
   - ridge masks fade out near beach ring and hard ocean edge;
   - ridge sharpness/height contribution.

6. **Catchment/gully pass**
   - drainage potential, flow accumulation or synthetic catchment channels;
   - gully incision constrained by slope, catchment mask, and coast buffer;
   - prevent beach-ring destruction unless explicitly allowed by coastal outlet rules.

7. **Crater/lava/crust pass**
   - crater depression/rim if terrain-affecting;
   - lava/crust masks as terrain channels;
   - hazard mask output only, not gameplay hazard behavior.

8. **Hard-blocker pass**
   - terrain-only blockers such as non-buildable cliffs, unsafe crater zones, or impassable terrain channels;
   - these are geometry/mask blockers, not player-system blockers.

9. **Post-composition invariant enforcement**
   - re-apply sea-level/coast contract;
   - validate no feature lifted square-world edge into land identity;
   - validate beach ring continuity and elevation tolerance;
   - clamp only through named invariant code, not hidden feature clamps.

### Feature API shape

```cpp
class FJGTerrainFeatureComposer
{
public:
    void BuildInvariantChannels(FJGTerrainBuildContext& Context) const;
    void ApplyBaseLandform(FJGTerrainBuildContext& Context) const;
    void ApplyMassif(FJGTerrainBuildContext& Context) const;
    void ApplyRidges(FJGTerrainBuildContext& Context) const;
    void ApplyCatchmentsAndGullies(FJGTerrainBuildContext& Context) const;
    void ApplyCraterAndLavaCrust(FJGTerrainBuildContext& Context) const;
    void ApplyHardBlockers(FJGTerrainBuildContext& Context) const;
    void EnforceFinalInvariants(FJGTerrainBuildContext& Context) const;
};
```

Each pass should emit a `FJGTerrainFeatureContributionSummary`:

```cpp
struct FJGTerrainFeatureContributionSummary
{
    FName FeatureName;
    FString FeatureVersion;
    uint32 DerivedSeed;
    double MinDeltaMeters;
    double MaxDeltaMeters;
    double MeanDeltaMeters;
    double ActiveCoverage01;
    TArray<FName> ReadChannels;
    TArray<FName> WrittenChannels;
};
```

### Avoiding spaghetti

Use these design rules:

- Every feature function must declare read channels and write channels.
- No feature may directly query actor properties.
- No feature may silently clamp to sea level; coast enforcement lives in the invariant layer.
- No feature may write arbitrary debug textures; all debug data goes through channel/export services.
- No feature may introduce a gameplay concept. `CraterHazardMask` is allowed as a terrain channel; actual player damage is not.
- No feature may own mesh generation.
- No mesh builder may alter canonical elevation except through documented quantization.
- No config default may exist only in a `.cpp` local literal if it affects terrain identity.

## Island Mask And Sea-Level Invariant

### Invariant statement

The island perimeter contract is the most important design constraint:

```text
island mask and sea-level beach ring are not optional post-effects
all terrain features blend against island/coast constraints
square-world edge remains ocean/container, not terrain identity
all exports and runtime logs include sea-level/coast metadata
```

This should be encoded as a generator-level invariant, not as comments around a feature.

### Required coast channels

At minimum, generate these channels before any major terrain feature pass:

| Channel | Range/unit | Meaning |
|---|---:|---|
| `IslandMask` | 0..1 | 1 inside terrain island, 0 ocean/container. |
| `SquareOceanMask` | 0..1 | 1 where square-world edge must remain ocean/container. |
| `CoastDistanceMeters` | meters | Signed/unsigned distance from coastline; define convention explicitly. |
| `BeachRingMask` | 0..1 | First-class beach blending ring near sea level. |
| `SeaLevelConstraintMask` | 0..1 | Where final elevation must remain near sea level. |
| `OceanDepthMeters` | meters | Bathymetry depth outside island, if generated. |
| `InteriorTerrainMask` | 0..1 | Where major terrain features are allowed to fully express. |

Suggested convention:

- `CoastDistanceMeters < 0` offshore;
- `CoastDistanceMeters = 0` coastline;
- `CoastDistanceMeters > 0` inland;
- `BeachRingMask = 1` across the intended beach band;
- `InteriorTerrainMask` ramps from 0 at beach ring to 1 inland.

### Coast generation algorithm shape

The island/coast pass should combine:

- an analytic island base shape, such as radial/squircle/superellipse mask;
- deterministic low-frequency coastline distortion;
- square-world ocean containment mask that forces edges to water;
- beach width and sea-level tolerance config;
- minimum ocean margin to prevent terrain identity at the square boundary.

Pseudo-code:

```cpp
for each sample:
    FJGTerrainDomainPoint P = Domain.FromGrid(X, Y);

    double islandField = EvaluateIslandField(P.NormalizedXY, CoastConfig);
    double squareEdgeField = EvaluateSquareOceanContainer(P.NormalizedXY, CoastConfig);

    double coastDistanceM = ConvertFieldToApproxDistance(islandField, CoastConfig);
    double islandMask = SmoothStep(0.0, CoastConfig.CoastBlendMeters, coastDistanceM);
    double beachMask = BandPass(coastDistanceM,
                               CoastConfig.BeachInnerMeters,
                               CoastConfig.BeachOuterMeters,
                               CoastConfig.BeachSoftnessMeters);

    islandMask *= squareEdgeField;

    Channels.Write(IslandMask, islandMask);
    Channels.Write(CoastDistanceMeters, coastDistanceM);
    Channels.Write(BeachRingMask, beachMask);
    Channels.Write(SeaLevelConstraintMask, max(beachMask, 1.0 - islandMask));
    Channels.Write(InteriorTerrainMask, SmoothStep(CoastConfig.InteriorStartMeters,
                                                   CoastConfig.InteriorFullMeters,
                                                   coastDistanceM));
```

### Elevation contract

The final elevation function should express coast rules explicitly:

```cpp
double FJGTerrainCoastInvariants::ApplyFinalCoastConstraint(
    double RawElevationMeters,
    const FJGTerrainCoastSample& Coast,
    const FJGIslandCoastConfig& Config)
{
    const double BeachTarget = Config.SeaLevelMeters + Config.BeachElevationOffsetMeters;
    const double BeachElevation = Lerp(RawElevationMeters, BeachTarget, Coast.BeachRingMask);
    const double OceanElevation = Min(BeachElevation, Config.SeaLevelMeters - Coast.OceanDepthMeters);
    return Lerp(OceanElevation, BeachElevation, Coast.IslandMask);
}
```

Final details will differ, but the ownership is the point: coast constraint is a named invariant function with tests.

### Required coast validation

Validation should fail if:

- any square-world edge sample has land identity above configured tolerance;
- beach ring is missing, discontinuous beyond configured gap tolerance, or not exported;
- sea-level beach ring elevation exceeds tolerance;
- major features are active in forbidden coast/ocean/container zones;
- exports/logs omit `sea_level_m`, `coast_schema`, `beach_width_m`, `ocean_margin_m`, or `coast_validation` result;
- massif/ridge/crater/gully features bypass the invariant pass.

Recommended validation metrics:

```text
coast.edge_land_pixels=0
coast.beach_ring_coverage=0.083
coast.beach_elevation_rmse_m=0.41
coast.beach_elevation_max_abs_error_m=1.92
coast.square_edge_ocean_min_margin_m=512
coast.feature_leak_pixels=0
coast.invariant_status=PASS
```

## Heightfield / Runtime Mesh Output Strategy

### Canonical heightfield first

The generator should produce a canonical `FJGTerrainHeightfield` independent of how terrain is rendered.

```cpp
struct FJGTerrainHeightfield
{
    FIntPoint Resolution;
    double WorldSizeMeters;
    double SeaLevelMeters;
    TArray<float> HeightMeters; // row-major, deterministic quantization policy documented

    float GetHeight(int32 X, int32 Y) const;
    void SetHeight(int32 X, int32 Y, float HeightMeters);
};
```

Runtime mesh, editor Landscape import, debug previews, and exports all consume this heightfield. They do not own terrain generation.

### Runtime mesh options

Two Unreal-side runtime mesh paths are relevant:

- `UProceduralMeshComponent`: allows custom triangle mesh geometry but Epic marks the feature experimental in the API documentation. It can be acceptable for a project prototype/runtime shell if already used, but the architecture should isolate it behind `FJGTerrainMeshBuilder`.
- `UDynamicMeshComponent`: stores renderable geometry via `UDynamicMesh`/`FDynamicMesh3` and has support for partial render buffer updates and chunk decomposition. It is a stronger long-term candidate if the project is already using UE Geometry Framework.

Do not let either component become the terrain source of truth.

Recommended adapter:

```cpp
class FJGTerrainMeshBuilder
{
public:
    FJGTerrainMeshBuildData BuildMeshData(const FJGTerrainHeightfield& Heightfield,
                                          const FJGTerrainChannelSet& Channels,
                                          const FJGTerrainMeshBuildConfig& Config) const;
};
```

Runtime component:

```cpp
UCLASS(ClassGroup=(Terrain), meta=(BlueprintSpawnableComponent))
class UJGTerrainRuntimeComponent : public USceneComponent
{
    GENERATED_BODY()

public:
    void ApplyTerrainBuildResult(const FJGTerrainBuildResult& BuildResult);
};
```

### Chunking guidance

For large terrain, implement chunking at the output layer and optionally at generation time, but keep deterministic domain sampling global.

Chunk rules:

- tile/chunk coordinates are part of deterministic sampling;
- feature seeds do not depend on task order;
- adjacent chunks share sample edges or deterministic seam policy;
- validation can run globally or per-tile plus aggregate;
- exports include tile manifests.

Suggested chunk metadata:

```cpp
struct FJGTerrainTileId
{
    int32 TileX;
    int32 TileY;
    int32 LOD;
};
```

### Landscape-compatible path

If the project wants UE Landscape output, keep it as a consumer/export path. Landscape has strict valid heightmap dimension rules and component/section architecture, so heightfield resolution should either target known valid sizes or export a conversion step that pads/resamples deterministically.

Recommended policy:

- canonical generator resolution can be project-owned;
- Landscape export validates against UE Landscape valid dimensions;
- conversion from canonical heightfield to Landscape heightmap must be deterministic and logged;
- no Landscape-specific constraints should leak back into terrain feature math unless intentionally configured.

### Height quantization

Use explicit quantization for export and test hash stability:

| Output | Suggested representation | Notes |
|---|---|---|
| Runtime canonical | `float` meters after deterministic computation | Keep canonical enough for mesh. |
| Validation stats | `double` accumulation | Avoid stat drift. |
| Heightmap export | `uint16` or RAW/R16 with documented min/max | Required for external tools/Landscape import. |
| JSON samples | rounded meters | Avoid huge output. |
| Hash | quantized byte buffer | Stable across platforms. |

## Terrain Masks And Channel Architecture

### Channel ownership

Channels should be explicit, typed, versioned data products. Do not treat them as debug leftovers.

```cpp
enum class EJGTerrainChannel : uint8
{
    IslandMask,
    SquareOceanMask,
    CoastDistanceMeters,
    BeachRingMask,
    SeaLevelConstraintMask,
    OceanDepthMeters,
    InteriorTerrainMask,

    BaseElevationMeters,
    MassifInfluence,
    MassifSlopeBudget,
    RidgeMask,
    RidgeStrength,
    CatchmentId,
    FlowAccumulation,
    GullyIncision,
    CraterMask,
    CraterRimMask,
    LavaCrustMask,
    HardBlockerMask,
    SlopeDegrees,
    Curvature,

    Count
};
```

`FJGTerrainChannelSet` should own channel arrays, metadata, and export behavior.

```cpp
struct FJGTerrainChannelDesc
{
    EJGTerrainChannel Channel;
    FName Name;
    FName Unit;
    float MinExpected;
    float MaxExpected;
    bool bExportByDefault;
    FString ProducerPass;
    FString Description;
};

class FJGTerrainChannelSet
{
public:
    void Allocate(FIntPoint Resolution, TConstArrayView<FJGTerrainChannelDesc> Channels);
    TArrayView<float> Mutable(EJGTerrainChannel Channel);
    TConstArrayView<float> Read(EJGTerrainChannel Channel) const;
    const FJGTerrainChannelDesc& Describe(EJGTerrainChannel Channel) const;
};
```

### Channel categories

| Category | Examples | Export default | Purpose |
|---|---|---:|---|
| Invariant | `IslandMask`, `BeachRingMask`, `CoastDistanceMeters` | Yes | Prove coast contract. |
| Elevation | `BaseElevationMeters`, final heightfield | Yes | Validate topography. |
| Feature | `MassifInfluence`, `RidgeMask`, `GullyIncision` | Yes for Batch 003 | Debug composition. |
| Hazard terrain | `CraterMask`, `LavaCrustMask` | Yes | Terrain-only hazardous zones, no gameplay. |
| Constraint | `HardBlockerMask`, `MassifSlopeBudget` | Yes | Prevent invalid terrain. |
| Derived analytic | `SlopeDegrees`, `Curvature`, `FlowAccumulation` | Optional but useful | Validate plausibility. |

### Mask composition rules

- `IslandMask` multiplies land features.
- `InteriorTerrainMask` controls feature expression strength.
- `BeachRingMask` pulls final elevation toward sea-level beach target.
- `SquareOceanMask` has final authority at world edges.
- `HardBlockerMask` should be a channel/result of terrain analysis, not a hidden list of actor bounds.
- `CraterMask` and `LavaCrustMask` are terrain annotations until gameplay explicitly owns them in a later batch.
- Channel writes must be documented by pass.

### Mask/channel export naming

Use stable names in artifacts:

```text
terrain_009_<config_id>_<config_hash_short>/
  manifest.json
  height_final_m.r16
  channel_island_mask.png
  channel_beach_ring_mask.png
  channel_coast_distance_m.exr
  channel_massif_influence.png
  channel_ridge_mask.png
  channel_flow_accumulation.exr
  channel_crater_mask.png
  channel_lava_crust_mask.png
  channel_hard_blocker_mask.png
  validation_report.json
  validation_report.md
  feature_summary.csv
  build.logtoken.txt
```

Avoid names like `debug1.png`, `mask_final_final.png`, or `temp_coast.png`.

## Export / Topographic Validation Architecture

### Validation as a build stage

Validation should run after final invariant enforcement and before runtime result acceptance. It should be possible to run validation in editor, automation tests, and commandlet-style workflows.

```cpp
class FJGTerrainValidator
{
public:
    FJGTerrainValidationReport Validate(const FJGTerrainBuildResult& Result,
                                        const FJGTerrainValidationConfig& Config) const;
};
```

Suggested report shape:

```cpp
struct FJGTerrainValidationReport
{
    bool bPassed = false;
    TArray<FJGTerrainValidationIssue> Issues;
    FJGTerrainCoastValidationMetrics Coast;
    FJGTerrainElevationMetrics Elevation;
    FJGTerrainFeatureValidationMetrics Features;
    FString StableResultHash;
};
```

### Required validation areas

#### Coast invariants

- square edge remains ocean/container;
- beach ring exists and is continuous;
- sea-level tolerance within configured values;
- coast channels exported and logged;
- features do not leak into forbidden coast/ocean bands.

#### Elevation envelope

- massif reaches intended maximum near 1400 m within tolerance;
- no impossible spikes or NaNs;
- slope limits are within configured budget except deliberate cliffs/rims;
- ocean elevation remains below sea level except beach/shore transition;
- heightfield min/max/mean are logged.

#### Feature composition

- massif, ridges, catchments/gullies, crater, lava/crust, and hard blockers each produce expected channel coverage;
- no feature silently overrides another without summary metadata;
- channel ranges are within expected units and normalized bounds;
- feature coverage is neither zero nor global unless intended by config.

#### Determinism

- same request produces same stable result hash;
- tile generation and whole-map generation match at shared samples;
- output order is deterministic;
- named seed streams are recorded.

### Export API

```cpp
class FJGTerrainExportService
{
public:
    bool ExportBuildArtifacts(const FJGTerrainBuildResult& Result,
                              const FJGTerrainExportConfig& ExportConfig,
                              FJGTerrainExportManifest& OutManifest) const;
};
```

Recommended export config:

```cpp
struct FJGTerrainExportConfig
{
    FString OutputRoot;
    bool bWriteManifestJson = true;
    bool bWriteValidationMarkdown = true;
    bool bWriteHeightR16 = true;
    bool bWriteChannelPNGs = true;
    bool bWriteFloatChannels = true;
    bool bWriteFeatureSummaryCsv = true;
    bool bWriteLogToken = true;
};
```

Unreal’s `FFileHelper` provides file load/save helpers in `Misc/FileHelper.h`, and Unreal’s JSON modules can be used for manifest/report serialization. Keep file export in one service so the actor does not scatter write calls across terrain generation.

### Runtime log token format

Every generation should log one compact machine-readable token and optionally a pretty summary.

Recommended single-line token:

```text
JG_TERRAIN_BUILD terrain_schema=terrain.v3 algorithm_version=JG-TerrainGen-0.3.0 config_id=JungleTerrain.MainIsland config_version=0.3 config_hash=sha256:9e4f3a7c root_seed=0x53A7C041 source_rev=abc1234 world_size_m=8192 resolution=4097x4097 sea_level_m=0 coast_schema=island-mask.v1/beach-ring.v1 beach_width_m=45 ocean_margin_m=512 massif_peak_m=1400 max_height_m=1401.8 min_height_m=-96.2 coast_status=PASS validation=PASS result_hash=sha256:27b8c0a2 exports=manifest.json
```

Required token fields:

- `terrain_schema`
- `algorithm_version`
- `config_id`
- `config_hash`
- `root_seed`
- `resolution`
- `world_size_m`
- `sea_level_m`
- `coast_schema`
- `beach_width_m`
- `ocean_margin_m`
- `massif_peak_m`
- `coast_status`
- `validation`
- `result_hash`

Do not bury sea-level/coast metadata in verbose logs only.

### Automation tests

Unreal’s Automation Test Framework is intended for unit, feature, and content stress testing. This terrain stack should use automation tests for deterministic pure C++ pieces.

Suggested tests:

```text
JungleGame.Terrain.ConfigHash.StableForEquivalentConfig
JungleGame.Terrain.Seed.DerivesNamedFeatureStreams
JungleGame.Terrain.Coast.SquareEdgeAlwaysOcean
JungleGame.Terrain.Coast.BeachRingNearSeaLevel
JungleGame.Terrain.Features.MassifPeakWithinTolerance
JungleGame.Terrain.Features.RidgesFadeBeforeBeach
JungleGame.Terrain.Features.GulliesDoNotDestroyBeachRing
JungleGame.Terrain.Export.ManifestContainsCoastMetadata
JungleGame.Terrain.Determinism.SameRequestSameResultHash
JungleGame.Terrain.Determinism.TiledMatchesUntiledSharedEdges
```

Unreal’s Data Validation plugin can also validate assets with custom rulesets, which is useful for checking terrain config assets before generation.

## UE5 Files And Classes Likely Touched

The exact current project filenames must be confirmed from the repo before patching. Based on the prompt, the likely touched areas are below.

### Build files

```text
Source/JungleGame/JungleGame.Build.cs
```

Likely dependency additions, depending on chosen output/export path:

```csharp
PublicDependencyModuleNames.AddRange(new [] {
    "Core",
    "CoreUObject",
    "Engine"
});

PrivateDependencyModuleNames.AddRange(new [] {
    "Json",
    "JsonUtilities",
    "Projects"
});

// Only if used:
PrivateDependencyModuleNames.AddRange(new [] {
    "ProceduralMeshComponent",
    "GeometryCore",
    "GeometryFramework"
});

// Editor-only or separate editor module if used:
PrivateDependencyModuleNames.AddRange(new [] {
    "DataValidation",
    "UnrealEd"
});
```

Keep editor-only modules out of runtime builds unless the project already has an editor module.

### Existing actor/component files

Likely existing files to modify/replace:

```text
Source/JungleGame/Public/.../TerrainShellActor.h
Source/JungleGame/Private/.../TerrainShellActor.cpp
```

Refactor path:

- keep actor spawn behavior stable;
- move config defaults into `FJGTerrainConfig`;
- move terrain math into `FJGTerrainGenerator` and feature composer;
- move mesh construction into `FJGTerrainMeshBuilder`/`UJGTerrainRuntimeComponent`;
- keep old actor name as wrapper only if existing maps reference it;
- add deprecation comments only if needed.

### New terrain source files

```text
Source/JungleGame/Public/Terrain/JGTerrainTypes.h
Source/JungleGame/Public/Terrain/JGTerrainConfig.h
Source/JungleGame/Public/Terrain/JGTerrainBuildMetadata.h
Source/JungleGame/Public/Terrain/JGTerrainChannels.h
Source/JungleGame/Public/Terrain/JGTerrainSampling.h
Source/JungleGame/Public/Terrain/JGTerrainFeatures.h
Source/JungleGame/Public/Terrain/JGTerrainGenerator.h
Source/JungleGame/Public/Terrain/JGTerrainMeshBuilder.h
Source/JungleGame/Public/Terrain/JGTerrainValidation.h
Source/JungleGame/Public/Terrain/JGTerrainExport.h
Source/JungleGame/Public/Terrain/JGTerrainRuntimeComponent.h
Source/JungleGame/Public/Terrain/JGTerrainGeneratorActor.h
```

Private `.cpp` equivalents should implement the actual logic.

### Optional editor-only files

```text
Source/JungleGameEditor/Private/Terrain/JGTerrainConfigValidator.cpp
Source/JungleGameEditor/Private/Terrain/JGTerrainExportCommandlet.cpp
```

Only add these if the repo already has an editor module or the batch includes editor validation/export tooling.

### Suggested class list

| Class/struct | Type | Responsibility |
|---|---|---|
| `FJGTerrainConfig` | `UStruct`/plain struct | Canonical config. |
| `UJGTerrainGeneratorConfig` | `UPrimaryDataAsset` | Source-controlled editor config wrapper. |
| `FJGTerrainBuildMetadata` | struct | Schema/version/seed/hash identity. |
| `FJGTerrainBuildRequest` | struct | Immutable generator input. |
| `FJGTerrainBuildResult` | struct | Canonical output plus validation. |
| `FJGTerrainDomain` | struct/class | Grid/world coordinate conversion. |
| `FJGTerrainSampler` | class | Deterministic sample/noise primitives. |
| `FJGTerrainChannelSet` | class | Typed masks/channels. |
| `FJGTerrainFeatureComposer` | class | Ordered terrain pass composition. |
| `FJGTerrainGenerator` | class | Top-level pure generation function. |
| `FJGTerrainMeshBuilder` | class | Mesh data conversion only. |
| `UJGTerrainRuntimeComponent` | component | Applies mesh/heightfield output in world. |
| `AJGTerrainGeneratorActor` | actor | Thin runtime/editor host. |
| `FJGTerrainValidator` | class | Validation checks. |
| `FJGTerrainExportService` | class | Artifact export. |
| `UJGTerrainConfigValidator` | editor validator | Data asset validation. |

## Large PR Sequencing Guidance

PR5 Batch 003 should be implemented as major, meaningful slices. Avoid reducing the work into tiny patches that leave the shell actor as the real generator.

### Recommended sequence: three substantial PRs

#### PR 003-A — Terrain core architecture and coast invariant foundation

Scope:

- add terrain config structs/data asset wrapper;
- add build metadata and config hash;
- add deterministic seed derivation;
- add domain/sampling primitives;
- add heightfield/channel set;
- add island/coast invariant channels;
- add initial validator for square edge ocean and beach ring;
- add automation tests for config hash, seed, and coast invariants;
- refactor shell actor enough to call the new generator for a simple base heightfield.

Acceptance:

- generator can produce a deterministic heightfield from config;
- sea-level/coast metadata appears in logs and validation report;
- actor no longer owns coast math;
- tests prove square-world edge remains ocean/container.

This PR can be large because it establishes ownership boundaries.

#### PR 003-B — Terrain feature composition: massif, ridges, catchments, crater, lava/crust, blockers

Scope:

- implement feature composer pass order;
- implement massif target reaching ~1400 m;
- implement ridge and gully/catchment passes constrained by coast/interior masks;
- implement crater/lava/crust channels as terrain-only outputs;
- implement hard-blocker mask/channel;
- emit feature contribution summaries;
- expand validation for elevation envelope, feature coverage, and coast leaks;
- add tests for feature/coast interaction.

Acceptance:

- every required feature is represented by code and channels;
- features fade/blend against island/coast constraints;
- beach ring remains intact after full composition;
- feature summaries report seed, coverage, min/max/mean contribution, read channels, and write channels;
- no gameplay/environment systems are introduced.

This PR should be large enough to make the architecture real. A PR that only adds a single ridge helper while leaving everything else in the actor is ineffective.

#### PR 003-C — Runtime output, export artifacts, validation report, and integration cleanup

Scope:

- add mesh builder/runtime component output path;
- migrate shell actor to generator-backed host;
- add export service for manifest, heightfield, selected channels, report Markdown/JSON, feature CSV, log token;
- validate deterministic result hash;
- add tile/chunk seam policy if required for scale;
- add docs explaining terrain-only boundaries and config identity;
- remove dead actor-owned terrain math.

Acceptance:

- runtime actor builds terrain from `FJGTerrainBuildResult`;
- exports include all required coast/sea-level metadata;
- validation report is produced from the same result used for runtime mesh;
- shell actor contains no embedded feature composition logic;
- old debug paths either route through export service or are removed.

### Alternative sequence: two larger PRs

If repository review cadence supports large PRs, combine as:

1. **Core + coast + actor migration**;
2. **Feature composition + validation/export + cleanup**.

Do not split into ten tiny PRs unless each PR lands a complete architecture layer with tests. Terrain architecture will fail if review pressure turns the work into cosmetic rearrangement.

### Branch and review hygiene

For each PR:

- start from clean updated `main`;
- keep terrain-only scope;
- inspect `git diff --stat` and full diff;
- stage intended files only;
- run relevant tests/checks;
- include validation/export artifacts only if they are intentionally committed docs, not generated junk;
- reject unrelated generated/cache/temp files;
- keep editor-only dependencies out of runtime build;
- document any accepted tolerance changes.

## Acceptance Criteria

### Architecture acceptance

- Terrain generation is owned by `FJGTerrainGenerator` or equivalent pure source class, not by the runtime shell actor.
- The actor/component only selects config, triggers generation, and applies output.
- Config identity includes schema version, algorithm version, config ID/version, root seed, canonical config hash, resolution, world size, and sea-level/coast metadata.
- Seed streams are named and deterministic.
- Same build request produces same stable result hash.
- Terrain heightfield is canonical and independent of runtime mesh/Landscape output.

### Coast invariant acceptance

- Island mask is generated before features.
- Beach ring is generated before features.
- Square-world edge remains ocean/container.
- All terrain features blend or clamp through coast/interior masks.
- Beach ring remains near configured sea level after massif/ridge/gully/crater/lava/blocker composition.
- Coast metadata appears in runtime log token, manifest, validation JSON, validation Markdown, and channel exports.

### Feature acceptance

- 1400 m volcanic massif target exists with validation tolerance.
- Ridges have explicit masks/channels and fade near coast.
- Catchments/gullies have explicit flow/incision channels and do not destroy beach ring.
- Crater has explicit crater/rim/hazard terrain channels.
- Lava/crust has explicit terrain mask/channel output.
- Hard blockers exist as terrain constraint channels, not gameplay systems.
- Each feature reports seed, version, read/write channels, coverage, and contribution stats.

### Output/export acceptance

- Runtime terrain can be rebuilt from config.
- Mesh builder consumes heightfield/channels and does not own feature math.
- Export service writes manifest, height output, selected channels, validation report, feature summary, and log token.
- Generated export names are stable and include config identity.
- Validation can fail the build/report when coast invariant breaks.

### Scope acceptance

- No foliage/ecology/weather/audio/survival/NPC/story/VFX/final material/player UI changes.
- No player-facing map/minimap/compass/GPS/objective/bearing/time/orientation UI.
- Debug visualization is terrain-channel validation only.

## Risks And Stop Conditions

### Architecture risks

| Risk | Impact | Mitigation |
|---|---|---|
| Generator remains inside actor | Refactor fails; future terrain work becomes spaghetti | Move math to pure generator before adding more features. |
| Coast handled as post-effect | Massif/ridges/gullies break beaches and island edge | Generate coast channels first and enforce final invariant. |
| Config identity not hashable | Reproducibility/debugging fail | Canonical JSON/hash and metadata in every build. |
| Unstable random usage | Same seed changes between runs | Named seed streams, no globals, determinism tests. |
| Mesh output mutates terrain | Runtime view diverges from exports | Heightfield is canonical; mesh builder consumes only. |
| Channels are informal debug arrays | Feature interactions become opaque | Typed `EJGTerrainChannel` and descriptors. |
| Editor-only dependencies leak runtime | Linux/runtime build risk | Separate runtime/editor modules or compile guards. |
| Massive PR lacks validation | Review cannot prove correctness | Include automation tests and export/report evidence. |

### Stop conditions

Stop implementation and report scope creep if any of these appear:

- a patch begins implementing foliage, ecology, animals, weather, audio, survival, NPC, story, VFX, final materials, asset packs, or player UI;
- a patch recommends or adds player-facing map/minimap/compass/GPS/objective/bearing/time/orientation UI;
- beach ring or island mask is implemented as a cosmetic post-process instead of an invariant channel;
- square-world edge becomes playable terrain identity;
- runtime logs or exports omit sea-level/coast metadata;
- feature functions write directly to actor state;
- actor code still owns massif/ridge/gully/crater/lava composition after the refactor;
- random generation depends on clock time, global RNG, object address, actor construction order, or task scheduling;
- generated artifacts/cache/temp files are committed unintentionally;
- new dependencies or editor modules break Linux-first runtime assumptions;
- a “minimal” PR only renames code and leaves architecture unchanged.

### Non-blocking deferrals

These can be deferred if the core architecture lands cleanly:

- full plugin extraction;
- full Landscape editor import path;
- high-performance multithreaded tiling;
- advanced erosion simulation;
- final material blending;
- external GIS-style export formats;
- Nanite/static mesh bake pipeline.

Do not defer island/coast invariant, config identity, generator extraction, or validation metadata. Those are core architecture, not polish.

## Rejected Or Forbidden Approaches

### Forbidden: giant terrain actor

Do not keep adding feature functions, arrays, config defaults, debug exporters, and mesh generation into the runtime shell actor. That duplicates the current problem.

### Forbidden: beach as post-effect

Do not generate mountains/ridges/gullies first and then paint a beach mask afterward. The beach ring must constrain feature expression and final elevation.

### Forbidden: square boundary as terrain identity

The square-world edge is a container/ocean edge. It must not become island outline identity or raised terrain.

### Forbidden: ad hoc random calls

Do not use global random functions or object construction order. Named seed derivation is mandatory.

### Forbidden: Blueprint-only generator architecture

Blueprint editor controls can exist, but deterministic terrain architecture should be C++ source-owned and testable.

### Forbidden: hidden config mutation

Do not allow runtime or editor conveniences to mutate config without updating identity/hash/report metadata.

### Forbidden: mesh-first terrain

Do not make the procedural mesh data the only terrain representation. Heightfield/channels are canonical.

### Forbidden: gameplay hazard implementation

`CraterHazardMask` or `LavaCrustMask` is allowed as terrain metadata. Damage, player status effects, UI warnings, quests, or NPC responses are forbidden in this batch.

### Forbidden: final environment art scope

Do not add foliage, ecology, weather, VFX, final materials, or asset packs. Debug materials or simple channel visualization are allowed only when they validate terrain output.

### Forbidden: tiny ineffective PR slicing

Do not split the work into patches so small that they cannot establish architecture boundaries. Each PR should land a complete architecture layer or meaningful terrain feature composition stage.

## Sources And References

The following sources informed the Unreal Engine architecture recommendations and validation/export guidance:

1. Epic Games, **Asset Management in Unreal Engine** — Primary and Secondary Asset model; useful basis for terrain config data asset identity.  
   https://dev.epicgames.com/documentation/unreal-engine/asset-management-in-unreal-engine

2. Epic Games, **Random Streams in Unreal Engine** — repeatable random streams for procedural distributions.  
   https://dev.epicgames.com/documentation/unreal-engine/random-streams-in-unreal-engine

3. Epic Games, **FRandomStream API Reference** — seed/name initialization and repeatable random stream API surface.  
   https://dev.epicgames.com/documentation/unreal-engine/API/Runtime/Core/FRandomStream

4. Epic Games, **UProceduralMeshComponent API Reference** — runtime custom triangle mesh component; documented as experimental.  
   https://dev.epicgames.com/documentation/unreal-engine/API/Plugins/ProceduralMeshComponent/UProceduralMeshComponent

5. Epic Games, **UDynamicMeshComponent API Reference** — dynamic mesh component based on `UDynamicMesh`/`FDynamicMesh3`, with render update/chunk support.  
   https://dev.epicgames.com/documentation/unreal-engine/API/Runtime/GeometryFramework/UDynamicMeshComponent

6. Epic Games, **Landscape Technical Guide in Unreal Engine** — Landscape heightmap/component architecture and valid dimension constraints.  
   https://dev.epicgames.com/documentation/unreal-engine/landscape-technical-guide-in-unreal-engine

7. Epic Games, **Data Validation in Unreal Engine** — custom validation rulesets for assets.  
   https://dev.epicgames.com/documentation/unreal-engine/data-validation-in-unreal-engine

8. Epic Games, **Automation Test Framework in Unreal Engine** — Unreal automation system for unit, feature, and content stress testing.  
   https://dev.epicgames.com/documentation/unreal-engine/automation-test-framework-in-unreal-engine

9. Epic Games, **FFileHelper API Reference** — file read/write helper API for export services.  
   https://dev.epicgames.com/documentation/unreal-engine/API/Runtime/Core/FFileHelper

10. Chance, Ghobrial, McAreavey, Lemaignan, Pipe, and Eder, **On Determinism of Game Engines used for Simulation-based Autonomous Vehicle Verification** — discusses determinism as a repeatability/debuggability requirement in Unreal-based simulation contexts.  
   https://arxiv.org/abs/2104.06262
