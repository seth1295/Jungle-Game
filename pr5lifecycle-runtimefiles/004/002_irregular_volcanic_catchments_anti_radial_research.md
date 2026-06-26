# Irregular Volcanic Catchments And Anti-Radial Drainage Research

## Executive Summary

The current Jungle Game PR5 Batch 003 island terrain has a working coast and a usable volcanic mass, but the interior drainage morphology fails because ridge and gully masks read as equal-angle polar spokes. The fix should not be another layer of random noise. The fix should replace the radial wedge model with a deterministic irregular drainage system: nonuniform basin outlets along the coast, offset valley heads on the volcanic flanks, curved basin centerlines, competitive ridge divides, controlled secondary and tertiary tributaries, and locally disruptive volcanic events such as crater breach, lava resurfacing, lahar corridors, landslide scars, and coastal fans.

The key implementation shift is from **angle-indexed radial functions** to a **seeded basin graph over warped island space**. Each basin becomes an authored-by-code landform object with deterministic seed data, outlet, head, trunk path, width envelope, incision envelope, divide pressure, branch set, volcanic-process modifiers, and validation metrics. The generator should then rasterize these basin objects into height, ridge, gully, erosion, and debug masks while preserving the existing coast by applying interior contributions through a coast-safe falloff and reasserting shoreline/beach/shelf/ocean invariants after interior terrain synthesis.

The design must scale to the Batch 004 6x linear increase: approximately **97.536 km-class world extent** rather than the old 16.256 km footprint. The basin system should therefore use normalized island coordinates for topology, world meters for geomorphic widths and validation thresholds, and chunk-stable deterministic hashes for repeatability. At the larger scale, the terrain can support fewer giant basins, many mid-scale basins, and nested tributary systems without degenerating into fine noise.

Research grounding: recent volcanic-edifice drainage work shows that volcanic basins may begin more uniform on young radial forms but become less uniform through erosion, divide migration, basin competition, and localized volcanic resurfacing; useful metrics include basin length, width, drainage density, hypsometry, outlet spacing, and divide stability [1]. DEM hydrology tooling commonly validates drainage by flow direction, flow accumulation, stream extraction, and watershed basin delineation [2][3]. Lahars, landslides, and lava interactions should be represented as valley-following and topography-controlled disturbances rather than repeated wedges [4][5][6]. Procedural terrain literature supports graph/hydrology-inspired terrain synthesis and controlled erosion constraints rather than unstructured noise-only terrain [7][8].

## Terrain-Only Scope Boundary

This document is terrain morphology only. It specifies how to produce believable irregular volcanic island drainage and terrain masks for a UE5 Linux-first first-person project.

Included:

- island-scale heightfield morphology;
- volcanic flank drainage basin layout;
- ridge divides, gullies, valleys, valley heads, outlets, coastal fans;
- lahar corridors, lava-flow terrain effects, crater breach morphology, landslide scars;
- terrain masks and developer preview/debug layers;
- validation metrics for anti-radial acceptance;
- coast preservation ordering and invariants.

Excluded:

- foliage, ecology, weather, audio, survival systems, NPCs, VFX, final materials, buildings, POIs, quests, player UI;
- player-facing map, minimap, compass, GPS, objective, bearing, time, or orientation UI;
- gameplay traversal tuning except where terrain morphology would create impossible/unusable terrain by accident;
- final art direction outside terrain form and masks.

## Current Preview Failure

The preview failure is not merely “too many ridges” or “not enough erosion.” It is a structural generator issue. The ridge/gully mask is visibly governed by an angular partition around a central peak. The terrain therefore reads as:

```text
central volcano + equal-angle polar spokes + repeated fan blades
```

This is visually synthetic because the human eye sees repeated angular spacing before it sees any local noise detail. Adding fBm/noise on top of a regular polar structure will not solve the problem; it will produce noisy fan blades. The implementation must break the equal-angle assumption at the basin identity level.

Current likely failure mode:

1. A fixed number of ridge/gully sectors is generated around the volcano.
2. Sector center angles are evenly spaced or nearly evenly spaced.
3. Ridge/gully amplitude is a function of angular distance to each sector.
4. Radial distance modulates strength, causing similar spokes from summit to coast.
5. Noise perturbs only the surface detail, not the basin topology.
6. The coast works because coastal masks are stable, but interior terrain reads like a polar plot.

The rewrite should preserve the coast system but replace the interior drainage system with a deterministic graph of irregular basins.

## Hard Anti-Radial Requirements

The next generator must reject the following at preview/CI validation time:

```text
equal-angle radial catchments
perfectly repeated fan blades
gear teeth around the volcano
uniform spoke gullies
uniform spoke ridges
single-ring crater with symmetric drainage
ridge/gully masks that look like a polar plot
```

Implementation rules:

1. **No basin identity from integer sector angle alone.** A basin can have an angular footprint, but its outlet, head, trunk, width envelope, curvature, and neighbors must come from seeded graph data.
2. **No equal spacing fallback.** If seed generation fails, do not fall back to `2π / N` radial sectors. Fail validation and emit debug artifacts.
3. **No globally repeated ridge/gully curves.** Ridge and gully strength curves may share a library, but each basin must vary width, length, curvature, incision, head elevation, outlet character, and process modifiers.
4. **No symmetric crater-only solution.** A crater may exist, but at least one breach, inherited flank scar, lava-resurfaced area, or dominant basin asymmetry must disrupt perfect summit symmetry.
5. **No noise-only fix.** fBm/domain warp is allowed only as controlled perturbation of basin anchors, paths, widths, and local micro-relief, not as the primary drainage model.
6. **Coast invariants are hard gates.** Anti-radial improvement cannot regress shoreline/beach/shelf/ocean metrics.

## Natural Volcanic Drainage Goals

The goal is not to remove radial influence entirely. A volcanic island should still inherit a broad outward drainage tendency from the edifice. The target is **irregular volcanic radial-dendritic morphology** rather than regular polar spokes.

Natural goals:

- major basins have uneven outlet spacing around the island perimeter;
- some basins are dominant and wide, others are narrow passive chute-like basins squeezed by stronger neighbors;
- basin widths vary with elevation and radial distance, not uniformly from center to coast;
- headwaters are offset around the summit and do not all start at one exact crater center;
- valleys curve, hook, or are deflected by older lava ridges, landslide scarps, and asymmetric cone growth;
- ridge divides migrate/lean toward weaker basins and are not centered halfway between equal sectors;
- secondary tributaries join trunks at varied angles and elevations;
- tertiary gullies exist as short controlled incisions, not full-length radial stripes;
- crater breach and lahar corridors create a few overprinted, strongly incised routes;
- lava flows locally bury/armor/deflect older drainage;
- coastal outlets create uneven fans, deltas, notches, or alluvial aprons without breaking beach continuity.

Volcanic-edifice studies support this direction: drainage basin geometry, outlet spacing, width, length, drainage density, and divide behavior change as volcanic terrain matures, with basin competition producing non-uniform configurations and dominant/passive basin relationships [1]. This is the right conceptual anchor for Jungle Game: keep an edifice-scale outward tendency, but let erosion, competition, and volcanic events break the fan-blade regularity.

## Deterministic Irregular Basin Seeding

### Core principle

Seed **basins as objects**, not angular functions.

The generator should create a stable `FVolcanicDrainageNetwork` from the world seed and terrain version. The network owns arrays of primary basins, secondary branches, tertiary gullies, volcanic process features, outlets, valley heads, ridge divides, and validation samples. The heightfield/mask pass then rasterizes this network.

### Recommended deterministic data structures

```cpp
struct FVolcanicBasinSeed
{
    int32 BasinId;
    uint64 SeedHash;

    // Normalized island-space anchors. Center is volcano reference frame.
    float OutletT;              // normalized arclength along accepted coastline, not raw angle
    FVector2D OutletXY_N;       // normalized island coordinates
    float OutletAngleRad;       // derived only for metrics/debug
    float OutletRadiusN;

    FVector2D HeadXY_N;         // upper flank / crater breach / shoulder head
    float HeadElevationN;
    float HeadAngleRad;
    float HeadRadialDistanceN;

    FVector2D MidAnchorA_N;     // path curvature anchor
    FVector2D MidAnchorB_N;     // optional second curvature anchor
    FVector2D BasinBiasXY_N;    // asymmetric drift/warp vector

    float Dominance;            // controls width, divide pressure, capture strength
    float Maturity;             // older = more branched/incised; younger = smoother/resurfaced
    float IncisionStrength;
    float RidgeReliefStrength;
    float WidthScaleM;
    float WidthAsymmetry;
    float CurvatureStrength;
    float OutletFanStrength;
    float CoastProtectionFalloffM;

    EVolcanicBasinType Type;    // Dominant, PassiveChute, Breach, Lahar, LavaDeflected, LandslideCaptured
};
```

```cpp
struct FDrainageBranchSeed
{
    int32 BranchId;
    int32 ParentBasinId;
    int32 ParentBranchId;       // -1 for trunk
    uint64 SeedHash;

    float JoinT;                // 0=head, 1=outlet along parent trunk
    float SideSign;             // -1 or +1 relative to trunk direction
    float LengthM;
    float WidthM;
    float IncisionStrength;
    float JoinAngleDeg;
    float StartElevationN;
    float EndElevationN;
    float CurvatureStrength;
    int32 StrahlerOrderHint;    // trunk high, secondary medium, tertiary low
};
```

```cpp
struct FDrainageValidationMetrics
{
    float AngularGapCV;
    float AngularRegularityScore;
    float OutletSpacingCV;
    float BasinWidthCV;
    float RidgeGullyRotationalSymmetryScore;
    float PolarSpectrumPeakRatio;
    float FanBladeScore;
    float CoastInvariantErrorMaxM;
    float BeachContinuityPct;
    float OceanBelowSeaPct;
    int32 SquareEdgeOceanViolations;
};
```

### Seed generation order

1. Build or receive the accepted island/coastline model from the existing coast system.
2. Extract the coast sampling polyline/ring used by the preview exporter.
3. Generate candidate primary outlet positions by coastline arclength, not by angle.
4. Use a deterministic nonuniform gap distribution to place primary basins.
5. Assign dominance/maturity/process type to each basin.
6. Generate valley heads from upper-flank target zones with deterministic angular offsets and elevation variation.
7. Build curved trunk paths from head to outlet using warped anchors.
8. Resolve basin adjacency and initial ridge divides.
9. Add secondary and tertiary branches according to basin dominance, length, maturity, and available side space.
10. Apply volcanic process overprints: crater breach, lava deflection, lahar corridors, landslide scars.
11. Validate anti-radial metrics before heightfield rasterization.
12. Rasterize interior terrain with coast-safe falloff.
13. Reassert coast invariants and export preview metrics.

### Nonuniform outlet spacing

Do not use equal-angle sectors. Use one of these deterministic methods:

#### Option A: Gamma/Dirichlet angular gaps with rejection gates

Generate `N` positive gap weights from a deterministic gamma distribution, normalize to `2π`, then reject if gaps are too uniform or too extreme.

Recommended settings:

```text
primary_basin_count_97km: 14-26
min_gap_deg: 5-9
max_gap_deg: 45-80 depending island shape
angular_gap_cv_min: 0.28
angular_gap_cv_target: 0.45-0.85
angular_gap_cv_max: 1.25
```

This produces uneven spacing without clustering all outlets into one side.

#### Option B: Coastline Poisson samples with weighted repulsion

Sample outlet candidates along the real coastline arclength. Apply deterministic weighted repulsion where dominant basins have larger exclusion radii and passive basins can exist in tighter gaps.

Recommended:

```text
dominant outlet exclusion radius: 4.5-9.0 km at 97.536 km scale
normal outlet exclusion radius: 2.0-5.5 km
passive chute min spacing: 0.9-2.2 km
```

This is better than angle spacing because irregular coast geometry naturally affects outlet distribution.

#### Option C: Hybrid arclength + volcano-visible angle constraints

Use coastline arclength for candidate placement but validate angular distribution from the volcano center. Reject if the final set has equal-angle artifacts.

This is the recommended option because the preview failure is angular, while the coast preservation requirement depends on the actual perimeter.

### Basin head seeding

Valley heads should not all begin at the crater center. Generate them from upper-flank zones:

```text
head_radial_distance_n: 0.10-0.38 from volcano reference center
head_elevation_n: 0.55-0.96, varied by basin type
head_angle_offset_from_outlet: -35° to +35°, biased by flank warp
breach basin head: may start at crater rim breach, not full crater center
passive chute head: starts lower, 0.28-0.55 radial distance
landslide-captured basin head: starts at scar headwall or sidewall
lava-deflected basin head: starts beside lava levee/high area
```

Head placement should use deterministic domain-warped vectors:

```cpp
FVector2D HeadBase = VolcanoCenter + RadialDir(OutletAngle + HeadOffset) * HeadRadiusN;
FVector2D HeadWarp = DomainWarp2D(HeadBase, SeedHash, LargeScaleWarp) * HeadWarpAmplitudeN;
HeadXY_N = ClampToUpperFlankMask(HeadBase + HeadWarp);
```

### Basin anchor warping

Each trunk valley should be a spline/polyline, not a straight radial ray.

Recommended trunk path construction:

1. `P0 = HeadXY_N`
2. `P3 = OutletXY_N`
3. `P1 = lerp(P0, P3, 0.28) + tangent_perp * curveA + warpA`
4. `P2 = lerp(P0, P3, 0.68) + tangent_perp * curveB + warpB`
5. Convert cubic Bezier to a sampled valley centerline.
6. Enforce generally descending profile from head to outlet.
7. Reject if path crosses too many stronger basins or exits land mask before outlet.

The perpendicular offsets must vary by basin, dominance, and inherited flank bias. Allow rare strong hooks, especially for lava-deflected or landslide-captured basins.

### Determinism requirements

- Use stable integer hashes from `(WorldSeed, TerrainVersion, BasinId, FeatureKind)`.
- Do not depend on thread iteration order.
- Do not use platform-dependent random streams for validation-critical layout unless already proven deterministic across Linux/Windows targets.
- Store all generated basin seed values in debug output so a bad preview can be reproduced exactly.
- Normalize all island-space seed coordinates, then convert to world meters at rasterization time.
- Use scale-aware constants: widths, branch lengths, and validation windows should be expressed in meters, not heightmap pixels.

## Variable Catchment Width And Curvature

### Width envelope

Each primary basin should have a width function along its trunk, not a constant angular wedge.

Use `t = 0` at valley head and `t = 1` at outlet.

Recommended width model:

```text
W(t) = W_base
     * dominance_scale
     * maturity_scale
     * head_taper(t)
     * midflank_bulge(t)
     * outlet_taper_or_fan(t)
     * local_asymmetry(t)
```

Example curve components:

```text
head_taper(t)      = smoothstep(0.00, 0.22, t)
midflank_bulge(t)  = 1.0 + bulge_strength * exp(-((t - bulge_center)^2 / bulge_width))
outlet_taper(t)    = lerp(0.55, 1.10, smoothstep(0.72, 0.95, t))
fan_spread(t)      = 1.0 + fan_strength * smoothstep(0.84, 1.00, t)
```

Important: do not make every basin widest at the same `t`. Recent volcanic-edifice drainage analysis notes that basin maximum widths can occur at different heights on the edifice, with divide asymmetry often in mid/lower flanks [1]. Encode this directly:

```text
bulge_center: 0.35-0.82
bulge_width: 0.12-0.35
bulge_strength: 0.15-1.40
```

### Basin length variation

At 97.536 km scale, the island can support several length classes:

| Basin class | Trunk length target | Role |
|---|---:|---|
| Dominant long basin | 22-48 km | Major drainage valleys from upper flank to coast |
| Normal primary basin | 12-34 km | Main surrounding catchments |
| Passive chute basin | 5-18 km | Narrow basins squeezed between dominant basins |
| Crater breach / lahar basin | 18-55 km | Strong overprinted corridor |
| Lava-deflected basin | 10-35 km | Curved path diverted around lava ridge or young flow |
| Landslide-captured basin | 8-30 km | Abrupt headwall/scar plus captured drainage |

These numbers are not fixed authorial demands. They are scale-aware targets to prevent the old 16.256 km assumptions from producing toy-sized drainage on the enlarged island.

### Curvature model

Curvature should come from multiple causes:

- coast outlet mismatch with head angle;
- volcano center offset relative to island centroid;
- domain-warped older structural lineaments;
- lava-flow levees/highs deflecting valleys;
- landslide scars capturing adjacent gullies;
- basin competition pushing divides sideways.

Recommended trunk curvature metrics:

```text
path_sinuosity = path_length / straight_head_to_outlet_distance
primary basin target: 1.04-1.45
lava-deflected target: 1.15-1.75
passive chute target: 1.00-1.18
breach/lahar target: 1.02-1.35 but deeper incision
```

Reject a network if more than 45% of primary basins have `sinuosity < 1.05`, because this still reads like straight spokes.

### Elevation head variation

Head elevation should vary with type:

```text
dominant basin head: 0.72-0.96 normalized volcano height
normal basin head:   0.55-0.88
passive chute head:  0.36-0.70
breach basin head:   crater rim / breach notch
landslide head:      scar headwall, often 0.48-0.82
lava-deflected:      beside/above flow margin, 0.45-0.85
```

This prevents every gully from starting at the same summit ring.

### Outlet shape variation

Outlets should vary between:

- narrow stream notch cutting through coastal shelf;
- small fan apron landward of beach;
- broad alluvial/lahar fan behind coast lock band;
- split/braided low-gradient fan inside the land mask;
- cliff/notch outlet where steep terrain reaches near the beach;
- buried or deflected outlet where lava flow creates a local high.

The outlet fan must never lower ocean cells, break beach continuity, or create square-edge ocean artifacts.

## Ridge Divide Model

### Replace midpoint angular ridges with competitive divides

A ridge divide should emerge from competition between adjacent basins, not from fixed sector boundaries.

For every sample point on land, compute basin influence from valley centerlines:

```text
Influence_i(p) = basin_weight_i
               * width_envelope_i(t_i)
               * longitudinal_relevance_i(t_i)
               / (distance_to_trunk_i + epsilon)^falloff_i
```

Then identify the strongest and second-strongest basins:

```text
winner = argmax Influence_i(p)
runner_up = second_argmax Influence_i(p)
competition_margin = winner_strength - runner_up_strength
```

Ridge divide potential is high where:

- two basins compete closely;
- the point is not inside a trunk valley/gully core;
- local slope supports a divide;
- the candidate divide is not inside the coast lock band;
- dominant basin pressure can push the divide toward weaker basins.

### Divide asymmetry

Do not place divides halfway between basins. Use dominance and maturity:

```text
divide_bias = (dominance_A - dominance_B) * dominance_bias_scale
            + (incision_A - incision_B) * incision_bias_scale
            + local_structural_warp
```

The stronger basin should widen and push the divide toward the weaker basin. Passive chute basins become narrow because neighboring dominant basins compress them.

### Ridge strength curve

Ridge masks should be strongest on divides and shoulders, not as full radial stripes.

Recommended:

```text
ridge_strength(p) = divide_competition_peak
                  * shoulder_elevation_weight
                  * not_valley_core_weight
                  * not_coast_lock_weight
                  * process_modifier
```

Where:

```text
divide_competition_peak = exp(-abs(log(winner_strength / runner_up_strength)) * sharpness)
shoulder_elevation_weight = smoothstep(low_flank, mid_flank, elevation_n) * (1 - summit_crater_suppression)
not_valley_core_weight = 1 - max_gully_core_mask
```

### Ridge divide continuity

Ridges should be traceable but not perfectly straight. Use a divide graph:

1. Sample candidate divide points from competition fields.
2. Connect points into divide polylines between basin pairs.
3. Smooth lightly in island space.
4. Apply low-amplitude domain warp.
5. Rasterize as ridge mask with varied width.

Reject isolated ridge flecks and full-length radial stripes. A divide should usually separate two named basins and terminate at upper flank shoulders, a crater rim, a landslide scar boundary, or a coastal high/fan edge.

### Ridge/gully competition rule

Ridge and gully masks must be mutually exclusive in their cores:

```text
gully_core_mask > 0.65 => ridge_mask <= 0.15
ridge_core_mask > 0.65 => gully_mask <= 0.20
transition shoulders may blend
```

Use a final normalization pass:

```cpp
float G = Saturate(GullyRaw);
float R = Saturate(RidgeRaw * (1.0f - SmoothStep(0.35f, 0.85f, G)));
float Shoulder = Saturate(ShoulderRaw * (1.0f - Max(G, R) * 0.45f));
```

This prevents masks from both drawing the same spoke.

## Gully And Valley Incision Model

### Valley trunk incision

Each basin trunk should carve a valley profile perpendicular to its centerline. Use signed distance to the trunk and local width.

Recommended normalized cross-section:

```text
x = lateral_distance / valley_half_width
valley_core = exp(-x^2 * core_sharpness)
valley_floor = smoothstep(1.0, 0.0, abs(x))^floor_power
shoulder = smoothstep(0.5, 1.6, abs(x)) * smoothstep(2.4, 1.2, abs(x))
```

Height contribution:

```text
height_delta = -incision_depth(t) * valley_core
             + shoulder_lift(t) * shoulder
             + local_bed_roughness
```

Incision depth should vary by basin and downstream position:

```text
incision_depth(t) = basin_incision
                  * slope_energy(t)
                  * flow_accumulation_proxy(t)
                  * maturity
                  * process_boost
```

Use a proxy before full DEM flow routing:

```text
flow_accumulation_proxy = trunk_order_weight + upstream_branch_count_weight + catchment_area_estimate
```

### Longitudinal profile

The valley floor should generally descend toward the outlet. A simple monotonic profile can be generated along trunk length:

```text
z_floor(t) = mix(head_z, outlet_z, profile_curve(t)) - incision_depth(t)
profile_curve(t) = pow(t, concavity_exponent)
concavity_exponent: 0.55-1.35 by basin maturity/type
```

Avoid perfectly identical concavity. Mature dominant basins can be smoother and more graded. Young lava-resurfaced basins can have abrupt steps, perched channels, or underdeveloped incision.

### Gully mask strength

Gully mask should represent valley incision and drainage lines, not all downhill directions.

```text
gully_mask = trunk_core
           + secondary_branch_core
           + tertiary_gully_core
           + lahar_corridor_boost
           - lava_armor_suppression
           - fan_deposition_smoothing
```

Clamp and blur only within local anisotropic tangent directions. Do not globally blur around the volcano, because that will reintroduce ring/spoke artifacts.

### Noise/fBm/domain warp use

Allowed:

- perturb trunk control anchors before rasterization;
- perturb valley width envelope by low-frequency basin-local noise;
- add minor bank roughness and scar texture inside a basin;
- vary branch start positions and lateral offsets;
- add nonuniform coastal fan lobes.

Forbidden:

- using polar noise as the main ridge/gully identity;
- high-frequency noise that creates unreadable drainage mush;
- per-pixel random erosion that is not tied to basin graph structure;
- non-deterministic GPU-only noise for validation-critical layout.

Recommended domain warp scales for 97.536 km world:

```text
large structural warp wavelength: 12-35 km
basin path warp wavelength:       4-14 km
valley-bank roughness wavelength:  0.35-2.5 km
tertiary gully wavelength:         0.15-0.8 km
```

## Secondary / Tertiary Drainage Branching

### Branching goal

Branches should add believable drainage hierarchy without turning the terrain into random scratches. The branch system should be graph-controlled, order-limited, and basin-aware.

DEM hydrology workflows commonly reason in terms of flow direction, accumulation, stream networks, and watershed basins [2][3]. Jungle Game does not need full offline hydrologic simulation for every preview, but it should imitate those structures enough that debug flow accumulation agrees with visible valleys.

### Secondary branch rules

Generate secondary tributaries from each primary basin using basin length, width, maturity, and dominance.

Recommended counts at 97.536 km scale:

```text
dominant basin: 4-12 secondary branches
normal basin:   2-7 secondary branches
passive chute:  0-3 secondary branches
breach/lahar:   1-6 branches, many captured/parallel near corridor
lava-young:     0-4 branches, weak incision or deflected
landslide:      2-8 branches around scar margins
```

Branch placement:

```text
JoinT: 0.18-0.86, avoid exact regular spacing
SideSign: choose side with available basin area
Length: proportional to local basin half-width, capped by adjacent divide distance
JoinAngle: 25°-70° upstream-facing relative to trunk downstream direction
Incision: weaker than trunk, increases with maturity and catchment area
```

Do not make branches evenly alternate left/right. Use deterministic side-space scoring:

```text
side_score = distance_to_divide_on_side
           + local_slope_convergence
           - nearby_branch_penalty
           + process_bias
```

### Tertiary gully rules

Tertiary gullies should be short, slope-controlled incisions, not long radial spokes.

Recommended:

```text
length: 0.4-4.5 km at 97.536 km scale
width: 20-180 m depending heightfield resolution and visual target
join_t: biased toward upper/mid basin, 0.08-0.72
max_order: 3 for preview rewrite; do not recurse endlessly
min_spacing: 250-900 m depending local scale
```

Tertiary gullies should be culled if:

- they cross a primary trunk unrealistically;
- they run parallel to the volcano radial vector for too long;
- they approach the coast lock band;
- they create a dense scratch field with no visible trunk hierarchy;
- they push gully coverage above validation max.

### Branch geometry

Branch start points should come from slopes near divides or upper shoulders and join parent valleys at an acute upstream angle.

Algorithm:

1. Pick join point on parent trunk.
2. Evaluate left/right available catchment width.
3. Pick branch source near divide shoulder or upper slope.
4. Build curved branch path into parent join point.
5. Enforce descending or mostly descending profile.
6. Rasterize with lower incision and narrower width than parent.
7. Update gully mask and branch debug graph.

### Avoiding noise mush

Use these gates:

```text
max_gully_mask_coverage_land_pct: 8-22% depending target terrain roughness
max_tertiary_to_primary_length_ratio: 0.28
min_visible_trunk_to_branch_incision_ratio: 1.5
min_branch_spacing_m: resolution dependent, default >= 350 m
branch_crossing_count: must be 0 except deliberate capture cases
```

If the preview becomes uniformly scratched, branch generation must fail validation rather than hiding the failure with smoothing.

## Lahar, Lava, Breach, And Landslide Interaction

### Principle

Volcanic process features should overprint the drainage network locally. They must not be implemented as repeated radial wedges.

### Crater breach

A crater breach is a high-priority asymmetric outlet from the summit/crater rim into one basin.

Recommended:

- choose 1 major breach, optionally 1 minor older breach;
- breach direction should be biased by weakest rim elevation, not equal sectors;
- breach basin receives stronger upper-flank incision and lahar potential;
- crater rim is incomplete/asymmetric;
- breach path connects to a trunk valley, not a full symmetric ring.

Implementation:

```text
breach_notch_mask = carved rim gap along selected basin head
breach_corridor = upper trunk segment with boosted incision
rim_preservation = crater rim mask suppressed only near breach arc
```

Reject if crater rim becomes a perfect circle with identical gullies around it.

### Lahar corridors

Lahars are water-rich mixtures of volcanic material that move downslope and typically enter river valleys; they can entrain material and grow as they move downstream [4]. Therefore, represent lahars as **valley-following corridors** with deposition zones, not straight radial stripes.

Recommended lahar behavior:

- start from crater breach, landslide scar, loose pyroclastic deposit, or upper flank channel;
- follow an existing trunk valley or carve a nearby lower-resistance path;
- deepen/narrow upper corridor;
- widen and deposit on lower gradients;
- create irregular levees/fan lobes near lower flank/coastal plain;
- interact with coastal fans only inside coast-safe landward bands.

Corridor profile:

```text
upper t: incision boost, narrow channel
mid t: high gully mask, rough banks, levee hints
lower t: deposition smoothing, fan widening, reduced incision
```

### Lava flows

Lava paths are controlled by topography; depressions can thicken/accelerate flow while mounds can divert it [6]. Use this as terrain grammar:

- lava flows preferentially descend but may pond in depressions;
- lava creates young smoother/armored surfaces with weaker dendritic dissection;
- lava margins form subtle levees/highs that deflect later drainage;
- older drainage can be buried, forcing new gullies around the flow edge;
- flow termini can form lobate lower-flank shapes, not triangular wedges.

Implementation effects:

```text
lava_surface_mask: smooths pre-existing gully/ridge detail
lava_margin_ridge_mask: subtle positive relief along irregular margins
lava_deflection_field: biases trunk/branch paths away from armored center
lava_age: controls how much new incision cuts into flow
```

Do not draw lava as repeated symmetric radial slices. Each flow should have its own source vent/fissure, path, width, lobe structure, and age.

### Landslide scars

Volcanic cones are prone to landslides because they are tall, steep, and can be weakened by hydrothermal alteration and layers of lava/fragmental debris [5]. Landslides may also transform into lahars if enough water/fine material is involved [5].

Terrain treatment:

- horseshoe or amphitheater headwall scar on one flank;
- hummocky/depositional lower area inside a basin or across basin boundaries;
- captured/deflected drainage around scar margins;
- truncated ridges at scar edges;
- local suppression of regular basin divides;
- possible downstream lahar corridor seeded from scar toe.

Recommended:

```text
large scars per island: 0-2
small scars per island: 2-8
scar width: 1.5-12 km at 97.536 km scale
scar head elevation: mid/upper flank
scar runout: follows valley but may cross low divides if major
```

### Combined process ordering

Recommended process order:

1. Base volcano mass and broad island form.
2. Irregular primary basin graph.
3. Competitive ridge divides.
4. Primary trunk incision.
5. Secondary/tertiary branch incision.
6. Crater breach overprint.
7. Landslide scar cuts and deposits.
8. Lava flow armor/levee/deflection overprint by relative age.
9. Lahar corridor incision/deposition overprint.
10. Coastal fan/apron landward treatment.
11. Coast lock and shoreline invariants reasserted.
12. Final mask normalization and metrics.

Relative age should matter. A young lava flow may bury older gullies; a younger lahar may cut through a lava surface; older landslide scars may be partly re-incised by new tributaries.

## Coastal Outlet And Fan Treatment

### Outlet placement

Outlets should be sampled from the accepted coastline model by arclength. Never infer outlets from a perfect circle unless the coastline itself is circular.

Each outlet should store:

```text
coast arclength t
coast position XY
coast normal/tangent
beach/shelf lock mask value
local coastal slope
nearest valid landward fan anchor
outlet type
```

### Fan/apron types

| Type | Terrain effect | Use case |
|---|---|---|
| Narrow notch | small incised cut reaching beach backline | small/steep basins |
| Landward alluvial fan | triangular/rounded fan behind beach | mature basin on lower gradient |
| Lahar fan | wider, rougher lobe, levee hints | breach/lahar corridor |
| Split fan | trunk splits into 2-4 shallow distributaries inside land | large low-gradient outlet |
| Lava-blocked outlet | stream deflects along lava margin before exit | lava-deflected basin |
| Cliff outlet | minimal fan, strong valley to coast edge | steep flank/coast zone |

### Coast-safe fan rasterization

Fans may modify lower-flank terrain but must respect a coast protection envelope.

Recommended masks:

```text
M_ocean: existing ocean/container terrain mask
M_shelf: existing coastal shelf mask
M_beach: existing beach continuity mask
M_backbeach: inland transition band where minor fan shaping is allowed
M_interior: full interior terrain contribution allowed
```

Fan shaping rules:

```text
if M_ocean > 0: no interior height/mask edits
if M_shelf > 0: no interior height edits except approved shelf blend from coast system
if M_beach > 0: preserve beach elevation/continuity; only metadata/debug may mark outlet
if M_backbeach > 0: allow limited fan smoothing/deposition with bounded amplitude
if M_interior > 0: allow full fan shaping
```

Fan shapes should vary by outlet tangent, local valley direction, and low-gradient spread. Do not draw identical triangular fans at every outlet.

### Outlet/channel connection

A basin trunk should connect to the outlet in terrain masks, but the final meters near beach should be coast-safe. Use a “visual outlet notch” only where existing coast logic permits it. If the coast acceptance requires untouched beach, keep the stream/fan visible landward of the beach and mark outlet in debug overlay only.

## Coast Preservation Rules

The coast is already working and must remain protected.

Hard metrics to preserve:

```text
shoreline_error_max_m = 0 or effectively 0
beach_continuity_pct = 100%
ocean_below_sea_pct = 100%
square_edge_ocean_violations = 0
outer world edge = ocean/container terrain
island perimeter = beach/coastal shelf/sea-level transition
```

### Coast lock ordering

The terrain pipeline should treat coast as a late-stage invariant:

```text
Base island + volcano
Interior drainage synthesis
Volcanic process overprints
Coastal fan preliminary shaping
Coast-safe blend mask applied
Coast/beach/shelf/ocean invariants reasserted
Metrics calculated after final coast reassertion
```

### Interior contribution falloff

Every interior height contribution must be multiplied by a coast-safe weight:

```text
W_interior = smoothstep(coast_lock_end_m, coast_lock_start_m, distance_to_coast_m)
```

Example for 97.536 km scale:

```text
coast_lock_start_m: 0-250 m from accepted shoreline/beach/shelf critical band
coast_lock_end_m: 800-2500 m depending beach/shelf design
backbeach_fan_max_delta_m: small bounded value, e.g. 0.5-4.0 m if allowed
```

Where exact coast metrics are strict, use zero fan edits in beach/shelf/ocean bands.

### Sea-level clamp

After interior drainage and fans:

1. Reapply ocean/container terrain height below sea level.
2. Reapply shelf profile.
3. Reapply beach profile and continuity.
4. Clamp any interior-generated negative height that leaks into ocean/shelf masks.
5. Recompute shoreline error from final heightfield, not from pre-clamp data.
6. Fail if clamping hides excessive leakage; report `coast_leak_preclamp_max_m` as developer warning.

### Coast-safe basin outlet validation

Each basin outlet must pass:

```text
outlet is on accepted land/coast interface
trunk reaches landward outlet anchor
fan does not edit ocean/shelf cells
beach continuity remains 100%
no square-edge ocean violations
no basin path exits land except at intended outlet
```

### Do not couple anti-radial success to coast breakage

If interior rewrite produces good basins but breaks coast metrics, the PR is not acceptable. The correct solution is ordering/masking, not weakening coast acceptance.

## Preview / Mask Validation Metrics

Validation must prove two things simultaneously:

1. the interior no longer reads as radial/fan-blade/gear-shaped;
2. the coast remains exactly as accepted.

### Required metrics

#### Coast preservation metrics

```text
shoreline_error_max_m
beach_continuity_pct
ocean_below_sea_pct
square_edge_ocean_violations
coast_leak_preclamp_max_m
coast_leak_preclamp_cell_count
```

Acceptance:

```text
shoreline_error_max_m <= existing accepted tolerance, target 0
beach_continuity_pct == 100%
ocean_below_sea_pct == 100%
square_edge_ocean_violations == 0
coast_leak_preclamp_cell_count == 0 preferred; small warning only if final invariants remain exact and cause is understood
```

#### Angular regularity score

Compute outlet angles around volcano center. Sort angles. Compute angular gaps.

```text
mean_gap = 2π / N
gap_cv = std(gaps) / mean(gaps)
regularity_score = 1 - clamp(gap_cv / target_irregular_cv, 0, 1)
```

Interpretation:

```text
regularity_score near 1 = too regular / bad
regularity_score near 0 = sufficiently irregular / good
```

Acceptance starting target:

```text
angular_gap_cv >= 0.28
angular_regular_score <= 0.55
```

Do not overfit. Extremely clustered outlets are also bad:

```text
angular_gap_cv <= 1.25 unless island shape strongly justifies it
min_gap_deg >= 5° for primary outlets unless passive chute classified
```

#### Outlet-spacing variance

Compute spacing along actual coastline arclength, not only angle.

```text
outlet_spacing_cv = std(coast_arclength_gaps_m) / mean(coast_arclength_gaps_m)
```

Target:

```text
outlet_spacing_cv >= 0.25
outlet_spacing_cv <= 1.35
```

#### Basin-width variance

For each basin, sample width envelope at fixed normalized trunk positions:

```text
t_samples = [0.20, 0.35, 0.50, 0.65, 0.80]
width_cv_by_t = std(width_i(t)) / mean(width_i(t))
max_width_location_variance = variance(argmax_t(width_i(t)))
```

Target:

```text
mean width_cv_by_t >= 0.22
argmax_t distribution must not collapse to one shared t
```

#### Ridge/gully rotational symmetry score

Use mask autocorrelation under rotation around volcano center.

Algorithm:

1. Convert ridge/gully mask to polar sample grid for metrics only.
2. For rotation offsets `Δθ`, compute correlation with original mask.
3. Identify peaks near repeated sector intervals.
4. Penalize strong repeated peaks.

```text
rot_sym_score = max_corr_at_nontrivial_regular_offsets
```

Acceptance:

```text
ridge_rot_sym_score <= 0.45
gully_rot_sym_score <= 0.42
combined_rot_sym_score <= 0.44
```

Tune thresholds against accepted/bad previews.

#### Polar spectrum peak ratio

For annular bands around the volcano, compute angular Fourier spectrum of ridge/gully mask. Equal spokes produce strong narrow peaks at sector frequency.

```text
polar_peak_ratio = dominant_nonzero_angular_frequency_power / broadband_power
```

Acceptance:

```text
polar_peak_ratio <= calibrated_bad_preview_ratio * 0.55
```

Initial generic target:

```text
polar_peak_ratio <= 0.30-0.45 depending mask normalization
```

#### Fan-blade rejection gate

Detect repeated triangular basin sectors:

```text
fan_blade_score = weighted_sum(
    low_gap_cv_penalty,
    high_rotational_symmetry,
    low_path_sinuosity_fraction,
    shared_head_radius_penalty,
    shared_width_curve_penalty,
    angular_fourier_peak_penalty
)
```

Hard reject:

```text
fan_blade_score >= 0.60
or
angular_gap_cv < 0.20 and rot_sym_score > 0.50
or
>50% primary trunks have sinuosity < 1.04
or
>60% basin heads fall within same radial/elevation bin
```

#### Drainage hierarchy metrics

```text
primary_basin_count
secondary_branch_count
tertiary_gully_count
branch_crossing_count
mean_branch_join_angle_deg
branch_join_angle_stddev
gully_mask_coverage_land_pct
primary_to_secondary_incision_ratio
flow_accumulation_agrees_with_trunks_pct
```

Acceptance:

```text
branch_crossing_count == 0 except documented capture/landslide cases
gully_mask_coverage_land_pct within configured range
flow_accumulation_agrees_with_trunks_pct >= 80% target for debug DEM pass
```

#### Process-feature metrics

```text
crater_breach_count
breach_asymmetry_score
lava_flow_count
lava_flow_sinuosity_mean
lava_resurfaced_gully_suppression_pct
landslide_scar_count
scar_ridge_truncation_count
lahar_corridor_count
lahar_corridor_follows_valley_pct
```

Acceptance:

```text
major crater breach count: 1 preferred if crater exists
lava/lahar/scar features must not share equal angular spacing
lahar_corridor_follows_valley_pct high, target >= 75%
```

### Preview image requirements

The preview exporter should output:

1. final height shaded relief;
2. basin ID colored overlay;
3. primary trunk centerlines;
4. secondary/tertiary branch graph;
5. outlet points and coast arclength labels;
6. ridge divide overlay;
7. gully mask overlay;
8. ridge/gully competition mask;
9. crater breach/lava/lahar/landslide feature overlay;
10. angular outlet gap histogram;
11. polar spectrum debug plot;
12. rotational symmetry heatmap or small metric panel;
13. coast preservation metric panel;
14. before/after comparison against the rejected radial preview when available.

The issue should be obvious in one atlas: if the old mask looked like a polar plot, the new atlas should show named basin shapes with varied widths, curved trunks, and uneven outlets.

## Developer-Only Debug Outputs

Do not expose these as player UI. These are preview/export artifacts for developers only.

### JSON metrics file

Export:

```text
Saved/PR5Preview/terrain_metrics.json
```

Suggested content:

```json
{
  "world_seed": 12345,
  "terrain_version": "pr5_batch004_anti_radial_v1",
  "world_size_m": 97536.0,
  "primary_basin_count": 21,
  "secondary_branch_count": 118,
  "tertiary_gully_count": 402,
  "angular_gap_cv": 0.52,
  "angular_regular_score": 0.28,
  "outlet_spacing_cv": 0.61,
  "basin_width_cv_mean": 0.37,
  "ridge_rot_sym_score": 0.31,
  "gully_rot_sym_score": 0.29,
  "polar_peak_ratio": 0.26,
  "fan_blade_score": 0.22,
  "shoreline_error_max_m": 0.0,
  "beach_continuity_pct": 100.0,
  "ocean_below_sea_pct": 100.0,
  "square_edge_ocean_violations": 0
}
```

### Basin seed dump

Export:

```text
Saved/PR5Preview/drainage_network_seed_dump.json
```

Include every basin/branch seed value. This is critical for reproducing a bad seed without rerunning hidden randomness.

### Mask atlas

Export PNGs/EXRs:

```text
height_final.png
slope.png
basin_id.png
basin_dominance.png
primary_trunks.png
secondary_branches.png
tertiary_gullies.png
ridge_divides.png
gully_mask.png
ridge_mask.png
ridge_gully_competition.png
coast_lock_mask.png
coast_distance.png
outlet_fans.png
volcanic_process_features.png
polar_symmetry_debug.png
```

### CSV summaries

Export:

```text
basin_summary.csv
outlet_spacing.csv
branch_summary.csv
validation_failures.csv
```

`basin_summary.csv` should include:

```text
basin_id,type,dominance,maturity,outlet_angle_deg,outlet_arclength_m,head_angle_deg,
trunk_length_m,sinuosity,width_mean_m,width_max_m,width_argmax_t,secondary_count,
tertiary_count,incision_strength,fan_type,process_flags
```

### Visual failure thumbnails

When a seed fails anti-radial validation, export a failure thumbnail with overlaid red labels:

```text
FAILED: angular_gap_cv too low
FAILED: high rotational symmetry
FAILED: >50% straight trunks
FAILED: coast metric regression
```

This avoids wasting time arguing from subjective screenshots.

## Files And Areas Likely Touched

Actual filenames must be verified in the repo, but the implementation likely touches these areas/classes by responsibility.

### Terrain generation core

Likely affected responsibilities:

- island/volcano heightfield generation;
- ridge/gully mask generation;
- erosion/drainage contribution pass;
- crater/rim/lava/lahar/landslide terrain modifiers;
- coast blending and sea-level finalization.

Likely file/class search targets:

```text
VolcanicIslandTerrain
VolcanoTerrainGenerator
TerrainHeightfieldGenerator
IslandTerrainGenerator
PR5TerrainGenerator
RidgeGullyGenerator
DrainageGenerator
ErosionMaskGenerator
TerrainMaskAtlas
CoastlineGenerator
CoastalShelfGenerator
TerrainPreviewExporter
```

### New recommended classes

```cpp
class FVolcanicDrainageNetwork;
class FVolcanicBasinSeedGenerator;
class FVolcanicBasinRasterizer;
class FDrainageBranchGenerator;
class FRidgeDivideSolver;
class FVolcanicProcessOverprintSolver;
class FCoastInvariantTerrainFinalizer;
class FAntiRadialTerrainValidator;
class FDrainagePreviewDebugExporter;
```

### New or modified config structs

```cpp
struct FVolcanicDrainageConfig;
struct FVolcanicBasinSeed;
struct FDrainageBranchSeed;
struct FRidgeDivideSeed;
struct FCoastalOutletFanSeed;
struct FVolcanicProcessFeatureSeed;
struct FAntiRadialValidationConfig;
struct FCoastInvariantValidationConfig;
```

### Preview/exporter area

The preview exporter must add overlays/metrics for:

- basin ID;
- outlet spacing;
- trunk/branch paths;
- ridge divide graph;
- anti-radial metrics;
- coast preservation metrics;
- seed dump JSON.

### Test/validation area

Add deterministic tests:

```text
same seed -> same drainage network
same seed -> same metrics
Batch004 world size -> no old-scale constants
bad equal-angle synthetic network -> validation fails
known accepted coast -> remains accepted after interior rewrite
```

## Large PR Implementation Scope

This is a major PR. It should be staged internally even if delivered as one branch.

### Phase 1: Baseline and protection

- Capture current accepted coast metrics as test fixtures.
- Add coast invariant finalizer if not already isolated.
- Add anti-radial validator skeleton with metrics computed from existing preview.
- Confirm the current bad radial preview fails the new anti-radial gates.

Deliverable:

```text
coast metrics still pass
old radial masks fail anti-radial metrics
```

### Phase 2: Basin network seeding

- Implement deterministic outlet sampling from coastline arclength.
- Implement nonuniform spacing/gap rejection.
- Generate basin heads, dominance, maturity, process type, and curved trunk anchors.
- Export basin seed dump and basin ID preview.

Deliverable:

```text
network is deterministic
outlets uneven
trunks curved
basin count scale-aware
```

### Phase 3: Primary trunk rasterization

- Rasterize valley trunk incision and shoulders.
- Replace or bypass old radial gully core.
- Generate gully mask from trunk cores.
- Keep coast lock active.

Deliverable:

```text
primary valleys visible
no full equal spokes
coast unchanged
```

### Phase 4: Ridge divide competition

- Implement influence fields from basin centerlines.
- Solve/rasterize competitive divides.
- Normalize ridge/gully mask competition.
- Export ridge divide debug overlay.

Deliverable:

```text
ridge masks separate named basins
ridge/gully cores do not overlap
rotational symmetry reduced
```

### Phase 5: Secondary and tertiary branches

- Add controlled branch graph generation.
- Cull crossings and noise mush.
- Add branch incision/mask contribution.
- Export branch summary CSV.

Deliverable:

```text
hierarchical drainage exists
branches are not random scratches
flow debug agrees with visible trunks
```

### Phase 6: Volcanic process overprints

- Add crater breach overprint.
- Add lava flow armor/deflection masks.
- Add landslide scar cut/deposit morphology.
- Add lahar corridor valley-following overprint.

Deliverable:

```text
volcanic history breaks symmetry
process features are local and irregular
no repeated wedges
```

### Phase 7: Coastal fans and final validation

- Add coast-safe fan/apron treatment.
- Reassert coast invariants.
- Export final preview atlas and metrics JSON.
- Tune thresholds against before/after previews.

Deliverable:

```text
anti-radial gates pass
coast gates pass
large-scale island reads natural from mask atlas and shaded relief
```

## Acceptance Criteria

### Hard acceptance

```text
shoreline_error_max_m == 0 or effectively 0 in preview evidence
beach_continuity_pct == 100%
ocean_below_sea_pct == 100%
square_edge_ocean_violations == 0
outer world edge remains ocean/container terrain
island perimeter remains beach/coastal shelf/sea-level transition
```

```text
old equal-angle fan-blade visual is gone
ridge/gully masks no longer look like a polar plot
primary basin outlets are nonuniform
primary basin trunks are curved/varied
basin widths vary by basin and elevation
ridge divides are competitive/asymmetric
secondary/tertiary drainage is hierarchical, not mush
volcanic overprints are local/irregular, not repeated wedges
```

### Quantitative starting gates

These should be tuned after collecting current bad-preview and target-preview data, but they are suitable initial gates:

```text
angular_gap_cv >= 0.28
angular_regular_score <= 0.55
outlet_spacing_cv >= 0.25
mean_basin_width_cv >= 0.22
ridge_rot_sym_score <= 0.45
gully_rot_sym_score <= 0.42
combined_rot_sym_score <= 0.44
fan_blade_score < 0.60
straight_primary_trunk_fraction_sinuosity_lt_1_04 <= 0.50
branch_crossing_count == 0 except documented capture cases
gully_mask_coverage_land_pct between configured min/max
```

### Scale acceptance

```text
world_size_m approximately 97536 handled without old 16256 m assumptions
basin widths/lengths expressed in meters or normalized island units intentionally
preview exporter labels scale in km/m correctly
branch counts and widths remain plausible at 97.536 km scale
no heightfield pixel-size constants masquerade as meters
```

### Determinism acceptance

```text
same seed + same config + same terrain version -> identical basin seed dump
same seed + same config + same terrain version -> identical metrics
thread count/order does not change basin layout
Linux preview output remains stable across runs
```

## Risks And Stop Conditions

### Risk: noise hides but does not fix radial topology

Stop if masks still show regular spokes after adding noise. The topology must be changed, not merely perturbed.

### Risk: coast regressions

Stop if any of these regress:

```text
shoreline_error_max_m
beach_continuity_pct
ocean_below_sea_pct
square_edge_ocean_violations
```

Do not accept a tradeoff where interior looks better but coast breaks.

### Risk: branch mush

Stop if tertiary gullies create uniform scratch noise. Reduce branch count, increase spacing, strengthen hierarchy, or require branches to join named trunks.

### Risk: overfitted metrics

Metrics should catch visible failure, not become the only target. Keep atlas screenshots in review. If a preview passes metrics but looks like fan blades, adjust the metrics.

### Risk: too few basins at 97 km scale

A 97.536 km island with only a handful of basins may look empty and oversized. Increase basin count and branch hierarchy, but keep primary basins visually readable.

### Risk: too many equal-width basins

Even with uneven outlet angles, equal width curves can still read artificial. Validate width variance and maximum-width-position variance.

### Risk: process features become repeated decorations

Lava flows, lahars, and landslides must arise from seeded sources and local topography. Stop if they look like evenly placed stamps.

### Risk: hidden nondeterminism

Any nondeterministic layout step undermines reproducible preview validation. Stop and seed-dump before optimizing or threading.

## Rejected Or Forbidden Approaches

Forbidden:

- equal-angle sector catchments;
- fixed `N` spokes around the volcano;
- radial sine/cosine ridge masks as the primary gully/ridge model;
- uniform crater ring with identical outward gullies;
- global polar noise used as drainage identity;
- full-length straight gullies from summit to coast;
- identical valley width curves for every basin;
- branch recursion without graph limits;
- random scratch/gully noise with no trunk hierarchy;
- coastal fan edits that modify ocean/shelf/beach invariants;
- hiding coast breaks with final images while metrics fail;
- using old 16.256 km constants in the 97.536 km world;
- player-facing navigation/orientation UI recommendations.

Rejected fallback:

```text
if irregular seed fails, use equal sectors
```

Correct fallback:

```text
fail validation, export seed/debug dump, and retry deterministic seed substream with logged attempt index
```

Also rejected:

- “make it natural with more fBm” as the main solution;
- placing outlets by angle when coastline arclength is available;
- making every basin start at the exact volcano center;
- making every basin widest at the same normalized radial distance;
- using lava/lahar/landslide features as symmetrical wedge stamps;
- weakening coast acceptance to accommodate interior terrain.

## Sources And References

[1] O'Hara, D., Goren, L., van Wees, R. M. J., Campforts, B., Grosse, P., Lahitte, P., Kereszturi, G., and Kervyn, M. (2024). “Time-varying drainage basin development and erosion on volcanic edifices.” *Earth Surface Dynamics*, 12, 709-726. Discusses volcanic edifice drainage metrics, basin competition, divide migration, outlet spacing, and the evolution from more uniform young radial basins toward less uniform mature basin configurations. https://esurf.copernicus.org/articles/12/709/2024/

[2] GRASS GIS `r.watershed` manual. Documents generation of flow accumulation, drainage direction, streams, watershed basins, and related terrain derivatives from elevation data. Useful reference for developer validation outputs and hydrology-style debug passes. https://grass.osgeo.org/grass-stable/manuals/r.watershed.html

[3] TauDEM, “Terrain Analysis Using Digital Elevation Models.” Describes DEM tools for extracting and analyzing hydrologic information from topography, including watershed and drainage-network operations. Useful reference for validation vocabulary and DEM-style processing expectations. https://hydrology.usu.edu/taudem/taudem5/

[4] U.S. Geological Survey Volcano Hazards Program. “Lahars move rapidly down valleys like rivers of concrete.” Defines lahars as water/rock mixtures flowing down volcanic slopes into river valleys and notes entrainment, growth, rainfall triggering, and downstream deposition/damage. https://www.usgs.gov/programs/VHP/lahars-move-rapidly-down-valleys-rivers-concrete

[5] U.S. Geological Survey Volcano Hazards Program. “Landslides are common on tall, steep, and weak volcanic cones.” Explains volcanic landslide causes, hydrothermal weakening, debris avalanches, and transformation into lahars under suitable water/fine-material conditions. https://www.usgs.gov/programs/VHP/landslides-are-common-tall-steep-and-weak-volcanic-cones

[6] Saville, J. M., Hinton, E. M., and Huppert, H. E. (2022). “Predicting Safe Regions within Lava Flows over Topography.” Shows how topography affects lava thickness, acceleration, diversion, and ponding; useful for lava deflection/armor terrain logic. https://arxiv.org/abs/2207.05917

[7] Génevaux, J.-D., Galin, E., Guérin, E., Peytavie, A., and Beneš, B. (2013). “Terrain Generation Using Procedural Models Based on Hydrology.” ACM Transactions on Graphics / SIGGRAPH 2013. Presents hydrology-inspired procedural terrain modeling using drainage-network concepts. https://hal.science/hal-01339224/file/siggraph2013.pdf

[8] Lim, F. Y., Tan, Y. W., and Bhojan, A. (2022). “Visually Improved Erosion Algorithm for the Procedural Generation of Tile-based Terrain.” Discusses graph-based fluvial erosion, constraint maps, realistic erosion strength controls, and terrain-generator evaluation. https://arxiv.org/abs/2210.14496

[9] British Geological Survey. “Volcanic hazards.” General reference for volcanic hazards including lava flows, pyroclastic flows, lahars, landslides/debris avalanches, ash, gas, and tsunamis. Useful as a broad process checklist, not as implementation detail. https://www.bgs.ac.uk/discovering-geology/earth-hazards/volcanoes/volcanic-hazards/
