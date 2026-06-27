# DEM Benchmarking And Real Volcanic Island Calibration Research

## Executive Summary

Jungle Game needs a terrain generator that reads as a large tropical volcanic island, not as a mathematically neat radial cone placed inside an ocean square. The calibration strategy should not copy a real island DEM. It should instead emulate measurable families of volcanic-island terrain: broad shield construction, steeper stratovolcanic cones, tropical erosional dissection, flank-collapse scars, coastal lava benches, fan-delta/lowland aprons, and mixed-age island-chain remnants.

The correct benchmark is therefore **distributional and structural**, not one-to-one geographic matching. The generator should be evaluated against DEM-derived metrics such as:

- island-wide and basin-level hypsometric curves;
- slope histograms with separate coast, lowland, highland, and active-volcano masks;
- drainage density and basin area distributions;
- ridge/valley plan and profile curvature distributions;
- relief ratios by radial band and catchment;
- cone footprint, cone height/base-width ratio, crater/caldera scale, and active-volcano mask share;
- coastal lowland width, lava bench width, cliff share, fan area, and fan spacing;
- asymmetry and anti-radial-artifact metrics, especially ring-band variance, contour irregularity, basin hierarchy, and azimuthal non-uniformity.

The terrain must preserve the existing hard constraints:

```text
world_size_m: 97536.0 target
shoreline_error_max_m: 0 or effectively 0
beach_continuity_pct: 100%
ocean_below_sea_pct: 100%
square_edge_ocean_violations: 0
outer world edge remains ocean/container terrain
```

The recommended target is a **hybrid eroded shield / stratovolcanic island profile**: old, dissected highlands and remnant ridges over most of the island, plus one currently active volcanic center that is visually obvious but does not dominate every drainage basin. On a 97.536 km terrain, the active cone should normally occupy about **8-20% of land area**, use an effective basal radius of roughly **8-18 km**, and rise **900-2,000 m above its local base**. Island peak elevation should usually fall around **1,800-3,400 m**, with a default target around **2,600-3,100 m** unless gameplay traversal strongly favors lower relief.

At preview resolution, metrics matter more than visual judgement. A 1024 px preview over 97,536 m gives **95.25 m/px**, enough for hypsometry, slope classes, broad catchments, coastline masks, and radial-artifact detection, but not enough for small rills, fine lava-flow texture, or detailed channel heads. An 8129-class source grid over the same world gives **12 m spacing** when interpreted as 8128 intervals, making it suitable for production-grade hydrologic conditioning, slope/curvature validation, source-heightfield baking, and high-resolution debug rasters.

## Terrain-Only Scope Boundary

This document is limited to terrain research, DEM comparison, measurable terrain targets, validation metrics, preview workflows, and implementation-facing calibration structures.

It does **not** design or specify:

- foliage, jungle ecology, plant distribution, biome rules, habitat simulation, or plant assets;
- weather, climate simulation, cloud systems, rain, wind, or storm design;
- animals, NPCs, humans, buildings, roads, ruins, camps, or survival systems;
- materials, final landscape shader art direction, ocean rendering, wetness, decals, or runtime VFX;
- UI, maps, minimaps, compass, GPS, player-facing overlays, or objective markers.

Any downstream implementation must treat the DEM calibration layer as a terrain foundation. It can output debug images and JSON metrics for developers, but it should not create player-facing navigational systems.

## Why Batch 004 Failed The Real-Terrain Test

Batch 004 apparently proved that the coast constraints can pass, but the island interior still read as a procedural radial volcano. That failure is not mainly a resolution problem. It is a **terrain-structure problem**.

A radial volcanic terrain usually fails the real-terrain test when it contains these signatures:

1. **Single-center radial dependence dominates elevation.** Elevation is too predictable from distance-to-peak. Real volcanic islands often begin with radial construction, but erosion, collapse, rift zones, lithologic contrasts, rainfall gradients, and basin competition disturb the simple ring pattern.
2. **Drainage basins are evenly spaced and equally important.** Young volcanic cones can show parasol-like radial gullies, but eroded volcanic edifices develop basin hierarchy: some catchments widen, capture headwaters, deepen, and push weaker basins downslope.
3. **Slope bands form concentric rings.** Real DEM slope rasters have strong local variance: cliffs, benches, incised valley walls, perched remnants, fans, lava fields, and lowland flats break radial smoothness.
4. **Contours remain circular or smoothly elliptical.** Erosion increases contour irregularity. Mature islands should show concave bites, scalloped highlands, amphitheater-like valley heads, collapse-scar notches, and uneven ridge persistence.
5. **Coastal lowlands are uniform.** Real volcanic islands often alternate between cliffs, narrow benches, pocket beaches, fan mouths, deltas, lava benches, and occasional broader coastal plains.
6. **Valley curvature is too synthetic.** Procedural valleys often have consistent width/depth and identical curvature. Real valleys show mixed V-shaped incision, amphitheater headwalls, hanging tributaries, convex interfluves, and spatially variable plan curvature.
7. **The active volcano owns the whole island.** A credible large island can have an active cone, but the older island mass should retain independent catchments, old ridgelines, collapse scars, and non-active highlands.

A real-eroded volcanic island should still contain constructional volcanic signals, but those signals must be visibly overprinted by degradation, drainage competition, flank failures, coastal attack, lava-bench preservation, and basin-scale asymmetry. NPS summarizes the general principle: volcanoes are constructional landforms, but weathering and erosion can reduce volcanic edifices to relict mass, with older volcanic landforms generally more eroded than younger ones.[^nps-erosional]

## Relevant Volcanic Island Families

The generator should not select one real island to imitate. It should use reference **families** and blend their metric envelopes.

### 1. Large basaltic shield volcanic islands

**Use for:** broad island mass, gentle lava-flow construction surfaces, low angle outer flanks, wide base relative to height, rift-zone elongation, summit caldera logic, coastal lava benches.

Shield volcanoes are relevant because they create large oceanic islands with broad bases and gentle flank slopes. NPS notes that shield volcanoes have very gentle slopes of only a few percent and that their width relative to height comes from low-viscosity lava flows, fissure eruptions, rift-zone widening, and summit/rift-zone subsidence.[^nps-shield]

**Jungle Game translation:**

- Use shield-like construction for the old island foundation.
- Keep many outer flanks in the **2-10°** slope range before valley incision.
- Use elongated rift-controlled shoulders rather than perfect circles.
- Add old benches and lava-flow plateaus, but erode them into discontinuous remnants.
- Avoid making the whole island a perfect shield dome; the game target is jungle-eroded, not fresh Mauna Loa-style smoothness.

### 2. Stratovolcanic island / composite cone centers

**Use for:** active volcano silhouette, steeper central cone, summit crater/caldera, radial ash/lahar valley logic, young cone overlay on older island.

Stratovolcanoes and composite cones provide a compact active-volcano form with steeper slopes and more obvious summit relief than shield islands. Recent volcanic-edifice morphometry work quantifies edifice height, radius, height-radius ratio, main-flank slope, ellipticity, irregularity, slope variance, eroded volume, drainage density, basin hypsometry, basin width, and basin length as useful descriptors.[^ohara-slope]

**Jungle Game translation:**

- Use one active cone as a major landmark, not the sole organizing field.
- The active cone can be more radial than the old island, but should still have breached gullies, asymmetric collapse notches, and nonuniform lava/ash aprons.
- Cone metrics should be reported separately from island-wide metrics so the volcano does not hide lowland/highland calibration failures.

### 3. Eroded tropical volcanic islands

**Use for:** deep valleys, amphitheater headwalls, high-relief ridges, nonuniform catchments, high rainfall incision, degraded older volcanic mass.

Tropical oceanic volcanic islands are especially relevant because high rainfall, steep relief, and short steep catchments create dramatic incision. Recent work on volcanic edifice erosion shows that drainage density, basin hypsometry, basin length, basin width, relief, and mean slope are useful to quantify erosional maturity.[^ohara-dd] The same study describes a transition from narrow, uniform radial drainages to wider, more dendritic, dominant basins as volcanic edifices degrade.[^ohara-evolution]

**Jungle Game translation:**

- Make older island areas strongly dissected.
- Require basin hierarchy: a few large catchments should dominate total drainage area.
- Valley heads should not all touch the active summit.
- Let some old ridges be disconnected from the active cone.
- Include lowland fans at major basin outlets.

### 4. Oceanic island chains and mixed-age volcanic complexes

**Use for:** multiple old centers, inherited ridges, offset peaks, partially buried older volcanoes, island asymmetry, remnant topographic grains.

Hawaiian volcano evolution is useful as a conceptual family because volcanic islands pass through growth, waning, degradation, and rejuvenated phases. USGS describes idealized Hawaiian volcanoes passing through preshield, shield, postshield, and rejuvenated stages, with the shield stage producing most of the ultimate volume and later volcanism waning as the volcano moves away from the hot spot.[^usgs-hawaii]

**Jungle Game translation:**

- Use 2-4 buried/remnant construction centers as low-frequency terrain controls.
- Only one needs to be active.
- Old centers should appear mostly through ridge arcs, highland shoulders, breached basins, and benches rather than clean cones.
- This is the main antidote to the procedural radial-volcano look.

### 5. Flank-collapse and sector-collapse islands

**Use for:** large amphitheater scar, breached volcano side, debris-avalanche topography, asymmetric coastal indentation, steep headwall, hummock fields if represented in terrain.

Volcanic flank collapses are rare but geomorphically important. USGS describes volcanic edifice flank collapses as low-frequency, high-magnitude events that can mobilize tens of cubic kilometers and transport material more than 10 km from the edifice.[^usgs-collapse]

**Jungle Game translation:**

- Include at least one old or partially healed flank-collapse scar.
- The scar should be asymmetric and should interrupt radial symmetry.
- It may connect upland amphitheater terrain to a coastal embayment or fan-complex lowland.
- It should not produce ocean-edge violations or break beach continuity unless beach continuity is already defined to include cliff/rock transitions in the project validator.

### 6. Coastal volcanic erosion / lava bench islands

**Use for:** cliff/bench alternation, wave-cut platforms, coastal plains limited to valley/fan mouths, old lava shelves, coastal terrace remnants.

Volcanic island coasts evolve through interactions between volcanism, erosion, sedimentation, sea-level change, and biological production. Coastal erosion of volcanic islands can also be quantified through reconstruction of former island topography and comparison against modern eroded forms.[^bossis-coast]

**Jungle Game translation:**

- Preserve the exact coastline and ocean constraints first.
- Add terrain credibility by varying **inland lowland width** behind the fixed shoreline.
- Use cliffs and benches as terrain-height bands, not shoreline mask changes.
- Add fans only where major catchments meet the coast.

## DEM Metrics Worth Emulating

DEM benchmarking should compare generated terrain to **families of metrics** rather than DEM pixels. Geomorphometry is the quantitative analysis of land-surface form from DEMs, extracting land-surface parameters and objects such as slope, curvature, watersheds, and landforms.[^geomorphometry]

### Required masks

Metrics should be computed over masks, because island-wide averages hide failure modes.

| Mask | Purpose | Suggested definition |
|---|---:|---|
| `IslandLand` | All land above sea level inside fixed coastline | `height_m > 0` and inside shoreline mask |
| `CoastBand` | Immediate terrain behind shoreline | land cells within 0-500 m of coastline |
| `NearCoastLowland` | coastal travel/lowland terrain | elevation 0-150 m and distance-to-coast <= 5 km |
| `InteriorLowland` | low-gradient inland basins/plains | elevation 0-300 m, slope <= 5°, distance-to-coast > 2 km |
| `Highland` | old dissected upland terrain | elevation > 600 m outside active-volcano mask |
| `ActiveVolcano` | active cone and crater apron | explicit generator mask, not inferred after the fact |
| `VolcanoDisabledIsland` | full island with active cone excluded/reconstructed | used to verify old-island realism |
| `CollapseScar` | sector-collapse amphitheater/debris path | explicit optional mask |
| `MajorCatchments` | hydrologic basins above threshold | flow accumulation-derived basins |
| `FansAndBenches` | depositional/lowland volcanic coast | explicit or derived low-slope coastal features |

### Core metric set

| Metric | Why it matters | Implementation notes |
|---|---|---|
| Hypsometric curve | Shows elevation-area distribution and erosion maturity | Compute per island, per major basin, per old highland, per active cone |
| Hypsometric integral | Single-number summary of area-elevation curve | Do not use alone; report with curve samples |
| Slope histogram | Detects too-flat, too-steep, or ringed terrain | Compute from meters, not normalized height |
| Relief ratio | Peak-to-base relief normalized by basin length/radius | Per basin and per volcano mask |
| Local relief | Captures valley depth/ridge ruggedness | Window sizes: 500 m, 1 km, 3 km, 5 km |
| Drainage density | Total channel length per area | Report threshold used; default threshold 1.0 km² at 1024 preview |
| Basin area histogram | Detects basin hierarchy vs uniform radial cuts | Use log bins and outlet-to-coast labeling |
| Basin elongation | Detects narrow procedural spokes | Major/minor axis or length/sqrt(area) |
| Basin outlet spacing | Detects uniform radial spacing | Report coefficient of variation and entropy |
| Plan curvature | Highlights ridges/valleys and flow convergence/divergence | Use smoothed DEM and consistent cell size |
| Profile curvature | Highlights convex/concave slope breaks, benches, fan toes | Useful for lava benches and coastal flats |
| Contour irregularity | Detects erosion against smooth cones | Compute by elevation bands and masks |
| Radial correlation | Detects elevation over-dependence on peak distance | Compare old island and active cone separately |
| Azimuthal variance | Detects nonuniform highland and basin structure | Compute elevation/slope per angular sector |

### DEM metric caveats

- **No single metric proves realism.** A terrain can pass hypsometry and still look synthetic if basin spacing is uniform.
- **Metrics must be resolution-tagged.** A drainage density at 95.25 m/px is not comparable to one at 12 m spacing unless channel extraction thresholds are normalized.
- **Use masks before averages.** Island-wide slope can pass while the coast is too flat or the active cone is too steep.
- **Do not benchmark against one island.** Build target envelopes from families and then require generator outputs to land inside envelopes without reproducing real DEM geometry.

## Hypsometric Curve Targets

Hypsometry describes the relationship between relative area and relative elevation. The hypsometric integral is the area under the hypsometric curve and is widely used as a topographic/geomorphic index; Frontiers in Earth Science describes it as an index that can characterize watershed development and erosion state when used with DEMs.[^frontiers-hi]

### Recommended hypsometric outputs

For each generated preview and source build, output:

```json
{
  "hypsometry": {
    "island_full": {
      "integral": 0.43,
      "samples": [
        {"relative_area_above": 1.00, "relative_elevation": 0.00},
        {"relative_area_above": 0.90, "relative_elevation": 0.05},
        {"relative_area_above": 0.75, "relative_elevation": 0.14},
        {"relative_area_above": 0.50, "relative_elevation": 0.31},
        {"relative_area_above": 0.25, "relative_elevation": 0.55},
        {"relative_area_above": 0.10, "relative_elevation": 0.76},
        {"relative_area_above": 0.01, "relative_elevation": 1.00}
      ]
    }
  }
}
```

### Target bands

These are **implementation calibration targets**, not universal laws.

| Terrain unit | Hypsometric integral target | Interpretation |
|---|---:|---|
| Full island, active volcano included | 0.38-0.55 | enough highland mass without youthful perfect cone dominance |
| Full island, active volcano disabled/reconstructed | 0.34-0.50 | old island reads eroded and basin-cut |
| Old highland basins | 0.30-0.48 | mature valley dissection, ridge remnants |
| Active cone mask | 0.45-0.68 | constructional cone still visually young |
| Collapse scar basin | 0.25-0.45 | excavated amphitheater / breached flank |
| Coastal lowland/fan masks | 0.05-0.22 | low relief depositional/coastal terrain |

### Shape targets

The curve should not be a perfect smooth S-curve. It should show terrain history:

- a broad low-elevation shelf for coastal benches and fans;
- a mid-elevation shoulder from old shield/lava benches;
- a steep high-elevation tail from active cone and residual high ridges;
- basin-to-basin variation instead of identical curves in every radial wedge.

### Failure signatures

| Failure | Hypsometric symptom | Likely cause |
|---|---|---|
| Radial cone dominates | high integral, smooth curve, little basin variance | distance-to-peak elevation function too strong |
| Terrain too flat | low integral and huge area below 100 m | over-wide lowland mask / weak highlands |
| Terrain too mountainous everywhere | high integral plus too little coastal area | old shield foundation not degraded |
| Volcano hides old-island failure | full island passes, volcano-disabled fails | active cone occupying too much relief budget |
| Preview lies about source | 1024 and 8129 curves diverge too much | source contains unresolved high-frequency terrain or bad resampling |

## Slope Histogram Targets

Slope is the first DEM derivative most likely to expose procedural artifacts. Shield construction surfaces can be gentle, while stratovolcanic cones, incised valley walls, cliffs, collapse headwalls, and inner crater walls can be steep. Slope should be binned by mask.

### Recommended slope bins

Use degrees for developer readability and store tangent/grade internally if useful.

| Bin | Class | Terrain meaning |
|---:|---|---|
| 0-2° | flat | beaches, fans, floodplains, lava benches, valley floors |
| 2-5° | gentle | lowland apron, broad shield surface, old coastal terrace |
| 5-10° | rolling | dissected lower flanks, low ridges, fan-head transitions |
| 10-20° | steep hillslope | common old volcanic highland and cone flank terrain |
| 20-30° | very steep | valley walls, cone upper flanks, headwalls |
| 30-40° | cliff/near-cliff | scarps, crater walls, collapse headwall, sea cliffs |
| >40° | extreme | rare cliffs, numerical spikes, should be reviewed |

### Island-wide target envelope

For a playable but credible large tropical volcanic island:

| Slope bin | Island-wide target share |
|---|---:|
| 0-2° | 6-16% |
| 2-5° | 10-22% |
| 5-10° | 18-30% |
| 10-20° | 25-40% |
| 20-30° | 8-18% |
| 30-40° | 1-6% |
| >40° | 0-2% |

### Mask-specific target envelope

| Mask | Mean slope target | Notes |
|---|---:|---|
| `CoastBand` | 2-12° | split between benches/fans and cliffs; avoid uniform flat ring |
| `NearCoastLowland` | 1-6° | lowlands and fan surfaces; local scarps permitted |
| `InteriorLowland` | 1-8° | low-gradient basin floors, old lava benches |
| `Highland` | 10-22° | dissected ridges and valley walls |
| `ActiveVolcano` | 8-24° | shield-like active center lower; stratovolcanic cone higher |
| `CollapseScar` | 12-30° | high variance; headwall pockets may exceed 35° |

### Shield and cone calibration

- Shield-like old surfaces: most construction slopes should sit in **2-10°**, with erosion adding local steep valley walls.
- Active stratovolcanic cone: main flanks can average **10-20°**, with localized crater/scar walls above 30°.
- Do not let the active cone force the whole island histogram upward. Report `island_without_active_volcano` slope statistics.

### Slope artifact checks

Add these derived checks:

```text
ring_slope_variance_min:
  For each 1 km radial band around active cone, sector mean slope CV should exceed 0.18 outside the inner cone.

slope_bin_ring_dominance_max:
  No single radial distance band may contain >35% of one slope class unless it is a coastline/sea-level mask band.

extreme_slope_spike_pct_max:
  >40° cells should be <=2% island-wide and should be spatially clustered in cliffs/scars/craters, not random noise.
```

## Drainage Density And Basin Statistics

Drainage density is the total channel length divided by drainage area. A DEM-derived drainage density depends heavily on stream-initiation threshold, smoothing, cell size, and hydrologic conditioning. Dragičević et al. define drainage density as total channel length per unit area and note that DEM and river-network derivation choices affect precision and spatial variability.[^dragicevic-dd]

O'Hara and coauthors used volcanic edifice drainage metrics including drainage density, basin hypsometry, basin length, width, relief, and average slope; they selected a **1.0 km²** drainage-area threshold for consistent edifice-scale comparisons after sensitivity analysis.[^ohara-dd]

### Recommended extraction rules

For Jungle Game previews:

| Resolution | Grid spacing | Recommended stream initiation | Purpose |
|---|---:|---:|---|
| 1024 preview | 95.25 m/px | 1.0 km² default; also test 0.5 and 2.0 km² sensitivity | broad catchments and artifact detection |
| 2048 preview, if used | 47.625 m/px | 0.5-1.0 km² | better valley/fan association |
| 8129 source | 12.0 m spacing | 0.05-0.25 km² for fine channels; 1.0 km² for cross-scale comparison | production hydrology and terrain validation |

Always report:

```text
stream_area_threshold_km2
sink_fill_or_breach_method
smoothing_window_m
cell_size_m
flow_routing_method
coast_outlet_count
endorheic_or_sink_count
```

### Drainage density targets

| Terrain unit | Target drainage density | Notes |
|---|---:|---|
| Full island at 1.0 km² threshold | 0.45-1.40 km/km² | broad DEM-style comparison range |
| Old dissected highland | 0.70-1.80 km/km² | tropical erosion and valley hierarchy |
| Active cone, young surfaces | 0.35-1.00 km/km² | fresh lava may be under-dissected; gullies radial |
| Collapse scar basin | 0.60-1.60 km/km² | depends on scar age and regrowth |
| Coastal lowlands/fans | 0.10-0.70 km/km² | low slope; channels concentrate into outlets |

Interpretation should follow O'Hara et al.'s trend cautiously: young radial volcanic edifices can have narrow, uniform drainages and high drainage density near ~1 km⁻¹, while degraded forms can show wider, more dendritic dominant basins and lower edifice-scale drainage density around ~0.5 km⁻¹ as basins expand and compete.[^ohara-evolution]

### Basin area distribution targets

A real island should have uneven basin hierarchy.

| Metric | Target |
|---|---:|
| Major coastal catchments above 10 km² | 8-22 |
| Dominant catchment share of land area | 8-22% |
| Top 5 catchments share of land area | 35-65% |
| Catchments under 2 km² | many, but mostly coastal/interfluvial |
| Basin area histogram | heavy-tailed/log-normal-like, not uniform |
| Outlet spacing coefficient of variation | >=0.35 for old island coast |
| Summit-reaching basins on active cone | 4-12 depending cone size |
| Summit-reaching basins on old highland | should not all point to active cone |

### Basin shape targets

| Metric | Target | Failure if |
|---|---:|---|
| Basin elongation `length / sqrt(area)` | 1.6-4.5 common; >5 only for chute-like/scar basins | all basins ~same elongation |
| Basin width angle near active summit | variable; 20-70° for summit basins | equal wedge slices everywhere |
| Tributary branching ratio | variable by basin; report Strahler order counts | channels are single straight spokes |
| Headwater elevation variance | high outside active cone | all headwaters form one ring |
| Outlet fan association | major basins should often create fan/lowland masks | large basins meet steep coast with no depositional expression |

## Ridge / Valley Curvature Statistics

Curvature metrics are essential because terrain can pass elevation and slope checks while still feeling synthetic. Plan curvature emphasizes ridges and valleys; profile curvature captures acceleration/deceleration zones, slope breaks, benches, and fan toes. Esri's curvature guidance describes plan curvature as influencing convergence/divergence of flow and profile curvature as influencing acceleration/deceleration, erosion, and deposition.[^esri-curvature]

### Required curvature rasters

Output developer-only rasters:

```text
plan_curvature_1024.png
profile_curvature_1024.png
mean_curvature_1024.png
ridge_valley_class_1024.png
curvature_percentiles.json
```

At 8129-class source resolution, prefer EXR/TIFF/RAW debug outputs if the pipeline already supports them.

### Curvature classes

Use normalized curvature after smoothing and scale tagging. Suggested classes:

| Class | Condition | Terrain interpretation |
|---|---|---|
| `StrongValley` | plan curvature <= p05 | convergent valley axis / channelized hollow |
| `WeakValley` | p05 < plan <= p25 | side hollows, minor swales |
| `Planar` | p25 < plan < p75 and profile near median | benches, planar slopes, fans |
| `WeakRidge` | p75 <= plan < p95 | convex interfluves |
| `StrongRidge` | plan >= p95 | sharp ridge/knife edge/spur |
| `ConvexBreak` | profile >= p90 | ridge crests, convex slope breaks |
| `ConcaveBreak` | profile <= p10 | toe slopes, valley bottoms, fan heads |

### Target ratios

| Mask | Valley class target | Ridge class target | Notes |
|---|---:|---:|---|
| Full island | 18-35% weak/strong valley | 18-35% weak/strong ridge | should be balanced enough to avoid smooth dome |
| Old highland | 25-45% valley | 25-45% ridge | strongly dissected terrain |
| Active cone | 12-30% valley | 12-30% ridge | younger cone may be smoother |
| Coastal lowland/fans | 5-20% valley | 5-20% ridge | low curvature except fan channels and scarps |
| Collapse scar | 25-50% valley/concave | 10-35% ridge | amphitheater plus debris/drainage path |

### Ridge/valley artifact checks

- Valley axes should not all radiate from the active summit.
- Ridge networks should include inherited arcs, rift-aligned ridges, and irregular interfluves.
- Curvature magnitude should vary by terrain unit.
- The curvature map should reveal basin hierarchy: large trunk valleys with tributaries, not evenly spaced identical grooves.
- Coastal fans should appear as low-slope convex-to-planar deposits at major outlets.

## Coastal Lowland And Fan Metrics

The coastline is already a hard-preserved constraint. Calibration should therefore adjust **terrain immediately inland from the shoreline**, not the shoreline itself.

### Coastal band definitions

| Band | Distance from shoreline | Purpose |
|---|---:|---|
| `ShorelineLockBand` | 0-1 cell or existing validator band | must preserve exact coast/ocean constraints |
| `BeachContinuityBand` | project-defined beach validation band | must remain 100% continuous if validator expects it |
| `CoastTerrainBand` | 0-500 m inland | cliffs, benches, beach backs, immediate coastal slope |
| `LowlandApronBand` | 0-3 km inland | fans, coastal plains, lava benches |
| `OuterCatchmentMouthBand` | 0-5 km inland | major basin outlets and fan-delta terrain |

### Coastal lowland targets

| Metric | Target |
|---|---:|
| Land area below 50 m | 4-14% |
| Land area below 150 m | 10-26% |
| Land area below 300 m | 18-38% |
| Median coastal lowland width | 0.4-2.0 km |
| 90th percentile lowland width | 2.0-6.0 km |
| Cliff/steep coast share in 0-500 m band | 15-45% |
| Gentle coast/fan/bench share in 0-500 m band | 35-70% |
| Major fan count | 6-18 |
| Major fan area | 0.5-12 km² each |
| Lava bench/terrace width | 0.2-2.5 km, discontinuous |

### Fan detection target

A coastal fan candidate should satisfy:

```text
distance_to_coast_m <= 5000
elevation_m <= 300
slope_deg <= 6 for core, <=10 for margin
located near major catchment outlet
positive/planar profile curvature transition at fan head
radial spreading away from valley mouth, not away from volcano summit
```

### Anti-uniform-coast checks

| Check | Target |
|---|---:|
| Lowland width coefficient of variation around coast | >=0.45 |
| Consecutive identical coast-slope segments | <=8 km unless intentionally lava bench |
| Major basin mouths with fan/lowland expression | >=60% |
| Sea cliff clusters | present but not continuous around whole island |
| Pocket lowlands | present between cliffed segments |

## Volcano Footprint And Relief Calibration

The active volcano needs to be scaled to the island, not simply maximized.

### Island scale assumptions

The world target is **97,536 m**. If the preview uses 1024 cells, the pixel pitch is:

```text
97536 / 1024 = 95.25 m per pixel
```

If source resolution is 8129 samples across 8128 intervals, the spacing is:

```text
97536 / 8128 = 12.0 m per interval
```

This matters because a cone with a 12 km radius is:

- ~126 px radius in a 1024 preview;
- 1000 source intervals at 8129-class resolution.

### Active volcano target envelope

| Metric | Target |
|---|---:|
| Active cone effective basal radius | 8-18 km |
| Active cone diameter | 16-36 km |
| Active cone land-area share | 8-20% |
| Active cone relief above local base | 900-2,000 m |
| Active summit elevation | 1,800-3,400 m default island target |
| Height/base-diameter ratio | 0.035-0.12 typical target |
| Crater/caldera diameter | 0.8-4.0 km |
| Crater depth | 80-450 m |
| Young lava/ash apron share inside cone mask | 15-45% |
| Cone center offset from island centroid | 5-25 km preferred |
| Cone overlap with older highlands | yes, but not total |

### Shield vs stratovolcano scaling

Use two profile options:

| Profile | Use case | Height/base-diameter ratio | Mean flank slope |
|---|---|---:|---:|
| Shield-active profile | broader basaltic active center | 0.025-0.070 | 4-12° |
| Stratovolcanic-active profile | sharper landmark cone | 0.070-0.160 | 10-22° |
| Hybrid recommended | broad base plus steeper summit cone | 0.045-0.110 | 8-18° |

For Jungle Game, the **hybrid** is the safest target: broad shield-like base to fit the island scale, steeper young summit to read as active, and asymmetric erosion/collapse to prevent a perfect cone.

### Volcano-disabled DEM comparison

Every manifest should report two terrain states:

1. `island_full`: all terrain included.
2. `island_without_active_volcano`: active cone removed from global metrics or reconstructed to the inferred old-island surface.

Use the second state to answer: *does the island still look like a real eroded volcanic island when the active landmark is ignored?*

Implementation options:

- exclude `ActiveVolcano` cells from island-wide statistics;
- fill the active cone mask with a low-frequency reconstructed old-shield surface for hypsometry and drainage comparison;
- compute all old-highland, lowland, and catchment metrics outside the active cone mask.

Acceptance should require both states to pass target envelopes.

## Flank Collapse And Old Lava Terrain Calibration

### Flank-collapse scar metrics

A flank collapse is one of the strongest ways to break radial symmetry, but it must be scaled carefully.

| Metric | Target |
|---|---:|
| Scar headwall width | 3-12 km |
| Scar length from upper flank to lower outlet | 8-25 km |
| Scar depth below reconstructed surface | 150-900 m |
| Scar azimuthal span | 30-120° |
| Scar area share of island | 2-10% |
| Scar slope variance | high; >1.25x surrounding highland |
| Scar outlet relation | may connect to coast/fan/embayment, but must preserve shoreline constraints |

The scar should be old enough to show drainage overprinting unless the project explicitly wants a fresh catastrophic scar. A mature scar has:

- amphitheater headwall remnants;
- deeply incised scar-floor channels;
- lateral ridges/spurs;
- an asymmetric fan or debris apron downslope;
- weakened but still visible sector boundary.

### Old lava benches and terraces

Old lava terrain is useful because it creates slope breaks and local plateaus that contrast with valley incision.

| Feature | Target |
|---|---:|
| Lava bench elevation bands | 80-250 m, 300-700 m, optional 900-1300 m remnants |
| Bench width | 0.2-2.5 km typical, 4 km max for rare broad shelf |
| Bench continuity | discontinuous; cut by valleys |
| Bench slope | 0-5° core, 5-10° eroded margins |
| Bench curvature | planar to weak convex top; concave toe at break |
| Bench relation to drainage | valleys cut through benches; fans may bury lower bench edges |

### Old highland ridge calibration

Old volcanic highlands should have multiple ridge families:

1. **Constructional/rift ridges:** long, low-frequency inherited volcanic grain.
2. **Erosional interfluves:** sharper ridges between major catchments.
3. **Collapse-scar margins:** arcuate or scalloped ridges.
4. **Residual cone/rim ridges:** partial circles or crescents, never perfect full rings.

Metric targets:

| Metric | Target |
|---|---:|
| Ridge orientation entropy | high outside active cone |
| Ridge length distribution | heavy-tailed; few long ridges, many short spurs |
| Ridge-to-valley relief at 1 km window | 80-450 m in highlands |
| Ridge crest slope | often 5-20°, locally steeper |
| Major ridge count above 5 km length | 8-30 |

## Preview Comparison Workflow

### Goal

The preview workflow should answer: **Does this generated heightfield match the statistical and structural signature of an eroded tropical volcanic island while preserving the exact island/ocean constraints?**

It should not answer: **Does this generated terrain match Island X?**

### 1024 px workflow

At 1024 px, use the preview as a fast validation layer.

1. Load heightfield and masks.
2. Confirm hard constraints first:
   - world size equals target;
   - shoreline error is zero/effectively zero;
   - beach continuity is 100%;
   - ocean below sea is 100%;
   - square world edge remains ocean/container terrain;
   - square-edge ocean violations equal zero.
3. Compute preview grid spacing: `world_size_m / 1024`.
4. Compute elevation histogram and hypsometry for masks.
5. Compute slope raster using metric spacing.
6. Compute slope class histogram per mask.
7. Smooth heightfield with 3x3 and/or 300 m equivalent window for structure metrics.
8. Compute plan/profile curvature rasters.
9. Hydrologically condition terrain for analysis only; do not mutate source terrain unless explicitly intended.
10. Extract flow direction, flow accumulation, stream network, and catchments.
11. Compute drainage density using 1.0 km² threshold plus sensitivity thresholds.
12. Compute radial-artifact metrics around active cone and island high point.
13. Emit preview PNG overlays and manifest JSON.

### 8129-class source workflow

At source resolution, use stricter hydrology and terrain-quality checks.

1. Confirm source spacing is 12 m if using 8129 samples over 97,536 m.
2. Recompute all 1024 metrics at source scale.
3. Downsample source to 1024 with documented method and compare aggregate metric drift.
4. Extract fine channels at 0.05-0.25 km² thresholds.
5. Verify major catchments remain stable when compared to 1.0 km² threshold.
6. Check slope spikes and quantization artifacts.
7. Export high-resolution debug rasters for slope, curvature, catchment IDs, ridge/valley class, fans, benches, active-volcano mask, and collapse-scar mask.
8. Confirm no high-frequency noise is being used to fake geomorphology.

### Cross-resolution stability targets

| Metric | 1024 vs downsampled-source tolerance |
|---|---:|
| Island hypsometric integral | ±0.03 |
| Old highland hypsometric integral | ±0.04 |
| Mean slope island-wide | ±2.0° |
| Slope bin share | ±5 percentage points per major bin |
| Major catchment count >10 km² | ±2 catchments |
| Top 5 catchment area share | ±8 percentage points |
| Drainage density at 1 km² threshold | ±0.20 km/km² |
| Coastal lowland area share | ±5 percentage points |
| Active cone area share | ±2 percentage points |

### DEM reference comparison workflow

Use reference DEMs only to build target envelopes.

1. Select reference family examples: shield, stratovolcanic, eroded tropical, flank-collapse, old island chain remnant.
2. Acquire DEMs from permissible sources such as SRTM, Copernicus DEM, or OpenTopography-hosted datasets.
3. Normalize each reference into comparable masks and metrics.
4. Store only aggregate metric summaries and citations, not copied heightfields.
5. Do not bake real DEM shapes into the generator.
6. Avoid naming one island as the generator target.
7. Keep reference DEM files out of shipped content unless their license and attribution are explicitly approved.

NASA describes SRTM as a Shuttle Endeavour radar mission that collected topographic data over nearly 80% of Earth's land surfaces, creating a near-global land-elevation dataset.[^nasa-srtm] Copernicus DEM GLO-30 provides global 30 m coverage and has explicit license/citation obligations.[^copernicus-dem] OpenTopography provides access to multiple global DEM datasets through APIs, but also requires API-key and licensing compliance.[^opentopography-api]

## Manifest Fields And Debug Outputs

### Required top-level manifest fields

```json
{
  "terrain_benchmark_schema_version": 1,
  "world_size_m": 97536.0,
  "grid_resolution_px": 1024,
  "grid_spacing_m": 95.25,
  "source_resolution_px": 8129,
  "source_grid_spacing_m": 12.0,
  "seed": 123456,
  "profile_name": "ErodedHybridVolcanicIsland_v1",
  "hard_constraints": {
    "shoreline_error_max_m": 0.0,
    "beach_continuity_pct": 100.0,
    "ocean_below_sea_pct": 100.0,
    "square_edge_ocean_violations": 0,
    "outer_world_edge_ocean": true
  }
}
```

### `FTopographicBenchmarkMetrics`

Suggested Unreal-facing structure:

```cpp
USTRUCT(BlueprintType)
struct FHistogramBin
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    double MinValue = 0.0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    double MaxValue = 0.0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int64 Count = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    double Fraction = 0.0;
};

USTRUCT(BlueprintType)
struct FHypsometrySample
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    double RelativeAreaAbove = 0.0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    double RelativeElevation = 0.0;
};

USTRUCT(BlueprintType)
struct FTopographicBenchmarkMetrics
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName MaskName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    double AreaKm2 = 0.0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    double MinElevationM = 0.0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    double MeanElevationM = 0.0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    double MedianElevationM = 0.0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    double MaxElevationM = 0.0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    double ReliefM = 0.0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    double HypsometricIntegral = 0.0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FHypsometrySample> HypsometrySamples;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    double MeanSlopeDeg = 0.0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    double MedianSlopeDeg = 0.0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    double SlopeP90Deg = 0.0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FHistogramBin> SlopeHistogramDeg;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    double LocalReliefP50M_1Km = 0.0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    double LocalReliefP90M_1Km = 0.0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    double PlanCurvatureP05 = 0.0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    double PlanCurvatureP95 = 0.0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    double ProfileCurvatureP05 = 0.0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    double ProfileCurvatureP95 = 0.0;
};
```

### `FDemComparisonTarget`

```cpp
USTRUCT(BlueprintType)
struct FMetricRange
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    double Min = 0.0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    double PreferredMin = 0.0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    double PreferredMax = 0.0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    double Max = 0.0;
};

USTRUCT(BlueprintType)
struct FDemComparisonTarget
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName TargetName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName MaskName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FMetricRange HypsometricIntegral;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FMetricRange MeanSlopeDeg;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FMetricRange DrainageDensityKmPerKm2;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FMetricRange LocalReliefP90M_1Km;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FMetricRange CoastalLowlandWidthM;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FMetricRange RadialElevationCorrelation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString SourceFamilyNote;
};
```

### `FVolcanicIslandCalibrationProfile`

```cpp
USTRUCT(BlueprintType)
struct FVolcanicIslandCalibrationProfile
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName ProfileName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    double WorldSizeM = 97536.0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    double TargetPeakElevationM = 2800.0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    double TargetMeanSlopeDeg = 12.0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    double TargetIslandHypsometricIntegral = 0.43;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    double TargetOldIslandHypsometricIntegral = 0.40;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    double ActiveVolcanoBaseRadiusM = 12000.0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    double ActiveVolcanoReliefM = 1500.0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    double ActiveVolcanoLandAreaFraction = 0.14;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    double DrainageAreaThresholdKm2Preview = 1.0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    double DrainageAreaThresholdKm2Source = 0.10;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FDemComparisonTarget> ComparisonTargets;
};
```

### Required debug outputs

| Output | Format | Purpose |
|---|---|---|
| `calibration_report.json` | JSON | all metrics and pass/fail state |
| `calibration_report.md` | Markdown | human-readable summary |
| `hypsometry_island.csv` | CSV | curve plotting data |
| `hypsometry_by_basin.csv` | CSV | basin-level curves |
| `slope_histogram.csv` | CSV | slope classes by mask |
| `catchment_area_histogram.csv` | CSV | basin hierarchy validation |
| `drainage_network_1024.png` | PNG | preview stream/catchment overlay |
| `catchment_ids_1024.png` | PNG | basin segmentation |
| `ridge_valley_curvature_1024.png` | PNG | curvature class map |
| `radial_artifact_report.json` | JSON | ring/azimuth/peak-distance checks |
| `coastal_lowland_width.csv` | CSV | shoreline-normal lowland-width samples |
| `mask_summary.png` | PNG | island/coast/highland/volcano/scar masks |

### Suggested pass/fail block

```json
{
  "calibration_pass": false,
  "blocking_failures": [
    "old_island_hypsometric_integral_below_min",
    "top_5_catchment_area_share_too_low",
    "radial_elevation_correlation_too_high"
  ],
  "warnings": [
    "active_volcano_area_fraction_near_upper_bound",
    "coastal_cliff_share_low"
  ]
}
```

## Files And Areas Likely Touched

No repository paths are assumed by this no-context research document. The implementation areas likely touched are:

| Area | Likely change |
|---|---|
| Terrain generation profile/config | Add `FVolcanicIslandCalibrationProfile` and terrain-family target ranges |
| Heightfield generation math | Add old-island, active-cone, collapse-scar, lava-bench, fan, and basin hierarchy controls |
| Mask generation | Emit stable masks for island, coast, lowland, highland, active volcano, collapse scar, fans/benches |
| Preview commandlet/tool | Compute 1024 px metrics and write debug artifacts |
| Source-resolution bake pipeline | Compute 8129-class metrics and cross-resolution stability checks |
| Hydrology analysis module | Flow routing, basin extraction, drainage density, outlet spacing, basin histograms |
| Geomorphometry module | Slope, local relief, curvature, contour irregularity, radial artifact metrics |
| JSON manifest schema | Add calibration fields and pass/fail blocks |
| Tests/golden previews | Add deterministic acceptance tests for shoreline constraints and DEM-style metrics |
| Developer docs | Explain benchmark philosophy, source licensing cautions, and metric interpretation |

## Large PR Implementation Scope

This is a large implementation because it touches math, masks, analysis, reporting, and acceptance tests. It should be staged rather than attempted as one opaque terrain rewrite.

### Recommended PR sequence

1. **Metrics foundation PR**
   - Add slope, hypsometry, relief, histogram, and mask summary metrics.
   - Preserve existing shoreline/ocean validators unchanged.
   - Emit JSON and CSV debug outputs.

2. **Hydrology and basin PR**
   - Add flow routing, flow accumulation, catchment extraction, drainage-density reporting, and basin-area histograms.
   - Add threshold sensitivity reporting.

3. **Curvature and radial-artifact PR**
   - Add plan/profile curvature outputs.
   - Add radial elevation correlation, ring-band slope variance, contour irregularity, and basin outlet spacing metrics.

4. **Terrain profile PR**
   - Add the hybrid volcanic-island calibration profile.
   - Add active volcano mask, old island mask, collapse-scar controls, coastal fan/bench controls.

5. **Acceptance and preview-suite PR**
   - Turn target envelopes into pass/warn/fail acceptance criteria.
   - Add deterministic seeds and preview comparison outputs.
   - Add regression tests to ensure shoreline/ocean constraints remain locked.

### Why this should be a large PR or batch of tightly linked PRs

The generator and validator must evolve together. If terrain math changes without metrics, visual judgement will regress to subjective screenshots. If metrics land without generator changes, the report will only prove the known radial failure. The safe implementation path is a series of scoped PRs that first create measurement, then use measurement to drive terrain changes.

## Acceptance Criteria

### Hard preservation gates

These are non-negotiable:

```text
world_size_m == 97536.0 target
shoreline_error_max_m == 0 or effectively 0
beach_continuity_pct == 100%
ocean_below_sea_pct == 100%
square_edge_ocean_violations == 0
outer world edge remains ocean/container terrain
```

If any hard gate fails, DEM calibration does not matter; the build fails.

### Terrain calibration gates

A preview passes the terrain benchmark when all required metrics are inside hard envelope and most are inside preferred envelope.

| Gate | Required target |
|---|---:|
| Island peak elevation | 1,800-3,400 m unless profile overrides |
| Full island hypsometric integral | 0.38-0.55 |
| Old island / volcano-disabled HI | 0.34-0.50 |
| Island mean slope | 8-16° preferred, 6-20° hard |
| Highland mean slope | 10-22° |
| Active cone land area share | 8-20% |
| Active cone base radius | 8-18 km |
| Active cone relief | 900-2,000 m |
| Drainage density at 1 km² threshold | 0.45-1.40 km/km² island-wide |
| Major catchments >10 km² | 8-22 |
| Top 5 catchment area share | 35-65% |
| Lowland below 150 m | 10-26% land area |
| Median coastal lowland width | 0.4-2.0 km |
| Major fan count | 6-18 |
| Radial elevation correlation outside active cone | <=0.75 preferred; <=0.85 hard |
| Old-island outlet spacing CV | >=0.35 |
| 1024/source HI drift | <=0.03 island-wide |

### Visual debug acceptance

Developer debug images should show:

- coastline remains unchanged;
- ocean edge is clean;
- old highlands are dissected by nonuniform basins;
- active cone is visible but not the only organizing structure;
- coastal lowlands vary around the island;
- major catchments create fans/lowlands at outlets;
- curvature map shows ridge/valley hierarchy;
- radial artifact report does not show concentric slope/elevation bands dominating outside the active cone.

### Stop conditions

Stop implementation or reject the build if:

- any shoreline/ocean hard gate regresses;
- the generator uses real DEM height samples or copied island outlines;
- the active cone occupies more than 25% of land area without explicit design approval;
- all major catchments point to the active summit;
- `island_without_active_volcano` fails old-island metrics;
- 1024 preview and 8129 source metrics diverge enough to imply preview validation is misleading;
- slope spikes exceed >40° in random/noise patterns rather than named scar/cliff/crater masks;
- hydrologic analysis creates many inland sinks not associated with crater/caldera/lake masks;
- source DEM licensing/attribution cannot be documented.

## Risks And Stop Conditions

### Risk: metric gaming

A generator can be tuned to pass numeric ranges while still looking fake. Mitigation: combine distribution metrics with structure metrics and debug rasters.

### Risk: copied DEM contamination

Real DEMs are useful for target envelopes, but copying a DEM shape creates legal, creative, and design problems. Mitigation: store only aggregate benchmark statistics and never ingest real DEM cells into shipped terrain generation.

### Risk: resolution mismatch

Drainage density, curvature, and slope can change with resolution. Mitigation: report cell size, smoothing, threshold, and cross-resolution drift.

### Risk: shoreline regression

Interior terrain improvements can accidentally perturb the fixed coast. Mitigation: run shoreline/ocean hard gates first and fail fast.

### Risk: volcano dominance

An impressive active cone can hide weak old-island terrain. Mitigation: require volcano-disabled metrics and old-highland metrics.

### Risk: excessive steep terrain

Real volcanic islands can be very steep, but game traversal may suffer. Mitigation: keep steepness localized in highlands, craters, scars, and cliffs; maintain lowland/fan/bench networks for traversable corridors without designing gameplay systems here.

### Risk: false hydrology from unconditioned terrain

Raw procedural noise can create pits and broken flow. Mitigation: use hydrologic conditioning for analysis; decide separately whether source terrain should be breached/fixed.

## Rejected Or Forbidden Approaches

1. **Do not copy any real island DEM.** Reference DEMs may inform aggregate metric ranges only.
2. **Do not use one real island as the map template.** The target is a synthetic but plausible family blend.
3. **Do not sacrifice shoreline/ocean constraints.** Coast correctness remains a hard gate.
4. **Do not make the island a single radial cone.** Even if the active cone is radial, old terrain must not be.
5. **Do not fake geomorphology with high-frequency noise.** Noise can texture surfaces but cannot replace catchments, ridges, benches, fans, and scars.
6. **Do not let island-wide averages hide masks.** Always report coast/lowland/highland/active-volcano metrics separately.
7. **Do not tune only from screenshots.** Every preview should emit numeric evidence.
8. **Do not design foliage, gameplay, UI, or player navigation in this terrain benchmark.** Keep scope terrain-only.
9. **Do not ship reference DEM data unless licensing is explicitly approved.** Even permissive data needs attribution and source tracking.
10. **Do not use a DEM comparison as a cell-by-cell similarity test.** That incentivizes copying and punishes valid synthetic variation.

## Sources And References

[^geomorphometry]: International Society for Geomorphometry, “Geomorphometry,” describes geomorphometry as quantitative land-surface analysis and extraction of land-surface parameters and objects from DEMs. https://www.geomorphometry.org/

[^nps-shield]: U.S. National Park Service, “Shield Volcanoes,” notes that shield volcanoes have very gentle slopes and great width relative to height because low-viscosity lavas spread widely. https://www.nps.gov/articles/000/shield-volcanoes.htm

[^nps-erosional]: U.S. National Park Service, “Erosional Volcanic Landforms,” describes volcanoes as constructional landforms that are modified by weathering and erosion, with older volcanoes generally more eroded. https://www.nps.gov/subjects/volcanoes/erosional-volcanic-landforms.htm

[^usgs-hawaii]: D. A. Clague and G. B. Dalrymple, “Growth and degradation of Hawaiian volcanoes,” USGS Professional Paper 1801, Chapter 3. The USGS page summarizes Hawaiian volcanic stages and notes that shield-stage volcanism produces most of a volcano's ultimate volume. https://pubs.usgs.gov/publication/pp18013

[^ohara-dd]: O'Hara et al., “Time-varying drainage basin development and erosion on volcanic edifices,” Earth Surface Dynamics, 2024. The article quantifies volcanic edifice drainage geometries with Hack's law, drainage density, basin hypsometry, length, width, relief, and average slope; it also discusses a 1.0 km² drainage-area threshold for consistency. https://esurf.copernicus.org/articles/12/709/2024/

[^ohara-slope]: O'Hara et al., 2024, discusses volcanic-edifice metrics including height, effective radius, height-radius ratio, main-flank mean slope, contour ellipticity, irregularity, slope variance, and normalized eroded volume. https://esurf.copernicus.org/articles/12/709/2024/

[^ohara-evolution]: O'Hara et al., 2024, describes degradation from narrow, uniform radial drainage toward wider, more dendritic dominant basins, lower drainage density, and basin hierarchy as volcanic edifices erode. https://esurf.copernicus.org/articles/12/709/2024/

[^frontiers-hi]: Duan, Pei, and Zhang, “The Hypsometric Integral Based on Digital Elevation Model for the Area West of Lvliang Mountains in Loess Plateau, Shanxi, China,” Frontiers in Earth Science, 2022. The article describes HI as a topographical index used to characterize watershed development and erosion state. https://www.frontiersin.org/articles/10.3389/feart.2022.827836/full

[^dragicevic-dd]: Dragičević, Karleuša, and Ožanić, “Different Approaches to Estimation of Drainage Density and Their Effect on the Erosion Potential Method,” Water, 2019. The article defines drainage density as total channel length per unit area and discusses DEM/network-method effects on drainage-density mapping. https://www.mdpi.com/2073-4441/11/3/593

[^esri-curvature]: Esri, “Understanding curvature rasters,” explains plan and profile curvature and their relevance to flow convergence/divergence, acceleration/deceleration, erosion, and deposition. https://www.esri.com/arcgis-blog/products/product/imagery/understanding-curvature-rasters

[^usgs-collapse]: U.S. Geological Survey, “Investigating the potential for volcanic island flank collapse and tsunami generation within the Pacific and Indian Oceans,” describes volcanic edifice flank collapses as low-frequency, high-magnitude events that can mobilize tens of km³ and move material more than 10 km. https://www.usgs.gov/media/videos/investigating-potential-volcanic-island-flank-collapse-and-tsunami-generation-within

[^bossis-coast]: Bossis et al., “Initial shape reconstruction of a volcanic island as a tool for quantifying long-term coastal erosion: the case of Corvo Island (Azores),” Earth Surface Dynamics, 2023. The article presents volcanic island reconstruction as a way to quantify long-term coastal erosion. https://esurf.copernicus.org/articles/11/529/2023/

[^nasa-srtm]: NASA Earthdata, “Shuttle Radar Topography Mission (SRTM),” describes SRTM as collecting topographic data over nearly 80% of Earth's land surfaces and creating near-global land-elevation data. https://www.earthdata.nasa.gov/data/instruments/srtm

[^copernicus-dem]: Copernicus Data Space Ecosystem, “Copernicus DEM,” describes GLO-30 and GLO-90 global coverage, resolution, access, licensing, attribution, and citation guidance. https://dataspace.copernicus.eu/explore-data/data-collections/copernicus-contributing-missions/collections-description/COP-DEM

[^opentopography-api]: OpenTopography, “OpenTopography for Developers” and “Terms of Use,” describe available global DEM APIs, API-key limitations, and data licensing/attribution obligations. https://opentopography.org/developers and https://opentopography.org/usageterms
