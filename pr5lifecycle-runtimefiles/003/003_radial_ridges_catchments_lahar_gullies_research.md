# Radial Ridges, Catchments, And Lahar Gully Terrain Research

## Executive Summary

PR5 Batch 003 should replace the current smooth terrain shell with a deterministic volcanic ridge-and-drainage system centered on a roughly **1400 m active massif** inside the **16.256 km square world**. The terrain should read as a young-to-mature volcanic island: radial ridges descend from the volcanic high, catchments form between ridge arms, gullies and lahar corridors incise the flanks, fan deposits build at slope breaks, and every major drainage path resolves coherently into the sea-level coast.

The recommended target is:

- **14 major radial ridges** descending from the central massif.
- **14 primary catchments**, one between each adjacent ridge pair.
- **5-7 lahar-capable trunk gullies** selected from the larger catchments.
- **2-3 major landslide/scar amphitheaters** on upper-to-mid flanks.
- **10-14 coastal outlet/fan systems**, with every primary drainage basin terminating at a beach break, creek mouth, coastal fan, or low shelf notch.
- A full set of developer-only outputs: ridge IDs, basin IDs, thalwegs, flow accumulation, slope class, gully depth, lahar corridor, fan deposit, outlet ID, coast termination, pit/sink, and walkability masks.

The core rule is: **ridges define divides; catchments define flow; gullies carve down existing drainage; fans deposit at slope breaks; the coast consumes every drainage line cleanly.**

## Terrain-Only Scope Boundary

This specification covers only terrain geometry and terrain-derived developer masks.

In scope:

- Heightfield generation.
- Ridge arms, spurs, shoulders, saddles, and divide geometry.
- Catchment basin layout.
- Creek/gully/lahar incision.
- Landslide scar geometry.
- Boulder/alluvial fan and creek-mouth terrain.
- Sea-level beach/shelf/coastal outlet termination.
- Slope, flow, catchment, and topographic diagnostic outputs.
- Walkable versus dangerous versus hard-blocking terrain classes.

Out of scope:

- Foliage, ecology, biomes, weather, audio, VFX, survival mechanics, NPCs, story, UI, map/minimap/compass/GPS/objective tools, water rendering, final materials, or final lahar/water simulation.
- Player-facing orientation or navigation UI.
- Decorative rivers without terrain incision.
- Any final gameplay hazard design beyond terrain slope/blocker masks.

Water, creeks, lahars, and drainage are discussed here only as **terrain incision, drainage paths, fan deposition, slope geometry, and masks**.

## Current Terrain Baseline

The current full-size terrain shell exists, but the prompt identifies it as:

- too smooth;
- too low;
- insufficiently volcanic;
- missing realistic radial ridge structure;
- missing coherent drainage;
- missing incised gullies/lahar channels;
- missing sea-level catchment termination.

Assumption for implementation: the existing world already occupies the correct **16.256 km square** footprint. This PR should not expand the world. It should replace or augment the terrain-generation pass that currently produces the smooth island shell.

For Unreal Landscape sizing, **8129 x 8129 vertices** is the clean high-resolution target for this world if the project is using 2 m terrain spacing: `8128 quads * 2 m = 16,256 m`. Epic's Landscape Technical Guide lists 8129 x 8129 as a recommended Landscape size and documents the 16-bit heightmap/Z-scale workflow used for large imported landscapes. [Epic Games, Landscape Technical Guide](https://dev.epicgames.com/documentation/unreal-engine/landscape-technical-guide-in-unreal-engine)

If the current project uses a tiled landscape or a lower working resolution, keep the same physical world size and apply this spec at the generator's canonical resolution, then downsample only after hydrologic validation.

## Major Implementation Goal

Build a deterministic volcanic island terrain system where the heightfield can prove the following:

1. The central massif reaches approximately **1350-1450 m**.
2. Major terrain divides are visible as radial ridges, not noise artifacts.
3. Basins between ridges drain downslope into identifiable channels.
4. Gully incision increases with flow accumulation and local slope.
5. Lahar corridors are deeper, wider, and more erosive than ordinary creek gullies.
6. Landslide scars remove mass from steep upper/mid slopes and feed matching lower fan deposits.
7. Fan deposits appear where steep confined channels lose gradient and spread near lower slopes/coast.
8. Every major drainage line terminates into sea-level terrain at a beach break, creek mouth, fan toe, or coastal shelf.
9. No ridge, gully, fan, or high terrain continues to the square-world edge.
10. Developer diagnostics can show ridges, catchments, flows, outlets, and bad drainage failures at a glance.

## Sea-Level Coast And Drainage Termination Requirements

The coastline requirement is a hard acceptance gate.

### Required coast model

Use three concentric/organic terrain zones:

| Zone | Approximate elevation | Purpose |
|---|---:|---|
| Ocean outside island mask | -40 m to -2 m | Guarantees square-world edge is ocean/below sea level. |
| Coastal shelf / beach ring | -2 m to +8 m | Low-gradient perimeter terrain, beach breaks, creek mouths. |
| Low coastal land / fan toes | +5 m to +80 m | Ridge toes fade, channels spread, fans merge into coast. |

The island mask should be circular/organic, not a perfect circle. Use a deterministic radial radius function:

```text
R_island(theta) = R_base
                + low_frequency_noise(theta)
                + 2-4 broad headland/bay harmonics
                - outlet_notches(theta)
```

Recommended values:

- `R_base`: 6.9-7.25 km from island center.
- `R_min`: no less than 6.25 km unless intentionally forming a bay.
- `R_max`: no more than 7.65 km, leaving ocean outside the mask inside the 8.128 km half-world.
- Coast/beach ring width: 180-420 m.
- Shelf gradient: normally 0.5-4 degrees.
- Coastal fan toe gradient: normally 2-8 degrees.

### Required drainage outlet model

Every primary catchment gets a deterministic outlet point on the beach/shelf ring.

For each catchment `i`:

1. Compute the angular sector between ridge `i` and ridge `i+1`.
2. Choose one outlet angle near the basin centerline, jittered by a seeded offset.
3. Snap the lower thalweg to this outlet.
4. Carve a low-gradient terminal channel from the final fan apex to the beach break.
5. Lower the beach locally at the creek mouth by 0.5-3 m so the drainage visually reaches sea level.
6. Blend the creek-mouth notch into a fan toe and coastal shelf, not a trench that cuts through the whole beach ring.

No drainage line may end randomly inland, on a dune/beach ridge, or inside a flat ring depression.

### Required ridge-to-coast fade

All ridge contribution masks must fade before the beach/shelf:

```text
ridge_strength *= smoothstep(R_island - 0.85 km, R_island - 1.65 km, radial_distance)
ridge_strength *= coastal_lowland_suppression
```

Meaning:

- Major ridge crests can descend into low coastal toes.
- Ridge toes should become broad shoulders, knolls, or low divides.
- No ridge crest should remain as raised geometry into the beach ring.
- No ridge mask should survive outside the island mask.

## Volcanic Ridge And Drainage Principles

Volcanic cones commonly develop **radial drainage** because streams flow outward from a central high point. Britannica summarizes radial drainage as typical of volcanic cones while the cone remains relatively intact. [Britannica, Radial Drainage Pattern](https://www.britannica.com/science/radial-drainage-pattern)

For Jungle Game, the massif should not be a perfect mathematical cone. It should be an eroded active volcanic island with:

- dominant radial flow;
- irregular ridge arms;
- catchment asymmetry;
- headward-eroding gullies;
- landslide scars;
- lahar corridors following valleys;
- lower-slope fan deposition;
- beach/shelf termination.

USGS describes lahars as volcanic mudflows/debris flows that move downslope through valleys and river channels, entraining sediment and depositing load as they decelerate in lower-gradient areas. That supports carving lahar corridors along drainage lines and widening/depositing them near lower slopes and fans rather than placing them as arbitrary straight grooves. [USGS, Lahars Move Rapidly Down Valleys Like Rivers of Concrete](https://www.usgs.gov/programs/VHP/lahars-move-rapidly-down-valleys-rivers-concrete)

The National Academies' alluvial fan reference describes fans forming where streams or debris flows leave steep, confined channels and enter zones of lower transport capacity due to reduced gradient, wider flow, or less confinement. That directly maps to fan generation at volcanic lower-slope breaks, valley mouths, and creek-mouth terrain. [National Research Council, Alluvial Fan Flooding](https://www.nationalacademies.org/read/5364/chapter/4)

## Recommended Ridge Network

### Major ridge count

Use **14 major radial ridges**.

Why 14:

- A 16.256 km square can comfortably hold a 13-15 km wide island.
- At a coastal radius of roughly 7.1 km, circumference is about 44.6 km.
- Fourteen primary ridges produce average coastal sector spacing of roughly 3.2 km.
- At mid-slope radius of 4.2 km, average spacing is roughly 1.9 km.
- This creates playable basins large enough to read as valleys, without making the volcano look like a saw blade or perfect wheel.
- An even number allows opposite-side macrostructure, while seeded angular jitter prevents artificial symmetry.

### Ridge class distribution

Use 14 ridges with varied strength:

| Class | Count | Description |
|---|---:|---|
| Dominant ridges | 5 | Longest, most persistent divides; visible from upper flank to lowland. |
| Standard ridges | 6 | Normal primary divides with branching spurs and basin separation. |
| Broken/truncated ridges | 3 | Interrupted by scars, saddles, or large gullies; still valid basin boundaries. |

All 14 must contribute to drainage partitioning. Not all 14 need to be equally tall or continuous.

### Seeded angular placement

Generate base angles as 14 near-even sectors with deterministic jitter:

```text
angle_i = i * (360 / 14) + jitter(seed, i, -7°, +7°)
```

Then apply high-level art-direction constraints:

- Avoid more than two adjacent weak ridges.
- Place at least one dominant ridge per quadrant.
- Place 2-3 broken/truncated ridges near planned landslide scars.
- Avoid exact north/east/south/west symmetry unless the island silhouette already needs it.

Example angular layout:

```text
006°, 031°, 055°, 083°, 108°, 135°, 160°,
187°, 214°, 240°, 266°, 294°, 319°, 344°
```

These are example seed outputs, not mandatory hard-coded angles.

### Ridge length bands

| Elevation/radial band | Ridge behavior |
|---|---|
| Summit/core, 1200-1450 m | Ridges start as broad ribs/shoulders, not knife edges. |
| Upper flank, 800-1200 m | Crests become more defined; gullies begin between them. |
| Mid flank, 300-800 m | Strong ridge/catchment contrast; spurs and saddles increase. |
| Lower flank, 80-300 m | Ridge toes widen and degrade into low divides/knolls. |
| Coast, 0-80 m | Ridge masks fade; drainage and fan geometry dominate. |

### Ridge generation formula

Recommended ridge height contribution:

```text
ridge_height_i =
    ridge_strength_i
  * along_profile(t)
  * cross_section(distance_to_centerline, width(t), asymmetry)
  * erosion_breakup_noise
  * coastal_fade
```

Where:

- `t`: normalized distance along ridge from massif to coast.
- `along_profile(t)`: strong on upper/mid flanks, fading at summit merge and coast.
- `cross_section`: asymmetric convex/triangular shoulder, not a flat plateau.
- `width(t)`: broader lower down, narrower mid-flank, broad again near massif shoulders.
- `erosion_breakup_noise`: low-amplitude deterministic roughness.
- `coastal_fade`: hard guarantee that ridge strength approaches zero near beach/shelf.

Recommended ridge dimensions:

| Band | Crest/shoulder width | Relative relief above adjacent basin |
|---|---:|---:|
| Upper flank | 140-260 m | 80-220 m |
| Mid flank | 220-520 m | 60-180 m |
| Lower flank | 400-900 m | 15-80 m |
| Coast toe | 700-1200 m diffuse | 0-30 m |

## Spur / Saddle / Shoulder Geometry

Ridges must read as volcanic terrain, not roads. The implementation should avoid long flat crests, uniform-width berms, and perfectly smooth spines.

### Shoulders

A primary ridge should have:

- one crest line;
- asymmetric left/right shoulders;
- roughened convex side slopes;
- small sub-knobs;
- intermittent narrow saddles;
- local eroded bites where tributary gullies cut near the ridge.

Cross-section recommendation:

```text
crest core:       narrow convex crown
upper shoulder:   12-24° slope
side shoulder:    22-38° slope
eroded sidewall:  35-48° local sections
basin transition: concave blend into valley floor
```

Avoid:

- flat road-like crests wider than 20-40 m for long distances;
- constant elevation benches;
- ridge profiles with perfectly even grade;
- ridge centerlines that are straight rays.

### Spurs

Each major ridge should spawn **2-5 secondary spurs**, depending on length and class.

Spur rules:

- Spawn on mid/lower flanks, usually below 1150 m.
- Alternate sides irregularly.
- Branch away from the parent ridge by 20-50 degrees.
- Fade after 500-1800 m.
- Tie into catchment subdivision without stealing the primary basin outlet.
- Use local relief of 15-90 m above neighboring swales.
- Avoid parallel comb-tooth repetition.

Spur generation:

```text
for each major_ridge:
    candidate_t = seeded_points_between(0.25, 0.82)
    side = seeded_left_right_biased_by_basin_area()
    spur_angle = parent_angle + side * seeded_range(22°, 48°)
    spur_length = seeded_range(500 m, 1800 m)
    spur_strength = parent_strength * seeded_range(0.22, 0.55)
```

### Saddles

Saddles should be generated where:

- a ridge is interrupted by headward erosion;
- two spurs create a natural pass;
- a landslide scar clips a ridge shoulder;
- a high basin nearly captures another basin.

Recommended saddle geometry:

| Saddle type | Elevation drop below adjacent crests | Width | Use |
|---|---:|---:|---|
| Minor shoulder saddle | 10-30 m | 80-180 m | Natural crossing, not a road. |
| Basin divide saddle | 30-80 m | 120-300 m | Visible topographic low between catchments. |
| Scar-cut saddle | 60-160 m | 150-450 m | Major terrain event linked to scar/gully. |

Saddles should preserve flow separation unless intentionally marked as a capture/overflow feature. If a saddle lets flow leak into the wrong catchment, either raise the divide or intentionally document it as a captured drainage.

## Catchment And Creek-Incision Layout

### Primary catchments

The ridge network defines **14 primary catchments**. Each catchment lies between adjacent major ridges and owns one coastal outlet.

For each catchment:

- Assign `basin_id`.
- Generate a main thalweg from upper/mid flank to the coastal outlet.
- Add 3-9 tributary gullies depending on catchment size.
- Keep tributaries dendritic or feathered toward the trunk, not parallel stripes.
- Ensure the catchment has no internal sink that traps flow above sea level.
- Clip and blend basin floor to adjacent ridge shoulders.

### Basin geometry by elevation

| Elevation band | Basin/gully character |
|---|---|
| 1200-1450 m | Radial ribs, shallow swales, crater/summit roughness if present. |
| 900-1200 m | Headwater gullies begin, narrow and steep. |
| 500-900 m | Strongly incised V-shaped creek gullies; tributary joins. |
| 150-500 m | Wider valley floors, terraces, debris benches, lahar widening. |
| 0-150 m | Low-gradient fan, creek-mouth, beach/shelf transition. |

### Creek/gully incision strategy

Do not place gullies as texture lines. Carve them into the heightfield after ridge/catchment assignment.

Recommended pass order:

1. Generate base massif and island/coast mask.
2. Generate primary ridge centerlines and ridge height contribution.
3. Generate catchment sectors and basin floor bias.
4. Generate main thalweg lines to locked coastal outlets.
5. Generate tributary thalwegs using flow potential and seeded headwater points.
6. Carve ordinary creek gullies.
7. Carve lahar corridors and landslide scars.
8. Add fan/deposit lobes.
9. Hydrologically condition final terrain.
10. Emit diagnostics.

### Ordinary gully dimensions

| Elevation band | Depth | Top width | Bed width | Wall character |
|---|---:|---:|---:|---|
| 1000-1400 m | 8-35 m | 20-90 m | 2-12 m | Sharp V, steep headcuts. |
| 600-1000 m | 15-55 m | 60-180 m | 6-24 m | Incised V/U hybrid. |
| 250-600 m | 12-45 m | 90-280 m | 12-45 m | Wider, locally terraced. |
| 80-250 m | 5-25 m | 120-360 m | 20-80 m | Broad lowland channel. |
| 0-80 m | 1-12 m | 80-260 m | 15-90 m | Fan distributary/creek mouth. |

Use local slope and contributing area to modulate incision:

```text
incision_depth =
    base_depth_by_elevation
  * flow_accumulation_factor
  * slope_factor
  * lithology_or_noise_factor
  * lahar_multiplier_if_applicable
```

Depth should taper near the coast so channels become creek mouths and fan swales rather than deep canyons through the beach.

### Hydrologic conditioning

Developer validation should use known DEM analysis concepts: flow direction, flow accumulation, stream delineation, watershed basins, and sink/pit handling. TauDEM documents DEM tools for pit removal, flow directions, slopes, contributing area, stream networks, and watershed delineation; GRASS `r.watershed` similarly generates flow accumulation, drainage direction, streams, and basin maps. [TauDEM, Terrain Analysis Using Digital Elevation Models](https://hydrology.usu.edu/taudem/taudem5/) [GRASS GIS, r.watershed](https://grass.osgeo.org/grass-stable/manuals/r.watershed.html)

Implementation does not need to vendor TauDEM or GRASS. It should replicate the relevant validation outputs in-engine or in the terrain build pipeline.

## Lahar Gully And Landslide Scar Geometry

### Lahar corridor count

Use **5-7 lahar-capable trunk gullies**. These should be selected from the largest/steepest catchments, not placed evenly.

Recommended distribution:

- 2 dominant lahar corridors from the steepest upper cone sectors.
- 2-3 standard lahar corridors in high-flow catchments.
- 1-2 older/partly abandoned lahar corridors with broader deposits and less sharp incision.

### Lahar channel geometry

Lahar gullies should be terrain-scale channels:

| Band | Depth | Top width | Bed width | Notes |
|---|---:|---:|---:|---|
| Upper flank, 900-1400 m | 25-90 m | 80-260 m | 8-30 m | Confined steep chute, local headcut. |
| Mid flank, 400-900 m | 35-120 m | 150-460 m | 20-80 m | Main destructive corridor, terraces/levees. |
| Lower flank, 80-400 m | 15-70 m | 220-700 m | 40-160 m | Begins widening and depositing. |
| Coastal/fan, 0-80 m | 2-25 m | 300-1000 m | 40-220 m | Distributary swales, fan toe, beach break. |

Lahar channels should:

- follow pre-existing catchment thalwegs;
- locally over-widen bends;
- undercut or bite ridge shoulders;
- include convex debris levees along parts of the channel;
- transition to fans at gradient breaks;
- not cross major ridges unless a landslide/scar explicitly cut through the divide.

### Landslide scars

Use **2-3 major landslide scar systems**.

Scar types:

| Type | Count | Geometry |
|---|---:|---|
| Summit/upper flank amphitheater | 1 | 300-700 m wide, 80-250 m relief, feeds major lahar chute. |
| Mid-flank slump scar | 1-2 | 250-900 m wide, 40-160 m relief, cuts ridge shoulder and feeds fan. |
| Old degraded scar | optional 1 | Softer rim, partly filled, still visible in slope map. |

Scar generation should subtract material from steep slopes and deposit mass downslope:

```text
scar_cut = negative_scoop(headwall, sidewalls, runout_axis)
debris_lobe = positive_convex_lobe(runout_axis, lower_gradient_zone)
net_volume_balance = approximate, not exact, but visually plausible
```

Scar features:

- arcuate headwall;
- steep concave interior;
- hummocky lower runout;
- linked gully incision;
- blocked/deflected small tributaries;
- fan/deposit lobe below.

Do not create perfectly circular craters unless the terrain feature is explicitly a summit crater. Landslide scars should be open downslope.

## Fan Deposit / Creek Mouth Terrain Geometry

### Fan placement

Fans must occur where:

- a steep confined channel exits to lower-gradient terrain;
- a lahar corridor leaves the mid-flank;
- a creek/gully reaches coastal lowland;
- a valley mouth opens into the beach/shelf ring.

Fan generation should be tied to catchment outlets and slope breaks, not random decorations.

The National Academies source describes alluvial fans as developing where streams or debris flows emerge from steep confined reaches and transport capacity decreases because of widening, gradient reduction, or other factors. This is the exact terrain logic to implement. [National Research Council, Alluvial Fan Flooding](https://www.nationalacademies.org/read/5364/chapter/4)

### Fan dimensions

| Fan type | Apex elevation | Radius/length | Width at toe | Relief/thickness |
|---|---:|---:|---:|---:|
| Small creek-mouth fan | 5-40 m | 150-450 m | 180-600 m | 1-8 m |
| Standard lower-slope fan | 30-150 m | 350-900 m | 500-1400 m | 4-22 m |
| Major lahar/debris fan | 20-250 m | 700-1800 m | 900-2500 m | 8-45 m |

Fan shape:

- convex downslope;
- highest near apex;
- broadens toward coast/lowland;
- shallow distributary swales;
- local levee ridges or abandoned channels;
- toe blends into beach/coastal shelf.

### Fan algorithm

For each terminal channel:

```text
apex = first point where channel_gradient < fan_gradient_threshold
fan_axis = normalized vector from apex to coastal outlet
fan_angle = seeded_range(28°, 70°)
fan_length = function(flow_accumulation, lahar_flag, local_space_to_coast)
fan_height = convex_lobe(apex_to_toe_distance)
fan_channels = 1 trunk + 1-4 shallow distributary swales
```

Fan height contribution:

```text
fan_deposit_height =
    deposit_thickness
  * radial_fan_kernel
  * downslope_convex_profile
  * lateral_noise
  * coast_blend
```

Then carve shallow swales over the deposit:

```text
fan_swale_depth = 0.5-8 m ordinary, 2-18 m lahar
```

### Creek mouths and beach breaks

At each outlet:

- Lower the beach berm locally to meet the drainage.
- Create a shallow mouth notch 20-140 m wide.
- Align the notch with the final thalweg.
- Blend to sea level over 50-180 m.
- Add a low fan toe or delta-like apron only as terrain, not rendered water.
- Ensure beach continues around the outlet but is interrupted by the creek mouth.

Forbidden:

- channels stopping at the landward side of the beach ring;
- beach ring acting as a continuous dam;
- straight trenches cut to the map edge;
- rivers ending in flat inland pools unless explicitly marked as a blocked/closed basin, which should be rare and not primary drainage.

## Traversable Versus Hard-Blocker Terrain Geometry

Slope classes should be emitted as developer masks. These are terrain classes, not UI.

Recommended terrain slope classes:

| Slope angle | Class | Terrain use |
|---:|---|---|
| 0-12° | Easy walkable | Beaches, fans, broad ridge tops, lower valley floors. |
| 12-22° | Normal walkable | Ridge shoulders, lower flanks, basin slopes. |
| 22-32° | Slow/rough walkable | Steeper ridge shoulders, gully approaches. |
| 32-42° | Dangerous/hard traversal | Loose slopes, scar interiors, steep gully walls. |
| 42-50° | Conditional blocker/danger | Cliffy gully walls, lahar cuts, unstable scar rims. |
| >50° | Hard blocker | Cliffs, headwalls, severe ravine walls, crater/scar cuts. |

### Target slope placement

| Terrain feature | Desired slope class |
|---|---|
| Ridge crest crowns | Mostly 8-24°, intermittent 24-32°. |
| Ridge shoulders | Mostly 18-38°, local 42° bites. |
| Ordinary gully beds | 3-18° along channel, steeper headcuts. |
| Ordinary gully walls | 28-48°, local >50°. |
| Lahar channel beds | 4-22°, steep upper chutes. |
| Lahar channel walls | 35-55°, especially mid/upper flank. |
| Landslide headwalls | 45-65°, hard blocker in sections. |
| Fan surfaces | 2-12°, broadly traversable. |
| Beach/shelf terrain | 0-6°, traversable unless ocean mask. |
| Summit/crater/scar rim | Mixed 22-55°, with natural blockers. |

Hard blockers must not create an unbroken circular wall around the whole volcano. They should be local, topographic consequences of scars, headwalls, cliffs, and severe gullies.

### Gameplay-adjacent but terrain-only rule

This spec may emit masks such as:

```text
walkable_easy_mask
walkable_rough_mask
danger_slope_mask
hard_blocker_slope_mask
gully_wall_mask
scar_headwall_mask
```

It must not design player-facing UI, guidance systems, markers, or objective tools.

## Slope And Flow Diagnostic Requirements

This PR should ship developer-only diagnostic outputs that can be toggled in editor, saved as images, or exported as data arrays.

### Required raster/map outputs

| Diagnostic | Purpose |
|---|---|
| `height_m` | Final terrain elevation in meters. |
| `slope_degrees` | Walkability/blocker validation. |
| `aspect` | Confirms radial island structure and slope-facing variety. |
| `flow_direction_d8` | Fast deterministic single-flow debug. |
| `flow_direction_mfd` | Optional multi-flow debug for fans/flat areas. |
| `flow_accumulation_log` | Shows thalwegs and drainage density. |
| `basin_id` | Shows 14 primary catchments and subdivisions. |
| `ridge_id` | Shows 14 primary ridge ownership. |
| `ridge_strength` | Confirms coastal fade and ridge hierarchy. |
| `thalweg_distance` | Distance-to-channel validation. |
| `ordinary_gully_depth` | Shows creek incision contribution. |
| `lahar_corridor_id` | Shows lahar-capable channels. |
| `scar_cut_mask` | Shows landslide scar subtraction. |
| `fan_deposit_mask` | Shows depositional terrain lobes. |
| `coastal_outlet_id` | Shows forced terminal outlets. |
| `sea_level_mask` | Shows beach/shelf/ocean transition. |
| `sink_pit_mask` | Flags internal drainage failures. |
| `inland_dead_end_channel_mask` | Flags channels that terminate before coast. |
| `square_edge_highland_mask` | Flags raised land reaching world boundary. |
| `walkability_class` | Encodes slope classes for validation. |

### Required debug visualizations

Produce debug images with consistent palettes and legends:

1. Height shaded relief.
2. Slope degrees.
3. Flow accumulation log scale.
4. Basin ID map.
5. Ridge ID map.
6. Lahar corridor map.
7. Fan/deposit map.
8. Coast outlet map.
9. Walkability/blocker map.
10. Failure overlay: pits, dead-end channels, raised square-edge terrain.

### Required numeric reports

Export a JSON or text summary:

```json
{
  "peak_height_m": 1412.6,
  "island_land_area_km2": 150.3,
  "primary_ridge_count": 14,
  "primary_basin_count": 14,
  "coastal_outlet_count": 14,
  "lahar_corridor_count": 6,
  "major_scar_count": 3,
  "inland_dead_end_major_channels": 0,
  "internal_sinks_above_20m": 0,
  "ridge_cells_outside_island_mask": 0,
  "raised_cells_near_square_edge_above_0m": 0,
  "percent_land_easy_walkable": 28.4,
  "percent_land_normal_walkable": 24.9,
  "percent_land_rough_walkable": 21.7,
  "percent_land_dangerous": 15.8,
  "percent_land_hard_blocker": 9.2
}
```

Values above are illustrative, not exact required outputs.

## Developer-Only Topographic Validation Targets

### Hard validation targets

These must pass.

| Target | Required value |
|---|---:|
| Peak height | 1350-1450 m. |
| Primary ridge count | 14. |
| Primary basin count | 14. |
| Coastal outlet count | At least 14, one per primary catchment. |
| Major lahar corridors | 5-7. |
| Major landslide/scar systems | 2-3. |
| Major inland dead-end channels | 0. |
| Internal sinks above 20 m elevation | 0 unless explicitly whitelisted. |
| Major thalwegs reaching coast | 100%. |
| Ridge mask outside island mask | 0 cells above epsilon. |
| Raised land at square-world edge | 0 cells above sea level. |
| Beach/shelf ring continuity | Continuous except valid creek-mouth breaks. |
| Major fan-to-coast blend | 100% of lahar corridors and primary outlets. |

### Soft validation targets

These should guide tuning but can be adjusted after visual review.

| Metric | Target |
|---|---|
| Average primary catchment area | 5-10% of island land area, with organic variation. |
| Smallest primary catchment | Not less than 2.5% unless intentionally truncated. |
| Largest primary catchment | Not more than 13% unless intentionally dominant. |
| Land <=22° slope | 40-60%. |
| Land 22-42° slope | 25-45%. |
| Land >42° slope | 5-18%. |
| Average ridge relief mid-flank | 60-180 m above adjacent basin floor. |
| Main gully incision mid-flank | 20-80 m. |
| Lahar gully incision mid-flank | 35-120 m. |
| Beach/shelf elevation | Mostly -2 to +8 m. |
| Fan slopes | Mostly 2-12°. |

### Visual validation checklist

A reviewer should be able to answer "yes" to all of these:

- Does the island immediately read as a volcanic island from shaded relief?
- Are ridge arms visible without looking like roads?
- Are valleys between ridges readable as catchments?
- Do flow accumulation maps align with visible gullies?
- Do lahar corridors follow plausible valleys?
- Do landslide scars remove material and feed deposits?
- Do fans occur at slope breaks and creek mouths?
- Does the beach ring remain low and coherent?
- Do drainage paths break the beach naturally?
- Does any major gully stop inland? It should not.
- Does any ridge continue to the square-world edge? It should not.
- Are hard blockers natural and local rather than arbitrary invisible walls?

## Large PR Implementation Scope

This should be a major implementation PR, not a tiny tuning patch.

### Recommended implementation passes

1. **Config and seed pass**
   - Add deterministic config for world size, peak height, island radius, ridge count, gully/lahar counts, fan counts, and validation thresholds.

2. **Base island/massif pass**
   - Generate organic island mask, sea-level shelf, coastal ring, and central massif.

3. **Primary ridge pass**
   - Generate 14 major ridge centerlines with seeded angular jitter and organic curvature.
   - Assign class: dominant, standard, broken.
   - Add ridge height contributions and coastal fade.

4. **Spur/saddle pass**
   - Add secondary spurs.
   - Add saddles and ridge interruptions.
   - Ensure saddles do not accidentally create invalid flow leaks.

5. **Catchment assignment pass**
   - Assign basin sectors between ridges.
   - Lock one outlet per basin on the coast.
   - Generate basin floor bias toward thalwegs.

6. **Thalweg and tributary pass**
   - Generate main channel lines to forced outlets.
   - Add tributary gullies from headwater points.
   - Use flow potential to avoid arbitrary grooves.

7. **Ordinary incision pass**
   - Carve creek/gully geometry using elevation band, slope, and flow accumulation.

8. **Lahar and scar pass**
   - Select 5-7 lahar trunk corridors.
   - Add 2-3 landslide scar systems.
   - Carve wider/deeper lahar channels.
   - Deposit debris/fan lobes downslope.

9. **Fan/coast termination pass**
   - Build alluvial/debris fans at slope breaks and outlets.
   - Cut beach breaks/creek mouths.
   - Ensure all major drainage reaches sea-level shelf.

10. **Hydrologic conditioning pass**
    - Remove/repair invalid sinks.
    - Recompute flow.
    - Verify no major inland dead ends.

11. **Mask/diagnostic export pass**
    - Emit all required masks, debug images, and numeric validation reports.

12. **Automation/acceptance pass**
    - Add automated tests for ridge count, basin count, outlet count, sea-level edge, sink failures, and major drainage termination.

### Pseudocode outline

```cpp
Terrain BuildVolcanicIslandTerrain(const TerrainConfig& C, uint64 Seed)
{
    Terrain T = GenerateBaseIslandAndMassif(C, Seed);

    RidgeSet ridges = GeneratePrimaryRidges(C, Seed, 14);
    ApplyRidgeHeight(T, ridges, C);

    SpurSet spurs = GenerateSpursAndSaddles(T, ridges, C, Seed);
    ApplySpurAndSaddleHeight(T, spurs, C);

    BasinSet basins = AssignCatchmentsBetweenRidges(T, ridges, C);
    OutletSet outlets = LockCoastalOutlets(T, basins, C, Seed);

    ChannelSet channels = GenerateThalwegsAndTributaries(T, basins, outlets, C, Seed);
    CarveOrdinaryGullies(T, channels, C);

    LaharSet lahars = SelectLaharCorridors(T, channels, basins, C, Seed);
    ScarSet scars = GenerateLandslideScars(T, lahars, ridges, C, Seed);
    CarveLaharsAndScars(T, lahars, scars, C);

    FanSet fans = GenerateFansAtSlopeBreaksAndOutlets(T, channels, lahars, outlets, C);
    ApplyFanDeposits(T, fans, C);
    CutCreekMouthsAndBeachBreaks(T, outlets, fans, C);

    HydrologyDiagnostics H = ComputeHydrologyDiagnostics(T, C);
    RepairInvalidSinksAndDeadEnds(T, H, C);

    TerrainDiagnostics D = EmitTopographicDiagnostics(T, ridges, basins, channels, lahars, scars, fans, outlets, C);
    ValidateOrFail(T, D, C);

    return T;
}
```

## Files And Areas Likely Touched

No repository was inspected for this research document, so exact filenames must be verified in the codebase. The likely implementation areas are:

| Area | Likely change |
|---|---|
| Terrain generator core | Add ridge/catchment/lahar/fan generation passes. |
| Terrain config/data assets | Add deterministic parameters and validation thresholds. |
| Heightmap export/import path | Preserve 16-bit height precision and UE Landscape-compatible resolution. |
| Mask generation | Add ridge, basin, flow, gully, lahar, scar, fan, outlet, slope, and coast masks. |
| Editor debug tooling | Add diagnostic map previews/exports. |
| Automated tests | Add topographic validation tests and JSON report snapshots. |
| Documentation | Add developer notes for terrain-generation tuning and acceptance checks. |

Potential concrete file patterns to search in the repo:

```text
*Terrain*
*Height*
*Landscape*
*WorldGen*
*Island*
*Generator*
*Mask*
*Diagnostic*
```

If the project has a previous terrain prompt/spec directory, place this file alongside the other PR5 Batch 003 research/spec outputs.

## Acceptance Criteria

### Functional acceptance

- The island terrain reaches approximately 1400 m at the massif.
- The island fits inside the 16.256 km square world.
- Outside the island mask is ocean or below sea level.
- The square-world edge contains no above-sea-level ridges or land.
- The beach/shelf ring resolves around the island perimeter.
- Fourteen primary radial ridges are present.
- Fourteen primary catchments are present.
- Each primary catchment has a coastal outlet.
- Major gullies drain to coastal fans, creek mouths, beach breaks, or shelf notches.
- Ridge toes fade into low coastal terrain.
- No rivers/gullies terminate randomly in the middle of the beach ring.
- No major channel ends inland above 20 m elevation.
- Lahar gullies are larger/deeper than ordinary creek gullies.
- Landslide scars connect to runout/deposit terrain.
- Fan deposits occur at slope breaks and outlets.
- Slope masks classify easy, normal, rough, dangerous, and hard-blocking terrain.

### Diagnostic acceptance

- Diagnostic maps can be exported for:
  - height;
  - slope;
  - flow accumulation;
  - flow direction;
  - basin ID;
  - ridge ID;
  - gully depth;
  - lahar corridors;
  - scar cuts;
  - fan deposits;
  - coastal outlets;
  - walkability classes;
  - failure overlay.
- Numeric validation report is generated.
- Automated validation fails if:
  - ridge count is wrong;
  - basin count is wrong;
  - any primary outlet is missing;
  - major drainage dead-ends inland;
  - internal unapproved sinks exist above 20 m;
  - raised land reaches square-world edge;
  - ridge masks survive outside island mask.

### Visual acceptance

From overhead shaded relief:

- The volcano reads as a radial volcanic massif.
- Ridges look like eroded terrain divides, not artificial roads.
- Valleys/gullies align with flow diagnostics.
- The coastline receives drainage coherently.
- Fans and creek mouths interrupt the beach naturally.
- Hard-blocking terrain is tied to real topographic forms.

## Risks And Stop Conditions

### Risks

| Risk | Why it matters | Mitigation |
|---|---|---|
| Perfect-spoke volcano | Looks procedural/artificial. | Angular jitter, curved ridge lines, varied ridge strength, scars, spurs. |
| Noise-only terrain | Looks rough but has no drainage logic. | Generate ridges/basins/channels explicitly, validate with flow accumulation. |
| Beach ring dam | Gullies stop inland or pool behind coast. | Lock outlets first, cut mouth notches, validate terminal drainage. |
| Ridge masks leak to edge | Breaks island/ocean requirement. | Apply island/coast fade and edge highland diagnostic. |
| Too many hard blockers | Makes traversal arbitrary. | Use slope distribution targets and local blockers only. |
| Gullies too shallow | Terrain still reads smooth. | Enforce elevation-band incision targets. |
| Lahar channels too decorative | Not terrain-scale. | Use deeper/wider corridor dimensions and linked fans/scars. |
| Fans look like flat pads | Artificial terrain. | Convex fan profiles, shallow distributary swales, rough toe blend. |
| Hydrologic conditioning erases artistry | Over-smoothing may flatten ridges/gullies. | Repair only invalid sinks/dead ends, preserve masks. |
| Resolution too low for gullies | Channels alias or disappear. | Validate at final import resolution; use 8129 target if 2 m world spacing. |

### Stop conditions

Stop and revise the implementation if any of the following are true:

- Any major catchment has no sea-level outlet.
- Any major gully stops inland without an intentional blocked-basin exception.
- Any ridge remains raised into the square-world edge.
- The coastline is a continuous raised dam.
- The peak cannot reach 1350-1450 m with current height scale.
- Final heightmap precision causes terracing or banding in gullies.
- Slope blockers form an unbroken ring around the massif.
- Diagnostic maps cannot be generated.
- Automated validation cannot distinguish success from failure.

## Rejected Or Forbidden Approaches

Do not use:

- Pure noise displacement as the main terrain system.
- Perfectly even radial spokes.
- Straight-line ridges from peak to coast.
- Flat ridge-top roads disguised as ridges.
- Uniform-width trenches for gullies.
- Decorative river masks without carved terrain.
- Rivers/gullies that terminate on the inland side of the beach.
- A continuous beach ring that blocks drainage.
- Ridges that continue to the square-world edge.
- Island falloff applied only at the final step if it destroys drainage outlets.
- Hard blockers that are invisible or unrelated to slope/topography.
- Player-facing map, minimap, compass, GPS, bearing, objective, or orientation UI.
- Foliage/ecology/material/VFX/audio/survival/NPC/story design inside this PR.

## Sources And References

- Britannica. **Radial drainage pattern.** Radial drainage is typical of volcanic cones where streams radiate from a central high.  
  https://www.britannica.com/science/radial-drainage-pattern

- U.S. Geological Survey. **Lahars move rapidly down valleys like rivers of concrete.** Lahars move downslope through valleys/channels, entrain material, and deposit as they decelerate in lower-gradient areas.  
  https://www.usgs.gov/programs/VHP/lahars-move-rapidly-down-valleys-rivers-concrete

- National Research Council. **Alluvial Fan Flooding — Flooding Processes and Environments on Alluvial Fans.** Fans form where streams or debris flows leave steep confined reaches and lose transport capacity due to widening/reduced gradient.  
  https://www.nationalacademies.org/read/5364/chapter/4

- TauDEM Hydrology Research Group. **Terrain Analysis Using Digital Elevation Models.** DEM-based hydrologic analysis includes pit removal, flow direction, slope, contributing area, stream networks, and watershed delineation.  
  https://hydrology.usu.edu/taudem/taudem5/

- GRASS GIS. **r.watershed manual.** `r.watershed` generates flow accumulation, drainage direction, stream, watershed basin, and slope-length outputs from elevation data.  
  https://grass.osgeo.org/grass-stable/manuals/r.watershed.html

- Epic Games. **Landscape Technical Guide in Unreal Engine.** Documents recommended Landscape sizes and heightmap/Z-scale handling for Unreal Landscape import.  
  https://dev.epicgames.com/documentation/unreal-engine/landscape-technical-guide-in-unreal-engine
