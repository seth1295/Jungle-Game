# SELF-ITERATIVE 008 — Morphology Diagnostics and Shape Metrics

## Status

implemented-runtime

## Goal

Replace the overly blunt Batch006 morphology acceptance gate with diagnostics that directly measure the visible terrain failures: broad synthetic highland coverage, weak drainage/relief incision, overly gentle slopes, and any remaining ridge/gully radial lock.

Runtime 007 softened catchment-domain transition effects and restored additional source/exporter parity, but the image is still not final-realistic. Runtime 008 should make the preview manifest tell the truth about why.

## Evidence From Runtime 007

Temporary 512 px evidence after CodeRabbit fixes:

- `shoreline_error_max_m: 0.0`
- `beach_continuity_pct: 100.0`
- `ocean_below_sea_pct: 100.0`
- `square_edge_ocean_violations: 0`
- `height_max_m: 3467.8707`
- `drainage_density_proxy: 0.20252`
- `highland_land_area_pct: 61.0856`
- `slope_pct_gentle_moderate_steep: [86.5674, 13.2322, 0.2005]`
- `ridge_gully_angular_lock_score: 0.08646`
- `catchment_angular_concentration_score: 0.95994`
- `morphology_diagnostics_accepted: false`
- `dem_calibration_accepted: false`

Visual inspection: old turbine spokes are gone, but the island still reads as a broad smooth highland/shield with weak incised drainage and insufficient mid-slope terrain complexity.

## Suspected Cause

The old acceptance gate still treats `catchment_angular_concentration_score` as the key morphology failure. That score is useful for debug-domain shape, but it is not the main visible terrain problem after runtime 007. The visible problem is terrain form: too much highland area, too little drainage/relief complexity, too many gentle slopes, and no enough independent shape criteria to guide the final evidence pass.

## Planned Scope

Update:

- `scripts/terrain-preview-export.py`

Targets:

1. keep existing hard coast/beach/ocean gates;
2. keep `catchment_angular_concentration_score` as a debug metric but stop using it as the sole morphology blocker;
3. add explicit morphology shape gates for highland area, drainage density, moderate-slope coverage, steep-slope ceiling, radial alignment, and volcano dominance;
4. add manifest booleans showing each morphology sub-gate;
5. generate temporary inspection evidence only;
6. do not stage final `Images/TerrainPreview/009/` evidence in this runtime.

## Acceptance Target

Runtime 008 is worth landing if:

- exporter syntax passes;
- temporary preview generation passes;
- hard coast gates remain clean;
- manifest now truthfully explains the remaining visual failure through shape metrics;
- CodeRabbit returns no valid blocking findings.

## Result

Runtime 008 updated the preview exporter manifest to report separate morphology sub-gates instead of using hard catchment angular concentration as the main visual blocker. Added gates:

- `morphology_hard_coast_gates_accepted`
- `morphology_radial_lock_accepted`
- `morphology_highland_area_accepted`
- `morphology_drainage_density_accepted`
- `morphology_slope_balance_accepted`
- `morphology_volcano_dominance_accepted`
- `morphology_domain_debug_accepted`

Temporary 512 px inspection output was generated under `Images/TerrainPreview/_scratch/008/`, inspected through `repo_image_view`, and removed before staging. The new diagnostics correctly identify the current visual terrain debt:

Passes:

- hard coast gates: shoreline error `0.0`, beach continuity `100.0%`, ocean below sea `100.0%`, square-edge ocean violations `0`
- radial lock: `ridge_gully_angular_lock_score: 0.08646`, `radial_alignment_artifact_score: 0.40478`
- volcano dominance: active volcano land area `1.0215%`, volcano dominance `11.3461%`
- domain debug: catchment entropy `0.93426`, catchment CV `0.66081`, catchment angular concentration `0.95994`

Fails:

- highland area: `61.0856%` against target `28.0 <= highland <= 55.0`
- drainage density: `0.20252` against target `0.24 <= drainage <= 0.44`
- slope balance: `[86.5674, 13.2322, 0.2005]` gentle/moderate/steep against targets moderate `>= 18.0`, gentle `<= 78.0`, steep `<= 8.0`

This makes runtime 009 targets explicit: reduce broad highland coverage, add real mid-slope/drainage relief, and keep coast/radial/volcano gates clean while exporting final tracked evidence under `Images/TerrainPreview/009/`.
