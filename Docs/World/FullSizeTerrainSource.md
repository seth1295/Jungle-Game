# Full-Size Terrain Source

Created: 2026-06-26
Runtime input: `pr5lifecycle-runtimefiles/002/002_deterministic_full_size_terrain_source_research.md`

## Scope

This document defines the deterministic source path for the first full-size Jungle Game terrain state. It is a terrain-source contract, not a final imported landscape, final foliage pass, or final PCG pass.

## Canonical Source Target

The first full-size source terrain target is:

| Field | Value |
| --- | --- |
| Source vertices X | `8129` |
| Source vertices Y | `8129` |
| Source cell spacing | `2 m` |
| Source extent | `16.256 km x 16.256 km` |
| Sea level | `0 m` |
| Height min | `-128 m` |
| Height max | `896 m` |
| Height range | `1024 m` |
| UE XY scale | `200 cm` |
| UE Z scale | `200` |
| UE landscape Z offset | `38400 cm` |
| Config id | `JG_FULL_TERRAIN_SOURCE_002` |
| Intended world | `/Game/JungleGame/Maps/WLD_JungleIsland_Full` |

## Deterministic Source Rule

The terrain must be reproducible from a small source config and fixed seed. The source config must define structural controls before any detail noise is applied.

Required source controls:

- island outline;
- coastline controls;
- ocean boundary ring;
- mountain spine;
- ridge and spur controls;
- saddle and slope-break controls;
- creek heads;
- basin and valley controls;
- gorge candidates;
- coast-type controls;
- deterministic seed and schema version.

Noise may be used only as a secondary detail layer. Noise must not be the primary island, ridge, creek, or traversal structure.

## Required Derived Channels

The deterministic source path must produce or define these channels:

| Channel | Unit / type | Purpose |
| --- | --- | --- |
| `height_m` | meters | canonical source elevation |
| `slope_deg` | degrees | traversal and terrain steepness |
| `aspect_deg` | degrees | sun/weather/ecology orientation inputs |
| `flow_accumulation` | cells or normalized | creek head and drainage validation |
| `ridge_valley_signed_distance_m` | meters | ridge/valley classification |
| `wetness_proxy` | normalized | wet valley and lowland influence |
| `coast_signed_distance_m` | meters | inland/ocean/coast influence |
| `creek_influence` | normalized | riparian/traversal classification |
| `coast_type` | category | beach, rocky shore, mangrove, creek mouth, cliff coast |
| `review_landform_class` | category | review/sample landform identity |

## Review Points

Initial developer-only terrain source review point intents:

| Review point | Intent |
| --- | --- |
| `FTS_RP_MountainSpine` | mountain spine height/ridge structure |
| `FTS_RP_RidgeSpur` | spur and ridge traversal affordance |
| `FTS_RP_SaddlePass` | saddle route candidate |
| `FTS_RP_CreekHead` | creek head and flow sanity |
| `FTS_RP_WetValley` | wet valley/riparian transition |
| `FTS_RP_AlluvialLowland` | lowland/alluvial flat |
| `FTS_RP_CreekMouth` | creek-to-coast transition |
| `FTS_RP_GorgeCandidate` | compressed gorge candidate |

These are developer review points only. They must not become player-facing navigation aids.

## Artifact Policy

Commit:

- deterministic source config or documented schema;
- small manifests;
- small Markdown/CSV sample tables;
- small downsampled previews only when explicitly useful and reviewable;
- hashes and channel stats.

Do not commit by default:

- full-resolution generated rasters;
- raw `.r16` import files;
- large intermediate arrays;
- DerivedDataCache;
- Saved/Intermediate/package output;
- local screenshot or log evidence.

## Acceptance Criteria

This PR is accepted when:

- the full-size source scale is documented;
- deterministic controls are defined;
- required derived channels are named;
- developer review point intents are named;
- artifact/source-control policy is clear;
- no player-facing map, minimap, compass, bearing readout, GPS, objective marker, HUD clock, explicit time display, or orientation-confidence UI is introduced.

## Next PR

PR5 Batch 002 / 003 should realize the first in-game full-size terrain shell from this source direction or a valid staged placeholder that preserves this source contract.
