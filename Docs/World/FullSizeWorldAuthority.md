# Full-Size World Authority

Created: 2026-06-26
Runtime input: `pr5lifecycle-runtimefiles/002/001_full_size_world_authority_research.md`

## Scope

This document defines the first PR5 Batch 002 authority state for the full-size Jungle Game world. It is intentionally an authority shell, not a claim that final terrain, foliage, PCG, or Cell 0 large-world integration is complete.

## Authority Target

The intended project-owned full-size world asset is:

```text
/Game/JungleGame/Maps/WLD_JungleIsland_Full
```

The current packaged bridge world remains:

```text
/Engine/Maps/Templates/OpenWorld
```

The bridge world is allowed only while the project has no committed project-owned `.umap` asset. It must not be described as the final island, final world, or completed full-size map state.

## Authority Status

Current status:

```text
AuthorityShell
```

Meaning:

- the full-size world identity is defined;
- Cell 0 is classified as a review anchor only;
- template OpenWorld is classified as a bridge/template source only;
- future full-size world runtime work must target the intended project-owned world identity;
- config must not be pointed at a missing `.umap` asset.

## Cell 0 Transition Policy

Cell 0 remains useful as a playable validation slice and future placed location, but it must not define or fake the full world.

Policy:

```text
LegacyCell0Policy: ReviewAnchorOnly
```

Allowed:

- keep Cell 0 playable as a bridge smoke target;
- use Cell 0 as a review anchor while the full-size world shell is being built;
- later place Cell 0 inside the full-size world.

Forbidden:

- treating Cell 0 as the whole island;
- claiming Cell 0 validates full-size world traversal;
- using Cell 0 as player-facing map/orientation rescue.

## Template OpenWorld Policy

Policy:

```text
TemplateOpenWorldPolicy: TemplateSourceOnly
```

Allowed:

- launch the template OpenWorld while no project-owned map asset exists;
- use it as a source-only bridge for runtime generated/source-authored validation geometry.

Forbidden:

- calling OpenWorld the full-size world;
- leaving it as the final intended authority path;
- using it as evidence that the full-size map state has been realized.

## Developer Review Points

Developer-only review points are named in source through `FJGFullSizeWorldAuthoritySpec`.

Initial review point intents:

| Review point | Intent |
| --- | --- |
| `FSW_RP_Cell0Bridge` | Existing Cell 0 bridge spawn/review anchor |
| `FSW_RP_FutureCoast` | Future coast review location |
| `FSW_RP_FutureRidge` | Future ridge review location |
| `FSW_RP_FutureCreek` | Future creek review location |
| `FSW_RP_FutureBasin` | Future basin review location |

These review points are developer-only and must not become player-facing navigation UI.

## Validation Evidence

Minimum evidence for this PR:

- source authority contract exists;
- docs define the intended full-size world asset path;
- docs explicitly classify Cell 0 and OpenWorld as non-authoritative bridge states;
- runtime research input is preserved;
- no player-facing map, minimap, compass, bearing readout, GPS, objective marker, HUD clock, explicit time display, or orientation confidence UI is added.

## Next PR

PR5 Batch 002 / 002 should define the deterministic full-size terrain source pipeline that feeds the future project-owned full-size world state.
