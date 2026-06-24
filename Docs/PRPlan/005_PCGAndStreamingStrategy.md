# PCG And Streaming Strategy

## Purpose

PCG should turn ecosystem masks into world content without hiding design intent. It must support large-world iteration, World Partition streaming, Data Layer organization, HLOD generation, and first-person readability.

Epic describes PCG as an extensible framework for procedural content inside Unreal Engine, including tools that can range from asset utilities to biome generation and entire worlds ([Epic PCG Framework](https://dev.epicgames.com/documentation/en-us/unreal-engine/procedural-content-generation-framework-in-unreal-engine)).

## PCG Inputs

PCG graphs should consume stable mask inputs:

- height;
- slope;
- aspect;
- creek distance;
- flow accumulation / wetness;
- ridge / valley score;
- soil / geology / land-zone proxy;
- canopy density;
- disturbance / regrowth;
- visibility density;
- night danger.
- traversal blocker / affordance.

PCG outputs should remain deterministic enough to review and reproduce.

PCG must not treat explicit authored trails as the first-order design method. It should place obstruction, vegetation, ground clutter, animal traces, erosion traces, and disturbance in response to terrain-derived traversal masks.

## Creek / Riparian / Wet Valley Ecosystems

Purpose:

- make creeks real navigation structures;
- make water sound and flow direction useful;
- make creek banks attractive but risky traversal affordances;
- make wet valleys denser, darker, louder, and harder to leave at night.

PCG should place:

- creek-edge vegetation bands;
- roots, fallen logs, bank clutter, wet rocks, and crossing candidates;
- denser fern and understory zones;
- sound emitters or audio volume anchors;
- visibility breaks along bends and gully turns;
- high night-danger areas near unstable wet banks.
- occasional animal or erosion traces only where terrain and wetness justify them.

Validation:

- creek influence follows creek geometry, not square cells;
- creek banks expose hard blockers, soft blockers, affordance zones, and false affordances clearly;
- crossings are legible but not everywhere;
- first-person player can infer upstream/downstream where intended.

## Ridge / Slope / Mountain Ecosystems

Purpose:

- create high-ground navigation;
- preserve mountain silhouettes and ridge memory;
- make slope exposure and traversal difficulty meaningful.

PCG should place:

- thinner or wind-shaped canopy on exposed ridges;
- denser sheltered-slope vegetation where aspect/wetness supports it;
- rock, root, and fallen-tree obstacles by slope class;
- landmark trees, notches, boulders, or skyline anchors;
- limited long-view windows.
- affordance-zone cues on ridges, spurs, saddles, and slope breaks without drawing trail lines first.

Validation:

- ridges can be followed and recognized;
- slope class affects traversal and visibility;
- spurs, saddles, and slope breaks create discovered movement options;
- skyline landmarks survive HLOD/proxy checks;
- steep areas do not become accidental player traps unless deliberately marked as danger.

## Lowland / Alluvial / Gorge Ecosystems

Purpose:

- provide basin movement structure, floodplain transitions, and gorge compression;
- vary rainforest structure through landform and wetness;
- create areas where sound, sightlines, and movement options change sharply.

PCG should place:

- broader canopy and larger trunks in stable lowland/alluvial zones;
- denser debris and wet vegetation in flood-prone areas;
- cliff/gorge wall exclusion or specialized placement rules;
- constrained movement corridors through gorge systems;
- false affordance gullies, dry creek beds, and misleading clearings where terrain supports them;
- major landmark transitions where a player can update their mental map.

Validation:

- lowlands are not flat empty filler;
- gorge zones feel constrained and directional;
- alluvial areas read as wetter/flatter without becoming arbitrary swamps;
- performance stays within streaming and draw-call budgets.

## World Partition Strategy

World Partition streams grid cells around streaming sources, such as the player ([Epic World Partition](https://dev.epicgames.com/documentation/en-us/unreal-engine/world-partition-in-unreal-engine)). PCG output must be compatible with this streaming behavior.

Rules:

- keep PCG generation scoped by region or volume where possible;
- avoid a single graph that forces world-scale regeneration for small changes;
- validate cell load/unload behavior after dense PCG passes;
- keep always-loaded managers minimal;
- avoid actor references that force unrelated cells to load together.

## Data Layer Strategy

Data Layers should organize content without becoming a hidden streaming burden. Epic warns that loading too many assets with Data Layers can cause performance issues ([Epic Data Layers](https://dev.epicgames.com/documentation/en-us/unreal-engine/world-partition---data-layers-in-unreal-engine)).

Candidate layers:

- editor debug masks;
- blockout/reference;
- creek ecosystem pass;
- ridge/slope/mountain pass;
- lowland/alluvial/gorge pass;
- night/weather prototype where runtime layer toggling is justified;
- Cell 0 integration.

Rules:

- prefer Editor Data Layers for organization and debug;
- use Runtime Data Layers only for actual gameplay state;
- document any runtime layer's load budget and activation conditions.

## HLOD Strategy

HLOD must preserve navigation readability. Distant ridges, mountain silhouettes, canopy masses, smoke columns, and major landmarks should remain stable as cells stream.

Rules:

- generate HLODs after major PCG density stabilizes;
- validate from known navigation viewpoints;
- check that HLOD does not erase camp smoke or mountain silhouettes;
- check that HLOD does not invent misleading landmarks.
- check that no HLOD, debug, or UI artifact becomes a player-facing map, compass, minimap, bearing readout, GPS marker, HUD clock, or objective marker substitute.

## Performance Risks

- PCG density can overload foliage, collision, shadow, and HLOD budgets.
- Dense rainforest can destroy sightlines and frame rate simultaneously.
- Runtime Data Layers can load too many assets.
- Creek and gorge zones can concentrate expensive detail in small streamed regions.
- Collision on small plants/rocks can become more expensive than visible geometry.

## Validation Checks

Each PCG pass should include:

- `git diff --stat` and full diff inspection;
- generated/cache/editor junk check;
- PCG graph/output scope summary;
- top-down debug screenshot or exported validation notes;
- first-person traversal check;
- World Partition cell load/unload check;
- HLOD impact note if relevant;
- performance note from representative dense areas.
