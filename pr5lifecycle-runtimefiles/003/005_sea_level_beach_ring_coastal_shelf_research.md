# Sea-Level Beach Ring And Coastal Shelf Research

## Executive Summary

PR5 Batch 003 should treat the island edge as a **shoreline constraint**, not as another noisy terrain boundary. The terrain generator must first define an explicit circular/organic island mask inside the 16.256 km square world container, then use signed distance from the organic shoreline to control elevation. The required invariant is simple:

```text
outside island mask        = ocean / below sea level
organic shoreline boundary = exactly sea level
edge land band             = beach, backshore, coastal shelf, or low coastal terrain
interior                   = inland volcanic terrain rising from the coast
square world edge          = ocean/container only; never raised island terrain
```

The most robust implementation is a layered height function:

1. Generate a stable organic island radius `R(theta)` around the world centre.
2. Convert every terrain sample to signed shoreline distance `d = R(theta) - r`, where `d > 0` is landward and `d < 0` is seaward.
3. Build a hard sea-level shoreline rule around `d = 0`.
4. Build ocean bathymetry from `d < 0`.
5. Build beach/backshore/coastal flats from `d > 0` before inland volcanic terrain is allowed to dominate.
6. Blend ridge, gully, fan, cliff, and headland modifiers through masks that are constrained by the shoreline rule.

This avoids the current failure mode where the square landscape or island rim becomes an elevated wall. The island can still have cliffs, rocky points, pocket beaches, creek mouths, lahar fans, and coastal flats, but all of those features must be expressed as **variation around a fixed sea-level shoreline**, not as uncontrolled radial noise at the world edge.

The recommended default for the 16.256 km domain is an organic island with a mean land radius of roughly **6.8–7.2 km**, leaving at least **0.7–1.3 km of ocean margin** to the square container boundary even at the widest headlands. The beach/backshore band should usually be **40–140 m** wide, with wider depositional bays and fan mouths reaching **180–300 m**. The nearshore coastal shelf should extend at least **250–900 m seaward** from the shoreline and fall smoothly from 0 m to roughly **-8 m to -25 m**, with deeper container-ocean bathymetry farther out.

## Terrain-Only Scope Boundary

This document is limited to terrain generation, terrain masks, heightfield logic, collision/elevation correctness, and developer-only validation. It does **not** design final water rendering, waves, foliage, ecology, weather, audio, survival mechanics, NPCs, story, VFX, final materials, or player-facing UI.

Allowed in scope:

- Heightfield elevation logic.
- Island mask and ocean mask.
- Sea-level shoreline rules.
- Beach/backshore/coastal shelf terrain profiles.
- Lowland transition geometry.
- Drainage, gully, ridge, fan, cliff, and headland termination logic.
- Terrain debug channels and validation maps.
- Developer-only visualization overlays in editor or diagnostic exports.

Forbidden in this document:

- Final water shader, wave simulation, foam, underwater rendering, or ocean art.
- Final beach materials, vegetation/ecology, weathering art, or gameplay loops.
- Player-facing map, minimap, compass, GPS, objective markers, bearing UI, time UI, or orientation UI.
- Survival, NPC, story, quest, crafting, or audio design.

## Current Terrain Baseline

The project context states that Jungle Game uses a **16.256 km square world container** and already has a runtime terrain shell. PR5 Batch 003 is a major implementation batch for a realistic active volcanic island. The island must be circular/organic inside the square domain rather than square-shaped.

The key baseline correction is that the terrain generator must stop treating the outer island edge as an uncontrolled raised boundary. In a volcanic island terrain, the outer mask boundary is not a cliff wall around the whole map. It is the transition from land to sea. The square world edge is not the island edge; it is the simulation/container boundary and should remain ocean, below sea level, or non-playable container terrain.

The likely existing terrain stack already has some combination of:

- world-coordinate terrain sampling;
- radial or noise-based massif shaping;
- volcanic highland/ridge/gully passes;
- height normalization;
- runtime mesh or landscape output;
- masks for slope, elevation, or biome-like terrain regions.

PR5 should insert the coastline model **before** final height normalization and before ridge/gully modifiers can reach the outer mask unsafely. The coastline must become a first-class constraint with its own diagnostics.

## Major Implementation Goal

The goal is to make the terrain generator obey this invariant for every terrain sample:

```text
H(x, y) <= seaLevel                    when outside the island mask
H(x, y) = seaLevel ± epsilon           on the shoreline constraint band
H(x, y) rises gently and plausibly     across beach/coastal lowland
H(x, y) may become rugged/volcanic     only after the coastal transition
```

Recommended constant:

```cpp
constexpr float SeaLevelM = 0.0f;
```

Use metres as the internal terrain unit during generation, then convert to UE centimetres or Landscape import scale only at the output boundary.

The coastline should be treated as a **constraint manifold**. Ridges, gullies, fan deposits, headlands, cliffs, and noise may vary the landward and seaward profile, but they must not move the actual sea-level crossing unpredictably after the shoreline has been solved.

Implementation target:

```text
domain:
  square size = 16256 m
  half size   = 8128 m

island:
  mean organic radius    = 6800–7200 m
  max radius incl. noise = <= 7400 m preferred
  minimum ocean margin   = >= 700 m to square boundary
```

This leaves the square world edge as ocean/container area and gives enough room for coastal shelf bathymetry.

## Island Mask Requirements

### Coordinate model

Define terrain coordinates in metres relative to the world centre:

```cpp
float2 p = float2(x - WorldCenterX, y - WorldCenterY);
float r = length(p);
float theta = atan2(p.y, p.x);
```

The 16.256 km square domain spans:

```text
x, y ∈ [-8128 m, +8128 m]
```

The island mask must be based on an **organic polar radius** rather than square-domain extents.

### Organic island radius

Generate a periodic radial function:

```cpp
float R0 = 7000.0f;        // mean island radius
float R(theta) = R0 + shorelineMacro(theta) + shorelineMeso(theta);
```

Recommended perturbation budget:

```text
macro shoreline variation: ±250–450 m
meso shoreline variation:  ±40–160 m
micro shoreline variation: disabled for shoreline solve; use only as beach/headland detail masks
hard max radius:           7400 m
hard min radius:           6200 m
```

Do not apply high-frequency noise directly to the final sea-level boundary. It creates aliasing, pinched bays, detached islands, and unstable waterline intersections. Instead:

- use low-frequency periodic noise for the actual coastline radius;
- use higher-frequency noise only for beach width, rocky patches, small headlands, and local slope variation;
- smooth `R(theta)` in angular space so adjacent terrain samples produce a continuous shoreline.

A practical workflow:

```cpp
float n1 = PeriodicNoise(theta * 3.0f + seedA);   // broad lobes
float n2 = PeriodicNoise(theta * 7.0f + seedB);   // bays / headlands
float n3 = PeriodicNoise(theta * 15.0f + seedC);  // smaller curvature

float rawOffset =
    360.0f * n1 +
    180.0f * n2 +
     70.0f * n3;

float Rtheta = clamp(R0 + SmoothAngular(rawOffset), 6200.0f, 7400.0f);
```

For a more deterministic implementation, precompute a 2048- or 4096-sample shoreline radius lookup table, smooth it, clamp it, and sample it by angle during terrain generation. This helps diagnostics because the actual island boundary can be exported and inspected independently of terrain height.

### Signed shoreline distance

Once `R(theta)` exists, every sample receives a signed distance:

```cpp
float d = Rtheta - r;
```

Interpretation:

```text
d > 0   landward of shoreline
d = 0   shoreline
d < 0   seaward/ocean side
```

This single variable should drive all coastal zones.

### Core masks

Use smooth masks derived from `d`:

```cpp
float OceanMask       = saturate((-d) / OceanShelfWidthM);
float ShorelineMask   = 1.0f - smoothstep(0.0f, ShorelineLockWidthM, abs(d));
float BeachMask       = smoothstep(0.0f, BeachWidthM, d) * (1.0f - smoothstep(BeachWidthM, BeachWidthM + 40.0f, d));
float CoastalFlatMask = smoothstep(BeachWidthM, LowlandWidthM, d) * (1.0f - smoothstep(LowlandWidthM, LowlandWidthM + 300.0f, d));
float InlandMask      = smoothstep(LowlandWidthM, LowlandWidthM + 700.0f, d);
```

Suggested defaults:

```text
ShorelineLockWidthM = 8–20 m
BeachWidthM         = 40–140 m, varied by coast sector
LowlandWidthM       = 250–900 m, wider at fans and bays
OceanShelfWidthM    = 250–900 m before deeper drop
```

### Avoid binary-only masks

Do not rely only on `r < R(theta)` as a binary land/ocean flag. Binary masks are useful for collision and classification, but the heightfield needs smooth signed-distance zones so the coastline can be both correct and natural.

Required outputs:

```text
IslandMask       = d >= 0
OceanMask        = d < 0
SignedCoastDistM = d
ShorelineMask    = near d = 0
BeachMask        = landward beach/backshore zone
CoastalFlatMask  = lowland transition zone
InlandMask       = volcanic terrain zone
```

## Sea-Level Shoreline Rules

### Rule 1: the shoreline is defined by the island mask boundary

The shoreline is the organic boundary where:

```text
r = R(theta)
d = 0
H = SeaLevelM
```

This must remain true after all terrain passes.

### Rule 2: sea level is not discovered after terrain generation

Do not generate mountainous terrain first and then hope that a 0 m contour appears in a clean circular ring. That produces broken coastlines, raised rims, drowned interior basins, and irregular detached water bodies.

Instead:

1. Generate the island mask.
2. Generate the signed shoreline distance.
3. Generate a base coastal profile pinned to sea level.
4. Blend inland terrain into that base.
5. Re-apply a final shoreline lock/clamp if necessary.

### Rule 3: final height must be locked near `d = 0`

Use a narrow lock band to prevent noise and ridge modifiers from pushing the actual shoreline above or below sea level.

Recommended final operation:

```cpp
float lock = 1.0f - smoothstep(0.0f, ShorelineLockWidthM, abs(d));
H = lerp(H, SeaLevelM, lock);
```

Where:

```text
ShorelineLockWidthM = 8–20 m
```

If heightfield resolution is coarse, set `ShorelineLockWidthM` to at least 1.5–2.5 terrain samples wide. A single-sample coastline lock will alias.

### Rule 4: outside island radius is never land

For `d < 0`, the terrain must be ocean bathymetry or below sea level:

```cpp
if (d < 0.0f)
{
    H = min(H, SeaLevelM - OceanMinDepthM);
}
```

But avoid a harsh vertical drop immediately outside the shoreline. The nearshore shelf should ramp down gradually.

### Rule 5: square world edge is not island edge

For any coordinate near the square world boundary, terrain must remain ocean/container unless the organic island radius actually reaches that area. With the recommended radius budget, it should not.

Required validation:

```text
minimum distance from max R(theta) to square edge >= 700 m
all four square edges are below sea level
all four square corners are below sea level
no terrain-connected land touches the square container boundary
```

### Rule 6: inland terrain cannot overwrite coastal constraints

Volcanic massif, ridges, gullies, lava shoulders, and noise should be multiplied by `InlandMask` or an equivalent attenuation term.

Example:

```cpp
float inland = smoothstep(LowlandWidthM * 0.65f, LowlandWidthM + 700.0f, d);
H += inland * VolcanicHeightContribution;
```

Ridge/gully fields may be computed globally, but their height influence must be suppressed or redirected as they approach the coast.

## Beach Ring Elevation Profile

### Recommended cross-shore land profile

The beach ring should not be a flat collar. It should be a low, gently rising, terrain-only band:

```text
d = 0 m                    H = 0.0 m
d = 10–30 m                H = 0.2–0.8 m
d = 40–90 m                H = 1.0–2.5 m
d = 80–160 m               H = 2.0–4.5 m, low backshore / berm / fan toe
d = 200–900 m              H = 4–35 m, coastal lowland transition
```

Default beach/backshore width:

```text
normal sandy/coarse beach: 60–100 m
narrow rocky beach:        20–50 m
wide depositional bay:     100–180 m
fan or creek mouth:        140–300 m
cliff/headland pocket:     0–40 m, with cliff toe still meeting sea level
```

These values are intentionally game-scale and implementation-scale rather than a claim of one universal real-world beach width. They provide enough area for a readable tropical island beach ring inside a 16.256 km world while preserving realistic cross-shore logic.

### Beach width variation

Beach width should vary by coast sector:

```cpp
float sector = PeriodicNoise(theta * 5.0f + seedBeach);
float exposure = PeriodicNoise(theta * 2.0f + seedExposure);

float BeachWidthM = lerp(50.0f, 130.0f, saturate(0.5f + 0.5f * sector));
BeachWidthM *= lerp(0.65f, 1.35f, BayMask);
BeachWidthM *= lerp(0.45f, 0.85f, RockyHeadlandMask);
BeachWidthM *= lerp(1.2f, 2.0f, FanMouthMask);
BeachWidthM = clamp(BeachWidthM, 20.0f, 300.0f);
```

### Beach elevation function

A safe baseline function for landward beach height:

```cpp
float t = saturate(d / BeachWidthM);

// low convex rise from waterline
float beachRise = BeachMaxM * pow(t, 0.75f);

// small berm/backshore lift near landward edge
float berm = BermHeightM * smoothstep(0.62f, 0.92f, t) * (1.0f - smoothstep(0.92f, 1.0f, t));

float Hbeach = SeaLevelM + beachRise + berm;
```

Suggested values:

```text
BeachMaxM  = 1.5–3.0 m
BermHeightM = 0.4–1.5 m
```

The beach band must remain low enough that it does not become a raised island rim. The backshore/berm can be locally higher, but it should not form a continuous wall. Break it up using creek mouths, fans, rocky points, and low coastal flats.

### Avoid uniform ring artifacts

A perfect ring beach looks artificial. Vary:

- beach width;
- beach slope;
- coarse/sandy/rocky terrain masks;
- pocket beaches in bays;
- narrow or absent beach at cliffs;
- depositional fans at gully exits;
- creek-mouth notches and shallow deltas;
- low coastal flats behind bays.

Do not vary the actual sea-level crossing. Vary the terrain on each side of it.

## Coastal Shelf And Ocean-Below-Sea-Level Model

### Nearshore bathymetry

For `d < 0`, the terrain should descend below sea level, not remain flat at 0 m forever.

Use seaward distance:

```cpp
float s = -d; // metres seaward from shoreline
```

Recommended profile:

```text
s = 0 m            H = 0.0 m
s = 25–80 m        H = -0.5 to -2.0 m
s = 150–350 m      H = -3 to -8 m
s = 500–900 m      H = -8 to -25 m
beyond shelf       H = -25 to -80 m, depending on container margin
```

A stable shelf formula:

```cpp
float shelfT = saturate(s / ShelfWidthM);
float shelfDepth = ShelfDepthM * pow(shelfT, ShelfExponent);
float Hocean = SeaLevelM - shelfDepth;
```

Suggested values:

```text
ShelfWidthM    = 250–900 m
ShelfDepthM    = 8–25 m at shelf break
ShelfExponent  = 0.55–1.25
DeepOceanDepth = 35–80 m near square container edge
```

A volcanic island can have steep sectors, but the generator should not make every coastline plunge instantly. Use cliffs/headlands as sector-based variations.

### Deeper container ocean

After the nearshore shelf, continue down or flatten to a stable bathymetric container:

```cpp
float deepT = smoothstep(ShelfWidthM, ShelfWidthM + 900.0f, s);
float Hdeep = SeaLevelM - DeepOceanDepthM;

Hocean = lerp(Hocean, Hdeep + OceanFloorNoise, deepT);
```

Use only low-amplitude ocean floor noise:

```text
OceanFloorNoise = ±1–5 m
```

Do not allow ocean floor noise to cross sea level.

Final ocean clamp:

```cpp
if (d < -ShorelineLockWidthM)
{
    H = min(H, SeaLevelM - 0.05f);
}
```

### Ocean mask outputs

The terrain generator should output:

```text
OceanMask:
  1 where d < 0
  0 where d >= 0

ShallowShelfMask:
  high from s = 0 to ShelfWidthM

DeepOceanMask:
  high after shelf transition

WaterlineMask:
  narrow band around d = 0

SubmergedTerrainDepthM:
  max(0, SeaLevelM - H)
```

These are terrain/debug masks only. They do not prescribe final water rendering.

### Why sea level needs a stable datum

Use a project-local height datum where `SeaLevelM = 0`. Real geodesy uses reference surfaces such as mean sea level/geoid and local tidal datums; for this project, the important implementation idea is not a real-world datum conversion, but a single internal datum that all terrain passes respect. NOAA defines tidal datums as base elevations for measuring water levels and mean sea level as the arithmetic mean of hourly heights observed over the National Tidal Datum Epoch; NOAA also describes the geoid as a global mean-sea-level-like reference surface used for elevations. The project should mirror that discipline by having one authoritative sea-level constant for terrain generation.

## Coastal Lowland Transition Geometry

### Purpose

The coastal lowland transition prevents the beach from snapping directly into volcanic mountains. A real volcanic island can rise steeply inland, but the terrain still needs coherent coastal flats, fan toes, valley mouths, and low-gradient transition zones.

Recommended distance band:

```text
from beach/backshore end to inland volcanic terrain:
  250–900 m default
  900–1600 m in major fan/bay sectors
  100–300 m at cliffs/headlands
```

### Base lowland profile

Let:

```cpp
float lowD = max(0.0f, d - BeachWidthM);
float lowT = saturate(lowD / LowlandWidthM);
```

A default lowland height:

```cpp
float Hlow = BeachBackHeightM + LowlandRiseM * pow(lowT, 1.35f);
```

Suggested values:

```text
BeachBackHeightM = 2–4 m
LowlandRiseM     = 12–35 m
```

Then blend to inland volcanic base:

```cpp
float inlandBlend = smoothstep(0.65f, 1.0f, lowT);
H = lerp(Hlow, Hinland, inlandBlend);
```

### Do not make a continuous coastal wall

The landward side of the beach may form berms, fan toes, or low terraces, but a continuous 5–20 m wall around the island would read as a raised rim. Break continuity with:

- drainage outlets;
- creek-mouth cuts;
- depositional fan lobes;
- low marsh/flat-like terrain masks if used as terrain only;
- pocket beaches;
- rocky headlands where the cliff face replaces the beach, not raises the whole ring.

### Coastal flat mask

Create a mask that can be used by later terrain and material stages:

```cpp
float CoastalFlatMask =
    smoothstep(BeachWidthM * 0.7f, BeachWidthM + 60.0f, d) *
    (1.0f - smoothstep(BeachWidthM + LowlandWidthM, BeachWidthM + LowlandWidthM + 300.0f, d));
```

The mask identifies terrain that is low, near-coast, and transitional. It should not depend on final materials.

## Ridge / Gully / Fan Termination At Coast

### Main rule

Ridges and gullies should be generated as inland drainage/erosion systems that resolve into coastal fans, creek mouths, or low-gradient outlet valleys before they reach the beach. They must not cut through the shoreline lock in a way that creates raised waterline steps or below-sea-level land gaps.

### Drainage direction

For a radial volcanic island, drainage generally runs from high interior terrain toward the coast. Use the signed shoreline distance and angular sectors to guide this:

```text
source: high volcanic massif/ridge network
path: downslope gullies and channels
termination: fan toe / creek mouth / bay / pocket beach
base level: sea level at d = 0
```

Each major drainage should have an outlet angle `thetaOutlet`. Its centerline runs from inland high terrain toward the coast. Near the coast, it widens and flattens.

### Gully attenuation near shoreline

Gully incision should fade before the shoreline lock:

```cpp
float coastSafe = smoothstep(ShorelineLockWidthM * 2.0f, BeachWidthM + 80.0f, d);
GullyIncision *= coastSafe;
```

This allows channels to remain visible inland and across lowlands while preventing them from making the shoreline discontinuous.

### Creek mouth notch

At the outlet, carve a shallow, wide notch in the beach/backshore, but pin the final waterline to 0 m.

```cpp
float outletAcross = abs(AngularDistance(theta, thetaOutlet)) * Rtheta;
float outletAlong = d;

float mouthMask =
    exp(-(outletAcross * outletAcross) / (2.0f * MouthHalfWidthM * MouthHalfWidthM)) *
    smoothstep(-40.0f, 40.0f, outletAlong) *
    (1.0f - smoothstep(MouthLengthM, MouthLengthM + 100.0f, outletAlong));

float notchDepth = lerp(0.2f, 1.2f, OutletStrength);
H -= mouthMask * notchDepth * coastSafe;
```

Suggested values:

```text
minor creek mouth half-width: 8–20 m
major creek mouth half-width: 25–80 m
mouth landward length:        80–250 m
maximum notch depth:          0.2–1.2 m, but final shoreline lock still applies
```

### Coastal fan deposits

At gully exits, add a fan lobe rather than a trench that simply cuts to the water. This is especially important for a volcanic island with lahar and volcaniclastic sediment delivery.

Fan logic:

```text
fan apex:      where steep gully leaves volcanic slope / enters lowland
fan body:      widening downslope lobe toward coast
fan toe:       low, broad deposit that blends into beach/backshore
base level:    sea level at shoreline
```

Height contribution:

```cpp
float fanLengthM = 350.0f to 1200.0f;
float fanHalfAngle = 5° to 14°;
float fanThicknessM = 1.0f to 6.0f;

float fanDownT = saturate((FanApexD - d) / fanLengthM);
float fanWidth = lerp(25.0f, 220.0f, fanDownT);
float fanAcrossMask = exp(-(across * across) / (2.0f * fanWidth * fanWidth));
float fanLongMask = smoothstep(0.0f, 0.15f, fanDownT) * (1.0f - smoothstep(0.9f, 1.0f, fanDownT));

float fanDeposit = fanThicknessM * fanAcrossMask * fanLongMask * (1.0f - ShorelineMask);
H += fanDeposit;
```

The shoreline lock should be applied after fan deposition so the fan toe does not raise the waterline. Fan deposits can widen beaches and create low coastal flats behind creek mouths.

### Avoid drainage failure modes

Forbidden failure modes:

- gully incision continues outside the island into the ocean as a land canyon above sea level;
- drainage outlet raises the shoreline into a delta wall;
- channel cuts the beach ring below sea level and creates inland ocean holes;
- ridge spurs reach the waterline as continuous walls around the island;
- every drainage terminates at the exact same beach width/profile;
- fan deposits overwrite sea-level clamp.

## Cliffs, Headlands, Creek Mouths, And Beach Variation

### Cliff and headland model

Cliffs and rocky headlands should be sector masks that alter the landward slope, narrow the beach, and steepen the nearshore shelf while preserving sea-level correctness.

A headland sector should do three things:

```text
1. push coastline radius outward slightly at low frequency;
2. narrow or remove sandy beach width;
3. steepen landward and/or seaward slope outside the shoreline lock.
```

Example:

```cpp
float HeadlandMask = SectorMask(theta, HeadlandCenter, HeadlandWidth);
BeachWidthM = lerp(BeachWidthM, 20.0f, HeadlandMask);

float cliffT = smoothstep(ShorelineLockWidthM, CliffRiseDistanceM, d);
float cliffRise = CliffHeightM * pow(cliffT, 0.55f) * HeadlandMask;

H = max(H, SeaLevelM + cliffRise);
H = lerp(H, SeaLevelM, ShorelineMask); // final lock
```

Suggested values:

```text
headland angular width:       4°–18°
cliff rise distance inland:   30–160 m
cliff height at back edge:    8–60 m
beach width at headland:      0–40 m
nearshore shelf width:        80–300 m, locally steeper
```

Even a cliff sector should hit sea level at the base. The cliff face can rise rapidly landward from the lock band, but it must not create an elevated ocean edge.

### Pocket beaches

Between rocky headlands, create pocket beaches:

```text
bay sector:
  inward coastline radius offset
  wider beach
  gentler shelf
  wider coastal lowland
  possible creek/fan outlet
```

Implementation:

```cpp
BeachWidthM = lerp(BeachWidthM, BeachWidthM * 1.4f, BayMask);
LowlandWidthM = lerp(LowlandWidthM, LowlandWidthM * 1.35f, BayMask);
ShelfWidthM = lerp(ShelfWidthM, ShelfWidthM * 1.25f, BayMask);
```

### Creek mouths

Creek mouths should be wide, low, and shallow. They may create breaks in beach berms and widen coastal flats. They should not break the 0 m sea-level contour.

Rules:

```text
creek mouth centerline intersects shoreline at d = 0
beach/backshore is locally lowered and widened
fan/deposit mask may raise adjacent banks
final shoreline lock runs after mouth shaping
ocean side may include shallow submerged channel, but always below sea level
```

### Coastal fans

A lahar/alluvial fan sector should:

- originate where drainage exits steep volcanic terrain;
- widen toward lowland/coast;
- create a broad low-gradient deposit;
- increase coastal flat width;
- widen or flatten beach;
- terminate at sea level.

Use fans sparingly. A 16.256 km island may support several major radial drainages and many minor gullies, but only some should produce large fan lobes.

Suggested distribution:

```text
major fan sectors: 4–8
minor fan sectors: 12–30
rocky headlands:   6–14
pocket beaches:    between headland sectors
major creek mouths: one per major drainage/fan
```

### Preserve continuity

After all cliff/headland/creek/fan variation:

```cpp
H = ApplyOceanBathymetryWhenDNegative(H, d);
H = ApplyFinalShorelineLock(H, d);
H = PreventOceanAboveSeaLevel(H, d);
```

This final pass is not optional.

## Terrain Masks And Debug Channels

The implementation should emit masks/debug channels alongside the heightfield. These are developer-facing outputs, not player UI.

Required terrain masks:

```text
HeightM
SignedCoastDistanceM
IslandMask
OceanMask
WaterlineMask
BeachMask
BackshoreMask
CoastalShelfMask
DeepOceanMask
CoastalFlatMask
InlandVolcanicMask
RidgeMask
GullyMask
DrainageOutletMask
FanDepositMask
CliffHeadlandMask
SlopeDegrees
CurvatureOrRoughness
SeaLevelErrorM
```

### Sea-level error channel

Compute:

```cpp
SeaLevelErrorM = abs(H - SeaLevelM) where WaterlineMask > threshold
```

Export as a heat map. This is the most important diagnostic channel for this prompt.

Suggested thresholds:

```text
excellent: max shoreline error <= 0.05 m
acceptable runtime terrain: max shoreline error <= 0.15 m
fail: any connected shoreline segment > 0.25 m above/below sea level
hard fail: raised island rim, square-edge land, or waterline discontinuity
```

### Coast distance visualization

Export signed distance as a diverging map:

```text
blue/cool: seaward d < 0
white:     d = 0 shoreline
warm:      landward d > 0
```

The 0 contour should form exactly one organic ring unless deliberate offshore rocks/islets are later introduced. This batch should avoid detached islands unless explicitly approved.

### Zone classification

Each terrain sample should have a primary zone classification:

```text
0 = deep ocean/container
1 = shallow shelf
2 = waterline lock band
3 = beach/backshore
4 = coastal flat/fan toe
5 = inland volcanic terrain
6 = cliff/headland sector
7 = drainage/fan special case
```

This supports automated tests and quick image validation.

## Developer-Only Coastline Validation Maps

### Required maps

Export or display the following developer-only maps after generation:

1. **0 m contour map**  
   Shows all terrain samples or interpolated line segments where height crosses sea level.

2. **Expected shoreline map**  
   Shows `r = R(theta)` from the island mask.

3. **Sea-level error map**  
   Shows `abs(H - 0)` within the waterline lock band.

4. **Ocean mask map**  
   Shows all `d < 0` samples.

5. **Beach mask map**  
   Shows beach/backshore width variation.

6. **Coastal shelf depth map**  
   Shows seaward depth from shoreline to shelf/deep ocean.

7. **Slope map**  
   Confirms beach/coastal flats are low-gradient and cliffs are localized.

8. **Drainage outlet map**  
   Confirms gullies/fans terminate at beach/lowland and do not break shoreline.

9. **Square-edge validation map**  
   Confirms all four world-container edges and corners are below sea level.

10. **Connected component map**  
    Confirms land is one intended island component and does not touch square edges.

### Quantitative checks

Implement automated validation after height generation:

```cpp
struct CoastValidation
{
    float MaxShorelineAbsErrorM;
    float MeanShorelineAbsErrorM;
    float P95ShorelineAbsErrorM;
    float MinSquareEdgeHeightM;
    float MaxSquareEdgeHeightM;
    float MinOceanHeightM;
    float MaxOceanHeightM;
    float MinBeachWidthM;
    float MaxBeachWidthM;
    float MinOceanMarginToSquareEdgeM;
    int32 NumLandComponents;
    bool bLandTouchesSquareEdge;
    bool bOceanHasAboveSeaSamples;
    bool bShorelineContinuityPassed;
};
```

Recommended acceptance values:

```text
MaxShorelineAbsErrorM:       <= 0.15 m
P95ShorelineAbsErrorM:       <= 0.05 m
MaxSquareEdgeHeightM:        < -5.0 m
MaxOceanHeightM outside lock: < -0.05 m
MinOceanMarginToSquareEdgeM: >= 700 m
NumLandComponents:           1
bLandTouchesSquareEdge:      false
```

### 0 m contour comparison

Compare generated 0 m contour to expected `R(theta)`:

```text
for each angle theta:
  expected radius = R(theta)
  measured radius = first H=0 crossing from centre outward
  radial error = measured - expected
```

Acceptance:

```text
mean radial error: <= 1 terrain sample
P95 radial error:  <= 2 terrain samples
no missing angle sectors
no duplicate large landward/oceanward crossings except local creek/ocean details explicitly marked
```

### Square boundary test

Sample all four square edges:

```cpp
for x in [-8128, +8128], all y:
    assert(H(x, y) < SeaLevelM - 5.0f)

for y in [-8128, +8128], all x:
    assert(H(x, y) < SeaLevelM - 5.0f)
```

Corners should be deeper ocean/container:

```text
corner target height: -35 to -80 m
```

### Beach width test

For each `theta`, estimate the landward beach/backshore band from `d = 0` to the first sample where `BeachMask` falls below threshold. Check:

```text
normal sectors: 40–140 m
fan/bay sectors: can reach 180–300 m
cliff/headland sectors: can narrow to 0–40 m
no global uniform ring unless debug mode intentionally forces it
```

## Large PR Implementation Scope

This batch is large enough to be implemented as a clear coastline subsystem rather than scattered noise tweaks.

Recommended PR scope:

1. Add shoreline/island-mask generation.
2. Add signed shoreline distance field.
3. Add sea-level shoreline lock.
4. Add beach/backshore profile.
5. Add shallow shelf and deep-ocean bathymetry profile.
6. Add coastal lowland transition.
7. Gate inland volcanic/ridge/gully modifiers by coastal masks.
8. Add cliff/headland/pocket beach sector masks.
9. Add drainage outlet/fan termination logic.
10. Add developer-only debug exports/maps.
11. Add automated validation checks and fail conditions.

Keep the implementation terrain-only. Do not attach final water, material, vegetation, UI, or gameplay systems to this PR.

### Suggested generation order

```text
1. World coordinates and polar coordinates.
2. Organic shoreline radius lookup R(theta).
3. Signed coast distance d = R(theta) - r.
4. Base ocean/shelf profile for d < 0.
5. Base beach/backshore profile for small d > 0.
6. Coastal lowland profile.
7. Inland volcanic base profile.
8. Ridge/gully modifiers, attenuated near coast.
9. Fan/cliff/headland/creek-mouth modifiers.
10. Final shoreline lock.
11. Final ocean clamp.
12. Debug masks.
13. Validation pass.
```

This order makes the coastline constraint explicit and resilient.

## Files And Areas Likely Touched

Exact filenames depend on the current codebase, but the likely areas are:

```text
Terrain generation core:
  - runtime height sampling
  - terrain profile/elevation function
  - world coordinate conversion
  - island mask/radial falloff code
  - noise/seed utilities

Terrain feature passes:
  - volcanic massif/ridge generation
  - gully/drainage carving
  - fan/deposit shaping
  - slope/erosion masks

Output and validation:
  - heightfield export
  - debug mask export
  - editor/debug preview tools
  - automated terrain validation tests
  - terrain generation configuration structs/assets
```

Recommended new or refactored modules:

```text
IslandMask / CoastlineMask
CoastalProfile
OceanBathymetryProfile
CoastlineValidation
TerrainDebugExport
DrainageOutletProfile
```

Configuration should be centralized:

```cpp
struct FCoastlineTerrainConfig
{
    float SeaLevelM = 0.0f;

    float WorldSizeM = 16256.0f;
    float MeanIslandRadiusM = 7000.0f;
    float MinIslandRadiusM = 6200.0f;
    float MaxIslandRadiusM = 7400.0f;
    float MinOceanMarginToSquareEdgeM = 700.0f;

    float ShorelineLockWidthM = 12.0f;

    float BeachWidthMinM = 40.0f;
    float BeachWidthMaxM = 140.0f;
    float FanBeachWidthMaxM = 300.0f;

    float LowlandWidthMinM = 250.0f;
    float LowlandWidthMaxM = 900.0f;

    float ShelfWidthMinM = 250.0f;
    float ShelfWidthMaxM = 900.0f;
    float ShelfDepthMinM = 8.0f;
    float ShelfDepthMaxM = 25.0f;

    float DeepOceanDepthMinM = 35.0f;
    float DeepOceanDepthMaxM = 80.0f;

    float MaxAllowedShorelineErrorM = 0.15f;
};
```

For UE Landscape import/export, keep the generator in metres and convert only at the boundary. Unreal Landscape height values use a signed range around -256 to 255.992 multiplied by Z scale, with 16-bit precision, so sea-level placement and vertical scale must be deliberate rather than incidental.

## Acceptance Criteria

The PR is acceptable only if all of these are true.

### Coastline correctness

- The organic island edge is sea level around the full island.
- The square world edge is ocean/container only.
- No raised island rim exists at the outer circular edge.
- No square-world cliff or square land sheet is generated.
- Outside the island radius, terrain is ocean or below sea level.
- Inside the island radius, terrain rises inland through beach/coastal lowland into volcanic terrain.
- The final shoreline lock is applied after cliffs, fans, gullies, and ridge modifiers.

### Beach/coastal shelf behavior

- Beach/backshore band exists around most coastline.
- Beach width varies plausibly by sector.
- Rocky/cliff sectors may narrow beaches but still meet sea level.
- Fan/bay/creek sectors may widen beaches and coastal flats.
- Shallow shelf bathymetry exists outside the shoreline.
- Deeper ocean/container terrain exists before the square world edge.

### Drainage and fan behavior

- Major gullies drain toward coast without cutting through sea-level continuity.
- Outlet channels are shallow/wide near the mouth.
- Fan deposits terminate at the beach/lowland and do not raise the waterline.
- Ridges fade, split, or become headlands near the coast rather than forming a continuous rim.

### Diagnostics

- Developer-only maps exist for 0 m contour, expected shoreline, sea-level error, ocean mask, beach mask, shelf depth, slope, drainage outlets, square-edge validation, and connected components.
- Automated validation reports max/mean/P95 shoreline error.
- Automated validation checks that all square edges/corners are below sea level.
- Automated validation checks land component count and square-edge contact.
- Validation fails loudly for raised rim, ocean above sea level, missing coastline sectors, or square-edge land.

### Terrain-only scope

- No final water rendering, wave, foliage, ecology, weather, audio, survival, NPC, story, VFX, final material, or player UI work is introduced.
- Debug overlays/maps remain developer-only.

## Risks And Stop Conditions

### Risk: shoreline generated after terrain

If the terrain is generated first and the coastline is inferred afterward, the shoreline will be unstable. Stop and refactor so the island mask and signed shoreline distance are generated first.

### Risk: high-frequency coastline noise

High-frequency noise on `R(theta)` can create jagged boundaries, tiny peninsulas, detached artifacts, and impossible shelf geometry. Stop if the coastline radius changes faster than the terrain resolution can support. Smooth the radius lookup.

### Risk: final modifiers overwrite sea level

Ridges, gullies, fan deposits, and cliff passes can accidentally push the waterline up/down. Stop if validation shows shoreline error over threshold. Reorder passes or reapply final lock.

### Risk: ocean terrain rises above sea level

Ocean floor noise or shelf variation must never cross 0 m outside the island. Stop if any seaward samples outside the shoreline lock are above sea level.

### Risk: square-edge land

Any land touching the square terrain boundary means the island mask failed or the radius budget is too large. Stop and reduce `MaxIslandRadiusM`, increase margin, or correct the mask.

### Risk: continuous coastal wall

A beach/backshore profile that rises too steeply or uniformly can become a ring wall. Stop if slope maps show a continuous high-slope belt around the island except for deliberate cliff sectors.

### Risk: water/rendering scope creep

This PR should solve terrain elevation and masks. Stop if implementation begins designing final water rendering, waves, materials, ecology, or player UI.

## Rejected Or Forbidden Approaches

Do not use these approaches:

1. **Square falloff as island boundary**  
   This creates a square island or square cliffs. The island must be circular/organic inside the square container.

2. **Raised rim around island edge**  
   The island edge is sea level, not a crater wall.

3. **Generate mountains first, then clamp a ring**  
   This often creates unnatural cuts and broken drainage. The coastline profile must be foundational.

4. **Binary mask only**  
   A hard land/ocean mask without signed distance cannot produce beach, shelf, and lowland transitions cleanly.

5. **Ocean as flat 0 m terrain outside island**  
   Outside terrain should be below sea level, with a shelf and deeper container ocean.

6. **High-frequency coastline noise as final boundary**  
   Use high-frequency detail for local terrain variation, not for the authoritative shoreline solve.

7. **Ridges/gullies allowed to overwrite shoreline**  
   All inland features must respect the sea-level lock.

8. **Continuous beach berm wall**  
   The backshore must be varied and broken by outlets, fans, flats, and headlands.

9. **Final water/art/material systems in this PR**  
   Terrain masks can support future systems, but this batch must not implement them.

10. **Player-facing navigation UI**  
    No map/minimap/compass/GPS/objective/bearing/time/orientation UI is part of this spec.

## Sources And References

The following sources support the terrain-generation principles used here. They are not direct implementation recipes for Jungle Game; they provide reliable grounding for sea-level datums, coastal profile concepts, shoreline change, volcanic drainage/fan behavior, DEM/heightfield handling, and UE Landscape scale constraints.

1. NOAA Tides & Currents, **Tidal Datums**. Defines tidal datums as base elevations for water-level measurement and defines mean sea level as an arithmetic mean of hourly heights over the National Tidal Datum Epoch.  
   <https://tidesandcurrents.noaa.gov/datum_options.html>

2. NOAA Ocean Service, **What is the geoid?** Describes the geoid as a model of global mean sea level and an elevation reference surface.  
   <https://oceanservice.noaa.gov/facts/geoid.html>

3. U.S. Geological Survey, **Coastal Change**. Summarizes coastal change research and the role of waves, currents, storms, sediment movement, shoreline change characterization, and seafloor mapping.  
   <https://www.usgs.gov/centers/whcmsc/science/coastal-change>

4. Geoscience Australia, **Coastal erosion**. Describes shoreline retreat, wave/current forcing, mass wasting, headland undercutting, sediment removal, and the use of mean sea level for coastline mapping products.  
   <https://www.ga.gov.au/education/natural-hazards/coastal-erosion>

5. Coastal Wiki, **Shoreface profile**. Defines the shoreface/beach profile, upper/lower shoreface, closure depth, and typical upper-shoreface scales and slopes.  
   <https://www.coastalwiki.org/wiki/Shoreface_profile>

6. U.S. Geological Survey Volcano Hazards Program, **Lahars move rapidly down valleys like rivers of concrete**. Describes lahars moving downslope through valleys and entraining material.  
   <https://www.usgs.gov/programs/VHP/lahars-move-rapidly-down-valleys-rivers-concrete>

7. Pierson et al., USGS Pinatubo volume, **Lahar deposition at Mount Pinatubo**. Documents lahar sediment transport and deposition on low-gradient alluvial fans and plains after the 1991 eruption.  
   <https://pubs.usgs.gov/pinatubo/pierson/>

8. NASA Earthdata, **Shuttle Radar Topography Mission (SRTM)**. Describes near-global land elevation data, useful as context for DEM/heightfield treatment of land surfaces.  
   <https://www.earthdata.nasa.gov/data/instruments/srtm>

9. Epic Games, **Landscape Technical Guide in Unreal Engine**. Documents UE Landscape height range, Z scale, and heightmap precision considerations.  
   <https://dev.epicgames.com/documentation/unreal-engine/landscape-technical-guide-in-unreal-engine>
