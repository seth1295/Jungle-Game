# Design Pillars

## Core Pillar

Use this exact pillar throughout the large-world plan:

```text
Terrain creates ecology.
Ecology creates navigation.
Navigation creates survival pressure.
Getting lost is gameplay.
The world is readable, but only if the player learns how to read it.
Reward environmental literacy, not UI literacy.
```

The world is the primary object. The player may eventually spawn in a valid semi-random world location. Camp, Cell 0, intro sequences, authored shelters, and story anchors may exist later, but they must be placed inside the world rather than used to define the whole structure.

The world should feel like it exists before the player understands it.

## Island World Framing

The target world is a 16 km x 16 km rainforest/mountain island. Ocean is the natural world boundary. The primary edge solution must not be invisible walls.

Implementation guidance:

- Make coastline reachable, readable, and useful, but not always safe.
- Include beach, mangrove, cliff coast, rocky shore, creek mouth, exposed headland, and difficult coastal terrain edge types.
- Use surf sound, coastline direction, creek mouths, mangroves, cliffs, headlands, and rocky shore transitions as navigation evidence.
- Make interior terrain mountain/ridge/valley/creek/gorge driven.
- Place authored locations later inside the island. Do not fake a world by building only authored spokes around a camp.

## Terrain Creates Ecology

Terrain is the first system. Height, slope, aspect, water movement, ridges, valleys, gorges, alluvial flats, and disturbance history decide what grows where.

Implementation guidance:

- Generate or import terrain before trying to paint ecosystems.
- Treat creek lines, valley bottoms, ridge crests, steep slopes, and sheltered aspects as design inputs, not decoration.
- Keep ecosystem masks derived from measurable terrain data wherever possible.
- Do not assign rainforest, ridge, gorge, or wet-valley zones by square technical cells.
- Make the player read landform changes through vegetation structure, visibility, footing, sound, humidity cues, passability, and movement cost.

The Queensland regional ecosystem framework is a useful real-world model because regional ecosystems are defined as vegetation communities in a bioregion associated with geology, landform, and soil, not arbitrary visual themes ([Queensland Government regional ecosystems](https://www.qld.gov.au/environment/plants-animals/plants/ecosystems/about)).

## Ecology Creates Navigation

Ecosystems must become a navigation and traversal language. A player should learn that creek-bank vegetation means water, valley direction, lower ground, noise masking, and risk. Ridge vegetation should imply elevation, visibility, exposure, and possible landmark silhouettes. Dense wet forest should imply slow movement and low sightlines.

Implementation guidance:

- Give each ecosystem a distinct navigation role.
- Use vegetation density, canopy openness, ground clutter, slope, water sound, insect sound, and landmark occlusion as spatial cues.
- Make ecosystem transitions continuous enough to feel natural, but distinct enough to teach the player.
- Build traversal affordances from landform first: ridges, spurs, saddles, basins, creek banks, gullies, slope breaks, and vegetation density.
- Do not draw explicit authored trails or route lines as the primary design method.
- Allow paths to appear later as consequences of terrain, animal movement, erosion, old disturbance, or human history.
- Make traversal feel discovered, not prescribed.
- Validate from first-person view, not only from top-down debug maps.

## Terrain-First Traversal Model

The design goal is to build the foundation for routes to happen, not to draw routes onto the map.

Traversal should classify terrain into four practical categories:

| Category | Meaning | Examples |
|---|---|---|
| Hard blockers | Areas the player cannot reasonably pass through or must route around | Cliffs, extreme slope, water obstacles, dense impassable vegetation |
| Soft blockers | Areas the player can cross, but at cost or risk | Steep fatigue slopes, mud, deadfall, thick regrowth, wait-a-while vines |
| Affordance zones | Areas that naturally invite movement or orientation | Ridges, spurs, saddles, open understory, creek edges, animal corridors, slope breaks |
| False affordances | Areas that look promising but may mislead, slow, or trap the player | Gullies, dry creek beds, disturbed regrowth, misleading clearings, old scars, beaches that end in cliffs or mangroves |

Implementation guidance:

- Start with terrain passability and ecosystem density, not trail drawing.
- Use hard blockers sparingly and visibly; invisible walls should be avoided.
- Use soft blockers to create choice, fatigue, risk, and route memory.
- Use affordance zones to let players discover movement lines.
- Use false affordances to support getting lost without making the world feel random.
- Paths, if added, should be traces of causes: runoff, animal movement, old human disturbance, repeated erosion, or terrain wear.
- Mountains should create landmarks, catchments, ridge systems, creek heads, basins, and traversal pressure. They must not become arbitrary steep noise or an impossible sliding simulator.

## Navigation Creates Survival Pressure

Navigation is not a convenience layer. It is part of the survival loop.

Implementation guidance:

- Distance from known safety, authored shelter, current spawn context, or camp if one exists should change risk.
- Getting turned around should create resource and exposure pressure, not a UI error.
- Landmarks should help only when the player has line of sight, weather allows it, or they have learned the surrounding landform.
- Creek flow, slope direction, ridge orientation, and sun position should remain legible but imperfect.
- Returning to known safety should feel like navigation success, not waypoint compliance.
- Do not add an orientation confidence system. Do not use character comments, hidden meters, or UI-like feedback to tell the player whether they are oriented. The game provides environmental evidence; the player does the interpretation.

## Getting Lost Is Gameplay

Getting lost is not a failure case. It is the game's core state of uncertainty.

Implementation guidance:

- Do not provide a player map.
- Do not provide a minimap.
- Do not provide a compass.
- Do not provide a bearing number.
- Do not provide a GPS marker.
- Do not provide objective marker navigation.
- Do not provide a HUD clock or explicit time readout.
- Do not provide an always-correct quest marker.
- Do not solve disorientation with UI breadcrumbs.
- Do provide readable world cues, recoverable terrain logic, and consequences that escalate rather than instantly punish.

The player should be able to form and test hypotheses:

- "The creek is flowing downhill, so following it should take me toward the basin."
- "The ridge silhouette means the known shelter is probably behind that shoulder."
- "This vegetation is wetter and denser, so I am near a drainage line."
- "That gully looks easy, but it may box me in or drag me away from the ridge."
- "The sun has shifted; I have been out too long."

## No Map / No Compass

The absence of a map and compass must be designed, not merely disabled. The game must not expose a player map, minimap, compass, bearing number/readout, GPS marker, objective marker navigation, HUD clock, or explicit time readout.

Implementation guidance:

- Build landmark visibility deliberately: mountain silhouettes, ridge notches, smoke, unusually shaped trees, creek bends, gorge walls, and canopy breaks.
- Make wrong turns recoverable by environmental logic.
- Add debug-only visualization for development, but keep it out of player-facing gameplay.
- Avoid diegetic shortcuts that function as a compass unless deliberately scoped and justified.
- Direction must be inferred through diegetic environmental cues only: sun position, shadow direction/length, light colour, slope, creek flow, coastline/ocean if present, mountain silhouettes, vegetation/ecosystem transitions, and sound.
- The player may estimate time of day from world presentation, but the game must not provide an explicit clock.
- Losing sight of the sun through canopy, cloud, gorge terrain, or night should create real navigation pressure.

## Night Changes The Rules

Night is a serious danger state. It should not be only a dark color grade.

Implementation guidance:

- Reduce landmark visibility and long-range orientation.
- Remove sun-based direction where canopy, clouds, gullies, gorges, mountain occlusion, rain, or night block the sky.
- Allow rough night direction through diegetic cues only: stars through canopy breaks, southern-sky orientation appropriate to FNQ/southern hemisphere, moonlight and moon position, ocean sound/coastline direction, mountain/ridge silhouettes against sky glow, creek flow, slope, and terrain memory.
- Change sound priority so the player hears water, canopy, movement, and weather differently.
- Make creek banks, slopes, and dense vegetation more dangerous to traverse.
- Make known shelter, fire, or smoke more valuable as visible and psychological anchors where they exist.
- Use night danger masks to drive spawn risk, visibility, traversal safety, and player decision pressure.
- Stars should be a real navigation affordance only when the world allows it; dense canopy, clouds, rain, gullies, gorges, and mountain occlusion should reduce or remove sky navigation.
- Keep night survivable but costly; the intended lesson is "I lost the sun and must read weaker cues," not "the game became unreadable."

## Player-Made Trail Marks

The player can mark the world diegetically. These are not GPS markers and must not become UI navigation.

Allowed mark concepts:

- broken branches;
- scratched bark;
- cloth strips;
- rock stacks;
- ash marks;
- cut vines;
- temporary ground markers.

Implementation guidance:

- Marks can be missed, washed out, hidden by darkness, confused with natural damage, or disturbed.
- Marks should help an attentive player, not guarantee route recovery.
- Weather, darkness, dense vegetation, and similar natural damage should make mark interpretation uncertain.

## Weather Attacks Navigation

Rain and weather should affect navigation, not just visuals.

Implementation guidance:

- Hide sun and stars.
- Muffle ocean, creek, and waterfall sound.
- Swell creeks and change crossing decisions.
- Erase or obscure trail marks.
- Worsen footing.
- Make slopes and mud more dangerous.
- Make night substantially more dangerous.

## Sound As Navigation

Sound should be a real navigation system. The player should sometimes need to stop moving and listen.

Implementation guidance:

- Surf indicates coast direction.
- Frogs and insects indicate wet zones.
- Waterfalls indicate gorges or creek drops.
- Wind indicates ridges or open canopy.
- Bird and insect changes indicate ecosystem transitions.
- Silence can signal danger or abnormality.

## Terrain Fatigue Instead Of Sliding

Steep terrain should not become a constant physics sliding battle.

Punish poor terrain choices with:

- slower movement;
- fatigue;
- noise;
- injury risk;
- time loss;
- route reconsideration;
- bad footing.

Avoid making ordinary mountain traversal feel like fighting Unreal collision.

## Footing And Injury Risk

Terrain should create moderate survival consequences:

- twisted ankle from bad descent;
- cuts from dense regrowth;
- scratches from wait-a-while;
- leeches or ticks in wet zones;
- infection risk if ignored;
- dehydration worsening stamina and judgement.

Do not overbuild medical simulation early. Keep it practical and gameplay-relevant.

## Ecosystem Tells

Plants and ecosystems are navigation data.

Examples:

- palms, ferns, and moss indicate wet creek/gully influence;
- open scrub and rock indicate ridge or exposed slope;
- mangroves indicate coast or estuary;
- cloud forest and moss indicate altitude;
- dense regrowth indicates disturbance, old damage, or a false route;
- wait-a-while indicates dense wet rainforest or regrowth friction.

The game should reward environmental literacy, not UI literacy.

## False Affordances

Some features should look like routes but become traps:

- dry creek beds;
- animal corridors;
- old logging or disturbance scars;
- regrowth gaps;
- gully descents;
- beach routes that end in cliff or mangrove;
- misleading clearings.

The jungle should make the player believe a route exists, then punish careless commitment.

## Sky Windows

Dense jungle blocks sky. Clearings, ridges, creek gaps, beaches, and canopy breaks allow the player to read sun, stars, moon, and silhouettes.

Looking for a sky window should become gameplay.

## Wait-A-While

Wait-a-while is a required soft-blocker and terrain-friction plant concept.

Gameplay role:

- slows movement;
- snags clothing or gear;
- causes scratches and cuts;
- makes noise;
- punishes panic movement;
- blocks careless shortcuts;
- appears in dense wet rainforest and regrowth zones;
- supports false affordances where a route looks possible but becomes painful or slow.

Wait-a-while should represent the jungle physically grabbing the player.

## FNQ / Wet Tropics Logic

The world should be rainforest and mountain terrain inspired by Far North Queensland and Wet Tropics logic, not a direct replica of protected places.

Implementation guidance:

- Use rugged topography, fast-flowing creeks, gorges, waterfalls, steep gradients, rainforest structural diversity, and sharp vegetation transitions as inspiration.
- The Australian Government describes the Wet Tropics as rugged terrain with rivers, gorges, waterfalls, mountains, steep environmental gradients, high seasonal rainfall, and diverse rainforest structure ([DCCEEW Wet Tropics](https://www.dcceew.gov.au/parks-heritage/heritage/places/world/wet-tropics)).
- Avoid copying sacred, protected, or culturally specific places. Use ecological and geomorphic logic, not direct real-world location reproduction.
