# Multi-Scale Terrain Detail, Feature Grammar, And Terrain Identity Research

## Executive Summary

Jungle Game already has a working deterministic island envelope: the 97.536 km world size, coast, beach, shelf, ocean, and square-edge container gates are accepted constraints and must remain hard post-composition invariants. The current failure is not coastline correctness; it is interior identity. The island reads as one procedural volcano with radial fan-blade ridges and repeated spoke gullies.

The recommended terrain identity system is a layered deterministic model:

1. **Graph, volcanology, and hydrology define terrain authority.** These systems own the island silhouette, volcanic massing, major drainage, ridgelines, basins, shelves, saddles, and region hierarchy.
2. **Feature grammar creates named landform intent.** A deterministic grammar places plateaus, benches, scarps, saddles, coastal flats, fans, floodplains, old lava fields, and secondary hills as explicit terrain entities with IDs, parents, constraints, masks, and ownership.
3. **Feature stamps implement the grammar as implicit fields.** Stamps are distance-field shapes with soft masks and typed field operations: raise, lower, flatten, terrace, incise, bevel, roughen, and preserve.
4. **Multi-scale noise adds supporting evidence only.** Noise bands are budgeted by physical wavelength and masked by feature ownership, slope, hydrology, region axis, and coastal constraints.
5. **Domain warping breaks algorithmic symmetry without becoming authority.** Warp fields must be vector fields in world/region space, not polar or center-radial artifacts. They must be low-frequency, bounded, deterministic, and separately inspectable.
6. **Preview evidence must prove structure before decoration.** Debug output must show grammar ownership, feature stamps, scale-band contribution, warp vectors, hydrology compatibility, and composition stages.

This design should make the island feel like a specific volcanic place with multiple landform systems rather than a generic procedural island. The implementation should treat terrain identity as a first-class deterministic artifact: config fingerprints, seed lineage, subsystem hashes, feature counts, feature coverage, radial-symmetry diagnostics, and preview atlases should be emitted every run.

## Terrain-Only Scope Boundary

This research/spec covers only:

- deterministic terrain synthesis;
- heightfield and mask composition;
- named landform grammar;
- terrain feature stamps;
- domain warping and scale-bounded detail fields;
- terrain identity manifests;
- preview/debug/validation outputs.

It explicitly excludes:

- foliage;
- ecology;
- weather;
- audio;
- NPCs;
- survival mechanics;
- buildings;
- final materials;
- UI;
- map, compass, minimap, GPS, markers, or player-facing navigation overlays.

The terrain system must preserve the already-working hard gates:

```text
world_size_m: 97536.0 target
shoreline_error_max_m: 0 or effectively 0
beach_continuity_pct: 100%
ocean_below_sea_pct: 100%
square_edge_ocean_violations: 0
outer world edge remains ocean/container terrain
coast/beach/shelf gates are final hard constraints after feature composition
```

## Current Procedural Identity Failure

The current interior appears too procedural because the major visible structure is dominated by one organizing center. The likely symptoms are:

- ridges radiate from the primary volcano with similar spacing;
- gullies repeat as spokes rather than forming basin-specific drainage histories;
- slope roughness is globally uniform instead of region-specific;
- detail fields visually compete with the large landforms;
- erosion-like patterns are created by noise, not by hydrology or region graph constraints;
- there is insufficient named terrain ownership: the player can see “noise on a cone,” not “old lava bench west of the high ridge, cut by younger valleys.”

The fix is not “more noise.” More octaves will usually make the terrain busier while preserving the same algorithmic authority. The fix is a deterministic identity model that produces multiple interacting landform systems with stable names, masks, debug ownership, and structural validation.

## Terrain Identity Goals

The island should be recognizable from repeated previews and should support phrases like:

- “the north-east dissected highland”;
- “the old south-west lava field”;
- “the western bench-and-scarp coast”;
- “the inner saddle between the main edifice and secondary hills”;
- “the broad floodplain below the split valley”;
- “the low coastal flat behind the beach shelf.”

Terrain identity goals:

| Goal | Required implementation behavior | Required debug proof |
|---|---|---|
| Preserve accepted island metrics | Reapply coast/beach/shelf/ocean constraints after all feature/detail passes | Final constraint delta map and acceptance gate report |
| Replace radial sameness | Use basin graph, region axes, secondary centers, and non-radial warp fields | Radial symmetry score and basin ownership map |
| Create named landforms | Emit deterministic feature records with stable IDs and labels | Feature ownership map and feature stamp atlas |
| Separate structure from decoration | Track contribution by composition stage and physical scale band | Stage previews and scale-band contribution atlas |
| Avoid detail mush | Budget amplitude by wavelength, slope, mask, and authority | Per-band energy metrics and slope-delta histograms |
| Keep hydrology plausible | Stamps must preserve or explicitly reroute drainage | Flow accumulation before/after and channel conflict report |
| Support future iteration | Hash every subsystem and config input | Terrain identity manifest with fingerprints |

## Detail Is Not Terrain Authority

### Authority split

Terrain authority should be explicitly partitioned:

| Terrain element | Primary source | Allowed detail support | Forbidden shortcut |
|---|---|---|---|
| Island footprint, coast, beach, shelf, ocean | existing deterministic coast/shelf system | no detail displacement except validated sub-meter visual material later | noise-driven coast mutation |
| Main volcanic mass | volcanology envelope and region graph | broad roughness and lava-sector anisotropy | one radial cone plus ridged fBm |
| Major ridges | graph, erosion, volcanology, watershed boundaries | crest breakup, asymmetric shoulders, local scarps | global ridged noise pretending to be ridgelines |
| Main valleys/channels | hydrology/erosion graph | bank roughness, tributary wrinkles, terrace micro-variation | noise gullies with no contributing area |
| Plateaus/benches/scarps | feature grammar stamps plus slope/region constraints | bevel noise, talus roughness, edge breakup | random flattened blobs |
| Fans/floodplains | hydrology plus depositional grammar | distributary texture, lobe boundary roughness | radial fan texture everywhere |
| Old lava fields | volcanology sector grammar | flow lobe roughness, levees, pressure-ridge bands | random black-noise roughness field |
| Secondary hills | region graph and volcanic/erosional remnant rules | asymmetric hill surface detail | cloned conelets or repeated bumps |

### Rule

Noise and domain warping may change **texture, roughness, edges, local relief, and mask transitions**. They must not create the primary readable terrain skeleton. The preview pipeline must make this auditable by showing terrain with all detail bands disabled.

### Practical enforcement

Implement each detail band with these fields:

```cpp
struct FNoiseScaleBand
{
    FName BandId;
    float MinWavelengthM;
    float MaxWavelengthM;
    float MaxAmplitudeM;
    float MaxSlopeDeltaDeg;
    int32 OctaveCount;
    float Lacunarity;
    float Gain;
    FName NoiseBasis;              // Gradient, Value, Simplex, Cellular, Ridged, CurlDerived
    FName AuthorityClass;          // StructuralSupport, EdgeBreakup, SurfaceRoughness, PreviewOnly
    TArray<FName> AllowedFeatureTypes;
    TArray<FName> RequiredMasks;
    TArray<FName> SuppressedMasks;
    bool bCanModifyHydrology;
    bool bCanModifyCoastBand;
    uint64 Seed;
    FString ConfigHash;
};
```

`bCanModifyHydrology` should default to `false`. `bCanModifyCoastBand` should always be `false` for the accepted coast/beach/shelf system unless a later explicit coastal PR reopens those gates.

## Physical Scale Bands For Detail

For a 97.536 km island, scale bands should be expressed in meters and tied to physical landform roles. Do not expose only octave counts; expose wavelength ranges and amplitude caps.

### Recommended bands

| Band | Wavelength | Max height amplitude | Authority | Typical uses | Notes |
|---|---:|---:|---|---|---|
| A. Island envelope | 24,000-97,536 m | structural | coast/volcanology graph | island silhouette, ocean container, main shield volume | Not a noise detail band |
| B. Macro regions | 8,000-24,000 m | structural | graph/volcanology | primary sectors, large ridges, amphitheater basins | Not decorative noise |
| C. Basin and ridge systems | 2,000-8,000 m | structural | hydrology/erosion/feature grammar | watersheds, ridge families, secondary massifs | Noise only as bounded support |
| D. Large feature stamps | 512-2,000 m | 20-120 m depending on feature | grammar/stamps | plateaus, benches, scarps, saddles, lava lobes, fans | Explicit feature records required |
| E. Mesoscale terrain evidence | 128-512 m | 4-35 m | detail support | ravine shoulders, lava-field lobes, bench edge breakup, talus texture | Must be masked to features/regions |
| F. Traversal-scale relief | 32-128 m | 0.75-8 m | detail support | local undulations, small gullies, levee roughness, floodplain swales | Must not introduce false hydrology |
| G. Fine terrain relief | 8-32 m | 0.1-1.5 m | limited terrain detail | small rockiness, subtle terrace irregularity | Only if represented in terrain resolution/virtual heightfield |
| H. Material/render micro | <8 m | material only | material system later | surface grain, root-scale roughness | Out of scope for terrain authority |

### Octave budgeting

Recommended octave rules:

- Budget octaves by wavelength, not by aesthetic count.
- Do not sample below twice the heightfield spacing or preview pixel size. Respect Nyquist: if the preview cannot resolve a wavelength, do not treat it as validation evidence.
- Every octave must declare its target physical wavelength and contribution cap.
- Macro bands A-C should not be generated as raw fBm. They should be graph, hydrology, volcanology, and grammar outputs.
- Detail bands E-G may use fBm-style layering, but only inside feature/region masks and only with bounded amplitude.
- Ridged variants must require an explicit `AllowedFeatureTypes` mask.

Example budget:

```text
WorldSizeM = 97536
PreviewHighResCellM = 24
TerrainWorkingCellM = 12 or 24 depending on generator budget

Band E: 512, 256, 128 m wavelengths; 3 octaves; max amplitude 35/18/9 m
Band F: 128, 64, 32 m wavelengths; 3 octaves; max amplitude 8/4/1.5 m
Band G: 32, 16, 8 m wavelengths; 3 octaves; max amplitude 1.5/0.6/0.2 m
```

A simple validation check should fail a config where total detail amplitude in a region exceeds a fraction of the parent feature relief. Recommended starting limit:

```text
sum_detail_abs_amplitude_m <= min(0.20 * parent_feature_relief_m, 45 m)
```

For floodplains and coastal flats, this cap should be far lower:

```text
floodplain_detail_abs_amplitude_m <= 2.5 m
coastal_flat_detail_abs_amplitude_m <= 1.5 m
beach/shelf/ocean_detail_abs_amplitude_m = 0 m unless handled by final material/water systems outside this scope
```

## Domain Warping And Vector Fields

Domain warping is useful when it breaks linear or grid-like sameness, but it is dangerous when it hides weak structure or creates new artifacts. The system should implement warp fields as deterministic, inspectable vector fields with physical amplitude limits.

### Core rules

1. **Do not use polar angle as a warp basis.** Any formula based on `theta = atan2(y - center.y, x - center.x)` risks circular spokes and rings.
2. **Do not reuse the volcano center as the universal warp origin.** Region-local anchors, basin axes, and independent low-frequency fields are safer.
3. **Use at least two independent vector fields.** One broad island/region warp and one local feature warp, with different seeds and wavelengths.
4. **Bound warp displacement by target wavelength.** Warp magnitude should normally be `0.05-0.25 * target_wavelength_m`; higher values can fold features into mush.
5. **Pin or fade warps near accepted constraints.** Coast/beach/shelf/ocean gates must be reasserted after composition.
6. **Emit a warp-vector map.** If a reviewer can see radial arrows or circular bands, the warp fails.

### Recommended data model

```cpp
struct FDomainWarpField
{
    FName WarpId;
    uint64 Seed;
    float MinWavelengthM;
    float MaxWavelengthM;
    float MaxDisplacementM;
    float DivergenceWeight;        // broad compression/expansion
    float CurlWeight;              // swirl/shear without sink/source dominance
    float DirectionalBiasWeight;   // align to region axis where appropriate
    FVector2D DirectionalBiasUnit;
    FName CoordinateSpace;         // WorldXY, RegionLocal, BasinLocal, FeatureLocal
    TArray<FName> ActiveRegionIds;
    TArray<FName> SuppressedMasks;
    bool bFadeNearCoast;
    float CoastFadeStartM;
    float CoastFadeEndM;
    bool bDebugExportVectors;
    FString ConfigHash;
};
```

### Warp generation pattern

A robust pattern is:

```text
base_position = world_xy
region_position = TransformWorldToRegionBasis(world_xy, region_axis, region_origin)

broad_vec = CurlNoise2D(region_position / 12000m, seedA) * 900m
shear_vec = GradientNoise2D(world_xy / 6000m, seedB) * 450m
feature_vec = AnisotropicNoise2D(feature_local_xy / 900m, seedC, feature_axis) * 90m

warp = coast_fade * region_mask * (broad_vec + shear_vec + feature_vec)
warped_sample_position = world_xy + warp
```

The key is that the warp vector is not derived from the same radial coordinate system as the volcano. Curl components are useful because they create lateral shearing and path bending without simply pulling everything toward or away from one center. Gradient/divergence components can be used sparingly for broad compression, but they should be region-local and bounded.

### Anisotropic detail alignment

Detail should often be anisotropic rather than isotropic:

| Alignment source | Suitable features | Implementation hint |
|---|---|---|
| Ridge graph tangent | ridge shoulders, scarps, benches | stretch detail along ridge tangent, compress across slope |
| Valley centerline tangent | floodplains, terraces, alluvial fans | longitudinal low-amplitude undulation plus cross-valley terrace steps |
| Prevailing slope | talus, gullies, lava flow texture | align streaks downslope but suppress false channels |
| Lava flow axis | old lava fields, lobes, levees | elongated noise in flow direction with lobe edge masks |
| Region axis | broad sectors, old massif remnants | rotate noise basis per region to avoid global repetition |

Recommended anisotropic sampler fields:

```cpp
struct FAnisotropicDetailBasis
{
    FName BasisId;
    FVector2D Origin;
    FVector2D AxisU;        // along feature/region
    FVector2D AxisV;        // across feature/region
    float StretchU;
    float StretchV;
    float RotationJitterDeg;
    uint64 Seed;
};
```

## Feature Grammar Overview

Feature grammar is the terrain equivalent of a style/shape grammar: a rule system that expands high-level region intent into specific child landforms. It should not be a random scatter pass. Each placed feature should have a reason, a parent, constraints, and debug identity.

Shape grammars and L-systems demonstrate that recognizable synthetic structure comes from rule-based expansion, hierarchy, and context-sensitive production rather than independent random events. Terrain grammar should apply the same principle spatially: regions produce landform candidates, candidates are constrained by masks and graph relationships, and accepted features receive stable IDs.

### Proposed grammar hierarchy

```text
IslandIdentity
  -> VolcanicSectors
    -> PrimaryHighlandSector
      -> RidgeFamilies
      -> DissectedValleys
      -> HighBenches
    -> OldLavaSector
      -> LavaLobes
      -> Levees
      -> PressureRidgeBands
    -> CoastalLowlandSector
      -> CoastalFlats
      -> Fans
      -> Floodplains
    -> SecondaryHillSector
      -> RemnantHills
      -> Saddles
      -> MinorScarps
```

### Recommended `FTerrainFeatureGrammar`

```cpp
struct FTerrainFeatureGrammar
{
    FName GrammarId;
    uint64 RootSeed;
    FString Version;
    FString ConfigHash;

    TArray<FName> RegionIds;
    TArray<FRegionFeatureRule> Rules;
    TMap<FName, float> FeatureTypeBudgetsByAreaPct;
    TMap<FName, int32> MinFeatureCounts;
    TMap<FName, int32> MaxFeatureCounts;

    bool bEmitRejectedCandidates;
    bool bEmitFeatureOwnershipMap;
    bool bFailOnUnownedInterior;
};
```

### Recommended `FRegionFeatureRule`

```cpp
struct FRegionFeatureRule
{
    FName RuleId;
    FName ParentRegionType;
    FName OutputFeatureType;
    int32 MinCount;
    int32 MaxCount;
    float TargetAreaPctMin;
    float TargetAreaPctMax;

    float MinElevationM;
    float MaxElevationM;
    float MinSlopeDeg;
    float MaxSlopeDeg;
    float MinDistanceToCoastM;
    float MaxDistanceToCoastM;
    float MinDistanceToMainChannelM;
    float MaxDistanceToMainChannelM;
    float MinDistanceToVolcanicCenterM;
    float MaxDistanceToVolcanicCenterM;

    FName RequiredParentFeatureType;
    TArray<FName> SuppressedOverlapFeatureTypes;
    float MaxOverlapPct;
    float Priority;

    FName PlacementSampler;     // Poisson, StratifiedByBasin, AlongPolyline, RegionInterior, ValleyMouth
    FName ShapeSampler;         // EllipseSDF, PolylineBufferSDF, LobeSDF, RidgeRibbonSDF, TerraceBandSDF
    uint64 RuleSeed;
};
```

### Grammar outputs

The grammar should emit a deterministic feature table:

```text
FeatureId, FeatureType, ParentRegionId, ParentFeatureId, Seed, AnchorXY,
AreaM2, ReliefM, MeanElevationM, MeanSlopeDeg, Priority,
HydrologyPolicy, CoastPolicy, ShapeParamsHash, Accepted/RejectedReason
```

Rejected candidates are valuable debug evidence. They prove the grammar is constrained instead of arbitrary.

## Feature Stamp Data Model

A feature stamp is an implicit terrain operation with a soft mask. It is not a pasted heightmap decal. Each stamp should have a signed/unsigned distance field, falloff, field operation, composition priority, hydrology policy, and ownership metadata.

### Recommended `FTerrainFeatureStamp`

```cpp
enum class ETerrainStampOp : uint8
{
    Raise,
    Lower,
    FlattenToPlane,
    BlendToProfile,
    Incise,
    Terrace,
    Scarp,
    Saddle,
    Roughen,
    Preserve
};

enum class ETerrainHydrologyPolicy : uint8
{
    PreserveExistingFlow,
    RerouteWithValidation,
    CreateDepositionalSurface,
    CreateIncision,
    NoChannelModification
};

struct FTerrainFeatureStamp
{
    FName FeatureId;
    FName FeatureType;
    FName ParentRegionId;
    FName ParentFeatureId;
    uint64 Seed;

    FVector2D AnchorXY;
    FVector2D AxisU;
    FVector2D AxisV;
    float LengthM;
    float WidthM;
    float InnerRadiusM;
    float OuterFalloffM;

    FName SdfType;              // Ellipse, Capsule, PolylineRibbon, Lobe, Polygon, RingSegment, SaddlePair
    FName ProfileType;          // Plane, Convex, Concave, LongitudinalFan, LavaLobe, TerraceSteps
    ETerrainStampOp Operation;
    ETerrainHydrologyPolicy HydrologyPolicy;

    float TargetElevationM;
    float RelativeRaiseM;
    float RelativeLowerM;
    float MaxSlopeDeltaDeg;
    float BlendStrength;
    float EdgeBevelM;
    float InteriorRoughnessM;
    float EdgeRoughnessM;

    float Priority;
    float MaxOverlapPct;
    TArray<FName> RequiredMasks;
    TArray<FName> SuppressedMasks;

    bool bProtectedCoastAware;
    float CoastProtectionDistanceM;
    bool bEmitDebugMask;
    FString ShapeParamsHash;
    FString ConfigHash;
};
```

### Stamp field formula

A typical stamp should compute:

```text
sdf = ShapeDistance(world_xy, stamp_shape)
inner_mask = smoothstep(outer_falloff, 0, sdf)
edge_mask = EdgeBand(sdf, edge_width)
constraint_mask = region_mask * coast_safe_mask * hydrology_safe_mask * slope_allowed_mask
stamp_mask = inner_mask * constraint_mask

candidate_height = ApplyProfile(base_height, world_xy, stamp_profile)
height_out = BlendByOperation(base_height, candidate_height, stamp_mask, priority)
```

The stamp should also output ownership:

```text
owner_feature_id = feature_id when stamp_mask > ownership_threshold
owner_strength = stamp_mask
operation_contribution_m = height_out - height_in
```

### Soft masks and conflict control

Use soft masks for visual blending, but hard constraints for protected systems:

- **Soft:** transition between bench and slope, lava lobe and surrounding terrain, fan boundary.
- **Hard:** ocean container, beach continuity, shelf validity, square-edge ocean, forbidden channel blockages.

Overlaps should be resolved by typed priority, not arbitrary order:

```text
ProtectedCoast > MainHydrology > StructuralFeature > DepositionalFeature > EdgeBreakup > Roughness
```

## Plateaus, Benches, Scarps, Saddles, Terraces, And Flats

### Plateaus

Plateaus should be region-specific, not random flattened ellipses.

Placement constraints:

- parent region: highland sector, old lava sector, or secondary hill sector;
- slope before stamping: generally below 18 degrees for broad plateaus, below 25 degrees for smaller remnant caps;
- distance from major channels: avoid blocking high-flow channels unless the plateau is explicitly an older surface incised by valleys;
- aspect/axis: align to region or ridge axis, not world north by default;
- count: 2-6 significant plateau features for a 97.536 km island, depending on sector layout.

Operations:

- `FlattenToPlane` or `BlendToProfile` with very low interior roughness;
- `EdgeBevelM` 80-350 m;
- edge breakup using Band E, not Band F/G only;
- post-stamp incision if a valley crosses the old surface.

Debug proof:

- plateau mask atlas;
- before/after slope map;
- drainage preservation map;
- plateau elevation histogram.

### Benches

Benches are narrow-to-broad shelf-like surfaces on slopes or coasts. They should be common enough to break cone smoothness.

Placement constraints:

- along ridge shoulders, valley walls, or old lava margins;
- length 800-6,000 m;
- width 80-800 m;
- elevation bands should vary by sector;
- avoid perfectly concentric rings around the volcano.

Operations:

- project a local plane/profile along a polyline ribbon;
- blend outer edge into parent slope;
- add scarp on one side where geologically plausible;
- add low-amplitude anisotropic roughness along the bench axis.

### Scarps

Scarps are linear/semi-linear slope breaks. They are important for identity because they create named edges and shadow structure.

Placement constraints:

- along plateau edges, lava field margins, valley sides, or coastal erosion margins;
- length 500-8,000 m;
- height change 10-180 m;
- must not form a perfect radial spoke family;
- should intersect other features in controlled ways.

Operations:

- `Scarp` op applies asymmetric cross-section;
- crest edge: roughened and broken;
- foot: optional talus apron mask;
- preserve hydrology by cutting notches where channels cross.

### Saddles

Saddles connect or separate terrain masses. They are critical for breaking the “single volcano cone” read.

Placement constraints:

- between main edifice and secondary hills;
- between adjacent ridge families;
- above major basin heads;
- avoid coastal protected band unless explicitly a low pass behind coastal flats.

Operations:

- `Saddle` op blends two descending profiles with a pass elevation;
- maintain or create watershed split line;
- stamp should emit a watershed-boundary diagnostic.

### Terraces

Terraces should be tied to valleys, floodplains, old shore levels, lava benches, or erosional/depositional stages.

Placement constraints:

- along valley centerlines or coastal lowland margins;
- stepped bands parallel to valley or coast axis;
- limited vertical spacing: 2-15 m for floodplain terraces, 10-60 m for older erosional/lava terraces;
- never global contour rings.

Operations:

- quantized profile only inside a terrace mask;
- add per-segment jitter to avoid perfect steps;
- incise active channel after terrace creation.

### Coastal flats

Coastal flats must respect final coast/beach/shelf gates.

Placement constraints:

- landward of accepted beach/coast masks;
- broad low-slope zones near valley mouths or old lava margins;
- no mutation of final shoreline;
- slope generally below 3 degrees after composition.

Operations:

- flatten/blend landward terrain only;
- tie inland boundary to fans/floodplains;
- detail amplitude no more than 1.5 m unless outside the protected coastal band.

## Coastal Fans, Floodplains, And Valley Floor Features

### Coastal fans

Coastal fans should form where valleys exit steep terrain into lower-gradient coastal zones. They should not be radial decoration.

Placement constraints:

- anchor at valley mouth or channel gradient break;
- fan axis follows downstream flow direction;
- lobe angle and radius depend on contributing area and local coastal plain width;
- avoid overlap with beach/shelf protected masks;
- count should be tied to actual drainage basins, not a fixed radial count.

Operations:

- `CreateDepositionalSurface` profile: convex-up/downstream fan surface;
- distributary swale detail at low amplitude;
- active channel incises or crosses the fan after fan creation;
- fan toe fades before protected beach/shelf.

Recommended fan parameters:

```text
radius_m: 400-3500
arc_degrees: 25-95
relief_m: 3-60
surface_slope_deg: 0.5-6
max_detail_amplitude_m: 0.3-3.0
```

### Floodplains

Floodplains should be valley-floor features generated from hydrology and local slope.

Placement constraints:

- high contributing area;
- low local gradient;
- valley width estimate above threshold;
- not on steep volcanic flanks;
- linked to main channel ID.

Operations:

- flatten to a valley-axis longitudinal profile;
- add subtle scroll/swale detail if desired;
- preserve active channel depression;
- terrace older floodplain margins.

### Valley floors

Valley floors should be generated after major valley incision but before fine surface detail.

Recommended operations:

```text
1. derive valley centerline and width from flow accumulation + slope confinement;
2. create valley-floor mask using distance to channel and valley wall slope;
3. blend floor to longitudinal channel profile;
4. apply terrace bands to margins;
5. re-run flow validation;
6. apply only low-amplitude detail.
```

Debug proof:

- valley centerline map;
- floodplain/fan ownership map;
- flow accumulation before and after;
- blocked-channel report.

## Old Lava Fields And Secondary Hill Features

### Old lava fields

Old lava fields should create non-radial terrain identity while remaining compatible with the volcanic island premise. They are not just rough material zones; they are heightfield features.

Placement constraints:

- parent: old lava sector or flank sector;
- source may be secondary vent, flank fissure, or older highland breach;
- flow axis follows plausible downslope route but can be partially eroded;
- avoid beach/shelf mutation;
- should be cut by younger drainage in places.

Operations:

- lobe-shaped SDFs chained along a flow path;
- levee ribbons on lobe margins;
- pressure-ridge bands transverse or oblique to flow direction;
- roughness stronger at lobe margins and old broken surfaces;
- younger channels incise through lobes.

Recommended parameters:

```text
lobe_length_m: 1500-9000
lobe_width_m: 250-1800
lobe_relief_m: 5-90
levee_height_m: 1-12
pressure_ridge_wavelength_m: 80-350
interior_roughness_m: 1-12
```

### Secondary hills

Secondary hills break the single-cone read. They can represent older vents, erosional remnants, or uplifted/isolated highland blocks.

Placement constraints:

- distance from primary volcanic center: generally 12-42 km;
- avoid uniform circular ring distribution;
- tied to region graph, old lava sectors, or ridge intersections;
- 3-9 secondary hill systems for a 97.536 km island, not dozens of noise bumps.

Operations:

- broad structural mass from graph/grammar;
- asymmetric profile, not cloned cones;
- saddles connecting to or separating from main highland;
- local radial detail allowed only within that hill’s own coordinate system and with asymmetry.

Debug proof:

- secondary hill ID map;
- hill profile asymmetry report;
- inter-hill distance/angle distribution;
- disabled-detail structural preview.

## Deterministic Identity And Config Fingerprints

Every terrain run should emit a manifest that makes identity reproducible and debuggable. The manifest should include both configuration fingerprints and generated feature lineage.

### Recommended `FTerrainIdentityMetrics`

```cpp
struct FTerrainIdentityMetrics
{
    FString TerrainBuildId;
    FString RootConfigHash;
    uint64 RootSeed;

    FString CoastSubsystemHash;
    FString VolcanologySubsystemHash;
    FString HydrologySubsystemHash;
    FString FeatureGrammarHash;
    FString DomainWarpHash;
    FString NoiseBandHash;
    FString CompositionOrderHash;

    int32 RegionCount;
    int32 FeatureStampCount;
    int32 AcceptedFeatureCount;
    int32 RejectedFeatureCandidateCount;
    TMap<FName, int32> FeatureCountByType;
    TMap<FName, float> FeatureCoveragePctByType;
    float OwnedInteriorPct;
    float UnownedInteriorPct;

    float RadialSymmetryScore;
    float RingArtifactScore;
    float RepeatedSpokeScore;
    float DetailEnergyStructuralRatio;
    float HydrologyConflictCount;
    float CoastConstraintDeltaMaxM;

    float ShorelineErrorMaxM;
    float BeachContinuityPct;
    float OceanBelowSeaPct;
    int32 SquareEdgeOceanViolations;
};
```

### Manifest requirements

Emit a JSON or TOML manifest with:

- root seed;
- build timestamp only as metadata, not as generation input;
- generator version;
- all subsystem config hashes;
- accepted hard gates;
- feature records;
- rejected candidate records;
- per-band amplitude/energy metrics;
- preview image filenames and hashes;
- any stop-condition warnings.

Example manifest excerpt:

```json
{
  "terrain_build_id": "jg_terrain_005_a13f...",
  "world_size_m": 97536.0,
  "root_seed": 1844674407370951,
  "subsystems": {
    "coast": "sha256:...",
    "volcanology": "sha256:...",
    "hydrology": "sha256:...",
    "feature_grammar": "sha256:...",
    "domain_warp": "sha256:...",
    "noise_bands": "sha256:..."
  },
  "hard_gates": {
    "shoreline_error_max_m": 0.0,
    "beach_continuity_pct": 100.0,
    "ocean_below_sea_pct": 100.0,
    "square_edge_ocean_violations": 0
  },
  "identity_metrics": {
    "region_count": 14,
    "feature_stamp_count": 86,
    "owned_interior_pct": 92.4,
    "radial_symmetry_score": 0.18,
    "repeated_spoke_score": 0.11
  }
}
```

### Stable IDs

Feature IDs should be deterministic from semantic lineage, not array order:

```text
FeatureId = Hash64(RootSeed, GrammarVersion, ParentRegionId, RuleId, CandidateOrdinal, QuantizedAnchorXY, FeatureType)
```

Do not include timestamps, thread order, container iteration order, or floating-point string formatting differences in IDs.

## Composition Order And Conflict Resolution

Recommended composition order:

```text
0. Load root config and seeds.
1. Generate/protect existing coast, beach, shelf, ocean, and container masks.
2. Generate base island envelope and volcanic macro regions.
3. Generate region graph: sectors, basins, ridges, saddles, secondary mass anchors.
4. Generate primary hydrology and erosion authority: major channels, watersheds, valley corridors.
5. Run feature grammar candidate placement.
6. Validate candidates against region, coast, slope, hydrology, and overlap constraints.
7. Apply structural feature stamps: secondary hills, plateaus, large benches, major scarps, saddles.
8. Re-run or update hydrology through structural features.
9. Apply depositional/incision features: fans, floodplains, terraces, old lava lobes, levees.
10. Validate hydrology conflicts and coast protection.
11. Apply domain-warped detail bands E-G inside allowed masks only.
12. Reapply final coast/beach/shelf/ocean hard constraints.
13. Emit metrics, manifests, and debug atlases.
```

### Conflict resolution

Conflicts should be resolved by typed policy:

| Conflict | Resolution |
|---|---|
| Feature overlaps protected coast/beach/shelf/ocean | Clip or reject feature; never mutate accepted final constraints |
| Stamp blocks main channel | Cut channel notch, reroute with validation, or reject stamp |
| Plateau overlaps floodplain | Higher-priority older plateau may exist only if floodplain/channel incises it; otherwise reject overlap |
| Lava lobe crosses valley | Allow if younger lobe dams/reroutes with explicit validation, or older lobe is incised by valley |
| Scarps create excessive slope | Clamp slope delta or widen bevel; report max slope delta |
| Detail band creates false channels | Suppress detail in hydrology masks or lower amplitude |
| Multiple features compete | Use priority plus semantic rules, not arbitrary application order |

### Coast preservation pattern

The coast/beach/shelf system must be final authority. Use this invariant:

```text
height_after_all_features = ComposeInteriorFeatures(...)
height_after_final_constraints = ApplyCoastBeachShelfOceanHardConstraints(height_after_all_features)
validate_final_constraints(height_after_final_constraints) must pass
```

The debug report must include the delta between pre-final and post-final coastal enforcement. A large delta indicates feature/detail systems are pushing into protected space and should be fixed even if the final hard gate passes.

## Preview / Debug Outputs

The preview pipeline should separate structural terrain from decorative detail. Required debug outputs:

| Output | Purpose | Failure it catches |
|---|---|---|
| `composition_stage_00_coast.png` | accepted coast/beach/shelf/ocean baseline | broken preserved gates |
| `composition_stage_01_volcanology.png` | base volcanic/region authority | weak base structure |
| `composition_stage_02_hydrology.png` | watersheds/channels/valley corridors | fake/noisy drainage |
| `composition_stage_03_feature_stamps.png` | structural and depositional feature effects | random blobs or overpowered stamps |
| `composition_stage_04_detail_bands.png` | detail-only contribution | detail replacing landforms |
| `composition_stage_05_final_constraints.png` | post-hard-gate terrain | coast mutation |
| `feature_stamp_atlas.png` | all accepted/rejected stamp masks and IDs | invisible/overlapping features |
| `feature_ownership_map.png` | dominant feature/region owner per pixel | unowned interior mush |
| `scale_band_contribution_atlas.png` | per-band signed contribution | overactive octaves |
| `warp_vector_map.png` | arrows/streamlines of warp vectors | radial/circular warp artifacts |
| `hydrology_conflict_map.png` | blocked/rerouted/validated flow | stamp-channel conflicts |
| `radial_symmetry_diagnostic.png` | angular spectrum/spoke score around main center | repeated radial spokes |
| `slope_delta_by_stage.png` | slope change per pass | excessive cliffs/noise |
| `coast_constraint_delta.png` | final coast enforcement delta | features invading protected coast |

### Preview modes

Add toggles:

```text
PreviewMode=StructuralOnly        // no detail bands, no fine roughness
PreviewMode=FeatureOnly           // feature stamps on base terrain, no noise detail
PreviewMode=DetailOnlyDelta       // signed detail contribution around zero
PreviewMode=WarpVectors           // vector visualization only
PreviewMode=HydrologyValidation   // flow accumulation/channel conflicts
PreviewMode=FinalBeautyTerrain    // full terrain, still terrain-only
```

The most important reviewer view is **StructuralOnly**. If the island still reads as a generic cone without details, the feature grammar is not doing enough.

## Validation Metrics

### Hard preservation metrics

These are non-negotiable:

```text
world_size_m == 97536.0 target
shoreline_error_max_m == 0 or effectively 0
beach_continuity_pct == 100%
ocean_below_sea_pct == 100%
square_edge_ocean_violations == 0
outer world edge remains ocean/container terrain
```

### Identity metrics

Recommended additional metrics:

| Metric | Target / interpretation |
|---|---|
| `RegionCount` | enough regions to avoid single-authority island; likely 10-24 named terrain regions |
| `FeatureStampCount` | enough accepted features for identity; likely 40-160 depending on granularity |
| `FeatureCoveragePctByType` | plateaus/benches/scarps/fans/floodplains/lava fields/hills all represented where intended |
| `OwnedInteriorPct` | target > 85%; low value means unstructured noise terrain |
| `UnownedInteriorPct` | target < 15%; investigate large unowned blobs |
| `DetailEnergyStructuralRatio` | target < 0.20 globally; lower in floodplains/coastal flats |
| `RadialSymmetryScore` | should drop from current baseline; exact threshold depends on current diagnostic implementation |
| `RepeatedSpokeScore` | should drop materially from current baseline |
| `RingArtifactScore` | should be near zero outside intentional crater/bench contexts |
| `HydrologyConflictCount` | zero unresolved major conflicts |
| `CoastConstraintDeltaMaxM` | near zero; large values mean interior features are invading protected masks |
| `RejectedFeatureCandidateCount` | should be non-zero in constrained grammars; zero can mean rules are not actually constrained |

### Radial artifact diagnostics

Implement a diagnostic around the primary volcanic center:

```text
1. Convert height/slope/ridge masks into polar bins around the main center for diagnostics only.
2. Compute angular energy spectrum for ridge/gully-like masks.
3. Detect repeated high-energy angular peaks.
4. Compare against basin/region ownership; peaks explained by named basins are less concerning.
5. Report RepeatedSpokeScore and RingArtifactScore.
```

This does not mean generation should use polar coordinates. It means diagnostics can measure the artifact.

### DEM-style validation concepts

DEM validation practice commonly compares elevation products against independent reference points or surfaces using error metrics such as RMSE and spatial error distributions. Jungle Game does not have a real DEM target, so use DEM-style thinking internally:

- compare each stage against the previous stage;
- measure signed elevation deltas, slope deltas, curvature deltas, and drainage changes;
- report spatial clustering of excessive deltas;
- validate that high-frequency detail does not dominate low-frequency structure;
- preserve known-control masks such as coast, beach, shelf, ocean, and container edges.

## Files And Areas Likely Touched

Exact paths depend on the current repo layout. Likely areas:

| Area | Likely additions/changes |
|---|---|
| Terrain config schema | add `FNoiseScaleBand`, `FDomainWarpField`, `FTerrainFeatureGrammar`, `FRegionFeatureRule`, `FTerrainFeatureStamp` |
| Terrain generator core | add composition stages, feature stamp evaluator, conflict resolver |
| Region/graph generation | add named region IDs, axes, parent-child ownership |
| Hydrology/erosion module | expose channel masks, basin IDs, valley width, flow accumulation for feature constraints |
| Volcanology module | expose sector IDs, lava source candidates, old/new lava field masks |
| Preview/debug renderer | add stage previews, stamp atlas, scale-band atlas, warp-vector map, ownership map |
| Metrics/manifest writer | add `FTerrainIdentityMetrics`, subsystem hashes, feature table, validation output |
| Tests/fixtures | deterministic seed tests, coast preservation tests, radial artifact regression test |

Potential Unreal-facing files if the project uses standard naming:

```text
Source/JungleGame/Terrain/TerrainGenerator.*
Source/JungleGame/Terrain/TerrainConfig.*
Source/JungleGame/Terrain/TerrainFeatureGrammar.*
Source/JungleGame/Terrain/TerrainFeatureStamp.*
Source/JungleGame/Terrain/TerrainDomainWarp.*
Source/JungleGame/Terrain/TerrainIdentityMetrics.*
Source/JungleGame/Terrain/TerrainPreviewDebug.*
Config/Terrain/*.json or *.ini
Saved/TerrainDebug/<BuildId>/manifest.json
Saved/TerrainDebug/<BuildId>/*.png
```

## Large PR Implementation Scope

A large implementation PR should be split internally into reviewable commits or modules, but it can still land as one PR if the repo process requires batch delivery.

Recommended scope:

1. **Data model and config parsing**
   - Add structs/enums for grammar, rules, stamps, scale bands, warp fields, and identity metrics.
   - Add config hashing and seed lineage.

2. **Feature grammar placement**
   - Generate deterministic feature candidates from region rules.
   - Validate against slope, elevation, coast, hydrology, overlap, and feature budgets.
   - Emit accepted and rejected candidate tables.

3. **Feature stamp evaluator**
   - Implement SDF stamp shapes and field operations.
   - Add ownership, priority, and conflict resolution.
   - Add hydrology/coast protection masks.

4. **Domain warp and detail bands**
   - Implement bounded vector warp fields.
   - Implement physical-scale noise bands with masks and contribution tracking.
   - Add anisotropic detail basis support.

5. **Preview and metrics**
   - Add stage previews, atlases, warp-vector map, feature ownership map, and manifest.
   - Add radial artifact and detail-energy diagnostics.
   - Re-run accepted coast/beach/shelf/ocean hard gates after all passes.

6. **Validation tests**
   - Determinism test: same config/seed gives same manifest hashes and feature table.
   - Coast preservation test: final constraints remain accepted.
   - Detail authority test: disabling detail bands preserves named structural landforms.
   - Warp artifact test: no excessive radial/ring score regression.

## Acceptance Criteria

The PR/research implementation is acceptable when:

- final Markdown/spec work is represented in implementation notes or docs;
- all existing coast/beach/shelf/ocean gates still pass exactly or effectively exactly;
- terrain can be previewed with structural-only, feature-only, detail-only, and final modes;
- the generator emits a deterministic terrain identity manifest;
- feature grammar produces named accepted feature records for plateaus, benches, scarps, saddles, coastal flats, fans, floodplains, old lava fields, and secondary hills;
- feature stamps use distance fields/soft masks with explicit operation types and ownership;
- detail bands are budgeted by physical wavelength and amplitude;
- warp fields are vector maps, not hidden scalar hacks;
- coast/beach/shelf constraints are re-applied and reported after all composition;
- preview evidence shows multiple designed landform systems without relying on final materials or foliage;
- radial fan-blade/spoke artifacts are reduced relative to the current baseline;
- hydrology conflicts are zero or explicitly resolved and reported;
- deterministic reruns produce the same feature IDs, metrics, and preview hashes for the same config/seed.

## Risks And Stop Conditions

Stop implementation or reject the output if any of these occur:

- shoreline, beach, shelf, ocean, or square-edge gates regress;
- feature stamps mutate protected coast/beach/shelf/ocean masks before final constraints in a way that requires large correction;
- detail bands create the main ridges/valleys instead of supporting graph/erosion structures;
- warp-vector debug output shows obvious radial or circular organization around the primary volcano;
- hydrology is visibly blocked by plateaus, lava fields, or benches without explicit reroute/incision validation;
- old lava fields become random rough patches with no lobe/levee/flow-axis identity;
- secondary hills are cloned mini-cones or evenly distributed bumps;
- preview only looks good with all detail enabled;
- feature ownership map shows large unowned interior areas;
- manifest hashes or feature IDs change between deterministic reruns;
- the implementation introduces non-terrain systems outside scope.

## Rejected Or Forbidden Approaches

Rejected approaches:

- adding more global fBm until the terrain looks busy;
- using ridged noise as the primary ridge/gully generator;
- polar-coordinate domain warping around the volcano center;
- concentric terrace rings unless tied to an explicit named feature and validated against artifact diagnostics;
- random flattened ellipses pretending to be plateaus;
- random coastal fans not tied to valley mouths and contributing area;
- lava fields implemented only as material masks;
- decorative stamps with no hydrology policy;
- hidden generation state not recorded in the manifest;
- accepting a final beauty preview without stage/ownership/debug evidence;
- changing coastline/beach/shelf/ocean behavior in this PR;
- adding foliage, ecology, weather, audio, NPCs, survival systems, buildings, final materials, UI, maps, compass, minimap, GPS, or objective markers.

## Sources And References

The following sources inform the recommended approach. They are used for concepts, not as direct implementation dependencies.

1. Ken Perlin, “An Image Synthesizer,” *ACM SIGGRAPH Computer Graphics*, 1985. Introduces gradient noise as a procedural synthesis primitive. https://dl.acm.org/doi/10.1145/325165.325247
2. Ken Perlin, “Improving Noise,” *ACM Transactions on Graphics / SIGGRAPH*, 2002. Improves interpolation/gradient behavior for procedural noise. https://www.cs.cmu.edu/~jkh/462_s07/paper445.pdf
3. F. Kenton Musgrave, Craig E. Kolb, Robert S. Mace, “The Synthesis and Rendering of Eroded Fractal Terrains,” *SIGGRAPH*, 1989. Important terrain synthesis reference for fractal terrain and erosion-style terrain rendering. https://doi.org/10.1145/74334.74337
4. David S. Ebert et al., *Texturing and Modeling: A Procedural Approach*, Morgan Kaufmann. General procedural texturing/modeling reference for noise, turbulence, and procedural composition.
5. Inigo Quilez, “Domain Warping” / warped fBm articles and shader notes. Practical reference for nested/warped procedural fields. https://iquilezles.org/articles/warp/
6. Ian McEwan, David Sheets, Stefan Gustavson, Mark Richardson, “Efficient Computational Noise in GLSL,” 2012. Discusses practical computational Perlin/simplex noise implementation without texture lookups. https://arxiv.org/abs/1204.1461
7. Pascal Müller, Peter Wonka, Simon Haegler, Andreas Ulmer, Luc Van Gool, “Procedural Modeling of Buildings,” *ACM SIGGRAPH*, 2006. Useful for context-sensitive grammar thinking and hierarchical procedural identity. https://dl.acm.org/doi/10.1145/1141911.1141931
8. Przemyslaw Prusinkiewicz and Aristid Lindenmayer, *The Algorithmic Beauty of Plants*, Springer, 1990. Classic reference for rule-based natural-form generation and hierarchical rewriting systems. https://algorithmicbotany.org/papers/abop/abop.pdf
9. Brian Wyvill, Craig McPheeters, Geoff Wyvill, “Data Structure for Soft Objects,” *The Visual Computer*, 1986; related field-function/implicit modeling literature. Useful for soft field blending and implicit feature masks. https://link.springer.com/article/10.1007/BF01900346
10. Paul Bourke, “Implicit Surfaces.” Practical overview of implicit fields, soft objects, and field functions. https://paulbourke.net/geometry/implicitsurf/
11. David G. Tarboton, “A New Method for the Determination of Flow Directions and Upslope Areas in Grid Digital Elevation Models,” *Water Resources Research*, 1997. D-infinity flow direction reference for DEM hydrology. https://doi.org/10.1029/96WR03137
12. Greg Tucker and Gregory Hancock, “Modelling Landscape Evolution,” *Earth Surface Processes and Landforms*, 2010. Useful overview of landscape evolution modeling concepts. https://doi.org/10.1002/esp.1952
13. R. Bossis et al., “Initial shape reconstruction of a volcanic island as a tool for quantifying long-term coastal erosion: Corvo Island, Azores,” *Earth Surface Dynamics*, 2023. Volcanic island erosion and coastal evolution case reference. https://esurf.copernicus.org/articles/11/529/2023/
14. M. Stokes and A. Gomes, “Alluvial fans on volcanic islands: A morphometric perspective,” *Geomorphology*, 2020. Useful for volcanic-island alluvial fan constraints and morphometric thinking. https://doi.org/10.1016/j.geomorph.2020.107356
15. U.S. Geological Survey and Federal Geographic Data Committee, “Geospatial Positioning Accuracy Standards / National Standard for Spatial Data Accuracy.” Reference point for DEM-style validation concepts such as RMSE and independent accuracy checks. https://www.fgdc.gov/standards/projects/accuracy/part3/chapter3
16. D. Tarboton, “Terrain Analysis Using Digital Elevation Models in Hydrology,” ESRI User Conference Proceedings, 2003. Practical DEM hydrology and terrain-analysis discussion. https://proceedings.esri.com/library/userconf/proc03/p1038.pdf
