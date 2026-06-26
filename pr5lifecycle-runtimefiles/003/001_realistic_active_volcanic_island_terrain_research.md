# Realistic Active Volcanic Island Terrain Research

## Executive Summary

PR5 Batch 003 should replace the current low-relief full-size shell with a deterministic, terrain-only volcanic-island foundation. The recommended landform driver is a **compact basaltic oceanic shield-volcano island with an active summit/vent complex and incised tropical catchments**, not a steep stratovolcano cone. Shield volcanoes are built by repeated low-viscosity basaltic lava flows from central vents and flank/rift-zone vents, producing broad domical slopes and radial lava-flow structure rather than a single theatrical cone.[^usgs-shield-types][^nps-shield]

The playable island should sit **inside** the 16.256 km square world, with the square boundary remaining ocean. The generator must model a circular or organically rounded island mask whose outer edge resolves through shallow shelf, beach/berm/coastal-flat bands, and then inland volcanic rise. The coastline must never be a vertical wall, square-world edge, arbitrary cliff, or random raised rim.

The implementation target is a heightfield/mesh system that can prove the following at developer-debug level:

- sea level is exactly defined as `0 m`;
- outside-island samples are ocean or below sea level;
- the island perimeter has a continuous beach/shelf/coastal transition;
- the central massif reaches approximately `1400 m`;
- radial ridges, gullies, catchments, vents, crater/rim terrain, fans, benches, beaches, and hard-blocker terrain masks are generated deterministically;
- diagnostic maps show height, slope, mask, catchment, ridge, gully, blocker, and coastline correctness;
- no player-facing navigation UI is introduced.

The current `129 x 129` runtime mesh at roughly `127 m` vertex spacing is sufficient only for validating full-domain macro shape. It is not sufficient to represent gullies, beach bands, crater rims, lava crust detail, or safe/dangerous slope transitions. The major PR should therefore introduce a higher-resolution authoritative terrain model and downsample/LOD path, even if the immediate runtime proof still displays a coarse mesh.

## Terrain-Only Scope Boundary

This document covers **terrain generation only**.

Allowed in this batch:

- island mask generation;
- ocean/outside-island mask behavior;
- sea-level handling;
- beach, shelf, coastal-flat, and coastal-bench geometry;
- volcanic massif height profile;
- crater, rim, vent, shoulder, rift, radial ridge, gully, catchment, fan, lava-crust, and blocker geometry;
- terrain masks and debug classifications;
- slope, relief, curvature, catchment, flow, and shoreline diagnostics;
- developer-only output maps, rasters, screenshots, CSV summaries, and pass/fail checks;
- basic debug materials only when needed to read terrain classifications.

Forbidden in this batch:

- foliage placement;
- ecology systems;
- weather systems;
- audio or ambience systems;
- survival mechanics;
- animals, NPCs, relict-hominin content, or story scripting;
- final VFX;
- final art materials beyond debug readability;
- asset-pack dressing;
- player UI;
- player-facing map, minimap, compass, GPS, objective marker, clock, bearing readout, orientation meter, or modern navigation tool.

Developer-only maps are allowed because they are validation tools, not gameplay affordances.

## Current Runtime Terrain Baseline

The committed proof shell is valuable because it proves full-domain runtime terrain coverage, but it is not the target terrain model.

Current baseline:

```text
world domain: 16.256 km x 16.256 km
runtime mesh: 129 x 129 vertices
runtime vertex spacing: about 127 m
runtime token: JG_FULL_TERRAIN_SHELL_003
current elevation range: about -101 m to +140 m
current slope range: about 0 to 7 degrees
```

Problems to solve:

1. **Relief is too low.** A `+140 m` maximum reads as a low island or hill, not a volcanic massif.
2. **Slope range is too gentle.** `0–7°` is insufficient for crater rims, radial ridges, ravines, lava benches, shoulders, and hard blockers.
3. **The mesh is too coarse for final terrain semantics.** At `~127 m` spacing, one vertex can cover an entire beach band, a gully, a narrow ridge crest, or a crater-wall transition.
4. **Coastline semantics must be explicit.** The landmass must not emerge as a raised circular plug or square-edged terrain block.
5. **Diagnostics must become first-class.** The generator needs developer-visible proof, not visual impression alone.

The PR should keep the existing full-world proof as historical evidence, but the generator should now produce a new terrain model with substantially larger elevation range, terrain masks, and validation artifacts.

## Major Implementation Goal

Build a deterministic full-scale volcanic island terrain generator that produces:

```text
world size: 16,256 m x 16,256 m
coordinate frame: centered world, x/y roughly [-8,128, +8,128] m
sea level: 0 m
organic island shoreline radius: target 6,700–7,150 m, with angular perturbation
safe square-edge ocean margin: at least 600–900 m after max shoreline perturbation
maximum massif elevation: target 1,350–1,450 m
summit/crater complex: active volcanic geometry near central massif
outside-island region: ocean / below sea level
shoreline: sea-level crossing, beach ring, coastal shelf, coastal flats
terrain masks: ocean, shelf, beach, coastal flat, shield slope, ridge, gully, catchment, fan, vent, crater, lava crust, blocker
diagnostics: height/slope/mask/catchment/coastline/ridge/gully/blocker output
```

This is a large implementation slice. It should not be constrained to a tiny patch if the existing terrain pipeline is too weak. The correct outcome is a coherent terrain foundation that later systems can consume.

The generator should output an **authoritative terrain data product** independent of the immediate render mesh:

- height samples in meters;
- normalized coordinate/radius values;
- deterministic seed/version metadata;
- terrain-class masks;
- slope and curvature fields;
- catchment/flow fields;
- debug images and statistics.

The runtime mesh may initially consume a coarse downsample. The source terrain model should not be limited by the `129 x 129` proof resolution.

## Recommended Volcanic Island Landform Type

Use a **basaltic oceanic shield-volcano island with active/rejuvenated vents and tropical erosion**.

Rationale:

- Shield volcanoes are built from many fluid basalt lava flows spreading outward from central and flank vents, matching broad radial terrain rather than a steep cone.[^usgs-shield-types]
- Hawaiian-style volcanic islands show above-sea-level shield profiles, lava-flow surfaces, rift/vent activity, and coastal interaction, which are useful analogs for an active tropical volcanic island game terrain.[^usgs-hawaii-revealed][^usgs-evolution]
- A compact game-world shield can support a `~1400 m` summit while still giving beaches, coastal flats, benches, gullies, ridges, hard blockers, and crater/vent terrain inside a `16.256 km` world.
- The shield model naturally produces radial descent from summit to sea, making it compatible with deterministic catchment and gully generation.

Avoid using a classic stratovolcano as the whole-island driver. Stratovolcano geometry would bias the world toward an over-steep central cone, narrow radial canyons, and unrealistic relief compression for a compact tropical island. The game can still include localized steep crater walls, vents, lava cliffs, and collapse scarps, but the island-scale form should be shield-volcano dominated.

Recommended terrain identity:

```text
primary form: compact basaltic shield volcano
summit: active crater / vent complex
flanks: broad volcanic shoulders with radial ridges
erosion: tropical incised gullies and catchments
coast: shelf + beach ring + low coastal flats + benches/fans
hazard terrain: local vents, lava crust fields, collapse scarps, cliffs, unstable blocky lava
```

## Island Mask, Sea Level, And Beach Ring Requirements

The island mask is a first-order terrain primitive. It must not be an afterthought.

Recommended coordinate model:

```text
world_half_extent_m = 8128.0
sea_level_m = 0.0
center = (cx, cy), normally near (0, 0)
theta = atan2(y - cy, x - cx)
r = length((x - cx, y - cy))
R_base = 6900.0 to 7050.0 m
R(theta) = R_base + organic_low_frequency_angular_offset(theta)
```

Organic shoreline perturbation should be low-frequency and bounded:

```text
shoreline_offset_amplitude = 150–450 m
max_effective_radius = <= 7350 m
minimum_ocean_margin_to_square_edge = >= 600 m
```

The square world edge should remain ocean or below sea level. No land feature should rely on the square boundary.

Recommended radial bands, measured inward from the organic shoreline:

| Band | Approx width | Elevation behavior | Required result |
|---|---:|---:|---|
| Deep/outer ocean | square edge to outer shelf | `-250 m` to `-40 m` | world border reads as ocean |
| Shallow coastal shelf | `250–700 m` seaward of shoreline | `-40 m` to `-1 m` | no cliff into ocean |
| Swash/sea-level beach ring | `60–180 m` around shoreline | `-1 m` to `+4 m` | sea-level transition |
| Beach berm / backshore | `40–140 m` inward | `+2 m` to `+10 m` | low, readable beach/berm |
| Coastal flats / lowlands | `350–900 m` inward | `+5 m` to `+80 m` | gentle playable low terrain |
| Inland volcanic rise | after coastal flats | rises toward shield | transition to ridges/catchments |

Coastal profile concepts such as beach face, berm, foreshore, backshore, nearshore, and offshore are standard shoreline subdivisions and should be represented as simple terrain bands in this generator, not as final coastal art.[^coastalwiki-beach][^opengeology-shorelines]

Critical pass/fail rule:

```text
For sampled points near R(theta), height must resolve continuously through sea level.
There must be no raised circular wall at the island edge.
There must be no square-edge landmass.
```

## 16.256 km World Scale Model

The world domain is:

```text
world_size_m = 16256.0
half_extent_m = 8128.0
area_m2 = 264,257,536
```

A circular island with an effective shoreline radius of `~6.9–7.1 km` fits inside the square while leaving visible ocean to all sides:

```text
R_base = 7000 m
square half extent = 8128 m
nominal ocean margin = 1128 m
with +350 m shoreline perturbation: margin = 778 m
```

Recommended major distances:

| Feature | Target distance/scale |
|---|---:|
| Square world side | `16,256 m` |
| Center-to-square-edge | `8,128 m` |
| Mean shoreline radius | `6,900–7,050 m` |
| Max shoreline radius after perturbation | `<= 7,350 m` |
| Ocean margin to square edge | `>= 600–900 m` |
| Summit/crater center offset | `0–600 m` from island center |
| Main summit elevation | `1,350–1,450 m` |
| Summit crater radius | `300–700 m` |
| Upper volcanic shoulder radius | `1,200–2,200 m` |
| Mid-slope radial ridge/gully zone | `2,000–5,700 m` |
| Coastal lowland inner boundary | `500–1,100 m` inland from shoreline |

A `1400 m` summit inside a `~7 km` radius island implies a compact profile. The generator should therefore avoid a perfectly linear ramp from sea to summit. Use a multi-zone profile:

1. low beach/shelf at the edge;
2. gentle coastal flats;
3. stronger inland rise;
4. broad volcanic shoulders;
5. summit rim/crater relief;
6. localized steep scarps and blockers.

This avoids turning the entire island into a single continuous `~11–13°` ramp while still reaching the target elevation.

## 1400 m Massif Placement And Proportions

Recommended massif placement:

```text
massif_center = island_center + small deterministic offset
offset_magnitude = 0–600 m
offset_purpose = create asymmetric catchments and coast distances
peak_target_m = 1400
valid_peak_range_m = 1350–1450
```

The massif should occupy the central island but not the whole island equally. It should have:

- central summit/crater/rim terrain;
- upper shoulder slopes;
- radial ridge spines descending from the upper/mid flanks;
- incised gully/catchment corridors;
- lava-flow benches or crust zones;
- gentler coastal fans and lowlands.

Recommended radial zones from massif center:

| Zone | Radius from massif center | Elevation character | Terrain role |
|---|---:|---|---|
| Active summit/crater | `0–800 m` | `1,050–1,450 m` | rim, crater floor, vents, hard blockers |
| Upper shoulder | `800–2,000 m` | `700–1,250 m` | steep shoulders, ridges, ravines |
| Mid-flank | `2,000–4,800 m` | `180–900 m` | radial ridge/gully/catchment network |
| Lower flank | `4,800–6,200 m` | `40–250 m` | fans, benches, lava terraces |
| Coast/lowland | shoreline inward `0–1,100 m` | `0–100 m` | beaches, flats, access corridors |
| Ocean/shelf | outside shoreline | `<0 m` | shelf and ocean boundary |

The summit should not be a sharp needle. Shield volcanoes are broad domical landforms, so the high massif should be a broad mass with localized crater/rim relief and vent terrain rather than a point peak.[^nps-shield][^usgs-shield-types]

Recommended summit geometry:

```text
rim_outer_radius = 450–850 m
crater_floor_radius = 180–450 m
rim_height_bonus = +80 to +180 m above local shoulder
crater_floor_drop = -80 to -250 m below rim
crater_floor_min_elevation = 950–1250 m
secondary vents = 3–12 local features along rift/ridge axes
```

## Macro Terrain Primitive Inventory

The first large implementation should include these primitives as generated data, not manual sculpting.

### 1. Organic island mask

Purpose:

- defines land/ocean classification;
- controls sea-level transition;
- prevents square-world terrain artifacts.

Output masks:

```text
ocean_mask
shelf_mask
shoreline_mask
beach_mask
coastal_flat_mask
land_mask
distance_to_shore_m
normalized_island_radius
```

### 2. Volcanic shield massif

Purpose:

- supplies the macro relief and `~1400 m` target;
- creates the whole-island drainage gradient.

Output:

```text
base_shield_height_m
massif_zone_mask
upper_shoulder_mask
mid_flank_mask
lower_flank_mask
```

### 3. Summit crater and rim

Purpose:

- establishes active volcanic identity;
- creates high-altitude hard terrain;
- supports later non-terrain systems without implementing them now.

Output:

```text
crater_floor_mask
crater_rim_mask
crater_wall_mask
summit_blocker_mask
```

### 4. Rift axes and vent chains

Purpose:

- create geologic structure beyond random radial noise;
- place secondary vents and lava-flow corridors along plausible paths.

Output:

```text
rift_axis_mask
vent_mask
lava_source_mask
lava_flow_corridor_mask
```

### 5. Radial ridges

Purpose:

- divide catchments;
- create navigable spines and hard/blocking crests;
- make the island read as volcanic from topographic maps.

Output:

```text
ridge_mask
ridge_crest_mask
ridge_shoulders_mask
ridge_blocker_mask
```

### 6. Gullies and ravines

Purpose:

- incise tropical volcanic flanks;
- create local terrain challenge;
- feed catchment maps and alluvial/coastal fans.

Output:

```text
gully_mask
ravine_mask
thalweg_mask
incision_depth_m
```

### 7. Catchment basins

Purpose:

- validate hydrologic structure;
- prevent arbitrary noise terrain;
- assign gullies to coast outlets.

Output:

```text
catchment_id_map
flow_direction_map
flow_accumulation_map
outlet_points
pit_sink_map
```

DEM-derived flow direction and flow accumulation are standard terrain-analysis products; D8 and D-infinity style approaches are common options for raster terrain hydrology.[^taudem-docs][^arcgis-flow-direction]

### 8. Coastal benches, beaches, and fans

Purpose:

- make the island edge believable;
- provide low terrain and shoreline continuity;
- prevent hard circular rim artifacts.

Output:

```text
bench_mask
beach_mask
berm_mask
fan_mask
coastal_flat_mask
shoreline_zero_crossing
```

### 9. Lava/crust hazard terrain

Purpose:

- reserve terrain-only geometry for later hazard systems;
- create blocky, uneven, fractured terrain masks.

Output:

```text
lava_crust_mask
lava_channel_mask
unstable_surface_mask
blocky_lava_mask
```

No VFX, damage system, final material, or gameplay hazard behavior should be implemented in this terrain batch. Only geometry and classification masks.

### 10. Hard blockers

Purpose:

- create natural terrain limits without UI or invisible arbitrary walls;
- ensure dangerous terrain is tagged and measurable.

Output:

```text
hard_blocker_mask
slope_blocker_mask
cliff_mask
crater_wall_blocker_mask
coastal_cliff_mask
lava_blocker_mask
```

Hard blockers should be terrain-derived and explainable: crater walls, collapse scarps, sea cliffs, ravine walls, unstable lava ridges, or very steep ridge crests.

## Elevation, Relief, And Slope Targets

Replace the current low-relief shell with a target range appropriate for a compact active volcanic island.

Recommended elevation targets:

| Terrain class | Elevation range |
|---|---:|
| Outer ocean / square edge | `-250 m` to `-40 m` |
| Nearshore shelf | `-40 m` to `-1 m` |
| Beach / swash / shoreline | `-1 m` to `+4 m` |
| Berm / backshore | `+2 m` to `+12 m` |
| Coastal flats | `+5 m` to `+80 m` |
| Lower flank | `+60 m` to `+250 m` |
| Mid flank | `+180 m` to `+900 m` |
| Upper shoulder | `+700 m` to `+1,250 m` |
| Crater floor | `+950 m` to `+1,250 m` |
| Crater rim / summit | `+1,250 m` to `+1,450 m` |

Recommended terrain-wide elevation acceptance:

```text
min_elevation_m <= -120
max_elevation_m between 1350 and 1450
shoreline median elevation between -0.5 and +0.5
beach band p90 elevation <= +8
square edge median elevation <= -40
```

Recommended slope bands for developer classification:

| Slope band | Degrees | Terrain meaning |
|---|---:|---|
| Flat / easy | `0–5°` | beaches, flats, benches, crater floor patches |
| Walkable / normal | `5–15°` | low flank, broad ridges, inland approach terrain |
| Steep / careful | `15–30°` | volcanic shoulders, gully approaches, ridge flanks |
| Dangerous | `30–45°` | ravine sides, crater walls, lava scarps, steep ridges |
| Hard blocker | `>45°` | cliffs, crater inner walls, collapse scarps, sea cliffs |

These slope bands are implementation/gameplay classification targets, not claims that every real shield volcano uses the same thresholds. Shield volcanoes are generally broad and gently sloping at macro scale, but a compressed game island needs local steeper terrain to support blockers, gullies, craters, and volcanic hazard geometry.[^nps-shield][^usgs-hawaii-revealed]

Suggested land-only slope distribution:

```text
0–5°:        8–20% of land samples
5–15°:       25–45%
15–30°:      25–45%
30–45°:      8–20%
>45°:        1–8%, all explainable and tagged
```

Acceptance conditions:

- no untagged `>45°` slopes;
- no broad coastline ring above `15°`;
- crater/ravine/cliff blockers are tagged by cause;
- beach/coastal shelf bands are predominantly low slope;
- the massif reaches target elevation without making the entire island uniformly steep.

## Coastline / Beach / Shelf Terrain Geometry

The coastline must be explicitly generated as a profile, not created incidentally by clipping a noise field.

Recommended signed-distance approach:

```text
d = R(theta) - r

d < -700 m       -> outer ocean
-700 <= d < -180 -> submarine shelf / nearshore slope
-180 <= d < -20  -> shallow shelf approaching sea level
-20 <= d <= 80   -> swash / beach face / sea-level crossing
80 < d <= 220    -> berm / backshore
220 < d <= 900   -> coastal flats / low volcanic lowland
d > 900          -> inland volcanic profile
```

Height behavior:

```text
if d < -700:
    H = -60 to -250 m, deepening toward square edge

if -700 <= d < -180:
    H = smooth transition from about -60 m to -8 m

if -180 <= d < -20:
    H = smooth transition from about -8 m to -1 m

if -20 <= d <= 80:
    H = -1 m to +4 m, low slope, beach mask

if 80 < d <= 220:
    H = +2 m to +10 m, berm/backshore mask

if 220 < d <= 900:
    H = +5 m to +80 m, coastal flats with mild undulation
```

The sea-level zero crossing should be continuous and testable:

```text
sample shoreline at N = 2048 angular points
for each theta, find r where H(r, theta) crosses 0
assert abs(r_zero - R(theta)) <= 40 m for at least 95% of samples
assert beach band slope_p90 <= 8°
assert no hard blockers inside beach band unless explicitly marked as rare rocky headland
```

Recommended coastal variation:

- `70–85%` of shoreline should be beach/shelf/coastal-flat transition.
- `10–25%` may be rocky headland or lava bench, still resolving to sea level.
- `0–5%` may be steep sea cliff, only where tagged as volcanic cliff/headland, never as the default circular edge.
- All coastal cliffs must be local features, not the island mask itself.

Volcanic islands can have steep submarine slopes and lava-ocean interaction, but the playable island edge still needs readable above-water beach/shelf geometry rather than a high perimeter wall.[^usgs-hawaii-revealed]

## Inland Rise And Lowland Terrain Geometry

The inland rise should make the island feel volcanic while preserving coastal traversal and terrain readability.

Recommended inland profile:

```text
rho = r / R(theta)
shore_distance_m = R(theta) - r
coast_inland_t = saturate((shore_distance_m - 220) / 900)
volcanic_t = saturate((1 - rho) / 0.82)
```

A useful base profile:

```text
H_base =
    coast_profile(d)
  + shield_profile(rho)
  + shoulder_profile(rho)
  + structural_ridge_bonus
  - gully_incision
  + crater_rim_bonus
  - crater_floor_depression
  + lava_bench_adjustments
```

Example shield profile shape:

```text
shield_profile_m = peak_m * pow(saturate(1 - pow(rho, 1.55)), 1.18)
```

Then clamp/shape by coastal band so the profile always returns to sea level at the shoreline:

```text
H_land = mix(coast_profile(d), shield_profile_m, inland_weight)
```

The lowland should not be flat noise. It should include:

- broad volcanic toes;
- coastal benches;
- alluvial/lava fans at gully outlets;
- old lava-flow terraces;
- gentle inland rise;
- occasional local ridges/headlands.

Recommended lowland geometry:

| Feature | Width/scale | Height behavior |
|---|---:|---|
| Coastal flats | `350–900 m` | `+5` to `+80 m` |
| Volcanic toes | `500–1,500 m` | smooth rise into lower flank |
| Alluvial/lava fans | `200–800 m` radius | convex fan at gully outlets |
| Coastal benches | `100–500 m` wide | near-level shelves at `+5` to `+40 m` |
| Headlands | local | low cliffs/rocky benches, sea-level base |

Acceptance checks:

```text
coastal_flat_mask exists and covers continuous sectors
mean coastal-flat slope <= 8°
coastal-flat elevation p90 <= 90 m
no coastal-flat hard blockers except tagged headlands/scarps
```

## Volcanic Ridge / Gully / Catchment Geometry

The terrain should be organized by catchments, not random roughness.

Recommended catchment count:

```text
primary catchments: 10–18
secondary gullies: 24–60
major radial ridges: 8–16
rift-dominant ridge axes: 2–3
```

### Radial ridge generation

Ridges should start near the upper shoulder and extend toward the lower flank, fading before beaches unless they become headlands.

Recommended ridge fields:

```text
ridge_angle_i = deterministic angular seed
ridge_curve_i(r) = theta_i + low_frequency_curve(r)
ridge_width_m = 80–350 m
ridge_height_bonus = +20 to +180 m
ridge_fade_upper = fades near crater unless attached to rim
ridge_fade_lower = fades into fans/benches/coastal flats
```

Ridge masks:

```text
ridge_core: high, narrow crest
ridge_shoulder: broader flank
ridge_blocker: only where slope or curvature exceeds threshold
```

### Gully generation

Gullies should be valley incisions between ridges and should drain downslope toward the coast.

Recommended gully fields:

```text
gully_count = 24–60
major_gully_count = 8–16
gully_width_m = 30–180 m source field, but rendered by heightfield resolution
major_gully_depth_m = 30–180 m
minor_gully_depth_m = 8–60 m
gully_head_elevation = 450–1100 m
gully_outlet = coastal fan/bench/beach sector
```

The current `129 x 129` mesh cannot faithfully render `30–80 m` gullies because spacing is about `127 m`. Therefore the generator must represent gullies in the authoritative heightfield/masks even if the debug render downsample makes them broad.

### Catchment validation

Required diagnostic products:

```text
catchment_id_map.png
flow_direction_map.png
flow_accumulation_map.png
gully_thalweg_map.png
outlet_points.csv
pit_sink_report.csv
```

Pass/fail targets:

```text
>= 10 primary catchments reach coastline
>= 90% of land samples have a valid downslope path to coast or tagged basin
all unfilled sinks are tagged as crater, lava pond, closed basin, or debug failure
major gullies align with catchment flow accumulation
ridge crests separate adjacent catchment ids
```

Use D8 or D-infinity for developer diagnostics. D8 is simpler and sufficient for a first PR; D-infinity or multiple-flow-direction can be added later if D8 creates too much angular stair-stepping.[^taudem-docs][^arcgis-flow-direction]

## Crater / Vent / Lava-Crust Terrain Geometry

The summit crater/vent complex should be terrain-first. Do not implement final smoke, gas, fire, heat, damage, audio, or VFX.

### Crater/rim

Recommended crater model:

```text
crater_center = massif_center + small offset
crater_outer_radius = 500–900 m
crater_floor_radius = 180–450 m
rim_width = 100–250 m
rim_bonus_m = +80 to +180
floor_drop_m = -80 to -250
inner_wall_slope_target = 30–60°
outer_wall_slope_target = 15–35°
```

Required masks:

```text
crater_floor_mask
crater_inner_wall_mask
crater_rim_mask
crater_outer_slope_mask
crater_blocker_mask
```

### Vents and rift chain

Place vents along `2–3` deterministic rift axes. Vents should be topographic features: cones, pits, cracks, raised rims, local lava benches, and channel heads.

Recommended vents:

```text
summit vents: 2–5
flank vents: 4–12
vent_radius_m = 30–180
vent_height_bonus_m = +5 to +80
vent_depression_m = -5 to -80
rift_axis_width_m = 80–250
```

### Lava/crust terrain

Implement as heightfield geometry and masks only:

```text
lava_crust_roughness_m = 1–12 m at high-res source
lava_bench_height_m = +2 to +30 m local
lava_channel_depth_m = 2–20 m
lava_channel_width_m = 20–120 m
```

Runtime caveat:

- High-frequency lava crust roughness should not be baked into a `129 x 129` mesh. At that resolution it aliases into lumpy terrain.
- Store it in the source terrain/masks now.
- Display it only when the runtime mesh or landscape resolution can support it.
- Use debug material/mask overlays for proof.

No final hazard behavior should be implemented. Terrain-only output is enough:

```text
unstable_lava_surface_mask
sharp_crust_mask
lava_channel_mask
vent_field_mask
thermal_terrain_reserved_mask
```

## Terrain Generation Implementation Guidance

### Required architecture

Implement the generator as a deterministic pipeline:

```text
TerrainSeed
WorldConfig
IslandMaskConfig
CoastProfileConfig
MassifProfileConfig
CraterConfig
RidgeConfig
GullyConfig
CatchmentConfig
VentConfig
LavaTerrainConfig
DiagnosticsConfig
```

Preferred pipeline stages:

1. Build coordinate grid in meters.
2. Compute organic island radius `R(theta)`.
3. Compute signed shoreline distance.
4. Generate ocean/shelf/beach/coastal-flat base profile.
5. Generate shield/massif profile.
6. Add crater/rim/vent/rift structural fields.
7. Add radial ridges.
8. Subtract gullies/ravines.
9. Add coastal benches/fans.
10. Generate lava/crust microterrain masks.
11. Apply terrain-class-aware smoothing/clamping.
12. Compute slope, curvature, flow direction, flow accumulation, catchments.
13. Emit heightfield, masks, summaries, and debug images.
14. Downsample or sample into runtime mesh/landscape representation.

### Determinism requirements

Every generated artifact must be reproducible from:

```text
terrain_version
seed
world_size_m
island_config
massif_config
generator_git_commit
```

The PR should write debug metadata:

```json
{
  "terrain_version": "JG_VOLCANIC_ISLAND_TERRAIN_001",
  "seed": 12345,
  "world_size_m": 16256.0,
  "sea_level_m": 0.0,
  "mean_island_radius_m": 7000.0,
  "max_effective_radius_m": 7350.0,
  "peak_elevation_m": 1400.0,
  "source_grid_resolution": "2017x2017 or higher recommended",
  "runtime_preview_resolution": "129x129 or 257x257 acceptable for macro preview only"
}
```

### Heightfield / mesh resolution guidance

Use an authoritative heightfield separate from the preview mesh.

Resolution implications:

| Resolution | Approx spacing over 16.256 km | Use |
|---|---:|---|
| `129 x 129` | `127.0 m` | macro silhouette only; current proof |
| `257 x 257` | `63.5 m` | rough ridges and broad gullies |
| `513 x 513` | `31.75 m` | readable major gullies/ridges |
| `1009 x 1009` | `~16.13 m` | UE-friendly mid-resolution debug size |
| `2017 x 2017` | `~8.06 m` | recommended source/debug target for major PR |
| `4033 x 4033` | `~4.03 m` | strong future high-res target |

Epic's Landscape Technical Guide lists valid/recommended landscape sizes such as `1009 x 1009`, `2017 x 2017`, and `4033 x 4033`, and explains heightmap dimension and Z-scale constraints.[^epic-landscape] If using UE Landscape import later, prefer those valid dimensions. If using runtime procedural mesh, keep the generator dimension independent and resample to render/LOD meshes.

The UE Procedural Mesh Component is a renderable component for procedurally creating and modifying mesh geometry, but terrain data should remain engine-agnostic enough that the same heightfield can feed procedural mesh, debug rasters, or future Landscape import.[^epic-procedural-mesh]

Recommended PR target:

```text
authoritative terrain source: 2017 x 2017 heightfield minimum
runtime preview mesh: 129 x 129 preserved only if needed
better runtime preview: 257 x 257 or 513 x 513 if performance allows
diagnostics: emitted at source resolution and thumbnail resolution
```

### Height encoding

Use meters internally. Convert to UE centimeters only at mesh/export boundary.

```text
internal_height_unit = meters
ue_position_z = height_m * 100.0
sea_level_m = 0.0
```

If exporting to UE Landscape heightmaps, calculate Z scale intentionally. Epic documents Unreal Landscape height values over a `-256` to `+255.992` range multiplied by Z scale, with custom height ranges requiring conversion to centimeters and scaling.[^epic-landscape]

For this terrain, reserve below-sea and above-summit headroom:

```text
target_vertical_range_m = -256 to +1536, or similar
useful_height_span_m = about 1792 m
sea_level_not_at_min = true
```

### Terrain class masks

Each sample should be able to answer:

```text
height_m
slope_degrees
terrain_class_primary
terrain_class_flags[]
distance_to_shore_m
catchment_id
flow_accumulation
is_blocker
is_developer_only_hazard_reserved
```

Suggested primary class enum:

```text
Ocean
Shelf
Beach
Berm
CoastalFlat
LowerFlank
MidFlank
UpperShoulder
Ridge
Gully
CraterFloor
CraterWall
CraterRim
Vent
LavaCrust
Fan
Cliff
HardBlocker
```

A sample can also carry secondary flags, because a ridge may also be a hard blocker, or a vent may sit inside a lava-crust field.

### Noise usage

Noise is allowed only as a detail layer after macro structure exists.

Allowed:

- low-frequency shoreline perturbation;
- low-frequency massif asymmetry;
- medium-scale ridge/gully variation;
- high-resolution lava crust roughness if masked and resolution-aware.

Forbidden:

- Perlin/noise-only terrain as the island generator;
- noise clipping as a coastline;
- random terrain that produces unexplainable blockers;
- noise that breaks beach/shelf sea-level continuity.

## Developer-Only Terrain Diagnostics

Diagnostics are mandatory acceptance artifacts, not optional screenshots.

DEM/heightfield products should follow real terrain-analysis practice: a DEM is a bare-earth topographic surface representation, and DEMs are commonly used to analyze topography, flow, flood risk, terrain, and hazards.[^usgs-dem][^nasa-dem]

Required output folder:

```text
Saved/TerrainDiagnostics/JG_VOLCANIC_ISLAND_TERRAIN_001/
```

Required files:

```text
terrain_metadata.json
heightmap_m.png
heightmap_m.r16 or .exr if available
slope_degrees.png
slope_histogram.csv
terrain_class_map.png
island_mask.png
shoreline_error_map.png
coast_profile_samples.csv
ocean_depth_map.png
beach_shelf_mask.png
ridge_gully_map.png
catchment_id_map.png
flow_direction_map.png
flow_accumulation_map.png
pit_sink_report.csv
blocker_mask.png
crater_vent_mask.png
lava_crust_mask.png
acceptance_summary.md
```

Required numeric report fields:

```text
min_elevation_m
max_elevation_m
mean_land_elevation_m
median_land_elevation_m
shoreline_median_error_m
shoreline_p95_error_m
square_edge_median_elevation_m
land_area_m2
ocean_area_m2
beach_area_m2
coastal_flat_area_m2
slope_band_percentages
hard_blocker_percentage
catchment_count
catchments_reaching_coast
untagged_sink_count
untagged_hard_slope_count
```

Suggested pass/fail thresholds:

| Check | Pass threshold |
|---|---:|
| Max elevation | `1350–1450 m` |
| Min elevation | `<= -120 m` |
| Square edge median elevation | `<= -40 m` |
| Shoreline median error | `<= 1 m` |
| Shoreline p95 error | `<= 8 m` |
| Beach band slope p90 | `<= 8°` |
| Coastal-flat elevation p90 | `<= 90 m` |
| Hard blocker land coverage | `1–8%` |
| Untagged hard slopes | `0` |
| Primary catchments reaching coast | `>= 10` |
| Untagged sinks/pits | `0` |
| Ocean samples outside mask above sea level | `< 0.5%` |
| Land samples inside shoreline below sea level | allowed only in tagged crater/lake/basin masks, otherwise `< 0.5%` |

Developer-only visual checks:

1. Heightmap shows circular/organic island, not square land.
2. Ocean margin is visible at all square edges.
3. Shoreline is continuous and low.
4. Beach/shelf ring is readable.
5. Massif reaches the correct height without a spike.
6. Ridges are radial but not mechanically perfect.
7. Gullies drain between ridges toward coast.
8. Catchment map has coherent basins.
9. Crater/vent terrain is visible in the summit diagnostics.
10. Hard blockers are explainable and not scattered noise.

## Large PR Acceptance Criteria

A PR implementing this batch is acceptable only if all of the following are true.

### Functional terrain generation

- Generates a deterministic `16.256 km x 16.256 km` terrain.
- Uses `0 m` sea level.
- Produces a circular/organic island inside the square world.
- Leaves the square world boundary as ocean/below sea level.
- Produces a continuous sea-level shoreline, beach/shelf band, and coastal flats.
- Reaches approximately `1400 m` maximum elevation.
- Includes a central volcanic massif, summit crater/rim, shoulders, ridges, gullies, catchments, vents, coastal benches, fans, lava/crust masks, and hard-blocker masks.
- Does not depend on foliage, final materials, weather, ecology, audio, VFX, story, UI, or player-facing navigation tools.

### Data architecture

- Stores terrain height in meters internally.
- Stores deterministic seed/config/version metadata.
- Separates source terrain resolution from runtime preview mesh resolution.
- Emits masks for terrain classes.
- Emits slope/catchment/blocker diagnostics.
- Provides a clear path to higher resolution without rewriting the generator.

### Coastline correctness

- Outside island mask is ocean/below sea level.
- Island edge resolves to sea level and beach/coastal shelf.
- No tall wall at the perimeter.
- No square-edge landmass.
- No random raised terrain at the circle edge.
- Local rocky headlands are allowed only when tagged and minority coverage.

### Slope/blocker correctness

- Slope bands are computed.
- Hard blockers are terrain-derived and tagged.
- Untagged `>45°` slopes fail the PR.
- Beach and coastal-flat regions are mostly low slope.
- Crater/ravine/cliff blockers are explainable from masks.

### Diagnostic evidence

- Required diagnostic files are generated.
- Acceptance summary includes numeric pass/fail values.
- At least one full-domain topographic debug image is attached or generated.
- Developer-only maps do not leak into player-facing UI.

### Regression compatibility

- Existing full-size terrain shell proof is either replaced cleanly or retained as a selectable debug baseline.
- Runtime token/version is updated so test logs make clear which terrain generator produced the result.
- The PR does not silently keep the old `-101 m to +140 m`, `0–7°` terrain as the main output.

## Risks And Stop Conditions

Stop implementation and report a blocker if any of these occur:

1. **Coastline failure.** The island edge becomes a raised circular wall, square-edge cliff, or arbitrary mask cutoff.
2. **Resolution mismatch.** The implementation claims to support gullies, beaches, or vents but only stores them in a `129 x 129` source mesh.
3. **No diagnostic output.** Visual inspection alone is not enough.
4. **No sea-level contract.** Sea level must be `0 m` and all coast bands must respect it.
5. **No source/runtime separation.** The generator should not be permanently bound to a coarse preview mesh.
6. **Noise-only terrain.** If landforms are not explainable by mask/profile/ridge/gully/catchment logic, stop.
7. **Player UI creep.** Any player-facing map, compass, GPS, objective marker, bearing display, minimap, time readout, or orientation meter is out of scope.
8. **Non-terrain system creep.** Foliage, ecology, weather, animals, story, survival, ambience, final VFX, and final materials must stay deferred.
9. **Unbounded blockers.** Hard blockers must be terrain-derived and tagged; random invisible walls or unexplained cliffs fail.
10. **No acceptance summary.** The PR must produce numeric pass/fail evidence.

## Rejected Or Forbidden Approaches

Reject these approaches:

- square world edges used as cliffs or terrain boundaries;
- square island, diamond island, or edge-to-edge landmass;
- circular terrain plug with a raised rim;
- coastline created by clipping a noise field;
- Perlin/noise-only island generator;
- single sharp volcano cone in the middle of the map;
- flat island with decorative volcano mesh on top;
- hard blockers implemented as invisible walls rather than terrain;
- player-facing navigation UI;
- final foliage/ecology/weather/audio/story/survival/VFX/material implementation;
- asset-pack dressing as a substitute for terrain generation;
- diagnostics that only show the camera view, not the terrain fields;
- any claim that the terrain is complete without height, slope, mask, catchment, and coastline debug evidence.

## Sources And References

[^usgs-shield-types]: U.S. Geological Survey. **Principal Types of Volcanoes — Shield Volcanoes.** Shield volcanoes are described as broad, gently sloping domical forms built from repeated fluid basalt lava flows from central vents and flank/rift-zone vents. https://pubs.usgs.gov/gip/volc/types.html

[^nps-shield]: National Park Service. **Shield Volcanoes.** Overview of shield volcano form and broad gentle slopes. https://www.nps.gov/articles/000/shield-volcanoes.htm

[^usgs-hawaii-revealed]: U.S. Geological Survey. **Hawaii's Volcanoes Revealed.** Describes above-sea-level shield profiles, lava-ocean interaction, steep submarine slopes, slumping, and coastal/submarine volcanic morphology. https://www.usgs.gov/maps/hawaiis-volcanoes-revealed

[^usgs-evolution]: U.S. Geological Survey Hawaiian Volcano Observatory. **Evolution of Hawaiian Volcanoes.** Describes volcanic stages, rejuvenated activity, shoreline eruptions, maars, and lava flows down eroded stream valleys. https://www.usgs.gov/observatories/hvo/evolution-hawaiian-volcanoes

[^coastalwiki-beach]: Coastal Wiki. **Definitions of Coastal Terms.** Definitions for beach berm, beach face/foreshore, backshore, intertidal bars, runnels, and related coastal morphology. https://www.coastalwiki.org/wiki/Definitions_of_coastal_terms

[^opengeology-shorelines]: Johnson, Affolter, Inkenbrandt, and Mosher. **An Introduction to Geology — Shorelines.** Educational summary of beach profile zones including offshore, nearshore, foreshore, and backshore. https://opengeology.org/textbook/12-shorelines/

[^usgs-dem]: U.S. Geological Survey. **What is a digital elevation model (DEM)?** Defines DEMs as bare-earth topographic surface representations. https://www.usgs.gov/faqs/what-a-digital-elevation-model-dem

[^nasa-dem]: NASA Earthdata. **Digital Elevation/Terrain Model (DEM).** Describes DEMs as bare-earth terrain models useful for terrain, flow, flood-risk, and hazard analysis. https://www.earthdata.nasa.gov/topics/land-surface/digital-elevation-terrain-model-dem

[^taudem-docs]: Utah State University Hydrology Research Group. **Terrain Analysis Using Digital Elevation Models (TauDEM) Documentation.** Documents DEM-based flow direction, contributing area, watershed delineation, and D8/D-infinity tools. https://hydrology.usu.edu/taudem/taudem5/documentation.html

[^arcgis-flow-direction]: Esri ArcGIS Pro Documentation. **Flow Direction.** Documents D8, Multiple Flow Direction, and D-Infinity flow methods; D8 models flow to the steepest downslope neighbor. https://pro.arcgis.com/en/pro-app/latest/tool-reference/spatial-analyst/flow-direction.htm

[^epic-landscape]: Epic Games. **Landscape Technical Guide in Unreal Engine.** Documents Landscape heightmap dimension constraints, Z-scale conversion, and recommended Landscape sizes including `1009 x 1009`, `2017 x 2017`, and `4033 x 4033`. https://dev.epicgames.com/documentation/unreal-engine/landscape-technical-guide-in-unreal-engine

[^epic-procedural-mesh]: Epic Games. **Procedural Mesh Component API.** Describes the Procedural Mesh Component as a renderable component and utilities for creating and modifying mesh geometry procedurally. https://dev.epicgames.com/documentation/unreal-engine/API/PluginIndex/ProceduralMeshComponent
