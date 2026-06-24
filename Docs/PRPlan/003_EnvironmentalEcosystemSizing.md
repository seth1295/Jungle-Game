# Environmental Ecosystem Sizing

## Purpose

This document defines practical ecosystem sizing for the large-world first playable. Ecosystems must be sized from environmental and landform logic, not arbitrary game-biome blocks. The world frame is a rainforest/mountain island with ocean as the natural boundary.

## Source-Backed Model

Queensland's regional ecosystem framework is the main planning analogy. The Queensland Government defines regional ecosystems as vegetation communities in a bioregion consistently associated with geology, landform, and soil ([Queensland regional ecosystems](https://www.qld.gov.au/environment/plants-animals/plants/ecosystems/about)).

Queensland Herbarium mapping is also useful as a process model. Its ecosystem mapping uses field survey, aerial photos, satellite imagery, geology and soil mapping, and historical survey plans. It maps most of Queensland at 1:100,000, with some Wet Tropics and South East Queensland areas mapped more finely at 1:50,000 or 1:25,000 ([Queensland Herbarium survey and mapping](https://www.qld.gov.au/environment/plants-animals/plants/herbarium/mapping-ecosystems)).

For game implementation, this means:

- landform and substrate proxies matter;
- hydrology matters;
- vegetation is mapped as a pattern over terrain, not as square gameplay tiles;
- fine-scale local variation can sit inside broader ecosystem zones.

## Wet Tropics Inspiration

The large-world terrain should take inspiration from Wet Tropics logic: rugged topography, fast-flowing rivers, gorges, waterfalls, mountains, high seasonal rainfall, steep environmental gradients, rainforest structural diversity, and sharp transitions between rainforest and adjacent vegetation. The Australian Government Wet Tropics description supports these as defining place characteristics ([DCCEEW Wet Tropics](https://www.dcceew.gov.au/parks-heritage/heritage/places/world/wet-tropics)).

This is inspiration, not replication. The game should avoid copying protected or culturally specific places.

## Proposed In-Game Scale Bands

| Band | Proposed size | Purpose |
|---|---:|---|
| Microhabitat | 2 m to 20 m | Local rock, buttress-root, fallen-log, seep, pool, canopy-gap, animal-track, or ground-cover variation |
| Creek bank / riparian band | 15 m to 120 m from channel, variable by stream order and valley width | Wet edge, bank vegetation, water sound, unstable footing, directional affordance, high night risk |
| Minimum ecosystem patch | 100 m to 250 m across, roughly 1 ha to 6 ha | Small but readable vegetation/structure change that a player can notice inside first-person traversal |
| Primary ecosystem zone | 250 m to 1000 m across, roughly 6 ha to 100 ha | Main player-readable navigation units: wet valley, steep slope forest, ridge forest, lowland alluvial forest |
| Landform ecosystem block | 1 km to 4 km across | Basin, ridge system, gorge system, mountain shoulder, alluvial flat, or creek catchment unit |
| Macro region | 4 km to 16 km across | Whole-world organizing regions: mountain spine, lowland basin, gorge corridor, coastal edge, outer wet uplands |

## Island Edge Ecosystems

Island edges should be reachable, readable, and useful, but not always safe.

Required edge types:

- beach;
- mangrove;
- cliff coast;
- rocky shore;
- creek mouth;
- exposed headland;
- difficult coastal terrain.

Implementation guidance:

- Use ocean sound and surf direction as navigation evidence.
- Make coastline direction readable where the player can hear or see it.
- Do not make the coast a guaranteed safe road around the island.
- Beaches can be false affordances when they end in cliff, mangrove, tide-obstructed rock, or exposed headland.
- Creek mouths should connect interior hydrology to coastal navigation.
- Mangroves should indicate coast/estuary but create slow, confusing, wet movement.

These sizes are implementation targets, not ecological claims. They convert real mapping concepts into readable game scale.

## Riparian And Creek Bands

Riparian zones should be variable-width bands shaped by hydrology and valley form, not fixed painted strips. USDA describes riparian forest buffers as areas adjacent to streams, lakes, or wetlands containing trees, shrubs, or other perennial plants, with functions including filtering runoff, stabilizing banks, providing shade, wildlife habitat, and corridors ([USDA National Agroforestry Center](https://www.fs.usda.gov/nac/practices/riparian-forest-buffers.php)).

Game interpretation:

- small headwater creek: 15 m to 35 m high-wetness influence each side;
- persistent creek: 35 m to 80 m each side;
- gorge or flood-prone valley: 80 m to 120 m or wider where the valley floor supports it;
- wetness and canopy density should feather outward with slope, floodplain width, and flow accumulation.

## Why Ecosystem Masks Must Cross UE Component Borders

The proposed 8129 x 8129 landscape uses 32 x 32 Landscape Components. At 2 m per quad, each component is about 508 m wide.

If ecosystems align to components:

- the world will reveal square artificial boundaries;
- creeks will cut awkwardly across "biome cells";
- ridge and valley systems will not read naturally;
- PCG density changes will expose technical grid seams;
- navigation will teach the player to read editor layout instead of terrain.

Therefore:

- ecosystem masks must be generated in world space;
- masks must sample height, slope, aspect, creek distance, wetness, ridge/valley score, and land-zone proxy continuously;
- technical component boundaries should be debug overlays only;
- masks should be validated for continuity across component borders.

## Practical Patch Rules

Microhabitats:

- may be smaller than player navigation scale;
- should reinforce local detail and clue density;
- should not define macro traversal structure alone.

Riparian bands:

- should follow channels and valley bottoms;
- should widen in alluvial flats and narrow in gorges;
- should influence sound, visibility, footing, wetness, insects, and night danger.

Minimum ecosystem patches:

- should be large enough for a player to enter, notice, and leave;
- should not flicker every few meters unless it is explicitly microhabitat.

Primary ecosystem zones:

- should be the main units of traversal memory;
- should often outsize one UE Landscape Component and cross several World Partition cells.

Landform ecosystem blocks:

- should match catchments, ridges, basins, slopes, and gorge systems;
- should be visible in top-down debug and first-person navigation.

Macro regions:

- should shape the player's mental map without giving them a UI map;
- should be named internally for development, not necessarily exposed to the player.

## Traversal Affordance Sizing

Traversal affordances should be generated from landform and ecosystem conditions:

- ridges and spurs can create natural high-ground movement lines;
- saddles can create plausible crossings between basins or ridge systems;
- creek banks can create directional movement with mud, obstruction, and night-risk tradeoffs;
- gullies can look attractive while becoming false affordances that pull the player into steep, wet, or enclosed terrain;
- slope breaks can create readable contour-following movement;
- open understory and animal corridors can create local movement invitations without becoming authored trails.
- beaches can become false affordances when they end in cliffs, mangroves, exposed rock, or tidal/coastal hazards.

Do not make the first ecosystem pass a route-drawing pass. The first goal is passability, resistance, and affordance. Explicit visible paths should come later only when caused by erosion, animal movement, old disturbance, or human history.

## Validation Expectations

Ecosystem sizing validation should include:

- top-down mask debug view;
- first-person walk-through screenshots or clips;
- component-border continuity check;
- creek/ridge/valley transition check;
- patch-size histogram or summary if tooling supports it;
- written list of any intentionally small exceptions.
