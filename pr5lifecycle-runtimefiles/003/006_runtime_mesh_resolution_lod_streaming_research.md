# Runtime Mesh Resolution, LOD, And Terrain Scale Research

## Executive Summary

Jungle Game should not try to solve island-scale terrain by simply raising the current single procedural mesh from `129 x 129` to a giant dense mesh. The existing mesh proves world scale and collision, but at roughly `16,256 m / 128 = 127 m` between vertices it cannot express readable beaches, crater rims, radial gullies, lahar channels, lava shelves, or slope transitions.

The recommended architecture is a **staged bridge**:

1. **Canonical terrain source first**: a deterministic heightfield/mask source sampled in world meters, independent of the render representation.
2. **Tiled procedural mesh bridge next**: replace the single coarse mesh with streamable/toggleable terrain tiles that sample the canonical source at several spacings.
3. **Developer exports in parallel**: export heightmaps, masks, seam maps, topographic images, and manifests from the same source used by runtime tiles.
4. **Landscape/World Partition target later**: when the generated source is validated, import/export as UE Landscape data for production-scale terrain streaming, collision, and LOD.

The clean production target for the current `16.256 km` terrain extent is an `8129 x 8129` heightfield at **2 m spacing**: `8128 quads * 2 m = 16,256 m`. This matches Epic's recommended `8129 x 8129` Landscape size, which uses `32 x 32 = 1024` Landscape Components and stays within Epic's documented largest recommended component count.[^ue-landscape-technical]

That does **not** mean the game should render or collide the whole island at 2 m immediately. The next implementation should preserve iteration speed by using:

- **source heightfield target**: 2 m full-island grid, optionally 1 m local oversampling for crater/beach validation;
- **runtime tile mesh target**: 512 m tiles, usually `129 x 129` vertices at 4 m near the player, `257 x 257` at 2 m only for local/crater/beach debug, and `65 x 65` or `33 x 33` for mid/far LOD;
- **collision target**: collision only on active near/playable tiles, with async cooking and lower-resolution collision for non-critical tiles;
- **coastline invariant**: shoreline is generated from the canonical source and signed distance/mask fields, not from tile-local falloff or LOD-local smoothing.

The most important rule is that **the island mask and sea-level perimeter must be owned by the canonical terrain source**. Tiles, LODs, collision, debug maps, and future Landscape imports must all consume the same world-space samples. If coastline is generated separately per tile or per LOD, the system will eventually produce square artifacts, shoreline drift, cracks, or mismatched masks.

## Terrain-Only Scope Boundary

This document covers terrain mesh/heightfield architecture only:

- terrain scale;
- heightfield resolution;
- procedural mesh tiling;
- Landscape bridge/export strategy;
- LOD/streaming;
- collision;
- seam preservation;
- coastline preservation;
- validation artifacts;
- generated artifact hygiene.

Out of scope:

- foliage;
- ecology;
- weather;
- audio;
- survival systems;
- NPCs;
- VFX;
- final materials;
- world dressing;
- player-facing UI;
- maps, minimaps, compasses, GPS, objectives, bearing widgets, time widgets, or orientation UI.

Developer-only terrain validation maps, editor-only overlays, commandlet exports, logs, and saved images are in scope. Player-facing navigation UI is not.

## Current Runtime Mesh Baseline

Current proof:

- terrain extent: `16.256 km` full-size island terrain area;
- mesh resolution: `129 x 129` vertices;
- intervals per axis: `128`;
- vertex spacing: `16,256 m / 128 = 127 m`;
- result: enough to prove world scale, rough collision, and macro presence;
- limitation: far too coarse for terrain readability.

At 127 m spacing, a single quad is larger than many beach widths, gully cross-sections, crater-edge details, hazard terraces, roads/tracks, and foot-scale traversal decisions. It can only express broad volcanic massing. Any visual readability at that spacing would have to come from material normals or meshes, which would be fake relative to collision and slope.

The baseline should remain useful as a **debug macro LOD** or fallback proof, but not as the main terrain representation for PR5 Batch 003.

## Major Implementation Goal

The implementation goal is to make terrain generation representation-independent:

```text
Canonical world-space terrain source
        |
        |-- runtime tiled procedural mesh
        |-- runtime/developer collision data
        |-- developer heightfield exports
        |-- developer mask exports
        |-- seam and coastline validation maps
        |-- future UE Landscape import source
```

The canonical source should answer the same query everywhere:

```cpp
FJGTerrainSample SampleTerrainMeters(double XMeters, double YMeters);
```

A sample should include at least:

```cpp
struct FJGTerrainSample
{
    double HeightMeters;
    double SignedIslandDistanceMeters;   // positive inside island, 0 at shoreline, negative outside
    double IslandMask01;
    double BeachMask01;
    double CoastalShelfMask01;
    double CraterMask01;
    double CraterHazardMask01;
    double RidgeMask01;
    double GullyMask01;
    double SlopeDegrees;
    double Curvature;
};
```

The exact member names can follow the existing code style. The important part is architectural: height, masks, and derived diagnostics come from one deterministic source.

Recommended high-level classes/modules:

| Area | Suggested type | Responsibility |
|---|---:|---|
| Terrain source | `UJGTerrainSource` or `FJGTerrainSource` | Own deterministic world-space height/mask sampling. |
| Coordinate mapping | `FJGTerrainCoordinates` | Convert world centimeters ↔ terrain meters ↔ normalized island coordinates. |
| Tile description | `FJGTerrainTileKey`, `FJGTerrainTileDesc` | Tile index, bounds, spacing, LOD, overlap policy. |
| Mesh builder | `FJGTerrainTileMeshBuilder` | Build vertices, indices, normals, UVs, tangents, masks from source samples. |
| Runtime actor | `AJGTiledTerrainActor` | Own tile components, update visible/active LOD rings, collision policy. |
| Export command | `UJGTerrainExportCommandlet` or editor utility | Export heightmaps, masks, manifests, seam maps, topographic debug images. |
| Validation | `UJGTerrainValidationSubsystem` or automation tests | Check shoreline, tile seams, LOD consistency, collision deltas. |
| Debug map | `L_TerrainResolutionValidation` | Developer-only level for deterministic terrain validation. |

The staged bridge avoids a premature hard dependency on final Landscape import while still producing data that can become a Landscape when ready.

## Sea-Level Edge And Tile Seam Requirements

The coastline requirement should be treated as a hard invariant, not a visual preference.

Required terrain invariants:

```text
outer circular island edge = sea level beach/coastal shelf
outside island mask = ocean/below sea level
LOD seams must not break the 0 m shoreline
tiles must not create square-edge terrain artifacts
coastline, beaches, ridges, gullies, and crater masks must stay aligned across resolution changes
```

### Canonical coastline source

Use a world-space signed distance field for the island footprint:

```text
SignedIslandDistanceMeters > 0  => inside island
SignedIslandDistanceMeters = 0  => shoreline / 0 m sea-level perimeter
SignedIslandDistanceMeters < 0  => outside island / ocean side
```

For a circular island proof, this can be analytic:

```text
Distance = IslandRadiusMeters - length(WorldXY - IslandCenterXY)
```

Later, this can become a shaped island SDF, but it should still be sampled in global island space rather than tile-local space.

### Sea-level clamp

The source should impose sea-level behavior before tiles are built:

```text
if outside island mask:
    height <= OceanFloorMeters or below sea level
if inside beach/coastal shelf band:
    height approaches 0 m smoothly and exactly reaches 0 m at shoreline
if shoreline sample:
    abs(height) <= shoreline_epsilon
```

Recommended validation epsilon:

```text
shoreline_epsilon = 0.05 m for source samples
runtime_visual_epsilon = 0.10 m after float conversion
collision_epsilon = 0.25 m unless using lower collision LOD
```

The exact numbers can be tuned after implementation, but a numeric tolerance must exist so regressions can fail deterministically.

### Tile seam preservation

Tiles must sample shared borders from the same global coordinates. Do **not** generate tile heights from local tile coordinates with independent noise seeds.

For a 512 m tile at 4 m spacing:

```text
quads per tile = 512 / 4 = 128
vertices per tile side = 129
```

Adjacent tile `A` east border and tile `B` west border must produce identical global sample coordinates and identical source values:

```text
A.XMax == B.XMin
A.Sample(XMax, Y) == B.Sample(XMin, Y)
```

Recommended seam test:

```text
for every adjacent tile pair:
    for every shared-border vertex:
        assert abs(A.Height - B.Height) <= 0.001 m at source precision
        assert abs(A.Mask - B.Mask) <= 0.001 for all terrain masks
```

Runtime mesh components may duplicate border vertices for independent components, but the duplicated vertices must contain the same sampled values.

### Avoiding square-edge terrain artifacts

Square tile boundaries must never define island shape. The island mask must cut across tiles naturally. Outside-island terrain should continue below sea level or be covered by ocean; it should not end at tile boundaries.

Forbidden:

```text
height *= tileLocalFalloff
mask = distanceToTileEdge
lower terrain at tile edge
fade island at tile edge
```

Allowed:

```text
height = TerrainSource.Sample(global_x, global_y).HeightMeters
mask = TerrainSource.Sample(global_x, global_y).IslandMask01
```

If skirts are used to hide terrain tile cracks, they must be strictly a mesh artifact below visible terrain and must not create or modify the island shoreline. Skirts are acceptable below ocean or along invisible far-tile boundaries; they are not a coastline solution.

## Resolution Targets By Terrain Feature

Resolution should be specified separately for source, rendered mesh, collision, and validation. A single number is not enough.

### Practical target tiers

| Tier | Spacing | Use | Notes |
|---|---:|---|---|
| Macro preview | 32-128 m | whole-island silhouette, debug map, old baseline replacement | Current 127 m is only macro proof. |
| Far terrain | 16-32 m | distant island shape, horizon, far ridges | Acceptable only if sourced from high-res canonical data. |
| Mid terrain | 8 m | readable cone, major ridges, large gullies | Good for streamed mid-distance tiles. |
| Near terrain | 4 m | first-person slope readability, beach band, ridge/gully traversal | Recommended default near runtime mesh target. |
| Critical terrain | 2 m | crater rim, beach shelf, hazard zones, narrow gullies | Recommended full Landscape target and local debug mesh tier. |
| Local oversample | 1 m | validation/source baking for crater/beach hazards | Use selectively; not necessarily full-island runtime render. |

### Feature-specific recommendations

| Terrain feature | Minimum useful spacing | Preferred source spacing | Preferred runtime spacing | Reason |
|---|---:|---:|---:|---|
| Island macro cone / volcanic massif | 32 m | 8-16 m | 16-32 m far, 8 m mid | Macro shape tolerates coarse spacing. |
| Major radial ridges | 8-16 m | 4-8 m | 4-8 m | Ridges need slope/crest readability. |
| Gullies / lahar channels | 4-8 m | 2-4 m | 4 m near, 8 m mid | Cross-sections vanish above 16-32 m. |
| Beach ring / coastal shelf | 2-4 m | 2 m | 2-4 m | Shoreline and shelf width need stable sea-level crossing. |
| Crater rim / crater floor | 2-4 m | 1-2 m | 2-4 m | Rim silhouette and walkable slopes need high fidelity. |
| Crater hazard / lava crust terrain | 1-2 m | 1 m local, 2 m full-island | 2 m local debug, 4 m gameplay until proven | Fine hazard terrain needs validation before full runtime cost. |
| Collision near player | 2-4 m | same as source | 4 m default, 2 m in critical areas | Avoid player floating/falling through readable slopes. |

### Full-island grid options

The terrain extent is `16,256 m`. The following grid sizes are useful:

| Full-island spacing | Required vertices | Approx samples | Recommended use |
|---:|---:|---:|---|
| 127 m | 129 x 129 | 16.6k | Existing baseline only. |
| 32 m | 509 x 509 | 259k | Macro debug export. |
| 16 m | 1017 x 1017 | 1.03M | Far terrain/debug source preview. |
| 8 m | 2033 x 2033 | 4.13M | Mid terrain export/debug. |
| 4 m | 4065 x 4065 | 16.5M | Near-quality procedural source or custom mesh cache. |
| 2 m | 8129 x 8129 | 66.1M | Best full-island Landscape target. |
| 1 m | 16257 x 16257 | 264M | Too heavy for normal full-island runtime; use local oversampling or tiled offline validation. |

Important fit: `8129 x 8129` at 2 m spacing exactly covers `16.256 km`. Epic lists `8129 x 8129` as a recommended Landscape size using `127` quads per section, `2 x 2` sections per component, and `1024` total components.[^ue-landscape-technical]

### DEM analogy

Real-world DEM products are a useful sanity check, not a direct requirement. USGS 1 arc-second DEM is approximately 30 m resolution and is intended for broad topographic surfaces; USGS 1 m DEM is lidar-derived and used for high-resolution bare-earth terrain work.[^usgs-30m-dem][^usgs-1m-dem] For a first-person volcanic island where beaches, gullies, and crater hazards are traversal-relevant, the useful game terrain source must be much closer to the high-resolution end than to 30 m DEM scale.

## Procedural Mesh Versus Landscape Tradeoff

### Single higher-resolution procedural mesh

A single monolithic procedural mesh is the least attractive option.

Pros:

- simplest conceptual extension of current implementation;
- easy to debug initially;
- no Landscape import workflow required.

Cons:

- large vertex/index buffers;
- expensive collision cooking;
- no natural tile streaming;
- poor edit/validation granularity;
- high risk of blocking the game thread during rebuilds;
- no production-grade Landscape LOD/mip/heightfield streaming;
- hard to preserve collision performance at island scale.

Verdict: **reject as final architecture**. It can exist only as a small debug view or compatibility fallback.

### Tiled procedural mesh

A tiled procedural mesh is the right next bridge.

Pros:

- preserves current runtime-generation workflow;
- works before Landscape import is ready;
- supports tile-level LOD and collision policy;
- makes seam validation explicit;
- can stream or selectively generate near tiles;
- can export the same source to Landscape later;
- keeps implementation testable in PR-sized chunks.

Cons:

- manual LOD seam handling required;
- manual tile streaming policy required;
- procedural mesh collision cooking can still be expensive;
- must not become a custom Landscape replacement forever.

Verdict: **implement next** as a bridge, not as the final terrain system.

### UE Landscape

UE Landscape is the likely final production terrain representation for this scope.

Pros:

- designed for large outdoor terrains;
- uses efficient heightfield storage;
- supports component-level LOD and streaming;
- integrates with World Partition;
- supports heightmap import/export and tiled heightmaps;
- exposes Landscape collision mip controls;
- can use Landscape proxy streaming and HLOD behavior;
- stores terrain data more efficiently than equivalent static meshes at the same vertex density.[^ue-landscape-overview]

Cons:

- import/export pipeline must be exact;
- valid dimensions/scales must be chosen carefully;
- runtime terrain mutation is not the main Landscape workflow;
- large `8129 x 8129` data can increase build/cook times;
- Nanite Landscape duplicates some data and can increase streaming/memory footprint.[^ue-nanite-landscape]

Verdict: **production target**, but only after the canonical heightfield and masks are validated through exports.

### Runtime-generated heightfield assets

Generated heightfield assets are useful as a bridge artifact:

- `.r16` or 16-bit PNG heightmaps;
- mask PNGs;
- tile manifests;
- import JSON sidecars for RAW where needed;
- debug topographic maps;
- source fingerprints/hashes.

UE supports importing Landscape heightmaps from 16-bit grayscale PNG, 8-bit `r8`, 16-bit `r16`, and RAW with a JSON sidecar; it also supports tiled heightmap import with World Partition.[^ue-heightmap-import]

Verdict: **yes for developer pipeline**, not as uncontrolled runtime-generated tracked content.

### Recommended decision

Use a **staged bridge architecture**:

```text
Now:
    current 129 x 129 mesh remains macro debug/fallback

Next PR:
    canonical source + tiled procedural mesh + validation exports

After validation:
    Landscape import/export path at 8129 x 8129 / 2 m target

Production target:
    UE Landscape + World Partition, with procedural source retained as generator of truth
```

## Tiled Terrain Architecture Options

### Recommended tile size

Use **512 m x 512 m** terrain tiles for the procedural bridge.

For `16,256 m` extent:

```text
16,256 / 512 = 31.75
```

Because the current full terrain is probably represented as a square extent centered around the island, use one of these policies:

| Policy | Tile count | Covered size | Notes |
|---|---:|---:|---|
| Exact 16.256 km custom edge | 32 x 32 with adjusted outer tile bounds | 16.256 km | Edge tiles slightly smaller or coordinate extent exact. |
| Normalized 512 m grid | 32 x 32 | 16.384 km | Adds 64 m margin per side; ocean/outside-island area absorbs margin. |
| Landscape-aligned source | 8128 quads @ 2 m | 16.256 km | Best final source; runtime tiles derive bounds from source grid. |

Recommended: keep the **source extent exactly 16.256 km** and allow tile descriptors to derive bounds from source indices. Avoid introducing accidental scale drift just because tiles are convenient.

### Recommended tile vertex counts

For 512 m nominal tiles:

| Spacing | Quads | Vertices per side | Vertices per tile | Use |
|---:|---:|---:|---:|---|
| 16 m | 32 | 33 | 1,089 | Far LOD. |
| 8 m | 64 | 65 | 4,225 | Mid LOD. |
| 4 m | 128 | 129 | 16,641 | Default near/playable LOD. |
| 2 m | 256 | 257 | 66,049 | Critical local debug/high-fidelity LOD. |
| 1 m | 512 | 513 | 263,169 | Avoid for normal runtime tiles; local validation only. |

A 32 x 32 tile grid has 1024 tiles. Rendering every tile at 4 m would be about 17 million vertices before indices and sections; that is not the right default. Rendering every tile at 16 m is about 1.1 million vertices and is much more plausible for a full-island debug view.

Recommended runtime policy:

```text
player tile and immediate neighbors: 4 m
critical crater/beach validation tiles: optional 2 m
mid ring: 8 m
far ring: 16 m or 32 m
outside island/ocean-only tiles: no collision, coarsest mesh, or omitted below ocean plane
```

### Tile coordinate policy

Use integer source-grid coordinates wherever possible.

For the 2 m final source:

```text
source_vertices_per_axis = 8129
source_quads_per_axis = 8128
source_spacing_m = 2.0
extent_m = 16256.0
```

For a runtime tile at 4 m:

```text
sample every 2nd source grid point
```

For a runtime tile at 8 m:

```text
sample every 4th source grid point
```

For a runtime tile at 16 m:

```text
sample every 8th source grid point
```

This keeps LOD samples nested. A coarse LOD vertex should correspond exactly to a high-resolution source sample, not to a separately evaluated approximation.

### Tile border policy

Each tile should include its max border vertex, even if adjacent tiles duplicate it. That means `quads + 1` vertices per tile side.

Rules:

- duplicate borders are allowed;
- border samples must be from identical world/source coordinates;
- normals on border vertices should be computed from source-neighbor samples, not from tile-local topology only;
- tangent/normal derivation must use ghost samples outside the tile to avoid lighting discontinuity;
- masks must be sampled from the source at the same coordinates as height.

### Tile metadata

Each generated tile should have a manifest entry:

```json
{
  "tile_x": 12,
  "tile_y": 17,
  "lod_spacing_m": 4.0,
  "bounds_min_m": [-1024.0, 512.0],
  "bounds_max_m": [-512.0, 1024.0],
  "vertex_count": [129, 129],
  "height_min_m": -40.0,
  "height_max_m": 312.5,
  "shoreline_sample_count": 42,
  "max_border_delta_m": 0.0,
  "has_collision": true,
  "source_hash": "..."
}
```

The exact JSON schema can be adjusted, but it should be stable enough for regression comparisons.

## Collision Strategy

Collision must be managed as a separate budget. Visual mesh density and collision density do not have to be identical everywhere.

### Procedural mesh bridge collision

For `UProceduralMeshComponent`, Epic exposes `bUseAsyncCooking`, `bUseComplexAsSimpleCollision`, collision body setup, and convex collision APIs.[^ue-procmesh-api] Use those controls deliberately:

Recommended bridge policy:

| Tile class | Visual spacing | Collision spacing | Collision enabled | Notes |
|---|---:|---:|---:|---|
| Player tile | 4 m | 4 m or 2 m in debug/crater zones | Yes | Highest priority. |
| Immediate neighbor tiles | 4-8 m | 4-8 m | Yes | Prevent edge fall-through. |
| Mid tiles | 8 m | 8-16 m | Optional | Enable only if traversal can reach soon. |
| Far tiles | 16-32 m | None | No | Visual only. |
| Outside island/ocean-only tiles | 16-32 m or omitted | None | No | Ocean handles visual cover. |

Implementation rules:

- Enable async cooking for procedural collision.
- Do not recook collision every frame.
- Do not rebuild collision for far visual-only LOD changes.
- Keep collision generation deterministic from the same height source.
- Prefer lower collision detail for non-critical tiles rather than complex full-resolution collision everywhere.
- Keep a runtime stat/log for collision cook requests, queued cooks, completed cooks, and cook time.

Recommended collision generation path:

```text
Generate visual tile mesh
Generate optional collision mesh from same source at collision spacing
Submit collision cook asynchronously
Keep previous valid collision until replacement is ready
Swap collision only after cook completion
```

Stop condition:

```text
If collision cook causes noticeable hitching during normal traversal, stop adding resolution and implement collision queue/rate limiting first.
```

### Landscape collision target

When the project moves to Landscape, use Landscape collision mip controls. Epic documents separate `Collision Mip Level` and `Simple Collision Mip Level` settings, including per-Component adjustment; lower mip values are more accurate and higher values are cheaper but less accurate.[^ue-landscape-collision]

Recommended Landscape collision policy:

- playable crater/beach/ridge areas: `Collision Mip Level = 0` initially;
- simple collision: test `Simple Collision Mip Level = 1` or `2` for memory/performance;
- non-playable or far components: higher collision mips or streamed out;
- validate with Player Collision and Visibility Collision view modes;
- keep collision changes per-component where possible.

Epic's Landscape overview notes that Landscape uses heightfield collision, can use reduced-resolution collision heightfields, and can stream collision/render components for distant terrain.[^ue-landscape-overview]

### Collision validation

Add developer validation traces:

- downward line traces at a grid of points over active tiles;
- compare hit `Z` against canonical source height;
- log max, mean, and percentile error;
- separately report beach, crater, gully, and steep-slope regions;
- fail validation if the player can fall through tile seams or if collision error exceeds threshold in playable regions.

Suggested thresholds:

```text
near playable terrain max collision error: <= 0.25 m
crater hazard local validation error: <= 0.15 m if using 2 m collision
far visual-only terrain: no collision requirement
seam fall-through: zero tolerance
```

## LOD / Streaming / Seam Strategy

### Use nested LOD samples

LOD levels must be nested powers of two from the same source grid:

```text
2 m source: every 1 sample
4 m LOD: every 2 samples
8 m LOD: every 4 samples
16 m LOD: every 8 samples
32 m LOD: every 16 samples
```

This ensures that a coarse vertex exists at the same source coordinate as a high-resolution vertex. It also makes border stitching and validation much simpler.

### Avoid independent LOD regeneration

Forbidden:

```text
LOD0 = generate terrain with seed A and resolution 2 m
LOD1 = generate terrain with seed A and resolution 4 m but different smoothing/noise path
LOD2 = generate terrain with separate simplified function
```

Allowed:

```text
LOD0 = sample canonical source at 2 m
LOD1 = sample canonical source at 4 m
LOD2 = sample canonical source at 8 m
```

If filtered mips are used, they must be derived from the source with coastline-aware rules. A naive averaged height mip can move the `0 m` shoreline because it blends above-sea and below-sea samples. For coastline, use one of these:

- exact SDF/mask resampling and explicit sea-level clamp at shoreline;
- constrained mip generation that preserves the shoreline zero crossing;
- render coarse terrain from height mips but derive coastline/beach masks from the SDF at current sample coordinates.

### LOD seam options

Ranked recommendation:

1. **Same-edge-resolution rings for first bridge**: keep adjacent visible tiles at matching edge resolution where possible. This avoids complex stitching in the first implementation.
2. **Transition strips**: if a 4 m tile borders an 8 m tile, add a stitch strip on the higher-resolution side that collapses every other edge vertex to the coarse edge.
3. **Geomorphing**: transition vertex heights/positions over time or distance to avoid popping.
4. **Skirts**: acceptable as a debug/far fallback below terrain; not acceptable as the primary visible seam solution and not a coastline solution.

First implementation should prefer option 1 plus validation. Complex stitching can come after the source/tile/export pipeline is proven.

### Streaming policy

A simple distance-ring policy is enough for the bridge:

```text
LOD0 / 2 m: optional, only selected crater/beach validation tiles
LOD1 / 4 m: player tile + 1 tile radius
LOD2 / 8 m: 2-4 tile radius
LOD3 / 16 m: rest of island visible terrain
No collision: far/ocean-only tiles
```

A later production Landscape path should rely on World Partition for large-world streaming. Epic describes World Partition as a distance-based system that stores a world in one persistent level divided into grid cells and streams cells based on streaming sources.[^ue-world-partition] Landscape also integrates with World Partition through Landscape Proxies that can be loaded and unloaded.[^ue-landscape-overview]

### World origin and precision

A 16 km world is not huge by UE open-world standards, but keep coordinates disciplined:

- terrain source operates in double-precision meters;
- UE component vertices are converted to centimeters at the edge;
- tile local origins keep vertex coordinates small;
- actor/component transforms place tiles in world;
- validation logs report both terrain meters and UE centimeters where useful.

### Normals and slopes across LODs

Normals must not be computed only from tile-local triangles if that creates border lighting seams. Use source-space finite differences with ghost samples:

```text
height_x_minus = Sample(x - spacing, y)
height_x_plus  = Sample(x + spacing, y)
height_y_minus = Sample(x, y - spacing)
height_y_plus  = Sample(x, y + spacing)
normal = derive from central differences
```

For border vertices, this samples outside the tile but still inside the source. This keeps normals consistent across tiles.

## Heightfield Source And Runtime Mesh Consistency

The runtime mesh, future Landscape import, masks, and validation images must all come from the same generator code path.

### Source fingerprint

Every export and runtime tile manifest should include a source fingerprint:

```text
terrain_generator_version
seed
extent_m
source_spacing_m
height_formula_version
mask_formula_version
shoreline_policy_version
crater_policy_version
ridge_gully_policy_version
```

Use a stable hash of the source config. The goal is to detect when a runtime capture and a heightmap export came from different terrain definitions.

### Height units

Keep source heights in meters. Convert to Unreal centimeters only at component/build time.

For Landscape Z scale, Epic documents the formula:

```text
ZScale = MaxHeightMeters * 100 * 0.001953125
```

This is based on Landscape's `-256 to 256` internal height range mapping.[^ue-landscape-technical] For Jungle Game, include both positive volcano height and below-sea/ocean margin in the max height range, not just the 1400 m summit. For example:

```text
minimum height target: -200 m ocean margin
maximum height target: 1500 m terrain margin
total vertical range: 1700 m
```

Do not set vertical range so tightly that 0 m sea level has no safe quantization margin.

### Recommended export targets

| Export | Resolution | Format | Purpose |
|---|---:|---|---|
| Full height source | `8129 x 8129` | `.r16` plus JSON sidecar or 16-bit PNG | Landscape import target. |
| Macro preview | `1017 x 1017` or `2033 x 2033` | PNG | Fast visual sanity check. |
| Island mask | same as height or downsampled | PNG | Confirm outside/inside and shoreline. |
| Shoreline SDF | same/downsampled | EXR/CSV/PNG debug | Verify 0 m perimeter independent of tile boundaries. |
| Beach mask | same/downsampled | PNG | Confirm beach ring continuity. |
| Crater mask | same/downsampled | PNG | Confirm crater hazard alignment. |
| Ridge/gully masks | same/downsampled | PNG | Confirm radial drainage/ridge consistency. |
| Slope map | downsampled | PNG | Confirm traversal readability. |
| Contour/topographic map | downsampled SVG/PNG | Developer review. |
| Tile seam heatmap | tile grid resolution | PNG/CSV | Detect cracks and mismatches. |
| Runtime tile manifest | per run | JSON | Compare runtime mesh to source/export. |

### Landscape import path

Recommended production import path:

```text
1. Generate canonical 8129 x 8129 heightfield at 2 m spacing.
2. Generate matching masks from same source.
3. Export height as r16/PNG and masks as developer artifacts.
4. Import into Landscape with World Partition enabled.
5. Use exact XY scale: 200 cm per vertex spacing.
6. Calculate Z scale from chosen vertical range.
7. Validate shoreline, collision, and tile/proxy seams.
8. Keep source generator as truth even after Landscape import.
```

UE supports custom heightmap import and tiled heightmap import workflows, including World Partition settings during import.[^ue-heightmap-import]

## Developer-Only Validation Outputs

Validation outputs are essential because the visual viewport can hide source/LOD mismatch until much later.

### Required debug commands

Suggested console/editor commands:

```text
jg.Terrain.ExportValidation
jg.Terrain.ExportHeightfield 8129
jg.Terrain.ExportMasks
jg.Terrain.DumpTile <x> <y> <lod>
jg.Terrain.ValidateSeams
jg.Terrain.ValidateShoreline
jg.Terrain.ValidateCollision
jg.Terrain.LogRuntimeTileStats
```

Names can follow project conventions. The command set should support automated and manual validation.

### Runtime logs

On terrain build, log:

```text
Terrain extent meters
Source spacing meters
Runtime tile count
Active tile count
Tile size meters
LOD spacing per tile
Vertex count per LOD
Triangle count per LOD
Collision-enabled tile count
Collision cook queue size
Max seam height delta
Max shoreline height error
Height min/max
Source fingerprint
Export path if validation output enabled
```

Example log shape:

```text
[JungleTerrain] Source extent=16256m spacing=2m vertices=8129x8129 seed=12345 hash=9B7...
[JungleTerrain] Runtime tiles=32x32 tileSize=512m active=49 lods={4m:9,8m:40,16m:975}
[JungleTerrain] SeamValidation maxHeightDelta=0.0000m maxMaskDelta=0.0000 result=PASS
[JungleTerrain] ShorelineValidation samples=32768 maxAbsHeight=0.042m result=PASS
[JungleTerrain] Collision cooks queued=9 completed=9 maxCookMs=18.4 result=PASS
```

### Validation maps

Add a developer-only validation level or mode that can show:

- raw heightfield preview;
- slope map;
- contour lines;
- shoreline points;
- tile boundaries;
- current LOD per tile;
- seam error heatmap;
- collision error trace points;
- crater/beach/ridge/gully masks.

This is not player UI. It is a developer terrain correctness view.

### Automated checks

Minimum automation checks:

1. **Source extent check**: generated source covers exactly the intended terrain bounds.
2. **Height range check**: no NaN/Inf; min/max within expected range.
3. **Sea-level check**: shoreline SDF zero samples are near 0 m height.
4. **Outside-island check**: outside mask samples are below sea level.
5. **Tile seam check**: adjacent tile border source samples match.
6. **LOD nesting check**: coarse LOD vertices equal corresponding high-source samples.
7. **Mask alignment check**: masks sampled at a coordinate match across LODs/exports.
8. **Collision delta check**: trace hits match source height in active collision tiles.
9. **Export consistency check**: exported heightfield hash matches runtime source hash.

## Artifact Hygiene Rules

Generated terrain artifacts can be huge and must stay ignored and unstaged unless the repo intentionally tracks a tiny fixture.

### Must stay ignored and unstaged

Do not stage normal generated outputs such as:

```text
Saved/
Intermediate/
DerivedDataCache/
Binaries/
.vscode/
.idea/
*.sln
*.VC.db
*.opensdf
*.opendb
```

Terrain-specific generated outputs should also stay ignored:

```text
Saved/TerrainExports/
Saved/TerrainValidation/
Saved/JungleGame/TerrainExports/
Saved/JungleGame/TerrainValidation/
Content/GeneratedTerrain/
Content/TerrainExports/
Content/__Generated__/
*.r16
*.raw
*.exr
*_height.png
*_heightmap.png
*_mask.png
*_slope.png
*_contours.png
*_seams.png
*_seam_heatmap.png
*_tile_manifest.json
*_terrain_manifest.json
*_runtime_capture.json
*_collision_validation.csv
*_topographic_debug.*
```

If small deterministic fixtures are needed for tests, put them in an explicit fixture path and keep them tiny:

```text
Source/JungleGameTests/Fixtures/Terrain/
```

Recommended rule:

```text
Only code, config, small hand-authored fixtures, and documentation are staged.
Large generated terrain data is regenerated locally and never committed.
```

### PR hygiene checks

Before commit/PR:

```text
git status --short
git diff --stat
git diff -- .gitignore Source Config Content/Maps Docs Tests
```

Block commit if generated terrain images, large heightfields, caches, cooked data, screenshots, profiling captures, or local editor artifacts appear in the staged set.

## Large PR Implementation Scope

PR5 Batch 003 is described as a major terrain-generation implementation batch, so this can be a large PR. It should still be scoped around terrain representation and validation only.

Recommended scope:

1. Extract current terrain generation into a canonical source object.
2. Keep current 129 x 129 mesh path as a baseline/debug mode.
3. Add tiled runtime mesh generation.
4. Add tile descriptors and coordinate conversion helpers.
5. Add nested LOD sampling from the same source.
6. Add near-tile collision policy with async cooking.
7. Add seam and shoreline validation.
8. Add developer exports for height/masks/manifests/debug maps.
9. Add artifact ignore rules.
10. Add logs and minimal automation tests.

Do not include foliage, ecology, weather, player map UI, survival mechanics, NPCs, VFX, final material art pass, or world dressing.

## Files And Areas Likely Touched

Exact project paths may differ, but likely areas are:

```text
Source/JungleGame/
Source/JungleGame/Terrain/
Source/JungleGame/Terrain/JGTerrainSource.*
Source/JungleGame/Terrain/JGTerrainCoordinates.*
Source/JungleGame/Terrain/JGTerrainTileTypes.*
Source/JungleGame/Terrain/JGTerrainTileMeshBuilder.*
Source/JungleGame/Terrain/JGTiledTerrainActor.*
Source/JungleGame/Terrain/JGTerrainValidation.*
Source/JungleGame/Terrain/JGTerrainExport.*
Source/JungleGame/Private/...
Source/JungleGame/Public/...
Config/DefaultEngine.ini
Config/DefaultGame.ini
.gitignore
Docs/ or PR5 research docs path
Content/Maps/L_TerrainResolutionValidation.umap   // optional developer-only map
```

If tests exist:

```text
Source/JungleGameTests/
Source/JungleGameTests/Terrain/
```

If commandlets/editor-only utilities are used:

```text
Source/JungleGameEditor/
Source/JungleGameEditor/Terrain/
```

Keep runtime modules separate from editor-only export/import helpers where possible.

## Acceptance Criteria

### Architecture acceptance

- Terrain generation has a canonical world-space source object.
- Runtime mesh generation samples the canonical source, not a duplicate algorithm.
- Height, island mask, beach mask, crater mask, ridge/gully masks, and slope diagnostics are aligned by coordinate.
- Current `129 x 129` path is either preserved as debug fallback or replaced by an equivalent macro debug LOD.

### Resolution acceptance

- The system can generate at least a 4 m near runtime tile mesh.
- The system can generate 8 m and 16 m lower LOD runtime tiles.
- The source/export path can target the `8129 x 8129` / 2 m Landscape-compatible grid, even if full Landscape import is deferred.
- Crater/beach validation can be run at 2 m and optionally 1 m local oversampling.

### Coastline acceptance

- Shoreline is derived from a canonical island SDF/mask.
- At the island perimeter, sampled height is 0 m within tolerance.
- Outside island mask, terrain is ocean/below sea level.
- Tile boundaries do not affect coastline shape.
- LOD changes do not move the shoreline.

### Seam acceptance

- Adjacent tiles share identical border source samples.
- Adjacent tile masks match on shared borders.
- Same-LOD borders have no cracks.
- Mixed-LOD borders are either avoided in first implementation or use a documented transition strategy.
- Seam validation output reports max seam delta.

### Collision acceptance

- Near/playable tiles have collision.
- Far visual tiles can disable collision.
- Collision cooking is asynchronous or otherwise demonstrated not to hitch traversal.
- Collision validation traces compare hit height to source height.
- Player cannot fall through tile seams.

### Export/validation acceptance

- Developer command can export heightfield and masks from the same source used by runtime mesh.
- Exports include a source fingerprint/hash.
- Runtime logs include terrain scale, LOD, tile, collision, seam, and shoreline stats.
- Debug artifacts are written under ignored paths.

### Artifact hygiene acceptance

- No generated heightfields, masks, seam maps, screenshots, caches, cooked data, or manifests are staged unless deliberately placed as tiny test fixtures.
- `.gitignore` or equivalent rules protect generated terrain outputs.
- PR diff/stat is inspected before commit.

## Risks And Stop Conditions

### Risk: single dense procedural mesh becomes unmaintainable

If implementation starts moving toward one giant procedural mesh with millions of vertices and full collision, stop and split into tiles first.

### Risk: coastline drift from LOD/mip filtering

If shoreline height differs between LODs or exported masks, stop and make the SDF/coastline clamp authoritative before adding more terrain features.

### Risk: tile-local noise creates seams

If adjacent tile borders disagree, stop all feature work and move noise/domain sampling into global source coordinates.

### Risk: collision cooking hitches

If terrain generation stalls during traversal due to collision cooking, stop increasing resolution and implement async/rate-limited collision updates first.

### Risk: Landscape dimension mismatch

If the exported heightfield requires awkward XY scaling or does not preserve `16.256 km`, stop and resolve dimensions before importing. The preferred final target is `8129 x 8129` at 2 m spacing.

### Risk: generated files enter git

If generated terrain outputs show up in `git status`, stop and fix ignore rules before continuing.

### Risk: visual terrain and collision terrain diverge

If collision is not sampled from the same canonical source, stop and unify source sampling before tuning gameplay traversal.

### Risk: scope creep

If the PR starts adding foliage, final materials, UI, weather, survival mechanics, NPCs, VFX, or art dressing, stop and split that work out.

## Rejected Or Forbidden Approaches

Rejected:

- replacing `129 x 129` with one massive full-island procedural mesh;
- generating tile heights in tile-local coordinates;
- using tile-edge falloff as island shape;
- using LOD-specific terrain algorithms that are not nested samples of the canonical source;
- relying on material normals to fake traversal-relevant gullies/ridges while collision remains coarse;
- generating full collision for every far tile;
- treating skirts as the coastline preservation method;
- committing large generated heightmaps/masks/debug captures;
- jumping to final Landscape import before source/export/shoreline validation exists.

Forbidden by scope:

- foliage design;
- ecology design;
- weather design;
- audio design;
- survival systems;
- NPC systems;
- VFX design;
- final material/art dressing;
- player-facing map/minimap/compass/GPS/objective/bearing/time/orientation UI.

## Sources And References

[^ue-landscape-technical]: Epic Games, **Landscape Technical Guide in Unreal Engine 5.8 Documentation**. Documents Landscape component sizing, recommended `8129 x 8129` size, component count, heightmap dimensions, accepted heightmap formats, and Z scale calculation. <https://dev.epicgames.com/documentation/unreal-engine/landscape-technical-guide-in-unreal-engine>

[^ue-landscape-overview]: Epic Games, **Landscape Overview in Unreal Engine 5.8 Documentation**. Documents Landscape memory advantages over Static Meshes, World Partition integration, continuous geo-mip LOD, height/weight data streaming, and heightfield collision. <https://dev.epicgames.com/documentation/unreal-engine/landscape-overview>

[^ue-heightmap-import]: Epic Games, **Importing and Exporting Landscape Heightmaps in Unreal Engine 5.8 Documentation**. Documents heightmap import/export formats, custom heightmap import, tiled heightmap import, and World Partition import settings. <https://dev.epicgames.com/documentation/unreal-engine/importing-and-exporting-landscape-heightmaps-in-unreal-engine>

[^ue-world-partition]: Epic Games, **World Partition in Unreal Engine 5.8 Documentation**. Describes World Partition as a distance-based large-world streaming system using grid cells and streaming sources. <https://dev.epicgames.com/documentation/unreal-engine/world-partition-in-unreal-engine>

[^ue-procmesh-api]: Epic Games, **UProceduralMeshComponent API Reference in Unreal Engine 5.8 Documentation**. Documents procedural mesh sections, async physics cooking, complex-as-simple collision, collision body setup, and collision helpers. <https://dev.epicgames.com/documentation/unreal-engine/API/Plugins/ProceduralMeshComponent/UProceduralMeshComponent>

[^ue-landscape-collision]: Epic Games, **Landscape Collision Guide in Unreal Engine 5.8 Documentation**. Documents Collision Mip Level, Simple Collision Mip Level, collision visualization, and per-component collision complexity adjustment. <https://dev.epicgames.com/documentation/unreal-engine/landscape-collision-guide-in-unreal-engine>

[^ue-nanite-landscape]: Epic Games, **Using Nanite with Landscapes in Unreal Engine 5.8 Documentation**. Documents Nanite Landscape behavior, build/update requirements, and the fact that Nanite Landscape data streams alongside existing Landscape data. <https://dev.epicgames.com/documentation/unreal-engine/using-nanite-with-landscapes-in-unreal-engine>

[^usgs-1m-dem]: U.S. Geological Survey, **1 meter Digital Elevation Models (DEMs) — 3DEP Downloadable Data Collection**. Describes one-meter DEMs as lidar-derived high-resolution bare-earth elevation surfaces. <https://data.usgs.gov/datacatalog/data/USGS%3A77ae0551-c61e-4979-aedd-d797abdcde0e>

[^usgs-30m-dem]: U.S. Geological Survey, **1 Arc-second Digital Elevation Models (DEMs) — 3DEP Downloadable Data Collection**. Describes 1 arc-second DEMs as approximately 30 m resolution bare-earth terrain data. <https://data.usgs.gov/datacatalog/data/USGS%3A35f9c4d4-b113-4c8d-8691-47c428c29a5b>
