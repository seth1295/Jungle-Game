# Hydrology, Erosion, And Catchment Solver Research

## Executive Summary

Jungle Game should implement a **hybrid two-stage hydrology terrain system**:

1. **Deterministic drainage graph precompute**: seed irregular coastal outlets, let catchments compete inland, derive ridge divides from catchment boundaries, then build a tributary tree that follows least-cost downhill corridors rather than crater-centered spokes.
2. **Solver-inspired heightfield pass**: run bounded flow routing, accumulation, stream-power incision, hillslope diffusion, sediment transport, and fan deposition over the graph-conditioned heightfield. Use the same deterministic config and seed path for both 1024 px previews and 8129-class source evidence, with preview-safe approximations and full-grid verification.

This is the best fit because a true long-running landscape-evolution simulation would be slow, hard to art-direct, and easy to destabilize near the accepted coast, while pure noise/curve drawing would repeat the current failure mode. The hybrid method keeps the island source-owned and deterministic while producing drainage basins that read as geomorphic systems: unequal outlets, unequal basin areas, curved trunk valleys, tributary hierarchy, real divides, valley-floor deposition, and volcanic lahar corridors that are captured by drainage instead of drawn as radial splines.

The coast remains locked. Hydrology may cut interior relief and shape valleys, but it must never move the accepted land/sea mask, beach continuity, square-edge ocean container, or sea-level relationship.

Recommended headline decision:

> Use a **graph-first, field-verified hydrology/erosion solver**: construct catchments and channels explicitly, rasterize them into a conditioning field, then run conservative erosion/deposition passes with hydrology metrics as hard acceptance gates.

## Terrain-Only Scope Boundary

This document is terrain morphology and terrain validation only. It does **not** design foliage, ecology, weather, audio, NPCs, gameplay systems, final materials, buildings, quests, player UI, map, compass, minimap, GPS, objective markers, navigation overlays, or player-facing debug overlays.

The terrain output may create internal developer/debug maps such as catchment IDs, stream order, accumulation, erosion depth, and ridge divide masks, but these are terrain-authoring evidence maps only.

The hydrology rewrite must preserve the existing coastal acceptance gates:

```text
world_size_m: 97536.0 target
shoreline_error_max_m: 0 or effectively 0
beach_continuity_pct: 100%
ocean_below_sea_pct: 100%
square_edge_ocean_violations: 0
outer square world edge: ocean/container terrain
beach/coastal shelf transition remains continuous
```

## Current Drainage Failure

The current island can pass coast, beach, and shelf gates while still failing geomorphically inland. The visible failure is not height amplitude; it is **organization**:

- ridges and gullies radiate from the crater center as if generated in polar coordinates;
- outlets appear visually regular or disconnected from basin scale;
- valley spacing is too even;
- tributaries are either absent, decorative, or mushy noise;
- ridges appear drawn between radial grooves rather than formed by watershed competition;
- deposition does not collect at slope breaks or coastal fan/apron zones;
- volcanic corridors look like clean radial splines instead of paths captured, blocked, diverted, or inherited by drainage.

A real large volcanic island can have radial tendencies near the cone, but the mature terrain should also include piracy, asymmetric basins, older breached sectors, irregular ridge arms, compound tributary trees, coastal alluvial fans, and basins distorted by lithology and prior lava/lahar paths. The solver must therefore create **drainage competition**, not merely perturb radial spokes.

## Hydrology Solver Goals

The solver should generate and validate the following terrain features:

- **deterministic coastal outlets** selected from the coastline mask, not equal-angle points;
- **major catchment basins** with unequal area, relief, elongation, curvature, and outlet spacing;
- **ridge divides** derived from basin label boundaries and high-ground reinforcement;
- **valley heads** placed near concave hillslope thresholds, not random interior dots;
- **secondary and tertiary tributary trees** with Strahler/Horton-style hierarchy;
- **flow routing maps** that drain all land cells to valid coastal outlets or sea cells;
- **flow accumulation `A`** usable for incision strength and stream extraction;
- **stream-power incision** using `E = K A^m S^n` with safe clamps;
- **hillslope diffusion / creep** using `dh/dt = D laplacian(h)` or a stable approximation;
- **sediment capacity and deposition** for valley floors and coastal/alluvial fans;
- **lahar-compatible volcanic corridors** coupled to drainage and slope breaks;
- **anti-radial diagnostic metrics** that fail the terrain if the network still behaves like a polar plot.

The target is not a scientific hydrology simulator. The target is a source-owned terrain-generation system that borrows the right hydrology and geomorphology principles, is deterministic, and generates credible evidence maps.

## Flow Routing Options

### D8

**D8** routes each cell to the steepest lower neighbor among the 8 surrounding cells. It is simple, fast, deterministic, and gives a directed acyclic graph when depressions are handled. It is appropriate for:

- fast preview metrics;
- extracting a single trunk path from each cell to an outlet;
- building Strahler order on a tree-like network;
- producing crisp stream masks.

Implementation approximation:

```cpp
int BestDownslopeNeighborD8(Cell c)
{
    float bestSlope = 0.0f;
    int best = INDEX_NONE;

    for (Neighbor n : D8Neighbors(c))
    {
        float distance = IsDiagonal(n) ? CellSizeM * 1.41421356f : CellSizeM;
        float slope = (Height[c] - Height[n]) / distance;
        if (slope > bestSlope)
        {
            bestSlope = slope;
            best = n.Index;
        }
    }
    return best;
}
```

D8 alone can create grid-aligned artifacts and over-concentrated drainage. Use it for **network extraction**, not as the only visual shaper.

### D-infinity / D∞

**D-infinity** represents flow direction as the steepest downward direction over triangular facets around a cell, then splits flow between the two neighboring cells that bracket the continuous downslope angle. It reduces D8 stair-stepping and is better for realistic contributing area and flow vector diagnostics. Tarboton introduced the D∞ method to compute flow direction and upslope contributing area using a continuous angle over grid facets [Tarboton 1997].

Use D∞ for:

- final full-grid flow direction field;
- accumulation heatmaps;
- drainage direction entropy;
- radial alignment score;
- smooth hillslope-to-channel transition.

Preview approximation:

- compute finite-difference gradient `grad h = (dh/dx, dh/dy)`;
- flow vector `F = normalize(-grad h)`;
- split flow into the two D8 neighbors whose directions bracket `F`;
- if the cell is flat, use depression-filled height plus deterministic tiny epsilon gradient toward assigned outlet.

### Multiple-Flow Direction / MFD

**Multiple-flow direction** routes flow to all lower neighbors with weights usually based on slope. Freeman's MFD/FD8 family allows divergent flow, which is useful on broad slopes, fans, and convex hillsides [Freeman 1991].

Use MFD for:

- alluvial fan spreading near outlets;
- broad upper-slope sheetwash;
- sediment distribution where a single channel should not own all material;
- post-incision validation that fans are not needle-shaped.

Approximation:

```text
w_i = max(S_i, 0)^p / sum_j max(S_j, 0)^p
```

where `S_i` is slope to lower neighbor `i` and `p` is a convergence exponent. Higher `p` behaves more like D8; lower `p` spreads flow.

Recommended routing split:

| Use case | Routing method | Reason |
|---|---:|---|
| Preview stream tree | D8 | stable, cheap, topological order is easy |
| Final accumulation | D∞ | fewer grid artifacts, better direction metrics |
| Fan deposition | MFD | permits distributary spreading |
| Watershed label validation | D8 + D∞ cross-check | labels must agree at basin scale |

## Flow Accumulation And Stream Ordering

### Flow accumulation

Flow accumulation is the contributing area upstream of a cell. For Jungle Game, use **rainfall-weighted contributing area** even if rainfall is only a synthetic scalar field:

```text
A[c] = cell_area_m2 * R[c] + sum_upstream(A[u] * flow_weight(u -> c))
```

Where:

- `A[c]` is contributing area or effective discharge proxy;
- `R[c]` is deterministic rainfall/runoff weight, usually `0.75..1.25`, optionally lower on permeable fresh lava;
- flow weights come from D8, D∞, or MFD;
- cells are processed from high elevation to low elevation after depression handling.

The accumulation map should be one of the primary terrain evidence outputs.

### Stream threshold

A cell becomes a stream candidate when accumulation and slope exceed thresholds:

```text
stream_candidate = A >= A_min && S >= S_min
```

Recommended defaults:

```text
A_min_major_preview = 0.35% to 1.20% of island land area
A_min_minor_preview = 0.03% to 0.15% of island land area
S_min_channel = 0.008 to 0.025 depending on vertical exaggeration
```

The threshold should vary by terrain region. Wet windward sectors and weaker pyroclastic material can channelize earlier; resistant lava shields channelize later.

### Strahler / Horton-style stream order

Use Strahler order on the extracted D8 channel tree:

```text
leaf/headwater stream = order 1
if two upstream children of same order k meet: downstream order = k + 1
otherwise downstream order = max(upstream orders)
```

Use order for terrain shaping:

| Order | Terrain effect |
|---:|---|
| 1 | subtle swales, valley heads, shallow incision |
| 2 | visible tributary grooves, concave valley floors |
| 3 | persistent valleys, side-slope organization |
| 4+ | trunk valleys, major relief control, floodplain/fan coupling |

This prevents tributaries from becoming noise mush. Random noise can perturb headwater position, but stream order determines width, incision, and persistence.

### Hack's law sanity check

Hack's law relates main channel length `L` to basin area `A` approximately as:

```text
L = c A^h
```

with `h` commonly around `0.55..0.60` in many basins, though it varies by region and scale [Hack 1957; Rigon et al. 1996; Sassolas-Serrayet et al. 2018]. For Jungle Game, use Hack's law as a **diagnostic and generator constraint**, not a hard scientific claim.

Per basin:

```text
hack_residual = log(L_main) - (log(c) + h * log(A_basin))
```

Fail only extreme residuals:

```text
abs(zscore(hack_residual)) <= 2.5 for at least 85% of mature basins
```

Basins that violate this can still be accepted if tagged as volcanic collapse, young lava obstruction, breached crater capture, or coastal micro-basin.

## Watershed / Catchment Segmentation

### Recommendation

Generate catchments by **outlet-seeded weighted competition**, then verify them with flow routing. Do not draw divides directly.

Pipeline:

1. Extract coastline cells from the accepted land/sea mask.
2. Select irregular outlet candidates along concave coastal/shelf breaks.
3. Assign each outlet a basin weight and capture bias.
4. Run inland weighted fast marching / Dijkstra growth over land cells.
5. Label every land cell with the winning outlet ID.
6. Extract watershed boundaries where labels meet.
7. Reinforce ridge height along those boundaries.
8. Run flow routing and accumulation over the conditioned heightfield.
9. Relabel any cells whose actual flow drains to a different outlet; either accept minor piracy or rerun competition locally.

### Outlet-seeded competition cost

Each cell should be assigned to the outlet that minimizes total travel cost:

```text
cost_to_outlet = distance_cost
               + climb_cost
               + cross_slope_cost
               + lithology_resistance_cost
               + curvature_cost
               + deterministic_noise_cost
               - inherited_channel_bonus
```

Suggested local transition cost from cell `a` to neighbor `b`:

```text
C(a,b,o) = d(a,b)
         * BasinScale[o]
         * (1 + W_elev * max(BaseHeight[b] - BaseHeight[a], 0))
         * (1 + W_rough * Roughness[b])
         * (1 + W_cross * abs(dot(step_dir, local_contour_dir)))
         * (1 + W_litho * Resistance[b])
         + NoiseCost(seed, a, b, o)
```

Where `o` is outlet ID. This makes basin shape emerge from competition over terrain resistance, not from angular sectors.

### Basin weights

Give each outlet a deterministic target scale:

```text
BasinScale[o] = lognormal(mean=1.0, sigma=0.35..0.75)
```

Then constrain the final basin-area distribution:

```text
catchment_area_cv >= 0.45
catchment_area_gini between 0.22 and 0.58
largest_basin_area <= 0.22 * island_land_area unless explicitly configured
smallest_major_basin_area >= 0.015 * island_land_area
```

This allows real irregularity without one outlet eating the entire island.

### Catchment piracy

After initial labels, route actual flow. If a high-accumulation stream crosses into another basin, classify it:

- **minor piracy**: acceptable; creates natural asymmetry;
- **major piracy**: update basin graph and metrics;
- **invalid leak**: if it breaks sea/coast gates or drains into a pit, stop.

A little piracy is useful. It is one of the easiest ways to break radial symmetry.

## Ridge Divide Extraction

Ridges should be derived from watershed labels, not drawn as radial ridgelines.

### Boundary extraction

For each land cell:

```text
ridge_boundary[c] = any_neighbor_has_different_catchment_id(c)
```

Then compute a ridge strength:

```text
ridge_strength = label_contrast
               * local_relief
               * divide_persistence
               * distance_from_outlet_weight
               * non_beach_mask
```

Where:

- `label_contrast` is high where two or more mature catchments meet;
- `local_relief` prevents ridges in low flat fan/beach zones;
- `divide_persistence` is high where the boundary survives smoothing/scale changes;
- `distance_from_outlet_weight` suppresses false divides near coastal outlet fans;
- `non_beach_mask` hard-protects coastal acceptance.

### Ridge reinforcement

Apply broad, asymmetric divide uplift before incision:

```text
h += ridge_height_m * smoothstep(0, ridge_width_m, distance_to_boundary)
```

But do not create razor walls. A ridge is a high drainage divide, not a fence. Use anisotropic blur along the divide and lower the ridge where stream capture or saddles are desired.

Recommended parameters:

```text
major_divide_width_m = 450..1800
minor_divide_width_m = 150..600
ridge_height_m = 15..180 depending on basin relief tier
saddle_probability_per_major_divide = 0.15..0.45
```

### Ridge validation

A divide is accepted only if it actually separates flow:

```text
for ridge sample cells:
    left_probe drains to basin A
    right_probe drains to basin B
    ridge cell is higher than both adjacent valley probes by margin
```

Metrics:

```text
ridge_boundary_flow_agreement_pct >= 92%
ridge_saddle_leak_count_major == configured piracy count only
mean_divide_relief_m >= basin_order_scaled_min
```

## Valley And Tributary Graph Generation

### Graph-owned drainage objects

Use explicit graph objects, then rasterize them into heightfield masks.

```cpp
struct FTerrainOutlet
{
    int32 OutletId;
    FIntPoint CoastCell;
    FVector2D WorldXY;
    float CoastArcLengthM;
    float MouthWidthM;
    float TargetBasinAreaM2;
    float OutletElevationM;      // locked near sea/beach transition
    float FanRadiusM;
    uint32 Seed;
};

struct FCatchmentBasin
{
    int32 BasinId;
    int32 OutletId;
    TArray<int32> NeighborBasinIds;
    float AreaM2;
    float ReliefM;
    float MainStemLengthM;
    float HackResidual;
    float Elongation;
    float MeanSlope;
    float DrainageDensity;
    FBox2D Bounds;
    uint32 Seed;
};

struct FDrainageNode
{
    int32 NodeId;
    int32 BasinId;
    FVector2D WorldXY;
    FIntPoint Cell;
    float ElevationM;
    float AccumulationM2;
    int32 StreamOrder;
    bool bIsOutlet;
    bool bIsConfluence;
    bool bIsHeadwater;
};

struct FDrainageEdge
{
    int32 EdgeId;
    int32 BasinId;
    int32 UpstreamNodeId;
    int32 DownstreamNodeId;
    TArray<FVector2D> Polyline;
    float LengthM;
    float MeanSlope;
    float Sinuosity;
    float IncisionScale;
    float ValleyWidthM;
    int32 StreamOrder;
    bool bLaharCompatible;
};

struct FErosionPassConfig
{
    int32 Iterations;
    float CellSizeM;
    float StreamPowerK;
    float StreamPowerM;
    float StreamPowerN;
    float MaxIncisionPerPassM;
    float DiffusionD;
    float DiffusionDt;
    float SedimentCapacityK;
    float DepositionRate;
    float BeachLockStartM;
    float BeachLockEndM;
    float SeaLevelM;
    uint32 Seed;
};

struct FHydrologyMetrics
{
    int32 OutletCount;
    float OutletSpacingCv;
    float CatchmentAreaCv;
    float CatchmentAreaGini;
    float DrainageDirectionEntropyBits;
    float RadialAlignmentScore;
    float StreamCurvatureMedian;
    float HackResidualRms;
    float ClosedPitCount;
    float DrainageToValidOutletPct;
    float RidgeBoundaryAgreementPct;
    float CoastMaskChangedPct;
    float BeachContinuityPct;
    int32 SquareEdgeOceanViolations;
};
```

### Tributary generation strategy

1. For each basin, trace a main stem from outlet inland along the lowest-cost path toward a high-relief source zone.
2. Estimate target main stem length from basin area using Hack's law with deterministic noise.
3. Place confluences along the main stem using nonuniform spacing: closer downstream for larger tributaries, finer branching upstream.
4. Grow tributaries from side basins/headwater zones using least-cost descent to the confluence.
5. Reject tributaries that are too straight, too parallel to radial vectors, or cross ridges without a saddle.
6. Rasterize accepted edges into valley masks with width/order/incision falloff.

### Branching rules that avoid noise mush

Do not stamp random grooves. Each tributary must satisfy:

```text
length_m >= min_length_by_order
sinuosity between configured min/max
confluence angle between 25 and 80 degrees for most tributaries
upstream area above threshold
endpoint near concave / high-slope source zone
no unresolved ridge crossing
```

For headwater density:

```text
headwater_density = f(mean_slope, rainfall_weight, lithology_erodibility, vegetation_placeholder_ignored)
```

Even though foliage/ecology is out of scope, a synthetic `rainfall_weight` and `lithology_erodibility` field can provide physical heterogeneity.

## Stream-Power Incision Model

Use stream-power incision as the main fluvial cutting term:

```text
E = K A^m S^n
```

Where:

- `E` is vertical incision rate/depth per pass;
- `K` is erodibility and timestep scaling;
- `A` is contributing area / discharge proxy;
- `S = |grad h|` is slope or channel gradient;
- `m` controls discharge sensitivity;
- `n` controls slope sensitivity.

The stream-power incision model is a standard abstraction in landscape-evolution modeling; Whipple and Tucker reviewed its dynamics and implications, and later reviews and model frameworks continue to use variants of the same area-slope law [Whipple & Tucker 1999; Tucker & Hancock 2010; Lague 2014; Kwang & Parker 2017].

### Practical Jungle Game formulation

Use normalized area to keep values stable across preview/full grids:

```text
A_norm = saturate((A - A_min) / (A_ref - A_min))
S_norm = saturate(S / S_ref)
E_m = K_local * pow(A_norm, m) * pow(S_norm, n) * ChannelMask * CoastErosionMask
```

Recommended default shape:

```text
m = 0.45..0.55
n = 1.0..1.35
K_local = base_K * lithology_erodibility * rainfall_weight
MaxIncisionPerPassM = 0.25..3.0 preview, 0.05..1.0 full-grid iterative
```

Apply incision as a valley cross-section, not a single-pixel trench:

```text
valley_profile(d) = exp(-d^2 / (2 * sigma_order^2))
incision[d] = E_m * valley_profile(d)
```

Order-scaled widths:

```text
order_1_sigma = 10..35 m
order_2_sigma = 25..80 m
order_3_sigma = 60..180 m
order_4_plus_sigma = 150..600 m
```

At 1024 px preview resolution, widths below one pixel should be accumulated into a subpixel coverage mask rather than discarded.

### Slope calculation

Use central differences for field maps:

```text
dh_dx = (h[x+1,y] - h[x-1,y]) / (2 * cell_size)
dh_dy = (h[x,y+1] - h[x,y-1]) / (2 * cell_size)
S = sqrt(dh_dx^2 + dh_dy^2)
```

Use along-edge channel slope for graph incision:

```text
S_edge = max((h_upstream - h_downstream) / edge_length_m, S_min)
```

Do not allow negative incision from inverted slopes. If a stream edge points uphill after height conditioning, either carve a monotonic bed profile or fail the pass.

### Monotonic channel bed enforcement

For every accepted drainage edge, enforce a downstream non-increasing bed profile with small deterministic steps:

```text
h_bed[i+1] <= h_bed[i] - min_drop_per_meter * segment_length
```

This is not a visual trench by itself. It is a hydrologic correction pass to keep the network drainable before naturalized smoothing.

## Hillslope Diffusion And Slope Failure Relaxation

Use diffusion/creep to smooth hillslopes and prevent the terrain from becoming a sawtooth drainage graph.

Classic linear hillslope diffusion:

```text
dh/dt = D laplacian(h)
```

Discrete 4-neighbor approximation:

```text
laplacian = (h[x+1,y] + h[x-1,y] + h[x,y+1] + h[x,y-1] - 4h[x,y]) / cell_size^2
h_next = h + dt * D * laplacian
```

For explicit stability in 2D, use a conservative CFL-like guard:

```text
D * dt / cell_size^2 <= 0.20
```

### Where to apply diffusion

Diffusion should be strongest on:

- oversteepened side slopes after incision;
- ridge shoulders;
- old pyroclastic aprons;
- shallow landslide/slope-failure masks.

Diffusion should be weaker on:

- locked beaches;
- main channel beds;
- hard lava ridges;
- coastal cliff detail if already accepted.

Use a mask:

```text
D_local = D_base
        * HillslopeMask
        * (1 - ChannelBedMask * 0.70)
        * (1 - BeachLockMask)
        * LithologyDiffusionFactor
```

### Slope failure relaxation

For slopes above an angle of repose threshold, apply talus-style redistribution:

```text
if slope_to_neighbor > S_crit:
    move = failure_rate * (slope_to_neighbor - S_crit) * cell_size
    h[current] -= move
    h[neighbor] += move * sediment_retention
```

Recommended:

```text
S_crit_soil = tan(32 deg)
S_crit_lava = tan(38..45 deg)
S_crit_wet_pyroclastic = tan(25..30 deg)
```

This gives slope processes without needing a full landslide system.

## Sediment Transport And Fan Deposition

The terrain should not only cut channels; it should move material downhill and deposit it where transport capacity falls.

### Sediment capacity approximation

Use a capacity law:

```text
Q = R * A
Capacity = Kc * Q^a * S^b
```

Where:

```text
a = 0.8..1.2
b = 1.0..2.0
```

Track sediment load `L` along flow order:

```text
if L < Capacity:
    erosion = min(Capacity - L, erodible_material, incision_cap)
    L += erosion * entrainment_fraction
else:
    deposition = deposition_rate * (L - Capacity)
    h += deposition
    L -= deposition
```

For a simpler implementation, derive deposition from slope breaks:

```text
deposition_potential = AccumulationNorm
                     * max(0, S_upstream - S_current)
                     * ValleyFloorMask
                     * SedimentSupply
```

### Valley-floor deposition

Where slopes flatten downstream, reduce incision and add low-amplitude deposition:

```text
valley_floor_fill = deposition_potential * smooth_valley_floor_mask
```

This helps avoid endless V-shaped cuts. Larger order streams should have wider valley floors with less noisy channel beds.

### Alluvial/coastal fan deposition near outlets

Fans should emerge near slope breaks and outlet mouths, not as perfect cones.

Fan trigger:

```text
bIsFanZone = distance_to_outlet < FanRadius
          && elevation < FanMaxElevation
          && slope decreases downstream
          && catchment_accumulation high
          && BeachLockMask not fully active
```

Fan radius:

```text
FanRadiusM = clamp(FanK * sqrt(BasinAreaM2), 250m, 3500m)
```

Fan deposition kernel:

```text
fan_weight = sector_mask_from_valley_direction
           * exp(-radial_distance / fan_decay)
           * distributary_noise
           * MFD_spread_weight
           * CoastPreservationMask
```

Important: the fan may raise interior lowland/floodplain terrain, but it must not move the shoreline mask. Deposition inside the beach lock zone is allowed only as a visual height blend that preserves `ocean_below_sea_pct` and beach continuity.

### Sediment evidence maps

Output:

```text
ErosionDepthMap
DepositionDepthMap
NetElevationDeltaMap
SedimentLoadMap optional
FanMaskMap
ValleyFloorMask
```

A healthy island should show:

- incision concentrated in ordered drainage;
- deposition in lower-gradient trunk valleys and outlet fans;
- minimal or zero net change inside hard-locked coast cells;
- no checkerboard erosion artifacts.

## Volcanic Lahar And Lava Corridor Coupling

Volcanic corridors must use drainage, but not become clean radial splines.

### Lahar corridors

Lahars are flow-like mass movement paths. For terrain morphology, route them with a modified hydrology cost:

```text
lahar_cost = distance
           + W_slope_low * penalty_for_too_flat
           + W_ridge * ridge_crossing_penalty
           + W_channel * negative_bonus_for_existing_valley
           + W_roughness * roughness_penalty
           + W_bend * bend_penalty_soft
```

Start lahars from configured volcanic source zones, then let them be captured by existing valleys. If the cheapest path is purely radial but nearby drainage offers a lower or comparable cost, snap/capture the corridor into the drainage network.

Rules:

```text
- lahar can follow trunk or tributary valleys;
- lahar can overtop a low saddle only if energy budget allows;
- lahar should widen/fill valley floor more than it incises bedrock;
- lahar deposits lobes at slope breaks and fan toes;
- lahar corridor must share outlet/fan logic where it reaches coast.
```

### Lava corridors

Lava is not water. It may build levees, block valleys, create perched ridges, and force later drainage diversion. Use it as a terrain-history modifier:

1. Generate older lava/pyroclastic resistance fields before catchment competition.
2. Let resistant lava ridges deflect drainage.
3. Let weak ash/pyroclastic sectors incise more deeply.
4. Route younger lava as broad lobes that may partially fill existing valleys.
5. Re-run local hydrology so streams divert around or cut through deposits.

This produces believable volcanic inheritance without clean spokes.

### Coupling maps

Output:

```text
VolcanicSourceMask
LaharCorridorMask
LavaResistanceMap
PyroclasticErodibilityMap
DrainageCaptureByLaharMap
HydrologyDiversionMap
```

Validation:

```text
lahar_drainage_overlap_major_pct >= 55%
lahar_radial_alignment_score <= stream_radial_alignment_score + 0.08
lava_diversion_events >= configured_min if young lava fields enabled
```

## Coast Preservation And Sea-Level Locking

The coast is already accepted. Hydrology must be subordinate to coastal gates.

### Immutable masks

Before hydrology runs, cache:

```text
OriginalLandSeaMask
OriginalShorelineCells
OriginalBeachBandMask
OriginalShelfTransitionMask
OriginalSquareEdgeOceanMask
SeaLevelM
```

Hydrology may read these masks but must not rewrite them.

### Coast erosion mask

Use a smooth inland ramp:

```text
CoastErosionMask = smoothstep(BeachLockEndM, BeachLockStartM, distance_from_shoreline)
```

Interpretation:

```text
0.0 = no hydrology height edits
1.0 = full hydrology height edits
```

Recommended:

```text
BeachLockStartM = 0..120m from accepted shoreline
BeachLockEndM = 450..1500m inland depending on shelf/beach width
```

Inside the hard lock:

```text
h_final = h_coast_accepted
land_sea_mask_final = land_sea_mask_original
```

Inside the soft transition:

```text
h_final = lerp(h_coast_accepted, h_hydrology, CoastErosionMask)
```

### Outlet mouth handling

Outlets need cuts through coastal terrain, but they cannot break the beach gate. Treat outlet mouths as **pre-approved notches inside the existing land mask**, not shoreline edits.

Rules:

- outlet cell must be on or immediately inside accepted coastline;
- outlet bed elevation approaches sea level smoothly;
- fan and mouth widening may modify inland beach-adjacent terrain only under the coast mask;
- no ocean cell may be raised above sea level;
- no land cell may be converted to ocean;
- square world edge remains ocean/container terrain.

### Coast validation after every pass

Hydrology pass is invalid if:

```text
shoreline_error_max_m > 0 or effectively 0 tolerance
beach_continuity_pct < 100%
ocean_below_sea_pct < 100%
square_edge_ocean_violations > 0
outer_edge_mask_changed == true
```

Coast checks run after every terrain write, not only at final export.

## Deterministic Implementation Strategy

### Recommended pipeline

```text
Input: accepted coast terrain + base island relief + deterministic seed/config

1. Build immutable coast/sea/beach/shelf masks.
2. Generate lithology, erodibility, rainfall/runoff, and roughness fields.
3. Select irregular coastal outlets from coastline candidates.
4. Run outlet-seeded catchment competition over land cells.
5. Extract ridge boundaries and reinforce broad divides.
6. Generate major drainage graph per catchment.
7. Generate secondary/tertiary tributaries with stream-order constraints.
8. Rasterize drainage graph into valley/channel masks.
9. Enforce monotonic channel beds and pit/depression handling.
10. Run flow routing and accumulation over conditioned terrain.
11. Apply stream-power incision with order-scaled valley profiles.
12. Apply hillslope diffusion and slope failure relaxation.
13. Apply sediment capacity/deposition and fan spreading.
14. Re-run routing/accumulation and hydrology metrics.
15. Apply coast-preservation blend from original coast terrain.
16. Export heightfield, maps, config fingerprint, and metrics.
```

### Pit filling and depression handling

Hydrologic analysis needs every non-endorheic land cell to drain. Real closed basins can exist, but this island generator should avoid accidental digital pits unless deliberately configured.

Use a Priority-Flood-style fill/carve pass. Priority-Flood floods inward from boundary/outlet cells using a priority queue and can fill depressions and label watersheds efficiently [Barnes, Lehman & Mulla 2014].

Recommended behavior:

```text
- seed priority queue with sea-adjacent outlet cells and valid coast drainage boundary cells;
- hard-exclude ocean cells from land filling;
- fill tiny numeric pits by epsilon lift;
- carve narrow escape only if fill would create an ugly lake/plateau;
- record every fill/carve delta in DepressionFixMap;
- fail if large unconfigured closed depression remains.
```

Depression policy:

| Depression type | Handling |
|---|---|
| one-cell numeric pit | epsilon fill |
| small flat artifact | fill + tiny deterministic gradient |
| valley blocked by fan/lava | carve spillway or accept configured diversion |
| crater lake | out of scope unless terrain spec later allows lakes |
| beach/ocean pit issue | fail; do not patch by moving coast |

### Preview grid vs full source grid

The preview grid and source grid must use the same conceptual pipeline but not identical iteration counts.

| Stage | 1024 px preview | 8129-class source evidence |
|---|---|---|
| Outlet selection | full fidelity | full fidelity |
| Catchment competition | full fidelity on preview grid | full fidelity on source grid/tiled |
| Tributary graph | full graph, simplified raster width | full graph, full raster width |
| Flow routing | D8 + approximate D∞ | D∞ + D8 stream extraction |
| Accumulation | normalized area | physical cell-area accumulation |
| Incision | 1-8 bounded passes | 10-80 bounded passes/tiled |
| Diffusion | separable blur / few explicit passes | stable explicit or implicit tiled pass |
| Sediment | slope-break approximation | capacity/load pass |
| Metrics | all anti-radial metrics | all final acceptance metrics |

### Config fingerprints

Every generated evidence output should include a config fingerprint:

```text
HydrologyConfigHash = Hash(
    WorldSizeM,
    CellSizeM,
    SeaLevelM,
    LandSeaMaskHash,
    CoastMaskHash,
    Seed,
    OutletConfig,
    CatchmentConfig,
    RoutingConfig,
    ErosionConfig,
    DiffusionConfig,
    SedimentConfig,
    VersionString
)
```

Write this to:

```text
HydrologyMetrics.json
HydrologyDebugManifest.json
heightfield metadata
preview screenshot metadata if available
```

No nondeterministic thread ordering should affect output. For parallel accumulation, use deterministic tile reductions or fixed-order integer/fixed-point accumulation where necessary.

## Preview / Debug Maps

Generate these maps for every accepted terrain preview and source evidence run:

```text
FlowDirectionMap
FlowAccumulationMap
CatchmentIdMap
StreamOrderMap
RidgeDivideMap
ValleyFloorMap
OutletFanMap
ErosionDepthMap
DepositionDepthMap
DepressionFixMap
RadialAlignmentHeatMap
DrainageDirectionEntropyMap
CoastLockMask
HydrologyEditMask
```

### Map expectations

| Map | Expected visual result |
|---|---|
| FlowDirectionMap | vectors curve through basins, not equal spokes from crater center |
| FlowAccumulationMap | few large trunk systems plus many uneven tributaries |
| CatchmentIdMap | irregular basin polygons with variable area/elongation |
| StreamOrderMap | branching hierarchy, no random uniform hairlines |
| RidgeDivideMap | boundaries between basins, not radial line art |
| ValleyFloorMap | broader downstream valleys and flatter fan approaches |
| OutletFanMap | fans at selected outlets only, sized by basin area and slope break |
| ErosionDepthMap | incision follows streams and upper gullies, not all slopes equally |
| DepositionDepthMap | lower valleys, fans, and slope breaks accumulate material |
| DepressionFixMap | sparse; large fills are reviewed/failures |
| RadialAlignmentHeatMap | hot radial segments exist locally but not globally dominant |
| CoastLockMask | confirms beach/shoreline preservation |

### Developer preview overlays

Internal debug overlays can include:

```text
- outlet IDs and basin area labels;
- stream order colors;
- confluence dots;
- ridge divide boundaries;
- invalid pit markers;
- coast lock boundaries;
- anti-radial metric summary.
```

These are developer diagnostics only and should not become player-facing navigation systems.

## Validation Metrics

Validation must prove three things:

1. coast gates remain intact;
2. drainage is hydrologically coherent;
3. drainage is not a radial/procedural spoke pattern.

### Coast preservation metrics

Hard gates:

```text
shoreline_error_max_m == 0 or effectively 0
beach_continuity_pct == 100%
ocean_below_sea_pct == 100%
square_edge_ocean_violations == 0
land_sea_mask_changed_pct == 0
coast_lock_height_delta_max_m <= configured epsilon inside hard lock
```

### Hydrology correctness metrics

```text
drainage_to_valid_outlet_pct >= 99.5%
closed_pit_count == 0 unless configured
major_stream_uphill_segment_count == 0
flow_accumulation_nan_count == 0
catchment_label_unassigned_count == 0
ridge_boundary_flow_agreement_pct >= 92%
stream_order_max >= 4 for a 97.536 km island target, unless outlet count very high
```

### Anti-radial metrics

#### Outlet spacing variance

Compute coastline arc-distance gaps between consecutive outlets:

```text
OutletSpacingCv = std(gaps) / mean(gaps)
```

Targets:

```text
OutletSpacingCv >= 0.35
no more than 35% of gaps within +/-10% of mean gap
largest_gap / smallest_gap >= 2.0 for mature outlet sets
```

#### Catchment area variance

```text
CatchmentAreaCv = std(area) / mean(area)
CatchmentAreaGini = gini(area)
```

Targets:

```text
CatchmentAreaCv >= 0.45
CatchmentAreaGini = 0.22..0.58
largest/smallest major basin area ratio >= 3.0
```

#### Drainage direction entropy

Bin flow directions into 16 angle bins, weighted by accumulation:

```text
P_i = weighted count in bin i / total weighted count
Entropy = -sum(P_i * log2(P_i))
```

Targets:

```text
DrainageDirectionEntropyBits >= 3.15 for 16 bins on stream cells
no single direction bin > 22% of weighted stream flow
```

#### Radial alignment score

For each stream cell, compare local flow direction with radial vector from crater/volcanic center:

```text
radial_alignment = abs(dot(flow_dir, normalize(cell_xy - crater_center_xy)))
RadialAlignmentScore = weighted_mean(radial_alignment, weight = sqrt(A))
```

Targets:

```text
RadialAlignmentScore <= 0.72 overall
RadialAlignmentScore <= 0.66 outside inner volcanic cone radius
no more than 25% of major stream length has radial_alignment > 0.90 outside cone
```

This allows real volcano-adjacent radial drainage while rejecting island-wide spokes.

#### Stream curvature score

For each stream edge:

```text
Sinuosity = polyline_length / straight_line_distance
```

Targets:

```text
median major_stream_sinuosity >= 1.08
median all_stream_sinuosity >= 1.12
at least 40% of order >= 3 streams have sinuosity >= 1.15
reject if too many streams are ruler-straight radial splines
```

#### Basin boundary non-radiality

Sample ridge divide segments and compare their tangent direction to radial/tangential directions from crater center. Reject if too many divide segments align as equal-angle sector boundaries.

Targets:

```text
ridge_radial_sector_score <= 0.60
angular_power_spectrum_equal_spacing_peak below configured threshold
```

### Fan/deposition metrics

```text
major_outlet_fan_presence_pct >= 60% where slope break exists
fan_area_correlates_with_basin_area_r >= 0.45
fan_deposition_inside_coast_hard_lock_pct == 0
net_deposition_in_lower_valleys > net_deposition_on_ridges
```

## Files And Areas Likely Touched

Exact file names should be resolved from the current repo, but the implementation will likely touch these areas:

```text
Source/JungleGame/Terrain/Generation/
Source/JungleGame/Terrain/Hydrology/
Source/JungleGame/Terrain/Erosion/
Source/JungleGame/Terrain/Validation/
Source/JungleGame/Terrain/Preview/
Source/JungleGame/Terrain/DebugMaps/
Config/TerrainGeneration/*.json or *.ini
Tests/Terrain/Hydrology/*
Tests/Terrain/Validation/*
Saved/PreviewEvidence/Hydrology/*
Docs/Terrain/*
```

Likely source-owned modules/classes:

```text
TerrainHydrologyTypes.h
TerrainOutletSeeder.cpp
TerrainCatchmentSolver.cpp
TerrainFlowRouter.cpp
TerrainAccumulation.cpp
TerrainStreamOrder.cpp
TerrainRidgeExtractor.cpp
TerrainErosionSolver.cpp
TerrainSedimentSolver.cpp
TerrainFanDepositor.cpp
TerrainHydrologyMetrics.cpp
TerrainDebugMapWriter.cpp
TerrainCoastPreservation.cpp
```

Do not import a black-box terrain generator. External libraries and papers can guide algorithms, but Jungle Game should own the generation logic, config, deterministic seeds, map outputs, and validation gates.

## Large PR Implementation Scope

A single large PR is possible if the goal is to land a complete terrain evidence feature, but it should be internally staged and reviewable. Recommended PR scope:

### Stage 1 — Types, config, and immutable masks

Deliver:

```text
FTerrainOutlet
FCatchmentBasin
FDrainageNode
FDrainageEdge
FErosionPassConfig
FHydrologyMetrics
coast/sea/beach/shelf lock masks
config fingerprinting
```

Acceptance:

```text
existing coast gates unchanged
config hash reproducible
no terrain visual rewrite yet except debug mask export
```

### Stage 2 — Outlet seeding and catchment competition

Deliver:

```text
irregular outlet selection
weighted basin competition
catchment ID map
basin area metrics
outlet spacing metrics
```

Acceptance:

```text
outlet spacing variance passes
catchment area variance passes
all land cells assigned
coast unchanged
```

### Stage 3 — Ridge extraction and drainage graph

Deliver:

```text
ridge divide map
main stem tracing
tributary graph
stream order map
valley rasterization masks
```

Acceptance:

```text
ridge boundaries separate basins
stream hierarchy visible
stream curvature/radial metrics improve
```

### Stage 4 — Flow routing, accumulation, and pit handling

Deliver:

```text
D8 preview router
D∞/MFD final router or approximation layer
Priority-Flood-style depression handling
accumulation map
valid outlet drainage metrics
```

Acceptance:

```text
drainage_to_valid_outlet_pct >= 99.5%
closed_pit_count == 0 unless configured
no major stream uphill segments
```

### Stage 5 — Erosion, diffusion, sediment, fans

Deliver:

```text
stream-power incision
hillslope diffusion
slope failure relaxation
sediment capacity/deposition
outlet fan deposition
hydrology edit mask
```

Acceptance:

```text
visible valleys and fans
no beach/coast regressions
erosion/deposition maps coherent
```

### Stage 6 — Evidence and gates

Deliver:

```text
all debug maps
HydrologyMetrics.json
preview/source comparison
anti-radial gate
coast preservation gate after hydrology
```

Acceptance:

```text
coast gates pass
hydrology correctness gates pass
anti-radial gates pass
source evidence reproducible from seed/config
```

## Acceptance Criteria

The hydrology/erosion solver is accepted when all of the following are true:

### Coast

```text
world_size_m remains 97536.0 target
shoreline_error_max_m == 0 or effectively 0
beach_continuity_pct == 100%
ocean_below_sea_pct == 100%
square_edge_ocean_violations == 0
land/sea mask unchanged by hydrology
beach/coastal shelf transition remains continuous
```

### Drainage morphology

```text
major catchments are irregular and unequal
ridge divides come from catchment boundaries
major streams drain to deterministic coastal outlets
tributaries branch with order, not random scratch noise
valley floors broaden downstream
coastal/alluvial fans appear where basin size and slope break justify them
lahar corridors couple to drainage and do not look like clean radial splines
```

### Hydrology maps

Must export:

```text
FlowDirectionMap
FlowAccumulationMap
CatchmentIdMap
StreamOrderMap
RidgeDivideMap
ValleyFloorMap
OutletFanMap
ErosionDepthMap
DepositionDepthMap
DepressionFixMap
CoastLockMask
HydrologyEditMask
```

### Numeric gates

```text
drainage_to_valid_outlet_pct >= 99.5%
closed_pit_count == 0 unless configured
major_stream_uphill_segment_count == 0
outlet_spacing_cv >= 0.35
catchment_area_cv >= 0.45
catchment_area_gini between 0.22 and 0.58
drainage_direction_entropy_bits >= 3.15
radial_alignment_score <= 0.72 overall
radial_alignment_score <= 0.66 outside inner cone
median_major_stream_sinuosity >= 1.08
ridge_boundary_flow_agreement_pct >= 92%
```

### Determinism

```text
same seed + same config + same input masks = same heightfield hash
same run exports same HydrologyMetrics.json
parallel/tiled source generation does not alter accepted output
config fingerprint included in all evidence artifacts
```

## Risks And Stop Conditions

Stop implementation or reject output if any of these occur:

### Coast risk

```text
shoreline mask changes
beach continuity drops below 100%
ocean cells rise above sea level
square edge ocean/container terrain is modified
outlet carving creates coast gate regressions
```

### Hydrology risk

```text
large unconfigured closed pits remain
streams flow uphill after correction
catchments are mostly equal angular sectors
major trunk streams are too straight/radial
ridge divides do not match watershed boundaries
tributaries look like random scratch noise
fan deposition crosses coast hard lock
```

### Performance risk

```text
1024 preview exceeds interactive budget
8129-class source pass cannot complete deterministically/tiled
memory spikes from storing too many full-resolution float maps at once
accumulation differs between thread counts
```

Mitigation:

- use tiled full-grid processing;
- store some maps as `uint16`/`uint32`/half-float where valid;
- keep graph in vector form and rasterize maps on demand;
- run expensive D∞/MFD only for final evidence, not every editor tweak;
- use fixed-order reductions for deterministic accumulation.

## Rejected Or Forbidden Approaches

Do not implement any of the following:

- equal angular outlet spacing around the island;
- crater-center radial spline valleys as the primary drainage system;
- “erosion noise” that does not compute flow direction and accumulation;
- drawing ridges first as radial spokes and pretending boundaries are watersheds;
- random tributary scratches without contributing area, stream order, or confluence rules;
- unrestricted erosion inside the accepted beach/shoreline hard lock;
- moving the land/sea mask to make hydrology look better;
- importing a black-box terrain package as the generator of record;
- nondeterministic runtime erosion that changes source evidence between runs;
- player-facing navigation UI, minimap, compass, GPS, or objective overlays as part of this terrain task.

## Sources And References

These sources support the mathematical and implementation direction. They are references for a source-owned Jungle Game implementation, not dependencies.

1. O'Callaghan, J. F., & Mark, D. M. (1984). **The extraction of drainage networks from digital elevation data.** *Computer Vision, Graphics, and Image Processing*, 28, 323–344. Commonly cited as the classic D8 drainage extraction paper.
2. Tarboton, D. G. (1997). **A new method for the determination of flow directions and upslope areas in grid digital elevation models.** *Water Resources Research*, 33(2), 309–319. D∞ flow direction and contributing area. https://doi.org/10.1029/96WR03137
3. Freeman, G. T. (1991). **Calculating catchment area with divergent flow based on a regular grid.** *Computers & Geosciences*, 17(3), 413–422. Multiple-flow direction / divergent flow routing.
4. Barnes, R., Lehman, C., & Mulla, D. (2014). **Priority-Flood: An optimal depression-filling and watershed-labeling algorithm for digital elevation models.** *Computers & Geosciences*, 62, 117–127. https://doi.org/10.1016/j.cageo.2013.04.024
5. Whipple, K. X., & Tucker, G. E. (1999). **Dynamics of the stream-power river incision model: Implications for height limits of mountain ranges, landscape response timescales, and research needs.** *Journal of Geophysical Research*, 104(B8), 17661–17674. https://doi.org/10.1029/1999JB900120
6. Tucker, G. E., & Hancock, G. R. (2010). **Modelling landscape evolution.** *Earth Surface Processes and Landforms*, 35(1), 28–50. https://doi.org/10.1002/esp.1952
7. Lague, D. (2014). **The stream power river incision model: evidence, theory and beyond.** *Earth Surface Processes and Landforms*, 39(1), 38–61. https://doi.org/10.1002/esp.3462
8. Kwang, J. S., & Parker, G. (2017). **Landscape evolution models using the stream power incision model show unrealistic behavior when m/n equals 0.5.** *Earth Surface Dynamics*, 5, 807–820. https://doi.org/10.5194/esurf-5-807-2017
9. Horton, R. E. (1945). **Erosional development of streams and their drainage basins; hydrophysical approach to quantitative morphology.** *Geological Society of America Bulletin*, 56(3), 275–370. https://doi.org/10.1130/0016-7606(1945)56[275:EDOSAT]2.0.CO;2
10. Strahler, A. N. (1952). **Hypsometric (area-altitude) analysis of erosional topography.** *Geological Society of America Bulletin*, 63(11), 1117–1142. https://doi.org/10.1130/0016-7606(1952)63[1117:HAAOET]2.0.CO;2
11. Strahler, A. N. (1957). **Quantitative analysis of watershed geomorphology.** *Transactions, American Geophysical Union*, 38(6), 913–920. Horton-Strahler stream ordering and watershed morphometry.
12. Hack, J. T. (1957). **Studies of longitudinal stream profiles in Virginia and Maryland.** *U.S. Geological Survey Professional Paper 294-B*. Original Hack's law context for stream length/drainage area scaling.
13. Rigon, R., Rodríguez-Iturbe, I., Maritan, A., Giacometti, A., Tarboton, D. G., & Rinaldo, A. (1996). **On Hack's law.** *Water Resources Research*, 32(11), 3367–3374. https://doi.org/10.1029/96WR02397
14. Sassolas-Serrayet, T., Cattin, R., & Ferry, M. (2018). **The shape of watersheds.** *Nature Communications*, 9, 3791. https://doi.org/10.1038/s41467-018-06210-4
15. Shobe, C. M., Tucker, G. E., & Barnhart, K. R. (2017). **The SPACE 1.0 model: a Landlab component for 2-D calculation of sediment transport, bedrock erosion, and landscape evolution.** *Geoscientific Model Development*, 10, 4577–4604. https://doi.org/10.5194/gmd-10-4577-2017
16. GRASS GIS. **r.watershed manual.** Documents standard outputs such as flow accumulation, drainage direction, streams, and watershed basins. https://grass.osgeo.org/grass-stable/manuals/r.watershed.html
17. Fastscape. **Software tools for landscape evolution modeling.** Reference ecosystem for efficient landscape evolution components using stream-power, diffusion, and sediment routines. https://fastscape.org/
18. Landlab documentation. **SPACE and stream-power components.** Reference implementation patterns for erosion, deposition, sediment conservation, and flow accumulation in landscape-evolution modeling. https://landlab.csdms.io/
