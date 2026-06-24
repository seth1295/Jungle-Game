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
- coast / ocean distance;
- canopy density;
- disturbance / regrowth;
- terrain-derived trail persistence potential;
- runtime player-made trail marks handled by gameplay persistence, not static PCG regeneration inputs;
- sound propagation;
- visibility density;
- night danger;
- sky window;
- footing / injury risk;
- ecosystem tells;
- traversal blocker / affordance.

PCG outputs should remain deterministic enough to review and reproduce.

PCG must not treat explicit authored trails as the first-order design method. It should place obstruction, vegetation, ground clutter, animal traces, erosion traces, and disturbance in response to terrain-derived traversal masks.

PCG must also support valid semi-random world-location spawn assumptions. Authored shelters, Cell 0, story anchors, and camp-like locations can exist later, but PCG should not assume the whole world is structured around a single camp.

## Island Edge Ecosystems

Purpose:

- make ocean the natural world boundary;
- keep coastline reachable, readable, and useful without making it always safe;
- avoid invisible walls as the primary edge solution.

PCG should support:

- beach;
- mangrove;
- cliff coast;
- rocky shore;
- creek mouth;
- exposed headland;
- difficult coastal terrain;
- beach false affordances that terminate in cliffs, mangroves, exposed rock, or other coastal hazards.

Validation:

- player can reach representative coastline types;
- surf/ocean sound and coastline direction are useful navigation evidence;
- coast is not a guaranteed safe loop road;
- island edge does not rely on invisible walls as the main boundary.

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
- high night-danger areas near unstable wet banks;
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
- limited long-view windows;
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
- wait-a-while, deadfall, mud, and thick regrowth as soft blockers where wet rainforest/regrowth logic supports them;
- major landmark transitions where a player can update their mental map.

Validation:

- lowlands are not flat empty filler;
- gorge zones feel constrained and directional;
- alluvial areas read as wetter/flatter without becoming arbitrary swamps;
- performance stays within streaming and draw-call budgets.

## Player-Made Trail Marks

Player-made marks are diegetic runtime world changes, not GPS markers and not stable PCG mask inputs.

PCG may provide static terrain-derived trail persistence potential, such as vegetation types, bark surfaces, mud, deadfall, regrowth density, and weather exposure that affect how well marks can exist or remain readable. The actual player-made marks should be handled by gameplay persistence so they do not force static PCG regeneration or create cross-cell streaming churn.

Gameplay should support:

- broken branches;
- scratched bark;
- cloth strips;
- rock stacks;
- ash marks;
- cut vines;
- temporary ground markers.

Validation:

- marks can be missed, washed out, hidden by darkness, confused with natural damage, or disturbed;
- weather and darkness affect readability;
- runtime mark persistence does not require static PCG graph regeneration;
- no UI marker or objective-marker behavior is attached to them.

## Weather, Sound, And Sky Windows

Weather should attack navigation:

- hides sun and stars;
- muffles ocean, creek, and waterfall sound;
- swells creeks;
- erases or obscures trail marks;
- worsens footing;
- makes slopes/mud more dangerous;
- makes night substantially more dangerous.

Sound should be a navigation system:

- surf indicates coast direction;
- frogs/insects indicate wet zones;
- waterfalls indicate gorges or creek drops;
- wind indicates ridge/open canopy;
- bird/insect changes indicate ecosystem transitions;
- silence can signal danger or abnormality.

Sky windows should be generated by clearings, ridges, creek gaps, beaches, and canopy breaks. Dense canopy, clouds, rain, gullies, gorges, and mountain occlusion should reduce or remove sky navigation.

Validation:

- the player sometimes benefits from stopping and listening;
- looking for a sky window is useful;
- night remains readable for skilled players when stars, moon, silhouettes, coast, creek flow, slope, or terrain memory are available.

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
- check that HLOD does not erase authored shelter smoke where present or mountain silhouettes;
- check that HLOD does not erase ridge/mountain silhouettes, coastal headlands, or other skyline navigation evidence;
- check that HLOD does not invent misleading landmarks;
- check that no HLOD, debug, or UI artifact becomes a player-facing map, compass, minimap, bearing readout, GPS marker, HUD clock, or objective marker substitute.

## Performance Risks

- PCG density can overload foliage, collision, shadow, and HLOD budgets.
- Dense rainforest can destroy sightlines and frame rate simultaneously.
- Runtime Data Layers can load too many assets.
- Creek and gorge zones can concentrate expensive detail in small streamed regions.
- Collision on small plants/rocks can become more expensive than visible geometry.
- Wait-a-while, deadfall, and dense regrowth can become frustrating if they feel like collision bugs instead of readable terrain friction.

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
