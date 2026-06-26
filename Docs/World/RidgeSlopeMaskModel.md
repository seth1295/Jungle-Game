# Ridge Slope Mask Model

Created: 2026-06-25

## Purpose

Ridges, spurs, saddles, and slope breaks are core no-map movement structures. They help players orient through landform, skyline, sky windows, and traversal pressure.

## Inputs

- ridge influence;
- slope band;
- aspect exposure;
- canopy closure;
- sky-window likelihood;
- wetness;
- valley distance;
- local obstacle density.

## Output Bands

### Ridge Crest

High relative ridge line. Potential movement affordance and skyline cue.

### Spur Shoulder

Descending ridge shoulder. Potential route into or out of valleys.

### Saddle

Low point between ridge highs. Natural route choice and orientation point.

### Slope Break

Transition between slope bands. Useful for movement decisions and biome transition.

### Exposed Slope

More open, wind/light affected, potentially better visibility but higher fatigue.

### Sheltered Slope

Darker, wetter, denser, and more likely to degrade sun and sky cues.

### Sky Window

Canopy opening where sun, stars, clouds, ridge silhouettes, or mountain direction can be inferred.

## Rules

Do not fill every ridge with uniform foliage. Preserve some sightline windows. Make ridge travel useful but not universally safe.

Steep slopes should create fatigue, noise, time cost, and injury risk rather than constant sliding.

## Review

Review ridge crest, spur shoulder, saddle, slope break, steep exposed slope, sheltered slope, and sky-window ridge points.
