# 1400 m Volcanic Massif Heightfield Research

## Executive Summary

PR5 Batch 003 should replace the current weak provisional terrain with a deterministic island-scale heightfield whose central landform reads as an active composite volcanic massif: broad lower flanks, concave steepening toward the summit, an asymmetric crater/rim complex, breached rim geometry, radial spurs, benches, saddles, and protected low coastal terrain.

The required change is not just “raise the noise amplitude.” The current 16.256 km square terrain shell and 129 x 129 procedural mesh produce roughly 127 m vertex spacing, so a 1400 m massif would be undersampled, faceted, and unable to represent a believable crater, rim, benches, ravines, or walkable slope bands. The generator should be split into a deterministic heightfield source model, an island/shoreline mask, a volcanic massif profile model, and developer-only validation outputs.

Recommended headline targets:

| Parameter | Target |
|---|---:|
| Square container size | 16,256 m x 16,256 m |
| Mean organic island radius | 6,850-7,150 m |
| Safe ocean margin to square edge | >= 700 m everywhere, target 900-1,200 m |
| Main massif center | Near center, offset <= 400 m for asymmetry |
| Massif base radius | 4,000-4,400 m |
| Shoulder radius | 2,700-3,200 m |
| Upper cone / summit approach radius | 1,100-1,600 m |
| Crater rim radius | 360-520 m |
| Crater floor radius | 160-260 m |
| Peak rim elevation | 1,380-1,430 m |
| Crater floor elevation | 1,160-1,260 m |
| Coast/beach ring elevation | -2 m to +8 m, clamped near sea level |
| Current mesh spacing | 127 m, blocker |
| Minimum usable heightfield resolution | 2017 x 2017 source, about 8.1 m spacing |
| Preferred heightfield resolution | 4033 x 4033 source, about 4.0 m spacing |

Composite volcano references support a conical but concave profile that becomes steeper near the summit, with summit craters, crater-wall breaks, flank flows, and large steep-sided dimensions. The target shape should therefore avoid a mathematically perfect cone while retaining a clear volcanic silhouette: radial symmetry at macro scale, asymmetry at medium scale, and roughness/channels at local scale.

## Terrain-Only Scope Boundary

This document is terrain generation only. It covers:

- elevation math;
- island mask and sea-level blend;
- massif profile, crater, rim, shoulders, benches, saddles, ravines, and slope breaks;
- terrain collision and hard-blocker geometry derived from slope/elevation;
- developer-only validation maps and pass/fail metrics;
- runtime mesh / heightfield resolution implications.

It does **not** design or recommend:

- foliage, ecology, animals, NPCs, weather, audio, VFX, story, quests, survival mechanics, player UI, minimaps, compass systems, objective markers, GPS, bearing UI, materials, final art, or final biome dressing;
- player-facing navigation aids;
- production art look-dev beyond terrain-form requirements.

## Current Heightfield Baseline

Current project baseline from the implementation prompt:

- world container: 16.256 km square;
- terrain mesh: 129 x 129 procedural mesh;
- current max height: about +140 m;
- current slopes: mostly below 7 degrees;
- current land identity issue: square shell can read as terrain identity unless masked into a circular/organic island;
- target batch type: large terrain implementation PR, not a minimal patch.

The current mesh is the primary blocker. A 129 x 129 mesh across 16,256 m has:

```text
quads_per_axis = 128
vertex_spacing = 16256 / 128 = 127 m
```

Consequences:

- a 420 m crater rim radius gets only about 3.3 samples from center to rim;
- a 200 m crater floor radius gets about 1.6 samples;
- a 500-800 m summit bench gets only 4-6 samples;
- radial spurs, breached rim notches, ravines, and slope bands alias into triangles;
- any 1400 m peak becomes a low-poly pyramid/mound rather than a readable volcanic massif;
- slope classification becomes numerically unstable because a single triangle covers too much terrain.

This PR should treat 129 x 129 as a debug placeholder only. The deterministic heightfield should be authored independently of render mesh density so validation can happen at source resolution.

## Major Implementation Goal

Implement a deterministic heightfield model that produces:

1. a circular or organically rounded island inside the 16.256 km square container;
2. ocean / below-sea terrain outside the island radius;
3. a protected beach and coastal shelf around the island perimeter;
4. a low coastal plain rising inland;
5. a central 1400 m-class volcanic massif rising from inland terrain, not from the world edge;
6. an asymmetric summit crater and rim complex;
7. volcanic shoulders, saddles, spurs, benches, ravines, and slope breaks;
8. developer-only validation maps and numeric acceptance gates.

The massif should read as volcanic because of its profile and summit structure, not because of art dressing. The terrain should be deterministic from seed constants and should not depend on nondeterministic runtime noise.

## Island Mask And Sea-Level Blend Requirements

### Coordinate System

Use world-space meters with the world centered at `(0, 0)`:

```text
world_size_m = 16256
world_half_m = 8128
sea_level_m = 0
```

Use a nearby but slightly offset massif center:

```text
island_center = vec2(0, 0)
massif_center = vec2(-220, 180)    // example, deterministic
```

The offset should be small enough that the mountain remains central, but large enough to avoid a perfect radial target.

### Organic Island Radius

The island radius should be angle-dependent but bounded, keeping the island circular/organic rather than square:

```cpp
float OrganicIslandRadius(float theta)
{
    float r0 = 7000.0f;
    float organic =
        0.035f * sin(3.0f * theta + 0.70f) +
        0.025f * sin(5.0f * theta - 1.10f) +
        0.015f * sin(9.0f * theta + 2.30f);

    return clamp(r0 * (1.0f + organic), 6600.0f, 7350.0f);
}
```

This leaves at least:

```text
8128 - 7350 = 778 m
```

between the farthest island edge and the square container edge, preserving the square as a container only.

### Normalized Island Radius

```cpp
float2 p = WorldXY;
float2 q = p - island_center;
float theta = atan2(q.y, q.x);
float island_radius = OrganicIslandRadius(theta);
float n = length(q) / island_radius;   // 0 at center, 1 at shoreline
```

### Coastal Height Envelope

The coastal mask should own all terrain near the island perimeter. The massif must not be allowed to leak into this region.

Recommended normalized island bands:

| Normalized radius `n` | Terrain identity | Elevation envelope |
|---:|---|---:|
| `n > 1.03` | ocean container | -80 m to -180 m |
| `1.00 < n <= 1.03` | shallow coastal shelf | -25 m to -2 m |
| `0.965 < n <= 1.00` | beach / swash / shoreline | -2 m to +4 m |
| `0.920 < n <= 0.965` | beach berm / backshore | +2 m to +8 m |
| `0.800 < n <= 0.920` | coastal plain | +8 m to +55 m |
| `0.680 < n <= 0.800` | inner lowland / foothill transition | +45 m to +140 m |
| `n <= 0.680` | massif-eligible interior | unconstrained by coast, still blended |

Suggested coastal base function:

```cpp
float CoastalBaseHeight(float n)
{
    if (n >= 1.03f)
        return -140.0f;

    if (n >= 1.00f)
    {
        float t = smoothstep(1.03f, 1.00f, n);
        return lerp(-35.0f, 0.0f, t);
    }

    if (n >= 0.965f)
    {
        float t = smoothstep(1.00f, 0.965f, n);
        return lerp(0.0f, 4.0f, t);
    }

    if (n >= 0.920f)
    {
        float t = smoothstep(0.965f, 0.920f, n);
        return lerp(4.0f, 12.0f, t);
    }

    if (n >= 0.800f)
    {
        float t = smoothstep(0.920f, 0.800f, n);
        return lerp(12.0f, 55.0f, t);
    }

    if (n >= 0.680f)
    {
        float t = smoothstep(0.800f, 0.680f, n);
        return lerp(55.0f, 120.0f, t);
    }

    return 120.0f;
}
```

### Massif Beach Protection Gate

The massif contribution must fade out before the coast:

```cpp
float MassifInteriorGate(float n)
{
    // Full mountain strength inside n <= 0.62.
    // Zero mountain strength by n >= 0.74.
    return 1.0f - smoothstep(0.62f, 0.74f, n);
}
```

This is separate from the massif radius check. It ensures even warped shoulders or spurs cannot invade the beach/coastal shelf.

Final terrain composition should be:

```cpp
float h_coast = CoastalBaseHeight(n);
float h_massif = MassifHeight(WorldXY);
float g_massif = MassifInteriorGate(n);
float h = h_coast + g_massif * h_massif;

// Hard safety clamps near perimeter.
if (n >= 0.965f) h = min(h, 6.0f);
if (n >= 1.000f) h = min(h, 0.0f);
if (n >= 1.030f) h = min(h, -80.0f);
```

## Massif Scale And Proportion Targets

The target is a 1400 m-class active volcanic massif inside a roughly 14 km diameter island. The mountain cannot occupy the whole island; it needs coastal plains and a beach ring around it.

Recommended radial dimensions from `massif_center`:

| Feature | Radius target | Elevation target | Purpose |
|---|---:|---:|---|
| Massif outer toe | 4,000-4,400 m | +80 to +140 m | blends into inner lowlands |
| Lower flank | 3,200-4,000 m | +140 to +350 m | broad volcanic base |
| Main shoulder | 2,700-3,200 m | +350 to +550 m | major slope break / bench system |
| Mid flank | 1,600-2,700 m | +550 to +950 m | readable steepening cone |
| Upper cone | 900-1,600 m | +950 to +1,250 m | steep volcanic summit approach |
| Summit apron | 550-900 m | +1,250 to +1,340 m | transition into crater rim |
| Crater rim | 360-520 m | +1,360 to +1,430 m | highest volcanic ring |
| Crater floor | 160-260 m | +1,160 to +1,260 m | central depression |
| Central vent mound/dome, optional terrain only | 40-100 m | +1,190 to +1,280 m | small internal floor relief, not VFX/story |

The base radius is intentionally large. A 1400 m rise over 4.2 km produces a whole-massif average grade near 33%, or about 18 degrees, before local slope shaping. That allows lower flanks to remain traversable while summit sectors can reach 30-35 degrees.

## Heightfield Shape Model

### Preferred Model: Deterministic Control-Point Profile + Procedural Modifiers

Use a deterministic radial elevation profile as the backbone, then apply bounded asymmetry, shoulders, ravines, and slope breaks. Avoid stacking unbounded noise fields.

Recommended radial control points, in meters from `massif_center`:

| Radius `r` | Elevation above coastal base contribution |
|---:|---:|
| 0 | 1,185 |
| 180 | 1,195 |
| 240 | 1,220 |
| 420 | 1,405 |
| 560 | 1,360 |
| 800 | 1,285 |
| 1,150 | 1,180 |
| 1,600 | 1,010 |
| 2,200 | 760 |
| 2,900 | 500 |
| 3,500 | 260 |
| 4,200 | 90 |
| 4,700 | 0 |

Interpolate these points with a smooth monotone/cubic interpolation where possible. The crater is intentionally non-monotone; handle the crater/rim section as a special summit profile rather than forcing global monotonicity.

### Deterministic Radial Warp

A perfect cone reads artificial. Warp the effective radius before profile lookup:

```cpp
float AngularHarmonics(float theta)
{
    return
        0.055f * sin(2.0f * theta + 0.40f) +
        0.040f * sin(3.0f * theta - 1.70f) +
        0.030f * sin(5.0f * theta + 2.10f);
}

float MassifWarpedRadius(float2 p)
{
    float2 q = p - massif_center;
    float r = length(q);
    float theta = atan2(q.y, q.x);

    float radialWeight = smoothstep(400.0f, 3600.0f, r) *
                         (1.0f - smoothstep(4300.0f, 4700.0f, r));

    float warp = 1.0f + radialWeight * AngularHarmonics(theta);
    return r / clamp(warp, 0.88f, 1.12f);
}
```

This changes the plan-view footprint but keeps the profile bounded.

### Massif Function Pseudocode

```cpp
float MassifHeight(float2 world_xy)
{
    float2 q = world_xy - massif_center;
    float r_raw = length(q);
    float theta = atan2(q.y, q.x);
    float r = MassifWarpedRadius(world_xy);

    float h = SampleMassifProfile(r);       // control-point profile above

    h += SecondaryShoulders(r_raw, theta);
    h += SpurRidges(r_raw, theta);
    h -= RavineCuts(r_raw, theta);
    h -= BreachedRimNotch(r_raw, theta);
    h += BenchesAndSlopeBreaks(r_raw, theta);

    // No negative mountain contribution outside toe.
    h *= 1.0f - smoothstep(4200.0f, 4700.0f, r_raw);
    return max(h, 0.0f);
}
```

### Secondary Shoulder Function

```cpp
float AngularGaussian(float theta, float center, float sigma)
{
    float d = atan2(sin(theta - center), cos(theta - center));
    return exp(-(d * d) / (2.0f * sigma * sigma));
}

float RingGaussian(float r, float center, float sigma)
{
    float d = r - center;
    return exp(-(d * d) / (2.0f * sigma * sigma));
}

float SecondaryShoulders(float r, float theta)
{
    float h = 0.0f;

    h += 65.0f * RingGaussian(r, 2950.0f, 360.0f) * AngularGaussian(theta, radians(35.0f), 0.55f);
    h += 45.0f * RingGaussian(r, 2500.0f, 300.0f) * AngularGaussian(theta, radians(150.0f), 0.45f);
    h += 55.0f * RingGaussian(r, 3300.0f, 420.0f) * AngularGaussian(theta, radians(255.0f), 0.50f);
    h += 35.0f * RingGaussian(r, 2100.0f, 260.0f) * AngularGaussian(theta, radians(310.0f), 0.38f);

    return h;
}
```

### Ravine Cuts

Ravines should cut down from upper and mid flanks but must fade before the beach-protection zone:

```cpp
float RavineCuts(float r, float theta)
{
    float radial = smoothstep(700.0f, 1700.0f, r) *
                   (1.0f - smoothstep(3800.0f, 4550.0f, r));

    float cut = 0.0f;
    cut += 70.0f * radial * AngularGaussian(theta, radians(70.0f), 0.10f);
    cut += 95.0f * radial * AngularGaussian(theta, radians(215.0f), 0.13f);
    cut += 55.0f * radial * AngularGaussian(theta, radians(320.0f), 0.11f);

    return cut;
}
```

### Bench / Slope-Break Function

Benches are low-amplitude radial flattening structures, not terraces that look hand-modeled:

```cpp
float BenchesAndSlopeBreaks(float r, float theta)
{
    float undulation = 0.0f;

    undulation += 18.0f * RingGaussian(r, 3150.0f, 180.0f);
    undulation -= 14.0f * RingGaussian(r, 2750.0f, 160.0f);
    undulation += 22.0f * RingGaussian(r, 1550.0f, 130.0f);
    undulation -= 16.0f * RingGaussian(r, 1250.0f, 100.0f);

    float asym = 0.65f + 0.35f * sin(4.0f * theta + 0.8f);
    return undulation * asym;
}
```

The sign pattern creates subtle convex/concave breaks without making contour rings perfectly even.

## Slope Profile Targets

Composite volcanoes should steepen toward the summit while keeping lower flanks broader and more traversable. The heightfield should target the following slope distribution after all modifiers are applied:

| Zone | Radius | Target slope band | Notes |
|---|---:|---:|---|
| Beach / shelf | island `n >= 0.965` | 0-3 degrees, local max 5 degrees | sea-level preservation |
| Coastal plain | island `0.80-0.965` | 1-7 degrees | low rolling approach terrain |
| Inner lowlands / toe | massif `3.8-4.7 km` | 4-12 degrees | mountain starts gradually |
| Lower flank | massif `3.0-3.8 km` | 8-16 degrees | broad volcanic flank |
| Mid flank | massif `1.7-3.0 km` | 12-24 degrees | main climb / mass read |
| Upper cone | massif `0.8-1.7 km` | 22-34 degrees | volcanic steepening |
| Crater outer rim | massif `0.36-0.60 km` | 28-42 degrees local | hard blockers allowed in sectors |
| Crater inner wall | massif `0.18-0.42 km` | 30-48 degrees local | hard blockers expected |
| Crater floor | massif `<0.24 km` | 0-12 degrees | depressed basin with uneven floor |

Slope should be computed from finite differences on the source heightfield:

```cpp
float dzdx = (h[x + 1, y] - h[x - 1, y]) / (2.0f * dx);
float dzdy = (h[x, y + 1] - h[x, y - 1]) / (2.0f * dy);
float slope_degrees = degrees(atan(sqrt(dzdx * dzdx + dzdy * dzdy)));
```

### Terrain-Only Slope Classification

These are terrain/collision/debug bands only, not player UI:

| Slope | Classification | Terrain/collision implication |
|---:|---|---|
| 0-12 degrees | normal walkable ground | no blocker geometry |
| 12-22 degrees | steep walkable ground | no blocker geometry; useful for flank identity |
| 22-32 degrees | difficult steep ground | no automatic blocker; validate collision normals |
| 32-42 degrees | dangerous / unstable terrain | selective blockers allowed, especially rim/channels |
| 42-55 degrees | hard-block candidate | blocker geometry expected unless deliberately passable |
| >55 degrees | cliff / impossible terrain | hard-block or collision simplification required |

Hard blockers should be derived from contiguous slope/elevation masks, not manually placed arbitrary invisible walls. The blocker mask should be inspectable as a developer-only map.

## Summit / Crater / Rim Geometry

### Target Geometry

| Feature | Target |
|---|---:|
| Rim mean radius | 420 m |
| Rim radius range after warp | 360-520 m |
| Rim crest elevation | 1,360-1,430 m |
| Highest point | 1,380-1,430 m, target 1,405 m |
| Crater floor radius | 180-240 m |
| Crater floor elevation | 1,160-1,260 m |
| Inner wall relief | 130-230 m from rim to floor |
| Breach notch width | 12-22 degrees angular width |
| Breach notch cut | 120-240 m below adjacent rim |
| Crater floor tilt | 15-35 m across diameter |

### Rim Model

Use a ring bump plus inner crater depression, or encode directly in the radial profile. If using additive functions:

```cpp
float CraterAndRim(float r, float theta)
{
    float rim_radius = 420.0f;
    float rim_sigma = 85.0f;
    float floor_radius = 210.0f;

    float rim = 95.0f * RingGaussian(r, rim_radius, rim_sigma);
    float bowl = 185.0f * (1.0f - smoothstep(floor_radius, rim_radius, r));

    // Crater floor should not be perfectly flat.
    float floorWarp = smoothstep(0.0f, floor_radius, r) *
                      (1.0f - smoothstep(floor_radius, rim_radius, r));
    float floorTilt = 22.0f * floorWarp * cos(theta - radians(235.0f));

    return rim - bowl + floorTilt;
}
```

### Breached Rim

The breach is important because many real volcanic craters/rims are not perfect rings, and USGS references note that lava can flow through breaks in crater walls. The breach should be terrain-only: a lowered rim sector and a connected downslope drainage/ravine, not lava/VFX.

```cpp
float BreachedRimNotch(float r, float theta)
{
    float breachTheta = radians(215.0f);
    float angular = AngularGaussian(theta, breachTheta, 0.16f);  // about 18 deg full visible sector

    float rimNotch = 180.0f * angular * RingGaussian(r, 430.0f, 150.0f);

    float chuteRadial = smoothstep(420.0f, 900.0f, r) *
                        (1.0f - smoothstep(3200.0f, 4050.0f, r));
    float chute = 75.0f * angular * chuteRadial;

    return rimNotch + chute;
}
```

Acceptance target: the breached sector should lower the rim crest to roughly 1,180-1,280 m while adjacent rim sectors remain above 1,360 m.

## Shoulder / Saddle / Spur Geometry

### Shoulders

Shoulders are broad raised sectors around the massif, mostly between 2.4 km and 3.5 km radius. They break the circular contour pattern and create secondary mass forms.

Recommended shoulder targets:

| Shoulder | Azimuth | Radius | Height effect | Purpose |
|---|---:|---:|---:|---|
| NE shoulder | 35 degrees | 2.7-3.2 km | +40 to +80 m | broad buttress |
| SE shoulder | 150 degrees | 2.3-2.8 km | +30 to +60 m | asymmetric flank mass |
| SW shoulder | 255 degrees | 3.0-3.6 km | +45 to +70 m | long lower shoulder |
| NW shoulder | 310 degrees | 1.9-2.4 km | +25 to +45 m | upper spur source |

### Saddles

Saddles are low passes between shoulders/spurs. They should be shallow enough to read as terrain structure, not artificial trenches.

Recommended saddle cuts:

| Saddle | Azimuth | Radius | Cut |
|---|---:|---:|---:|
| North saddle | 95 degrees | 2.4-3.0 km | -35 to -60 m |
| West saddle | 225 degrees | 2.8-3.5 km | -45 to -75 m |
| South saddle | 180 degrees | 1.6-2.2 km | -25 to -45 m |

Pseudocode:

```cpp
float SaddleCuts(float r, float theta)
{
    float cut = 0.0f;
    cut += 45.0f * RingGaussian(r, 2700.0f, 360.0f) * AngularGaussian(theta, radians(95.0f), 0.32f);
    cut += 65.0f * RingGaussian(r, 3150.0f, 420.0f) * AngularGaussian(theta, radians(225.0f), 0.38f);
    cut += 35.0f * RingGaussian(r, 1900.0f, 280.0f) * AngularGaussian(theta, radians(180.0f), 0.30f);
    return cut;
}
```

### Spurs

Spurs should radiate from upper and mid flanks, not from the beach. They are raised ridge-like sectors with adjacent ravine cuts.

Recommended spur rule:

```cpp
float SpurRidges(float r, float theta)
{
    float radial = smoothstep(700.0f, 1400.0f, r) *
                   (1.0f - smoothstep(3600.0f, 4300.0f, r));

    float ridge = 0.0f;
    ridge += 45.0f * radial * AngularGaussian(theta, radians(25.0f), 0.11f);
    ridge += 35.0f * radial * AngularGaussian(theta, radians(135.0f), 0.12f);
    ridge += 50.0f * radial * AngularGaussian(theta, radians(285.0f), 0.10f);

    return ridge;
}
```

Spur amplitude should be smaller than the core edifice. If spurs dominate the silhouette from the coastline, the mountain stops reading as a central volcanic massif.

## Anti-Cone Naturalization Strategy

The mountain should be volcanic, not a perfect cone. Use layered deterministic asymmetry:

1. **Macro radial symmetry preserved**  
   The massif should still have a central summit/rim and broadly radial flanks.

2. **Radius warp**  
   Low-order angular harmonics distort the base and shoulder footprint by about 5-12%.

3. **Concave slope profile**  
   Lower flanks start broad and less steep; upper flanks steepen. This is more important than noise.

4. **Rim asymmetry**  
   The crater rim has one breached sector and 2-3 uneven high sectors.

5. **Radial spurs and ravines**  
   Raised ridges and cut channels alternate around the flanks.

6. **Benches and slope breaks**  
   Low-amplitude rings and partial rings interrupt a smooth radial climb.

7. **Bounded local roughness**  
   Add detail only after the source heightfield has enough resolution. Use slope-aware amplitudes so local roughness does not destroy beach/cliff safety.

Recommended local roughness envelope:

| Zone | Max local detail amplitude |
|---|---:|
| Beach / shore | 0.3-1.0 m |
| Coastal plain | 1-4 m |
| Lower flank | 4-12 m |
| Mid flank | 8-20 m |
| Upper flank | 10-28 m |
| Crater/rim | 8-24 m |

Local noise must be slope-limited:

```cpp
float detailAmp = ZoneDetailAmplitude(zone);
float slopeLimit = 1.0f - smoothstep(38.0f, 48.0f, slope_degrees_pre_detail);
h += detailAmp * slopeLimit * DeterministicFbm(world_xy * frequency, seed);
```

This prevents high-frequency noise from creating accidental hard blockers everywhere.

## Beach Ring Protection Rules

The coastline requirement is a hard acceptance gate.

Rules:

1. The square world edge must never define the island silhouette.
2. Outside the organic island radius, terrain must be ocean / below sea level.
3. The island perimeter must resolve to sea level, beach, and coastal shelf.
4. The central massif must be zeroed or heavily faded before the beach ring.
5. No mountain shoulder, spur, ravine cut, local noise, or debug deformation can raise the beach ring above its elevation envelope.
6. The ocean margin from island edge to square container edge should remain visible in top-down validation.
7. Beach/shelf clamps should run after all terrain contributions.

Hard clamps:

```cpp
if (n >= 1.03f)
{
    h = min(h, -80.0f);
}
else if (n >= 1.00f)
{
    h = min(h, 0.0f);
}
else if (n >= 0.965f)
{
    h = clamp(h, -2.0f, 6.0f);
}
else if (n >= 0.920f)
{
    h = min(h, 12.0f);
}
```

Validation gates:

| Metric | Pass condition |
|---|---:|
| Max height for `n >= 0.965` | <= +6 m |
| Max height for `n >= 1.000` | <= 0 m |
| Min ocean depth for `n >= 1.030` | <= -80 m |
| Beach ring 95th-percentile slope | <= 4 degrees |
| Beach ring max slope | <= 7 degrees except rocky debug outcrops if explicitly enabled |
| Any massif contribution at `n >= 0.74` | exactly 0 or numerically negligible |
| Minimum square-edge ocean margin | >= 700 m |

## Runtime Mesh Resolution Implications

### Current Mesh Is Not Fit For Purpose

The 129 x 129 mesh gives 127 m spacing. That is too coarse for:

- first-person ground readability;
- crater/rim geometry;
- 1400 m vertical relief;
- slope-band validation;
- collision derived from terrain normals;
- hard-block masks;
- shoreline/beach transitions;
- saddles, benches, and ravines.

### Recommended Source Heightfield Resolutions

| Resolution | Spacing over 16,256 m | Use |
|---:|---:|---|
| 129 x 129 | 127.0 m | reject except debug thumbnail |
| 505 x 505 | 32.3 m | too coarse for first-person summit/crater, acceptable only for far LOD |
| 1009 x 1009 | 16.1 m | minimum far terrain source, not ideal for crater |
| 2017 x 2017 | 8.1 m | minimum acceptable full-island source |
| 4033 x 4033 | 4.0 m | preferred production source for this PR |
| 8129 x 8129 | 2.0 m | high-end source / tiled runtime target, heavy |

If using Unreal Landscape, Epic lists 4033 x 4033 and 8129 x 8129 among recommended landscape sizes, with 4033 coming from 32 x 32 components of 126 x 126 quads. If staying with procedural mesh, use a chunked/tiled mesh or clipmap/quadtree LOD so the source heightfield can remain high resolution without drawing the whole island as one mesh.

### Recommended Implementation Pattern

Separate height source from render mesh:

```text
Deterministic terrain model
    -> high-resolution CPU heightfield cache / sample function
    -> slope/elevation/mask debug rasters
    -> runtime mesh chunks / UE Landscape import / collision tiles
```

Do not bake terrain identity into a single 129 x 129 mesh.

### Suggested Runtime Geometry Strategy

Acceptable options:

1. **UE Landscape path**
   - Use 4033 x 4033 heightmap for the 16.256 km world.
   - Set XY scale to approximately 4.0317 m per vertex if matching exact world size.
   - Use 16-bit height storage.
   - Choose Z range that includes ocean depth and peak height.

2. **Procedural chunk path**
   - Source function sampled at 4-8 m for validation.
   - Runtime chunks use LOD: 4-8 m near player, 16-32 m far, coarser ocean/container.
   - Collision uses a separate simplified heightfield per chunk but must preserve hard blockers and slope thresholds.

3. **Hybrid path**
   - UE Landscape or static heightfield for base terrain.
   - Procedural collision/blocker meshes for crater rim cliffs and hard-slope sectors.

### Vertical Range / Z Precision

Target vertical envelope:

```text
min_height = -180 m
sea_level = 0 m
max_height = +1430 m
safe_total_range = 1700 m
```

For Unreal Landscape import, the Z scale should be based on the full intended height range, not just the positive mountain height. Epic documents that default Landscape height covers roughly -256 m to +256 m at Z scale 100, and custom height uses the 1/512 ratio after converting meters to centimeters. For a 1700 m total range:

```text
z_scale = 1700 m * 100 cm/m * (1 / 512)
        = 332.03125
```

If the import maps sea level to a biased midpoint, ensure the encoded height values leave room below sea level. Do not use a vertical range that clips ocean or rim height.

## Developer-Only Topographic Validation Targets

This PR should add developer-only validation outputs. These are not player UI.

### Required Debug Maps

Generate PNG/EXR/CSV or in-editor debug layers for:

1. elevation map;
2. slope degrees map;
3. island normalized radius map `n`;
4. beach-protection gate map;
5. massif contribution map;
6. crater/rim mask map;
7. hard-block candidate mask;
8. contour map at 50 m and 100 m intervals;
9. cross-section plots along at least 8 azimuths;
10. histogram of elevation and slope by zone.

### Numeric Pass/Fail Metrics

| Category | Pass condition |
|---|---:|
| Peak elevation | 1,380-1,430 m |
| Crater floor elevation | 1,160-1,260 m |
| Rim relief above crater floor | 130-230 m |
| Massif base radius | 4,000-4,400 m to main toe |
| Main shoulder radius | 2,700-3,200 m |
| Beach max elevation | <= +6 m for `n >= 0.965` |
| Ocean elevation | <= 0 m for `n >= 1.000` |
| Deep ocean/container | <= -80 m for `n >= 1.030` |
| Minimum island margin to square edge | >= 700 m |
| Slope >42 degrees | concentrated in rim/channels/blockers, not random noise |
| Slope <7 degrees in beach ring | >= 95% of beach cells |
| Massif leakage into beach gate | 0 cells with massif contribution >1 m where `n >= 0.74` |
| Contour shape | organic radial massif, no square-edge shoulders |
| Crater sampling at source resolution | rim radius represented by >= 40 samples preferred at 4 m, >= 20 minimum at 8 m |

### Cross-Section Targets

For 8 azimuths at 45-degree increments:

- confirm coastline is near 0 m at `n = 1.0`;
- confirm beach/coastal plain remains low until inner island;
- confirm massif toe begins around 4.0-4.4 km from massif center;
- confirm shoulder/slope break exists around 2.7-3.2 km;
- confirm summit/crater structure exists in all sections, but breach section has lowered rim;
- confirm no section shows a straight linear cone from coast to summit.

## Large PR Implementation Scope

This should be a major terrain implementation PR. Recommended scope:

1. Add deterministic terrain constants for world size, sea level, island radius, massif center, massif profile, crater, and slope bands.
2. Replace provisional height formula with layered heightfield pipeline:
   - island/coastal base;
   - massif profile;
   - crater/rim;
   - shoulders/spurs/saddles/ravines;
   - local detail;
   - beach protection clamp;
   - final height/slope classification.
3. Replace or bypass 129 x 129 production mesh path.
4. Add high-resolution source height sampling.
5. Add chunking/LOD or UE Landscape-compatible heightmap output.
6. Add developer-only validation maps and metrics.
7. Add tests for deterministic samples and hard mask gates.
8. Add before/after documentation with numeric metrics.

### Determinism Tests

Minimum deterministic tests:

```text
same seed + same world coordinate => same height
same seed + same world coordinate => same masks/slope class
massif center/rim sample heights within expected ranges
coast samples remain clamped near sea level
outside-island samples remain below sea level
```

### Suggested Unit Sample Points

| Sample | Coordinate type | Expected |
|---|---|---|
| World center / massif center | near summit/crater | 1,160-1,430 depending exact center/floor |
| Rim north | `r ~= 420 m` | >1,340 m unless breach sector |
| Breach sector rim | `theta ~= 215 deg, r ~= 420 m` | 1,180-1,300 m |
| Main shoulder | `r ~= 2900 m` | 400-650 m |
| Massif toe | `r ~= 4200 m` | 70-150 m |
| Beach ring | `n ~= 0.98` | -2 to +6 m |
| Shoreline | `n ~= 1.00` | approximately 0 m, never high |
| Ocean | `n > 1.03` | <= -80 m |
| Square corner | world corner | ocean/container, not land |

## Files And Areas Likely Touched

Actual file names depend on the repo, but the likely areas are:

| Area | Expected change |
|---|---|
| Terrain generator source | replace provisional height equation with deterministic island + massif model |
| Runtime procedural mesh actor/component | remove 129 x 129 production limit; add chunk/LOD or high-res sampling path |
| Terrain constants/config | add world, island, massif, crater, slope, and validation constants |
| Collision generation | derive hard blockers from slope/elevation masks or add blocker mesh pass |
| Debug/developer tools | elevation/slope/mask/contour exports |
| Tests | deterministic sample tests, beach clamp tests, slope classification tests |
| Documentation | PR notes, validation screenshots/maps, generated metric tables |

Do not touch unrelated systems such as foliage, ecology, weather, survival, NPCs, audio, story, or player-facing UI.

## Acceptance Criteria

The PR is acceptable when all of the following are true:

1. Terrain reaches a maximum elevation between 1,380 m and 1,430 m.
2. The summit includes a crater/rim system with a crater floor between 1,160 m and 1,260 m.
3. At least one crater rim breach exists and is connected to a downslope terrain channel.
4. The island is circular/organic and does not fill the square world as land.
5. All terrain outside the organic island radius is at or below sea level.
6. The beach ring is protected: no mountain shoulder leaks into the coastline.
7. The coastal shelf and beach ring are low, smooth, and numerically clamped after all terrain contributions.
8. The massif toe blends into inland terrain around 4.0-4.4 km from massif center.
9. The lower-to-upper flank profile steepens toward the summit rather than forming a uniform cone.
10. Secondary shoulders, saddles, spurs, benches, and ravines are visible in developer contour/slope validation.
11. Hard-blocker candidate geometry is derived from slope/elevation masks and is inspectable.
12. The production terrain path is no longer limited to 129 x 129 for the full 16.256 km world.
13. Source heightfield resolution is at least 2017 x 2017 equivalent, with 4033 x 4033 preferred.
14. Developer-only validation maps and metrics are generated and included in PR evidence.
15. Tests verify deterministic terrain samples and coastline protection gates.

## Risks And Stop Conditions

Stop or block the PR if any of these occur:

| Risk | Stop condition |
|---|---|
| Coastline leak | Any mountain contribution raises `n >= 0.965` above +6 m |
| Square-island failure | Land reaches or visually follows square world edge |
| Mesh undersampling | Production terrain still depends on 129 x 129 full-world mesh |
| Peak clipping | Height import/runtime clamps top below 1,380 m |
| Ocean clipping | Vertical range cannot represent below-sea shelf/ocean |
| Cone artifact | Contours form near-perfect concentric circles with no shoulders/breach/spurs |
| Noise artifact | High-frequency noise creates random hard-blockers across walkable flanks |
| Crater unreadable | Rim/floor/breach cannot be seen at source/runtime resolution |
| Non-determinism | Same seed/coordinate produces different height/masks between runs |
| Scope creep | PR adds foliage, weather, UI, survival, NPC, story, audio, or material systems |

## Rejected Or Forbidden Approaches

Do not use:

- a simple `height = noise * 1400` terrain;
- a perfect cone or radial paraboloid with no crater/rim asymmetry;
- square-edge landmass generation;
- mountain formulas that ignore island/coast masks;
- post-noise beach clamps only, without preventing massif leakage upstream;
- 129 x 129 production terrain for a 16.256 km first-person world;
- player-facing maps, minimaps, GPS, compass, marker, objective, bearing, or orientation UI;
- foliage/ecology/weather/audio/story/VFX/survival additions in this PR;
- arbitrary invisible blockers unrelated to slope/elevation validation;
- nondeterministic runtime noise that prevents reproducible tests.

## Sources And References

1. **U.S. Geological Survey — “Principal Types of Volcanoes.”** Composite volcanoes / stratovolcanoes are described as large, steep-sided cones built from alternating lava flows, volcanic ash, cinders, blocks, and bombs; most have summit craters, and lava may flow through breaks in crater walls or flank fissures.  
   URL: https://pubs.usgs.gov/gip/volc/types.html

2. **National Park Service — “Composite Volcanoes (Stratovolcanoes).”** Describes the classic composite volcano as conical with a concave profile that becomes steeper near the top, and notes that composite cones are many thousands of feet/meters tall and built from lava flows, pyroclastic deposits, lahar deposits, and domes.  
   URL: https://www.nps.gov/articles/000/composite-volcanoes.htm

3. **British Geological Survey — “Types of volcano.”** Notes that viscous magma forms steep-sided stratovolcanoes with slopes about 30-35 degrees, while shield volcanoes are generally below 10 degrees; this supports the upper-flank/rim slope targets and lower-flank distinction.  
   URL: https://www.bgs.ac.uk/discovering-geology/earth-hazards/volcanoes/how-volcanoes-form/

4. **Epic Games — “Landscape Technical Guide in Unreal Engine.”** Documents Landscape component architecture, valid/recommended heightmap sizes including 2017, 4033, and 8129, component-count performance considerations, and Z-scale calculation using the 1/512 ratio.  
   URL: https://dev.epicgames.com/documentation/unreal-engine/landscape-technical-guide-in-unreal-engine

5. **NASA Earthdata — “Shuttle Radar Topography Mission (SRTM).”** Provides DEM context: SRTM collected topographic data over nearly 80% of Earth’s land surfaces and produced near-global digital elevation models. This supports the validation concept of source heightfields, slope maps, and DEM-like elevation products.  
   URL: https://www.earthdata.nasa.gov/data/instruments/srtm

6. **USGS — “The South Carolina Coastal Erosion Study, Glossary of Coastal Terms.”** Defines beach/coastal terms such as berm and coastal plain; useful for keeping shoreline, beach/backshore, coastal plain, and shelf terminology explicit in the terrain mask.  
   URL: https://pubs.usgs.gov/of/2008/1206/html/glossary.html

7. **Coastal Wiki — “Shoreface profile” and “Definitions of coastal terms.”** Defines shoreface/beach-face concepts and supports separating shallow shelf, foreshore/beach, backshore/berm, and inland coastal plain bands in the mask.  
   URLs: https://www.coastalwiki.org/wiki/Shoreface_profile and https://www.coastalwiki.org/wiki/Definitions_of_coastal_terms
