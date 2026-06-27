# SELF-ITERATIVE 007 — Soft Catchment / Domain Blending

## Status

implemented-runtime

## Goal

Reduce visible hard graph-domain plate behavior in the Batch006 terrain by softening catchment-domain influence at graph ownership transitions while keeping the hard catchment ID output available as developer diagnostic evidence.

Runtime 006 restored source/exporter terrain parity. Runtime 007 can now target the actual terrain morphology without hiding source/evidence divergence.

## Evidence From Runtime 006

Runtime 006 temporary preview generation preserved hard coast gates but morphology remained unaccepted:

- `shoreline_error_max_m: 0.0`
- `beach_continuity_pct: 100.0`
- `ocean_below_sea_pct: 100.0`
- `square_edge_ocean_violations: 0`
- `catchment_angular_concentration_score: 0.95997`
- `ridge_gully_angular_lock_score: 0.08923`
- `morphology_diagnostics_accepted: false`
- `dem_calibration_accepted: false`

Visual review of `Images/TerrainPreview/008/` showed that old turbine spokes are mostly gone, but the color relief still reads as broad synthetic highland plates and the catchment ID diagnostic remains large hard graph regions.

## Suspected Cause

The graph-domain rewrite still selects a single winning catchment for terrain shaping. Ridge, gully, fan, branch, lahar, and catchment-driven detail use the hard winner's width/strength/curve values. Around graph-domain transitions this creates visible plate boundaries or overly broad same-domain terrain behavior, even though the ridge/gully polar lock metric is already low.

## Planned Scope

Update both:

- `Source/JungleGame/JungleVolcanicIslandTerrainModel.cpp`
- `scripts/terrain-preview-export.py`

Targets:

1. compute a soft graph-domain blend factor from the best-vs-second catchment graph cost gap;
2. widen and damp catchment-driven ridge/gully/fan influence near domain transition zones;
3. keep `CatchmentId` as a hard developer diagnostic output, but reduce how much hard ownership controls terrain relief;
4. generate temporary inspection preview output only;
5. inspect color relief, catchment ID, and radial diagnostic images before PR;
6. do not stage or overwrite final tracked `Images/TerrainPreview/009/` evidence in this runtime.

## Acceptance Target

Runtime 007 is worth landing if:

- coast/beach/ocean gates remain clean;
- ridge/gully angular lock remains low;
- visible color relief shows less hard graph-domain plate behavior than `008`;
- temporary output confirms no tracked preview folders were overwritten;
- source and exporter stay in parity for the changed terrain math;
- CodeRabbit returns no valid blocking findings.

## Result

Runtime 007 added soft catchment/domain transition damping in both C++ and the preview exporter:

- derives a graph-domain transition mask from best-vs-second catchment graph cost gap;
- widens/damps ridge, gully, branch, lahar, and fan influence near domain transitions;
- keeps hard `CatchmentId` output as developer diagnostic evidence instead of treating it as blended display output;
- keeps source and exporter terrain math in parity for the changed domain transition behavior.

This runtime also found and fixed additional exporter/source constant drift in the massif shape path: exporter massif base radius, concave shield exponents, upper steepening, and shoulder bench constants now match the C++ source.

Temporary 512 px inspection output was generated under `Images/TerrainPreview/_scratch/007/` and inspected through `repo_image_view`, then removed before staging. Results:

- `shoreline_error_max_m: 0.0`
- `beach_continuity_pct: 100.0`
- `ocean_below_sea_pct: 100.0`
- `square_edge_ocean_violations: 0`
- `height_max_m: 3467.8707`
- `volcano_disabled_peak_m: 3074.403`
- `ridge_gully_angular_lock_score: 0.0932`
- `catchment_angular_concentration_score: 0.95994`
- `morphology_diagnostics_accepted: false`
- `dem_calibration_accepted: false`

Visual inspection: color relief is less hard-plate obvious than `008`, but not final-realistic. It still reads as a broad synthetic highland with weak natural drainage incision. Runtime 008 should improve the morphology diagnostics and shape criteria so later evidence stops over-weighting hard catchment angular concentration and starts measuring the visible terrain issues directly.
