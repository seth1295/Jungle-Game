# PR5 Batch 004 X6 Terrain Preview Images

These files are labelled, tracked terrain evidence generated from the deterministic PR5 Batch 004 x6 anti-radial terrain model. They are not AI art, prompt renders, broad documentation, or UE screenshots.

Latest generated preview: `PR5_Batch004_X6_Island_1024px`.

## Files

- `PR5_Batch004_X6_Island_1024px_ColorRelief.png` — island color relief view with hillshade, coastline, beach, shelf, ridges, crater, and volcanic relief.
- `PR5_Batch004_X6_Island_1024px_Height.png` — grayscale sampled height image.
- `PR5_Batch004_X6_Island_1024px_Slope.png` — grayscale preview slope image.
- `PR5_Batch004_X6_Island_1024px_MaskAtlas_BeachR_RidgeGullyG_HazardB.png` — RGB mask atlas: red = beach, green = ridge/gully, blue = hazard.
- `PR5_Batch004_X6_Island_1024px_Manifest.json` — metrics and provenance for the generated preview.

## Regenerate

```bash
python3 scripts/terrain-preview-export.py --resolution 1024 --output-dir Images/TerrainPreview
```

Full 8129 x 8129 export at 12 m/sample remains a later heavyweight export path.
