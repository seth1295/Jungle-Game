# PR5 Batch 003 / 004: Active Crater, Lava Crust, And Terrain Hazards

## Purpose

This PR adds terrain-only active-volcano summit structure to the Batch 003 volcanic island generator.

The terrain model now includes:

- asymmetric summit crater depression;
- broken crater rim mask;
- nested active vent / pit mask;
- breached rim notch aligned to an upper-flank corridor;
- fissure corridor mask;
- lava-crust terrain mask;
- unstable-crust terrain mask;
- hard-blocker terrain mask;
- crater depression and rim-raise height contributions.

## Terrain-Only Rule

This is not a VFX, lava-rendering, heat, burn, gas, damage, weather, audio, or survival-mechanics PR.

The volcano reads through terrain geometry and developer masks only:

```text
crater depression
broken rim
nested vent/pit
breach incision
fissure corridor
lava/crust terrain mask
unstable-crust mask
hard-blocker mask
```

Future systems can consume these masks, but this PR does not implement those future systems.

## Runtime Evidence

The terrain metrics token advances to:

```text
JG_VOLCANIC_TERRAIN_004
```

The log line now includes:

- crater mask max;
- vent mask max;
- hard-blocker mask max;
- existing massif, catchment, ridge, gully, lahar, island, sea-level, and square-edge metrics.

## Scope Boundary

In scope:

- crater/rim/vent/breach/fissure terrain math;
- lava-crust and unstable-crust masks;
- hard-blocker terrain mask;
- developer-only metric evidence.

Out of scope:

- smoke;
- steam;
- sparks;
- glow;
- lava animation;
- heat haze;
- final materials;
- burn/heat/gas/damage mechanics;
- audio;
- weather;
- foliage/ecology;
- story/NPC/gameplay scripting;
- player-facing navigation UI.

No map, minimap, compass, GPS, bearing readout, objective marker, HUD clock, explicit time display, orientation meter, or orientation-confidence UI is introduced.

## Next Batch 003 Target

PR5 Batch 003 / 005 should harden the sea-level beach ring and coastal shelf acceptance layer, add shoreline/coast metrics, and create the tracked Batch 003 handoff after merge.
