# Ridge-Valley Graph And Landform Regions Research

## Executive Summary

Jungle Game’s current interior terrain failure is not primarily a noise problem. It is a missing intermediate-representation problem. The deterministic island already preserves the important coastal contract, but the interior reads as one procedural volcano because the heightfield appears to be driven by a single central radial authority. The fix is to place a deterministic **terrain graph and named landform-region system** between raw procedural fields and sampled height.

The proposed system builds island-scale identity from:

- a constrained planar terrain graph;
- hard semantic region IDs for debug traceability;
- soft scalar memberships for continuous height blending;
- basin/ridge adjacency;
- valley axes that connect uplands to legal coastal outlets;
- ridge divides derived from basin boundaries, not crater angle;
- saddle/pass features on ridge edges;
- terrace, bench, scarp, plateau, basin-floor, and coastal-flat features as graph-owned stamps;
- a compact active-volcano subsystem attached to the terrain graph instead of owning the whole island.

The core generation rule is:

```text
semantic ownership first
hydrologic/ridge topology second
height contribution fields third
noise and erosion detail last
```

The recommended minimum for a 97.536 km island preview is **24 named major landform regions**, with at least **7 primary drainage basins**, **5 coastal-flat/outlet regions**, **4 upland blocks**, **3 old terrace/bench provinces**, **2 secondary hill provinces**, **2 saddle/pass corridor regions**, and **1 compact active volcano complex**. This count is high enough to break the “one object” read while still being small enough for deterministic debug review.

The graph must preserve the existing world/coast gates:

```text
world_size_m: 97536.0 target
shoreline_error_max_m: 0 or effectively 0
beach_continuity_pct: 100%
ocean_below_sea_pct: 100%
square_edge_ocean_violations: 0
outer world edge remains ocean/container terrain
beach and shallow shelf remain continuous
```

The interior graph is therefore clipped to the island mask and must never be allowed to redefine the coastline, beach continuity, shelf continuity, sea level, or square-edge ocean constraints.

Relevant research basis:

- Hydrology-driven procedural terrain can use graph-structured drainage networks as primary modeling elements rather than deriving rivers from final noise-only terrain [Genevaux2013].
- Voronoi, weighted Voronoi, power diagrams, and anisotropic variants are standard ways to define spatial ownership, adjacency, and nonuniform influence zones [Aurenhammer1991], [Okabe1992], [LabelleShewchuk2003], [CGALApollonius].
- Drainage basins are naturally separated by drainage divides, which are ridges/hills that separate watersheds [USGSWatersheds].
- DEM landform classification uses slope, convexity, texture, topographic position, and geomorphon-like pattern classes to distinguish flats, ridges, shoulders, valleys, pits, and related terrain elements [IwahashiPike2007], [USGSGeomorphon], [WeissTPI].

## Terrain-Only Scope Boundary

This document specifies terrain graph, landform segmentation, heightfield construction, and developer debug validation.

It does **not** design:

- foliage;
- ecology;
- weather;
- audio;
- gameplay;
- NPCs;
- buildings;
- final materials;
- map, compass, minimap, GPS, objective markers, or player-facing navigation UI.

The system may emit terrain debug maps and preview manifests for developer review. Those maps are not player-facing navigation features.

## Current One-Feature Terrain Failure

The existing island passes coastal metrics but fails interior believability. The failure signature is:

```text
one dominant central volcanic feature
radial fan-blade ridges
radial gullies
weak basin identity
weak old-terrain identity
few believable saddles/passes
valleys appear angle-derived rather than basin-derived
volcano owns the island rather than existing inside it
```

This usually happens when the sampled height is dominated by a small number of fields such as:

```text
height = island_falloff + radial_mountain + ridged_noise + erosion_noise
```

Even with good noise, erosion, and coastal masking, this creates a terrain whose global structure is legible as “the formula,” not “a landscape.” A believable large island needs multiple regional authorities: old highlands, separate drainage basins, coastal plains, uplifted benches, scarps, secondary hills, and a compact active volcanic region.

The required shift is:

```text
from: heightfield directly creates all landforms
to: graph/regions create landform ownership, then heightfield realizes them
```

## Terrain Graph Goals

The graph must answer these questions before final height sampling:

1. Where are the main uplands?
2. Where are the named drainage basins?
3. Which ridges divide basins?
4. Where do valleys begin, merge, and exit?
5. Which low points are saddles or passes?
6. Which areas are old benches, terraces, coastal plains, scarps, basin floors, or secondary hills?
7. Where does the compact active volcano attach to the older island?
8. Which sampled pixels belong to which semantic terrain owner?
9. Which debug maps prove this ownership without relying on final material art?

Design goals:

- **Deterministic:** same seed, same graph, same region IDs, same height.
- **Asymmetric:** no equal-angle radial fan; no mirrored region layout.
- **Nonuniform:** regions vary meaningfully in size, elongation, elevation target, and drainage topology.
- **Planar:** graph edges should not cross except at explicit graph nodes.
- **Coast-preserving:** graph cannot change shoreline, beach, shelf, or ocean containment.
- **Hydrologically legible:** primary basins have outlets, valley axes, divides, and basin floors.
- **Debuggable:** hard region ID remains available even where height is softly blended.
- **Volcano-contained:** volcano contributes a compact subsystem, not the island’s global coordinate frame.

## Landform Region Taxonomy

Use a two-level taxonomy:

```cpp
enum class ELandformRegionClass : uint8
{
    HighlandSpine,
    UplandBlock,
    DrainageBasin,
    BasinFloor,
    RidgeDivide,
    SaddlePass,
    CoastalFlat,
    OldTerrace,
    Bench,
    Scarp,
    SecondaryHill,
    ActiveVolcano,
    VolcanoApron,
    LavaBench,
    Transition
};
```

Hard region IDs should be stable and named. A practical minimum set for the 97.536 km island is **24 major regions**:

| Region ID pattern | Minimum count | Purpose |
|---|---:|---|
| `SPINE_*` | 3 | Segments of the old highland spine; not one central cone. |
| `UPLAND_*` | 4 | Broad upland blocks with independent height targets and roughness. |
| `BASIN_*` | 7 | Primary drainage basins with outlets and valley axes. |
| `BASIN_FLOOR_*` | 3 | Lower-gradient basin interiors, not all valley pixels. |
| `COASTAL_FLAT_*` | 5 | Low coastal plains behind beach/shelf gates. |
| `OLD_TERRACE_*` | 3 | Uplifted old volcanic/coastal surfaces. |
| `SECONDARY_HILLS_*` | 2 | Non-volcano hill masses that break scale. |
| `PASS_CORRIDOR_*` | 2 | Named saddle/pass corridors through ridges. |
| `ACTIVE_VOLCANO_CORE` | 1 | Compact active volcanic cone/crater subsystem. |
| `VOLCANO_APRON` | 1 | Blended apron tying the active volcano to existing terrain. |

This yields more than 24 records if basin floors are represented as child regions, but the **major visible identity count** should never fall below 24. More is acceptable if the preview and manifest remain readable.

Recommended macro composition:

```text
northwest old spine segment
central-west spine segment
south highland spur
four upland blocks with different aspect and roughness
seven asymmetric basins with legal coastal outlets
two interior saddles connecting basins over divides
five coastal-flat provinces clipped behind beach/shelf continuity
three old terrace/bench provinces at distinct elevation bands
two secondary hill provinces away from the active volcano
one compact active volcano + apron
```

Forbidden region taxonomy patterns:

- one `CENTRAL_VOLCANO` owner covering most of the island;
- one `RADIAL_RIDGE_N` set generated from equal angular bins;
- region IDs assigned only after the heightfield is finished;
- soft masks without stable hard semantic IDs;
- basin regions with no outlet;
- ridge regions not tied to basin adjacency.

## Graph Data Model

Use a constrained planar terrain graph with explicit node and edge types. The graph owns **landform semantics** and **structural curves**. It does not directly own every final sample.

### Core structures

```cpp
enum class ETerrainGraphNodeType : uint8
{
    HighlandPeak,
    UplandCentroid,
    BasinCentroid,
    BasinOutlet,
    ValleyHead,
    ValleyConfluence,
    RidgeJunction,
    Saddle,
    Pass,
    TerraceAnchor,
    ScarpAnchor,
    VolcanoCore,
    VolcanoCrater,
    VolcanoApronAnchor,
    CoastalFlatAnchor
};

enum class ETerrainGraphEdgeType : uint8
{
    RidgeSpine,
    RidgeDivide,
    ValleyAxis,
    BasinAdjacency,
    OutletConnection,
    SaddleConnection,
    TerraceEdge,
    ScarpEdge,
    VolcanoAttachment,
    CoastalFlatBoundary
};

enum class ELandformRegionClass : uint8;

struct FTerrainGraphNode
{
    int32 NodeId;
    FName StableName;
    ETerrainGraphNodeType Type;

    FVector2D WorldXY;
    float ElevationTargetM;
    float InfluenceRadiusM;
    float AnisotropyRatio;
    float OrientationRad;

    int32 OwningRegionId;
    uint32 StableSeed;
};

struct FTerrainGraphEdge
{
    int32 EdgeId;
    FName StableName;
    ETerrainGraphEdgeType Type;

    int32 NodeA;
    int32 NodeB;
    TArray<FVector2D> ControlPoints; // deterministic spline/geodesic controls

    int32 LeftRegionId;
    int32 RightRegionId;

    float WidthM;
    float SoftnessM;
    float ReliefM;
    float LongitudinalFallM;
    float CurvatureTarget;
    uint32 StableSeed;
};

struct FLandformRegion
{
    int32 RegionId;
    FName StableName;
    ELandformRegionClass Class;

    int32 ParentRegionId; // -1 for macro regions
    TArray<int32> ChildRegionIds;
    TArray<int32> BoundaryEdgeIds;
    TArray<int32> NeighborRegionIds;

    FVector2D SeedXY;
    float AreaTargetKm2;
    float AreaActualKm2;

    float ElevationBaseM;
    float ElevationRangeM;
    float RoughnessScaleM;
    float RoughnessAmplitudeM;

    float CoastDistanceMinM;
    float CoastDistanceMaxM;
    float SlopeTargetMinDeg;
    float SlopeTargetMaxDeg;

    float BlendSoftnessM;
    uint32 StableSeed;
};

struct FRidgeSpine
{
    int32 SpineId;
    FName StableName;

    TArray<int32> NodeIds;
    TArray<int32> EdgeIds;
    TArray<int32> LeftBasinRegionIds;
    TArray<int32> RightBasinRegionIds;

    float CrestWidthM;
    float CrestReliefM;
    float DivideContinuityScore;
};

struct FValleyAxis
{
    int32 ValleyId;
    FName StableName;

    int32 SourceNodeId;
    int32 OutletNodeId;
    TArray<int32> ConfluenceNodeIds;
    TArray<FVector2D> Polyline;

    int32 BasinRegionId;
    float FloorWidthHeadM;
    float FloorWidthMouthM;
    float LongitudinalSlopeMean;
    float CurvatureScore;
};

struct FTerrainSaddle
{
    int32 SaddleId;
    FName StableName;

    int32 NodeId;
    int32 RidgeEdgeId;
    int32 BasinARegionId;
    int32 BasinBRegionId;

    FVector2D WorldXY;
    float ElevationM;
    float NotchDepthM;
    float PassWidthM;
    float CrossSlopeLimitDeg;
};

struct FTerrainTerrace
{
    int32 TerraceId;
    FName StableName;

    int32 RegionId;
    TArray<int32> BoundaryEdgeIds;

    float ElevationM;
    float WidthM;
    float InnerScarpHeightM;
    float OuterScarpHeightM;
    float ContinuityScore;
};

struct FLandformMetrics
{
    int32 RegionCount;
    int32 MajorRegionCount;
    int32 BasinCount;
    int32 OutletCount;
    int32 RidgeEdgeCount;
    int32 ValleyAxisCount;
    int32 SaddleCount;
    int32 TerraceCount;

    float RegionAreaGini;
    float RegionAreaMinKm2;
    float RegionAreaMaxKm2;

    float MeanAdjacencyDegree;
    float MaxAdjacencyDegree;
    float GraphBranchingMean;
    float ValleyCurvatureMean;
    float RidgeDivideContinuityMean;

    float VolcanoFootprintPctOfIslandLand;
    float CoastGateViolationCount;
};
```

### Graph invariants

The generator must enforce these invariants before heightfield sampling:

```text
all region seeds lie inside the legal island interior mask
all coastal-flat seeds lie landward of beach/shelf gates
all basin outlets touch legal coastal outlet zones without modifying shoreline
all valleys terminate at basin outlets or confluences
all basin regions have at least one outlet path
ridge divides lie between basin regions
saddles lie on ridge divides or ridge-spine edges
active volcano footprint is compact and below max island footprint percentage
no graph edge crosses another edge unless an explicit node exists there
no equal-angle radial ridge distribution from the volcano or island center
no mirrored or four-quadrant symmetric region placement
```

### Recommended graph layers

Use three explicit graph layers:

```text
Layer 0: macro landform region ownership
Layer 1: basin/ridge/valley topology
Layer 2: feature stamps and scalar-field conversion
```

Layer 0 is stable region segmentation. Layer 1 is the hydrologic and divide skeleton. Layer 2 turns graph features into continuous terrain fields.

## Deterministic Region Seeding

Region seeding must be deterministic, asymmetric, nonuniform, and rejection-tested.

### Inputs

```text
world_seed
island_mask
coast_distance_field
beach/shelf/ocean masks
existing high-level island falloff field
legal_interior_mask = island_mask - beach/shelf/protected coast bands
```

### Seed sampler

Use deterministic weighted rejection sampling:

```cpp
struct FLandformSeedCandidate
{
    FVector2D WorldXY;
    ELandformRegionClass DesiredClass;
    float CoastDistanceM;
    float InteriorScore;
    float SlopePrior;
    float NoisePrior;
    float AcceptanceWeight;
    uint32 StableSeed;
};
```

A candidate is accepted only if it satisfies class constraints:

| Class | Placement constraints |
|---|---|
| Highland spine | Far from coast; elongated; not centered exactly; has 2-4 linked segments. |
| Upland block | Interior or mid-island; separated from spine and volcano; varied elevation. |
| Drainage basin | Between upland and coast; must be connectable to outlet. |
| Coastal flat | Behind coast/beach band; low elevation; elongated along coast but clipped away from ocean. |
| Old terrace | Mid-elevation band; near old scarp/bench edge; partial continuity. |
| Secondary hill | Away from volcano; smaller radius; breaks monotony. |
| Active volcano | Compact; attached to graph; not global center; bounded footprint. |

### Weighted anisotropic distance

For each region seed `i`, define an anisotropic power distance:

```text
D_i(p) = (p - s_i)^T A_i (p - s_i) - w_i^2 + B_i(p)
```

Where:

- `s_i` is the seed position.
- `A_i = R(theta_i) diag(1/a_i^2, 1/b_i^2) R(theta_i)^T`.
- `a_i` and `b_i` are major/minor influence radii.
- `w_i` is a power weight for intentional area variation.
- `B_i(p)` is a deterministic bias term for coast distance, elevation prior, and class suitability.

Hard ownership:

```text
HardRegionId(p) = argmin_i D_i(p)
```

Soft membership:

```text
M_i(p) = exp(-D_i(p)/tau) / sum_j exp(-D_j(p)/tau)
```

Use `HardRegionId` for debug traceability and manifest ownership. Use `M_i` only for continuous height blending near boundaries.

This follows the practical strengths of Voronoi/power-diagram ownership: explicit cells, adjacency through shared boundaries, and controlled nonuniform region sizes [Aurenhammer1991], [Okabe1992]. Additively weighted diagrams and Apollonius-graph equivalents are useful references for weighted influence regions [CGALApollonius]. Anisotropic Voronoi concepts justify elliptical/metric-based influence rather than circular distance alone [LabelleShewchuk2003].

### Nonuniformity tests

After region assignment, reject and resample the graph if any of the following fail:

```text
region_area_gini < 0.22 or > 0.58
largest_major_region_area_pct > 18%
smallest_major_region_area_pct < 1.0% unless explicitly secondary/pass
mean_adjacency_degree < 2.4
basin_count < 7
coastal_flat_count < 5
volcano_footprint_pct > 8%
volcano_footprint_pct < 1.5%
angle_histogram_max_bin_pct > 28% for ridge or valley initial bearings
mirrored_pair_score > 0.72
```

The angle histogram should be measured from multiple anchors, not only the island center. This prevents hidden radial fans around the active volcano, the highland centroid, or the world center.

### Deterministic rejection procedure

```text
for attempt in 0..MaxAttempts:
    rng = Hash(world_seed, "LandformRegions", attempt)
    generate class quotas
    sample seed candidates
    construct anisotropic power regions
    clip to legal island interior
    compute adjacency
    connect basins to outlets
    derive ridges/divides
    place saddles/terraces/volcano
    compute metrics
    if all hard gates pass:
        accept graph with attempt index
        break
```

`attempt` must be stored in the preview manifest. That makes the final terrain reproducible and explains why a particular seed accepted on attempt N.

## Basin And Ridge Adjacency

Drainage basins should be first-class graph regions, not incidental byproducts of erosion noise. Hydrologically, a watershed/drainage basin is the land area draining toward an outlet, and ridges/hills separating watersheds are drainage divides [USGSWatersheds]. This maps directly to terrain graph ownership:

```text
basin region -> valley network -> coastal outlet
basin boundary -> ridge divide
basin adjacency -> possible saddle/pass or high divide
```

### Basin graph

Represent basin topology separately from generic region adjacency:

```cpp
struct FBasinGraph
{
    TArray<int32> BasinRegionIds;
    TArray<int32> OutletNodeIds;
    TArray<int32> DivideEdgeIds;
    TArray<int32> ValleyAxisIds;
    TArray<int32> SaddleIds;
};
```

For each basin:

```text
one primary outlet
one trunk valley axis
1-3 tributary headwater axes for large basins
shared ridge divides with adjacent basins
optional basin-floor child region
optional saddle to another basin
```

### Basin placement rules

Use the coast-distance field and macro-upland seeds:

1. Select legal outlet candidates along island coast-adjacent land, not on the protected beach/shelf cells.
2. Cluster outlet candidates into asymmetric coast sectors.
3. Pair each basin seed with one outlet using a cost function:

```text
C(seed, outlet) =
    distance(seed, outlet)
  + lambda_cross * graph_crossing_penalty
  + lambda_slope * uphill_outlet_penalty
  + lambda_spacing * outlet_spacing_penalty
  + lambda_radial * radial_alignment_penalty
```

4. Reject basin layouts where too many valley axes point toward a single compass sector.

### Ridge divide extraction

Given accepted basin regions, ridge divides are derived from adjacency boundaries:

```text
For each adjacent basin pair (A, B):
    boundary_curve = shared hard-region boundary
    ridge_curve = smoothed medial/biased boundary curve
    ridge_height = max(base(A), base(B)) + divide_relief
    create FRidgeSpine/FTerrainGraphEdge if length exceeds threshold
```

The divide is not an angle ray. It is a graph edge with left/right basin ownership. It may be interrupted or lowered by explicit saddle/pass nodes.

### Ridge continuity metric

```text
ridge_divide_continuity =
    continuous_length_of_valid_divide_edges / total_basin_boundary_length
```

Acceptance target:

```text
mean_ridge_divide_continuity >= 0.82
min_primary_divide_continuity >= 0.68
```

Continuity does not mean perfectly smooth. It means the debug overlay shows legible basin separation.

## Valley Curves And Outlet Connections

Valleys should be generated from basin topology and legal outlets, not from crater angle.

### Valley axis generation

For each basin:

1. Identify one `ValleyHead` near upland/highland boundary.
2. Identify one `BasinOutlet` near legal coastal outlet zone.
3. Route a curve through the basin using a weighted path cost field.
4. Add tributaries only where basin size supports them.

Path cost:

```text
PathCost(p) =
    w_region * outside_basin_penalty(p)
  + w_slope * uphill_penalty(p)
  + w_ridge * ridge_crossing_penalty(p)
  + w_coast * illegal_coast_penalty(p)
  + w_curve * curvature_regularizer(p)
  + w_noise * deterministic_meander_bias(p)
```

The route may be solved on a coarse grid, navmesh-like raster, or constrained Delaunay/Voronoi dual graph. The output is stored as a spline/polyline in `FValleyAxis`.

### Valley cross-section

A valley axis contributes a negative height field and a floor-flattening field:

```text
d = signed/lateral distance to valley axis
t = normalized distance from headwater to outlet

floor_width(t) = lerp(FloorWidthHeadM, FloorWidthMouthM, smoothstep(t))
incision(t) = lerp(HeadIncisionM, MouthIncisionM, t^gamma)
cross_section(d,t) =
    -incision(t) * exp(-(abs(d)/floor_width(t))^p)
```

Recommended parameters:

```text
headwater floor width: 80-180 m
trunk valley mid width: 250-700 m
lower basin floor width: 700-1800 m
incision head: 10-60 m
incision trunk: 80-260 m
mouth incision: reduced if coastal flat takes over
p: 1.7-2.8
```

### Valley curvature metric

Measure valley axis curvature by integrated turn angle per kilometer, then reject nearly straight radial valleys:

```text
curvature_km = sum(abs(delta_heading)) / length_km
```

Acceptance target:

```text
primary_valley_curvature_mean >= 0.11 rad/km
primary_valley_curvature_max <= 1.2 rad/km
straight_primary_valley_pct <= 20%
```

The goal is not serpentine rivers everywhere. It is to prevent straight fan-blades.

### Outlet connection rules

Valley outlets must:

```text
connect to legal coastal flat or outlet node
not cut through protected beach continuity
not produce ocean_below_sea violations
not change shoreline mask
flatten gradually into coastal flat
store outlet id in manifest
```

The heightfield may lower land toward the outlet, but the coast gate remains authoritative.

## Ridge Spines, Saddles, Passes, And Scarps

### Ridge spines

Ridge spines are graph curves with left/right region ownership. They may be:

- old highland-spine ridges;
- basin divides;
- secondary hill crests;
- volcano-apron ridges;
- scarp crest lines.

Ridge contribution:

```text
ridge_height(d,t) =
    relief(t) * exp(-(abs(d)/crest_width(t))^q)
```

With asymmetric shoulders:

```text
left_shoulder = exp(-(max(0, -d)/left_width)^q)
right_shoulder = exp(-(max(0, d)/right_width)^q)
```

Use different left/right widths to avoid symmetric procedural ridges.

### Saddles and passes

A saddle/pass is a local low point on a ridge divide connecting two basins or upland blocks.

Represent as `FTerrainSaddle` attached to a ridge edge:

```text
saddle position: point on ridge curve
saddle elevation: lower than neighboring ridge crest, higher than basin floors
notch depth: 25-180 m depending on relief
pass width: 200-1200 m
cross-slope target: <= 12-22 degrees for broad passes
```

Saddle shaping:

```text
S(p) =
    -notch_depth * exp(-((along_distance/a)^2 + (cross_distance/b)^2))
```

Where `a` controls pass length along the ridge and `b` controls cross-ridge widening.

Acceptance:

```text
at least 2 named pass corridors
each pass must connect two named regions
pass elevation must be below adjacent ridge crest by >= 25 m
pass elevation must be above adjacent basin floor by >= 40 m
```

### Scarps

Scarps are line or band features associated with terraces, benches, old volcanic surfaces, or uplifted coastal plains. They are not random cliffs.

Represent scarps as edges:

```cpp
struct FScarpFeature
{
    int32 ScarpId;
    FName StableName;
    int32 EdgeId;
    int32 UpperRegionId;
    int32 LowerRegionId;
    float HeightM;
    float WidthM;
    float FaceSlopeTargetDeg;
    float ToeSoftnessM;
};
```

Scarp field:

```text
scarp_step(p) = height * smoothstep(-width, width, signed_distance_to_scarp)
```

Use noise only to roughen scarp face and toe, not to define scarp placement.

### Ridge/saddle debug checks

Required debug outputs:

```text
ridge divide overlay with left/right basin labels
saddle/pass point map
saddle elevation labels
ridge continuity heatmap
cross-section sample plots through each pass
```

## Terraces, Benches, Plateaus, And Coastal Flats

### Terraces

Terraces are semi-continuous elevation bands or old surfaces. They can be volcanic, marine/coastal, or erosional. They should be graph-owned features, not threshold artifacts discovered after height generation.

Terrace region parameters:

```text
elevation band: base elevation +/- tolerance
width: 500-3000 m
continuity: partial, not full island rings
inner/outer scarp heights
surface roughness lower than adjacent dissected uplands
```

Terrace field:

```text
terrace_membership =
    region_mask * band_mask(elevation_prior) * lateral_continuity_mask

height =
    lerp(current_height, terrace_elevation + micro_relief, terrace_strength)
```

Do **not** make complete circular contour rings around the volcano. Old terraces should be broken, offset, and attached to old terrain regions.

### Benches

Benches are smaller planar or gently sloped surfaces, often on ridge shoulders, old lava surfaces, or eroded blocks.

Bench rules:

```text
owned by upland/old terrace/volcano apron region
area 0.5-8 km²
slope target 1-7 degrees
edge scarp optional
not placed uniformly around central point
```

### Plateaus

Plateaus are broad high or mid-elevation surfaces with stronger identity than benches.

Plateau rules:

```text
belongs to UplandBlock or HighlandSpine segment
area 8-60 km²
internal relief limited
edges shaped by scarps, ridges, or incised valleys
no perfectly flat tabletop unless explicitly debug/test
```

### Coastal flats

Coastal flats are low-gradient landward regions that smooth the transition from basin outlets to the protected beach system.

They must obey:

```text
beach continuity remains 100%
shallow shelf remains continuous
coastal flat cannot redefine shoreline
flat elevation remains landward and above sea where required
outlet lowering cannot create ocean or square-edge violations
```

Coastal flat shaping:

```text
flat_strength = smoothstep(coast_distance_min, coast_distance_max, coast_distance)
height = lerp(height, coastal_flat_elevation + drainage_gradient, flat_strength)
```

Recommended parameters:

```text
coastal flat width: 800-4500 m depending on coast sector
gradient: 0.2-1.5 degrees
micro relief amplitude: 0.5-5 m
outlet channel inset: 1-12 m if needed
```

### DEM classification basis

Validation can borrow ideas from DEM landform classification:

- slope gradient;
- local convexity;
- surface texture;
- multi-scale topographic position;
- geomorphon-like classes such as flat, peak, ridge, shoulder, spur, slope, hollow, footslope, valley, and pit.

Iwahashi and Pike’s automated DEM classification uses slope gradient, local convexity, and surface texture as terrain signatures [IwahashiPike2007]. USGS hydrography specifications summarize geomorphon landform classes into flat, peak, ridge, shoulder, spur, slope, hollow, footslope, valley, and pit classes [USGSGeomorphon]. TPI-based methods classify relative slope position such as valley, lower slope, flat, middle slope, upper slope, and ridge [WeissTPI]. These are useful as validation concepts, not as the primary generator.

## Scalar Field Conversion From Graph Features

The graph must be converted into continuous scalar fields while retaining hard semantic IDs.

### Field stack

Recommended field stack:

```cpp
struct FTerrainGraphSample
{
    FVector2D WorldXY;

    int32 HardRegionId;
    ELandformRegionClass HardRegionClass;

    TArray<int32> TopRegionIds;
    TArray<float> TopRegionWeights;

    float BaseElevationM;
    float RidgeContributionM;
    float ValleyContributionM;
    float SaddleContributionM;
    float TerraceContributionM;
    float ScarpContributionM;
    float VolcanoContributionM;
    float NoiseContributionM;

    float FinalHeightM;
};
```

Final height:

```text
H(p) =
    H_base_regions(p)
  + H_ridges(p)
  + H_valleys(p)
  + H_saddles(p)
  + H_terraces(p)
  + H_scarps(p)
  + H_volcano(p)
  + H_noise_detail(p)
```

Where:

```text
H_base_regions(p) = sum_i M_i(p) * H_i_base(p)
```

`M_i` is the soft membership. `HardRegionId` remains `argmin(D_i)` and is exported.

### Blend discipline

The common failure is to blend everything until debug meaning disappears. Avoid that by separating:

```text
HardRegionId: discrete owner used for debug/manifest/metrics
SoftRegionWeight: continuous scalar blend only near boundaries
FeatureId: graph feature that contributed ridge/valley/saddle/terrace
```

A sample can have:

```text
HardRegionId = BASIN_SOUTHWEST_02
TopRegionWeights = [BASIN_SOUTHWEST_02: 0.62, UPLAND_WEST_01: 0.28, TERRACE_WEST_OLD_01: 0.10]
DominantFeature = VALLEY_AXIS_SW_02
```

### Signed distance fields

Precompute or sample signed distance fields for:

```text
region boundary distance
ridge curve distance
valley axis distance
terrace edge distance
scarp edge signed side
coast distance
volcano footprint distance
```

Use these SDFs for stable blending and debug overlays.

### Feature priority

Recommended order:

1. Coast/ocean/beach/shelf hard gates.
2. Legal island mask.
3. Macro region base elevation.
4. Ridge divides/spines.
5. Valley axes and basin floors.
6. Saddles/passes.
7. Terraces/benches/scarps.
8. Volcano subsystem.
9. Local detail noise and erosion-style roughness.
10. Final coast revalidation clamp.

The coast gates must remain first and last.

### Noise discipline

Noise may vary local texture but must not decide macro structure.

Allowed:

```text
region-specific roughness
ridge shoulder breakup
valley sidewall variation
terrace micro-relief
scarp face roughness
volcano lava/apron roughness
```

Forbidden:

```text
ridged noise as primary island skeleton
central radial noise as ridge generator
noise-only basin discovery with no graph owner
noise warps that move coast/beach/shelf gates
```

## Volcano Subsystem Attachment Points

The active volcano should be a compact subsystem attached to the terrain graph.

### Volcano records

```cpp
struct FVolcanoSubsystem
{
    int32 CoreRegionId;        // ACTIVE_VOLCANO_CORE
    int32 ApronRegionId;       // VOLCANO_APRON
    int32 CraterNodeId;
    int32 AttachmentNodeId;

    TArray<int32> AttachmentEdgeIds; // to upland/basin/ridge graph
    TArray<int32> LavaBenchRegionIds;
    TArray<int32> RadialGullyFeatureIds; // capped and local only

    FVector2D CoreXY;
    float BaseElevationM;
    float SummitElevationM;
    float CraterRadiusM;
    float ConeRadiusM;
    float ApronRadiusM;
    float FootprintPctOfIslandLand;
};
```

### Placement constraints

```text
volcano core is not world center unless seed naturally resolves there and asymmetry checks pass
volcano core is not the highland spine root
volcano core attaches to 1-2 upland/old terrace regions
volcano footprint <= 8% of island land area
volcano apron + lava benches <= 14% of island land area
local volcano gullies cannot extend past volcano apron unless handed off to basin valleys
```

### Attachment model

The volcano may contribute:

- a compact cone or nested cone/crater;
- local lava benches;
- apron slope;
- short radial gullies within the apron;
- a blocked/deflected basin boundary if explicitly represented.

The volcano must not contribute:

- global radial ridge fans;
- all major drainage basins;
- island-scale height falloff;
- coast-to-summit structure;
- equal-angle spokes.

### Volcano-disabled debug map

A required preview output is a **volcano-disabled region map**:

```text
render final terrain with H_volcano = 0 but all non-volcano graph regions enabled
```

Acceptance:

```text
island still reads as a complete terrain without the active volcano
highland spine remains legible
basins still drain to outlets
coastal flats remain intact
old terraces/secondary hills remain visible
```

If the island collapses into an unstructured blob when volcano is disabled, the volcano is still carrying too much global structure.

## Debug Region Maps And Developer Review Points

The preview must emit deterministic debug outputs that prove the graph works before art/material polish.

### Required maps

1. **Hard region ID map**

   ```text
   each pixel colored by HardRegionId
   labels for major named regions
   adjacency boundaries visible
   coast/beach/shelf masks overlaid or outline-visible
   ```

2. **Graph overlay map**

   ```text
   nodes by type
   edges by type
   labels for basin outlets, saddles, volcano core, ridge junctions
   ```

3. **Ridge/valley skeleton map**

   ```text
   ridge divides
   highland spine
   valley axes
   tributary axes
   outlets
   confluences
   ```

4. **Saddle/pass map**

   ```text
   saddle points
   pass corridors
   cross-section sample lines
   notch depth labels
   pass elevation labels
   ```

5. **Plateau/terrace/bench map**

   ```text
   terrace regions
   bench stamps
   plateau masks
   scarp edges
   elevation-band labels
   ```

6. **Volcano-disabled region map**

   ```text
   terrain with volcano scalar fields removed
   graph/regions unchanged
   confirms active volcano is not global owner
   ```

7. **Coast preservation map**

   ```text
   shoreline error
   beach continuity
   shelf continuity
   ocean below sea
   square-edge ocean/container check
   ```

8. **Scalar contribution atlas**

   Separate grayscale maps for:

   ```text
   base region field
   ridge contribution
   valley contribution
   saddle contribution
   terrace/bench contribution
   scarp contribution
   volcano contribution
   final height
   ```

### Developer review checkpoints

Use these review checkpoints before implementation proceeds to detail noise:

| Checkpoint | What reviewer should see |
|---|---|
| Region map only | Island has 24+ named asymmetric landform regions. |
| Graph overlay only | Basins, ridges, passes, valleys, and volcano attachment are legible. |
| Height without noise | Terrain already reads as composed landforms. |
| Height without volcano | Island still reads as old terrain with basins and uplands. |
| Height with volcano | Active volcano is compact and integrated. |
| Coast gate overlay | Existing coast metrics remain unchanged. |

## Validation Metrics

Metrics must be exported to the preview manifest and optionally to a CSV/JSON debug artifact.

### Coast preservation metrics

These are hard gates:

```text
world_size_m == 97536.0 target
shoreline_error_max_m <= epsilon
beach_continuity_pct == 100
ocean_below_sea_pct == 100
square_edge_ocean_violations == 0
outer_world_edge_ocean_pct == 100
shallow_shelf_continuity_pct == 100
```

No graph improvement is valid if these regress.

### Region metrics

```text
major_region_count >= 24
hard_region_count >= 32 including child regions/features
region_area_gini between 0.22 and 0.58
largest_major_region_area_pct <= 18
smallest_major_region_area_pct >= 1.0 unless class is pass/scarp/bench
mean_region_adjacency_degree >= 2.4
max_region_adjacency_degree <= 9
mirrored_pair_score <= 0.72
equal_angle_distribution_score <= 0.40
```

### Basin metrics

```text
primary_basin_count >= 7
basins_with_outlet_pct == 100
basins_with_valley_axis_pct == 100
primary_outlet_count >= 5
mean_basin_area_km2 within configured target range
basin_area_gini between 0.18 and 0.55
basin_boundary_owned_pct >= 95
```

### Ridge metrics

```text
ridge_divide_edge_count >= basin_count
mean_ridge_divide_continuity >= 0.82
min_primary_divide_continuity >= 0.68
ridge_edges_with_left_right_basin_pct >= 90
ridge_angle_histogram_max_bin_pct <= 28
```

### Valley metrics

```text
primary_valley_axis_count >= primary_basin_count
valleys_connected_to_outlet_pct == 100
valley_axis_crossing_illegal_ridge_count == 0
primary_valley_curvature_mean >= 0.11 rad/km
straight_primary_valley_pct <= 20
confluence_count >= 3 for large island preview
```

### Saddle/pass metrics

```text
named_saddle_count >= 4
pass_corridor_region_count >= 2
saddle_on_ridge_pct == 100
saddle_notch_depth_min_m >= 25
saddle_above_adjacent_basin_floor_min_m >= 40
```

### Terrace/bench/scarp metrics

```text
old_terrace_region_count >= 3
bench_count >= 8
scarp_edge_count >= 6
terrace_continuity_mean between 0.35 and 0.85
complete_circular_terrace_ring_count == 0 unless specifically debug-authorized
```

### Volcano metrics

```text
active_volcano_core_count == 1
volcano_footprint_pct_of_island_land between 1.5 and 8.0
volcano_apron_pct_of_island_land <= 14.0
volcano_attachment_edge_count between 1 and 4
volcano_gully_outside_apron_count == 0 unless explicitly attached to basin valley
volcano_disabled_region_identity_score >= 0.75
```

`volcano_disabled_region_identity_score` can be approximated by the percentage of non-volcano hard regions that remain visually/geomorphically classified as their target class after `H_volcano` is removed.

### Manifest export

Add a terrain graph section to the preview manifest:

```json
{
  "terrain_graph": {
    "seed": 123456789,
    "accepted_attempt": 4,
    "major_region_count": 24,
    "hard_region_count": 41,
    "basin_count": 7,
    "outlet_count": 6,
    "ridge_edge_count": 13,
    "valley_axis_count": 11,
    "saddle_count": 5,
    "terrace_count": 4,
    "volcano_footprint_pct": 4.6,
    "region_area_gini": 0.37,
    "mean_adjacency_degree": 3.2,
    "ridge_divide_continuity_mean": 0.86,
    "primary_valley_curvature_mean_rad_per_km": 0.19,
    "coast_gate_violations": 0,
    "regions": [
      {
        "id": 101,
        "name": "BASIN_SOUTHWEST_02",
        "class": "DrainageBasin",
        "area_km2": 86.4,
        "neighbors": [102, 203, 301],
        "outlet_node": "OUTLET_SW_02",
        "primary_valley": "VALLEY_SW_02"
      }
    ]
  }
}
```

## Files And Areas Likely Touched

Exact paths depend on the current repo layout, but this feature is likely to touch the terrain preview generator, terrain math module, preview debug outputs, and manifest export.

Likely new files:

```text
Source/JungleGame/Terrain/TerrainGraphTypes.h
Source/JungleGame/Terrain/TerrainGraphTypes.cpp
Source/JungleGame/Terrain/LandformRegionGenerator.h
Source/JungleGame/Terrain/LandformRegionGenerator.cpp
Source/JungleGame/Terrain/BasinGraphGenerator.h
Source/JungleGame/Terrain/BasinGraphGenerator.cpp
Source/JungleGame/Terrain/TerrainGraphScalarFields.h
Source/JungleGame/Terrain/TerrainGraphScalarFields.cpp
Source/JungleGame/Terrain/TerrainGraphValidation.h
Source/JungleGame/Terrain/TerrainGraphValidation.cpp
Source/JungleGame/Terrain/TerrainGraphDebugExport.h
Source/JungleGame/Terrain/TerrainGraphDebugExport.cpp
```

Likely existing areas touched:

```text
terrain preview build/generation entrypoint
height sampling pipeline
island/coast mask accessors
debug image export
preview manifest export
terrain metrics/acceptance validation
developer preview command/config
automated tests for deterministic generation
```

Likely test files:

```text
Tests/TerrainGraphDeterminismTests.cpp
Tests/LandformRegionSeedingTests.cpp
Tests/BasinGraphValidationTests.cpp
Tests/TerrainGraphCoastPreservationTests.cpp
Tests/TerrainGraphManifestTests.cpp
```

## Large PR Implementation Scope

This is large enough to justify a scoped implementation PR rather than a small patch. The recommended implementation sequence is:

### Phase 1 — Types and deterministic seed framework

Deliver:

```text
FTerrainGraphNode
FTerrainGraphEdge
FLandformRegion
FRidgeSpine
FValleyAxis
FTerrainSaddle
FTerrainTerrace
FLandformMetrics
stable hash/RNG helpers for graph generation
manifest serialization skeleton
```

Acceptance:

```text
types compile
deterministic graph seed can be created
empty graph serializes to manifest
no terrain output changes yet except optional debug section
```

### Phase 2 — Region seeding and hard ID map

Deliver:

```text
deterministic rejection sampler
minimum 24 major named regions
anisotropic weighted distance ownership
hard region ID preview map
region adjacency extraction
region metrics
```

Acceptance:

```text
same seed generates identical region map
regions are asymmetric and nonuniform
region count/area/adjacency metrics pass
coast gates unchanged
```

### Phase 3 — Basin/ridge/valley topology

Deliver:

```text
basin graph
legal coastal outlet pairing
valley axis routing
ridge divide extraction from basin boundaries
saddle/pass placement
ridge/valley/saddle debug overlays
```

Acceptance:

```text
all basins have outlet-connected valley axes
ridge divides have left/right basin ownership
no illegal graph crossings
no equal-angle fan-blade valley/ridge pattern
```

### Phase 4 — Scalar field conversion

Deliver:

```text
base region elevation blend
ridge field
valley field
saddle/pass field
terrace/bench/scarp field
debug scalar contribution atlas
```

Acceptance:

```text
height without noise reads as authored landform composition
hard IDs remain traceable
coast gates unchanged
```

### Phase 5 — Volcano subsystem attachment

Deliver:

```text
ACTIVE_VOLCANO_CORE
VOLCANO_APRON
crater/cone/apron fields
local-only volcano gullies
volcano-disabled debug map
volcano footprint metrics
```

Acceptance:

```text
volcano is compact
terrain still reads properly when volcano contribution is disabled
volcano no longer creates island-scale radial fans
```

### Phase 6 — Final validation and acceptance preview

Deliver:

```text
full manifest metrics
all required debug maps
determinism tests
coast preservation tests
region graph acceptance tests
```

Acceptance:

```text
existing coast metrics remain green
new graph metrics pass
developer debug review confirms authored-yet-deterministic terrain identity
```

## Acceptance Criteria

The implementation is accepted when all of the following are true:

### Existing world/coast gates preserved

```text
world_size_m remains 97536.0 target
shoreline_error_max_m remains 0 or effectively 0
beach_continuity_pct remains 100%
ocean_below_sea_pct remains 100%
square_edge_ocean_violations remains 0
outer world edge remains ocean/container terrain
beach and shallow shelf remain continuous
```

### Graph/region identity exists before noise

```text
24+ named major landform regions
7+ primary drainage basins
5+ coastal-flat/outlet regions
4+ upland blocks
3+ old terrace/bench provinces
2+ secondary hill provinces
2+ pass corridor regions
1 compact active volcano complex
```

### Basin/ridge/valley topology is explicit

```text
every basin has a legal outlet
every primary basin has a valley axis
ridge divides derive from basin adjacency
saddles/passes attach to ridge/divide edges
valley curves are routed through basin/outlet graph, not crater angle
```

### Height conversion is continuous and traceable

```text
hard region ID map exported
soft weights blend height smoothly
feature contribution maps exported
manifest lists regions, graph nodes, graph edges, metrics
```

### Volcano is contained

```text
active volcano footprint within 1.5%-8% of island land
volcano disabled preview still reads as complete terrain
local volcano gullies do not become island-wide radial gullies
volcano has explicit attachment edges to old terrain graph
```

### Debug evidence is present

Required outputs:

```text
region ID map
graph overlay
ridge/valley skeleton
saddle/pass map
plateau/terrace/bench/scarp map
volcano-disabled region map
coast preservation map
scalar contribution atlas
preview manifest terrain_graph section
```

## Risks And Stop Conditions

### Hard stop conditions

Stop implementation and report if any of these occur:

```text
shoreline error increases
beach continuity drops below 100%
ocean below sea drops below 100%
square-edge ocean violation appears
terrain graph modifies protected coast/shelf masks
determinism differs between two runs with the same seed/config
region seeding cannot satisfy minimum counts after max attempts
basin outlets cannot be connected without crossing illegal coast/ridge gates
volcano-disabled map proves volcano is still global terrain owner
```

### Design risks

| Risk | Why it matters | Mitigation |
|---|---|---|
| Region map too fragmented | Terrain becomes patchwork instead of landform composition. | Use major/child hierarchy and minimum area thresholds. |
| Soft blending hides IDs | Debug traceability is lost. | Preserve hard IDs separately from soft weights. |
| Basins route straight to coast | Fan-blade look returns. | Enforce curvature and outlet-sector diversity metrics. |
| Volcano regains global control | Original failure returns in another form. | Footprint cap and volcano-disabled preview. |
| Terraces become perfect rings | Looks procedural and central. | Partial continuity and noncentral scarp/bench anchors. |
| Graph edge crossings | Hydrologic skeleton becomes incoherent. | Planar edge validation and crossing rejection. |
| Metrics pass but terrain looks bad | Numeric proxy too weak. | Require debug image review checkpoints. |

## Rejected Or Forbidden Approaches

Do not use these approaches as the primary solution:

### One central volcano as global terrain owner

Rejected because it reproduces the current failure. The active volcano must be compact and attached.

### Equal-angle radial ridge or valley generation

Rejected because it creates fan-blade ridges/gullies. Ridge and valley curves must derive from basin graph and adjacency.

### Noise-first macro terrain

Rejected because noise should add detail, not decide island-scale landform composition.

### Erosion-only rescue pass

Hydraulic/fluvial erosion can improve local plausibility, but it should not be expected to invent stable named regions, debug IDs, or authored basin topology after the fact. Hydrology-based procedural terrain research supports using river/drainage graphs as modeling elements [Genevaux2013], and erosion algorithms can then refine the graph-owned terrain.

### Full circular terrace rings

Rejected unless explicitly used as a debug test. Old terraces and benches should be broken, offset, and region-owned.

### Coast-modifying terrain graph

Rejected. The graph must preserve the existing coast/beach/shelf/ocean gates.

### Player-facing navigation features

Rejected by terrain-only scope. Developer debug overlays are allowed; map/minimap/GPS/objective systems are not.

### Post-hoc semantic labels only

Rejected. If labels are assigned only after the final heightfield, they cannot guide structure and cannot solve the procedural volcano problem.

## Sources And References

[Genevaux2013] J.-D. Génevaux, É. Galin, É. Guérin, A. Peytavie, and B. Benes, “Terrain Generation Using Procedural Models Based on Hydrology,” ACM Transactions on Graphics / SIGGRAPH 2013.  
https://www.cs.purdue.edu/cgvlab/www/resources/papers/Genevaux-ACM_Trans_Graph-2013-Terrain_Generation_Using_Procedural_Models_Based_on_Hydrology.pdf

[Aurenhammer1991] F. Aurenhammer, “Voronoi Diagrams — A Survey of a Fundamental Geometric Data Structure,” ACM Computing Surveys, 1991.  
https://www.wias-berlin.de/people/si/course/files/Aurenhammer91-Voronoi.pdf

[Okabe1992] A. Okabe, B. Boots, and K. Sugihara, *Spatial Tessellations: Concepts and Applications of Voronoi Diagrams*, Wiley, 1992.  
https://books.google.com/books/about/Spatial_Tessellations.html?id=dT7YH3mjeeIC

[LabelleShewchuk2003] F. Labelle and J. R. Shewchuk, “Anisotropic Voronoi Diagrams and Guaranteed-Quality Anisotropic Mesh Generation,” 2003.  
https://people.eecs.berkeley.edu/~jrs/papers/aniso.pdf

[CGALApollonius] CGAL Documentation, “2D Apollonius Graphs (Delaunay Graphs of Disks),” additively weighted Voronoi diagrams.  
https://doc.cgal.org/latest/Apollonius_graph_2/index.html

[USGSWatersheds] U.S. Geological Survey Water Science School, “Watersheds and Drainage Basins.”  
https://www.usgs.gov/water-science-school/science/watersheds-and-drainage-basins

[IwahashiPike2007] J. Iwahashi and R. J. Pike, “Automated classifications of topography from DEMs by an unsupervised nested-means algorithm and a three-part geometric signature,” *Geomorphology*, 2007.  
https://pubs.usgs.gov/publication/70029913

[USGSGeomorphon] U.S. Geological Survey, “Elevation-Derived Hydrography Data Acquisition Specifications,” geomorphon landform classes.  
https://www.usgs.gov/ngp-standards-and-specifications/elevation-derived-hydrography-data-acquisition-specifications-16

[WeissTPI] A. D. Weiss, “Topographic Position and Landforms Analysis,” poster/paper on multi-scale Topographic Position Index and landform classification.  
https://www.jennessent.com/downloads/TPI-poster-TNC_18x22.pdf

[GRASSGeomorphon] GRASS GIS Manual, `r.geomorphon`, geomorphon landform classification.  
https://grass.osgeo.org/grass-stable/manuals/r.geomorphon.html

[Lim2022] F. Y. Lim, Y. W. Tan, and A. Bhojan, “Visually Improved Erosion Algorithm for the Procedural Generation of Tile-based Terrain,” 2022.  
https://arxiv.org/abs/2210.14496
