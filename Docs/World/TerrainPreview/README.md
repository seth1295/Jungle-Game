# PR5 Batch 003 Terrain Preview Images

These files are labelled, tracked preview evidence generated from the deterministic PR5 Batch 003 terrain model. They are not AI art, prompt renders, or UE screenshots.

Latest generated preview: `PR5_Batch003_Island_1024px`.

## Primary 1024 px Evidence Set

Open this set first. It is the labelled, useful island preview set generated after the shoreline acceptance fix.

- `PR5_Batch003_Island_1024px_ColorRelief.png` — primary island color relief view with hillshade, coastline, beach, shelf, ridges, crater, and volcanic relief.
- `PR5_Batch003_Island_1024px_Height.png` — grayscale sampled height image.
- `PR5_Batch003_Island_1024px_Slope.png` — grayscale preview slope image.
- `PR5_Batch003_Island_1024px_MaskAtlas_BeachR_RidgeGullyG_HazardB.png` — RGB mask atlas: red = beach, green = ridge/gully, blue = hazard.
- `PR5_Batch003_Island_1024px_Manifest.json` — metrics and provenance for the generated preview.

## Lightweight 512 px Evidence Set

This set is retained for quick inspection and smaller diffs. It is generated from the same deterministic terrain math.

- `PR5_Batch003_Island_512px_ColorRelief.png` — quick island color relief preview.
- `PR5_Batch003_Island_512px_Height.png` — quick grayscale sampled height image.
- `PR5_Batch003_Island_512px_Slope.png` — quick grayscale preview slope image.
- `PR5_Batch003_Island_512px_MaskAtlas_BeachR_RidgeGullyG_HazardB.png` — quick RGB mask atlas: red = beach, green = ridge/gully, blue = hazard.
- `PR5_Batch003_Island_512px_Manifest.json` — metrics and provenance for the lightweight preview.

## Regenerate

```bash
python3 scripts/terrain-preview-export.py --resolution 1024
```

Full 8129 x 8129 export remains a later heavyweight export path.
