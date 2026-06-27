# SELF-ITERATIVE 004 — Domain Blend and Midland Landform Breakup

## Status

implemented-runtime

## Goal

Remove the remaining blocky graph-domain plates and make the broad midland/highland terrain read as natural landform regions instead of a smooth central shield with polygon catchment overlays.

## Evidence From Runtime 003

Runtime 003 generated `Images/TerrainPreview/006/`.

Improved:

- radial fan-blade morphology is gone compared with `003`/`004`;
- ridge/gully angular lock remains low at `0.08918`;
- peak moved off the old bullseye;
- volcano dominance dropped to `10.3481%`;
- coast/beach/ocean gates remain clean.

Still wrong:

- large graph-domain plates are visible in color relief and radial diagnostic;
- central/midland highland is still too smooth and center-owned;
- catchment-domain metric remains high at `0.95995`;
- `morphology_diagnostics_accepted` remains false.

## Suspected Cause

Runtime 002/003 changed the catchment source, but terrain height still uses hard best-domain membership and broad shield relief. The new graph domains are better than polar sectors, but their influence is still too discrete. The terrain needs blended domain influence and independent midland landform breakup so visible relief does not trace catchment plates.

## Planned Scope

Update both:

- `Source/JungleGame/JungleVolcanicIslandTerrainModel.cpp`
- `scripts/terrain-preview-export.py`

Targets:

1. blend the top two catchment domain scores into terrain-process height instead of letting hard `catchment_id` dominate visible relief;
2. add midland benches, scarp fragments, and basin-floor undulation independent of catchment IDs;
3. reduce visible plate boundaries in color relief while keeping catchment ID evidence available as debug/evidence only;
4. keep coast/beach/ocean gates clean;
5. regenerate evidence into `Images/TerrainPreview/007/`;
6. inspect color relief, radial diagnostic, and catchment ID images through `repo_image_view`.

## Acceptance Target

The next preview should no longer read as a set of polygon catchment plates. Numeric goals:

- keep `ridge_gully_angular_lock_score <= 0.38`;
- reduce `catchment_angular_concentration_score` below `0.95995` where practical;
- maintain clean coast/beach/ocean gates;
- image review should show broken midland relief, not a single smooth central shield.

If images still look bad, write `SELF-ITERATIVE/005_<scope>.md` from the new evidence. Runtime 005 is the last allowed PR in the current user loop unless the user extends the budget.

## Result

Runtime 004 reduced visible polygon-domain plate strength by lowering ridge/gully carving, adding midland breakup, and keeping the graph-domain source/tool parity. Generated evidence in `Images/TerrainPreview/007/` keeps coast/beach/ocean gates clean and maintains low ridge/gully polar lock:

- `shoreline_error_max_m: 0.0`
- `beach_continuity_pct: 100.0`
- `ocean_below_sea_pct: 100.0`
- `square_edge_ocean_violations: 0`
- `ridge_gully_angular_lock_score: 0.08918`

The image is not final. It is less polygonal, but now too smooth and broad in the central/midland highlands. The catchment-domain metric remains high at `0.95995`, and `morphology_diagnostics_accepted` remains false.

Next runtime: `SELF-ITERATIVE/005_non_radial_detail_and_acceptance_tuning.md`. This is the last allowed PR in the current five-PR loop.
