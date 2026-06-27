# Geomorphology Math Core Foundation Research

**Project:** Jungle Game  
**Terrain math source version:** `JG_TERRAIN_MATH_CORE_005_001`  
**Document version:** `PR5_BATCH_005_RESEARCH_001`  
**Primary deliverable:** mathematical foundation for a deterministic 97.536 km UE5 Linux-first volcanic jungle island terrain generator.

---

## Executive Summary

Batch 004 proved that the coastal system can pass the hard world-scale gates, but it also proved that the island interior is governed by the wrong authority. The current terrain reads as a volcano preview because the crater or volcano-centered radial system is acting as the global organizing coordinate frame. That cannot be fixed with prettier noise, stronger ridge masks, or small radial-gully tweaks. The terrain needs a new authority hierarchy where the island is a complete geomorphic object before the active volcano layer exists.

The recommended rewrite is a **hybrid field + graph + process + feature-stamp system**:

1. **Island mass and coastal envelope fields** preserve the existing coastline, beach, shelf, sea, and square-edge ocean guarantees.
2. **Basement / tectonic macro relief fields** create the island-scale asymmetry, old upland blocks, broad slope gradients, escarpments, and non-volcanic relief.
3. **Basin, ridge, outlet, and valley graph authority** controls catchments, drainage divides, valley floors, floodplains, saddles, and river mouths.
4. **Region-aware scalar-field composition** blends named landform regions without circular falloff seams.
5. **Noise, fBm, ridged multifractals, and domain warping** are restricted to texturing and mesoscale variation, not terrain authority.
6. **Physically motivated post-processes** add hydrologic coherence, stream-power incision, hillslope diffusion, slope relaxation, and coastal clamping.
7. **Old volcanic benches and secondary cones** are feature stamps and regional geology, not the island’s global frame.
8. **The active volcano becomes a compact subsystem** with strict footprint and contribution metrics.

The key acceptance test is `volcano_disabled_height_preview`: with the active volcano completely disabled, the island must still show named regions, uplands, lowlands, basin structure, drainage divides, valley floors, saddles, terraces, coastal fans, and plausible relief. The volcano-enabled version then adds a local volcanic subsystem on top of an already credible island.

References for this foundation come from hydrology-driven procedural terrain generation, hydraulic erosion modeling, stream-power incision geomorphology, DEM/geomorphometry practice, implicit distance-field modeling, and volcanic ocean-island evolution [R1]-[R10].

---

## Terrain-Only Scope Boundary

This document covers only:

- terrain mathematics;
- terrain generation architecture;
- deterministic source identity;
- preview/export evidence;
- non-player-facing validation metrics;
- code/tool areas likely touched.

This document does **not** design:

- foliage;
- ecology;
- weather;
- audio;
- NPCs;
- survival gameplay;
- combat;
- buildings;
- quests;
- final materials;
- UI;
- map, compass, minimap, GPS, or player-facing navigation overlays.

Terrain outputs may include developer-only region maps, drainage overlays, metric plots, and debug previews. These are validation artifacts, not game UI.

---

## Batch 004 Failure Evidence

Batch 004 is useful because it separated coastal success from interior failure.

### Proven preservation target

The rewrite must preserve the already-working acceptance gates:

```text
world_size_m remains 97536.0 unless explicitly justified
linear scale vs Batch 003 remains 6.0
area scale vs Batch 003 remains 36.0
shoreline_error_max_m must remain 0 or effectively 0
beach_continuity_pct must remain 100%
ocean_below_sea_pct must remain 100%
square_edge_ocean_violations must remain 0
outer world edge remains ocean/container terrain
island perimeter resolves through beach/coastal shelf/sea-level transition
```

### Structural failure to replace

The interior failure is not cosmetic:

```text
Batch 004 terrain reads as a procedural volcano preview, not a real island.
The volcano is the only dominant feature.
The island is controlled by crater-centered math.
Ridge and gully masks still read as turbine / flower / fan-blade structures.
The terrain lacks independent macro landforms, basins, valleys, plateaus,
coastal flats, saddles, and old volcanic history.
```

### Design consequence

The Batch 004 math should be treated as a failed organizing hierarchy, not as a base to tune. The coast can be preserved, but the interior authority should be replaced.

The generator must reject any terrain that can pass coastline metrics while still failing as a non-volcanic island.

---

## Required Terrain Authority Hierarchy

The correct hierarchy is:

```text
0. deterministic identity / config / units
1. island mass, coastline SDF, ocean container, beach/shelf fields
2. basement macro relief and uplift/tilt fields
3. regional landform regions and upland blocks
4. basin / ridge / outlet / valley graph authority
5. scalar-field composition for valley floors, divides, terraces, fans, saddles
6. constrained spectral/noise detail by region and process mask
7. old volcanic benches, eroded shields, secondary cones, lava benches
8. compact active volcano subsystem
9. hydrology/erosion/slope/coastal post-processing
10. preview export, metrics, hard gates, source fingerprint
```

### Authority rules

| Layer | Role | May dominate whole island? | Deterministic? | Reject if |
|---|---:|---:|---:|---|
| Coast SDF / island mass | Preserve land/ocean boundary | Yes, but only land/ocean | Yes | shoreline or ocean gates regress |
| Basement macro relief | First true interior terrain authority | Yes | Yes | volcano-disabled island is flat/noisy |
| Landform regions | Named terrain zones and broad morphology | Regionally | Yes | region IDs are decorative only |
| Basin/ridge graph | Drainage, divides, outlets, valleys | Regionally | Yes | graph does not control height |
| Noise/detail | Texture and local roughness | No | Yes | terrain shape disappears without noise |
| Old volcanics | Geological history, benches, cones | No | Yes | becomes radial global frame |
| Active volcano | Local volcano feature | No | Yes | contribution leaks over whole island |
| Erosion/post-process | Coherence and plausibility | No | Yes | moves shoreline or breaks coast |

### Major rule

The volcano must be a **child layer** of the island, never the island's parent coordinate system.

---

## Mathematical Foundation Overview

The foundation should use four complementary representations.

### 1. Field-based authority

Use implicit scalar fields and signed distance fields for:

- coastline and island mass;
- land/ocean/coast/beach/shelf masks;
- broad uplift and macro relief;
- region masks;
- escarpment bands;
- terrace bands;
- blending weights;
- feature-stamp footprints.

A signed distance field is especially suitable for coastline and region envelopes because it gives continuous distance-to-boundary information that can be clamped, blended, and debugged. Hart's distance-field work is a canonical implicit-surface reference for robust distance-based modeling [R6].

### 2. Graph-based authority

Use graphs for:

- major basin outlets;
- basin polygons;
- drainage divides;
- ridge spines;
- mainstem valley paths;
- tributary paths;
- saddles / passes;
- floodplain widening;
- coastal fans.

Hydrology-driven procedural terrain work shows why drainage networks are better treated as structural objects rather than emergent noise artifacts; Genevaux et al. model terrain from river networks and hydrology-inspired rules rather than relying on unconstrained fractal terrain alone [R1].

### 3. Process-based post-processing

Use deterministic, limited-step geomorphic post-processes for:

- hillslope diffusion;
- thermal/slope relaxation;
- stream-power incision;
- local deposition/fan smoothing;
- pit filling / route correction;
- coastal clamping.

Do not run uncontrolled erosion until the terrain "looks good." Use bounded, reproducible passes with metric outputs. Stream-power incision models represent river incision as a function of drainage area and slope, commonly written as `I = K A^m S^n`, with known limitations that should be respected rather than blindly simulated [R3], [R4].

### 4. Feature-stamp authority

Use feature stamps for localized historical geology:

- old volcanic benches;
- eroded secondary cones;
- tuff rings;
- lava deltas or lava benches;
- terrace treads;
- saddle cuts;
- landslide scars;
- alluvial/coastal fans.

Feature stamps must be constrained by region masks, SDF footprints, graph anchors, and blend operators. A stamp may create a recognizable landform, but it may not establish the whole island’s coordinate system.

---

## Island Mass And Coast Envelope Fields

The existing coast acceptance gates are the non-negotiable base. The rewrite must keep the land/ocean interface authority isolated from the interior relief rewrite.

### Unit constants

```cpp
constexpr double JG_WORLD_SIZE_M = 97536.0;
constexpr double JG_WORLD_HALF_EXTENT_M = 48768.0;
constexpr double JG_LINEAR_SCALE_VS_BATCH003 = 6.0;
constexpr double JG_AREA_SCALE_VS_BATCH003 = 36.0;
constexpr const char* JG_TERRAIN_MATH_VERSION = "JG_TERRAIN_MATH_CORE_005_001";
```

Useful preview-grid resolutions:

| Grid | Meters per sample across 97.536 km world | Use |
|---:|---:|---|
| 1024 | 95.25 m/px | fast CI smoke preview |
| 2048 | 47.625 m/px | broad morphology metrics |
| 4096 | 23.8125 m/px | default acceptance preview |
| 8192 | 11.90625 m/px | final visual/evidence preview |
| 16384 | 5.953125 m/px | expensive close inspection only |

World square area:

```text
97.536 km * 97.536 km = 9513.271296 km²
```

### Core fields

Use a small set of explicit fields:

```text
F_coast_sdf(x, y)          signed distance to preserved island shoreline
F_ocean_container(x, y)    square-edge ocean / outside-world safety
F_beach_band(x, y)         beach continuity mask
F_shelf_band(x, y)         coastal shelf / shallow sea transition
F_land_mask(x, y)          land if F_coast_sdf > 0, ocean if <= 0
F_coastal_clamp(x, y)      post-process clamp strength near sea level
```

Convention:

```text
F_coast_sdf > 0 = inside island land
F_coast_sdf = 0 = shoreline
F_coast_sdf < 0 = ocean / offshore
```

### Coast-preserving rule

Interior height functions may read `F_coast_sdf` as a distance-to-coast control, but they must not mutate the shoreline field.

Bad:

```text
final_land = coastline_from_volcano_radius(...)
```

Good:

```text
land_mask = F_coast_sdf > 0
interior_height = H_macro(...) + H_graph(...) + H_regions(...) + H_detail(...)
final_height = coastal_clamp(interior_height, F_coast_sdf)
```

### Coastal bands

Recommended default meter bands, to be tuned against existing Batch 004 evidence:

| Band | Distance from shoreline | Purpose |
|---|---:|---|
| `shoreline_lock` | -10 m to +10 m | exact shoreline, no relocation |
| `beach_band` | +10 m to +120 m | continuous beach / beach-lowland transition |
| `coastal_flat_candidate` | +120 m to +900 m | coastal plains, fans, lagoon backshore, low terraces |
| `coastal_slope_transition` | +900 m to +2400 m | transition from lowlands to regional terrain |
| `interior_free` | >2400 m | full interior authority |

These are not player-facing biomes. They are terrain-construction bands.

### Hard preservation gates

The new foundation fails immediately if any of these regress:

```text
world_size_m == 97536.0
linear_scale_vs_batch003 == 6.0
area_scale_vs_batch003 == 36.0
shoreline_error_max_m <= 0.25 m
beach_continuity_pct == 100.0
ocean_below_sea_pct == 100.0
square_edge_ocean_violations == 0
outer_world_edge_all_ocean == true
island_perimeter_has_beach_shelf_sea_transition == true
```

`shoreline_error_max_m <= 0.25 m` is an implementation tolerance for floating-point/sample-grid reporting. The target remains effectively zero.

---

## Macro Relief And Regional Upland Design

The island must be interesting before the volcano exists. Macro relief should therefore come from basement/uplift fields and landform regions.

### Required macro components

A credible 97.536 km volcanic jungle island needs:

- one to three old highland spines or upland blocks;
- multiple asymmetric lowland basins;
- coastal flats and river-mouth fans;
- interior saddles/passes between drainage basins;
- old volcanic benches and truncated surfaces;
- escarpments or resistant ridges;
- independent high points that are not the active volcano;
- a compact active volcano added later.

### Macro relief field

A basic field form:

```text
H_base(x, y) =
    H_island_dome(x, y)             // broad island emergence, not crater radial
  + H_uplift_spine(x, y)            // anisotropic highland belt(s)
  + H_upland_blocks(x, y)           // plateau/block relief
  + H_structural_tilt(x, y)         // old asymmetry
  + H_region_bias(x, y)             // named region base elevation
  - H_basin_bowls(x, y)             // broad basin depressions
```

`H_island_dome` may use distance from coastline or a low-order island SDF field, but it must not be a volcano-centered radial dome.

### Example non-radial macro primitives

#### Anisotropic spine

```text
d_axis = signed_distance_to_polyline(P, spine_polyline)
t_axis = normalized_distance_along_polyline(P, spine_polyline)

W_spine = smooth_exp(-(d_axis / width(t_axis))^2)
H_spine = amplitude(t_axis) * W_spine * coastal_fade(F_coast_sdf)
```

Properties:

- follows a curved, seeded spine polyline;
- width varies along the spine;
- amplitude varies along the spine;
- may branch into upland blocks;
- unrelated to crater radius.

#### Upland block

```text
F_block = sdf_rounded_polygon(P, block_polygon)
W_block = smoothstep(-blend_m, +core_m, F_block)
H_block = plateau_height * W_block + edge_escarpment(F_block)
```

Properties:

- uses SDF to define block envelope;
- permits escarpments on one or more sides;
- can be cut by basin graph valleys.

#### Structural tilt

```text
H_tilt = dot(P - P0, normalized_tilt_vector) * tilt_gradient_m_per_m
```

Recommended magnitude:

```text
tilt_gradient_m_per_km = 1.5 to 6.0 m/km
```

This is enough to bias drainage and asymmetry without creating an obvious ramp.

### Macro region targets

Compute these over `land_mask == true`, excluding ocean and optionally excluding the locked shoreline band.

| Metric | Target |
|---|---:|
| Named terrain regions | 8-14 |
| Major upland blocks/spines | 2-5 |
| Major drainage basins | 8-16 |
| Largest non-ocean region | <= 25% of land area |
| Largest basin | <= 24% of land area |
| Upland not connected to active volcano | >= 35% of highland area |
| Active volcano local footprint | <= 6% of land area at contribution weight > 0.25 |
| Active volcano strong footprint | <= 3% of land area at contribution weight > 0.50 |

### Elevation ranges

Use target ranges rather than fixed single values:

| Elevation class | Height above sea level | Land-area target |
|---|---:|---:|
| Beach / immediate shore | 0-8 m | existing beach gate, 100% continuous |
| Coastal flats / low terraces | 8-80 m | 8-18% |
| Lowlands | 0-120 m | 22-38% |
| Lower midlands | 120-350 m | 18-30% |
| Upper midlands | 350-650 m | 16-28% |
| Highlands | 650-1100 m | 10-22% |
| Non-volcanic high peaks | 900-1300 m | 1-6% |
| Active volcano summit zone | configurable, typically 1300-2100 m | <= 3% strong footprint |

The active volcano can be the tallest local feature, but it must not be the only source of high terrain.

---

## Basin / Ridge / Valley Graph Authority

A terrain graph is required because drainage and divides are structural, not decorative. A coherent island should have valleys that know their outlets, ridges that separate basins, and valley floors that widen downstream.

Hydrologic terrain generation research supports treating river networks as construction authority rather than incidental detail [R1]. DEM hydrology practice also treats flow direction, flow accumulation, and watershed delineation as core terrain descriptors, with D8, multiple-flow, and D-infinity methods used to derive drainage networks from gridded elevation [R8].

### Graph objects

Use at least four related graph layers:

```text
G_outlets       coastal outlets and river mouths
G_basins        major basin polygons and parent/child basins
G_valleys       mainstem and tributary valley paths
G_divides       ridge/divide paths separating basins
```

### Basin generation sequence

1. Select outlet candidates on the coastline field.
2. Reject outlets that are too evenly angular around a volcano center.
3. Seed major basin anchors inward from coastline, biased by macro relief and region masks.
4. Build basin polygons by weighted Voronoi / power diagram in an anisotropic distance metric.
5. Derive divide paths from basin adjacency boundaries.
6. Generate valley mainstems from highland source zones to outlets.
7. Add tributaries where local relief and catchment area justify them.
8. Carve valley floors and floodplains into the scalar field.
9. Validate that flow routing from final height reaches the intended outlet or ocean.

### Basin count and area targets

For a 97.536 km world, target island-scale basins like this:

| Metric | Target |
|---|---:|
| Major basins | 8-16 |
| Secondary basins | 20-45 |
| Basin outlets on coast | >= 8 |
| No outlet fan sector repetition | pass angular spectrum gate |
| Largest basin area | <= 24% land area |
| Smallest major basin area | >= 2.5% land area |
| Basin area coefficient of variation | 0.35-0.90 |
| Inland endorheic basins | 0 by default, <= 1 special case if explicitly configured |
| Flow cells reaching ocean | >= 97% of land cells after pit correction |
| Unresolved pits outside wetlands | <= 0.25% land cells |

### Ridge and divide requirements

A ridge divide is not just a ridged-noise mask. It must be associated with basin separation.

Each major divide should have:

```cpp
struct FJGRidgeDivideEdge
{
    int32 DivideId;
    int32 BasinLeftId;
    int32 BasinRightId;
    TArray<FVector2D> PolylineMeters;
    double CrestBaseElevationM;
    double CrestAmplitudeM;
    double HalfWidthM;
    double Asymmetry;          // -1..+1 cross-slope bias
    double SaddleProbability;
    uint64 LocalSeed;
};
```

Ridge height contribution should be strongest at divides and fade anisotropically into adjacent basins:

```text
d = signed_distance_to_divide(P)
W = exp(-(abs(d) / width)^p)
H_ridge = crest_profile(t) * W * basin_context_weight
```

Use `p = 1.5..3.0`. Avoid perfect Gaussian sameness by varying width/profile along the divide.

### Valley authority

A valley should be defined by path distance and downstream coordinate:

```text
d_v = distance_to_valley_polyline(P)
t_v = normalized_distance_downstream(P)
A_v = upstream_area_proxy(t_v)
w_v = valley_width(A_v, slope, region)
D_v = incision_depth(A_v, slope, region)
```

Valley floor shape:

```text
cross_section = smooth_U_or_flat_bottom(d_v / w_v)
H_valley_cut = -D_v * cross_section * region_mask
```

Downstream widening:

```text
w_v(t) = w_source + k_width * sqrt(max(A_v(t), 0))
```

Recommended floor widths:

| Feature | Width |
|---|---:|
| steep headwater channel | 10-50 m |
| small tributary valley | 60-180 m |
| major interior valley | 180-600 m |
| lower floodplain | 400-1400 m |
| coastal fan / river mouth | 800-2500 m |

At default 4096 preview resolution, widths below roughly one pixel are invisible. Metrics should therefore be computed at 4096 or higher for valley-floor evidence.

### Saddles and passes

Saddles should occur where divide edges are intentionally lowered:

```text
H_saddle = -saddle_drop_m * exp(-(d_to_saddle_center / radius_m)^2)
```

Targets:

| Metric | Target |
|---|---:|
| Major named saddles/passes | 4-10 |
| Saddles connecting adjacent basins | >= 70% of saddles |
| Saddle drop from adjacent ridge crest | 60-280 m |
| Saddle width | 150-900 m |
| Saddles within active volcano strong footprint | <= 1 unless explicitly volcanic |

---

## Scalar Field Composition And Blending Rules

The generator should evaluate terrain as a composition of named contributors. Every contributor must be inspectable.

### Evaluation form

```text
H_raw(P) =
    H_coastal_base(P)
  + H_macro_relief(P)
  + H_regions(P)
  + H_basin_graph(P)
  + H_valley_floors(P)
  + H_ridges_divides(P)
  + H_old_volcanics(P)
  + H_active_volcano(P)
  + H_detail(P)

H_final(P) =
    coastal_clamp(
      slope_relax(
        hydrology_correct(
          H_raw(P)
        )
      ),
      F_coast_sdf(P)
    )
```

### Contributor debug map

Every sampled point should optionally output:

```cpp
struct FJGTerrainContribSample
{
    double CoastBaseM;
    double MacroReliefM;
    double RegionBiasM;
    double BasinBowlM;
    double ValleyCutM;
    double RidgeDivideM;
    double OldVolcanicM;
    double ActiveVolcanoM;
    double DetailNoiseM;
    double PostProcessDeltaM;
    int32 LandformRegionId;
    int32 BasinId;
    int32 DominantContributorId;
};
```

### Blend rules

Use SDF and normalized weighted sums, not stacked circular falloffs.

Bad blend:

```text
height += cone_height * smoothstep(radius - distance_to_center)
```

Good blend:

```text
region_weight_i = normalized_softmax(region_field_i / temperature)
height = sum(region_weight_i * region_height_i)
```

or:

```text
height = smooth_union_sdf_controlled(height_a, height_b, boundary_sdf, blend_width)
```

### Required blend invariants

| Invariant | Gate |
|---|---:|
| No height discontinuity at region boundary | max sampled step <= 2 m at 4096 preview |
| No slope discontinuity seam | boundary gradient jump <= regional 95th percentile * 1.5 |
| No visible circular stamp edge | circularity score below threshold |
| No repeated turbine sector | angular harmonic gate pass |
| Region has terrain effect | region contribution variance > minimal threshold |
| Noise not primary shape | disabling detail noise preserves region/basin structure |

### Circular falloff rejection

Feature masks should avoid obvious isotropic circles unless the real landform is circular, such as a cinder cone or tuff ring. Even then, the active circular feature must be local.

For non-circular landforms, use:

- polyline SDFs;
- polygon SDFs;
- anisotropic distance metrics;
- warped local coordinates;
- graph-aligned coordinates;
- region boundary fields;
- erosion masks;
- asymmetric cross-sections.

### Softmax region composition

```text
W_i(P) = exp(F_i(P) / τ) / Σ_j exp(F_j(P) / τ)
H_regions(P) = Σ_i W_i(P) * H_i(P)
```

Recommended:

```text
τ = 150 m to 1200 m equivalent field temperature
```

Low `τ` gives hard boundaries; high `τ` makes mush. Track seam metrics.

### Smooth minimum / maximum use

Smooth min/max operators are valid for SDF blending, but they should be reserved for local operations. Do not use a repeated smooth-min stack that creates a blobby procedural island.

---

## Noise, Domain Warping, And Multifractal Use

Noise is necessary but insufficient. Fractal terrain research shows noise and erosion can synthesize plausible roughness and valleys, but unconstrained noise lacks hydrologic and geological intent [R5]. Hydrology-based procedural terrain research explicitly addresses this by representing river networks and terrain patches as structural inputs [R1].

### Allowed noise roles

Use noise for:

- local roughness;
- drainage texture;
- small ravines;
- cliff roughening;
- terrace edge breakup;
- old lava surface unevenness;
- slope-dependent microvariation;
- region-specific spectral character.

Do not use noise for:

- main island mass;
- coastline identity;
- basin topology;
- primary ridge system;
- major valleys;
- active volcano global organization.

### Recommended fields

| Field | Use |
|---|---|
| fBm | broad-to-local natural roughness |
| ridged multifractal | ridge texture and dissected highlands |
| hybrid multifractal | rough volcanic / old shield textures |
| domain warp | break straight/regular edges |
| curl/noise vector warp | lateral valley/ridge irregularity |
| blue-noise seeds | feature/stamp distribution |
| low-order spectral field | broad asymmetry only, not final authority |

### Region-specific spectral controls

```cpp
struct FJGRegionNoiseProfile
{
    double BaseAmplitudeM;
    double Lacunarity;
    double Gain;
    int32 Octaves;
    double DomainWarpM;
    double RidgeNoiseWeight;
    double ValleyNoiseWeight;
    double SlopeMaskMinDeg;
    double SlopeMaskMaxDeg;
    uint64 LocalSeed;
};
```

Examples:

| Region | Noise amplitude | Ridge noise | Warp | Notes |
|---|---:|---:|---:|---|
| coastal flats | 0.5-6 m | low | low | must stay navigable/flat-looking |
| floodplains | 1-10 m | very low | medium | no noisy channel mess |
| old uplands | 15-80 m | medium | medium | dissected but structured |
| escarpments | 20-120 m | high | medium | rough faces, controlled crest |
| old lava benches | 5-35 m | medium | low-medium | broken but bench-readable |
| active cone | 10-80 m | radial locally allowed | low-medium | local only |

### Noise-disable gate

Run a preview with:

```text
detail_noise_enabled = false
active_volcano_enabled = false
```

Pass if:

- coastline still passes;
- region map still readable;
- basins still route to outlets;
- macro uplands/lowlands still visible;
- no turbine/radial structure appears.

This proves the foundation does not depend on noise to hide bad authority.

---

## Hypsometry, Relief, And Slope Distribution Targets

Geomorphometry treats gridded elevation models as the usual input for quantitative terrain analysis [R7]. Jungle Game should apply DEM-style metrics to its generated height previews.

### Hypsometric curve

The hypsometric curve should not be a simple cone profile. It should show:

- substantial lowland/coastal area;
- broad mid-elevation area;
- limited high summit area;
- non-volcanic highland area even with volcano disabled.

Compute normalized elevation:

```text
z_norm = clamp((z - sea_level_m) / (z_99_land - sea_level_m), 0, 1)
```

Compute cumulative area:

```text
A(z) = fraction_of_land_area_with_elevation <= z
```

### Recommended hypsometric targets

For `volcano_disabled_height_preview`:

| Percentile | Target elevation |
|---:|---:|
| P05 | 3-20 m |
| P10 | 8-45 m |
| P25 | 50-160 m |
| P50 | 180-420 m |
| P75 | 430-780 m |
| P90 | 700-1050 m |
| P95 | 850-1250 m |
| P99 | 1050-1450 m |

For active-volcano-enabled preview:

| Percentile | Target elevation |
|---:|---:|
| P95 | may increase by 0-250 m |
| P99 | may increase by 150-700 m |
| Max | 1300-2100 m unless config says otherwise |

The key is that the volcano should affect the upper tail, not the entire distribution.

### Area-class targets

Compute over land cells:

| Class | Elevation | Volcano disabled target | Volcano enabled target |
|---|---:|---:|---:|
| coastal/beach | 0-8 m | fixed by coast gates | fixed by coast gates |
| lowland | 0-120 m | 22-38% | 20-36% |
| lower midland | 120-350 m | 18-30% | 17-29% |
| upper midland | 350-650 m | 16-28% | 15-27% |
| highland | >650 m | 10-22% | 11-25% |
| very high | >1100 m | 0.5-6% | 1-9% |
| active volcano strong footprint | contribution weight >0.50 | 0% | <=3% |

### Relief targets

| Metric | Target |
|---|---:|
| Land relief P99-P01, volcano disabled | 900-1450 m |
| Land relief P99-P01, volcano enabled | 1200-2100 m |
| Non-volcanic high local maxima | >= 4 |
| Independent highland centers outside volcano footprint | >= 3 |
| Major escarpment relief | 150-600 m |
| Valley incision depth, major valleys | 80-450 m |
| Coastal fan gradient | 0.2°-4.0° |
| Floodplain gradient | 0.05°-2.0° |

### Slope distribution targets

Compute slope in degrees from the final height raster at 4096 and optionally 8192. Report with and without coastline/beach band.

| Slope bin | Target land-area share |
|---|---:|
| 0°-3° | 12-25% |
| 3°-8° | 20-35% |
| 8°-15° | 22-35% |
| 15°-25° | 15-28% |
| 25°-35° | 5-15% |
| >35° | 0.5-6% |
| >45° | <=1.5%, cliffs/scars only |

Slope failure relaxation should reduce impossible spikes while preserving real escarpment faces.

---

## Volcano-Disabled Island Acceptance Test

The most important new gate is a full terrain preview with the active volcano removed.

### Required config

```json
{
  "terrain_math_version": "JG_TERRAIN_MATH_CORE_005_001",
  "world_size_m": 97536.0,
  "active_volcano_enabled": false,
  "old_volcanic_history_enabled": true,
  "basin_graph_enabled": true,
  "macro_relief_enabled": true,
  "detail_noise_enabled": true,
  "coastal_constraints_enabled": true
}
```

### Required output file family

```text
volcano_disabled_height_preview.exr
volcano_disabled_height_preview.png
volcano_disabled_slope_preview.png
volcano_disabled_hypsometry.csv
volcano_disabled_metrics.json
volcano_disabled_region_id.png
volcano_disabled_basin_id.png
volcano_disabled_ridge_valley_overlay.png
volcano_disabled_acceptance_report.md
```

### Acceptance criteria

The volcano-disabled island passes only if:

```text
coast gates pass
named landform regions >= 8
major basins between 8 and 16
major basin outlets >= 8
largest basin <= 24% land area
land cells routed to ocean >= 97%
unresolved non-wetland pits <= 0.25% land cells
lowlands 0-120 m are 22-38% land area
highlands >650 m are 10-22% land area
non-volcanic high local maxima >= 4
major saddle/pass count >= 4
major valley floor count >= 8
coastal fan / river-mouth lowland count >= 5
region_id coverage over land == 100%
dominant non-ocean region <= 25% land area
radial/turbine gates pass
```

### Visual acceptance

A reviewer should be able to hide all volcano layers and still identify:

- at least two broad highland systems;
- several basins draining to different coast sectors;
- valley floors that widen downstream;
- ridge divides between basins;
- low coastal plains/fans;
- saddles/passes between uplands;
- old volcanic benches or eroded features;
- a real island silhouette with preserved coast.

If the disabled preview reads as "flat island with noise" or "same volcano with summit cut off," fail.

---

## Determinism, Seeds, Config, And Fingerprints

Determinism is a core requirement. Every preview must be reproducible from source, config, and seed.

### Required identity object

```cpp
struct FJGTerrainIdentity
{
    FString MathVersion;          // "JG_TERRAIN_MATH_CORE_005_001"
    FString ConfigSchemaVersion;  // e.g. "JG_TERRAIN_CONFIG_SCHEMA_005_001"
    uint64 MasterSeed;
    FString NormalizedConfigHash; // SHA-256 of canonical JSON/config
    FString SourceFingerprint;    // git commit or source hash
    FString CoastFingerprint;     // preserved coastline/coast field identity
    FString BuildPlatform;        // Linux editor/exporter, CI, etc.
};
```

### Seed splitting

Use stable named seed streams:

```text
seed.coast_preserved
seed.macro_relief
seed.region_layout
seed.basin_outlets
seed.basin_graph
seed.ridge_divides
seed.valley_paths
seed.old_volcanics
seed.active_volcano
seed.noise_detail
seed.erosion_postprocess
seed.preview_sampling
```

Seed streams must be derived with a stable hash, not random runtime order.

Example:

```text
SeedStream(name) = xxhash64(master_seed || math_version || normalized_config_hash || name)
```

### Deterministic config requirements

- No dependency on wall-clock time.
- No dependency on unordered map iteration order.
- No platform-dependent random implementation.
- No editor-only random state.
- Floating-point tolerances declared.
- Preview resolution recorded.
- All metrics exported with source fingerprint.
- All acceptance reports include the exact config hash.

### Recommended fingerprint JSON

```json
{
  "terrain_math_version": "JG_TERRAIN_MATH_CORE_005_001",
  "config_schema_version": "JG_TERRAIN_CONFIG_SCHEMA_005_001",
  "master_seed": 5005001,
  "world_size_m": 97536.0,
  "linear_scale_vs_batch003": 6.0,
  "area_scale_vs_batch003": 36.0,
  "source_fingerprint": "<git-or-source-hash>",
  "coast_fingerprint": "<coast-field-hash>",
  "normalized_config_sha256": "<sha256>",
  "preview_resolution": 4096,
  "meters_per_sample": 23.8125,
  "build_platform": "linux",
  "active_volcano_enabled": false
}
```

---

## Preview And Developer Evidence Requirements

The preview exporter should become a terrain evidence tool, not just an image renderer.

### Required previews

For each acceptance run, export both volcano-disabled and volcano-enabled variants.

```text
height_preview.exr
height_preview.png
slope_preview.png
aspect_preview.png
curvature_preview.png
landform_region_id.png
basin_id.png
ridge_divide_overlay.png
valley_graph_overlay.png
outlet_points_overlay.png
contribution_dominance.png
coast_distance_field.png
coast_band_debug.png
hypsometry_plot.png
slope_histogram.png
radial_turbine_spectrum.png
acceptance_report.md
metrics.json
fingerprint.json
```

### Region ID map

`landform_region_id` is required. It should be a deterministic integer map over land cells.

Suggested region taxonomy:

| ID range | Region type |
|---:|---|
| 0 | ocean |
| 1 | locked beach/coast |
| 2-19 | named lowland/coastal regions |
| 20-39 | basin/floodplain regions |
| 40-59 | upland/highland blocks |
| 60-79 | escarpment/ridge regions |
| 80-99 | old volcanic benches/cones |
| 100-119 | active volcano local subsystem |
| 120+ | reserved/debug |

Example named regions:

```text
Northwest Windward Uplands
Old East Shield Bench
Southwest Coastal Flats
Central Saddle Basin
North River Lowlands
Eastern Escarpment Block
South Fan Plain
Interior Cloud Ridge
Old Cone Field
Active Volcano Core
```

### Metrics JSON

Minimum schema:

```json
{
  "coast": {
    "shoreline_error_max_m": 0.0,
    "beach_continuity_pct": 100.0,
    "ocean_below_sea_pct": 100.0,
    "square_edge_ocean_violations": 0
  },
  "hypsometry": {
    "p05_m": 0,
    "p10_m": 0,
    "p25_m": 0,
    "p50_m": 0,
    "p75_m": 0,
    "p90_m": 0,
    "p95_m": 0,
    "p99_m": 0
  },
  "area_classes": {
    "lowland_0_120_pct": 0,
    "midland_120_650_pct": 0,
    "highland_gt_650_pct": 0,
    "very_high_gt_1100_pct": 0
  },
  "basins": {
    "major_basin_count": 0,
    "major_outlet_count": 0,
    "largest_basin_pct": 0,
    "ocean_routed_land_pct": 0,
    "unresolved_pit_pct": 0
  },
  "regions": {
    "named_region_count": 0,
    "land_region_coverage_pct": 0,
    "largest_region_pct": 0
  },
  "volcano": {
    "active_enabled": false,
    "strong_footprint_pct": 0,
    "weak_footprint_pct": 0,
    "max_contribution_m": 0
  },
  "artifact_rejection": {
    "volcano_radius_height_correlation": 0,
    "ridge_orientation_entropy": 0,
    "max_angular_harmonic_ratio": 0,
    "turbine_score": 0,
    "circular_stamp_edge_score": 0
  }
}
```

### Evidence report

The Markdown report should include:

- pass/fail table;
- config identity and fingerprint;
- exact preview paths;
- all coast gates;
- volcano-disabled gates;
- region/basin counts;
- hypsometry;
- slope histogram;
- artifact rejection metrics;
- top 10 largest regions;
- top 10 largest basins;
- known risks/failures.

---

## Files And Areas Likely Touched

Exact repository names must be confirmed before editing. Based on the described Jungle Game terrain source and preview exporter, likely touched areas are:

### Terrain source

```text
Source/JungleGame/Terrain/
Source/JungleGame/Terrain/JGTerrainConfig.*
Source/JungleGame/Terrain/JGTerrainMathVersion.*
Source/JungleGame/Terrain/JGIslandTerrainSource.*
Source/JungleGame/Terrain/JGCoastlineFields.*
Source/JungleGame/Terrain/JGHeightSampler.*
Source/JungleGame/Terrain/JGVolcanoTerrain.*
Source/JungleGame/Terrain/JGTerrainRegion.*
Source/JungleGame/Terrain/JGLandformGraph.*
Source/JungleGame/Terrain/JGBasinGraph.*
Source/JungleGame/Terrain/JGRidgeValleyGraph.*
Source/JungleGame/Terrain/JGFeatureStamp.*
Source/JungleGame/Terrain/JGTerrainContribDebug.*
```

### Preview/export tooling

```text
Tools/TerrainPreviewExporter/
Tools/TerrainPreviewExporter/terrain_preview_exporter.*
Tools/TerrainPreviewExporter/metrics/
Tools/TerrainPreviewExporter/render/
Scripts/terrain_preview_exporter.py
Scripts/export_terrain_acceptance.py
```

### Tests / validation

```text
Tests/Terrain/
Tests/Terrain/test_coast_acceptance.*
Tests/Terrain/test_terrain_determinism.*
Tests/Terrain/test_volcano_disabled_preview.*
Tests/Terrain/test_basin_graph.*
Tests/Terrain/test_hypsometry.*
Tests/Terrain/test_radial_artifact_rejection.*
```

### Config / generated evidence

```text
Config/Terrain/JG_TERRAIN_MATH_CORE_005_001.json
Saved/TerrainEvidence/PR5_Batch005/
Saved/TerrainEvidence/PR5_Batch005/volcano_disabled/
Saved/TerrainEvidence/PR5_Batch005/volcano_enabled/
```

### Important source-control rule

Do not rename or mutate the working coast source until all preservation gates are locked by tests. The interior rewrite should be staged so that coast fingerprints can be compared before and after.

---

## Large PR Implementation Scope

This is a large PR, not a patch. Recommended scope:

### Phase 1 — identity, config, and coastline lock

- Add `JG_TERRAIN_MATH_CORE_005_001`.
- Freeze existing coast field fingerprint.
- Add deterministic identity/fingerprint output.
- Add no-op preview exporter changes that report existing coast gates.

Exit only when current coast acceptance remains green.

### Phase 2 — macro relief and region maps

- Add field-based macro relief independent of volcano.
- Add named `landform_region_id`.
- Add region debug preview.
- Add volcano-disabled height preview.
- Add hypsometry and slope metrics.

Exit only when volcano-disabled preview has readable macro island form.

### Phase 3 — basin/ridge/valley graph

- Add outlet, basin, divide, valley graph data structures.
- Add deterministic basin generation.
- Add ridge divide and valley floor height contribution.
- Export basin/ridge/valley overlays.
- Add routing metrics.

Exit only when basin graph visibly controls the heightfield.

### Phase 4 — old volcanic history and compact active volcano

- Add old volcanic benches, secondary cones, eroded shields.
- Re-introduce active volcano as compact subsystem.
- Add volcano contribution maps.
- Add footprint gates.

Exit only when volcano-enabled preview improves terrain without taking over.

### Phase 5 — post-processes and artifact rejection

- Add bounded hydrology correction, incision, diffusion, slope relaxation.
- Add angular/radial/turbine artifact metrics.
- Add acceptance reports.
- Add CI-compatible preview smoke tests.

Exit only when all gates pass at target preview resolution.

---

## Acceptance Criteria

### Hard coast criteria

```text
world_size_m == 97536.0
linear_scale_vs_batch003 == 6.0
area_scale_vs_batch003 == 36.0
shoreline_error_max_m <= 0.25
beach_continuity_pct == 100.0
ocean_below_sea_pct == 100.0
square_edge_ocean_violations == 0
outer_world_edge_all_ocean == true
```

### Determinism criteria

```text
same seed + same config + same source fingerprint = byte-identical metrics JSON
same seed + same config + same source fingerprint = visually identical preview raster within declared image encoding tolerance
all seed streams named and fingerprinted
no wall-clock/random-device dependency
```

### Volcano-disabled criteria

```text
active_volcano_enabled == false
active_volcano_contribution_max_m == 0
coast gates pass
major_basins = 8..16
named_landform_regions >= 8
major_valley_floors >= 8
major_saddles >= 4
coastal_fans >= 5
lowland_0_120_pct = 22..38
highland_gt_650_pct = 10..22
non_volcanic_high_local_maxima >= 4
largest_basin_pct <= 24
largest_region_pct <= 25
ocean_routed_land_pct >= 97
```

### Volcano-enabled criteria

```text
active_volcano_strong_footprint_pct <= 3
active_volcano_weak_footprint_pct <= 6
volcano_radius_height_correlation_outside_footprint <= 0.35 absolute
non_volcanic_highland_area_retained_pct >= 85 of disabled run
major_basin_count unchanged or changes by <= 1
coast gates pass unchanged
```

### Artifact rejection criteria

```text
ridge_orientation_entropy >= 0.65 normalized
max_angular_harmonic_ratio <= 2.25
turbine_score <= configured threshold
flower_blade_sector_count_detected == 0
dominant_crater_radial_control == false
circular_falloff_edge_score <= configured threshold
detail_noise_disabled_preview_still_structured == true
```

### Preview evidence criteria

```text
height preview exported
slope preview exported
region_id map exported
basin_id map exported
ridge/valley overlay exported
hypsometry exported
slope histogram exported
radial/turbine spectrum exported
metrics JSON exported
fingerprint JSON exported
acceptance report exported
```

---

## Risks And Stop Conditions

### Stop immediately if coast gates regress

Any shoreline, beach, ocean, shelf, or square-edge regression stops the PR. Do not continue interior work until the coast fingerprint and metrics are restored.

### Stop if volcano remains global authority

If disabling the volcano removes the island's primary relief, the hierarchy is still wrong.

### Stop if graph is decorative

If basin/valley/ridge overlays do not match heightfield forms, graph authority has not actually been integrated.

### Stop if noise hides structure

If disabling detail noise destroys the island shape, noise is doing authority work and the foundation is invalid.

### Stop if radial artifacts pass by visual luck

The generator must include numeric radial/turbine rejection gates. Visual review alone is not enough.

### Stop if determinism is unproven

Every preview and metrics report must be reproducible from seed, config, source fingerprint, and coast fingerprint.

### Technical risks

| Risk | Mitigation |
|---|---|
| Field blending creates mush | enforce region/basin dominance and seam metrics |
| Basin graph fights macro relief | generate graph after macro relief, then feed graph back into height |
| Erosion destroys coast | apply coastal clamp after erosion and lock shoreline band |
| Too many knobs | versioned config schema with defaults and metric reports |
| Active volcano dominates again | hard footprint and contribution gates |
| Preview too slow | 1024/2048 smoke path, 4096 acceptance, 8192 final evidence |
| Source architecture grows too fast | phase implementation and keep coast tests first |

---

## Rejected Or Forbidden Approaches

### Forbidden

- Using crater-centered radial coordinates as the global terrain frame.
- Making volcano radius the primary control for island elevation.
- Tuning Batch 004 fan-blade/ridge masks instead of replacing authority.
- Passing coast gates while ignoring interior geomorphology.
- Treating noise as the main terrain structure.
- Generating ridges as repeated angular sectors from a center point.
- Applying circular falloff stamps everywhere.
- Letting erosion/post-process move the shoreline.
- Accepting a terrain with no volcano-disabled island preview.
- Accepting a terrain with no deterministic source/config fingerprint.

### Rejected as insufficient

- "Add more noise."
- "Warp the radial volcano masks."
- "Lower the crater."
- "Make gullies less symmetrical."
- "Add a second cone and call it old history."
- "Paint region colors without region authority."
- "Use hydraulic erosion as a black-box beautifier."
- "Review only by screenshot."

### Allowed only locally

- Radial coordinates for the active crater/cone subsystem.
- Circular SDFs for cinder cones, tuff rings, crater rims, and localized volcanic stamps.
- Ridged noise for local ridge texture after graph/divide authority exists.
- Domain warping to break edges after the base landform is valid.

---

## Sources And References

[R1] Jean-David Génevaux, Éric Galin, Éric Guérin, Adrien Peytavie, Bedřich Beneš. "Terrain Generation Using Procedural Models Based on Hydrology." *ACM Transactions on Graphics*, 32(4), 2013. DOI: `10.1145/2461912.2461996`.  
https://www.cs.purdue.edu/cgvlab/www/resources/papers/Genevaux-ACM_Trans_Graph-2013-Terrain_Generation_Using_Procedural_Models_Based_on_Hydrology.pdf

[R2] Bedřich Beneš, Rafael Forsbach. "Visual Simulation of Hydraulic Erosion." *Journal of WSCG*, 2002.  
https://www.cs.purdue.edu/cgvlab/www/resources/papers/Benes-2002-Visual_simulation_of_hydraulic_erosion.pdf

[R3] Kelin X. Whipple, Gregory E. Tucker. "Dynamics of the stream-power river incision model: Implications for height limits of mountain ranges, landscape response timescales, and research needs." *Journal of Geophysical Research*, 1999. DOI: `10.1029/1999JB900120`.  
https://sseh.uchicago.edu/doc/Whipple_and_Tucker_1999.pdf

[R4] Dimitri Lague. "The stream power river incision model: evidence, theory and beyond." *Earth Surface Processes and Landforms*, 39, 2014. DOI: `10.1002/esp.3462`.  
https://wpg.forestry.oregonstate.edu/sites/default/files/seminars/2014_Lague_ESPL.pdf

[R5] F. Kenton Musgrave, Craig E. Kolb, Robert S. Mace. "The synthesis and rendering of eroded fractal terrains." *SIGGRAPH Computer Graphics*, 23(3), 1989. DOI: `10.1145/74334.74337`.  
https://dl.acm.org/doi/10.1145/74334.74337

[R6] John C. Hart. "Sphere tracing: A geometric method for the antialiased ray tracing of implicit surfaces." *The Visual Computer*, 12(10), 1996. DOI: `10.1007/s003710050084`.  
https://experts.illinois.edu/en/publications/sphere-tracing-a-geometric-method-for-the-antialiased-ray-tracing/

[R7] Richard J. Pike, Ian S. Evans, Tom Hengl. "Geomorphometry: A Brief Guide." In *Geomorphometry: Concepts, Software, Applications*, 2008.  
https://www.geomorphometry.org/uploads/pdf/Pike_2008_Geomorphometry_ch1.pdf

[R8] David G. Tarboton. "A new method for the determination of flow directions and upslope areas in grid digital elevation models." *Water Resources Research*, 33(2), 1997. DOI: `10.1029/96WR03137`.  
https://saga-gis.sourceforge.io/saga_tool_doc/9.0.3/ta_hydrology_1.html

[R9] David A. Clague, David R. Sherrod. "Growth and degradation of Hawaiian volcanoes." USGS Professional Paper 1801, 2014.  
https://pubs.usgs.gov/publication/pp18013

[R10] Ricardo S. Ramalho, Rui Quartau, Alan S. Trenhaile, Neil C. Mitchell, Colin D. Woodroffe, Sérgio P. Ávila. "Coastal evolution on volcanic oceanic islands: A complex interplay between volcanism, erosion, sedimentation, sea-level change and biogenic production." *Earth-Science Reviews*, 127, 2013. DOI: `10.1016/j.earscirev.2013.10.007`.  
https://research-information.bris.ac.uk/en/publications/coastal-evolution-on-volcanic-oceanic-islands-a-complex-interplay/

[R11] Paul Prusinkiewicz, Mark Hammel. "A Fractal Model of Mountains with Rivers." *Graphics Interface*, 1993.  
https://algorithmicbotany.org/papers/mountains.gi93.pdf

[R12] National Park Service. "Shield Volcanoes." 2023.  
https://www.nps.gov/articles/000/shield-volcanoes.htm

[R13] USGS. "The origin of rejuvenation-stage volcanism is still poorly understood." Volcano Watch.  
https://www.usgs.gov/news/volcano-watch-origin-rejuvenation-stage-volcanism-still-poorly-understood

[R14] M. P. Lamb, W. E. Dietrich, L. S. Sklar. "Formation of amphitheater-headed valleys by waterfall erosion after large-scale slumping on Hawai'i." *Geological Society of America Bulletin*, 2007.  
https://lamb.caltech.edu/documents/19593/Lamb_et_al_GSA_2007.pdf

---

## Appendix A — Proposed Minimal C++ Data Structures

```cpp
enum class EJGLandformRegionType : uint8
{
    Ocean,
    LockedCoast,
    CoastalFlat,
    Floodplain,
    BasinLowland,
    UplandBlock,
    HighlandSpine,
    Escarpment,
    Saddle,
    OldVolcanicBench,
    SecondaryConeField,
    ActiveVolcanoCore
};

struct FJGLandformRegion
{
    int32 Id;
    FString Name;
    EJGLandformRegionType Type;
    TArray<FVector2D> BoundaryPolygonMeters;
    double MinElevationM;
    double MaxElevationM;
    double BlendWidthM;
    double RoughnessAmplitudeM;
    double DrainageBias;
    uint64 LocalSeed;
};

enum class EJGLandformNodeType : uint8
{
    UplandPeak,
    UplandBlock,
    BasinCenter,
    BasinOutlet,
    ValleyConfluence,
    RidgeJunction,
    SaddlePass,
    CoastalFan,
    OldCone,
    OldBench,
    ActiveCone
};

struct FJGLandformNode
{
    int32 Id;
    EJGLandformNodeType Type;
    int32 RegionId;
    FVector2D PositionM;
    double ElevationTargetM;
    double RadiusM;
    double Strength;
    uint64 LocalSeed;
};

struct FJGBasinNode
{
    int32 BasinId;
    FString Name;
    int32 ParentBasinId;
    int32 OutletNodeId;
    TArray<int32> ChildBasinIds;
    TArray<FVector2D> BasinPolygonM;
    double AreaKm2;
    double MeanElevationM;
    double ReliefM;
    double TargetDrainageDensity;
};

struct FJGValleyEdge
{
    int32 EdgeId;
    int32 BasinId;
    int32 UpstreamNodeId;
    int32 DownstreamNodeId;
    TArray<FVector2D> PolylineM;
    double SourceElevationM;
    double MouthElevationM;
    double InitialWidthM;
    double FinalWidthM;
    double MaxIncisionM;
    double FloodplainWidthM;
    uint64 LocalSeed;
};

struct FJGFeatureStamp
{
    int32 StampId;
    FString Name;
    int32 RegionId;
    FVector2D CenterM;
    FVector2D AxisUM;
    FVector2D AxisVM;
    double RadiusUM;
    double RadiusVM;
    double HeightAmplitudeM;
    double BlendWidthM;
    double MaxSlopeDeg;
    uint64 LocalSeed;
};
```

---

## Appendix B — Required Failure Gates For Radial / Turbine Terrain

### Volcano radial correlation gate

Compute outside active volcano footprint and outside locked coast:

```text
r_v = distance(P, active_volcano_center)
corr = correlation(height(P), r_v)
```

Fail if:

```text
abs(corr) > 0.35
```

This prevents volcano radius from acting as the global elevation controller.

### Angular harmonic gate

Around active volcano center, sample ridge/valley energy by angle. Compute angular Fourier/harmonic energy.

Fail if:

```text
max_harmonic_energy(m = 3..16) / median_harmonic_energy > 2.25
```

or if a known blade count dominates visual ridge/gully masks.

### Ridge orientation entropy gate

Compute ridge/valley orientation histogram over interior land cells.

Pass if:

```text
normalized_orientation_entropy >= 0.65
```

Fail if orientation bins show repeated fan sectors.

### Sector repetition gate

Divide interior into angular sectors around the active volcano center.

Fail if:

```text
sector_pattern_similarity_mean > 0.72
```

where sector pattern similarity compares normalized radial profiles of slope/ridge energy between adjacent or repeating sectors.

### Circular stamp edge gate

For each feature stamp not explicitly marked as circular volcanic geology:

```text
edge_circularity_score <= threshold
```

Fail if many region boundaries or height transitions are near-perfect circles.

---

## Appendix C — Generator Disable-Matrix

The preview exporter should support a disable matrix so source authority can be inspected:

| Preview | Macro | Graph | Old volcanics | Active volcano | Detail noise | Purpose |
|---|---:|---:|---:|---:|---:|---|
| `coast_only` | off | off | off | off | off | prove coast field |
| `macro_only` | on | off | off | off | off | prove non-volcanic mass |
| `macro_graph` | on | on | off | off | off | prove basin/ridge authority |
| `volcano_disabled_height_preview` | on | on | on | off | on | main non-volcanic acceptance |
| `volcano_enabled_height_preview` | on | on | on | on | on | final terrain |
| `detail_disabled` | on | on | on | on/off | off | prove noise is not hiding failure |
| `volcano_contribution_only` | off | off | off | on | off | prove compact footprint |

---

## Appendix D — Final Implementation Principle

A reviewer should be able to inspect the terrain like this:

```text
Show me the coast field.
Show me the macro relief without graph.
Show me the basins and divides.
Show me the valley floors.
Show me the named landform regions.
Show me old volcanic history.
Show me the active volcano contribution.
Show me the final terrain.
Show me the metrics proving the volcano did not organize the whole island.
```

If any one of those cannot be shown, the terrain source is not inspectable enough for this rewrite.
