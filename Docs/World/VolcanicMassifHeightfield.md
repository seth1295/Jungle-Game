# PR5 Batch 003 / 002: 1400 m Volcanic Massif Heightfield

## Purpose

This PR moves the Batch 003 terrain substrate from actor-local height math into a source-owned volcanic island terrain model.

The runtime mesh is still a preview output, but terrain authority now lives in `FJungleVolcanicIslandTerrainModel`:

- 16,256 m square container.
- 0 m sea-level datum.
- organic island shoreline radius.
- ocean/shelf/beach/coastal-lowland masks.
- protected coastal transition before inland volcanic terrain dominates.
- 1400 m-class central volcanic massif.
- deterministic runtime metrics log token.

## Terrain Contract

The world square remains a technical container. The island is an organic/circular landmass inside that square.

Required invariants introduced here:

```text
world_size_m = 16256
sea_level_m = 0
mean_island_radius_m = 7000
max_island_radius_m = 7350
target_peak_height_m = 1400
minimum_square_edge_ocean_margin_m = 778 nominal from max island radius
```

The terrain model samples in metres and the shell actor converts the result to Unreal centimetres only at the runtime mesh boundary.

## Runtime Evidence

The terrain shell logs a deterministic metrics line:

```text
JG_VOLCANIC_TERRAIN_002
```

The log reports:

- world size;
- sea level;
- sampled min/max elevation;
- min/max organic island radius;
- minimum ocean margin to square edge;
- square-edge height range;
- target peak elevation.

## Scope Boundary

In scope:

- deterministic terrain height function;
- source-owned terrain model class;
- massif profile;
- beach/coastal/ocean protection masks;
- runtime metrics log evidence.

Out of scope:

- foliage;
- ecology;
- weather;
- audio;
- VFX;
- smoke or lava rendering;
- survival damage/heat mechanics;
- animals/NPC/story content;
- player-facing navigation UI.

Developer-only diagnostics are allowed. No map, minimap, compass, GPS, bearing readout, objective marker, HUD clock, explicit time readout, orientation meter, or orientation-confidence UI is introduced.

## Next Batch 003 Target

PR5 Batch 003 / 003 should add radial ridges, catchments, gullies, lahar-capable drainage paths, and coast-resolving outlet/fan logic to the same source-owned terrain model.
