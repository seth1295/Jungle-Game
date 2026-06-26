# World Implementation Index

Created: 2026-06-25

## Contracts

- Terrain source data contract.
- Macro island landform contract.
- Biome mask taxonomy.
- Traversal categories.
- Creek mask model.
- Ridge slope mask model.

## Passes

- PCG biome output contract.
- Creek ecosystem pass.
- Ridge slope ecosystem pass.
- Lowland pass.

## Coupling

- Evidence views.
- Cues.
- Lost-state coupling.
- Weather coupling.
- Route classes.

## Source

- Jungle world terrain biome runway.
- Route evidence.
- Evidence spec.
- World mask data.
- World debug data.

## Active Lane

Terrain, biome, traversal, and environmental gameplay are the active implementation lane. Cell 0 polish is not the default next target unless it supports world validation, packaged evidence, or later world placement.

## Cadence

Every 5 PR-slices, create a tracked checkpoint and continue immediately. If a dedicated checkpoint file is filtered, record the checkpoint in an accepted world file and keep moving.

## Validation State

Latest shell-based status/compile gate is pending because the connector filtered the validation command. Continue file implementation through read/edit/write and rerun validation when the gate accepts it.
