# Macro Island Terrain Navigation Research

## Executive Summary

Jungle Game should treat the island as the primary navigation system. The player should not be guided by a map, compass, bearing readout, objective marker, HUD clock, or orientation-confidence UI. Instead, orientation should emerge from repeated contact with macro landforms: a mountain/upland spine, catchments, creek heads, ridges, spurs, saddles, gullies, basins, coastlines, mangrove transitions, beaches, cliff coasts, rocky headlands, and skyline forms.

The strongest design principle is **hydrological legibility**: water, slope, ridge, vegetation density, mud, rock exposure, sound, and light should agree with one another. Real wet tropical terrain is not random steep noise. In the FNQ Wet Tropics reference environment, rugged topography, rivers, gorges, waterfalls, mountains, rainforest coastline, mangroves, and steep environmental gradients are defining features. The Wet Tropics are described by the Australian Government as rugged, rivered, gorge-cut, waterfall-rich, mountainous, and ecologically diverse; terrain there is tied to rainforest structure, coastal transitions, and hydrology rather than arbitrary arena layout. ([dcceew.gov.au](https://www.dcceew.gov.au/parks-heritage/heritage/places/world/wet-tropics))

The island should be generated and authored around a small number of physically meaningful systems:

- **An upland spine** that defines the island’s main watershed divides.
- **Primary catchments** that drain from the spine to different coastal outlets.
- **Secondary ridges and spurs** that let the player infer direction by slope and drainage.
- **Creek corridors** that are reliable but costly navigation routes.
- **Saddles and passes** that offer crossing points between catchments.
- **Basins and swampy lowlands** that collect water and create slow, ambiguous traversal.
- **Coastal sectors** that are visually distinct but not a clean circular path.
- **Vegetation, sound, and light gradients** that reinforce landform identity.

For PR5 Batch 001 / File 001, the immediate target should be a **macro terrain evidence specification**, not a full production terrain pipeline. Implement now: landform taxonomy, terrain channels, debug evidence points, traversal categories, and a first island blockout with coherent catchments. Defer: learned terrain generation, final erosion simulation, dynamic seasonal hydrology, full biome placement, and any InfiniteDiffusion/Terrain Diffusion integration.

InfiniteDiffusion / Terrain Diffusion is valuable as a future architecture reference because it argues for learned terrain generation with seed consistency, lazy/unbounded evaluation, constant-time random access, and a procedural-noise-like interface. The project page and arXiv abstract present it as an emerging learned procedural terrain approach, not as a proven UE5 production dependency for this project. ([xandergos.github.io](https://xandergos.github.io/terrain-diffusion/))

## Design Goals For Jungle Game

### Core terrain goal

The island should be navigable because it is **geographically coherent**, not because the player has UI assistance. Every major navigation decision should be explainable in terrain terms:

- “This creek must drain toward the coast.”
- “This ridge separates two catchments.”
- “That saddle likely crosses into the next valley.”
- “The mangroves mean tidal lowland, not inland upland.”
- “The sound of surf means the coast is near, but cliffs may block descent.”
- “This spur should descend toward a creek junction.”
- “This basin will slow me down and may hide line-of-sight.”

### Required player-facing constraints

Do not implement or imply any of the following:

- Player-facing map.
- Minimap.
- Compass.
- Bearing number or readout.
- GPS.
- Objective marker navigation.
- HUD clock.
- Explicit time readout.
- Orientation confidence UI.
- Hidden orientation meter.
- Character comments that tell the player whether they are oriented.

### Positive replacement systems

Use these instead:

- Landform memory.
- Creek sound and flow direction.
- Skyline silhouettes.
- Slope direction.
- Vegetation transitions.
- Ground wetness and mud.
- Beach/coast material change.
- Rock type and outcrop shape.
- Wind/surf/rain audio gradients.
- Sunlight direction and canopy break quality, without converting it into a UI clock.
- Repeated view corridors from ridges, saddles, creek openings, cliff edges, and beaches.

### Design phrase

**The world tells the truth, but never tells the answer.**

The terrain should provide evidence. It should not interpret that evidence for the player.

## Real-World Landform Principles

### Drainage defines readable island structure

A wet tropical island should be organized around drainage. Rivers, streams, and creeks flow from higher elevation to lower elevation under gravity; small creeks merge into larger streams and rivers, and ridges of higher land separate valleys and drainage basins. ([usgs.gov](https://www.usgs.gov/water-science-school/science/rivers-streams-and-creeks))

Recommended rule:

> Every significant valley, gully, and creek corridor must belong to a named catchment, and every catchment must have a plausible outlet.

Implementation implication:

- Do not scatter creeks as decorative spline lines.
- Do not allow creek segments to flow uphill unless there is an explicit local obstruction, pool, waterfall lip, or wet-season overflow.
- Do not build isolated “cool creek areas” that do not connect to catchment logic.
- Each creek head should make sense as a headwater area below a ridge, saddle, seep, spring zone, or wet upland hollow.

### Ridges and divides are navigation skeletons

Ridges and hills separate watersheds. In first person, the player does not need a contour map to benefit from this; they need consistent slope, vegetation, skyline, and drainage behavior. Orienteering and bushwalking references describe saddles as low points between higher points, gullies as small water-cut valleys, spurs as sloping ridges jutting from hillsides, and ridges as high lines separating lower ground. These forms are useful because they connect into coherent systems rather than isolated landmarks. ([act.orienteering.asn.au](https://act.orienteering.asn.au/resources/skills/advanced-skills/lesson-5-introduction-to-contours/))

Recommended rule:

> A ridge is not just a high strip. It is a divider, a route, a skyline object, and a generator of spurs and gullies.

### Terrain controls ecology

Terrain affects soil moisture, local weather patterns, microclimates, vegetation distribution, ecosystem processes, and biodiversity. Queensland WetlandInfo explicitly notes that terrain influences soil types, soil moisture, vegetation distribution, local weather, and microclimates. ([wetlandinfo.detsi.qld.gov.au](https://wetlandinfo.detsi.qld.gov.au/wetlands/ecology/components/terrain/))

Recommended rule:

> Foliage placement must be terrain-informed. The same jungle mesh density everywhere will destroy navigation evidence.

Terrain-to-ecology expectations:

- Creek corridors: wetter ground, darker soil, denser riparian vegetation, more insects/amphibian audio, more moss/fern cues.
- Ridges: better drainage, more root exposure, occasional rock, thinner understorey in wind-exposed or drier sections.
- Saddles: compressed passage, animal/hominin crossing plausibility, mixed drainage direction.
- Basins: standing water, mud, poor sightlines, slow movement.
- Headlands: wind shear, exposed rock, lower or distorted coastal vegetation.
- Mangroves: tidal mud, root mazes, salinity transition, limited dry footing.

### Wet tropical coastlines should vary

FNQ-inspired coastlines should not be clean circular racetracks. The Wet Tropics reference landscape includes rainforest coastline, beaches, rocky/coastal scenery, mangroves, gorges, and river outlets. Mangroves are intertidal communities found along tropical/subtropical coastal rivers, estuaries, and bays, with zonation influenced by salinity, oxygen, nutrients, and wave energy. ([dcceew.gov.au](https://www.dcceew.gov.au/parks-heritage/heritage/places/world/wet-tropics))

Coastal erosion and slope failure are natural processes on shorelines, especially where waves, currents, storms, heavy rainfall, cliff undercutting, and saturated soils affect coastal landforms. ([ga.gov.au](https://www.ga.gov.au/education/natural-hazards/coastal-erosion))

Recommended rule:

> The coastline should be a chain of different navigation propositions, not a universal safe perimeter path.

## Recommended Island Landform Set

### Macro island layout

Use one primary island mass with a strong asymmetric upland structure:

1. **Central or offset upland spine**
   - Not perfectly centered.
   - Runs along a long axis of the island.
   - Creates distinct windward/leeward and wet/drier-feeling sides.
   - Produces recognizable skyline silhouettes from beaches, river mouths, and ridges.

2. **Primary summit or summit cluster**
   - One dominant high point or clustered high knobs.
   - Visible only from selected breaks: beach openings, ridge crests, river mouths, cliffs, canopy gaps.
   - Should not be constantly visible from dense jungle floor.

3. **Primary watershed divides**
   - Main ridgeline separates major catchments.
   - Secondary ridges branch from it.
   - Each branch ridge throws spurs downslope.

4. **Catchment fan**
   - 5–9 primary catchments are enough for a medium island.
   - Each catchment should have a distinct outlet: beach creek, mangrove creek, cliff waterfall, rocky cove, swamp lagoon, or tidal estuary.

5. **Coastal variation ring**
   - Beaches where sediment can plausibly accumulate.
   - Cliff coasts where steep terrain reaches the ocean.
   - Rocky headlands where resistant rock forms projection points.
   - Mangrove/tidal flats in sheltered low-energy embayments and creek mouths.
   - Boulder beaches or cobble coves below cliffs.

### Landform category names

Recommended `EJGLandformClass` names:

```text id="jhqzfd"
UplandSpine
PrimaryRidge
SecondaryRidge
Spur
Saddle
Knoll
SummitShoulder
Headwall
CreekHead
Gully
Ravine
ValleyFloor
CreekCorridor
CreekConfluence
WaterfallDrop
PlungePool
AlluvialBench
FloodplainFlat
Basin
SwampBasin
ColluvialFootslope
BoulderField
RockOutcrop
Beach
BeachRidge
DuneBackshore
TidalFlat
MangroveFlat
TidalCreek
EstuaryMouth
RockyHeadland
CliffCoast
CoastalTerrace
LagoonEdge
```

### Macro landform target table

| Landform | Navigation role | Traversal role | Evidence cues |
|---|---:|---:|---|
| Upland spine | Island-scale orientation anchor | Hard/soft depending slope | Skyline, high wind audio, thinner soil, rock exposure |
| Primary ridge | Catchment divider | Affordance route with risk | Long crest, side slopes falling away, intermittent views |
| Spur | Downslope route from ridge to creek | Affordance or neutral friction | Nose of land, drainage on both sides, changing creek sound |
| Saddle | Catchment crossing point | High-value affordance | Lower point between rises, wind funnel, trail-like compression |
| Creek head | Start of drainage evidence | Soft blocker in wet season | Seepage, fern/moss, converging gullies |
| Gully | Directional slope evidence | Soft blocker / false affordance | V-shaped floor, leaf litter, slippery sides, water after rain |
| Creek corridor | Reliable route and water evidence | Affordance + friction | Flow sound, exposed roots, banks, confluences |
| Basin | Local disorientation pocket | Soft blocker | Flat wet floor, poor sightlines, repeated vegetation |
| Mangrove flat | Coastal/tidal transition | Hard/soft blocker | Root maze, mud, tidal creek, salinity vegetation shift |
| Beach | Coastal orientation reset | Affordance | Surf, open sky, sand, driftwood line |
| Cliff coast | Edge definition | Hard blocker | Surf below, vertical drop, wind, rock face |
| Rocky headland | Landmark and coast segmentation | Affordance/risk | Exposed rock, altered vegetation, views along coast |

## Terrain Navigation Evidence

### Evidence hierarchy

Navigation evidence should be layered from macro to micro:

1. **Macro evidence**
   - Upland skyline.
   - Coastline type.
   - Main catchment direction.
   - Major ridge/valley relationship.

2. **Meso evidence**
   - Spur/gully rhythm.
   - Creek junctions.
   - Saddle crossings.
   - Basin edges.
   - Slope breaks.

3. **Micro evidence**
   - Root direction and exposure.
   - Mud depth.
   - Water noise.
   - Fern/moss density.
   - Leaf litter movement.
   - Rock wetness.
   - Canopy gap shape.
   - Insect/frog/bird sound zones.

### Mountain/upland spine behavior

The upland spine should create the island’s primary logic.

Required outputs from the spine:

- **Catchment IDs:** each side of the spine drains to different coastal outlets.
- **Creek heads:** placed below saddles, headwalls, seep zones, and concave hollows.
- **Primary ridges:** long high routes separating major basins.
- **Secondary spurs:** descending from ridges toward creek corridors.
- **Saddles:** low crossing points between high knobs or ridge sections.
- **Basins:** concave low-energy areas where water and sediment accumulate.
- **Slope breaks:** transitions from steep upland to gentler footslope or valley floor.
- **Skyline landmarks:** distinctive high silhouettes visible from selective points.

Recommended authoring check:

> From any major creek, walking upstream should eventually clarify the headwall/ridge system; walking downstream should eventually clarify the coast/outlet system.

### Ridges

Ridges should support no-map orientation by:

- Providing higher, drier, more open movement than valley floors.
- Letting the player occasionally see other landforms through canopy breaks.
- Separating creek sounds: water should be audible on one side, both sides, or neither depending crest position.
- Creating a meaningful choice: stay high and risk exposure/steep descent later, or drop to creek and risk slow wet travel.

Avoid:

- Perfectly smooth ridges.
- Razorback ridges everywhere.
- Ridges with no relation to water.
- Ridges that terminate in arbitrary cliffs without geological reason.

### Spurs

Spurs are crucial because they are first-person-readable descent routes.

Use spurs to:

- Lead from ridges to creek junctions.
- Offer safer descent than gullies.
- Create “is this the right spur?” uncertainty without UI.
- Provide repeated but varied landform rhythm: ridge → spur → creek → opposite spur.

A good spur should have:

- A rounded or narrowing nose.
- Drainage or gullies on both sides.
- Slightly better footing than adjacent gullies.
- Vegetation that changes as it approaches wetter lower ground.

### Saddles

Saddles should be rare enough to matter.

Use saddles as:

- Catchment crossing points.
- Natural movement funnels.
- Good locations for subtle nonviolent relict hominin environmental tests.
- Places where wind, sound, and drainage ambiguity become noticeable.

A saddle should tell the player:

- “I am between two higher points.”
- “Water could drain away in two different directions.”
- “Crossing here may put me into a new valley.”

Do not label or explain this. Make it visible through terrain.

### Valleys, gullies, and ravines

Valleys and gullies should provide strong direction evidence but high traversal pressure.

Use:

- **Broad valleys** for slower, more open catchment floors.
- **Narrow gullies** for slippery, enclosed, confusing routes.
- **Ravines** for hard blockers or one-way descent risk.
- **Creek confluences** as memory landmarks.
- **Waterfall drops** as blockers, sound landmarks, and vertical transitions.

In wet tropical terrain, gullies should not just be visual trenches. They should be tied to wetness, vegetation, erosion, and sound.

### Basins and swamp basins

Basins should be local disorientation pockets.

Use basins to:

- Interrupt ridge/creek certainty.
- Create slow movement and repeated-looking vegetation.
- Hold standing water after rain.
- Make sound direction less reliable due to insects, frogs, rain, and dense vegetation.
- Create false exits that only reveal themselves as basin loops.

Basins must still have plausible overflow or drainage. A basin with no hydrological explanation will feel like a level-design arena.

### Creek corridors

Creeks are the most important natural navigation affordance.

They should:

- Flow downhill.
- Join other creeks.
- Change sound by flow rate, gradient, rock, and obstruction.
- Become more substantial downstream.
- Lead to different coast types depending catchment.
- Offer water, route memory, and environmental evidence, while also slowing movement.

Creeks should not always be safe highways. They can include:

- Slippery boulders.
- Dense lawyer cane / vine tangles where appropriate.
- Bank collapses.
- Deep pools.
- Waterfall lips.
- Flood debris.
- Mosquito/insect zones.
- Muddy banks.
- Deadfall jams.

### Coastlines

The coast should be a navigation reset, not an automatic solution.

Coastal types:

- **Open beach:** easier movement, strong surf audio, open sky, but exposed and possibly interrupted by headlands.
- **Pocket beach:** local reset; may not connect to the next beach.
- **Cliff coast:** confirms edge of island but blocks travel.
- **Rocky headland:** strong landmark; may allow views but force scrambling or inland bypass.
- **Mangrove coast:** strong tidal evidence but slow, muddy, maze-like, and poor for straight-line travel.
- **Estuary mouth:** high-value orientation node; creek + tide + coast meet.
- **Lagoon edge:** ambiguous; may look like coast but behave as enclosed water.

Mangrove wetlands in Queensland are intertidal, often muddy/anaerobic, influenced by salinity and tidal inundation, and can form linear, branching, or block-like patterns depending coast, tidal creeks, streams, flats, deltas, and shelter. ([wetlandinfo.detsi.qld.gov.au](https://wetlandinfo.detsi.qld.gov.au/resources/static/pdf/resources/fact-sheets/profiles/p01867aa.pdf))

## Traversal Category Mapping

### Traversal categories

Recommended `EJGTraversalClass` values:

```text id="jhvbgd"
HardBlocker
SoftBlocker
Affordance
FalseAffordance
NeutralFriction
HazardousAffordance
OneWayPressure
DebugOnly
```

### Category definitions

| Category | Definition | Terrain examples | Design rule |
|---|---|---|---|
| HardBlocker | Cannot pass without alternate route | Vertical cliff, deep ravine, impassable mangrove root wall, waterfall drop | Must be visibly credible before contact |
| SoftBlocker | Passable but slow/costly/risky | Mud basin, dense vine patch, slippery gully, steep wet slope | Must communicate cost through ground, foliage, audio, animation |
| Affordance | Naturally invites movement | Ridge crest, spur nose, dry creek bank, beach, saddle | Must not become a glowing path replacement |
| FalseAffordance | Looks useful but becomes costly or loops | Dry-looking basin crust, gully shortcut, pocket beach blocked by headland | Must be fair: evidence of risk should exist |
| NeutralFriction | Normal traversal pressure | Leaf litter, roots, uneven slope, shallow creek crossings | Should build texture without dominating decisions |
| HazardousAffordance | Useful route with danger/cost | Cliff-top view route, boulder creek, steep spur descent | High information value justifies risk |
| OneWayPressure | Easy one direction, costly reverse | Waterfall descent, mudslide slope, collapsed bank | Use sparingly; avoid soft-locks |
| DebugOnly | Editor/test data only | Landform overlays, flow arrows, validation photo points | Never shown to player |

### Hard blockers

Good hard blockers:

- Cliff coasts.
- Near-vertical ravines.
- Waterfall drops.
- Dense mangrove root/mud zones below safe walking threshold.
- Large unstable boulder choke.
- Storm-felled tree walls only when physically plausible.
- Deep tidal channels.

Bad hard blockers:

- Invisible walls.
- Arbitrary foliage walls.
- Same-looking jungle patch that suddenly blocks movement.
- Circular cliffs enclosing an arena.
- Coastline barriers that exist only to stop the player.

### Soft blockers

Good soft blockers:

- Muddy tidal flat.
- Basin swamp.
- Dense understorey at creek edge.
- Wet slippery gully.
- Steep convex slope with poor view of bottom.
- Tangle around fallen trees.
- Boulder field with ankle-risk movement.

Soft blockers should alter movement, camera, footstep sound, stamina-like effort if such systems exist, and route choice. They should not simply reduce speed invisibly.

### Affordances

Good natural affordances:

- Ridge crest.
- Spur.
- Saddle.
- Dry creek bank.
- Beach strandline.
- Rock shelf.
- Animal/hominin-used crossing, if subtle and non-UI.
- Canopy break revealing skyline.

Affordances should be readable but not over-clean. A path that is too obvious becomes an objective marker by another name.

### False affordances

False affordances are excellent for Jungle Game because the relict hominin presence evaluates the player without direct violence.

Acceptable false affordances:

- A gully that looks like a shortcut but ends in a drop.
- A beach that looks like a coast route but is blocked by a headland.
- A dry basin floor that becomes mud after rain.
- A ridge that narrows into unsafe rock.
- A creek bank that forces repeated crossings.
- A saddle that crosses into the wrong catchment for the player’s intended return.

Unacceptable false affordances:

- Untelegraphed death traps.
- Fantasy terrain shifts.
- Deliberately impossible mazes.
- “Gotcha” invisible boundaries.

## First-Person Readability Rules

### Rule 1: Every macro landform needs a first-person signature

A landform is not valid just because it looks correct from a top-down heightmap. It needs a recognizable first-person signature.

Examples:

| Landform | First-person signature |
|---|---|
| Ridge | Ground falls away on both sides; wind/canopy changes; occasional skyline glimpses |
| Spur | Nose-like descent; drainage sound left/right; slope narrows toward creek |
| Saddle | Higher ground ahead/behind, lower ground to sides; wind funnel; crossing feel |
| Gully | Enclosed V/U shape; damp floor; water/debris line; limited escape angles |
| Basin | Flat wet repetition; muffled distance cues; poor drainage; insects/frogs |
| Creek confluence | Sound layering; bank geometry; debris wedge; vegetation junction |
| Headland | Exposed rock, wind, surf both sides, coastal view split |
| Mangrove flat | Mud, roots, tidal smell/sound implication, low visibility, channels |

### Rule 2: Use slope breaks as evidence

Slope breaks are essential because players sense slope through camera angle, movement effort, footstep timing, and horizon. Use them to mark transitions:

- Ridge crest to side slope.
- Upland slope to footslope.
- Footslope to valley floor.
- Valley floor to creek bank.
- Backshore to beach.
- Beach to dune/backshore.
- Freshwater creek to tidal influence.
- Cliff top to coastal drop.

### Rule 3: Do not hide all terrain under foliage

Dense jungle can still be readable, but only if foliage supports terrain rather than erases it.

Implementation recommendations:

- Reduce understorey density on selected ridge crests and spur noses.
- Use ferns/moss/riparian plants to mark wet gullies and creek heads.
- Use root buttresses and exposed roots to emphasize slope direction.
- Use leaf litter and drier ground on ridges.
- Use vine tangles as local soft blockers, not universal wallpaper.
- Use canopy gaps at saddles, creek junctions, stormfall, cliffs, and beaches.

The Wet Tropics include diverse rainforest structural types and mangrove communities, so uniform foliage is specifically wrong for the reference direction. ([dcceew.gov.au](https://www.dcceew.gov.au/parks-heritage/heritage/places/world/wet-tropics))

### Rule 4: Landmarks must be partial, repeated, and angle-dependent

Avoid always-visible landmarks. In dense rainforest, landmark value comes from recognition over time:

- Summit silhouette glimpsed from beach, ridge, and creek opening.
- Distinct headland profile visible from multiple coastal sectors.
- Large emergent tree visible only from certain saddles.
- Waterfall sound heard before seen.
- Rock outcrop shape recognized from different approach angles.
- Mangrove smell/sound/ground cue before visual confirmation.

### Rule 5: Audio is terrain evidence

Audio should express terrain:

- Surf grows with coastal exposure.
- Creek pitch changes with gradient.
- Waterfalls are audible across ravines and basins.
- Dense wet basins have louder insect/frog beds.
- Ridges have more wind and less water masking.
- Rain on canopy differs from rain in open creek or beach.
- Mangroves have tidal water, mud, insects, and muffled movement.

Do not convert audio into objective ping systems.

### Rule 6: Weather can alter readability, not replace it

Rain should:

- Increase creek volume and sound.
- Make gullies more dangerous.
- Expand mud soft blockers.
- Reduce visibility.
- Create temporary flow in minor gullies.
- Increase waterfall audibility.
- Add debris lines after storms.

Rain should not:

- Randomly flood implausible high ground.
- Turn every slope into identical mud.
- Create impossible route closures without evidence.
- Serve as a hidden timer.

## Terrain Generation Architecture Notes

### Classical procedural terrain

Classical procedural terrain uses noise functions, fractals, ridged noise, domain warping, erosion filters, splines, masks, and hand-authored heightfields.

Strengths:

- Fast.
- Deterministic.
- Easy to seed.
- Easy to run offline.
- Easy to integrate with UE5 heightmaps.
- Good for iteration.
- Controllable with masks and artist edits.

Weaknesses:

- Raw noise often produces arbitrary steepness.
- Hydrology is usually fake unless explicitly modeled.
- Creeks may not respect catchments.
- Repetition becomes visible.
- Gameplay readability can be poor if terrain is generated only for visual plausibility.
- Jungle foliage can hide all generated structure.

Recommended classical approach:

1. Author or generate a **macro heightfield** first.
2. Solve or approximate **flow accumulation and catchments**.
3. Define ridges/divides from hydrology.
4. Place creek skeletons from flow.
5. Generate landform masks.
6. Apply erosion/detail passes.
7. Apply foliage from landform + wetness + slope + exposure.
8. Validate in first person.

### Learned terrain generation

Learned terrain generation attempts to synthesize more realistic terrain from data. It may produce more natural macro coherence than hand-tuned noise, but it introduces control, training, licensing, reproducibility, runtime, and integration risks.

For Jungle Game, learned terrain should be treated as:

- A research reference.
- A possible offline candidate generator.
- A future architecture direction.
- Not a PR5 dependency.

### InfiniteDiffusion / Terrain Diffusion reference

The InfiniteDiffusion project page describes a training-free method that reformulates diffusion sampling for lazy and unbounded generation, with seed consistency, constant-time random access, deterministic coordinate-indexed generation, and a bounded cache used as a performance optimization. The Terrain Diffusion page presents a learned procedural terrain generator with a procedural-noise-like interface, hierarchical diffusion models, large dynamic elevation range handling, and consumer-hardware demos. ([xandergos.github.io](https://xandergos.github.io/terrain-diffusion/))

The arXiv abstract similarly claims that InfiniteDiffusion bridges diffusion fidelity with procedural properties: seamless infinite extent, seed consistency, constant-time random access, hierarchical model stacks, Laplacian encoding, and an infinite-tensor framework. ([arxiv.org](https://arxiv.org/abs/2512.08309))

Relevant lessons for Jungle Game:

- **Seed-addressed generation:** even a bounded island benefits from deterministic seed + coordinate generation for reproducible terrain builds.
- **Lazy evaluation:** do not require the entire world to be fully processed for every experiment; build tools that can regenerate specific tiles/cells.
- **Random access:** validation should be able to query any terrain cell, landform class, or evidence point without replaying generation history.
- **Hierarchical generation:** separate island-scale context from local detail. Macro terrain should not be overwritten by micro noise.
- **Learned fidelity is not gameplay fidelity:** realistic-looking terrain still needs explicit traversal categories, evidence points, and first-person readability checks.
- **Bounded island-first workflow:** Jungle Game does not need infinite terrain. The useful idea is deterministic, addressable, hierarchical generation—not unbounded runtime generation.

Recommended position:

> Track InfiniteDiffusion / Terrain Diffusion as a future research option, but implement the island now with deterministic macro terrain data, hydrology-aware masks, UE5 Landscape/World Partition support, and editor-only evidence validation.

### Architecture options and tradeoffs

| Option | Description | Pros | Cons | Recommendation |
|---|---|---|---|---|
| Hand-authored macro heightfield + procedural masks | Artist/sculpted island base with generated catchment/landform layers | Best control, PR-friendly, UE5-compatible | Requires discipline and validation | Implement now |
| Classical procedural + erosion | Noise/terrain functions constrained by hydrology and erosion | Deterministic, automatable, strong iteration | Can still look synthetic; erosion tuning cost | Good near-term |
| GIS-inspired synthetic island | Build from real geomorphology principles without copying protected places | Grounded and controllable | Needs tooling and art pass | Strong target |
| Offline learned candidate generation | Use learned model to propose terrain, then lock/edit | Potentially high fidelity | Dependency/tooling/licensing risk | Defer |
| Runtime learned infinite terrain | Generate terrain around player dynamically | Technically interesting | Wrong scale for island-first authored survival game | Do not implement now |

## UE5 Implementation Guidance

### UE5 systems to align with

Use UE5 Landscape for the primary terrain surface. Unreal’s Landscape system supports mountains, valleys, sloped ground, and terrain modification; its component architecture affects rendering, visibility, collision, LOD, and performance. ([dev.epicgames.com](https://dev.epicgames.com/documentation/unreal-engine/landscape-overview?utm_source=chatgpt.com))

Use World Partition for large-world management. Epic describes World Partition as storing a world in one persistent level separated into streamable grid cells loaded/unloaded by streaming sources such as the player. ([dev.epicgames.com](https://dev.epicgames.com/documentation/unreal-engine/world-partition-in-unreal-engine))

Use PCG for terrain-dependent placement, not for inventing macro geography after the fact. Epic describes PCG as a framework for procedural tools and content from asset utilities to whole-world workflows, with points carrying transform, bounds, density, steepness, seed, and custom attributes. ([dev.epicgames.com](https://dev.epicgames.com/documentation/unreal-engine/procedural-content-generation-overview))

Use Landscape Edit Layers for non-destructive terrain work. Epic describes edit layers as stack-based, non-destructive containers for heightmap, weightmap, splines, patches, and procedural/manual terrain edits. ([dev.epicgames.com](https://dev.epicgames.com/documentation/unreal-engine/landscape-edit-layers-in-unreal-engine))

### Recommended terrain channel names

Use explicit, stable names. These should exist as generated rasters, landscape weightmaps, PCG attributes, or debug-only derived fields depending implementation stage.

Core terrain channels:

```text id="9e7snx"
HeightMeters
SlopeDegrees
AspectRadians
ProfileCurvature
PlanCurvature
FlowDirection
FlowAccumulation
DrainageOrder
CatchmentId
SubcatchmentId
DistanceToCreekMeters
DistanceToCoastMeters
ElevationBand
WetnessIndex
ExposureIndex
TerrainRoughness
LandformClass
TraversalClass
CliffRisk
MudSoftness
RockExposure
SoilDepthEstimate
CanopyDensity
UnderstoryDensity
VisibilityDistanceMeters
AudioWaterInfluence
AudioSurfInfluence
SkylineLandmarkId
```

Recommended material/PCG layer names:

```text id="vrbb0v"
L_Terrain_BaseHeight
L_Terrain_MacroSpine
L_Terrain_Ridges
L_Terrain_Spurs
L_Terrain_Saddles
L_Terrain_Creeks
L_Terrain_Basins
L_Terrain_Coast
L_Terrain_Mangrove
L_Terrain_Rock
L_Terrain_DebugEvidence
```

Recommended UE Data Layer naming:

```text id="3fbyok"
DL_Terrain_Macro
DL_Terrain_Hydrology
DL_Terrain_Coast
DL_Terrain_Foliage_Ridge
DL_Terrain_Foliage_Riparian
DL_Terrain_Foliage_Mangrove
DL_Terrain_Rocks
DL_Terrain_DebugEvidence
```

Epic’s PCG + World Partition docs note that PCG assets assigned to World Partition Data Layers and HLOD Layers can generate actors assigned to those same layers, which supports separating terrain-dependent generated content for streaming and editor management. ([dev.epicgames.com](https://dev.epicgames.com/documentation/unreal-engine/using-pcg-with-world-partition-in-unreal-engine))

### Suggested data enum names

Pseudocode only:

```text id="tnt1mv"
enum class EJGLandformClass : uint8
enum class EJGTraversalClass : uint8
enum class EJGTerrainEvidenceType : uint8
enum class EJGHydrologyClass : uint8
enum class EJGCoastClass : uint8
```

Suggested struct names:

```text id="y6b2ki"
FJGLandformSample
FJGTerrainChannelSample
FJGTerrainEvidencePoint
FJGFirstPersonEvidenceShot
FJGCatchmentDescriptor
FJGCreekSegmentDescriptor
FJGCoastalSectorDescriptor
FJGTraversalPressureSample
```

Suggested asset/data table names:

```text id="iiybo9"
DA_JG_LandformTaxonomy
DA_JG_TerrainChannelSchema
DA_JG_TraversalCategoryRules
DA_JG_CatchmentSet_Cell0Island
DT_JG_TerrainEvidencePoints
DT_JG_CoastalSectors
```

### Collision and traversal validation

Landscape collision settings matter because traversal readability fails if what the player sees does not match what blocks movement. Epic’s Landscape Collision Guide notes that higher collision mip levels are cheaper but less accurate, and recommends low/simple collision settings for most cases to avoid visible inaccuracies. ([dev.epicgames.com](https://dev.epicgames.com/documentation/unreal-engine/landscape-collision-guide-in-unreal-engine))

Recommendations:

- Use accurate collision in playable terrain evidence zones.
- Use lower detail collision only for non-playable distant components.
- Validate hard blockers in Player Collision view.
- Never let a visually gentle slope behave as a hard collision blocker.
- Never let a visibly impossible cliff be walkable unless there is an authored ledge/route.

### PCG placement rules

PCG should consume terrain channels:

- `SlopeDegrees` filters tree placement, boulder placement, and climbability.
- `WetnessIndex` controls riparian plants, moss, mud, frogs/insects.
- `FlowAccumulation` drives creek rocks, debris, scoured banks.
- `LandformClass` drives biome subgraphs.
- `ExposureIndex` drives wind-shaped coastal vegetation and ridge thinning.
- `DistanceToCoastMeters` and `CoastClass` drive beach/mangrove/headland transitions.
- `VisibilityDistanceMeters` marks potential view corridors but must not create artificial tunnels everywhere.

Use PCG partitioning/hierarchical generation for scale. Epic’s PCG generation modes split generation into grids for larger worlds, allowing larger meshes on large grids and smaller detail on smaller grids. ([dev.epicgames.com](https://dev.epicgames.com/documentation/unreal-engine/using-pcg-generation-modes-in-unreal-engine))

## Validation Points And Evidence Plan

### Core validation principle

A terrain feature is not accepted until it works from first person without UI.

Validation should capture:

- First-person screenshots.
- Short first-person route clips.
- Debug-only orthographic context.
- Terrain channel samples.
- Player traversal outcome metrics.
- Audio cue notes.
- Foliage readability notes.

### Sample validation point types

Recommended `EJGTerrainEvidenceType` values:

```text id="wpqbld"
RidgeCrestRead
SpurDescentRead
SaddleCrossingRead
CreekUpstreamDownstreamRead
CreekConfluenceRead
BasinConfusionRead
MangroveTransitionRead
BeachCoastResetRead
HeadlandLandmarkRead
CliffBlockerRead
SkylineGlimpseRead
SlopeBreakRead
FalseAffordanceRead
SoftBlockerRead
HardBlockerRead
```

### Evidence point fields

Suggested `FJGTerrainEvidencePoint` fields:

```text id="oro0x1"
PointId
Location
FacingYaw
ExpectedLandformClass
ExpectedTraversalClass
ExpectedEvidenceType
CatchmentId
CoastalSectorId
RequiredVisibleCue
RequiredAudioCue
ExpectedPlayerInference
bRequiresNoHUD
bRequiresFirstPersonScreenshot
bRequiresCollisionCheck
bRequiresRainVariant
Notes
```

### Required first-person screenshot sets

Capture these later:

1. **Upland spine from coast**
   - Beach or headland view showing partial summit/spine silhouette.
   - Purpose: macro landmark recognition.

2. **Ridge crest route**
   - First person standing on ridge with ground falling away on both sides.
   - Purpose: ridge as affordance and divider.

3. **Spur descent**
   - Looking down a spur toward lower wet vegetation/creek sound.
   - Purpose: spur as route evidence.

4. **Saddle crossing**
   - Looking across low point between two higher rises.
   - Purpose: catchment transition readability.

5. **Creek head**
   - Wet seep/gully start below headwall/ridge.
   - Purpose: hydrology origin evidence.

6. **Creek confluence**
   - Two streams meeting with debris/sound/vegetation cue.
   - Purpose: memory landmark.

7. **Basin floor**
   - Flat, wet, low-visibility area with multiple ambiguous exits.
   - Purpose: controlled disorientation without UI.

8. **Mangrove transition**
   - Freshwater/riverine edge shifting into tidal mud/root systems.
   - Purpose: coast/tide evidence.

9. **Pocket beach blocked by headland**
   - Beach that resets orientation but does not guarantee travel.
   - Purpose: false coast-path affordance.

10. **Cliff coast**
    - Visible hard blocker with surf/wind evidence.
    - Purpose: fair boundary.

### Debug-only evidence views

Allowed only in editor/test builds:

- Landform class overlay.
- Traversal class overlay.
- Catchment ID overlay.
- Flow direction arrows.
- Flow accumulation heatmap.
- Slope degrees heatmap.
- Wetness index heatmap.
- First-person evidence point actor icons.
- Route trace visualization.
- Collision mismatch visualization.
- View corridor debug cones.
- Audio influence debug volumes.

Forbidden in player builds:

- Any overlay that functions as a map.
- Any orientation confidence display.
- Any marker showing “correct” direction.
- Any compass-like display.
- Any hidden system that makes character comments about orientation.

### Route validation tests

Create later test routes:

| Test route | Start | Goal | Expected evidence |
|---|---|---|---|
| Downstream confidence | Creek midsection | Coast outlet | Flow direction, increasing creek scale, coastal audio |
| Ridge-to-creek descent | Ridge crest | Creek corridor | Spur selection, gully avoidance, wetness increase |
| Catchment crossing | Valley A | Valley B | Saddle recognition, drainage direction shift |
| Coast reset failure | Pocket beach | Next coastal sector | Headland blocks clean coast loop; inland bypass needed |
| Basin confusion | Basin entry | Basin exit | Repeated vegetation, subtle overflow clue |
| Mangrove edge | River mouth | Dry inland route | Tidal mud/root blocker pushes player to higher ground |
| Cliff bypass | Cliff coast | Adjacent beach | Surf confirms coast, cliff forces ridge/headland decision |

### Metrics to capture later

Do not expose these to the player.

- Time spent in each landform class.
- Route chosen after first sight of saddle/spur/creek.
- Number of creek crossings.
- Number of failed hard-blocker approaches.
- Soft-blocker dwell time.
- Whether players infer downstream direction without prompting.
- Whether players can re-identify a headland/summit from a second angle.
- Whether players confuse spur and gully repeatedly.
- Whether foliage density hides required terrain evidence.
- Screenshot pass/fail for each evidence point.
- Collision visual mismatch count.

## Risks, Pitfalls, And Forbidden Drift

### Pitfall: arbitrary steep noise

Raw noise creates lumpy terrain without hydrology. It may look “natural” from a distance but fails first-person navigation because ridges, gullies, and creeks do not form a readable system.

Prevention:

- Generate catchments before details.
- Validate all creeks downstream.
- Limit high-frequency height displacement on route-critical landforms.
- Use erosion/detail as a secondary pass, not the terrain’s main logic.

### Pitfall: too-flat jungle arenas

Flat arenas make foliage carry all navigation. This leads to fake paths, invisible boundaries, and objective-marker thinking.

Prevention:

- Keep continuous slope evidence.
- Place basins deliberately, not everywhere.
- Use microrelief and drainage even in lowlands.
- Ensure flat areas have hydrological explanation.

### Pitfall: uniform foliage blanket

A uniform dense foliage pass destroys landform readability.

Prevention:

- Tie foliage density to terrain channels.
- Preserve view corridors at selected evidence points.
- Differentiate ridge, creek, basin, coast, and mangrove vegetation.
- Avoid asset-pack scattering without terrain rules.

### Pitfall: clean circular coast path

A circular coast path lets players solve the island by wall-following.

Prevention:

- Alternate beach, cliff, headland, mangrove, estuary, boulder coast, and pocket coves.
- Require inland bypasses.
- Let coast reset orientation but not trivialize traversal.
- Avoid perfectly walkable continuous shoreline.

### Pitfall: invisible-wall boundaries

Invisible walls violate the world-first premise.

Prevention:

- Use cliffs, deep tidal channels, unstable slopes, dense root/mud systems, or surf-swept rock as visible hard blockers.
- Validate from first person before collision.
- Use collision view to confirm blocker fairness.

### Pitfall: top-down-only beauty

Terrain that looks excellent in an editor aerial view may fail in first person.

Prevention:

- Every landform class needs first-person screenshot validation.
- Require route tests without UI.
- Check readability in rain, dusk-like low light, and dense canopy.
- Treat debug maps as diagnosis only.

### Pitfall: overusing landmarks

If every major landmark is always visible, navigation becomes too easy and artificial.

Prevention:

- Use partial glimpses.
- Use angle-dependent recognition.
- Use repeated silhouettes from different sectors.
- Hide landmarks under canopy most of the time.

### Pitfall: fantasy jungle drift

The unsettling element is the nonviolent relict hominin presence, not magical terrain.

Forbidden drift:

- Terrain morphing with no physical explanation.
- Monster/chase framing.
- Gore/combat-driven route pressure.
- Fantasy biomes.
- Generic pine forest assumptions.
- Asset pack dumps.
- Exact replication of protected or culturally sensitive real places.
- Player-facing navigation UI.

### Pitfall: learned terrain dependency too early

InfiniteDiffusion / Terrain Diffusion is promising, but adopting it now would distract from the island-first terrain spec.

Prevention:

- Convert its lessons into architecture principles.
- Do not add runtime learned generation now.
- Do not block terrain work on model/tool availability.
- Keep deterministic build artifacts and editable UE5 layers.

## PR-Ready Recommendations

### Implement now

1. **Create terrain taxonomy**
   - Add landform category names.
   - Add traversal category names.
   - Add terrain evidence type names.
   - Keep them project-specific and stable.

2. **Define terrain channel schema**
   - Include height, slope, aspect, curvature, flow, catchment, wetness, exposure, landform, traversal, canopy, and audio influence channels.
   - Store as data spec first; implementation can be raster, data asset, landscape layer, or generated attribute later.

3. **Build a macro island blockout**
   - One upland spine.
   - 5–9 primary catchments.
   - Distinct coastal outlets.
   - Major ridges, spurs, saddles, basins, and creek corridors.
   - No final foliage required yet.

4. **Add debug-only evidence point plan**
   - Data table or actor class for evidence points.
   - First-person screenshot requirements.
   - Expected landform/traversal/cue fields.

5. **Create first traversal mapping**
   - Hard blockers, soft blockers, affordances, false affordances, neutral friction.
   - Apply to macro landforms before detailed mechanics.

6. **Use UE5 Landscape Edit Layers**
   - Separate macro base, ridges/spurs, hydrology, coast, mangrove, rock, and debug evidence layers.
   - Keep procedural/manual edits non-destructive where possible.

7. **Use PCG only after macro terrain exists**
   - PCG consumes terrain evidence channels.
   - PCG should not invent the island’s macro logic.

### Defer

- Runtime learned terrain generation.
- InfiniteDiffusion/Terrain Diffusion integration.
- Full erosion simulation.
- Seasonal hydrology simulation.
- Final rainforest biome generation.
- Dynamic relict hominin environmental manipulation.
- Full audio propagation model.
- Production performance tuning.
- Final coast asset set.
- Cultural/place-specific replication.

### Acceptance criteria for this PR input

A later PR using this document should be considered aligned if it produces:

- A named landform taxonomy.
- A named terrain channel schema.
- A traversal category mapping.
- A macro island terrain plan with hydrological logic.
- Debug-only validation point definitions.
- No player-facing navigation aid.
- No modern tool/navigation UI drift.
- No combat/monster/fantasy drift.
- A clear “now vs deferred” split.

## Sources

- Australian Government Department of Climate Change, Energy, the Environment and Water — “Wet Tropics of Queensland”
  https://www.dcceew.gov.au/parks-heritage/heritage/places/world/wet-tropics

- Queensland State of the Environment Report 2024 — “Wet Tropics of Queensland World Heritage natural criteria”
  https://www.stateoftheenvironment.detsi.qld.gov.au/heritage/world/wet-tropics-of-queensland/wet-tropics-of-queensland-world-heritage-natural-criteria

- Queensland WetlandInfo — “Terrain”
  https://wetlandinfo.detsi.qld.gov.au/wetlands/ecology/components/terrain/

- U.S. Geological Survey Water Science School — “Rivers, Streams, and Creeks”
  https://www.usgs.gov/water-science-school/science/rivers-streams-and-creeks

- Geoscience Australia — “Coastal erosion”
  https://www.ga.gov.au/education/natural-hazards/coastal-erosion

- Australian Institute of Marine Science — “Field Guide to the Mangroves of Queensland”
  https://www.aims.gov.au/sites/default/files/field-guide-tothe-mangroves-of-qld.pdf

- Queensland Wetlands Programme — “Wetland Management Profile: Mangrove Wetlands”
  https://wetlandinfo.detsi.qld.gov.au/resources/static/pdf/resources/fact-sheets/profiles/p01867aa.pdf

- Orienteering ACT — “Lesson 5: Introduction to Contours”
  https://act.orienteering.asn.au/resources/skills/advanced-skills/lesson-5-introduction-to-contours/

- Bushwalking 101 / National Parks Association of NSW and Wildwalks — “Interpreting map features”
  https://www.bushwalking101.org/interpreting-map-features

- Epic Games Documentation — “World Partition in Unreal Engine”
  https://dev.epicgames.com/documentation/unreal-engine/world-partition-in-unreal-engine

- Epic Games Documentation — “Landscape Technical Guide in Unreal Engine”
  https://dev.epicgames.com/documentation/unreal-engine/landscape-technical-guide-in-unreal-engine

- Epic Games Documentation — “Importing and Exporting Landscape Heightmaps in Unreal Engine”
  https://dev.epicgames.com/documentation/unreal-engine/importing-and-exporting-landscape-heightmaps-in-unreal-engine

- Epic Games Documentation — “Landscape Edit Layers in Unreal Engine”
  https://dev.epicgames.com/documentation/unreal-engine/landscape-edit-layers-in-unreal-engine

- Epic Games Documentation — “Procedural Content Generation Overview”
  https://dev.epicgames.com/documentation/unreal-engine/procedural-content-generation-overview

- Epic Games Documentation — “Using PCG with World Partition in Unreal Engine”
  https://dev.epicgames.com/documentation/unreal-engine/using-pcg-with-world-partition-in-unreal-engine

- Epic Games Documentation — “Using PCG Generation Modes in Unreal Engine”
  https://dev.epicgames.com/documentation/unreal-engine/using-pcg-generation-modes-in-unreal-engine

- Epic Games Documentation — “Landscape Collision Guide in Unreal Engine”
  https://dev.epicgames.com/documentation/unreal-engine/landscape-collision-guide-in-unreal-engine

- Alexander Goslin — “InfiniteDiffusion: Bridging Learned Fidelity and Procedural Utility for Open-World Terrain Generation” project page
  https://xandergos.github.io/terrain-diffusion/

- Alexander Goslin — “InfiniteDiffusion: Bridging Learned Fidelity and Procedural Utility for Open-World Terrain Generation” arXiv page
  https://arxiv.org/abs/2512.08309
