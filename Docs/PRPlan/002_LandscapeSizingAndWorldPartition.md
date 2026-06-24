# Landscape Sizing And World Partition

## Target

The large-world first playable targets a 16 km x 16 km landscape, roughly 256 km2.

The preferred research target is an 8129 x 8129 heightmap at 2 m horizontal scale:

```text
8129 vertices per side
8128 quads per side
2 m per quad
8128 * 2 m = 16,256 m
world footprint = 16.256 km x 16.256 km
approximate area = 264.3 km2
```

This slightly exceeds the nominal 16 km x 16 km target while using an Unreal-recommended landscape size.

## Why 8129 x 8129

Epic's Landscape Technical Guide lists 8129 x 8129 as a recommended landscape size using:

- 127 quads per section;
- 4 subsections per component, arranged 2x2;
- 254 x 254 quads per Landscape Component;
- 1024 total Landscape Components, arranged 32 x 32.

Source: [Epic Landscape Technical Guide](https://dev.epicgames.com/documentation/en-us/unreal-engine/landscape-technical-guide-in-unreal-engine).

At 2 m per quad:

```text
component width = 254 quads * 2 m = 508 m
component grid = 32 x 32
total width = 508 m * 32 = 16,256 m
```

This fits the requested world size while keeping the component count at Epic's documented large-landscape ceiling of 1024 components.

## Component And Subsection Implications

Unreal Landscape dimensions are constrained by component and subsection layout. Epic documents that component height data is stored in textures, section sizes must be power-of-two vertices, and overall dimensions are calculated from component count and quads per component ([Epic Landscape Technical Guide](https://dev.epicgames.com/documentation/en-us/unreal-engine/landscape-technical-guide-in-unreal-engine)).

The proposed setup:

| Item | Value |
|---|---:|
| Heightmap | 8129 x 8129 |
| Horizontal scale | 2 m per quad |
| Sections per component | 2 x 2 |
| Quads per section | 127 |
| Quads per component | 254 x 254 |
| Component size | 508 m x 508 m |
| Components | 32 x 32 |
| Landscape width | 16.256 km |

Design consequence:

- A single component is too large to define an ecosystem by itself.
- A single ecosystem zone may span many components.
- Creek bands and ridge masks must cross component borders.
- Debug views should show both ecological boundaries and technical component/grid boundaries so accidental alignment is visible.

## World Partition Requirement

World Partition is required because the target world is too large for a traditional always-loaded level. Epic describes World Partition as an automatic data-management and distance-based streaming system that stores a world in a persistent level divided into grid cells and streams cells around streaming sources ([Epic World Partition](https://dev.epicgames.com/documentation/en-us/unreal-engine/world-partition-in-unreal-engine)).

Large-world first playable requirements:

- player-centered streaming;
- editor ability to load and unload regions;
- debugging of streaming cells;
- cook/package validation for a partitioned world;
- ability to keep the whole world in context without requiring the whole world to be loaded.

## One File Per Actor Requirement

One File Per Actor is required because large-world development will create many placed actors, PCG volumes, debug actors, data-layer actors, and landmarks. Epic notes that with OFPA, actors are saved to individual files, reducing the need to check out the main level file for actor changes ([Epic OFPA](https://dev.epicgames.com/documentation/en-us/unreal-engine/one-file-per-actor-in-unreal-engine)).

Planning consequence:

- PRs must keep actor changes scoped.
- Review reports must list any binary actor/map files when they eventually become unavoidable.
- Docs-only PR #6 must not add OFPA actor files.

## HLOD Requirement

HLOD is required because 16 km sightlines, ridge views, mountain silhouettes, and distant canopy masses must remain visually legible without loading full-detail content everywhere. Epic documents World Partition HLOD generation through HLOD Actors and HLOD Layers ([Epic World Partition HLOD](https://dev.epicgames.com/documentation/en-us/unreal-engine/world-partition---hierarchical-level-of-detail-in-unreal-engine)).

Validation expectations:

- distant mountains and ridges remain recognizable;
- HLOD generation does not explode package size;
- HLOD artifacts do not create false navigation landmarks;
- HLOD checks are included before PR #20 release notes.

## Data Layers Requirement

Data Layers are required for editor organization and for selected runtime environmental states. Epic describes Data Layers as a World Partition system for organizing actors in editor and runtime, with runtime layers controllable through Blueprint or C++ ([Epic Data Layers](https://dev.epicgames.com/documentation/en-us/unreal-engine/world-partition---data-layers-in-unreal-engine)).

Likely uses:

- editor-only debug layers;
- ecosystem validation overlays;
- temporary blockout/reference layers;
- night/weather danger prototype layers if runtime toggling is justified;
- camp smoke or first-playable integration layers only when they are real gameplay state.

Risk:

- Data Layers can hurt streaming performance if widely used runtime layers pull in too many assets. Epic explicitly warns that loading too many assets with Data Layers can cause performance issues ([Epic Data Layers](https://dev.epicgames.com/documentation/en-us/unreal-engine/world-partition---data-layers-in-unreal-engine)).

## Risks

- 8129 x 8129 at 2 m scale may be heavy for local editor iteration.
- Landscape import may produce binary/editor files that require careful review discipline.
- Component count sits at the documented 1024 large-landscape maximum.
- PCG density can exceed streaming and draw-call budgets quickly.
- HLOD can produce misleading silhouettes if generated from unfinished proxies.
- The technical grid may tempt square biome design; ecosystem masks must resist that.

## Validation Expectations

PR #7 should validate:

- clean World Partition level creation;
- 8129 x 8129 / 2 m scale decision or documented fallback;
- component and subsection settings recorded;
- no unrelated generated/cache files staged;
- world opens or commandlet-validates;
- World Partition streaming/debug view can be inspected;
- landscape dimensions are measured in editor or by deterministic tooling.
