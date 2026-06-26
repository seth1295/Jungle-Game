# Packaged Full-Size World Smoke

Created: 2026-06-26
Runtime input: `pr5lifecycle-runtimefiles/002/005_packaged_full_size_world_smoke_evidence_research.md`

## Scope

This document defines the packaged full-size world smoke evidence contract for PR5 Batch 002. It does not commit generated screenshots, package output, logs, Saved, Intermediate, DerivedDataCache, or cooked artifacts.

## Smoke Identity

Smoke id:

```text
JG_FULL_WORLD_PACKAGE_SMOKE_002
```

Intended final world path:

```text
/Game/JungleGame/Maps/WLD_JungleIsland_Full
```

Current bridge map path while no project-owned `.umap` exists:

```text
/Engine/Maps/Templates/OpenWorld
```

Required runtime log token from PR5 Batch 002 / 003:

```text
Full-size terrain shell spawned
```

## Required Review Points

Packaged full-size smoke evidence should cover these developer-only review points:

| Review point | Expected evidence |
| --- | --- |
| `Review_Coast_North` | shoreline and ocean boundary |
| `Review_Coast_CreekMouth` | creek-to-coast transition |
| `Review_Ridge_Spine` | high-ground orientation structure |
| `Review_Creek_Valley` | low channel corridor |
| `Review_Basin_Lowland` | safe lowland start |
| `Review_Highland_Shoulder` | lowland-to-highland transition |
| `Review_Mountain_Lookout` | world-scale visibility |
| `Review_Cell0_BridgeAnchor` | legacy anchor only, not full world authority |

Every point is developer-only evidence. These names must not become player-facing map, objective, compass, bearing, or route UI.

## Required Log Fields

A packaged smoke report should record:

- smoke id;
- executable/build configuration;
- loaded map name;
- intended world asset path;
- current bridge map path if still in use;
- terrain shell runtime log token observed or missing;
- review point name;
- player location;
- downward terrain hit result;
- radial visibility hits where practical;
- screenshot/artifact external path;
- result: `NotRun`, `Pass`, `Fail`, or `Blocked`;
- blocker reason where relevant.

## Artifact Policy

Commit only docs/specs and source contracts.

Do not commit:

- generated screenshots;
- generated logs;
- packaged builds;
- Saved;
- Intermediate;
- DerivedDataCache;
- full generated terrain rasters;
- local validation output.

## Current Validation State

The source-authored runtime shell has been added, but a fresh packaged build was not produced in this PR because the available UE Build.sh invocation hit the DevSpace 300s timeout during PR5 Batch 002 / 003 without returning a compiler error.

The next packaged validation run should prove that the packaged executable includes the new runtime shell by finding:

```text
Full-size terrain shell spawned
```

in runtime logs.

## Batch 002 Completion

PR5 Batch 002 establishes:

1. full-size world authority shell;
2. deterministic full-size terrain source contract;
3. source-authored full-size terrain runtime shell;
4. developer-only mask/biome/traversal debug state;
5. packaged smoke evidence contract and batch handoff.

## Next Batch

PR5 Batch 003 should move from cube-block full-size world state to a deterministic procedural terrain substrate before deeper traversal and ecosystem readability work. The next first slice should produce a coarse continuous terrain mesh or heightfield with basin, creek-valley, ridge, mountain-shoulder, coast, and ocean-edge shape logic while keeping fixed seed/config identity, source-owned generation, developer-only review points, and the existing cube shell as fallback/debug evidence.

After that terrain shell v2 / PCG terrain foundation exists, continue into:

- creek/riparian traversal;
- ridge/slope/mountain readability;
- lowland/alluvial/gorge/coast transitions;
- diegetic navigation cues;
- day/night/weather traversal pressure.
