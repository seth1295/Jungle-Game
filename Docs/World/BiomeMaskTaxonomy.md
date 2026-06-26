# Biome Mask Taxonomy

Created: 2026-06-25
Track: world-first terrain/biome implementation

## Purpose

Define the named mask families that drive biome placement, traversal friction, and environmental navigation. Masks are developer/runtime data, not player UI.

## Core Mask Families

### Elevation Band

Separates coast, lowland, foothill, upland, ridge, and mountain shoulder zones.

### Slope Band

Separates flat, gentle, moderate, steep, and extreme ground. Used for traversal friction, injury risk, vegetation density, and route pressure.

### Aspect Exposure

Separates exposed and sheltered faces. Used for dry/wet variation, wind exposure, sun cue readability, and vegetation structure.

### Wetness

Combines valley index, creek distance, flow, basin position, and canopy. Used for riparian density, mud risk, insects, water sound, and lowland heaviness.

### Ridge Influence

Highlights ridges, spurs, saddles, and slope breaks. Used for movement affordances, skyline cues, sky windows, and landmark readability.

### Valley Influence

Highlights gullies, basins, creek corridors, and gorge compression. Used for water systems, sound behavior, visibility reduction, and route traps.

### Coast Influence

Highlights beach, rocky shore, cliff coast, headland, creek mouth, and mangrove transition zones. Used for island boundary readability and coastal route logic.

### Canopy Closure

Estimates overhead cover. Used for sky window availability, sun/shadow cue degradation, rain behavior, night readability, and visual capture expectations.

### Disturbance Gap

Estimates regrowth gaps, treefall openings, washouts, scars, and misleading clearings. Used for false affordances and temporary orientation cues.

### Traversal Resistance

Composite mask that converts terrain and biome conditions into movement friction. It must distinguish useful resistance from noisy collision clutter.

## Biome Outputs

Initial biome outputs should include:

- creek/riparian;
- wet valley;
- ridge crest;
- slope shoulder;
- lowland alluvial;
- gorge/gully;
- coast transition;
- canopy gap;
- dense friction pocket;
- misleading clearing.

## Validation

Each mask family needs representative review points and later debug visualization. Debug views must remain developer-only.
