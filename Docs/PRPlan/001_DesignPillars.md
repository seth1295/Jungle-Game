# Design Pillars

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
| Soft blockers | Areas the player can cross, but at cost or risk | Steep fatigue slopes, mud, deadfall, thick regrowth |
| Affordance zones | Areas that naturally invite movement or orientation | Ridges, spurs, saddles, open understory, creek edges, animal corridors |
| False affordances | Areas that look promising but may mislead, slow, or trap the player | Gullies, dry creek beds, disturbed regrowth, misleading clearings |

Implementation guidance:

- Start with terrain passability and ecosystem density, not trail drawing.
- Use hard blockers sparingly and visibly; invisible walls should be avoided.
- Use soft blockers to create choice, fatigue, risk, and route memory.
- Use affordance zones to let players discover movement lines.
- Use false affordances to support getting lost without making the world feel random.
- Paths, if added, should be traces of causes: runoff, animal movement, old human disturbance, repeated erosion, or terrain wear.

## Navigation Creates Survival Pressure

Navigation is not a convenience layer. It is part of the survival loop.

Implementation guidance:

- Distance from camp, darkness, rain, creek crossings, slope fatigue, and visibility loss should change risk.
- Getting turned around should create resource and exposure pressure, not a UI error.
- Landmarks should help only when the player has line of sight, weather allows it, or they have learned the surrounding landform.
- Creek flow, slope direction, ridge orientation, and sun position should remain legible but imperfect.
- Returning to camp should feel like navigation success, not waypoint compliance.

## Getting Lost Is Gameplay

Getting lost is not a failure case. It is the game's core state of uncertainty.

Implementation guidance:

- Do not provide a player map.
- Do not provide a compass.
- Do not provide an always-correct quest marker.
- Do not solve disorientation with UI breadcrumbs.
- Do provide readable world cues, recoverable terrain logic, and consequences that escalate rather than instantly punish.

The player should be able to form and test hypotheses:

- "The creek is flowing downhill, so following it should take me toward the basin."
- "The ridge silhouette means camp is probably behind that shoulder."
- "This vegetation is wetter and denser, so I am near a drainage line."
- "That gully looks easy, but it may box me in or drag me away from the ridge."
- "The sun has shifted; I have been out too long."

## No Map / No Compass

The absence of a map and compass must be designed, not merely disabled. The game must not expose a player map, compass, minimap, bearing readout, GPS marker, HUD clock, or objective marker.

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
- Make camp smoke/fire more valuable as a visible and psychological anchor.
- Use night danger masks to drive spawn risk, visibility, traversal safety, and player decision pressure.
- Stars should be a real navigation affordance only when the world allows it; dense canopy, clouds, rain, gullies, gorges, and mountain occlusion should reduce or remove sky navigation.
- Keep night survivable but costly; the intended lesson is "I lost the sun and must read weaker cues," not "the game became unreadable."

## FNQ / Wet Tropics Logic

The world should be rainforest and mountain terrain inspired by Far North Queensland and Wet Tropics logic, not a direct replica of protected places.

Implementation guidance:

- Use rugged topography, fast-flowing creeks, gorges, waterfalls, steep gradients, rainforest structural diversity, and sharp vegetation transitions as inspiration.
- The Australian Government describes the Wet Tropics as rugged terrain with rivers, gorges, waterfalls, mountains, steep environmental gradients, high seasonal rainfall, and diverse rainforest structure ([DCCEEW Wet Tropics](https://www.dcceew.gov.au/parks-heritage/heritage/places/world/wet-tropics)).
- Avoid copying sacred, protected, or culturally specific places. Use ecological and geomorphic logic, not direct real-world location reproduction.
