# SELF-ITERATIVE 005 — Non-Radial Detail and Acceptance Tuning

## Status

implemented-runtime

## Goal

Use the final allowed PR in this self-iterative loop to make the Batch006 preview read better by restoring natural local terrain detail without reintroducing radial spokes or hard graph-domain plates.

## Evidence From Runtime 004

Runtime 004 generated `Images/TerrainPreview/007/`.

Improved:

- old turbine/fan-blade morphology remains gone;
- graph-domain plates are less visually harsh than `005`/`006`;
- ridge/gully polar lock stays low at `0.08918`;
- coast/beach/ocean gates remain clean.

Still wrong:

- central and midland highlands are too smooth and broad;
- terrain lacks natural local roughness, erosional texture, and secondary landform complexity;
- `catchment_angular_concentration_score` remains high at `0.95995`;
- `morphology_diagnostics_accepted` remains false.

## Suspected Cause

The first four runtimes removed radial spokes and reduced blocky domain plates, but runtime 004 reduced ridge/gully relief enough that the preview became overly smooth. The final pass should add non-radial terrain character from local landform texture and acceptance tuning, not from catchment-sector carving.

## Planned Scope

Update both:

- `Source/JungleGame/JungleVolcanicIslandTerrainModel.cpp`
- `scripts/terrain-preview-export.py`

Targets:

1. add non-radial local landform texture using cross-cutting world-space waves/noise independent of catchment ID;
2. add subtle terrace/scarp fragments to break smooth central/midland slopes;
3. preserve low `ridge_gully_angular_lock_score`;
4. tune the catchment-domain acceptance so it reflects the current debug-domain limitation without falsely re-accepting the old spoke morphology;
5. regenerate final loop evidence into `Images/TerrainPreview/008/`;
6. inspect color relief, radial diagnostic, height, and catchment ID images through `repo_image_view`.

## Acceptance Target

This is the last PR in the current five-PR loop. The target is not perfect production terrain; it is a materially better Batch006 preview state with truthful acceptance evidence.

Must keep:

- `shoreline_error_max_m <= 0.25`
- `beach_continuity_pct >= 99.0`
- `ocean_below_sea_pct >= 99.0`
- `square_edge_ocean_violations == 0`
- `ridge_gully_angular_lock_score <= 0.38`

Should improve:

- central/midland visual detail;
- visible naturalness of color relief;
- diagnostic honesty so old radial spokes cannot pass again.

If images still do not look good enough after this runtime, stop at five PRs and report remaining morphology debt instead of silently continuing.

## Result

Runtime 005 added non-radial local midland detail, terrace fragments, and scarp fragments in source and exporter. Generated evidence in `Images/TerrainPreview/008/` preserves the hard coast gates and keeps ridge/gully polar lock low:

- `shoreline_error_max_m: 0.0`
- `beach_continuity_pct: 100.0`
- `ocean_below_sea_pct: 100.0`
- `square_edge_ocean_violations: 0`
- `ridge_gully_angular_lock_score: 0.08918`
- `volcano_dominance_pct: 10.8915`

The loop did not fully solve Batch006 terrain morphology inside the five-PR budget. The generated image is substantially less turbine-like than `003`, but still synthetic: the broad highland remains too smooth and the catchment debug domains remain too concentrated. Final remaining failing evidence:

- `catchment_angular_concentration_score: 0.95995`
- `morphology_diagnostics_accepted: false`
- `dem_calibration_accepted: false`

Stop after runtime 005 unless the user explicitly authorizes another self-iterative loop or extends the PR budget.
