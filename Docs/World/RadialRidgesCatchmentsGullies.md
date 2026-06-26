# PR5 Batch 003 / 003: Radial Ridges, Catchments, Gullies, And Lahar Corridors

## Purpose

This PR adds the first deterministic volcanic drainage structure to the Batch 003 terrain model.

The island terrain now includes:

- 14 primary radial catchment sectors;
- ridge masks on catchment divides;
- gully masks through catchment centres;
- selected lahar-capable trunk corridors;
- lower-slope coastal fan masks;
- metric logging for ridge, gully, lahar, and catchment evidence.

## Terrain Rules

Ridges, gullies, and fans are terrain-generation features only. They are not authored trails, route UI, quest paths, water rendering, or gameplay hazards.

The terrain process ordering is:

```text
organic island mask
sea-level/ocean/coastal envelope
1400 m massif profile
radial ridge divide raise
gully / lahar incision
coastal fan deposition
runtime preview mesh sample
```

Ridge and gully contributions are constrained by land/coast masks so they fade before the beach ring and do not raise terrain outside the island mask.

## Runtime Evidence

The terrain metrics token advances to:

```text
JG_VOLCANIC_TERRAIN_003
```

The log line reports:

- catchment count;
- max ridge mask;
- max gully mask;
- max lahar corridor mask;
- existing world, sea-level, island radius, ocean margin, square-edge, and height metrics.

## Scope Boundary

In scope:

- terrain height and mask math;
- source-owned catchment IDs;
- ridge/gully/lahar/fan geometry;
- developer-only metrics.

Out of scope:

- foliage;
- ecology;
- weather;
- audio;
- water simulation/rendering;
- lahar gameplay simulation;
- damage/survival systems;
- final art materials;
- player-facing navigation UI.

No map, minimap, compass, GPS, bearing readout, objective marker, HUD clock, explicit time display, orientation meter, or orientation-confidence UI is introduced.

## Next Batch 003 Target

PR5 Batch 003 / 004 should add active crater, breached rim, nested vent, lava-crust terrain masks, unstable crust masks, and hard-blocker terrain geometry to the same source-owned terrain model.
