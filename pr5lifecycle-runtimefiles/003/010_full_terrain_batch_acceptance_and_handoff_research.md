# Full Terrain Batch Acceptance And Handoff Research

## Executive Summary

PR5 Batch 003 should be accepted only when Jungle Game has a reproducible, testable, terrain-only foundation for a realistic active volcanic island inside a 16.256 km square world container. The batch is not successful merely because a heightmap exists, screenshots look better, or a single island mesh can be opened in Unreal. It is successful when the generated terrain can be validated numerically, inspected in developer-only views, exported as auditable topographic evidence, and handed off as stable input for later traversal and environment work.

The required terrain result is an organic circular island with a continuous sea-level coastal edge, ocean outside the island mask, a volcanic massif near 1400 m above sea level, believable ridge/gully/catchment structure, crater and lava-crust hazard terrain masks, sane resolution/LOD behavior, and generated analysis maps. The acceptance standard should combine game-specific targets with geospatial quality practice: explicit height range, raster dimensions, channel masks, catchment outputs, slope histograms, flow-accumulation maps, reproducible seeds, and source-control hygiene. Unreal’s Landscape system uses 16-bit height values and Z scale conversion, and Epic’s recommended landscape sizes include 8129 x 8129 and 4033 x 4033 vertex grids; those facts matter because the batch must not accidentally ship an arbitrary terrain resolution that cannot round-trip cleanly through the UE5 pipeline. [^ue-landscape]

The batch should remain terrain-only. No foliage, ecology, survival design, NPCs, story, weather, audio, VFX polish, final materials, or player-facing map/orientation UI should be accepted as terrain progress. Later systems may resume only after the batch handoff declares the terrain contract stable: coordinate frame, sea level, height range, masks/channels, collision assumptions, developer maps, exports, known limitations, and forbidden downstream assumptions.

**Batch-level acceptance is therefore:**

- A deterministic terrain build can regenerate the same island, masks, metadata, and validation outputs from recorded inputs.
- The island is geographically coherent inside the 16.256 km square container while remaining organic/circular rather than square.
- Sea level, coastline, beach/coastal shelf, massif elevation, ridges, gullies, crater, lava/crust terrain hazards, and ocean mask all pass numeric thresholds.
- Runtime UE5 evidence proves the terrain can be loaded, streamed, collision-tested, and debug-inspected at the intended scale.
- A terrain-only handoff document exists and explicitly gates later traversal/environment systems.

## Terrain-Only Scope Boundary

Batch 003 is a terrain-generation lifecycle batch. Its responsibility is to create and prove the topographic substrate. It should not solve how the island looks as a final shipped biome, how the player navigates with UI, or how later systems populate the world.

**Allowed scope:**

- Heightfield / landscape / runtime mesh generation.
- Terrain import/export path for UE5 Linux-first development.
- Island/ocean/coast/beach/shelf masks.
- Massif, ridge, gully, catchment, crater, lava/crust hazard terrain geometry and masks.
- Developer-only topographic overlays.
- Terrain validation metrics and evidence exports.
- Terrain coordinate conventions, scale, Z range, LOD, collision, streaming, and source-control hygiene.
- Handoff contract for later traversal/environment systems.

**Forbidden scope:**

- Foliage, ecology, jungle density, biomes, animal behavior, NPCs, story, quests, survival systems, final materials, asset packs, weather, audio, cinematics, VFX polish, water gameplay, map/minimap/compass/GPS/bearing/objective UI, or player-facing orientation tools.

**Boundary rule:** if a proposed PR cannot be evaluated with terrain geometry, masks, UE5 runtime terrain evidence, or generated topographic analysis, it is outside Batch 003.

## Batch 003 Terrain Goal

The goal is to establish a stable, extensible volcanic-island terrain foundation before non-terrain systems resume.

**World container:**

- Square world container: **16.256 km x 16.256 km**.
- Terrain coordinate convention: define origin, X/Y axes, Z-up scale, sea level, and island center in generated metadata.
- Actual island shape: **organic circular island**, not a square or heightmap-filling blob.
- Outside island mask: ocean / below sea level.
- Outer island edge: continuous sea-level beach/coastal shelf.
- Main landform: active volcanic massif with peak target around **1400 m above sea level**.

**Required batch output:** a terrain package that later systems can trust. That package must include a primary terrain representation, masks, maps, metrics, and a handoff file. It should be possible to review the batch without launching the full game by inspecting the exported rasters/metadata and validation report.

**Core acceptance principle:** Batch 003 should reject “art terrain” that only looks plausible from selected camera angles. Terrain acceptance must be numeric, repeatable, and reviewable.

## Required Major Implementation Outcomes

### 1. Deterministic generation pipeline

The batch must provide a deterministic terrain generation pipeline that records all inputs needed to reproduce the terrain:

- generator version / commit SHA;
- seed(s);
- world dimensions;
- raster dimensions and meters-per-pixel;
- Z scale and vertical range;
- sea-level value;
- island radius / falloff parameters;
- massif, crater, ridge, gully, and hazard parameters;
- mask/channel names and encoding;
- output file manifest and checksums.

**Pass:** a clean checkout can regenerate the same terrain outputs within stated numeric tolerances.

**Fail:** output exists only as manually edited editor state, untracked binary state, or undocumented assets.

### 2. Primary terrain representation

At least one primary terrain representation must exist and be accepted:

- UE5 Landscape heightfield, runtime mesh, or hybrid representation;
- importable/exportable height data;
- matching collision representation;
- matching developer validation maps.

For UE Landscape, the batch must choose dimensions and Z scale intentionally. Epic documents that Unreal calculates landscape height from a 16-bit precision range multiplied by import Z scale, and lists recommended sizes such as 8129 x 8129 and 4033 x 4033 vertices to maximize area while minimizing component count. [^ue-landscape]

**Recommended Batch 003 decision:** use **8129 x 8129** as the terrain acceptance reference grid for a full 16.256 km container if the project can tolerate roughly **2.0 m vertex spacing**. If runtime cost or tooling demands lower resolution, use a deliberate tiled/multi-resolution strategy and prove it with LOD/runtime evidence rather than silently reducing fidelity.

### 3. Terrain masks and channel atlas

The batch must output an explicit mask/channel atlas with stable names and value conventions. Required masks:

- `island_mask` — land/island inclusion.
- `ocean_mask` — outside island / below sea level.
- `beach_ring_mask` — continuous beach/coastal shelf band.
- `coastal_shelf_mask` — shallow nearshore terrain, if separated from beach.
- `massif_mask` — volcanic edifice/massif domain.
- `ridge_mask` — ridgelines/spurs.
- `gully_mask` — incised valleys/gullies.
- `catchment_id_map` — drainage/catchment partition.
- `flow_accumulation_map` — hydrological accumulation or equivalent terrain-flow proxy.
- `slope_map` — slope in degrees or percent grade.
- `crater_mask` — summit crater / caldera rim / crater floor domain.
- `lava_crust_hazard_mask` — hard-blocker or movement-risk terrain crust.
- `steep_blocker_mask` — terrain too steep for baseline traversal assumptions.
- `void_invalid_mask` — invalid/non-sampled/no-data cells, expected to be empty inside required play terrain.

### 4. Developer-only terrain maps

The batch must generate maps for validation and debugging, not for player UI:

- height tint map;
- hillshade map;
- slope heat map;
- aspect map;
- beach continuity map;
- sea-level error map;
- ridge/gully overlay;
- catchment ID map;
- flow accumulation / channel-initiation map;
- crater/hazard overlay;
- blocker mask overlay;
- LOD/collision debug screenshots or captures.

QGIS training material describes catchment area as flow accumulation and uses it as a threshold input for channel-network generation; it also notes that changing the initiation threshold changes channel density. Batch 003 should use that same principle: channel density must be an explicit parameter and validation output, not an accidental artifact. [^qgis-hydro]

### 5. Handoff package

The final batch PR must include a terrain-only handoff document with:

- accepted generator version;
- terrain dimensions and scale;
- sea level;
- height range;
- mask/channel table;
- validation thresholds and measured results;
- generated evidence manifest;
- runtime evidence links/screenshots;
- known limitations;
- later-system resume gate;
- explicit “not included” list to prevent downstream drift.

## Sea-Level Beach Ring Acceptance Criteria

The coastline is a hard terrain contract. The island may be organic, but the outer coastal band must be continuous, believable, and numerically stable.

### Required properties

- The island edge is organic/circular, not square or clipped to the world container.
- The transition from island to ocean is continuous around the island perimeter.
- The beach ring reaches sea level consistently and does not float above or sink far below target.
- The coastal shelf outside the beach is below sea level and slopes outward into ocean.
- No inland ocean leaks appear inside the island mask unless intentionally defined as crater lake or inland water mask; Batch 003 should not add such features unless explicitly scoped as terrain-only.
- The beach band has enough width for later traversal/environment placement but does not become a uniform perfect ring.

### Recommended numeric targets

- **Sea level:** `0.0 m` in generated terrain metadata.
- **Island boundary elevation:** pass if 99.0% of coastline samples are within **±1.0 m** of sea level; warn at ±2.0 m; fail beyond ±3.0 m or if errors form visible discontinuities.
- **Beach band width:** target **40–160 m** local width, with organic variation; pass if 95% of perimeter has at least **25 m** continuous traversable coastal band.
- **Coastal shelf:** target at least **80–250 m** shallow shelf outside beach before deeper ocean falloff, if the container allows it.
- **Coastline continuity:** pass if one dominant island polygon contains at least **98%** of land area and there are no unintended detached land slivers > **0.02 km²**.
- **Ocean outside island:** pass if 99.5% of samples outside `island_mask` are below sea level.
- **Beach slope:** median beach slope **2–8°**; warn if median > **10°**; fail if continuous beach segments require > **15°** slope unless marked non-traversable cliff exceptions.

### Required evidence

- Coastline sample CSV: angle/radius/elevation/error/beach width.
- Sea-level error heat map.
- Beach ring continuity image.
- Ocean mask image.
- At least 8 runtime screenshots or captures around the coastline, evenly distributed by azimuth.

### Fail cases

- Square island boundary.
- Terrain fills the square world as land.
- Beach exists only on one side.
- Beach ring is visually present but not represented in masks.
- Ocean mask disagrees with heightfield sea level.
- Later systems would need to guess coastline width or sea-level behavior.

## 1400 m Volcanic Massif Acceptance Criteria

The central or near-central active volcanic massif is the dominant terrain feature. It should read as a volcanic edifice rather than a generic noise mountain.

USGS defines a volcanic edifice as the main portion of a volcano built by lava, tephra, pyroclastic flows, lahars, and related deposits; this supports the acceptance requirement that the massif be structured by volcanic landform logic rather than arbitrary high-frequency noise. [^usgs-glossary]

### Required properties

- Peak elevation is approximately **1400 m above sea level**.
- Massif has coherent radial/flank structure, not a single cone pasted onto unrelated terrain.
- Summit crater or crater complex exists and is geomorphically connected to the massif.
- Flanks include ridge/gully/catchment structure that descends toward the coast.
- Massif does not consume the entire island; there is readable coastal and lower-slope terrain.
- Peak and crater are not clipped by Z range, heightmap precision, landscape import scale, or LOD simplification.

### Recommended numeric targets

- **Maximum elevation target:** pass at **1350–1450 m**; warn at **1300–1500 m**; fail outside **1250–1550 m** unless the design target is formally changed.
- **Massif footprint:** target **18–40%** of land area above 300 m or inside `massif_mask`; warn if < 12% or > 50%.
- **Hypsometry:** land elevation distribution should include low coastal land, mid slopes, and high massif; fail if the island is mostly flat with a spike, or mostly mountain with no coherent coast.
- **Summit/crater relation:** crater rim elevation should sit within the high massif domain; crater floor lower than rim by **40–250 m** unless intentionally shallow.
- **Radial relief:** at least **8 major downhill sectors** from massif toward coast should show coherent descent paths or catchments.
- **Vertical quantization:** no visible terracing from height encoding; inspect both source raster and imported UE terrain.

### Required evidence

- Elevation histogram.
- Hypsometric curve.
- Max/min/mean/median elevation summary.
- Summit location and elevation marker.
- Massif mask overlay.
- Crater rim/floor profile.
- Four radial cross-sections through the massif.

### Fail cases

- A generic noise peak with no crater or catchments.
- A correct max elevation achieved by one spike or artifact.
- A crater mask exists but the heightfield has no crater depression/rim.
- The massif is only visual material masking, not terrain geometry.
- Z scale makes the mountain technically present but collision/LOD destroys it at runtime.

## Ridge / Gully / Catchment Acceptance Criteria

Ridges, gullies, and catchments are required terrain structure, not decoration. They provide the terrain grammar later traversal and environment systems will depend on.

Geomorphometric workflows commonly derive terrain attributes such as slope, aspect, curvature, relative position, and roughness from gridded terrain data; Batch 003 should output the equivalent analysis maps so reviewers can see whether ridges and gullies are real terrain features rather than visual overlays. [^geomorphometry]

### Required properties

- Ridges and gullies emerge from the massif and secondary terrain, then descend toward lower land or coast.
- Catchments partition the island into coherent drainage basins.
- Flow accumulation/channel maps align with visible gullies.
- Ridges and gullies are represented in masks/channels.
- Gully incision is terrain geometry, not only material color.
- Ridge/gully masks do not fragment into noise speckles.
- Catchments do not route water uphill or across obvious divides.

### Recommended numeric targets

- **Major catchments:** target **12–36** island-scale catchments depending on island radius and massif placement.
- **Catchment coverage:** pass if **95%+** of land cells belong to exactly one catchment or an explicit excluded class.
- **Main drainage/gully length density:** target **0.6–2.5 km of major channel per km² of land**, tuned by island steepness; warn outside range; fail if near-zero or dense spaghetti network.
- **Ridge/gully separation:** ridge masks and gully masks should overlap less than **3%** by area except at saddles/junction tolerance.
- **Flow correctness:** at least **95%** of sampled gully cells should descend downstream monotonically within a small local tolerance; fail if channels visibly climb or terminate randomly.
- **Noise control:** connected components smaller than **100 m²** should be removed or classified as microdetail, not major ridge/gully mask.

### Required evidence

- Catchment ID raster/map.
- Flow accumulation raster/map.
- Channel-initiation threshold value and rationale.
- Ridge/gully overlay on hillshade.
- Longitudinal profiles for at least 6 major gullies.
- Watershed summary table: ID, area, max elevation, outlet elevation, main-channel length, relief ratio.

### Fail cases

- Gullies are merely texture/material lines.
- Ridges are random noise artifacts.
- Catchment map is absent.
- Flow accumulation is computed but not used for acceptance.
- Gully/ridge masks are unstable across regeneration with the same seed.

## Crater / Lava-Crust Terrain Hazard Acceptance Criteria

The crater and lava/crust hazard areas must be terrain features and masks. They should not become gameplay systems in this batch.

USGS lava-flow guidance describes two terrain-relevant ideas that matter for Batch 003: lava paths can be approximated from past pathways and steepest-descent paths on DEM topography, and lava moves downslope through low areas analogous to water flow through gullies and valleys. [^usgs-lava] Batch 003 should therefore validate lava/crust hazard masks against topographic descent and terrain context, not arbitrary paint.

### Required crater properties

- Crater or caldera feature exists as heightfield geometry.
- Crater mask, rim mask if used, and hazard mask are distinct or explicitly encoded.
- Crater connects logically to high volcanic massif.
- Rim/floor are inspectable in cross-section.
- Crater does not create unintentional no-data holes, collision voids, or LOD collapse.

### Required lava/crust hazard properties

- Hazard terrain masks represent physically plausible crust/lava-flow domains or unstable terrain domains.
- Hazard masks are terrain-only blockers/risks for later traversal systems, not final gameplay damage logic.
- Hazard masks align with slope/descent/ridges/gullies where appropriate.
- Hard-blocker masks are explicit and conservative.
- Hazards do not silently cover most of the island.

### Recommended numeric targets

- **Crater diameter:** target **250–900 m** depending on chosen volcano style; warn outside range; fail if crater is too tiny to matter or consumes the massif.
- **Crater depth below rim:** target **40–250 m**; warn below 20 m or above 350 m.
- **Crater rim continuity:** pass if **90%+** of rim path is detectable within rim mask/elevation gradient.
- **Hazard mask area:** target **2–12%** of land area for hard lava/crust hazard; warn at 12–20%; fail above 25% unless scope changes.
- **Hard blocker correctness:** pass if all hard-blocker cells are represented in both mask exports and runtime debug views.
- **Topographic plausibility:** lava/crust hazard corridors should preferentially occupy steepest descent / low-path sectors; fail if randomly distributed without terrain rationale.

### Required evidence

- Crater plan-view overlay.
- Crater rim/floor elevation profile.
- Hazard mask raster.
- Hard-blocker overlay.
- Steepest descent / flow proxy overlay for lava-prone corridors.
- Runtime collision/debug visualization showing hazard/blocker regions.

### Fail cases

- Crater exists only as a texture.
- Hazard masks are gameplay-only flags with no terrain evidence.
- Hazard mask overlaps beach/coast in a way that blocks the whole island perimeter without explicit design approval.
- Hard blockers are not exported or not visible in debug maps.

## Runtime Mesh / Resolution Acceptance Criteria

Runtime acceptance must prove the terrain is usable in UE5, not just correct in offline rasters. World Partition is relevant because Epic describes it as a system that stores the world in a persistent level subdivided into streamable grid cells, loading and unloading cells based on streaming sources. [^ue-worldpartition]

### Required runtime properties

- Terrain loads in the target UE5 Linux-first workflow.
- World scale matches metadata: 16.256 km square container, correct Z scale, sea level at expected world Z.
- Collision matches visual terrain closely enough for later traversal prototyping.
- LOD does not erase coast, crater, ridges, gullies, or hazard masks at acceptance viewing distances.
- World Partition or equivalent streaming strategy is configured intentionally for the terrain scale.
- Developer-only debug views can show masks/channels in-engine.
- No source-control-only asset dependency is missing from a clean checkout.

### Recommended resolution targets

For a 16.256 km terrain container:

| Candidate grid | Approx. spacing | Batch 003 interpretation |
|---:|---:|---|
| 8129 x 8129 | ~2.0 m | Preferred full-container acceptance reference if performance/tooling allow. |
| 4033 x 4033 | ~4.0 m | Acceptable for early runtime proof only if ridges/gullies/crater are still validated and masks are higher-fidelity or tiled. |
| < 4033 x 4033 | >4.0 m | Warning/fail unless this is a proxy mesh plus separate high-resolution source data. |
| Tiled/multi-res | variable | Acceptable only with explicit tile manifest, seam checks, and LOD evidence. |

### Required evidence

- UE5 import/build settings screenshot or text export.
- Landscape/runtime mesh dimensions.
- Z scale calculation and sea-level world Z.
- Collision validation captures on beach, massif, crater, gully, ridge, and hazard areas.
- LOD screenshots at near/mid/far distances.
- World Partition/grid/cell evidence if used.
- Performance sanity check: terrain loads without runaway memory, shader compile dependency, or editor-only state.

### Pass/warn/fail guidance

- **Pass:** terrain can be regenerated, imported/loaded, debug-inspected, collision-tested, and streamed at target scale.
- **Warn:** terrain is correct offline but runtime import/LOD/collision has open issues with documented fixes in the batch.
- **Fail:** terrain only exists offline, only opens on one machine, cannot be regenerated, or loses key landforms in runtime representation.

## Terrain Masks And Channel Acceptance Criteria

Masks are part of the terrain contract. Later systems should be able to consume them without reverse-engineering the heightfield.

GDAL’s GeoTIFF driver supports georeferencing through tiepoints, pixel size, transformation matrices, or ground control points; when exporting terrain rasters, Batch 003 should preserve enough spatial metadata for maps to align cleanly with the heightfield and with each other. [^gdal-gtiff]

### Required mask/channel rules

- Every mask has a stable name, type, resolution, coordinate transform, value range, and ownership.
- Masks align pixel-for-pixel with the accepted heightfield or include explicit resampling metadata.
- Binary masks use `0/1` or `0/255` consistently.
- Continuous maps specify units: degrees, meters, normalized weight, ID value, or no-data.
- Overlap rules are documented.
- Hard blockers are conservative and auditable.
- Channel packing, if used, has a clear atlas table.

### Required mask manifest fields

For every mask/map:

```text
name:
purpose:
file:
resolution:
coordinate frame:
units:
value range:
no-data value:
derived from:
consumer contract:
overlap rules:
source-control policy:
```

### Required source-control hygiene

- Commit small metadata files and validation reports.
- Commit source parameters, deterministic generator code, and lightweight preview maps.
- Avoid committing massive derived binaries unless they are required for the project build or explicitly accepted by repo policy.
- If large terrain binaries are required, store them through the chosen large-file mechanism and include checksums in the manifest.
- Generated evidence should be reproducible; do not treat manually exported screenshots as the only source of truth.
- Separate source terrain data from generated cache/intermediate files.

### Fail cases

- Masks exist only as material layers with no export.
- Same mask name means different things across PRs.
- Heightfield and masks are off by one pixel, rotated, mirrored, or scaled without detection.
- Hard-blocker data is embedded in undocumented material graph logic.
- Later traversal/environment code would need to infer hazards from color, texture, or screenshot evidence.

## Topographic Validation Evidence Requirements

The validation package should make terrain review boring: reviewers should be able to see the target, the threshold, the measured result, and the evidence path.

USGS topographic data quality levels define DEM quality using both point-density/spacing and vertical positional accuracy, with example DEM cell sizes from 0.5 m to 5 m depending on quality level. Although Jungle Game terrain is synthetic rather than surveyed, Batch 003 should borrow the discipline: resolution and vertical error/tolerance must be stated explicitly. [^usgs-3dep]

### Required generated files

At final acceptance, generate an evidence directory similar to:

```text
TerrainEvidence/Batch003/
  manifest.json
  terrain_metrics.json
  terrain_metrics.md
  heightfield_preview.png
  hillshade.png
  slope_degrees.png
  aspect.png
  sea_level_error.png
  island_mask.png
  ocean_mask.png
  beach_ring_mask.png
  beach_width_samples.csv
  massif_mask.png
  elevation_histogram.png
  hypsometric_curve.png
  ridge_gully_overlay.png
  catchment_id_map.png
  flow_accumulation_log.png
  crater_hazard_overlay.png
  blocker_mask.png
  radial_profiles/
  runtime_captures/
  checksums.sha256
```

### Required metrics

- World size and raster dimensions.
- Meters per pixel / vertex spacing.
- Min, max, mean, median elevation.
- Sea-level boundary error summary.
- Land area, ocean area, island perimeter.
- Beach continuity and width statistics.
- Slope distribution by percentiles: p50, p75, p90, p95, p99.
- Area by slope class.
- Massif footprint and peak elevation.
- Crater rim/floor metrics.
- Catchment count, area distribution, outlet correctness.
- Main-channel/gully length density.
- Ridge/gully mask area and overlap.
- Hazard/blocker area and overlap with beach/coast.
- Invalid/no-data count inside land.
- Tile seam error if tiled.
- Regeneration checksum comparison.

### Required developer-only in-engine evidence

- Full-island overhead developer capture.
- Coastline captures at N/E/S/W and diagonals.
- Massif summit capture.
- Crater capture.
- Ridge/gully capture.
- Hazard/blocker overlay capture.
- LOD near/mid/far capture.
- Collision walk/trace evidence on representative terrain classes.

### Evidence acceptance rule

Evidence that cannot be regenerated is weak evidence. Evidence that cannot be mapped back to a specific generator seed/version is not sufficient for final Batch 003 acceptance.

## Numeric Pass / Warning / Fail Thresholds

The following thresholds are project acceptance targets. They are not claims about real-world volcanic islands; they are synthetic terrain QA gates designed to prevent terrain drift and incomplete implementation.

| Category | Pass | Warning | Fail |
|---|---:|---:|---:|
| World container size | 16.256 km x 16.256 km ±0.1% | ±0.5% | >±0.5% or undocumented |
| Island shape | Organic/circular, one dominant landmass | Minor slivers or too symmetrical | Square/clipped/fills container |
| Outside-island ocean | 99.5% below sea level | 98–99.5% | <98% or large land leaks |
| Coastline sea-level error | 99% within ±1 m | 99% within ±2 m | discontinuities or >±3 m |
| Beach continuity | 95% perimeter has ≥25 m band | 85–95% | <85% or broken ring |
| Beach width | 40–160 m target | 25–250 m | absent or extreme uniform strip |
| Median beach slope | 2–8° | 8–10° | >10° median or >15° required path |
| Peak elevation | 1350–1450 m | 1300–1500 m | <1250 or >1550 m |
| Massif footprint | 18–40% land domain | 12–18% or 40–50% | <12% or >50% |
| Crater diameter | 250–900 m | 150–250 or 900–1200 m | absent, tiny, or island-scale |
| Crater depth | 40–250 m | 20–40 or 250–350 m | <20 or >350 m unless approved |
| Major catchments | 12–36 | 8–12 or 36–48 | <8 or >48 without rationale |
| Catchment coverage | ≥95% land assigned | 90–95% | <90% or ambiguous IDs |
| Gully/channel density | 0.6–2.5 km/km² | 0.3–0.6 or 2.5–3.5 | <0.3 or >3.5 |
| Ridge/gully mask overlap | <3% | 3–6% | >6% or masks meaningless |
| Hazard hard-blocker area | 2–12% land | 12–20% | >25% or absent when required |
| Hazard/coast conflict | <5% beach blocked | 5–10% | >10% or blocks island loop |
| Invalid land cells | 0 | ≤0.01% documented | >0.01% or collision holes |
| Tile seam height error | ≤0.25 m p99 | 0.25–1 m | >1 m visible seams |
| Runtime terrain scale | Matches metadata | Minor editor-only mismatch | scale/Z/sea level incorrect |
| Regeneration | Checksums match or tolerances pass | Noncritical derived diff | non-deterministic core outputs |

## Required Handoff Contents

The final handoff should be a short, explicit contract. It should not be a narrative devlog.

### Terrain-only handoff template

```markdown
# PR5 Batch 003 Terrain Handoff

## Accepted Terrain Version
- Generator commit:
- Terrain asset commit:
- Seed(s):
- Date accepted:
- Accepted by:

## Terrain Scale Contract
- World container:
- Terrain representation:
- Heightfield/grid dimensions:
- Horizontal spacing:
- Vertical range:
- Sea level:
- UE Z scale/import settings:
- Origin and axes:

## Required Outputs Present
- Heightfield/mesh:
- Island mask:
- Ocean mask:
- Beach ring mask:
- Coastal shelf mask:
- Massif mask:
- Ridge mask:
- Gully mask:
- Catchment ID map:
- Flow accumulation map:
- Slope map:
- Crater mask:
- Lava/crust hazard mask:
- Hard-blocker mask:
- Topographic exports:

## Acceptance Metrics Summary
| Metric | Target | Measured | Status |
|---|---:|---:|---|
| Peak elevation | 1350–1450 m |  |  |
| Coastline sea-level error | 99% ±1 m |  |  |
| Beach continuity | ≥95% |  |  |
| Catchment coverage | ≥95% |  |  |
| Hazard hard-blocker area | 2–12% |  |  |
| Invalid land cells | 0 |  |  |

## Runtime Evidence
- UE version:
- Linux validation environment:
- World Partition/streaming settings:
- Collision evidence:
- LOD evidence:
- Developer overlay evidence:

## Known Terrain Limitations
- Limitation:
- Impact:
- Owner:
- Must fix before:

## Later-System Resume Gate
- Traversal may resume: yes/no
- Environment placement may resume: yes/no
- Systems must consume masks by name, not infer from screenshots/materials.
- Systems must not assume final foliage, weather, audio, survival, NPC, story, or player UI decisions.

## Explicitly Not Included
- Foliage/ecology:
- Weather:
- Audio:
- Survival gameplay:
- NPC/story:
- Final materials/VFX:
- Player-facing map/orientation UI:
```

### Required final acceptance checklist

- [ ] Clean checkout can regenerate core terrain outputs.
- [ ] Terrain manifest exists and includes seed, dimensions, scale, sea level, Z scale, and checksums.
- [ ] UE5 runtime terrain loads on the Linux-first dev target.
- [ ] Heightfield/mesh dimensions are intentional and documented.
- [ ] Organic island mask fits inside 16.256 km square container.
- [ ] Ocean outside island is below sea level.
- [ ] Sea-level beach/coastal shelf ring passes continuity and elevation thresholds.
- [ ] Massif peak is near 1400 m and validated by histogram/profile evidence.
- [ ] Crater exists as terrain geometry and mask.
- [ ] Ridge/gully/catchment maps exist and align with terrain.
- [ ] Lava/crust hazard and hard-blocker masks exist and are visible in developer maps.
- [ ] Slope/catchment/topographic exports exist.
- [ ] Runtime collision and LOD evidence exists.
- [ ] Source-control policy for generated artifacts is documented.
- [ ] Handoff explicitly blocks non-terrain drift and enables later-system resume only through stable terrain contracts.

## Later-System Resume Gate

Traversal, environment, foliage, materials, water, weather, audio, and gameplay systems should resume only when the terrain handoff is accepted.

### Resume allowed when

- Terrain geometry is stable enough that later systems will not be invalidated by large-scale island/massif/coast changes.
- Masks have stable names and value conventions.
- Beach/coast/massif/crater/hazard/catchment contracts are documented.
- Runtime scale/collision/LOD evidence passes.
- Known limitations do not affect the downstream system being resumed.
- Any downstream system has a clear rule: consume terrain masks and metrics, do not invent terrain interpretation from visual screenshots.

### Resume not allowed when

- Coastline, sea level, or world scale is still changing.
- Heightfield resolution or UE representation is undecided.
- Hazard/blocker masks are missing.
- Catchments/ridges/gullies are still placeholders.
- Runtime terrain import is not validated.
- The batch has added unrelated foliage/ecology/weather/gameplay work instead of closing terrain blockers.

## Large PR Sequencing Recommendations

Batch 003 may require 5, 10, or more implementation PRs. The number should be determined by terrain dependency boundaries and validation gates, not by an arbitrary target count.

### Recommended PR breakdown

1. **Terrain contract and generator manifest PR**
   - Define world scale, coordinate frame, sea level, seed metadata, output manifest, file naming, and evidence directory structure.

2. **Base island/ocean/coast PR**
   - Generate organic island mask, ocean mask, coastal shelf, beach ring, sea-level validation, and beach continuity evidence.

3. **Massif/elevation PR**
   - Add 1400 m volcanic massif, vertical range/Z scale validation, elevation histograms, hypsometric curve, and radial profiles.

4. **Crater/hazard terrain PR**
   - Add crater geometry, crater masks, lava/crust hazard masks, hard-blocker masks, and descent-path plausibility evidence.

5. **Ridge/gully/catchment PR**
   - Add ridge/gully generation, flow accumulation, catchment ID map, channel density controls, and watershed metrics.

6. **UE5 runtime import/mesh PR**
   - Import/load terrain in UE5, configure Landscape/runtime mesh, validate scale/collision/LOD/World Partition behavior.

7. **Developer debug maps PR**
   - Add in-engine overlays and generated preview maps for height/slope/catchment/hazard/coastline evidence.

8. **Topographic validation automation PR**
   - Add scripts/tests that fail CI or local validation on numeric threshold violations.

9. **Source-control hygiene/export PR**
   - Separate source parameters, generated outputs, cache files, large binaries, checksums, and evidence artifacts.

10. **Final handoff/acceptance PR**
   - Freeze terrain contract, summarize metrics, document known limitations, and explicitly reopen later-system work.

### When 5 PRs is enough

Five PRs may be enough if:

- the project already has terrain generator infrastructure;
- UE5 import/runtime path already exists;
- masks and validation automation are straightforward;
- each PR remains reviewable and passes evidence gates;
- no major refactor or representation decision remains.

A compressed 5-PR structure:

1. Contract + base island/coast.
2. Massif + crater/hazards.
3. Ridges/gullies/catchments.
4. Runtime UE5 + debug overlays.
5. Validation automation + final handoff.

### When 10 PRs is right

Ten PRs is right if:

- base terrain, masks, runtime import, and validation are all still immature;
- there are multiple binary/export formats;
- source-control policy for generated terrain is unsettled;
- world partition/collision/LOD needs focused testing;
- reviewers need clean evidence for each major terrain domain.

### When more than 10 PRs is justified

More than 10 PRs is justified only when a terrain blocker splits into real independent work:

- tiling/seam architecture is needed;
- high-res source terrain plus lower-res runtime terrain must both be maintained;
- automated validation requires a new tool layer;
- runtime representation changes from Landscape to mesh/hybrid;
- Linux build/import tooling requires separate fixes;
- hazard/catchment generation requires algorithmic iteration.

**Do not split PRs by cosmetic output.** Split by reviewable terrain contracts and irreversible dependencies.

## Risks And Stop Conditions

### Major risks

- **Scale drift:** terrain looks correct but is not 16.256 km in UE units.
- **Z-scale mismatch:** peak height and sea level differ between source raster and UE runtime.
- **Resolution mismatch:** source maps and runtime terrain use different pixel/vertex alignment.
- **Mask drift:** masks become material/editor artifacts rather than exported contracts.
- **Coastline instability:** later systems start before the beach/ocean boundary is final.
- **Hydrology fakery:** gullies/ridges are visual noise and catchments are absent.
- **Hazard ambiguity:** hard blockers are not explicit, causing traversal/environment assumptions to diverge.
- **Binary bloat:** generated terrain artifacts flood source control without manifest/checksum discipline.
- **Scope creep:** foliage, weather, materials, gameplay, or UI work enters PRs and hides terrain incompleteness.

### Hard stop conditions

Stop the batch and fix terrain before merging final handoff if any of these occur:

- The terrain cannot be regenerated from committed parameters/code.
- The world container size, origin, sea level, or Z scale is unknown.
- The island is square, clipped, or fills the world container as land.
- The beach ring is discontinuous or lacks numeric evidence.
- Peak elevation is achieved by an artifact rather than massif geometry.
- Crater/hazard masks do not correspond to terrain geometry.
- Catchment/ridge/gully maps are absent.
- Runtime UE5 terrain cannot be loaded, collision-tested, or debug-inspected.
- Hard-blocker masks are missing or not exported.
- Later systems are already depending on unstable terrain interpretation.

## Rejected Or Forbidden Approaches

Reject these as incomplete terrain work:

- **Screenshot-only acceptance:** attractive screenshots without metrics, masks, or reproducible outputs.
- **Material-only terrain features:** beach, crater, gullies, lava crust, or hazards represented only by textures/material colors.
- **Untracked editor state:** terrain edits only stored in local editor assets without generator/source manifest.
- **Square island acceptance:** landmass follows the square world bounds or uses a rectangular falloff.
- **No sea-level contract:** ocean/beach exists visually but sea level is undefined or inconsistent.
- **No catchment evidence:** ridges/gullies exist but there is no flow/catchment validation.
- **No hard-blocker export:** dangerous/impassable terrain is not available to later systems as a mask.
- **Premature ecology/gameplay PRs:** foliage, survival, NPC, weather, audio, VFX, or player UI work presented as terrain progress.
- **Unbounded generated artifacts:** large binary terrain dumps committed without policy, checksums, or regeneration path.
- **One giant unreviewable PR:** all terrain generation, masks, runtime import, validation, and handoff merged together without intermediate gates.

## Sources And References

[^ue-landscape]: Epic Games, “Landscape Technical Guide in Unreal Engine.” Relevant points: UE Landscape height values are stored with 16-bit precision and multiplied by Z scale; Epic lists recommended Landscape sizes such as 8129 x 8129 and 4033 x 4033 vertices. <https://dev.epicgames.com/documentation/unreal-engine/landscape-technical-guide-in-unreal-engine>

[^ue-worldpartition]: Epic Games, “World Partition in Unreal Engine.” Relevant point: World Partition stores a large world in one persistent level divided into streamable grid cells loaded/unloaded by streaming sources. <https://dev.epicgames.com/documentation/unreal-engine/world-partition-in-unreal-engine>

[^usgs-3dep]: U.S. Geological Survey, “Topographic Data Quality Levels (QLs).” Relevant point: DEM quality levels specify vertical accuracy and DEM cell sizes, illustrating why synthetic terrain QA should state raster resolution and vertical tolerances explicitly. <https://www.usgs.gov/3d-elevation-program/topographic-data-quality-levels-qls>

[^usgs-glossary]: U.S. Geological Survey, “Volcano Hazards Program Glossary.” Relevant point: a volcanic edifice is the main volcano body built from lava, tephra, pyroclastic flows, lahars, and related deposits. <https://www.usgs.gov/glossary/volcano-hazards-program-glossary>

[^usgs-lava]: U.S. Geological Survey Hawaiian Volcano Observatory, “Lava Flow Hazards Zones and Flow Forecast Methods, Island of Hawai‘i.” Relevant points: lava-flow path forecasting uses mapped previous pathways and steepest descent paths derived from DEM topography; lava tends to move downslope through gullies, channels, and valleys. <https://www.usgs.gov/observatories/hvo/science/lava-flow-hazards-zones-and-flow-forecast-methods-island-hawaii>

[^gdal-gtiff]: GDAL, “GTiff — GeoTIFF File Format.” Relevant point: GeoTIFF supports georeferencing through tiepoints/pixel size, transformation matrices, or ground control points. <https://gdal.org/en/stable/drivers/raster/gtiff.html>

[^qgis-hydro]: QGIS Documentation, “Hydrological analysis.” Relevant point: catchment area / flow accumulation can be used as a channel-initiation threshold, and changing the threshold changes channel-network density. <https://docs.qgis.org/3.44/en/docs/training_manual/processing/hydro.html>

[^geomorphometry]: Geomorphometry.org, “MultiscaleDTM: An open-source R package for multiscale geomorphometric analysis.” Relevant point: terrain attributes such as slope, aspect, curvature, relative position, and roughness can be calculated from gridded terrain data across scales. <http://www.geomorphometry.org/2023/05/03/multiscaledtm-article/>
