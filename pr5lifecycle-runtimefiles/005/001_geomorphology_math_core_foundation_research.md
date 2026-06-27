# Geomorphology Math Core Foundation Research

**Project:** Jungle Game — UE5 Linux-first first-person survival-simulation terrain source and preview exporter  
**Research prompt:** PR5 Batch 005 Research Prompt 001  
**Recommended source identity:** `JG_TERRAIN_MATH_CORE_005_001`  
**Primary objective:** replace crater-centered island math with a deterministic geomorphology-first terrain authority where the island remains structurally interesting with the active volcano disabled.

---

## Executive Summary

Batch 004 proved that Jungle Game can preserve the large-scale coastal gates: 97.536 km world size, 6x linear scale versus Batch 003, stable shoreline, continuous beach, shelf/ocean handling, and square-edge ocean containment. The remaining failure is not a coastal problem. It is an interior terrain authority problem.

The new terrain foundation should be a **hierarchical landform authority**, not a decorated volcano. The island must be generated in this order:

1. **Locked island mass / coastline envelope** — the already-working beach, shelf, ocean, and edge-container behavior remain authoritative.
2. **Basement / tectonic macro relief** — island-scale asymmetric highlands and old eroded volcanic or basement blocks exist before any active volcano.
3. **Regional landform regions** — named uplands, lowlands, terraces, coastal plains, old benches, and scarps are resolved as inspectable scalar fields and `landform_region_id` output.
4. **Basin / ridge / valley graph** — catchments, divides, outlets, trunk valleys, saddles, floodplains, and coastal fans come from a graph/skeleton, not crater-centered radial coordinates.
5. **Scalar field composition** — fields are blended by signed distance fields, anisotropic distance-to-feature fields, partition-of-unity weights, and graph distances rather than circular falloff around a crater.
6. **Spectral detail and domain warping** — fBm, ridged multifractals, multifractals, and warping add roughness and lithologic variation but do not determine macro landform layout.
7. **Physically motivated post-processing** — diffusion, stream-power-like incision, slope failure relaxation, terrace/floodplain flattening, and coastal clamping make the heightfield plausible and validateable.
8. **Compact active volcano subsystem** — the active crater/cone is a local subsystem with bounded footprint, not the coordinate origin of the island.
9. **Preview evidence and gates** — `volcano_disabled_height_preview`, `landform_region_id`, catchment overlays, radial-artifact metrics, and preservation of coastal gates are required.

The key design rule is simple: **the volcano may be spectacular, but it must not be the island's coordinate system.**

---

## Terrain-Only Scope Boundary

This document covers only:

- deterministic terrain mathematics;
- landform authority architecture;
- terrain validation metrics;
- source/config/fingerprint identity;
- preview evidence outputs;
- implementation guidance for terrain source and preview exporter changes.

This document does **not** design foliage, ecology, weather, audio, NPCs, survival gameplay, combat, buildings, quests, final materials, UI, map, compass, minimap, GPS, objective markers, or player-facing navigation overlays.

Any generated masks named here are **developer terrain evidence masks**, not gameplay navigation aids.

---

## Batch 004 Failure Evidence

Batch 004 should be treated as a successful coastal proof and a failed interior proof.

### Preserved working evidence

The rewrite must preserve these gates unless a later prompt explicitly authorizes changing them:

| Gate | Required value |
|---|---:|
| `world_size_m` | `97536.0` |
| linear scale vs Batch 003 | `6.0` |
| area scale vs Batch 003 | `36.0` |
| `shoreline_error_max_m` | `0` or effectively `0` |
| `beach_continuity_pct` | `100%` |
| `ocean_below_sea_pct` | `100%` |
| `square_edge_ocean_violations` | `0` |
| outer world edge | ocean/container terrain |
| island perimeter | beach/coastal shelf/sea-level transition |

### Structural failure

The failed interior terrain reads as:

- a procedural volcano preview rather than a real island;
- one dominant central crater/cone feature;
- global terrain organized by crater-centered math;
- ridge and gully masks with turbine, flower, spoke, or fan-blade patterns;
- missing independent basins, valleys, saddles, coastal flats, plateaus, old benches, old cones, and eroded volcanic history.

The fix is not to add more noise or to perturb the crater radial mask. The fix is to **move the authority upward**: island mass, macro relief, region graph, basin graph, and hydrologic skeleton must exist before the active volcano is evaluated.

---

## Required Terrain Authority Hierarchy

### Recommended authority order

```text
JG_TERRAIN_MATH_CORE_005_001

1. World constants and deterministic seed identity
2. Locked island mass / coastline SDF / coastal envelope
3. Basement macro relief field
4. Regional upland / lowland / terrace / old-volcanic region fields
5. Basin, ridge-divide, outlet, trunk-valley graph
6. Valley floor / floodplain / saddle / coastal fan scalar fields
7. Hypsometric remap and slope distribution control
8. Noise/detail fields under landform masks
9. Physically motivated erosion/relaxation passes
10. Compact active volcano subsystem
11. Final coastal clamp and existing coastal validation gates
12. Developer previews, metrics, fingerprints, and rejection gates
```

### Authority responsibilities

| Layer | Authority type | Owns | Must not own |
|---|---|---|---|
| Island mass / coast | SDF / implicit scalar field | shoreline, beach, shelf, ocean transition | interior ridge/gully direction |
| Basement macro relief | scalar fields + region graph | island asymmetry, old highlands, upland blocks | crater spokes |
| Landform regions | region IDs + partition weights | named regions, lithologic roughness, benches, flats | final hydrologic connectivity alone |
| Basin/ridge/valley graph | graph/skeleton | outlets, divides, trunk valleys, catchment shape | coastline location |
| Valley/floodplain/fan stamps | anisotropic feature fields | floor width, terraces, fans, saddles | global radial organization |
| Noise/detail | spectral fields | texture, roughness, secondary relief | macro landform layout |
| Erosion/relaxation | iterative/post-process | incision, diffusion, slope failure, drainage consistency | coastline mutation |
| Active volcano | local feature subsystem | crater, fresh cone, lava benches, young gullies | global island identity |
| Coastal clamp | post-process constraint | sea, beach, shelf, edge ocean gates | inland landform generation |

### Non-negotiable rule

Outside the active volcano footprint, the final height must be explainable without reference to distance or angle from the active crater. The generator may contain local polar math inside the volcano subsystem, but global terrain must use island/coast/region/basin graph coordinates.

---

## Mathematical Foundation Overview

### Core representation

Use a height function over world coordinates in meters:

```text
p = (x, y), x/y in meters
world_size_m = 97536.0
half_world_m = 48768.0
sea_level_m = 0.0
H_final(p) = height above sea level in meters
```

The terrain should be a composition of inspectable intermediate fields:

```text
S_island(p)          signed island/coast distance field
C_coast(p)           coastal band classifier / normalized coast distance
R_region_i(p)        landform region partition weights
B_basin_id(p)        basin/catchment ID
D_ridge(p)           distance/height influence from ridge graph
D_valley(p)          distance/depth/flatness influence from valley graph
H_basement(p)        non-volcanic macro relief
H_regions(p)         region-conditioned relief and benches
H_valley(p)          valley/floodplain/fan carving and floor flattening
H_noise(p)           masked spectral detail
H_erosion(p)         physically motivated incision/relaxation result
H_volcano(p)         compact active volcano contribution
H_precoast(p)        final before coastal clamp
H_final(p)           coastal-clamped final terrain
```

### Recommended final composition

```text
H_nonvolcano(p) =
    CoastMaskLand(p) *
    HypsometricRemap(
        H_basement(p)
      + H_regions(p)
      + H_ridge_graph(p)
      - H_valley_graph(p)
      + H_terrace_fan_saddle(p)
      + H_noise_masked(p)
      + H_erosion_delta(p)
    )

H_with_volcano(p) = H_nonvolcano(p) + H_volcano_local(p)

H_final(p) = CoastalClamp(H_with_volcano(p), S_island(p), C_coast(p))
```

The `H_nonvolcano` output must be exported directly as `volcano_disabled_height_preview` and must pass its own acceptance tests.

### Why this fixes Batch 004

Batch 004 likely failed because the crater or volcano layer acted as a global basis function. That tends to create:

- circular elevation falloff;
- radial gully spacing;
- angle-periodic ridge masks;
- one-feature terrain;
- visually obvious procedural origin.

The proposed foundation gives the active volcano a bounded local role and moves primary terrain meaning into non-radial fields and graphs.

---

## Island Mass And Coast Envelope Fields

### Preserve coastline authority first

The coastline already passes. Therefore, the safest implementation is:

```text
Do not regenerate the accepted shoreline unless required.
Do not move beach/shelf/ocean thresholds.
Do not let inland erosion alter the final sea-level crossing.
Do not let volcano-disabled remapping change coastline SDF topology.
```

If the existing coastline source is already deterministic, wrap it as the authoritative `S_island(p)` field and build the new interior on top of it.

### Signed distance field convention

Use a clear convention and assert it in tests:

```text
S_island(p) > 0  inside land
S_island(p) = 0  shoreline target
S_island(p) < 0  ocean
```

Distance-field literature defines distance fields as scalar fields encoding minimum distance to a shape, with signed variants distinguishing inside and outside. That is the correct abstraction for the island/coast envelope because it makes coast bands, shelf, beach, and edge-ocean containment explicit and testable.

### Coastal band fields

Define meters-based coast bands from `S_island`:

```text
shoreline_m = 0
beach_width_m = existing accepted value, preserve if already set
coastal_flat_inland_m = 600 to 1800
coastal_fan_max_inland_m = 3500 to 6500
shelf_ocean_m = existing accepted value, preserve if already set
edge_ocean_margin_m = must keep square edge fully ocean
```

Recommended normalized masks:

```text
M_ocean(p)        = smoothstep(-shelf_width_m, 0, -S_island(p))
M_beach(p)        = band around S_island(p) == 0
M_coastal_flat(p) = smoothstep(coastal_flat_inland_m, 0, S_island(p))
M_interior(p)     = smoothstep(1200, 3500, S_island(p))
```

Use these masks only as coast constraints. Do not use them as a radial basis for all interior relief.

### Coastline generation if forced

If the coastline itself must be regenerated later, prefer a closed non-circular spline or level-set generated from multiple island-mass lobes:

```text
S_base(p) = signed_distance_to_closed_spline(p)
S_warped(p) = S_base(p + W_lowfreq(p)) + coastal_notch_field(p)
```

Constraints:

- no starburst coast generated from uniform angular sine lobes;
- no crater-origin polar coastline;
- no periodic angle pattern with obvious rotational symmetry;
- at least 8-14 major coast sectors with varied bay/headland geometry;
- outer square edge remains ocean with a robust margin.

---

## Macro Relief And Regional Upland Design

### Goal

The island should read as a large, old, asymmetric volcanic jungle island even before adding the active volcano.

The macro terrain should contain:

- a regional highland spine offset from the island center;
- 3-6 upland blocks or old volcanic remnants;
- 8-16 primary catchments draining to the sea;
- lowland coastal plains and valley mouths;
- saddles between highland blocks;
- old benches, terraces, scarps, and degraded secondary cones;
- at least one broad non-volcanic plateau/upland not aligned with the active crater.

### Macro relief field

Use a low-frequency basement field with anisotropic components:

```text
H_basement(p) =
    A_spine * SpineField(p, spine_polyline, width_left, width_right)
  + Σ A_block_i * UplandBlockField(p, block_i)
  + Σ A_scarp_i * EscarpmentField(p, scarp_i)
  + Σ A_bench_i * OldBenchField(p, bench_i)
  - Σ A_basin_i * BasinDepressionField(p, basin_i)
```

Where each component is defined in meters and each feature has its own orientation, width, taper, and asymmetric falloff.

### Regional highland spine

Use a polyline, not a radial axis:

```text
struct HighlandSpine {
    uint64 id;
    string name;
    vector<Vec2m> control_points;
    float crest_elev_m;        // 700-1400 non-volcanic
    float left_width_m;        // 3500-9000
    float right_width_m;       // 2500-11000
    float along_taper_power;   // 0.7-2.2
    float asymmetry;           // -1..1
    uint64 seed;
}
```

Field evaluation:

```text
u = closest_along_spine_parameter(p)
v = signed_perpendicular_distance_to_spine(p)
width = mix(left_width_m, right_width_m, v > 0)
crest = crest_elev_m * taper(u)
spine_field = crest * exp(-abs(v / width)^profile_power)
```

Add low-frequency along-spine undulation, but never equal angular spacing around a crater.

### Upland blocks

Use irregular oriented superellipse or SDF blobs:

```text
struct UplandBlock {
    uint64 id;
    string name;
    Vec2m center;
    float major_axis_m;       // 4500-14000
    float minor_axis_m;       // 2500-9000
    float rotation_rad;
    float summit_elev_m;      // 450-1300 non-volcanic
    float shoulder_elev_m;    // 180-550
    float rim_roughness_m;    // 20-120
    float erosion_age;        // 0 old/smooth .. 1 young/sharp
    uint64 seed;
}
```

There should be at least 3 non-volcanic upland blocks with centers separated by at least `8000 m` and not collinear with the active volcano.

### Old volcanic history without active-crater control

Old cones and benches are allowed, but they must be subordinated to regional fields:

```text
struct OldVolcanicFeature {
    uint64 id;
    string name;
    enum kind { degraded_cone, breached_cone, lava_bench, caldera_remnant, tuff_ring, old_flow_lobe };
    Vec2m center;
    float footprint_radius_m;    // 700-4500
    float max_relief_m;          // 40-350
    float breach_azimuth_rad;    // local only
    float erosion_age;           // usually 0.55-0.95 old
    uint64 seed;
}
```

Rules:

- old features may use local polar math inside their own footprint;
- no old feature may set global drainage directions;
- old features should be partly breached, eroded, offset, or truncated by valleys/terraces;
- no repeating cone chain unless justified by a local fissure/lineament field.

---

## Basin / Ridge / Valley Graph Authority

### Why graph authority is required

Noise fields can produce attractive roughness, but they do not guarantee coherent drainage basins, named valleys, ridge divides, outlets, saddles, floodplains, and coastal fans. Terrain-generation research based on hydrology uses river networks and construction trees because drainage structure is a first-order organizer of real terrain. DEM hydrology likewise derives flow direction, flow accumulation, catchments, watersheds, and channel networks from terrain flow fields.

Jungle Game should explicitly represent basin and valley structure as a deterministic graph, then fit scalar fields to it.

### Required graph objects

```text
struct BasinOutlet {
    uint64 id;
    string name;
    Vec2m coast_position;
    float mouth_width_m;         // 150-900
    float fan_radius_m;          // 600-3500
    float target_base_elev_m;    // 0-12 near coast
    uint64 seed;
}

struct BasinNode {
    uint64 id;
    string name;
    uint64 outlet_id;
    vector<uint64> trunk_valley_ids;
    vector<uint64> ridge_divide_ids;
    float target_area_km2;
    float hypsometric_target;    // 0.32-0.62
    float drainage_density_target_km_per_km2;
    uint64 seed;
}

struct RidgeDivide {
    uint64 id;
    string name;
    vector<Vec2m> polyline;
    float crest_elev_m;          // 250-1300 non-volcanic
    float left_basin_id;
    float right_basin_id;
    float saddle_min_elev_m;
    float sharpness;             // 0 broad .. 1 knife
    uint64 seed;
}

struct ValleySegment {
    uint64 id;
    string name;
    uint64 basin_id;
    vector<Vec2m> centerline;
    float source_elev_m;
    float mouth_elev_m;
    float floor_width_source_m;  // 20-120
    float floor_width_mouth_m;   // 150-1000
    float incision_depth_m;      // 20-450
    float sinuosity;             // 1.02-1.55
    float terrace_count;         // 0-4
    uint64 seed;
}

struct SaddleNode {
    uint64 id;
    string name;
    Vec2m position;
    uint64 ridge_divide_id;
    float target_elev_m;
    float width_m;               // 200-1600
    uint64 seed;
}
```

### Graph generation sequence

1. Sample `N_outlets = 8-16` coastal outlets from the locked coastline.
2. Bias outlet spacing by coast sector, not by equal angle from center.
3. Generate regional highland spine and upland blocks.
4. Generate ridge divides as polylines between highland blocks and saddle nodes.
5. Assign basin regions using constrained Voronoi / watershed-like partition over distance-to-outlet and ridge barriers.
6. Generate trunk valley centerlines from uplands to outlets using a cost field:

```text
Cost(p) =
    w_slope * uphill_penalty(p)
  + w_region * region_resistance(p)
  + w_ridge * ridge_crossing_penalty(p)
  + w_coast * outlet_alignment_cost(p)
  + w_noise * lowfreq_meander_cost(p)
```

7. Fit valley floors, floodplains, terraces, and fans as anisotropic fields around centerlines.
8. Run hydrologic validation on the resulting heightfield and adjust only in-scope graph parameters.

### Basin area targets

Use land area, not square world area, as the denominator.

| Metric | Initial target |
|---|---:|
| primary basin count | `8-16` |
| largest basin area | `< 24%` of land area |
| smallest primary basin area | `> 2.5%` of land area |
| basin area coefficient of variation | `0.35-0.85` |
| outlets with visible valley/fan | `>= 75%` |
| basins whose trunk valley reaches coast | `100%` |
| basin divide crossing by trunk valleys | `0` except explicit capture passes |

### Valley floor and floodplain targets

| Metric | Initial target |
|---|---:|
| valley/floodplain floor area | `8-18%` of land area |
| coastal fan area | `3-9%` of land area |
| named saddle count | `>= 18` at 97.536 km scale |
| trunk valley median slope | decreases downstream; no sustained uphill segments |
| valley floor width trend | generally widens downstream |
| valley orientations | no single radial mode dominates |

---

## Scalar Field Composition And Blending Rules

### Use scalar fields as composable terrain evidence

Every major landform should expose a field, not hide inside a monolithic height expression:

```text
float evalIslandSDF(Vec2m p);
float evalCoastMask(Vec2m p);
float evalRegionWeight(uint32 region_id, Vec2m p);
float evalBasinWeight(uint32 basin_id, Vec2m p);
float evalRidgeInfluence(uint32 ridge_id, Vec2m p);
float evalValleyInfluence(uint32 valley_id, Vec2m p);
float evalVolcanoLocalMask(Vec2m p);
```

### Partition-of-unity blending

For landform regions, use normalized weights:

```text
w_i_raw(p) = exp(-d_i(p)^2 / sigma_i^2) * gate_i(p)
w_i(p) = w_i_raw(p) / max(epsilon, Σ_j w_j_raw(p))
H_regions(p) = Σ_i w_i(p) * H_region_i(p)
```

Use `epsilon >= 1e-6` and export `region_weight_sum_error_max`.

### Prefer anisotropic distances

Circular falloff is the enemy of this rewrite. Use anisotropic distances for ridges, valleys, escarpments, terraces, and fans:

```text
d_aniso(p, feature) = sqrt((u / length_scale)^2 + (v / width_scale)^2)
```

Where:

- `u` is distance along the feature frame;
- `v` is perpendicular distance;
- width changes along `u`;
- falloff can differ left/right;
- the feature can be clipped by basin or region masks.

### Safe blending operators

Recommended operators:

```text
smoothstep edge gates
softmax / normalized weighted sum for region partitions
smooth min/max only for local constructive fields
height-preserving terrace quantization with blend width
slope-aware relaxation after composition
```

Avoid:

```text
global radial distance to crater
uniform polar angle functions around crater or island center
equal angular ridge/gully generation
hard max/min seams between large regions
unmasked high-frequency noise as structural relief
```

### Seam validation

Export seam metrics:

| Metric | Gate |
|---|---:|
| `region_weight_sum_error_max` | `< 1e-4` |
| `height_jump_p99_region_boundary_m` | `< 6 m` at preview resolution, except cliffs/scarps tagged as intentional |
| `slope_jump_p99_region_boundary_deg` | `< 8°`, except tagged scarps |
| `unexplained_boundary_seam_count` | `0` |

---

## Noise, Domain Warping, And Multifractal Use

### Noise is detail, not authority

Procedural noise is repeatable, smooth, translation-invariant, and useful for terrain detail. Perlin-style improved noise is appropriate for deterministic texture/detail fields. But noise alone is insufficient because it does not guarantee catchment topology, coherent valleys, downstream-widening floodplains, basin divides, saddles, terraces, or geomorphic history.

Use noise in these roles:

- local roughness inside a named region;
- rock/soil/lithology resistance variation;
- valley-wall roughness;
- old erosion texture;
- minor ridge crenulation;
- coastline micro-notches only if coastline is regenerated and revalidated;
- local active volcano roughness.

Do not use noise as:

- the only source of macro relief;
- the basin authority;
- a substitute for trunk valley graphs;
- a radial gully mask around the crater.

### Spectral layers

Recommended noise layers at 97.536 km scale:

| Layer | Wavelength | Amplitude | Mask |
|---|---:|---:|---|
| basement undulation | `12-35 km` | `80-350 m` | interior/highland only |
| regional roughness | `3-12 km` | `30-160 m` | by `landform_region_id` |
| ridge wall roughness | `500-2500 m` | `10-80 m` | ridge/valley wall masks |
| local terrain texture | `80-500 m` | `2-25 m` | slope/region masks |
| micro preview texture | `<80 m` | preview-dependent | not structural |

### fBm and multifractals

Use fBm for broad roughness:

```text
fbm(p) = Σ_{o=0}^{n-1} amp_o * noise(freq_o * p)
freq_o = base_freq * lacunarity^o
amp_o = base_amp * persistence^o
```

Use ridged multifractal for resistant ridge texture only where a ridge/crest field already exists:

```text
ridged(p) = Σ amp_o * (1 - abs(noise(freq_o * p)))^2
```

Use multifractal modulation to vary roughness by region:

```text
roughness_amp(p) = RegionRoughness(region_id) * LithologyResistance(p) * SlopeMask(p)
```

### Domain warping

Domain warping is useful for breaking grid alignment and simple bands:

```text
p_warped = p + W_macro(p) + W_region(p)
```

Rules:

- macro warp wavelength should be `15-40 km`, amplitude `300-1600 m`;
- region warp wavelength should be `3-10 km`, amplitude `80-500 m`;
- do not warp coast after coastal gate lock unless reprojected back to shoreline;
- do not use volcano-centered warp as a global field.

---

## Hypsometry, Relief, And Slope Distribution Targets

### Why hypsometry matters

Hypsometric curves and area-slope relationships are established catchment descriptors used in geomorphology and DEM analysis. They let the generator reject plausible-looking but structurally wrong heightfields by checking how much land lies in lowlands, midlands, and highlands, and how slopes are distributed by basin.

### Unit-aware scale

```text
world_size_m = 97536.0
world_area_km2 = 9513.271296
half_world_m = 48768.0
```

The island land area is not specified by the prompt and should be measured from the locked coastline:

```text
land_area_km2 = count(S_island > 0) * cell_area_km2
```

All percentage targets below use `land_area_km2` as denominator unless stated otherwise.

### Elevation targets

Initial non-volcanic elevation distribution target:

| Metric | Target |
|---|---:|
| `sea_level_m` | `0` |
| `H_nonvolcano_p05_m` | `2-25` |
| `H_nonvolcano_p25_m` | `60-180` |
| `H_nonvolcano_p50_m` | `220-480` |
| `H_nonvolcano_p75_m` | `520-900` |
| `H_nonvolcano_p90_m` | `850-1350` |
| `H_nonvolcano_max_m` | `1150-1700` |
| `H_final_max_m` with active volcano | `1600-2600` unless art direction says otherwise |

### Area targets

| Terrain class | Definition | Target |
|---|---|---:|
| beach / immediate coast | accepted existing beach mask | preserve existing 100% continuity |
| coastal flats | `0-80 m`, slope `<5°`, within coast/valley/fan fields | `8-18%` |
| lowlands | `0-200 m` | `18-35%` |
| midlands | `200-700 m` | `35-50%` |
| highlands | `>700 m` non-volcanic | `15-30%` |
| steep escarpment/cliff | slope `>35°` | `2-8%`, mostly tagged scarps/cliffs |
| active volcano footprint | `M_volcano > 0.2` | `<= 8%` |
| crater/core active footprint | `M_volcano > 0.75` | `<= 2.5%` |

### Slope targets

Compute slope in degrees from meter-aware gradients:

```text
slope_rad = atan(sqrt((dH/dx)^2 + (dH/dy)^2))
slope_deg = slope_rad * 180 / pi
```

Initial slope distribution:

| Slope class | Target share of land |
|---|---:|
| `<3°` flats/floodplains/beaches | `6-14%` |
| `3-12°` low rolling terrain | `20-35%` |
| `12-28°` dissected hills/uplands | `32-48%` |
| `28-38°` steep slopes/escarpments | `8-18%` |
| `>38°` cliffs/fresh cone/local scarps | `<= 6%` |

### Hypsometric integral target

For each basin:

```text
HI = (mean_elev - min_elev) / max(epsilon, max_elev - min_elev)
```

Targets:

| Metric | Gate |
|---|---:|
| island-wide `HI_nonvolcano` | `0.42-0.58` |
| basin `HI` range | `0.30-0.68` |
| basins below `HI=0.25` | `0` unless intentionally drowned lowland |
| basins above `HI=0.72` | `0` outside active volcano |
| coefficient of variation of basin HI | `>= 0.08` |

This prevents all basins from reading like copies of the same procedural fan.

---

## Volcano-Disabled Island Acceptance Test

### Required export

Add a preview artifact named exactly:

```text
volcano_disabled_height_preview
```

It must be generated with:

```text
active_volcano_enabled = false
old_volcanic_features_enabled = true
basement_macro_relief_enabled = true
basin_valley_graph_enabled = true
coastal_clamp_enabled = true
```

The active crater/cone/fresh lava subsystem is disabled. Old eroded volcanic history remains because it is part of the island's inherited terrain.

### Required preview outputs

```text
volcano_disabled_height_preview.png
volcano_disabled_slope_preview.png
volcano_disabled_hillshade_preview.png
volcano_disabled_basin_id_preview.png
volcano_disabled_landform_region_id_preview.png
volcano_disabled_ridge_valley_graph_overlay.png
volcano_disabled_metrics.json
```

### Acceptance criteria

The volcano-disabled island passes only if:

| Gate | Required result |
|---|---:|
| coastal gates | all existing coast/beach/shelf/ocean gates still pass |
| non-volcanic max relief | `>= 900 m` and `<= 1700 m` initially |
| independent highland blocks | `>= 3` blocks, each `> 15 km2`, separated by `>= 8000 m` |
| primary basins | `8-16` valid basins |
| visible trunk valleys | `>= 75%` of primary basins |
| valley/floodplain/fan floor area | `10-24%` combined |
| named saddle count | `>= 18` |
| lowland share | `18-35%` |
| highland share | `15-30%` |
| active volcano contribution | exactly `0` in disabled preview |
| radial/turbine gates | all pass |
| visual read | island still reads as complete terrain, not blank base awaiting volcano |

### Failure conditions

Reject the preview if any of these are true:

- the island becomes visually flat without the active volcano;
- all ridges point to or away from the active crater location even while disabled;
- basin outlets are evenly angle-spaced like a flower;
- the largest height gradient is dominated by distance from the volcano point;
- highlands collapse to one central mound;
- valley floors are absent or merely noise depressions;
- coastal gates pass but interior terrain reads as turbine/fan-blade procedural geometry.

---

## Determinism, Seeds, Config, And Fingerprints

### Version identity

Use a source-level version constant:

```text
terrain_math_core_version = "JG_TERRAIN_MATH_CORE_005_001"
```

The preview exporter should include this value in every metrics JSON and image metadata sidecar.

### Seed hierarchy

Use a deterministic hierarchical seed tree:

```text
root_seed
  ├── coastline_seed                 // preserve existing accepted seed if present
  ├── basement_seed
  ├── landform_region_seed
  ├── basin_graph_seed
  ├── ridge_graph_seed
  ├── valley_graph_seed
  ├── old_volcanic_history_seed
  ├── active_volcano_seed
  ├── noise_seed_pack
  └── erosion_seed
```

Every derived seed should be produced by a stable hash:

```text
derived_seed = Hash64(root_seed, terrain_math_core_version, subsystem_name, index)
```

Do not depend on non-deterministic container ordering, unordered map iteration, locale, platform-specific floating behavior where avoidable, or current time.

### Config object

Recommended top-level config:

```text
struct TerrainMathCoreConfig {
    string version;                       // JG_TERRAIN_MATH_CORE_005_001
    uint64 root_seed;
    double world_size_m;                  // 97536.0
    double sea_level_m;                   // 0.0
    CoastConfig coast;
    BasementReliefConfig basement;
    LandformRegionConfig regions;
    BasinGraphConfig basins;
    ValleyGraphConfig valleys;
    ErosionConfig erosion;
    OldVolcanicHistoryConfig old_volcanic;
    ActiveVolcanoConfig active_volcano;
    ValidationConfig validation;
}
```

### Fingerprint

Export a stable fingerprint:

```text
terrain_source_fingerprint = SHA256(
    canonical_json(TerrainMathCoreConfig)
  + source_version_strings
  + sorted_feature_ids_and_params
  + preview_resolution
)
```

Required metrics JSON fields:

```text
{
  "terrain_math_core_version": "JG_TERRAIN_MATH_CORE_005_001",
  "terrain_source_fingerprint": "...",
  "root_seed": "...",
  "world_size_m": 97536.0,
  "linear_scale_vs_batch003": 6.0,
  "area_scale_vs_batch003": 36.0,
  "active_volcano_enabled": false,
  "coastal_gates": {...},
  "hypsometry": {...},
  "slope_distribution": {...},
  "basin_metrics": {...},
  "radial_artifact_metrics": {...},
  "landform_region_metrics": {...}
}
```

---

## Preview And Developer Evidence Requirements

### Required map exports

At minimum:

```text
height_final_preview.png
height_nonvolcano_preview.png
volcano_disabled_height_preview.png
slope_preview.png
hillshade_preview.png
coast_sdf_preview.png
coast_band_preview.png
landform_region_id_preview.png
landform_region_weight_debug.png
basin_id_preview.png
ridge_valley_graph_overlay.png
outlet_fan_preview.png
hypsometry_plot.png
slope_distribution_plot.png
radial_artifact_report.json
terrain_metrics.json
```

### `landform_region_id` map

The preview exporter must output an integer region map. Initial named regions should be deterministic and human-readable in JSON:

```text
0 ocean_container
1 beach_and_shelf_transition
2 north_windward_coastal_plain
3 west_old_lava_bench
4 central_highland_spine
5 south_eroded_upland_block
6 east_coastal_fan_lowland
7 interior_saddle_chain
8 old_caldera_remnant
9 secondary_degraded_cone_field
10 active_volcano_local_footprint
11 active_crater_core
```

The names can change if the actual island layout changes, but the map must show **multiple named terrain authorities** and not just volcano/coast/interior.

### Radial/turbine rejection metrics

Compute radial artifact metrics around both:

```text
active_volcano_center
island_mass_centroid
```

Only the active volcano local footprint may show strong radial organization.

#### Height-distance dominance

Outside active volcano footprint:

```text
corr_h_r = corr(H_nonvolcano(p), distance(p, active_volcano_center))
```

Gate:

```text
abs(corr_h_r) < 0.45
```

Prefer `<0.35` after tuning.

#### Angular Fourier power

For annular samples around the crater center, compute angular Fourier amplitudes of height, slope, ridge mask, and valley mask:

```text
A_k = amplitude of angular mode k, k = 2..16
radial_periodicity_score = max(A_k) / sum(A_1..A_16)
```

Gate outside active volcano:

```text
radial_periodicity_score_height < 0.22
radial_periodicity_score_ridge_mask < 0.18
radial_periodicity_score_valley_mask < 0.18
```

#### Orientation histogram spoke score

Compute ridge/valley local orientation and compare to bearing from crater:

```text
alignment = abs(dot(unit_feature_direction, unit_radial_direction))
```

Gate outside active volcano:

```text
p75_alignment < 0.72
p90_alignment < 0.86
```

A terrain with many spokes will fail this even if its coastline passes.

#### Basin outlet angular uniformity

For basin outlet bearings around island centroid, compute nearest-neighbor angular spacing variance and compare to a jittered nonuniform baseline. Reject if outlets are too evenly spaced:

```text
outlet_spacing_uniformity_score < 0.80
```

A perfect flower/fan layout tends toward `1.0` and should fail.

### Visual evidence panels

Every Batch 005 terrain preview should include a panel layout:

```text
A final height + hillshade
B volcano-disabled height + hillshade
C landform_region_id
D basin_id + outlets
E ridge/valley graph overlay
F slope distribution + hypsometric curve
G radial artifact report summary
H coastal gate summary
```

---

## Files And Areas Likely Touched

The existing repository is not inspected by this document, so the names below are likely areas and search targets rather than guaranteed file paths. The implementation PR should search for the current terrain source and preview exporter equivalents.

### Likely terrain source areas

Search terms:

```text
Terrain
Island
Volcano
Crater
Height
Heightfield
Generator
Preview
Exporter
Coast
Beach
Shelf
Biome if terrain masks are colocated, but do not add ecology scope
```

Likely touched source categories:

| Area | Expected change |
|---|---|
| terrain math core | replace crater-centered global height composition with hierarchy |
| terrain config/schema | add versioned `TerrainMathCoreConfig` and seed tree |
| coastline/coast clamp | preserve accepted gates; expose `S_island` and coast bands |
| volcano subsystem | bound active volcano by local mask; remove global authority |
| graph generation | add basin/outlet/ridge/valley/saddle graph structures |
| landform regions | add named `landform_region_id` and region weights |
| erosion/postprocess | add deterministic diffusion/incision/slope relaxation passes |
| preview exporter | add volcano-disabled, region, basin, graph, slope, hypsometry, radial metrics |
| validation/tests | add coastal preservation and interior anti-radial gates |
| docs/research | add this spec and implementation notes |

### Likely class/module names to create if absent

```text
JG::Terrain::TerrainMathCore005001
JG::Terrain::TerrainMathCoreConfig
JG::Terrain::IslandCoastEnvelope
JG::Terrain::BasementReliefField
JG::Terrain::LandformRegionMap
JG::Terrain::BasinGraph
JG::Terrain::RidgeValleySkeleton
JG::Terrain::TerrainFieldComposer
JG::Terrain::DeterministicErosionPass
JG::Terrain::ActiveVolcanoSubsystem
JG::Terrain::TerrainValidationMetrics
JG::Terrain::PreviewExportBatch005
```

### Implementation caution

Do not rename or delete accepted Batch 004 coastal validation code until the new terrain core can prove the same coastal metrics. Prefer an adapter/wrapper that preserves old coast authority while replacing interior generation.

---

## Large PR Implementation Scope

This rewrite is large and should be implemented as one scoped PR only if the repo can absorb it cleanly. Otherwise split into staged PRs.

### Recommended staged plan

#### Stage 1 — Source identity and non-destructive wrappers

- Add `JG_TERRAIN_MATH_CORE_005_001` version constant.
- Add config/fingerprint export.
- Wrap existing accepted coast as `IslandCoastEnvelope`.
- Preserve all Batch 004 coastal tests.
- Add preview exporter slots for new maps even if initially blank.

Exit criteria:

```text
existing coast metrics unchanged
terrain fingerprint exported
no visual terrain rewrite yet
```

#### Stage 2 — Basement relief and landform regions

- Implement macro highland spine.
- Implement upland blocks, old benches, coastal plains, and saddles.
- Export `landform_region_id` and region weights.
- Add hypsometry and slope metrics.

Exit criteria:

```text
volcano_disabled_height_preview has visible independent macro terrain
active volcano disabled contribution = 0
lowland/highland/slope gates close to target
```

#### Stage 3 — Basin/ridge/valley graph

- Add outlets, basins, ridge divides, trunk valleys, floodplains, fans.
- Export basin graph overlays and `basin_id`.
- Add valley floor and catchment metrics.

Exit criteria:

```text
8-16 primary basins
>=75% visible trunk valleys
no single radial basin pattern
```

#### Stage 4 — Erosion/relaxation

- Add diffusion pass.
- Add stream-power-like incision pass.
- Add slope failure/angle-of-repose relaxation.
- Re-clamp coast.

Exit criteria:

```text
hydrologic consistency improved
no coastline regression
slope distribution in target range
```

#### Stage 5 — Compact active volcano

- Reintroduce active volcano as local subsystem.
- Bound active footprint.
- Add active crater/core maps.
- Ensure volcano does not control global terrain.

Exit criteria:

```text
final terrain has strong active volcano
volcano_disabled terrain still passes
active volcano footprint <=8% land area
radial metrics pass outside volcano footprint
```

#### Stage 6 — Acceptance gates and visual evidence

- Add full metrics JSON.
- Add radial/turbine artifact gates.
- Add preview contact sheet.
- Add source docs.

Exit criteria:

```text
all coastal gates pass
all volcano-disabled gates pass
all radial/turbine gates pass
all metrics are deterministic across repeated runs
```

---

## Acceptance Criteria

### Hard acceptance gates

```text
terrain_math_core_version == JG_TERRAIN_MATH_CORE_005_001
world_size_m == 97536.0
linear_scale_vs_batch003 == 6.0
area_scale_vs_batch003 == 36.0
shoreline_error_max_m == 0 or effectively 0
beach_continuity_pct == 100
ocean_below_sea_pct == 100
square_edge_ocean_violations == 0
outer world edge remains ocean/container terrain
```

### Interior authority gates

```text
volcano_disabled_height_preview exists
active_volcano_enabled=false produces active_volcano_delta_max_m == 0
nonvolcanic_highland_block_count >= 3
primary_basin_count between 8 and 16
named_landform_region_count >= 8
named_saddle_count >= 18
visible_trunk_valley_basin_pct >= 75
coastal_fan_or_valley_mouth_count >= 6
largest_basin_area_pct < 24
active_volcano_footprint_pct <= 8
```

### Distribution gates

```text
lowland_area_pct between 18 and 35
midland_area_pct between 35 and 50
highland_area_pct between 15 and 30
coastal_flat_area_pct between 8 and 18
island_hypsometric_integral_nonvolcano between 0.42 and 0.58
slope_gt_38deg_pct <= 6 except tagged intentional cliffs/fresh cone
```

### Anti-procedural artifact gates

```text
height_distance_corr_to_volcano_abs < 0.45 outside active volcano footprint
radial_periodicity_score_height < 0.22 outside active volcano footprint
radial_periodicity_score_ridge_mask < 0.18 outside active volcano footprint
radial_periodicity_score_valley_mask < 0.18 outside active volcano footprint
orientation_radial_alignment_p75 < 0.72 outside active volcano footprint
orientation_radial_alignment_p90 < 0.86 outside active volcano footprint
outlet_spacing_uniformity_score < 0.80
unexplained_boundary_seam_count == 0
```

### Determinism gates

```text
same config + same seed + same platform mode => same fingerprint
same fingerprint => same metrics JSON
same fingerprint => same preview hashes within accepted tolerance
feature IDs stable across runs
no unordered iteration affects exported order
```

---

## Risks And Stop Conditions

### Risks

| Risk | Why it matters | Mitigation |
|---|---|---|
| coastline regression | existing gates are already working | wrap coast; run coast tests after every stage |
| overfitted metrics | terrain passes numbers but still reads fake | require visual evidence panels and anti-radial gates |
| graph complexity | basin graph may become hard to debug | export every graph node/edge and ID map |
| erosion nondeterminism | iterative erosion can diverge by platform | fixed iteration counts, deterministic ordering, tolerance tests |
| excessive smoothing | diffusion can erase jungle island relief | slope-aware relaxation only; preserve tagged scarps/ridges |
| volcano becomes global again | easiest visual trick is to raise crater influence | enforce footprint and correlation gates |
| noise-as-authority relapse | noise can hide missing structure | require graph/region previews with volcano disabled |
| hard region seams | region fields may create visible stitching | partition-of-unity weights and seam metrics |

### Stop conditions

Stop implementation and report failure if:

- any coastal gate regresses;
- terrain source cannot produce a deterministic fingerprint;
- `volcano_disabled_height_preview` fails to generate;
- active volcano disabled terrain has no independent highlands/basins;
- radial/turbine gates fail after two in-scope tuning passes;
- implementation requires gameplay/UI/ecology scope to hide terrain defects;
- generated output contains unrelated repo changes, generated junk, or cache files.

---

## Rejected Or Forbidden Approaches

Forbidden for this rewrite:

```text
Global crater-centered polar coordinates as terrain authority.
Global distance-to-crater elevation falloff.
Global angle-periodic ridge or gully masks.
Equal-spaced radial basins/outlets around volcano or island center.
Adding more noise to disguise missing basins.
Using the active volcano as the only highland.
Moving coastline to fix interior terrain.
Letting erosion/postprocess violate accepted beach/shelf/ocean gates.
Hard region seams hidden by materials instead of fixed in height math.
Gameplay/navigation/UI overlays as compensation for terrain readability.
```

Allowed but constrained:

```text
Local polar math inside active volcano footprint.
Local polar math inside old degraded cone footprints.
Noise for roughness/detail under graph/region masks.
Feature stamps for old benches, cones, terraces, and scarps.
Erosion approximations if deterministic and inspectable.
```

---

## Sources And References

The following sources support the mathematical and geomorphic direction of this spec. They are references for the terrain foundation, not requirements to implement exact scientific simulation.

1. Sarah F. Frisken, Ronald N. Perry, Alyn P. Rockwood, Thouis R. Jones, **“Adaptively Sampled Distance Fields: A General Representation of Shape for Computer Graphics,”** MERL Technical Report TR2000-15, 2000. Supports distance fields as scalar shape representations and signed distance distinction for inside/outside tests.  
   URL: https://www.merl.com/publications/docs/TR2000-15.pdf

2. Jean-David Génevaux, Éric Galin, Éric Guérin, Adrien Peytavie, Bedřich Beneš, **“Terrain Generation Using Procedural Models Based on Hydrology,”** ACM Transactions on Graphics, 32(4), Article 143, 2013. Supports graph/hydrology-based procedural terrain, river networks, construction trees, blending, and terrain patches rather than pure noise.  
   URL: https://www.cs.purdue.edu/cgvlab/www/resources/papers/Genevaux-ACM_Trans_Graph-2013-Terrain_Generation_Using_Procedural_Models_Based_on_Hydrology.pdf

3. Ken Perlin, **“Improving Noise,”** ACM Transactions on Graphics / SIGGRAPH 2002. Supports deterministic coherent noise as a repeatable procedural primitive, with known limitations as a structural terrain authority.  
   URL: https://dl.acm.org/doi/10.1145/566654.566636

4. Simon Green, **“Chapter 26. Implementing Improved Perlin Noise,”** GPU Gems 2, NVIDIA Developer. Supports properties useful for deterministic procedural noise: repeatable pseudorandom values, known range, smooth/band-limited behavior, large period, and CPU/GPU consistency considerations.  
   URL: https://developer.nvidia.com/gpugems/gpugems2/part-iii-high-quality-rendering/chapter-26-implementing-improved-perlin-noise

5. David G. Tarboton, **“Reflections on Geomorphometry and Hydrology,”** Geomorphometry 2025. Supports DEM-derived flow direction, flow accumulation, catchments, watersheds, channel networks, D-infinity flow routing, and HAND-style derived terrain quantities.  
   URL: https://www.geomorphometry.org/uploads/pdf/pdf2025/Geomorphometry_2025_Tarboton.pdf

6. Jeffrey S. Kwang and Gary Parker, **“Landscape evolution models using the stream power incision model show unrealistic behavior when m/n equals 0.5,”** Earth Surface Dynamics, 5, 807-820, 2017. Supports stream-power incision formulation and cautions about naive model parameterization and scale behavior.  
   URL: https://esurf.copernicus.org/articles/5/807/2017/esurf-5-807-2017.pdf

7. Landlab documentation, **“Using the Landlab Shared Stream Power Model,”** 2024. Supports practical landscape evolution model components that transition between bedrock incision and sediment transport/deposition regimes.  
   URL: https://landlab.readthedocs.io/en/latest/tutorials/landscape_evolution/erosion_deposition/shared_stream_power.html

8. Sagy Cohen, Garry Willgoose, Greg Hancock, **“A methodology for calculating the spatial distribution of the area-slope equation and the hypsometric integral within a catchment,”** Journal of Geophysical Research, 113, F03027, 2008. Supports area-slope and hypsometric integral as catchment descriptors for quantitative terrain validation.  
   URL: https://sdml.ua.edu/wp-content/uploads/2012/09/Cohen.et_.al_.AreaSlopeHI.pdf

9. U.S. National Park Service, **“Types of Volcanoes — Volcanoes, Craters & Lava Flows,”** updated 2022. Supports volcanic landform diversity, monogenetic/polygenetic distinctions, constructional/excavational forms, cinder cones, shield volcanoes, stratovolcanoes, domes, maars, tuff rings, calderas, and complex volcanoes.  
   URL: https://www.nps.gov/subjects/volcanoes/types-of-volcanoes.htm

10. British Geological Survey, **“Types of volcano,”** Discovering Geology — Volcanoes. Supports slope and form differences between shield volcanoes and stratovolcanoes, including low-viscosity basaltic lava forming broad gentle shield slopes and viscous magma forming steeper cones.  
    URL: https://www.bgs.ac.uk/discovering-geology/earth-hazards/volcanoes/how-volcanoes-form/

---

## Final Implementation Position

The correct Batch 005 direction is a **new terrain authority foundation**:

```text
coastline locked;
volcano demoted to local subsystem;
macro island terrain generated first;
landform regions named and exported;
basins/ridges/valleys graph-authored;
noise used only as masked detail;
physically motivated post-processing applied deterministically;
coastal gates re-applied and preserved;
radial/turbine artifacts explicitly rejected.
```

The decisive evidence artifact is `volcano_disabled_height_preview`. If the island does not look like a complete, asymmetric, basin-rich volcanic jungle island with the active volcano disabled, the terrain foundation has not passed Batch 005.
