# SELF-ITERATIVE 006 — Source / Exporter Terrain Parity

## Status

implemented-runtime

## Goal

Fix the first verified mismatch in the Batch006 terrain evidence path: the Python preview exporter includes `massif_height_m` in the main terrain process height, while the UE C++ terrain source calculates `MassifHeightM` but does not include it in `VolcanoDisabledProcessHeightM` / `TerrainProcessHeightM`.

This runtime exists to make future image evidence trustworthy. The preview image cannot be the acceptance source if the exporter and C++ runtime are not sampling the same terrain composition.

## Evidence From Inspection

Exporter path in `scripts/terrain-preview-export.py`:

```py
terrain_process_height_m = (
    basement_height_m
    + regional_landform_height_m
    + massif_height_m
    ...
)
```

C++ path in `Source/JungleGame/JungleVolcanicIslandTerrainModel.cpp` before this runtime:

```cpp
const float MassifHeightM = ...;
...
const float VolcanoDisabledProcessHeightM = BasementHeightM + RegionalLandformHeightM - CentralSaddleCutM + ...;
const float TerrainProcessHeightM = VolcanoDisabledProcessHeightM + ActiveVolcanoContributionM;
```

`MassifHeightM` was stored in `Sample.MassifHeightM`, but it was not participating in final terrain height.

## Suspected Cause

The C++ source and exporter diverged during the self-iterative morphology loop. The exporter retained the broad massif/base shield contribution as part of the preview terrain, while the C++ terrain source no longer includes that contribution in final height composition.

## Planned Scope

Update:

- `Source/JungleGame/JungleVolcanicIslandTerrainModel.cpp`

Targets:

1. include `MassifHeightM` in the C++ `VolcanoDisabledProcessHeightM` composition at the same position as exporter `massif_height_m`;
2. keep coastal hard-lock, beach acceptance, ocean constraints, and existing active-volcano contribution behavior unchanged;
3. run diff checks and C++ build if available;
4. run the preview exporter as inspection evidence only, without committing intermediate image output;
5. do not overwrite or stage existing `Images/TerrainPreview/001` through `008`;
6. reserve tracked final accepted evidence for `Images/TerrainPreview/009/` in the later evidence-export runtime.

## Acceptance Target

This runtime is worth landing if:

- C++ source composition matches the exporter massif/base height path;
- `git diff --check` passes;
- Python exporter syntax passes;
- preview exporter can still generate clean temporary output;
- UBT either passes or a real environment blocker is recorded;
- no tracked terrain evidence folders are overwritten.

## Result

Runtime 006 fixed the verified source/exporter mismatch by adding `MassifHeightM` to the C++ `VolcanoDisabledProcessHeightM` terrain composition, matching exporter `massif_height_m` participation in `terrain_process_height_m`.

Validation:

- `git diff --check` passed.
- `python3 -m py_compile scripts/terrain-preview-export.py` passed.
- Temporary 256 px exporter generation to `/tmp/jungle-si-006-preview` passed.
- Temporary manifest preserved hard coast gates: `shoreline_error_max_m: 0.0`, `beach_continuity_pct: 100.0`, `ocean_below_sea_pct: 100.0`, and `square_edge_ocean_violations: 0`.
- As expected, morphology remained not accepted: `catchment_angular_concentration_score: 0.95997`, `ridge_gully_angular_lock_score: 0.08923`, `morphology_diagnostics_accepted: false`, and `dem_calibration_accepted: false`.
- UBT was environment-blocked in this shell because `/run/media/seth/UE5_WORKSPACE/UnrealEngine/Engine/Build/BatchFiles/Linux/Build.sh` was not present and path discovery under `/run/media/seth` and `/mnt` found no `Build.sh`.

No tracked `Images/TerrainPreview/001` through `008` evidence was overwritten. Runtime 007 should proceed to soft catchment/domain blending now that the preview evidence path and C++ terrain source are back in parity.
