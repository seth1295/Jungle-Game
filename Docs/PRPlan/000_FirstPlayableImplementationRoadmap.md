# First Playable Implementation Roadmap

## Purpose

This document defines the implementation chain from the current visible Cell 0 runtime blockout toward a packaged large-world first playable.

PR #6 is this docs-only planning PR. It must not implement gameplay, maps, assets, C++, Blueprints, project settings, generated files, or editor output. Its only output is Markdown documentation under `Docs/PRPlan/`.

## Final Target

The final target for this roadmap is a packaged large-world first playable:

- one 16 km x 16 km World Partition landscape target, roughly 256 km2;
- terrain shaped by mountains, ridges, valleys, creeks, basins, traversal affordances, and local landmarks;
- ecosystem masks derived from landform and hydrology logic;
- PCG-driven rainforest, riparian, slope, ridge, gorge, lowland, and disturbance passes;
- no player map, compass, minimap, bearing readout, GPS marker, HUD clock, or objective marker;
- navigation through diegetic environmental cues only: sun position, shadow direction/length, light colour, slope, creek flow, coastline/ocean if present, ridgelines, mountain silhouettes, vegetation/ecosystem changes, sound, weather, sky visibility, stars, moonlight, and landmarks;
- traversal affordances that emerge from ridges, spurs, saddles, basins, creek banks, gullies, slope breaks, and vegetation density;
- a serious night danger state;
- Cell 0 integrated as a real first-playable location inside the larger world;
- package, launch, smoke, and performance evidence.

This is not the full game. It does not require final art, final AI, final survival systems, final story delivery, final audio, final UX, full optimization, shipping save support, or full content density across every square meter. It proves that the game's core large-world structure can support first-person survival-navigation gameplay.

## Design Pillar

The implementation chain must protect this design pillar:

> terrain creates ecology; ecology creates navigation; navigation creates survival pressure.

Getting lost is gameplay. The player should learn the world by reading it, not by following a map overlay.

## Technical Spine

The large-world implementation should be built around Unreal Engine's large-world stack:

- World Partition for distance-based streaming of large map cells ([Epic World Partition](https://dev.epicgames.com/documentation/en-us/unreal-engine/world-partition-in-unreal-engine)).
- One File Per Actor for reviewable multi-actor editing in a large persistent level ([Epic OFPA](https://dev.epicgames.com/documentation/en-us/unreal-engine/one-file-per-actor-in-unreal-engine)).
- HLOD for distant simplified representations of streamed world content ([Epic World Partition HLOD](https://dev.epicgames.com/documentation/en-us/unreal-engine/world-partition---hierarchical-level-of-detail-in-unreal-engine)).
- Data Layers for editor organization and runtime environmental state where appropriate ([Epic Data Layers](https://dev.epicgames.com/documentation/en-us/unreal-engine/world-partition---data-layers-in-unreal-engine)).
- Landscape for the 16 km terrain foundation ([Epic Landscape Technical Guide](https://dev.epicgames.com/documentation/en-us/unreal-engine/landscape-technical-guide-in-unreal-engine)).
- PCG for controlled procedural placement and ecosystem iteration ([Epic PCG Framework](https://dev.epicgames.com/documentation/en-us/unreal-engine/procedural-content-generation-framework-in-unreal-engine)).

## Implementation Sequence

PR #7 through PR #20 are the planned implementation sequence:

| PR | Title | Output |
|---|---|---|
| #7 | 16x16 World Partition landscape foundation | Empty large-world level foundation and streaming setup |
| #8 | Landscape generation/import/tooling scaffold | Repeatable terrain import/generation workflow |
| #9 | Macro terrain: mountains, ridges, creek systems, camp basin | First readable landform pass |
| #10 | Ecosystem mask system | Terrain-derived masks and classification data path |
| #11 | Ecosystem debug visualisation and validation | Debug views and mask sanity checks |
| #12 | PCG biome framework | PCG graph/data framework without final density |
| #13 | Creek/riparian/wet valley ecosystem pass | Creek and wet-valley ecology pass |
| #14 | Ridge/slope/mountain ecosystem pass | Upland/ridge/slope ecology pass |
| #15 | Lowland/alluvial/gorge ecosystem pass | Lowland, alluvial, gorge, and transition ecology pass |
| #16 | Navigation pillars: sun, creeks, ridges, landmarks, camp smoke | Readable navigation cues |
| #17 | No-map/no-compass lost-state gameplay loop | Lost-state rules without map/compass fallback |
| #18 | Day/night/weather danger prototype | Time/weather danger-state prototype |
| #19 | Cell 0 first playable integration inside the large world | Cell 0 placed as a real world location |
| #20 | Packaged large-world first playable smoke, performance checks, and release notes | Packaged first-playable evidence |

Each PR must stay inside its stated scope and avoid generated/editor/cache output unless a later PR explicitly authorizes a binary/editor asset with a clear reason.

## Roadmap Boundaries

In scope:

- large-world technical foundation;
- repeatable landscape workflow;
- ecosystem mask and PCG strategy;
- navigation and traversal cues that arise from terrain/ecology;
- first-pass survival pressure from being lost, darkness, weather, and route uncertainty;
- first-playable validation and packaging.

Out of scope:

- full game completion;
- final visual target;
- final asset library;
- final creature or hominin AI;
- combat systems;
- minimap, player map, compass, bearing readout, GPS marker, HUD clock, objective marker, or modern navigation gear;
- exhaustive world content coverage;
- shipping performance guarantees beyond first-playable smoke/performance evidence.

## Validation Standard

Every implementation PR should include:

- clean `git status`;
- `git diff --stat` and full diff inspection before staging;
- validation appropriate to the changed area;
- explicit generated/editor/cache exclusion;
- package/cook/smoke evidence when the PR claims packaged or runtime readiness;
- stop/ask notes where the next change would cross scope.
