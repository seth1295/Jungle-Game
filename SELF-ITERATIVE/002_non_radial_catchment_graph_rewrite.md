# SELF-ITERATIVE 002 — Non-Radial Catchment Graph Rewrite

## Status

implemented-runtime

## Goal

Replace the verified Batch006 polar-sector catchment/ridge/gully failure with non-radial catchment-domain math while preserving the clean coast/beach/ocean gates.

## Evidence From Runtime 001

Runtime 001 generated `Images/TerrainPreview/004/` from the current Batch006 terrain and added hard diagnostics. The existing terrain failed because:

- catchments are assigned from closest warped polar angle around the massif center;
- ridge and gully masks are also driven by angular deltas from the same field;
- the previous manifest accepted the terrain because entropy/CV can pass even when catchments are radial sectors.

Failing evidence:

- `catchment_angular_concentration_score: 0.98963`
- `ridge_gully_angular_lock_score: 0.97187`
- `morphology_diagnostics_accepted: false`
- `dem_calibration_accepted: false`

## Suspected Cause

The source and exporter both use `WarpedTheta`, `BestGullyDelta`, and `BestRidgeDelta` as the controlling basis for catchment identity and ridge/gully masks. This creates many balanced catchments, but they remain spoke-shaped because they are angular sectors.

## Planned Scope

Implement a graph/domain-based catchment rewrite in both:

- `Source/JungleGame/JungleVolcanicIslandTerrainModel.cpp`
- `scripts/terrain-preview-export.py`

The rewrite should:

1. assign catchments from deterministic off-center basin/outlet/domain anchors rather than nearest polar angle;
2. use curved trunk corridors based on distances to basin polylines or anchor paths, not only angular deltas;
3. derive ridge/divide masks from boundaries between catchment influence fields;
4. preserve sea-level shoreline, beach continuity, ocean-below-sea, square-edge ocean, x6 world scale, and active-volcano footprint constraints;
5. regenerate preview evidence into `Images/TerrainPreview/005/`;
6. inspect generated images with `repo_image_view`.

## Acceptance Target

The next generated manifest should aim for:

- `shoreline_error_max_m <= 0.25`
- `beach_continuity_pct >= 99.0`
- `ocean_below_sea_pct >= 99.0`
- `square_edge_ocean_violations == 0`
- `catchment_angular_concentration_score <= 0.82`
- `ridge_gully_angular_lock_score <= 0.38`
- visibly reduced turbine/fan-blade morphology in color relief, radial diagnostic, and catchment preview.

If the numeric gates improve but the image still looks bad, write `SELF-ITERATIVE/003_<scope>.md` from the new evidence instead of claiming completion.

## Result

Runtime 002 replaced the ridge/gully polar-sector basis with deterministic curved graph-domain catchments in both the preview exporter and C++ terrain source. Generated evidence in `Images/TerrainPreview/005/` shows the turbine spokes are reduced and ridge/gully angular lock now passes:

- `ridge_gully_angular_lock_score: 0.08188`
- `shoreline_error_max_m: 0.0`
- `beach_continuity_pct: 100.0`
- `ocean_below_sea_pct: 100.0`
- `square_edge_ocean_violations: 0`

The images are still not good enough. They now read less like fan blades but still show blocky catchment domains and a broad center-owned volcanic shield. Remaining failing metric:

- `catchment_angular_concentration_score: 0.95995`

Next runtime: `SELF-ITERATIVE/003_catchment_domain_smoothing_and_relief_breakup.md`.
