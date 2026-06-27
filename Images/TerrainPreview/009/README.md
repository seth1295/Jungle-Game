# PR5 Batch 006 Terrain Calibration Preview Images

These preview image files are labelled, tracked terrain evidence generated from the deterministic PR5 Batch 006 terrain calibration model. They are not AI art, prompt renders, or UE screenshots.

Latest generated preview: `PR5_Batch006_TerrainCalibration_1024px`.

## Files

- `PR5_Batch006_TerrainCalibration_1024px_ColorRelief.png` — island color relief view with hillshade, coastline, beach, shelf, ridges, crater, and volcanic relief.
- `PR5_Batch006_TerrainCalibration_1024px_Height.png` — grayscale sampled height image.
- `PR5_Batch006_TerrainCalibration_1024px_Slope.png` — grayscale preview slope image.
- `PR5_Batch006_TerrainCalibration_1024px_MaskAtlas_BeachR_RidgeGullyG_HazardB.png` — RGB mask atlas: red = beach, green = ridge/gully, blue = hazard.
- `PR5_Batch006_TerrainCalibration_1024px_CatchmentId.png` — catchment identity preview for basin diversity review.
- `PR5_Batch006_TerrainCalibration_1024px_RadialDiagnostic.png` — radial-artifact diagnostic: red = ridge/gully alignment pressure, green = anti-artifact signal, blue = active volcano.
- `PR5_Batch006_TerrainCalibration_1024px_VolcanoDisabledHeight.png` — grayscale approximated volcano-disabled height evidence.
- `PR5_Batch006_TerrainCalibration_1024px_Manifest.json` — metrics and provenance for the generated preview.

## Regenerate

```bash
python3 scripts/terrain-preview-export.py --resolution 1024 --output-dir Images/TerrainPreview
```

Full 8129 x 8129 export at 12 m/sample remains a later heavyweight export path.
