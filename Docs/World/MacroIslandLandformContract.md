# Macro Island Landform Contract

Created: 2026-06-25
Track: world-first terrain/biome implementation

## Purpose

Define the large island landform logic before expanding biomes or gameplay systems. The world should read like a real wet tropical island landscape, not a flat arena with jungle decorations.

## Primary Landform Elements

### Mountain Spine

The island needs a readable mountain or upland spine that creates:

- skyline landmarks;
- catchments;
- creek heads;
- ridge/spur networks;
- saddles;
- weather exposure;
- cloud/canopy/sun occlusion differences.

The spine must not become arbitrary steep noise or a constant slide simulator. It should create route pressure, fatigue, risk, and reconsideration.

### Ridge and Spur Network

Ridges and spurs are the core no-map movement structures. They should provide intermittent sky windows, less-wet footing, silhouette anchors, and imperfect orientation cues.

Design requirements:

- preserve some sightline windows;
- avoid uniform foliage density on every ridge;
- create exposed/sheltered aspect variation;
- produce saddles and slope breaks;
- make ridge travel useful but not always safest.

### Creek and Wet-Valley System

Creeks should emerge from upland catchments and gather into wet valleys. They are primary navigation evidence and survival pressure.

Creek corridors can be:

- affordance zones along banks;
- soft blockers from mud, deadfall, wait-a-while, wet rocks, or dense riparian clutter;
- hard blockers where water, gorge, or steep bank geometry blocks traversal;
- false affordances where the player is tempted into a route that worsens.

### Basin and Alluvial Zones

Basins and alluvial flats should create wetter, flatter, lower-visibility movement spaces. These are not arbitrary swamp vibes; they must derive from slope, valley index, flow accumulation, and drainage position.

### Gorge and Gully Compression

Gorges and gullies should compress sightlines and sound. They can become strong route channels, false shortcuts, or risk zones depending on wetness, slope, and exit availability.

### Coastline Classes

The ocean is the natural world boundary. Coastline must vary:

- beach;
- rocky shore;
- mangrove/creek mouth;
- cliff coast;
- exposed headland;
- coastal scrub transition;
- dead-end false affordance coast routes.

Do not make the coast a clean circular track unless the terrain supports it.

## First-Person Readability

Every macro landform should eventually be readable from ground level through:

- slope feel;
- sound;
- sky windows;
- canopy density;
- water behavior;
- vegetation transition;
- landmark silhouettes;
- footing and route resistance;
- weather and light differences.

## Acceptance Criteria

A reviewer should be able to explain why a route is possible, risky, blocked, or misleading using terrain and biome evidence rather than UI markers or authored path lines.

## Forbidden Drift

Do not add route lines, map UI, compass UI, bearing readouts, GPS, objective arrows, HUD clocks, explicit time, or orientation-confidence feedback.
