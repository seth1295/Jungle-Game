# SELF-ITERATIVE 003 — Catchment Domain Smoothing and Relief Breakup

## Status

implemented-runtime

## Goal

Smooth the remaining graph-domain blockiness and break up the broad center-owned volcanic shield visible in `Images/TerrainPreview/005/` while preserving the Runtime 002 improvement that removed the strongest ridge/gully polar lock.

## Evidence From Runtime 002

Runtime 002 generated `Images/TerrainPreview/005/` and changed both source and exporter to use curved graph-domain catchments instead of the shared polar-sector field.

Improved:

- `ridge_gully_angular_lock_score: 0.08188`
- coast/beach/ocean gates stayed clean
- the color relief no longer reads as the same clean turbine/fan-blade pattern from `003`/`004`

Still wrong:

- catchment domains read as hard blocky polygons;
- center volcanic shield still owns too much of the island visually;
- catchment angular concentration remains too high at `0.95995`;
- the color relief still does not look like natural terrain.

## Suspected Cause

The new graph-domain catchment assignment is too winner-take-all. The terrain now follows deterministic graph domains, but the hard best-domain selection and divide masks create large polygonal plates. Relief also still preserves a broad circular shield around the center, so the image keeps a center-owned look even after removing most ridge/gully polar lock.

## Planned Scope

Update both:

- `Source/JungleGame/JungleVolcanicIslandTerrainModel.cpp`
- `scripts/terrain-preview-export.py`

Targets:

1. soften catchment-domain transitions with blended top-two domain influence instead of hard winner-take-all masks;
2. reduce polygonal boundaries in ridge/divide masks using domain-margin smoothing and noise/detail breakup;
3. break the broad central shield with asymmetric terraces, saddles, benches, and secondary upland relief;
4. keep coast/beach/ocean gates clean;
5. regenerate evidence into `Images/TerrainPreview/006/`;
6. inspect color relief, radial diagnostic, and catchment images through `repo_image_view`.

## Acceptance Target

The next preview should visibly reduce blocky/polygon catchment edges and center-owned shield dominance. Numeric targets:

- `shoreline_error_max_m <= 0.25`
- `beach_continuity_pct >= 99.0`
- `ocean_below_sea_pct >= 99.0`
- `square_edge_ocean_violations == 0`
- keep `ridge_gully_angular_lock_score <= 0.38`
- reduce `catchment_angular_concentration_score` from `0.95995`
- image review should show naturalized domains rather than obvious polygons.

If images still look bad after this runtime, write `SELF-ITERATIVE/004_<scope>.md` from the new evidence.

## Result

Runtime 003 added asymmetric shield breakup, stronger saddle cuts, secondary upland pushes, and softened ridge/domain breakup in source and exporter. Generated evidence in `Images/TerrainPreview/006/` shows visible improvement over `005`: peak moved off the old bullseye, volcano dominance dropped to `10.3481%`, and coast/beach/ocean gates remain clean.

Still not final:

- color relief retains large blocky graph-domain plates;
- central highland still reads too smooth and broad;
- catchment-domain metric remains high: `catchment_angular_concentration_score: 0.95995`;
- `morphology_diagnostics_accepted: false`.

Next runtime: `SELF-ITERATIVE/004_domain_blend_and_midland_landform_breakup.md`.
