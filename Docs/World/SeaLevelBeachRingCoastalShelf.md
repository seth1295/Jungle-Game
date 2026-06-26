# PR5 Batch 003 / 005: Sea-Level Beach Ring And Coastal Shelf

## Purpose

This PR hardens the island-edge terrain invariant for Batch 003:

```text
outside island mask = ocean / below sea level
organic shoreline boundary = sea level
landward edge band = beach, berm, coastal flat, or fan toe
square world edge = ocean/container only
```

The terrain generator now applies a shoreline constraint mask around the organic island radius so the runtime terrain resolves through the 0 m datum instead of allowing massif, ridge, gully, crater, or hazard modifiers to overwrite the coast.

## Runtime Evidence

The terrain metrics token advances to:

```text
JG_VOLCANIC_TERRAIN_005
```

The log line now includes:

- maximum shoreline absolute error;
- beach continuity percentage for sampled beach-band cells;
- square-edge ocean violation count;
- existing island radius, ocean margin, height, ridge, gully, lahar, crater, vent, and hard-blocker metrics.

## Terrain Contract

The shoreline is a terrain constraint, not a decoration pass. Terrain modifiers may create headlands, fans, outlet notches, and local erosion, but they must not create a raised circular rim or square-edge landmass.

The model tracks:

```text
shoreline_constraint_mask
shoreline_error_m
beach_ring_mask
coastal_shelf_mask
coastal_lowland_mask
square_edge_ocean_violations
```

## Scope Boundary

In scope:

- sea-level shoreline locking;
- beach-band validation fields;
- square-edge ocean validation;
- developer-only metric evidence;
- batch handoff for PR5 Batch 003 / 001-005.

Out of scope:

- final water rendering;
- wave simulation;
- foam;
- final beach materials;
- foliage/ecology;
- weather;
- audio;
- survival mechanics;
- story/NPC/gameplay scripting;
- player-facing navigation UI.

No map, minimap, compass, GPS, bearing readout, objective marker, HUD clock, explicit time display, orientation meter, or orientation-confidence UI is introduced.

## Stop Point

After this PR lands, PR5 Batch 003 is intentionally stopped at runtime file 005 per user instruction. Do not start runtime file 006 until explicitly requested.
