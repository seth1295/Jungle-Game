# Terrain Source Data Contract

Created: 2026-06-25
Track: world-first terrain/biome implementation

## Purpose

Define the terrain-derived source data that every later biome, traversal, and no-map navigation system consumes. The world must not be authored as isolated set pieces first; landform and terrain evidence are the primary game substrate.

## Required Terrain Channels

### Height

World-space elevation relative to island sea level. Used for mountain spine, basin structure, creek head formation, coastal compression, and line-of-sight expectations.

### Slope

Local steepness. Used for fatigue pressure, fall/injury risk, traversability, ridge readability, false affordances, and biome density limits.

### Aspect

Facing direction of terrain. Used for exposed/sheltered vegetation differences, sun/shadow readability, moisture retention, and wind/weather exposure.

### Ridge Index

Relative ridgeline/spur/saddle likelihood. Used for no-map navigation cues, sky-window preservation, skyline landmarks, and movement affordance bands.

### Valley Index

Relative gully/valley/basin likelihood. Used for creek influence, wetness, sound propagation, compressed visibility, and ambush/uncertainty pressure without explicit horror UI.

### Flow Accumulation

Proxy for drainage paths and creek scale. Used for creek/riparian biome passes, crossing candidates, wet valley density, and water-sound anchors.

### Coast Distance

Distance to ocean/coastline. Used for island boundary readability, beach/mangrove/cliff-coast transitions, ocean-sound cues, and coastal false affordances.

### Creek Distance

Distance to channel center or wet drainage corridor. Used for bank affordances, mud/deadfall/wait-a-while soft blockers, leech/tick placeholder risk, and frog/insect/waterfall sound anchors.

### Canopy Closure Proxy

Expected overhead vegetation closure. Used for sky-window navigation, sun cue degradation, night readability, rain muffling, and screenshot validation.

### Disturbance / Gap Proxy

Expected regrowth gaps, treefall openings, washouts, scars, and misleading clearings. Used for false affordances, route temptation, temporary visibility, and ecological tells.

## Derived Traversal Class Inputs

Every playable terrain cell or sampled point should be classifiable as one of:

- hard blocker;
- soft blocker;
- affordance;
- false affordance;
- neutral friction.

The classification must derive from terrain and ecosystem channels, not authored route lines.

## Validation Expectations

Representative sample points must be recorded for:

- coastline;
- creek bank;
- creek crossing candidate;
- wet valley;
- ridge spur;
- saddle;
- steep slope;
- basin floor;
- gorge corridor;
- lowland alluvial zone;
- canopy gap;
- false clearing.

Each point should eventually be inspectable with raw channel values, derived traversal category, biome expectation, and first-person screenshot evidence.

## Forbidden Outputs

Do not turn this data into player-facing map, minimap, compass, bearing number, GPS, objective marker, HUD clock, explicit time display, or orientation-confidence UI.
