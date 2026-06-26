# PCG Biome Output Contract

Created: 2026-06-25

## Purpose

Define the first named biome outputs that the PCG framework should produce from terrain-derived masks. The output should support readable jungle traversal, not decorative scatter.

## Output Families

### Creek Riparian

Dense wet-edge vegetation, bank clutter, crossing hints, water-sound anchors, and wet footing cues.

### Wet Valley

Low, damp, closed vegetation with heavier movement friction, muffled sound, and reduced sightlines.

### Ridge Crest

More legible movement line with selective sky windows, exposed roots/rock, and landmark visibility.

### Spur Shoulder

Transitional route surface between ridge and valley. Useful for route choice and descent/ascent decisions.

### Lowland Alluvial

Flat wet ground, thicker understory, regrowth gaps, and false clearings.

### Gorge Gully

Compressed corridor with stronger sound behavior, limited exits, water-cut obstacles, and route uncertainty.

### Coast Transition

Beach, rock, mangrove, cliff, headland, and coastal scrub transitions.

### Canopy Gap

Sky and light opening that supports sun, cloud, star, or mountain silhouette inference.

### Dense Friction Pocket

Natural soft-resistance zone. It should slow and redirect, not become random collision spam.

### Misleading Clearing

A route temptation that may naturally worsen, terminate, or redirect.

## Density Budget Rule

Each output needs a density budget and traversal effect category:

- visual only;
- sound cue;
- soft friction;
- hard blocker;
- affordance;
- false affordance.

## Determinism

Outputs should be deterministic for a given terrain/mask state. Avoid random scatter that changes route meaning between runs unless explicitly seeded and reviewable.

## Review

Each biome output must eventually have debug evidence and first-person screenshot evidence from at least one representative world review point.
