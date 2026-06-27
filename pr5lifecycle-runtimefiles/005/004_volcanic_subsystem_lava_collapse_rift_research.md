# Volcanic Subsystem, Lava, Collapse, And Rift Math Research

## Executive Summary

The volcano should be rebuilt as a **compact, local geological subsystem** embedded inside the larger Jungle Game island terrain graph. The current failure mode is not “the volcano is too dramatic”; it is that crater-centered math has become a global coordinate authority. The replacement design should preserve the volcano as a dominant landmark while making the rest of the 97.536 km island structurally interesting even when all volcanic contributions are disabled.

The recommended approach is:

1. Generate or preserve the **non-volcanic island graph first**: coastline, regional ridges, catchments, basins, gullies, benches, old uplift surfaces, and coastal constraints.
2. Select a **volcano anchor** from that graph, not from world center or island centroid.
3. Build an active volcanic construct with a strict compact footprint:
   - active cone target: **4–7.5 km diameter**, hard max **9 km diameter** or **2.5% of land area**, whichever is smaller;
   - broader volcanic highland target: **12–18 km diameter**, hard max **22 km diameter** or **10% of land area**, whichever is smaller;
   - old volcanic terrain/bench fields: distributed and degraded, hard max **20% of land area** and never allowed to override regional hydrology.
4. Use local polar/radial math only inside explicit volcano masks: cone profile, rim profile, vent bowl, local ring roughness, and secondary cone microforms.
5. Route crater breach, collapse scar, lava paths, and lahar corridors from **terrain context**: downslope vectors, drainage accumulation, valley confinement, rift-line structure, catchments, saddles, and existing gullies.
6. Compose volcanic terrain into the base island, then run a coast clamp and revalidate all terrain acceptance gates.

A good volcano-disabled preview should still show coherent ridges, catchments, old benches, valleys, and traversable/non-traversable terrain. The volcano-on preview should add a recognizable volcanic subsystem without turning the island into a radial heightfield.

## Terrain-Only Scope Boundary

This document only covers **heightfield geometry, terrain masks, hydrology/catchment coupling, and terrain-preview validation**.

In scope:

- volcanic construct masks;
- cone, crater, vent, rim, breach, scar, fissure, lava-flow, lahar, ash/scree, crust, and hard-blocker terrain masks;
- heightfield deformation;
- erosion/degradation rules;
- preview/debug maps;
- deterministic metrics.

Out of scope:

- smoke, fire, lava glow, particles, VFX, decals, audio;
- player damage, heat, oxygen, gas, AI, combat, quests, objective design;
- buildings, props, final materials, UI, map, compass, GPS, or player-facing hazard overlays;
- biome art pass, foliage placement, survival mechanics, or navigation markers.

“Lava,” “crust,” “unstable,” “hazard,” and “blocker” in this document mean **terrain-derived masks and heightfield geometry only**.

## Current Volcano Failure

The present terrain read is wrong because the volcano behaves like a global generator primitive:

- the cone appears to own most of the island;
- ridges/gullies read as radial spokes from a crater instead of catchment-owned drainage;
- the active construct is too large relative to the island;
- lava and gully directions appear distributed by angular sectors rather than topography;
- the island risks becoming visually empty when the volcano is disabled;
- global radial falloff leaks into coastline, old highlands, and non-volcanic terrain;
- the coastline acceptance gates are at risk whenever the volcano deformation reaches the outer world edge.

The core fix is architectural: **the island graph must own the volcano, not the volcano owning the island graph**.

## Volcanic Subsystem Goals

The subsystem should create a believable tropical volcanic island interior with:

- a compact active cone and summit crater;
- nested vent/crater floor geometry;
- broken rim and terrain-selected breached sector;
- flank collapse amphitheater or horseshoe scar;
- one or more rift/fissure alignments with secondary cones;
- old lava benches and eroded flow fields;
- lava channels that follow actual gullies;
- lahar corridors coupled to drainage/catchment data;
- unstable crust and hard-blocker masks derived from terrain shape;
- local radial detail limited to active volcanic forms.

The design target is **morphological plausibility**, not full geophysical simulation. Use real volcanic geomorphology as constraints, then implement deterministic approximations suitable for UE5 terrain preview generation.

## Volcano Footprint And Relief Targets

### World and land scale

The terrain preview world size is fixed:

```text
world_size_m = 97536.0
world_square_area_km2 = 9513.27
```

All volcanic footprint gates should be measured against **land area**, not the square world area, because the outer world edge remains ocean/container terrain.

### Recommended footprint classes

| Zone | Role | Target equivalent diameter | Hard maximum | Height/relief intent |
|---|---:|---:|---:|---|
| `ActiveCone` | Current cone, crater, fresh flows | 4.0–7.5 km | 9.0 km or 2.5% of land area | main landmark relief |
| `SummitCrater` | Crater/rim/vent floor | 0.7–1.8 km | 2.2 km | depression and asymmetric rim |
| `CollapseScar` | Horseshoe cut/amphitheater | 1.5–4.5 km wide | 5.5 km | cut into one flank, not whole cone |
| `RiftLine` | Fissures, vents, secondary cones | 4–14 km polyline | 18 km | structural alignment, low/moderate relief |
| `BroaderHighland` | volcanic highland base | 12–18 km | 22 km or 10% land area | supports cone, does not dominate island |
| `OldVolcanicTerrain` | benches, degraded flows, old aprons | 15–35 km distributed fields | 20% land area | low relief, eroded, river-cut |

### Relief targets

Use relief relative to local regional terrain, not sea level alone:

```text
active_cone_relief_target_m: 650–1450
active_cone_relief_hard_max_m: 1800 unless art direction explicitly approves
broader_highland_relief_target_m: 180–650
old_lava_bench_relief_target_m: 25–220
secondary_cone_height_target_m: 35–180
collapse_scar_cut_depth_target_m: 120–650
```

For a tropical island at this scale, a single high volcanic landmark can be large, but the active cone should not be wide enough to dictate the whole drainage map. Broad shield-like slopes are acceptable only as a **highland envelope**, while the active cone should be compact.

### Gate formulas

Prefer equivalent-area radius so irregular/elliptical masks can be validated:

```cpp
EquivalentRadiusKm = sqrt(MaskAreaKm2 / PI);
EquivalentDiameterKm = 2.0 * EquivalentRadiusKm;
LandAreaFraction = MaskAreaKm2 / LandAreaKm2;
```

Recommended hard gates:

```text
active_cone_equivalent_diameter_km <= 9.0
active_cone_land_fraction <= 0.025

broader_highland_equivalent_diameter_km <= 22.0
broader_highland_land_fraction <= 0.10

old_volcanic_terrain_land_fraction <= 0.20
old_volcanic_relief_p95_m <= 220
old_volcanic_slope_p95_deg <= 18 unless inside eroded scar/channel walls

volcanic_total_high_relief_land_fraction <= 0.12
```

`old_volcanic_terrain_land_fraction` can be larger than active cone fraction because old benches are degraded, low-amplitude terrain, not a global peak generator.

## Active Cone And Crater Geometry

### Geological basis

A volcanic construct can mix shield-like lower slopes, composite/stratovolcano-like upper steepening, cinder/secondary cones, lava-flow aprons, and lahar/pyroclastic deposits. BGS distinguishes steep stratovolcano slopes from broad shield volcano slopes and gives shield slopes as gentle, often less than 10°, while viscous stratovolcano material can produce much steeper 30–35° slopes near repose limits. NPS describes composite volcanoes as conical, concave, steeper near the summit, and built from lava flows, pyroclastic deposits, lahars, and domes. [BGS volcano types][src-bgs-volcano-types] [NPS composite volcanoes][src-nps-composite]

### Height profile

Use a bounded, concave volcanic construct profile with lower-slope broadening and upper-slope steepening.

Do not use a single global radial cone. Use a local anisotropic coordinate window:

```cpp
// Local volcano-space coordinate.
FVector2d P = WorldXY - SummitXY;
FVector2d Q = Rotate(P, -ConstructAzimuthRad);

// Elliptical distance, allowed only inside BroaderHighland/ActiveCone masks.
double q = sqrt(square(Q.X / RadiusA_m) + square(Q.Y / RadiusB_m));
```

Use a profile that has a smooth base, shield-like lower slope, and steeper active cone interior:

```text
base_profile(q)  = smoothstep(1.0, 0.0, q)
upper_weight(q)  = smoothstep(0.72, 0.18, q)
shield_profile   = pow(base_profile, 1.45)
strato_profile   = pow(base_profile, 0.82) * upper_weight
construct_height = HighlandRelief * shield_profile + ConeRelief * strato_profile
```

Then clamp by local slope:

```text
lower_flank_slope_target: 4–10°
mid_flank_slope_target: 9–18°
upper_cone_slope_target: 18–32°
hard_slope_cap_construct: 34° except scar/cliff masks
```

Because grid spacing and UE scale can exaggerate slopes, slope constraints should be applied in meters before final engine scaling.

### Terrain attachment

The cone is added to an already-existing regional terrain:

```cpp
FinalPreClampHeight =
    BaseIslandHeight
  + HighlandMask * HighlandDelta
  + ActiveConeMask * ConeDelta
  + CraterDelta
  + CollapseScarDelta
  + LavaDepositDelta
  + OldBenchDelta;
```

The volcano should attach by matching local saddles and ridge trends:

```text
attachment_blend_width_m: 2500–6500
highland_edge_delta_limit_m: 0 at mask edge
base_slope_continuity_target: |slope_before - slope_after| <= 6° at highland boundary
catchment_preservation_target: downstream trunk rivers keep identity after composition
```

Avoid circular bathtub seams. Blend along irregular highland masks shaped by ridgelines and drainage divides.

## Nested Vent, Broken Rim, And Breach Model

### Crater geometry

The summit crater should be a nested, asymmetric depression, not a perfect circular bowl.

Recommended parameters:

```text
crater_radius_m: 350–900 target, 1100 hard max
crater_depth_m: 70–260 target
rim_height_m: 35–180 target
crater_floor_radius_fraction: 0.32–0.58
nested_vent_radius_m: 60–260
nested_vent_depth_m: 20–120
rim_roughness_m: 8–55
```

Crater depression:

```cpp
double craterQ = EllipseDistance(WorldXY, CraterCenter, CraterAxes, CraterAzimuth);
double bowl = -CraterDepth * smoothstep(1.0, 0.0, craterQ);
double flatFloor = lerp(bowl, -CraterDepth, smoothstep(0.55, 0.20, craterQ));
```

Rim annulus:

```cpp
double rimBand = exp(-square((craterQ - 1.0) / RimSigma));
double rimAsym = 1.0 + 0.18 * Noise2D(WorldXY * RimNoiseFreq)
                     + 0.22 * Dot(Normalized(WorldXY - CraterCenter), RimHighDirection);
double rimHeight = RimBaseHeight * rimBand * rimAsym;
```

### Broken rim and breach

The crater breach should not be selected by a fixed compass angle or equal-angle radial sector. It should be selected from terrain context.

Candidate breach directions should score:

```text
score = 
  + 0.32 * downhill_alignment_from_crater_floor
  + 0.24 * nearest_major_drainage_alignment
  + 0.18 * lowest_rim_saddle
  + 0.12 * collapse_scar_alignment
  + 0.08 * rift_line_alignment
  + 0.06 * deterministic_noise
```

Where:

- `downhill_alignment_from_crater_floor`: dot product between candidate direction and local steepest descent on post-cone terrain;
- `nearest_major_drainage_alignment`: alignment to a trunk gully/catchment outlet;
- `lowest_rim_saddle`: local minimum along rim annulus;
- `collapse_scar_alignment`: positive if breach drains into collapse amphitheater;
- `rift_line_alignment`: positive if fissure system plausibly intersects crater flank;
- deterministic noise breaks ties only.

Breach cut:

```cpp
double angleDelta = SignedAngle(LocalDirection, BreachDirection);
double breachAngularMask = exp(-square(angleDelta / BreachHalfWidthRad));
double breachRadialMask = smoothstep(0.85, 1.20, craterQ) * smoothstep(1.85, 1.05, craterQ);
double breachCut = -BreachDepth * breachAngularMask * breachRadialMask;
```

This formula is allowed only inside the crater/rim window. It must not create radial gullies down the island.

## Flank Collapse / Horseshoe Scar Model

### Geological basis

Volcanic edifice collapse can produce horseshoe-shaped amphitheaters, spoon-shaped scarps, and sector scars several kilometers wide. MTU’s volcanic edifice collapse summary notes avalanche scars ranging from shallow spoon-shaped scarps to large horseshoe-shaped amphitheaters and distinguishes widening/subparallel amphitheater walls from erosionally breached calderas. A review of lateral collapse processes describes sector scars with wide opening angles and non-parallel walls that can include the summit. [MTU edifice collapse][src-mtu-collapse] [Romero et al. 2021][src-romero-collapse]

### Direction choice

Collapse direction should be terrain-selected:

```text
collapse_axis =
  normalize(
    0.36 * regional_downslope_vector
  + 0.22 * major_catchment_outlet_vector
  + 0.16 * rift_or_fissure_weakness_vector
  + 0.12 * steepest_flank_instability_vector
  + 0.08 * hydrothermal_alteration_proxy_vector
  + 0.06 * deterministic_seed_vector
  )
```

Useful proxies:

- `regional_downslope_vector`: direction from summit toward lower terrain over 4–10 km;
- `major_catchment_outlet_vector`: direction of the most connected downstream drainage;
- `rift_or_fissure_weakness_vector`: existing structural line;
- `steepest_flank_instability_vector`: sector with steep slopes, high convexity, high wetness;
- `hydrothermal_alteration_proxy_vector`: crater-breach/vent proximity plus high slope/concavity, purely geometric for now.

### Scar geometry

Represent the collapse scar in local axis coordinates:

```cpp
FVector2d D = Rotate(WorldXY - SummitXY, -CollapseAzimuth);
double along = D.X;        // positive downslope along scar
double across = abs(D.Y);

double headwall = smoothstep(-HeadwallBack_m, HeadwallFront_m, along);
double openWidth = lerp(HeadwallWidth_m, MouthWidth_m, saturate(along / ScarLength_m));
double sideMask = smoothstep(openWidth, openWidth * 0.72, across);
double lengthMask = smoothstep(-HeadwallBack_m, 0.0, along) * smoothstep(ScarLength_m, ScarLength_m * 0.78, along);

double scarMask = headwall * sideMask * lengthMask;
```

Cut depth should be strongest near the headwall and fade downslope:

```cpp
double headDepth = CollapseCutDepth_m * exp(-square((along - HeadwallFocus_m) / HeadwallSigma_m));
double floorCut = CollapseFloorCut_m * smoothstep(0.0, ScarLength_m * 0.65, along);
double scarCut = -(headDepth + floorCut) * scarMask;
```

Add steep scar walls:

```cpp
double wallBand = exp(-square((across - openWidth * 0.78) / WallSigma_m));
double wallRoughness = Noise2D(WorldXY * 0.004) * WallRoughness_m;
```

### Debris apron

A collapse should leave a downslope debris-avalanche/debris apron, but this should be lower-relief and terrain-constrained:

```text
debris_apron_length_m: 2500–9000
debris_apron_width_m: 1200–4500
debris_hummock_height_m: 8–70
debris_height_p95_m: <= 110
debris_follows_valley_score: required
```

The apron should spread down the selected catchment, not spray as an equal-angle fan.

## Rift Zones, Fissures, And Secondary Cones

### Geological basis

A fissure is an elongate fracture/crack at the surface from which lava erupts, and cinder cones are steep conical hills formed by accumulation of lava fragments around a vent. USGS definitions also describe the edifice as the volcano body built by lava, tephra, pyroclastic flows, lahars, and related deposits, with shield lava flows sometimes extending beyond the edifice. [USGS glossary][src-usgs-glossary]

### Rift-line placement

Rift lines should be structural polylines, not radial spokes. Place them using a blend of regional terrain fabric and volcano-local stress:

```text
rift_axis_score =
  + 0.30 * alignment_with_regional_ridge_or_fault_proxy
  + 0.24 * alignment_with_long_highland_axis
  + 0.18 * saddle_chain_continuity
  + 0.12 * secondary_catchment_divide_alignment
  + 0.10 * low_curvature_path_cost
  + 0.06 * deterministic_noise
```

Construct `FRiftLine` as a polyline with variable width:

```cpp
struct FRiftLine
{
    FGuid Id;
    TArray<FVector2d> ControlPoints;
    double HalfWidthMin_m;
    double HalfWidthMax_m;
    double StructuralWeight;
    double VentProbability;
    double MaxLength_m;
    double BranchProbability;
    uint64 Seed;
};
```

Recommended parameters:

```text
rift_length_target_m: 4000–14000
rift_length_hard_max_m: 18000
rift_half_width_m: 80–420
rift_branch_count: 0–2
rift_not_required_to_cross_summit: true
```

### Secondary cones and parasitic vents

Secondary cones should occur along the rift where terrain allows eruption points:

```text
secondary_cone_count_target: 2–7
secondary_cone_spacing_m: irregular 800–2800
secondary_cone_radius_m: 250–850
secondary_cone_height_m: 35–180
secondary_crater_radius_fraction: 0.15–0.35
secondary_cone_slope_target_deg: 18–32
```

Vent placement score:

```text
score =
  + rift_proximity
  + local_saddle_score
  + highland_mask
  + slope_break_score
  - major_river_intersection_penalty
  - coastline_proximity_penalty
  - active_crater_overlap_penalty
```

The spacing must be jittered by terrain score. Do not place cones at equal intervals.

## Lava Flow Routing And Lobe Deposition

### Geological basis

Lava emplacement is strongly topography-controlled. Probabilistic lava models often treat lava as a gravitational current following steepest descent with random deviations, and MrLavaLoba-style methods model emplacement using sequential lobes while accounting for volume and topographic modification. The Fagradalsfjall lava hazard study documents MrLavaLoba use for valley infill and spillover assessment, and Flowy summarizes the probabilistic steepest-descent/lobe approach. [Fagradalsfjall MrLavaLoba study][src-fagradalsfjall] [Flowy/MrLavaLoba][src-flowy]

### Algorithm requirement

Lava channels must be routed over terrain. They must not be radial rays.

Recommended deterministic terrain-derived algorithm:

1. Build `PostConstructDEM`:
   - base island terrain;
   - highland/cone/crater;
   - crater breach;
   - collapse scar;
   - rift cones;
   - no final lava deposits yet.
2. Compute hydrology/flow fields:
   - D∞ or multiple-flow direction for downslope routing;
   - D8 only as fallback because it introduces 45° grid bias;
   - flow accumulation;
   - valley confinement;
   - local slope;
   - HAND-like height above drainage if available.
3. Seed flows from crater breach, flank vents, or secondary cones.
4. Step centerline downhill with probabilistic but deterministic weighting.
5. Deposit lobes iteratively, updating lava thickness and optionally local height.
6. Derive levees, crust, stopped-flow masks, and channel masks from accumulated path/lobe state.

### Path step score

For each neighboring candidate or facet direction:

```text
candidate_score =
  + 0.42 * downslope_descent_score
  + 0.20 * valley_confinement_score
  + 0.14 * flow_accumulation_score
  + 0.10 * inertia_alignment_score
  + 0.08 * existing_lava_channel_score
  + 0.04 * rift_alignment_score
  + 0.02 * deterministic_noise
  - 0.30 * uphill_penalty
  - 0.25 * ocean_penalty_unless_littoral_flow_allowed
  - 0.20 * protected_coast_clamp_penalty
```

Small uphill moves may be allowed only when lava thickness is sufficient to pond and spill:

```text
allow_uphill_if: uphill_delta_m <= min(0.35 * local_lava_thickness_m, 12.0)
```

### Lobe deposition

Use oriented elliptical lobes inspired by MrLavaLoba-style terrain deposition, but simplified for game terrain:

```cpp
struct FLavaLobe
{
    int32 FlowId;
    FVector2d CenterXY;
    FVector2d Direction;
    double MajorAxis_m;
    double MinorAxis_m;
    double Thickness_m;
    double Age01;          // 0 fresh, 1 old/degraded
    double Roughness_m;
    double LeveeWeight;
    double CrustWeight;
};
```

Lobe shape:

```text
major_axis_m: 80–450 active, 200–900 old/degraded
minor_axis_m: 20–160 active, 80–350 old/degraded
thickness_m: 1–45 active, 0.5–18 old
orientation: local path direction blended with steepest descent
overlap: allowed; repeated overlap thickens channel/levees
```

Deposit height:

```cpp
double lobeQ = EllipseDistance(WorldXY, Lobe.CenterXY, LobeAxes, LobeAzimuth);
double lobeCore = smoothstep(1.0, 0.0, lobeQ);
double deposit = Lobe.Thickness_m * pow(lobeCore, 1.7);
```

Levees:

```cpp
double sideDistance = abs(ProjectAcross(WorldXY - Center, Direction));
double leveeBand = exp(-square((sideDistance - MinorAxis_m * 0.82) / LeveeSigma_m));
double leveeHeight = Lobe.LeveeWeight * Lobe.Thickness_m * 0.25 * leveeBand;
```

### Stop conditions

A lava path stops when:

```text
remaining_volume <= 0
cooling_length_reached
slope < min_slope and ponding capacity full
entered ocean/coast clamp zone
blocked by pre-existing ridge or lobe thickness insufficient to spill
path exceeds max_length_m
```

Recommended lava path lengths:

```text
fresh_active_flow_length_m: 1200–6500
old_flow_length_m: 3500–15000
hard_max_flow_length_m: 18000 unless old low-relief bench field
```

Long old flows are acceptable if degraded and catchment-following. Fresh flows should be compact.

## Lahar Corridor Coupling To Drainage

### Geological basis

USGS describes lahars as volcanic mudflows/debris flows that originate on volcanic slopes and move rapidly down valleys; they can form from eruptions, rainfall on loose volcanic sediment, landslides, crater-lake breakout, and other processes. USGS LAHARZ generates lahar/debris-flow hazard zones using topography and estimates downstream inundation zones. [USGS lahars][src-usgs-lahars] [USGS LAHARZ][src-usgs-laharz]

### Terrain-only lahar intent

For Jungle Game, lahar corridors are not gameplay hazard overlays. They are:

- valley-floor fill masks;
- loose/debris terrain masks;
- aggraded channel floors;
- unstable banks;
- hard-blocker boulder/debris choke masks;
- erosion/exposure controls for old volcanic deposits.

### Source masks

Lahar source cells should come from terrain/volcanic substrate:

```text
source_score =
  + loose_tephra_mask
  + scree_apron_mask
  + collapse_debris_mask
  + fresh_lava_fragmented_edge_mask
  + steep_ash_slope_mask
  + crater_breach_debris_mask
  - dense_old_bedrock_mask
```

### Catchment coupling

1. Compute volcano source catchments over the post-construct DEM.
2. Intersect source masks with drainage initiation cells.
3. Trace downstream through the drainage network.
4. Widen corridor according to volume class, valley width, confinement, and HAND.
5. Stop at coastal clamp/ocean or low-gradient floodplain sink.

```text
corridor_width_m =
  base_width
  * volume_class_scale
  * lerp(0.65, 1.45, valley_floor_width_norm)
  * lerp(1.25, 0.75, confinement_norm)
```

Recommended width classes:

```text
minor_lahar_corridor_width_m: 20–70
moderate_lahar_corridor_width_m: 60–180
large_lahar_corridor_width_m: 160–480
```

Lahar masks should follow blue-line drainage in debug output. A lahar corridor that cuts across divides is a failure unless explicitly caused by a modeled dam/spillover.

## Old Lava Benches And Eroded Volcanic Terrain

Old volcanic terrain should feel older, lower, wetter, and more integrated with the island graph than the active cone.

### Old bench behavior

Old benches are:

- broad, gently dipping lava shelves;
- eroded by trunk streams;
- interrupted by non-volcanic ridges;
- partly soil-covered;
- sometimes stranded as terraces;
- not active vents, not glowing flows, not radial gutters.

Recommended old bench parameters:

```text
old_bench_height_m: 15–140
old_bench_edge_relief_m: 5–55
old_bench_slope_deg: 1–9 typical, 15 hard p95
old_flow_roughness_m: 1–18
old_flow_age01: 0.65–1.0
erosion_cut_strength: high along rivers/catchments
```

### Placement

Place old benches where previous lava plausibly spread:

```text
placement_score =
  + downstream_from_volcanic_highland
  + gentle_to_moderate_slope
  + existing_low_ridge_or_terrace
  + old_lava_path_density
  + distance_from_active_crater
  - active_cone_mask
  - coastline_clamp_zone
  - major_current_river_core
```

Old bench fields may occupy larger areas, but their height contribution should be bounded and eroded. They should improve non-volcano terrain, not erase it.

### Degradation

Apply degradation to old flows:

```text
eroded_height = bench_height
              * (1.0 - 0.45 * drainage_cut_mask)
              * (1.0 - 0.25 * slope_relaxation_mask)
              * (1.0 - 0.18 * weathering_noise)
```

Cut old benches with real drainage:

```text
bench_cut = -river_incise_depth * flow_accumulation_norm * old_bench_mask
```

## Scree, Ash, Crust, And Hard-Blocker Terrain Masks

### Scree and ash aprons

Scree/ash should be slope-derived and deposit-derived:

```text
scree_mask =
  volcanic_material_mask
  * slope_band(22°, 38°)
  * convexity_or_cliff_source
  * downslope_transport_kernel
```

```text
ash_apron_mask =
  active_cone_mask
  * loose_tephra_probability
  * wind_or_asymmetry_bias_optional
  * slope_relaxation
```

Do not use ash fall to paint the whole island. Keep ash/tephra strongest near the construct and downwind/downvalley only if a wind/terrain field exists.

### Crust mask

`Crust` is a terrain-surface class derived from lava thickness, age proxy, lobe overlap, cooling/stopped-flow state, and slope. It is not VFX.

```text
crust_mask =
  lava_deposit_mask
  * smoothstep(min_thickness, crust_thickness_target, lava_thickness)
  * smoothstep(min_age, crust_age_target, age_proxy)
```

Subclasses:

```text
fresh_crust_mask: young, high roughness, high channel identity
old_crust_mask: weathered, lower roughness, more soil/erosion
thin_crust_mask: lobe margin, low thickness, high crack density
ponded_crust_mask: depression-filled lava, flat core, raised edge
```

### Hard blockers

Hard blockers must be terrain-derived:

```text
hard_blocker_mask =
    cliff_slope_mask
  | collapse_headwall_mask
  | crater_inner_wall_mask
  | fresh_lava_levee_wall_mask
  | fissure_crack_wall_mask
  | boulder_choke_mask
  | oversteep_scree_chute_mask;
```

Recommended thresholds:

```text
walkability_slope_reference_deg: project-specific
blocker_slope_threshold_deg: walkability + 8° to 15°
collapse_headwall_blocker_min_height_m: 12
fissure_blocker_min_depth_m: 3
levee_wall_blocker_min_height_m: 2.5 where narrow/continuous
boulder_choke_relief_m: 1.5–8
```

Use morphology metrics:

```text
slope
curvature
local_relief
roughness
height_discontinuity
lava_thickness_gradient
scar_wall_band
crack_depth
```

No blocker should be manually painted because “lava is dangerous.” If the terrain would not physically block or destabilize traversal, it should not be in the hard-blocker mask.

## Local Radial Math Limits

### Allowed radial/polar uses

Local polar/radial math is acceptable only inside bounded masks:

| Use | Allowed window | Notes |
|---|---:|---|
| Active cone slope profile | `ActiveConeMask` | Must fade to zero by mask boundary |
| Summit crater/rim | `CraterMask` | Elliptical/asymmetric, not perfect ring |
| Nested vent | `VentMask` | Small local bowl |
| Secondary cone microform | `SecondaryConeMask` | Each cone has its own local frame |
| Rim roughness | `RimBandMask` | Noise breaks symmetry |
| Crater breach cut | `BreachLocalMask` | Direction terrain-selected |
| Local lava pond/lobe ellipse | `FLavaLobe` extent | Oriented by path/slope |

### Forbidden radial uses

Never use crater-centered radial math for:

- island-wide ridge generation;
- island-wide gully placement;
- coastline influence;
- regional highland placement;
- catchment boundaries;
- lava path direction;
- lahar direction;
- collapse scar direction;
- rift/fissure sector placement;
- old lava bench boundaries;
- whole-island noise domain;
- beach/ocean masks;
- square world edge behavior.

### Radial leakage validation

Add an angular autocorrelation/sector artifact check outside the local masks:

```text
radial_spoke_score =
  angular_power_about_crater_center(
    terrain_gradient_or_gully_mask,
    outside_radius = active_cone_radius * 1.35
  )
```

Acceptance:

```text
radial_spoke_score_outside_active <= baseline_nonvolcanic_score * 1.15
no visible equal-angle spokes in debug preview
no crater-centered gully rays outside BroaderHighlandMask
```

## Integration With Island Graph And Hydrology

### Correct generation order

Recommended order:

```text
1. Generate base island land/ocean/coast masks.
2. Generate non-volcanic island graph:
   - regional ridge network;
   - catchments;
   - drainage;
   - beaches/coastal flats;
   - old non-volcanic uplands;
   - terrain interest with volcano disabled.
3. Validate base island gates.
4. Select volcano anchor from island graph.
5. Build broader volcanic highland mask from terrain-compatible ridges/divides.
6. Build active cone/crater/collapse/rift components locally.
7. Recompute local hydrology in volcano influence bounds.
8. Route lava and lahar masks from local hydrology/catchments.
9. Add old lava benches and degraded deposits with erosion cuts.
10. Compose final heightfield.
11. Coast-clamp volcanic contributions after composition.
12. Revalidate all world/coast/ocean gates.
13. Export preview/debug maps and metrics.
```

### Anchor selection

Do not place the summit at world center by default. Candidate anchors should be scored:

```text
anchor_score =
  + 0.22 * distance_from_coast_target_band
  + 0.20 * regional_highland_support
  + 0.17 * drainage_divide_or_saddle_context
  + 0.14 * visibility_landmark_score
  + 0.11 * room_for_highland_mask
  + 0.08 * non_overlap_with_primary_nonvolcanic_feature
  + 0.05 * rift_alignment_potential
  + 0.03 * deterministic_noise
```

Constraints:

```text
summit_distance_to_coast_m >= 8500 target, 6000 hard min
summit_distance_to_square_edge_m >= 16000
active_cone_mask_ocean_overlap_pct == 0 after coast clamp
broader_highland_ocean_overlap_pct <= small coastal-clamped tolerance
```

The volcano can be offset from the island centroid by **12–28% of island land bounding radius**, giving a more natural island composition and leaving room for independent terrain regions.

### Hydrology ownership

Hydrology should own lava/lahar pathing:

```text
drainage_network = ComputeDrainage(PostConstructDEM)
lava_routes = RouteLava(Vents, PostConstructDEM, drainage_network)
lahar_corridors = RouteLahars(SourceMasks, drainage_network)
```

Do not derive hydrology from radial sectors.

## Preview / Debug Outputs

Required preview/debug outputs:

1. `volcano_footprint_mask.png`
   - active cone, broader highland, old volcanic fields, no hidden global influence.
2. `volcano_disabled_height_preview.png`
   - base island without volcanic contributions.
3. `volcano_enabled_height_preview.png`
   - final composed heightfield.
4. `volcano_delta_height.png`
   - volcanic height contribution only.
5. `crater_rim_breach_map.png`
   - rim annulus, nested vent, breach cut, rim high/low sectors.
6. `collapse_scar_map.png`
   - headwall, scar floor, sidewalls, debris apron.
7. `rift_line_secondary_cones.png`
   - rift polylines, fissure widths, vent/cone points.
8. `lava_path_graph.png`
   - centerline graph, lobe footprints, levees, stopped-flow masks.
9. `lahar_catchment_coupling_map.png`
   - source masks, drainage traces, widened lahar corridors.
10. `old_lava_bench_mask.png`
    - old flow shelves, erosion cuts, degraded flow fields.
11. `terrain_blocker_masks.png`
    - slope blockers, scar walls, levees, fissures, boulder chokes.
12. `radial_leakage_diagnostic.png`
    - angular artifact check outside active mask.
13. `coast_gate_overlay.png`
    - final coastline/ocean/edge verification after volcanic clamp.

Each preview should be deterministic and include seed, world scale, min/max/percentile metrics, and whether the volcano is enabled.

## Validation Metrics

### Existing terrain acceptance gates

These must remain unchanged:

```text
world_size_m: 97536.0 target
shoreline_error_max_m: 0 or effectively 0
beach_continuity_pct: 100%
ocean_below_sea_pct: 100%
square_edge_ocean_violations: 0
outer world edge remains ocean/container terrain
volcanic terrain contributions must be coast-clamped after composition
```

### New volcanic subsystem metrics

```cpp
struct FVolcanicMetrics
{
    double LandAreaKm2;

    double ActiveConeAreaKm2;
    double ActiveConeEquivalentDiameterKm;
    double ActiveConeLandFraction;

    double HighlandAreaKm2;
    double HighlandEquivalentDiameterKm;
    double HighlandLandFraction;

    double OldVolcanicAreaKm2;
    double OldVolcanicLandFraction;

    double MaxVolcanicDeltaHeightM;
    double P95VolcanicDeltaHeightM;
    double ActiveConeSlopeP95Deg;
    double OldBenchSlopeP95Deg;

    double SummitDistanceToCoastM;
    double SummitDistanceToWorldEdgeM;

    double LavaPathHydrologyAlignment01;
    double LaharDrainageAlignment01;
    double CollapseAxisTerrainScore01;

    double RadialLeakageScoreOutsideActive;
    double VolcanoDisabledReliefP90P10M;
    int32 VolcanoDisabledMajorCatchmentCount;

    int32 CoastViolations;
    int32 OceanBelowSeaViolations;
    int32 SquareEdgeOceanViolations;
};
```

Recommended pass gates:

```text
ActiveConeEquivalentDiameterKm <= 9.0
ActiveConeLandFraction <= 0.025

HighlandEquivalentDiameterKm <= 22.0
HighlandLandFraction <= 0.10

OldVolcanicLandFraction <= 0.20
OldBenchSlopeP95Deg <= 18

LavaPathHydrologyAlignment01 >= 0.72
LaharDrainageAlignment01 >= 0.85
CollapseAxisTerrainScore01 >= 0.65

RadialLeakageScoreOutsideActive <= BaselineRadialLeakage * 1.15

VolcanoDisabledMajorCatchmentCount >= 8
VolcanoDisabledReliefP90P10M >= project_baseline_min
VolcanoDisabledPreviewInteresting == true by debug review

CoastViolations == 0
OceanBelowSeaViolations == 0
SquareEdgeOceanViolations == 0
```

`project_baseline_min` should be calibrated from the accepted terrain preview. If no baseline exists yet, use a temporary target such as **P90–P10 relief >= 220 m** outside beaches and coastal flats, then replace with measured project baseline.

## Files And Areas Likely Touched

Exact file names depend on the current Jungle Game terrain codebase, but the likely implementation areas are:

```text
Terrain generation core
- island graph / regional terrain authority
- heightfield composition
- mask composition
- deterministic seed utilities

Volcanic subsystem
- FVolcanicConstruct
- FCraterRim
- FCollapseScar
- FRiftLine
- FSecondaryCone
- FLavaFlowPath
- FLavaLobe
- FLaharCorridor
- FVolcanicMetrics

Hydrology/catchment subsystem
- D8/D∞ or multiple-flow routing
- flow accumulation
- catchment labeling
- drainage graph tracing
- valley confinement / HAND-like metrics

Preview/debug exporters
- height preview maps
- masks
- vector/path graph overlays
- metrics report

Validation
- existing coastline gates
- new footprint gates
- radial leakage diagnostic
- volcano-disabled terrain interest gate
```

Suggested file/module grouping:

```text
Source/JungleTerrain/Public/Terrain/Volcanic/VolcanicTypes.h
Source/JungleTerrain/Private/Terrain/Volcanic/VolcanicConstruct.cpp
Source/JungleTerrain/Private/Terrain/Volcanic/VolcanicCrater.cpp
Source/JungleTerrain/Private/Terrain/Volcanic/VolcanicCollapse.cpp
Source/JungleTerrain/Private/Terrain/Volcanic/VolcanicRift.cpp
Source/JungleTerrain/Private/Terrain/Volcanic/VolcanicLava.cpp
Source/JungleTerrain/Private/Terrain/Volcanic/VolcanicLahar.cpp
Source/JungleTerrain/Private/Terrain/Volcanic/VolcanicMetrics.cpp
Source/JungleTerrain/Private/Terrain/Debug/VolcanicPreviewExport.cpp
```

## Large PR Implementation Scope

A large implementation PR should be scoped as a subsystem replacement, not an art/content pass.

Recommended PR boundaries:

1. Add volcanic data structures and metric structs.
2. Add local volcanic construct generation with strict footprint gates.
3. Add crater, nested vent, rim asymmetry, and terrain-selected breach.
4. Add collapse scar/horseshoe model with terrain-selected axis.
5. Add rift-line and secondary cone placement.
6. Add terrain-derived lava flow pathing and lobe deposition.
7. Add lahar corridor generation coupled to drainage/catchments.
8. Add old lava bench/degraded volcanic terrain masks.
9. Add terrain-derived crust/scree/blocker masks.
10. Add preview/debug exporters.
11. Add validation metrics and stop gates.
12. Preserve existing coastline/world/ocean acceptance gates.

Do not mix this PR with VFX, gameplay hazards, UI, material finalization, structures, quests, NPCs, survival systems, or foliage art.

## Acceptance Criteria

### Functional acceptance

- The volcano is visibly compact and embedded.
- The active cone no longer owns the whole island.
- The crater has a nested vent, broken rim, and terrain-selected breach.
- Collapse scar reads as a sector failure/horseshoe scar, not a circular crater or equal-angle bite.
- Rift/fissure lines have irregular terrain-owned alignment with secondary cones.
- Lava paths follow gullies, saddles, valleys, and slope descent.
- Lahar corridors follow drainage/catchments.
- Old lava benches are degraded, eroded, and low-relief.
- Blocker/unstable masks derive from slopes, scar walls, fissures, levees, lava thickness/edges, roughness, and boulder chokes.
- Disabling volcanic contributions still leaves an interesting island.

### Metric acceptance

```text
world_size_m == 97536.0 target
shoreline_error_max_m == 0 or effectively 0
beach_continuity_pct == 100
ocean_below_sea_pct == 100
square_edge_ocean_violations == 0

active_cone_equivalent_diameter_km <= 9.0
active_cone_land_fraction <= 0.025
broader_highland_equivalent_diameter_km <= 22.0
broader_highland_land_fraction <= 0.10
old_volcanic_terrain_land_fraction <= 0.20

lava_path_hydrology_alignment >= 0.72
lahar_drainage_alignment >= 0.85
radial_leakage_outside_active <= baseline * 1.15

volcano_disabled_preview passes terrain-interest review
```

### Visual/debug acceptance

Required preview images prove:

- compact active cone;
- bounded broader highland;
- no global crater-centered radial authority;
- lava channels are terrain-owned;
- lahars are catchment-owned;
- old lava benches are eroded and integrated;
- the coastline remains unchanged;
- volcano-disabled terrain remains interesting.

## Risks And Stop Conditions

Stop implementation or fail validation if:

- any coastline/ocean/square-edge gate fails;
- active cone or highland masks exceed footprint limits;
- crater-centered radial spokes appear outside active cone/highland masks;
- lava path debug lines ignore gullies and cut across drainage divides;
- lahar corridors do not follow drainage networks;
- collapse scar direction is a fixed sector rather than terrain-selected;
- volcano-disabled preview becomes bland or structurally empty;
- old lava benches flatten/erase non-volcanic ridges or catchments;
- blocker masks are gameplay-painted rather than terrain-derived;
- final composition changes world size or coastal continuity;
- preview outputs cannot explain where volcanic contributions are applied.

## Rejected Or Forbidden Approaches

Forbidden:

- global crater-centered radial terrain authority;
- whole-island radial ridge/gully generation from the summit;
- equal-angle lava channels;
- equal-angle lahar sectors;
- fixed compass breach/collapse directions without terrain scoring;
- volcano-centered noise domain controlling the whole island;
- active cone footprint exceeding hard gates;
- using lava/crust/blockers as gameplay/VFX hazards in this subsystem;
- painting blockers because “lava is dangerous” rather than because terrain blocks traversal;
- allowing volcanic height contributions to bypass coast clamp;
- changing coastline/ocean/edge acceptance gates to make volcano pass;
- hiding failures by smoothing everything into a generic mound;
- making old lava benches perfectly flat plates;
- making secondary cones evenly spaced beads on a radial line.

## Sources And References

The following sources informed the terrain morphology and approximation strategy. They should be used as constraints, not copied as simulation requirements.

- [British Geological Survey — Types of volcano][src-bgs-volcano-types]  
  Used for the distinction between steep stratovolcano/composite slopes and gentle shield-like basaltic slopes.

- [National Park Service — Composite Volcanoes / Stratovolcanoes][src-nps-composite]  
  Used for composite-cone morphology, concave profile, and mixed lava/pyroclastic/lahar construction.

- [USGS Volcano Hazards Program Glossary][src-usgs-glossary]  
  Used for terrain-relevant definitions: cinder cone, composite volcano, edifice, fissure, lahar, lava tube, debris avalanche, drainage, and related volcanic terms.

- [Michigan Tech Volcanoes — Volcanic Edifice Collapse and Its Products][src-mtu-collapse]  
  Used for horseshoe amphitheater/scar morphology and collapse-scar scale references.

- [Romero et al. 2021 — Volcanic Lateral Collapse Processes in Mafic Arc Edifices][src-romero-collapse]  
  Used for sector-collapse scar geometry, opening-angle behavior, and collapse consequences.

- [USGS — Lahars move rapidly down valleys like rivers of concrete][src-usgs-lahars]  
  Used for lahar generation mechanisms and the requirement that lahar terrain corridors follow valleys/drainage.

- [USGS — LAHARZ][src-usgs-laharz]  
  Used for the principle that lahar/debris-flow inundation mapping can be estimated rapidly from topography.

- [USGS Fact Sheet 2018-3024 — Lahar: River of Volcanic Mud and Debris][src-usgs-lahar-fs]  
  Used for lahar generation, valley routing, volume classes, and drainage-linked hazard mapping examples.

- [Tarboton 2025 — Reflections on Geomorphometry and Hydrology][src-tarboton]  
  Used for D∞ flow-direction concepts, D8 limitations, flow accumulation, and terrain-based drainage analysis.

- [Génevaux et al. 2013 — Terrain Generation Using Procedural Models Based on Hydrology][src-genevaux]  
  Used for hydrology-first procedural terrain principles: river/catchment structure should inform terrain synthesis.

- [Pedersen et al. 2023 — Lava flow hazard modeling during the 2021 Fagradalsfjall eruption][src-fagradalsfjall]  
  Used for MrLavaLoba-style lava flow modeling, valley infill/spillover, and topographic modification concepts.

- [Sallermann et al. 2024 — Flowy: High performance probabilistic lava emplacement prediction][src-flowy]  
  Used for probabilistic steepest-descent lava routing and lobe-emplacement concepts.

[src-bgs-volcano-types]: https://www.bgs.ac.uk/discovering-geology/earth-hazards/volcanoes/how-volcanoes-form/
[src-nps-composite]: https://www.nps.gov/articles/000/composite-volcanoes.htm
[src-usgs-glossary]: https://www.usgs.gov/glossary/volcano-hazards-program-glossary
[src-mtu-collapse]: https://www.geo.mtu.edu/volcanoes/fuego/prod.html
[src-romero-collapse]: https://www.frontiersin.org/journals/earth-science/articles/10.3389/feart.2021.639825/full
[src-usgs-lahars]: https://www.usgs.gov/programs/VHP/lahars-move-rapidly-down-valleys-rivers-concrete
[src-usgs-laharz]: https://www.usgs.gov/software/laharz
[src-usgs-lahar-fs]: https://pubs.usgs.gov/fs/2018/3024/fs20183024.pdf
[src-tarboton]: https://www.geomorphometry.org/uploads/pdf/pdf2025/Geomorphometry_2025_Tarboton.pdf
[src-genevaux]: https://hal.science/hal-01339224/file/siggraph2013.pdf
[src-fagradalsfjall]: https://nhess.copernicus.org/articles/23/3147/2023/
[src-flowy]: https://arxiv.org/html/2405.20144v1
