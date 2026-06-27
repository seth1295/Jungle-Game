# SELF-ITERATIVE 001 — Batch006 Radial Morphology Diagnosis

## Status

diagnosed-runtime

## Goal

Diagnose why PR5 Batch006 terrain preview evidence under `Images/TerrainPreview/003/` passes manifest metrics while still visually reading as radial, turbine-like, and center-convergent.

This runtime file does not assume the image alone is the source of truth. It requires tool-driven diagnosis across source, exporter, manifest metrics, generated images, and numeric diagnostics.

## Observed Issue

Batch006 preview evidence has clean coast/beach/ocean gates but poor interior morphology:

- ridge/gully structure reads as radial fan blades;
- catchments appear center-convergent;
- the radial pattern remains visible when volcano height is disabled;
- at least one sector/wedge artifact appears in the preview;
- manifest acceptance is too permissive because `dem_calibration_accepted: true` can pass while visible morphology is still wrong.

## Tool Evidence To Use

Use the strongest available project tools:

- inspect `Source/JungleGame/JungleVolcanicIslandTerrainModel.*`;
- inspect `Source/JungleGame/JungleTerrainBatch006Calibration.*`;
- inspect `scripts/terrain-preview-export.py`;
- read `Images/TerrainPreview/003/PR5_Batch006_TerrainCalibration_1024px_Manifest.json`;
- view existing `Images/TerrainPreview/003/*.png` with `repo_image_view`;
- run the preview exporter only when validating a scoped change;
- inspect generated evidence in the next numbered `Images/TerrainPreview/` folder, never overwriting `001`, `002`, or `003`;
- run `git diff --check`, Python syntax checks, and UBT where available;
- run CodeRabbit during PR lifecycle.

## Suspected Cause To Verify

The likely failure is in terrain/catchment/ridge generation math, not only the active volcano overlay:

- basin/ridge/gully directions still derive too strongly from central polar angle;
- catchment IDs may be assigned by angular sector instead of graph-like drainage domains;
- ridge/gully masks may be synchronized to the same radial basis;
- volcano-disabled terrain may retain the same spoke field because the base landform graph is still radial;
- acceptance metrics measure coast, beach, entropy, and a radial score but do not hard-fail the visible fan-blade morphology.

## Planned Scope

This first runtime unit is diagnostic and gate-focused.

Implement or update diagnostics so the current Batch006 morphology cannot pass acceptance simply because coast/beach/ocean metrics are clean.

Valid source/tool targets include:

- Batch006 calibration metrics;
- terrain model morphology evidence fields;
- preview exporter manifest metrics;
- radial/catchment/seam diagnostic computation;
- explicit acceptance booleans for anti-radial morphology.

Do not perform a broad terrain rewrite in this runtime file unless the diagnostic work proves a tiny surgical correction is sufficient.

## Required Validation

Before PR completion:

- current `003`-style morphology must be classified as failing or suspect by the new diagnostics;
- coast/beach/ocean gates must remain separately measurable;
- generated evidence must be placed in the next numbered preview folder if exporter output is regenerated;
- generated images must be inspected through `repo_image_view`;
- `.mex/context/active-memory.md` must record the evidence and next runtime decision.

## Result

Runtime 001 verified the cause is not merely image interpretation. The source and exporter assign catchments and ridge/gully masks from a shared warped polar-angle field around the massif center. Existing entropy/CV metrics allowed this to pass because angular sectors can be numerous and area-balanced while still visually reading as turbine/fan-blade terrain.

Generated evidence in `Images/TerrainPreview/004/` preserves the previous terrain shape but adds failing diagnostics:

- `catchment_angular_concentration_score: 0.98963`
- `ridge_gully_angular_lock_score: 0.97187`
- `morphology_diagnostics_accepted: false`
- `dem_calibration_accepted: false`

Coast/beach/ocean gates remain clean, so the failure is isolated to interior morphology.

## Next Runtime Trigger

`SELF-ITERATIVE/002_non_radial_catchment_graph_rewrite.md` is required. It must target the verified cause: catchment identity and ridge/gully structure must stop being owned by a polar-sector field around the massif center.
