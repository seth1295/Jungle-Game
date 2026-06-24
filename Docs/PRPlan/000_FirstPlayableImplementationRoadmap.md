# First Playable Implementation Roadmap

## Purpose

This document defines the implementation chain from the current visible Cell 0 runtime blockout toward a packaged large-world first playable. The large world is the primary object. The plan must not frame the game as "spawn at camp and explore outward."

PR #6 is this docs-only planning PR. It must not implement gameplay, maps, assets, C++, Blueprints, project settings, generated files, or editor output. Its only output is Markdown documentation under `Docs/PRPlan/`.

## Final Target

The final target for this roadmap is a packaged large-world first playable:

- one 16 km x 16 km rainforest/mountain island world target, roughly 256 km2;
- ocean as the natural world boundary, with no invisible walls as the primary edge solution;
- reachable, readable, useful coastline that is not always safe;
- island edge types including beach, mangrove, cliff coast, rocky shore, creek mouth, exposed headland, and difficult coastal terrain;
- interior terrain shaped by mountains, ridges, valleys, creeks, gorges, basins, traversal affordances, and local landmarks;
- ecosystem masks derived from landform and hydrology logic;
- PCG-driven rainforest, riparian, slope, ridge, gorge, lowland, and disturbance passes;
- no player map, minimap, compass, bearing number/readout, GPS marker, objective marker navigation, HUD clock, or explicit time readout;
- navigation through diegetic environmental cues only: sun position, shadow direction/length, light colour, slope, creek flow, coastline/ocean if present, ridgelines, mountain silhouettes, vegetation/ecosystem changes, sound, weather, sky visibility, stars, moonlight, and landmarks;
- traversal affordances that emerge from ridges, spurs, saddles, basins, creek banks, gullies, slope breaks, and vegetation density;
- valid semi-random world-location spawn support as a future target, so camp, Cell 0, intro sequences, authored shelters, and story anchors do not define the whole world structure;
- player-made trail marks, weather-driven navigation pressure, sound-based navigation, sky windows, terrain fatigue, practical footing/injury risk, ecosystem tells, false affordances, and wait-a-while terrain friction documented for later implementation;
- a serious night danger state;
- authored locations, including Cell 0, placed inside the world later rather than used to fake the world;
- package, launch, smoke, and performance evidence.

This is not the full game. It does not require final art, final AI, final survival systems, final story delivery, final audio, final UX, full optimization, shipping save support, or full content density across every square meter. It proves that the game's core large-world structure can support first-person survival-navigation gameplay.

## Design Pillar

The implementation chain must protect this exact design pillar:

```text
Terrain creates ecology.
Ecology creates navigation.
Navigation creates survival pressure.
Getting lost is gameplay.
The world is readable, but only if the player learns how to read it.
Reward environmental literacy, not UI literacy.
```

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
| #9 | Macro terrain: island mountains, ridges, creek systems, basins, coastline | First readable island landform pass |
| #10 | Ecosystem mask system | Terrain-derived masks and classification data path |
| #11 | Ecosystem debug visualisation and validation | Debug views and mask sanity checks |
| #12 | PCG biome framework | PCG graph/data framework without final density |
| #13 | Creek/riparian/wet valley ecosystem pass | Creek and wet-valley ecology pass |
| #14 | Ridge/slope/mountain ecosystem pass | Upland/ridge/slope ecology pass |
| #15 | Lowland/alluvial/gorge ecosystem pass | Lowland, alluvial, gorge, and transition ecology pass |
| #16 | Navigation pillars: sun, sky, sound, coast, creeks, ridges, landmarks | Readable diegetic navigation cues |
| #17 | No-map/no-compass lost-state gameplay loop | Lost-state rules without map/compass fallback |
| #18 | Day/night/weather danger prototype | Time/weather danger-state prototype |
| #19 | Cell 0 first playable integration inside the large world | Cell 0 placed as a real world location without defining the whole world |
| #20 | Packaged large-world first playable smoke, performance checks, and release notes | Packaged first-playable evidence |

Each PR must stay inside its stated scope and avoid generated/editor/cache output unless a later PR explicitly authorizes a binary/editor asset with a clear reason.

## Roadmap Boundaries

In scope:

- large-world technical foundation;
- repeatable landscape workflow;
- ecosystem mask and PCG strategy;
- navigation and traversal cues that arise from terrain/ecology;
- island boundary/coastline planning;
- valid future semi-random spawn assumptions;
- first-pass survival pressure from being lost, darkness, weather, and traversal uncertainty;
- first-playable validation and packaging.

Out of scope:

- full game completion;
- final visual target;
- final asset library;
- final creature or hominin AI;
- combat systems;
- minimap, player map, compass, bearing number/readout, GPS marker, objective marker navigation, HUD clock, explicit time readout, or modern navigation gear;
- orientation confidence systems, hidden orientation meters, character comments that tell the player whether they are oriented, or UI-like orientation feedback;
- invisible walls as the primary island-edge solution;
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
