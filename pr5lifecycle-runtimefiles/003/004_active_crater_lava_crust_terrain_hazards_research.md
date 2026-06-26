# Active Crater, Lava, And Crust Terrain Hazards Research

## Executive Summary

Jungle Game should use a **hybrid active-volcano terrain model** rather than a single decorative summit bowl:

- a **broken summit crater** below the volcanic high point;
- a **nested active vent / pit depression** inside the crater floor;
- one **breached rim notch** that drains into a major lava/collapse channel;
- a short **upper-flank fissure / side-vent field** aligned with the breach;
- **crusted lava shelves, platey lava floor, collapse steps, fissures, talus, ash/cinder aprons, and hard terrain blockers** around the crater edge;
- one or more **downslope lava-flow scar / lahar-channel systems** that taper into lower coastal fan terrain while respecting the island sea-level mask.

The active zone should read through geometry before final VFX/materials exist. The silhouette should show a damaged, asymmetric crater rim. The crater floor should not be a flat circular arena. It should be fractured, terraced, offset, and visibly unsafe through heightfield shape, collision masks, and slope diagnostics.

The strongest fit for a roughly **1400 m tropical volcanic massif** in a **16.256 km square island world** is a **summit crater + breached crater + nested vent + side-vent/fissure hybrid**. A true caldera-scale feature should be avoided unless the crater exceeds the playable summit space; USGS distinguishes craters from calderas partly by collapse scale, with calderas generally larger than about 1 km across and associated with major magma-chamber collapse. Keep the main crater mostly below that threshold so the terrain reads as a large active summit crater, not a full island-dominating caldera. [USGS-Crater-Caldera]

Key recommended targets:

| Feature | Recommended target |
|---|---:|
| Active crater elevation band | ~1180-1400 m, with rim peaks near massif summit |
| Main crater long axis | 620-780 m |
| Main crater short axis | 460-620 m |
| Main crater depth from rim crest to crater floor | 90-180 m |
| Nested vent / active pit diameter | 130-220 m |
| Nested vent / active pit drop below crater floor | 35-90 m |
| Main rim crest width | 30-90 m, irregular and discontinuous |
| Breach notch width | 110-220 m |
| Breach vertical relief | 70-160 m from rim crest into channel |
| Stable approach ridge width | 12-30 m usable crown, with wider terrain shoulders |
| Safe-ish overlook pads | 12-35 m across, slopes mostly <=8° |
| Ash/cinder slopes | 25-32° where loose cone/apron behavior is intended |
| Talus / collapse apron slopes | 28-38°, roughened and interrupted by shelves |
| Hard blocker slopes | sustained >52° or cliff/drop geometry exceeding defined drop criteria |
| Fissure width/depth | 2-18 m wide, 3-25 m deep depending on blocker role |
| Upper flow scar/channel length | 1.5-4.5 km from active zone toward lower fan terrain |
| Coastal/lower termination | tapered fan or channel merge into sea-level/coastal shelf mask, never square world-edge cliffs |

The implementation should ship with developer-only evidence: slope heatmaps, height/topographic contours, mask overlays, collision blocker maps, crater cross-sections, sea-level compliance maps, and automated pass/fail metrics for unsafe crust, hard blockers, stable ridges, breach drainage, and coastal tapering.

## Terrain-Only Scope Boundary

This document specifies **terrain generation, geometry, collision shape, masks, and developer diagnostics only**.

Allowed:

- terrain heightfields;
- generated terrain meshes where UE Landscape heightfields cannot represent required blocker geometry;
- collision surfaces and hard-blocking terrain;
- crater, rim, breach, vent, fissure, shelf, talus, ash/cinder, lava/crust, lahar-channel, lava-flow-scar, and coastal-fan masks;
- developer-only debug materials/maps for slope, elevation, mask, blocker, and collision validation;
- deterministic command-line/headless validation outputs.

Forbidden in this scope:

- VFX, smoke, steam, sparks, embers, glow, heat haze, lava particles, or eruption effects;
- heat shaders, final lava materials, molten animation, emissive gameplay feedback, or post-process effects;
- audio;
- damage systems, survival mechanics, stamina penalties, burns, gas poisoning, or temperature mechanics;
- weather, foliage, ecology, fauna, NPCs, story, quests, interactables, or player UI;
- player-facing map, minimap, compass, GPS, objective marker, bearing indicator, clock, route UI, or orientation UI.

Terrain masks may reserve semantic hazard zones for future systems, but this PR should not implement those systems.

## Current Terrain Baseline

The prompt defines the available baseline as:

- **UE5 Linux-first first-person project**;
- **16.256 km square world**;
- **circular/organic sea-level island landmass**;
- **roughly 1400 m volcanic massif**;
- active crater zone is **inland and high elevation**;
- all volcanic scars, lava-flow channels, collapse channels, and lahar-related terrain must respect the island-scale sea-level mask;
- terrain generation is the only scope.

Assume the current terrain generator already has, or needs to expose, these baseline concepts:

1. `WorldExtentMeters = 16256`
2. `SeaLevelMeters = 0`
3. `IslandMask` where:
   - inside mask = land generation allowed;
   - outer island edge = beach/coastal shelf near sea level;
   - outside mask = ocean/below sea level;
   - no volcanic feature may raise terrain outside the land mask.
4. `MassifHeightMeters ~= 1400`
5. deterministic mask outputs suitable for UE Landscape import, collision generation, and debug visualization.

No current code/file names are assumed beyond that prompt.

## Major Implementation Goal

Implement a large, terrain-readable active volcanic summit zone that produces the following player-scale impressions without relying on final materials or VFX:

1. **The summit is active and geologically dangerous.**
   - Broken rim.
   - Nested pit.
   - collapsed shelves.
   - fissures.
   - lava-crust plate fields.
   - inaccessible drops.
   - ash/cinder/talus aprons.

2. **The volcano is not a theme-park bowl.**
   - Irregular crater outline.
   - asymmetric breach.
   - off-center vent.
   - multiple terrain processes.
   - believable drainage/flow alignment.
   - partial safe edges rather than a full circular walkway.

3. **The terrain is inspectable around its edges.**
   - stable approach ridges;
   - overlook pads;
   - safe-ish rim saddles;
   - physically legible hard blockers;
   - no invisible walls as the primary boundary technique.

4. **The volcano belongs to the island.**
   - breach and flow scars descend the massif;
   - lahar/collapse channels follow topographic lows;
   - scars taper into lower fans/coastal shelves;
   - no active terrain feature creates square-edge cliffs or raised land at world bounds.

## Island Mask And Sea-Level Constraints

The active crater is high-elevation, but its downstream scars must obey island-scale terrain rules.

### Required sea-level behavior

All lava-flow scars, lahar channels, collapse chutes, and debris fans must satisfy:

```text
if outside IslandMask:
    terrain height <= SeaLevelMeters - ocean_margin
    volcanic feature mask contribution = 0
else if near coastal shelf:
    channel/fan height blends toward local coastal terrain
    no raised volcanic ridge continues into ocean unless deliberately shaped as a coastal fan/delta inside mask
```

Recommended values:

| Parameter | Target |
|---|---:|
| Ocean margin outside island mask | 4-20 m below sea level |
| Coastal shelf blend width | 180-450 m |
| Volcanic scar fade before beach | 120-300 m unless forming a fan |
| Coastal fan max height above local shelf | 3-18 m |
| Coastal fan slope | mostly <=6°, local lobes <=12° |
| No-feature world-boundary safety band | 256-512 m from square world edge |

### Channel termination rule

Downslope volcanic terrain should terminate as one of these, selected by local topography:

1. **Coastal fan**
   - widening, flattening deposit;
   - shallow distributary grooves;
   - no hard square edge;
   - gradually merges with beach/coastal shelf.

2. **Valley merge**
   - lava/lahar scar becomes a normal drainage valley;
   - channel depth decreases;
   - wall steepness reduces;
   - mask fades before sea-level shelf.

3. **Older lava bench / low fan inside island mask**
   - small raised, rough lobe above shelf;
   - bounded by smooth beach transition;
   - not extended outside the land mask.

Do not allow a volcanic channel to reach the square world boundary as a raised strip, trench, or cliff. The edge of the simulated island is ocean/coastal terrain, not a map-cut volcanic wall.

### Drainage logic

Because USGS describes lahars as debris/mud flows that move down valleys and river channels, especially after loose volcanic material is eroded by rainfall on steep slopes, lahar-related geometry should be routed through existing or generated topographic lows rather than painted as random scars. [USGS-Lahars]

Recommended implementation:

1. Generate the crater/breach.
2. Compute/approximate downslope flow accumulation from the breach and side-vent field.
3. Select 1-3 primary flow corridors.
4. Carve each corridor with decreasing intensity downslope.
5. Widen and flatten each corridor into lower fan geometry.
6. Clip/fade masks against `IslandMask` and `CoastalShelfMask`.

## Recommended Active Crater / Vent Terrain Model

### Recommended model

Use a **hybrid model**:

```text
broken summit crater
+ off-center nested active vent / pit crater
+ one breached rim notch
+ upper-flank fissure / side vent field
+ crusted lava shelves and collapse terraces
+ downslope flow/lahar scars
```

This is stronger than any single option:

| Option | Use? | Reason |
|---|---:|---|
| Summit crater | Yes | Gives the 1400 m massif a clear active summit identity. |
| Breached crater | Yes | Prevents a perfect theme-park bowl and creates terrain drainage logic. |
| Side vent | Yes, subordinate | Adds upper-flank realism and alternative active loci. |
| Fissure field | Yes, short/upper | Supports fractured active-zone terrain without needing VFX. |
| Lava lake | Limited / implied | A huge open lake risks visual/VFX dependency; use nested crusted vent/pit floor instead. |
| Crusted lava shelf | Yes | Critical terrain-readable hazard geometry. |
| Full caldera | No for PR5 | Too large and dominant unless the whole island design changes. |

### Why this model fits

USGS describes lava flows as issuing from eruptive vents, with flow behavior controlled by lava type/viscosity, ground steepness, whether flow is a sheet/channel/tube, and effusion rate. Fluid basaltic flows can travel far, especially when channelized or tube-fed. [USGS-Lava-Flows] The active terrain should therefore include both **source geometry** and **flow-routing geometry**: vents, fissures, channels, shelves, and downslope scars.

USGS/HVO lava-lake observations identify moving crustal plates, spatter, and collapse scars on crater rims as notable active-crater features. This supports using a nested pit with crustal plate terrain, spatter/raised rim fragments, and collapse scars rather than a clean circular pool. [USGS-Halemaumau-Lava-Lake]

NPS describes cinder cones as having ash/cinder slopes governed by angle of repose, typically **25-32°**, and notes that side vent areas can contain lava lakes, spatter deposits, push-ups, and rafted flank fragments. [NPS-Cinder-Cones] That supports ash/cinder aprons, small side vents, rough welded/spatter ridges, and transported blocks as terrain forms.

### Terrain identity

The player should read the active zone as:

- **summit source**: broken crater and nested pit;
- **recent lava/crust zone**: platey, blocky, fissured crater floor and shelves;
- **collapse zone**: inward rim scars, drop shelves, talus wedges;
- **vent/fissure zone**: linear cracks and small raised spatter/cinder mounds;
- **escape/drainage path**: breach leading into channelized scars;
- **safe-ish observation terrain**: ridges and saddles that stay outside the unstable crust masks.

## Crater Rim And Breach Geometry

### Crater placement

Place the active crater slightly off the massif’s absolute centerline:

| Placement element | Target |
|---|---:|
| Crater center elevation | ~1260-1340 m |
| Highest rim point | ~1380-1420 m |
| Lowest breached rim point | ~1190-1280 m |
| Offset from massif center | 120-350 m |
| Breach direction | toward strongest natural downslope corridor, preferably not cardinal-perfect |

Avoid dead-center placement unless the rest of the massif is very asymmetric. A centered perfect bowl reads artificial.

### Main crater dimensions

Recommended first-pass dimensions:

| Dimension | Target |
|---|---:|
| Long axis | 620-780 m |
| Short axis | 460-620 m |
| Rim crest elevation variation | 40-130 m |
| Crater floor elevation | 1160-1260 m |
| Rim-to-floor relief | 90-180 m |
| Rim crest width | 30-90 m |
| Inner wall horizontal run | 140-260 m |
| Crater aspect ratio | 1.15-1.45 |
| Rim noise displacement | 25-80 m lateral, low-frequency only |

Keep the crater mostly below **1 km diameter** to preserve “large summit crater” identity rather than caldera identity. USGS notes that craters are smaller collapse/ejection depressions while calderas are generally larger and tied to major collapse. [USGS-Crater-Caldera]

### Rim shape

The rim should be generated from multiple overlapping fields:

1. **Base elliptical crater depression**
   - long axis aligned with massif stress/drainage direction;
   - not circular;
   - not perfectly symmetrical.

2. **Rim uplift**
   - raised ring, but discontinuous;
   - crest width varies;
   - crest has local saddles and missing chunks.

3. **Collapse bites**
   - inward scallops removed from the rim;
   - form shelf/drop/talus geometry;
   - strongest on breach-facing side and one opposing unstable wall.

4. **Spatter/cinder armor**
   - raised rough local ridgelets near vent-facing rim;
   - do not overuse as final material proxy;
   - geometry only.

5. **Overlook ridges**
   - 2-4 selected stable rim/ridge segments;
   - smoother crown;
   - outside hard-blocker/unstable masks.

### Breach geometry

The breach is the main escape/drainage scar from the crater. It should feel physically carved or collapsed, not like a tunnel cut.

| Breach element | Target |
|---|---:|
| Breach width at rim | 110-220 m |
| Breach floor width | 35-90 m |
| Breach vertical relief | 70-160 m |
| Breach axis length from crater to outer flank | 300-850 m |
| Breach inner floor slope | 10-22° |
| Breach sidewall slope | 35-60° |
| Breach sidewall hard-block sections | sustained >52° |
| Breach roughness amplitude | 2-12 m at 8-45 m wavelength |

The breach should:

- interrupt the rim completely;
- connect crater floor to upper-flow/lahar channel;
- have hard-block sidewalls where traversal would break scale;
- include talus/debris aprons at the toe;
- feed at least one primary downslope scar;
- taper into lower terrain before coast.

### Rim and crater wall slope bands

| Zone | Intended role | Slope target |
|---|---|---:|
| Stable approach ridge crown | inspectable route | <=12° average, local <=18° |
| Overlook pad | observation edge | <=8° average, local <=12° |
| Outer ash/cinder apron | loose flank terrain | 18-32° |
| Inner crater wall | dangerous descent | 32-50° |
| Collapse scarp | hard blocker | 50-70° |
| Vertical-ish cliff proxy | blocker/drop | >70° only in short, controlled segments |
| Talus wedge | broken debris | 28-38°, rough, interrupted |

Use hard geometry/slope rather than invisible blocker volumes wherever possible. Invisible blockers may be used as final safety net, but the visible terrain must justify the block.

## Lava / Crusted Surface Terrain Geometry

### Terrain-only interpretation of lava/crust

In this PR, “lava” means terrain forms and masks, not final molten rendering.

Represent active/recent lava areas as:

- crusted plates;
- black-surface placeholder mask only in debug;
- rafted slabs;
- crack networks;
- pressure ridges;
- levees;
- collapsed crust shelves;
- skylight-like roof failures;
- tube/channel scars;
- unsafe void/crust masks.

USGS notes that lava-flow movement is influenced by whether it is broad sheet flow, confined in a channel, or in a lava tube. [USGS-Lava-Flows] HVO describes skylights as openings where a lava tube roof collapsed or was pushed up by magma pressure, and also describes lava bench collapse near ocean entries. [USGS-Skylights] Terrain should therefore include channel/tube roof-collapse analogues as heightfield depressions, fissure openings, and shelf failures.

### Crater floor layout

Recommended crater floor zones:

```text
rim wall / talus apron
    -> rough collapsed shelf ring
        -> crusted lava plate field
            -> nested active vent / pit
```

Approximate dimensions:

| Zone | Radius/width target | Notes |
|---|---:|---|
| Talus apron below inner wall | 30-90 m | Broken debris wedge at crater wall base. |
| Collapse shelf ring | 40-130 m | Terraced offsets and abrupt steps. |
| Crusted plate field | 180-420 m across | Not perfectly flat; 0.5-5 m plate offsets. |
| Nested vent/pit | 130-220 m diameter | Off-center; should interrupt plate field. |
| Vent rim/spatter rise | 4-18 m high | Broken, asymmetric. |
| Vent inner drop | 35-90 m | Hard-blocked unless future design says otherwise. |

### Crusted plate geometry

Use layered procedural components:

1. **Base shelf surface**
   - slopes gently toward vent/pit and breach;
   - 2-8° broad gradient;
   - not a flat arena.

2. **Plate cells**
   - irregular cells 8-45 m across;
   - separated by crack depressions 0.4-4 m deep;
   - local plate tilt 1-9°;
   - occasional larger raft plates 45-90 m across.

3. **Pressure ridges**
   - narrow ridges 0.8-5 m high;
   - 3-15 m wide;
   - length 20-120 m;
   - align with flow direction or vent stress lines.

4. **Levees**
   - raised sides along channels;
   - 2-12 m high;
   - 10-45 m wide;
   - rough/asymmetric.

5. **Collapsed shelves**
   - abrupt height offsets 2-18 m;
   - local slopes >50° where hard-blocked;
   - talus at base.

6. **Open fissures**
   - 2-18 m wide;
   - 3-25 m deep;
   - discontinuous and branching.

### Avoiding material dependency

The lava/crust zone should still read as hazardous in untextured graybox by using:

- plate offsets visible in silhouette;
- crack depth visible in side profile;
- rim drops and shelves visible in topo map;
- roughness contrast between stable ridge and crust field;
- hard-block slope colors in debug;
- inaccessible vent pit geometry;
- channel levees and breach flow direction.

No “red glowing flat plane” assumption should be required.

### Upper lava channel / scar geometry

From the breach and side vent, generate 1-3 upper scars:

| Feature | Target |
|---|---:|
| Channel/scar length | 1.5-4.5 km, terrain-dependent |
| Upper channel width | 30-120 m |
| Lower fan width | 120-450 m |
| Channel depth | 4-35 m upper, fading downslope |
| Levee height | 2-18 m upper, fading downslope |
| Channel slope | follows existing terrain; avoid impossible uphill runs |
| Meander amplitude | 20-120 m |
| Roughness | high in upper lava/crust; lower in fan |

Use the topography to guide flow. Research on lava/topography interaction shows that depressions can thicken/accelerate flows, while elevated mounds can divert flows and create protected areas with faster/deeper flow around margins. [Saville-2022-Lava-Topography] For Jungle Game, this means:

- lava/crust channels should prefer topographic lows;
- raised stable ridges can plausibly remain lava-free;
- flow margins near ridges should be rougher/deeper;
- safe overlook ridges should be uphill/lateral to flow scars, not randomly placed in the center.

## Fissure / Vent / Collapse Terrain Geometry

### Fissure field

Add a short fissure system aligned with the breach and massif stress direction.

Recommended targets:

| Element | Target |
|---|---:|
| Fissure field start | 150-450 m from crater center |
| Fissure field length | 500-1100 m |
| Number of major fissures | 2-5 |
| Major fissure length | 80-260 m |
| Minor crack length | 15-90 m |
| Width | 2-18 m |
| Depth | 3-25 m |
| Edge slope | 45-75° for hard-blocked cracks |
| Alignment jitter | 8-25° from main stress axis |

Fissures should be discontinuous, stepped, and partially filled in places. Avoid making them look like clean trenches.

### Side vent / spatter cone

Place one main upper-flank side vent plus smaller cracks.

Recommended dimensions:

| Element | Target |
|---|---:|
| Side vent distance from crater rim | 350-900 m |
| Side vent basal diameter | 70-180 m |
| Side vent height above local terrain | 12-45 m |
| Bowl/pit diameter | 25-80 m |
| Bowl/pit depth | 8-35 m |
| Spatter ridge height | 3-16 m |
| Cinder apron slope | 20-32° |

NPS notes that cinder cone side vent areas can be complex, with lava lakes, spatter deposits, push-ups, and rafted flank pieces. [NPS-Cinder-Cones] Use that as permission for rough terrain complexity, not for overbuilding a second full volcano.

### Collapse shelves

Collapse shelves make the terrain feel active and unsafe.

Use:

- rim-bite scallops;
- inward-rotated ledges;
- abrupt floor offsets;
- talus wedges under scarps;
- crust plate sagging toward vent;
- broken steps at breach entrance;
- skylight-like holes where channel/tube roofs failed.

Recommended dimensions:

| Collapse feature | Target |
|---|---:|
| Small shelf height offset | 1-4 m |
| Major shelf height offset | 5-18 m |
| Shelf width | 8-60 m |
| Shelf length | 30-220 m |
| Talus wedge length | 12-80 m |
| Hard scarp slope | >52° |
| Scallop bite into rim | 20-100 m |

### Nested active vent / pit

The active pit is the terrain heart of the crater.

Recommended targets:

| Element | Target |
|---|---:|
| Pit diameter | 130-220 m |
| Pit shape | oval/irregular, not circular |
| Pit drop below crater floor | 35-90 m |
| Pit wall slope | 45-75° |
| Pit rim offset/raised lip | 3-18 m |
| Pit floor | not generally accessible |
| Connection to fissures | 1-3 radial cracks |
| Connection to breach | subtle floor gradient/channel |

A visible central pit avoids needing a large exposed lava lake. If future visuals add molten lava, the terrain can support it, but the PR5 terrain must already function without it.

## Stable Approach Versus Unstable Terrain

### Stable approach terrain

Stable routes should exist, but only in selected ridges and overlooks. The player should not be able to casually walk the full crater rim.

Stable terrain criteria:

| Criterion | Target |
|---|---:|
| Main approach ridge usable crown | 12-30 m wide |
| Minimum pinch width | >=6 m for short segments only |
| Average slope | <=12° |
| Local slope | <=18° over short spans |
| Cross-slope | <=10° preferred |
| Roughness amplitude | <=0.7 m at player-foot scale |
| Edge buffer from hard drop | 4-12 m where possible |
| Connection to overlook pads | 1-3 pads, not continuous ring |

The stable approach should lead to controlled views:

1. **Primary overlook**
   - views nested pit and crusted floor;
   - separated from unstable crust by hard scarp/fissure/berm;
   - no invisible wall needed as primary barrier.

2. **Breach overlook**
   - views flow channel descending the island;
   - shows terrain relation between crater and coast.

3. **Side-vent overlook**
   - optional;
   - views fissure/vent field from above or lateral ridge.

### Unstable terrain

Unstable zones are terrain-readable “do not enter” areas.

Unstable terrain criteria:

| Zone | Geometry signal |
|---|---|
| Crusted plate field | offset slabs, cracks, roughness, sagging toward vent |
| Open fissure | width/depth beyond traversal, steep edges |
| Collapse shelf | abrupt ledges and talus |
| Vent pit | steep walls, nested drop |
| Breach sidewall | slope/height hard-blocker |
| Ash/cinder apron | angle-of-repose loose slopes and erosion gullies |
| Lava tube/skylight analogue | collapsed roof holes, rimmed openings |
| Coastal lava bench analogue | low fan/bench with unstable edge only inside island mask |

### Transition design

Stable and unstable terrain should not meet as a texture-only boundary. Use geometry transitions:

```text
stable ridge crown
  -> rough shoulder
    -> shallow crack / sag
      -> hard scarp or fissure
        -> unstable crust field
```

Recommended transition width: **12-45 m**.

This prevents a cheap “walkable path next to flat lava texture” look.

## Hard-Blocker Terrain Criteria

Hard blockers should be generated from terrain geometry first, with optional invisible collision only as a failsafe.

### Hard blocker definitions

A terrain area should be flagged `HardBlockerMask = 1` if any of these are true:

| Condition | Pass/fail threshold |
|---|---:|
| Sustained slope too steep | >52° for >=8 m horizontal span |
| Severe local cliff | >70° for >=2 m vertical relief |
| Drop ledge | >=3 m vertical drop over <=2 m run |
| Major crater/vent wall | pit/crater wall zone with slope >45° and drop >8 m |
| Fissure gap | width >=2.5 m and depth >=4 m |
| Collapse void/skylight | opening width >=3 m and depth >=3 m |
| Lava bench/coastal edge | unstable bench drop >=2.5 m inside island mask |
| Breach sidewall | slope >52° or drop >6 m |
| World-boundary protection | any volcanic feature within no-feature boundary band |

### Recommended blocker types

| Blocker type | Terrain form | Notes |
|---|---|---|
| Crater wall blocker | steep inner wall + talus toe | Use slope and drop, not invisible wall. |
| Vent pit blocker | nested depression with steep wall | Must remain readable in debug and graybox. |
| Fissure blocker | open crack with vertical-ish sides | Avoid ultra-thin cracks that collision misses. |
| Collapse shelf blocker | abrupt step/ledge | Support with talus apron below. |
| Breach sidewall blocker | steep channel walls | Leave breach floor visible but not necessarily traversable. |
| Crust instability blocker | plate offset + crack web | Use as partial blocker, not full flat no-go paint. |
| Coastal feature blocker | fan/bench edge inside island mask | Must not extend outside island mask. |

### UE5 collision implications

UE Landscape collision is a heightfield, and Epic documents that Landscape uses a heightfield object for collision; each target layer can specify a physical material and collision can use the dominant layer at a position. [Epic-Landscape-Overview]

Practical implications:

1. **No true overhangs in Landscape heightfields.**
   - Avoid requiring undercut shelves or cave roofs in the base landscape.
   - Represent collapsed roofs/skylights as open pits, cracks, and steep-sided depressions.

2. **Thin cracks must be wide enough for collision resolution.**
   - Fissures should be several landscape samples wide.
   - Recommended minimum: 3-5 vertices across the opening at target import resolution.
   - If a 1 m/vertex landscape is used, minimum reliable fissure width is 4-8 m.
   - If coarser, scale widths upward.

3. **Hard blockers need geometry margin.**
   - A 1 m ledge may be visually readable but unreliable as a traversal blocker.
   - Use >=3 m drops for major hard blockers.

4. **Physical material/layer masks can label hazard terrain.**
   - This PR should not implement gameplay damage.
   - It can set or export dominant-layer masks for future hazard semantics.

5. **If generated mesh terrain is used for blocker lips**, keep it terrain-owned:
   - WorldStatic collision;
   - deterministic generation;
   - no decorative prop dependency;
   - included in debug collision validation.

Epic’s Landscape Technical Guide also documents UE’s heightmap value range and Z-scale conversion, which matters if the island is imported as a 16-bit heightmap with 1400 m above sea level plus below-sea ocean room. [Epic-Landscape-Technical]

## Terrain Masks And Debug Channels

The PR should produce deterministic masks as first-class outputs, not side effects.

### Required masks

| Mask | Purpose |
|---|---|
| `IslandMask` | Authoritative land/ocean boundary. |
| `CoastalShelfMask` | Sea-level beach/shelf blending zone. |
| `MassifMask` | Volcanic massif influence area. |
| `ActiveCraterMask` | Main crater footprint. |
| `CraterFloorMask` | Lower interior floor. |
| `CraterWallMask` | Inner and outer wall slopes. |
| `RimCrestMask` | Broken rim crest terrain. |
| `RimBreachMask` | Breach notch and upper drainage. |
| `NestedVentPitMask` | Active pit/drop zone. |
| `VentRimMask` | Raised lip/spatter around nested pit. |
| `LavaCrustMask` | Crusted floor/shelf/lava surface geometry. |
| `UnstableCrustMask` | Crust zones intended as unsafe or future hazard areas. |
| `CrustPlateMask` | Plate-cell surface regions. |
| `CrackNetworkMask` | Fine-to-medium crust cracks. |
| `OpenFissureMask` | Major open fissures/gaps. |
| `FissureFieldMask` | Broader fissure/side-vent field. |
| `SideVentMask` | Upper-flank vent/cone/bowl. |
| `SpatterCinderMask` | Spatter ridges and cinder aprons. |
| `CollapseShelfMask` | Terraces, rim bites, shelf offsets. |
| `TalusApronMask` | Debris wedges under scarps. |
| `AshCinderSlopeMask` | Loose ash/cinder slopes, target 25-32° where cone-like. |
| `LavaFlowScarMask` | Downstream lava-flow scar corridors. |
| `LaharChannelMask` | Downstream debris-flow / erosion-channel corridors. |
| `CoastalFanTerminationMask` | Lower fan where scars taper into coastal shelf. |
| `StableApproachRidgeMask` | Inspectable approach ridges. |
| `SafeOverlookMask` | Developer-defined safe-ish overlook pads. |
| `HardBlockerMask` | Terrain-owned hard blockers. |
| `NoFeatureBoundaryMask` | World-edge/ocean safety exclusion. |

### Optional masks

| Mask | Purpose |
|---|---|
| `FlowDirectionMask` | Encoded or visualized flow routing from breach/vents. |
| `RimInstabilityMask` | Rim regions likely to be collapsed/blocked. |
| `FutureHazardSemanticMask` | Reserved non-gameplay semantic tag for future systems. |
| `DebugContourMask` | Thin contour lines for screenshot evidence. |
| `TraversalCandidateMask` | Developer-only route sanity visualization. |

### Mask precedence

Recommended precedence from highest to lowest:

1. `IslandMask` / ocean clipping
2. `NoFeatureBoundaryMask`
3. `HardBlockerMask`
4. `NestedVentPitMask`
5. `OpenFissureMask`
6. `CollapseShelfMask`
7. `RimBreachMask`
8. `CraterWallMask`
9. `LavaCrustMask`
10. `AshCinderSlopeMask`
11. `LavaFlowScarMask` / `LaharChannelMask`
12. `CoastalFanTerminationMask`
13. `StableApproachRidgeMask`
14. `SafeOverlookMask`

Stable masks should never silently override hard blockers. If a stable route intersects a hard blocker, that is a validation failure unless the blocker is deliberately carved away and regenerated.

### Debug output requirements

Generate these developer-only artifacts:

| Output | Purpose |
|---|---|
| `topo_active_crater.png` | Elevation contours and crater/breach relation. |
| `slope_active_crater.png` | Slope bins for walkable/danger/blocker validation. |
| `masks_active_crater.png` | Combined color mask overlay. |
| `hard_blocker_active_crater.png` | Hard blocker terrain/collision mask. |
| `sea_level_compliance.png` | Island/ocean/coastal taper validation. |
| `crater_cross_sections.png` | North-south/east-west and breach-axis elevation profiles. |
| `flow_scar_profiles.png` | Longitudinal profiles from breach to lower fan/coast. |
| `collision_probe_report.json` | Numeric route/blocker/collision checks. |
| `mask_area_stats.json` | Area and overlap stats for all masks. |
| `terrain_validation_summary.md` | Human-readable pass/fail evidence. |

These artifacts are not player UI.

## Collision And Slope Validation Requirements

### Slope bins

Compute slope in degrees from the generated heightfield:

```text
slope_degrees = atan(local_gradient) * 180 / pi
```

Recommended debug bins:

| Slope bin | Meaning |
|---|---|
| 0-8° | flat / overlook-capable |
| 8-12° | comfortable traversal |
| 12-18° | rough traversal / ridge approach |
| 18-25° | steep but possible in limited areas |
| 25-32° | ash/cinder angle-of-repose target |
| 32-38° | talus / unstable slope |
| 38-52° | dangerous slope / transition blocker |
| 52-70° | hard blocker |
| >70° | cliff/drop proxy; use sparingly |

### Validation checks

The implementation should fail if:

1. **Crater dimensions out of range**
   - main crater long axis <500 m or >950 m;
   - short axis <350 m or >800 m;
   - crater becomes a near-perfect circle unless explicitly intended;
   - nested pit missing.

2. **Rim too clean**
   - rim crest is continuous and uniform for >70% of circumference;
   - no breach;
   - no collapse bites;
   - no saddle variation.

3. **No stable inspection**
   - no stable approach ridge reaches a crater overlook;
   - overlook pads exceed slope thresholds;
   - stable route crosses unstable crust or fissure masks.

4. **Hard blockers unreadable**
   - hard blocker mask exists but terrain slope/drop does not justify it;
   - invisible collision does most of the work;
   - fissures are too narrow to be collision-reliable.

5. **Crust field too flat**
   - lava/crust area lacks plate offsets, cracks, shelves, or roughness;
   - nested vent pit is absent or just a texture/mask.

6. **Flow scars ignore topography**
   - scars climb uphill without explanation;
   - scars are straight cardinal lines;
   - channels do not connect to breach/side vents;
   - channels do not taper downslope.

7. **Sea-level violation**
   - volcanic feature raises terrain outside `IslandMask`;
   - flow/lahar channel forms cliff at world bounds;
   - coastal shelf has square-edge volcanic cuts;
   - outside-island terrain is above sea level.

8. **Theme-park crater**
   - perfect circular rim;
   - full walkable ring;
   - flat lava pool center;
   - symmetrical bridges/ramps;
   - hazard boundary depends on future VFX/materials.

### Collision probes

Run automated probes over:

| Probe | Expected result |
|---|---|
| Stable ridge path | continuous, slope-compliant, no hard blocker overlap |
| Primary overlook pad | flat enough, adequate area, bounded by readable terrain |
| Crater wall descent | blocked by slope/drop before reaching active pit |
| Vent pit edge | blocked by terrain/collision |
| Major fissures | cannot be crossed if flagged hard blocker |
| Breach sidewalls | blocked except any deliberate breach-floor route |
| Unstable crust | not classified as stable traversal |
| Coastal fan | walkable/soft terrain only if not active hazard; no mask outside island |
| Ocean/outside mask | below sea level and no volcanic feature contribution |

## Developer-Only Topographic Validation Targets

### Required numeric evidence

Produce a validation summary with at least:

| Metric | Target |
|---|---:|
| Main crater long axis | 620-780 m preferred |
| Main crater short axis | 460-620 m preferred |
| Main crater relief | 90-180 m |
| Nested pit diameter | 130-220 m |
| Nested pit depth | 35-90 m |
| Breach width | 110-220 m at rim |
| Breach relief | 70-160 m |
| Stable overlook count | 1-3 |
| Safe overlook average slope | <=8° |
| Stable ridge average slope | <=12° |
| Hard blocker terrain justification | >=95% of hard blocker cells meet slope/drop/fissure criteria |
| Ash/cinder area in 25-32° bin | >=50% of `AshCinderSlopeMask` preferred |
| Crust plate area with offset/crack influence | >=60% of `LavaCrustMask` |
| Unstable crust overlap with stable masks | 0% except approved transition cells |
| Volcanic feature outside island mask | 0 cells |
| Raised feature inside no-feature world-boundary band | 0 cells |
| Coastal fan/channel taper | channel depth/levee height trends downward within final 300 m before shelf |

### Required visual evidence

Commit or attach validation images showing:

1. elevation/topographic map of active crater;
2. slope heatmap;
3. mask overlay;
4. hard-blocker overlay;
5. crater cross-section through nested pit;
6. breach-axis cross-section into upper flow scar;
7. full island map showing flow/lahar scars tapering before sea-level shelf;
8. ocean/outside-mask compliance.

### Cross-section targets

Required profiles:

| Profile | Must show |
|---|---|
| Main crater long axis | asymmetric rim, floor, nested pit, opposite wall. |
| Main crater short axis | broken rim, wall slopes, talus/shelves. |
| Breach axis | rim notch, breach floor, channel, downstream taper. |
| Stable overlook axis | safe pad, edge blocker, crater interior beyond. |
| Side vent axis | cone/bowl/fissure relation. |
| Coastal fan axis | scar/channel shallowing into coastal terrain. |

USGS notes that DEM comparison and topographic analysis are used to measure volcanic surface change, volumes, debris-flow thickness, stream-channel shape, and volcanic hazard maps. [USGS-DEMs] Jungle Game should copy that discipline in developer diagnostics: the PR should prove the terrain with DEM-style maps and profiles, not only beauty screenshots.

## Large PR Implementation Scope

This is a large terrain-generation PR. It should be implemented as a coherent terrain module, not scattered one-off noise patches.

### Recommended implementation phases

#### Phase 1 — Active-zone layout

- Define active crater center, axes, elevation band, rim elevation targets.
- Define breach direction from local/topographic drainage.
- Define side-vent/fissure field alignment.
- Reserve stable approach ridges and overlook candidates early so hazard geometry does not consume all access.

#### Phase 2 — Heightfield morphology

- Generate crater depression.
- Generate broken rim uplift.
- Cut breach notch.
- Add nested vent/pit.
- Add crater wall/talus/collapse shelves.
- Add crusted crater floor plate offsets.
- Add fissures and side-vent terrain.
- Add lava-flow/lahar scars and lower coastal fan termination.
- Re-apply island/ocean/sea-level constraints.

#### Phase 3 — Masks

- Emit all required masks.
- Validate mask precedence.
- Validate stable/unstable overlap.
- Validate hard blockers against terrain evidence.
- Validate sea-level clipping.

#### Phase 4 — Collision

- Generate Landscape collision from heightfield.
- Add terrain-owned collision only where needed for generated mesh blocker lips.
- Probe traversal and blocker scenarios.
- Export collision report.

#### Phase 5 — Diagnostics

- Generate PNG overlays.
- Generate JSON stats.
- Generate Markdown validation summary.
- Save cross-section plots.
- Include pass/fail summary in PR evidence.

### Determinism

All active-crater generation must be deterministic under seed/config:

```text
same seed + same config + same height resolution = same terrain + same masks + same diagnostics
```

Use stable procedural noise. Avoid runtime-random crater damage.

## Files And Areas Likely Touched

Exact repo paths are unknown from this prompt, so these are implementation areas rather than assumed filenames.

Likely touched areas:

1. **Terrain generation config**
   - active crater dimensions;
   - massif height/elevation band;
   - breach and side-vent parameters;
   - mask names;
   - validation thresholds.

2. **Heightfield generator**
   - crater depression function;
   - rim uplift/noise;
   - breach carving;
   - nested pit carving;
   - shelf/drop/talus generation;
   - lava/crust plate displacement;
   - fissure/channel carving;
   - downstream scar/fan taper;
   - sea-level mask enforcement.

3. **Mask generator/exporter**
   - named mask outputs;
   - overlap/precedence rules;
   - debug color mapping;
   - area statistics.

4. **UE Landscape import/collision setup**
   - height range/Z scale;
   - layer/physical material placeholders;
   - collision resolution settings;
   - generated mesh collision only if needed.

5. **Developer diagnostics**
   - slope heatmap;
   - topographic contours;
   - hard-blocker map;
   - cross-section plotter;
   - sea-level compliance plot;
   - JSON/Markdown validation reports.

6. **Tests/validation**
   - deterministic generation test;
   - crater dimension test;
   - mask overlap test;
   - hard-blocker slope/drop test;
   - sea-level/ocean clipping test;
   - stable route/overlook test.

### Do not touch in this PR

- final material art;
- particles/VFX;
- sound;
- damage/survival systems;
- player-facing UI;
- quest/story systems;
- foliage/ecology;
- NPCs.

## Acceptance Criteria

The PR is acceptable only if all required terrain and validation outputs exist.

### Terrain acceptance

- [ ] Main crater exists at high elevation and is not a perfect circular bowl.
- [ ] Crater dimensions fall within target range or are justified in config.
- [ ] Rim is broken, discontinuous, and has at least one major breach.
- [ ] Nested active vent/pit exists and is hard-blocked by terrain/collision.
- [ ] Crater floor contains crusted plate/shelf/fissure geometry independent of materials.
- [ ] Collapse shelves and talus aprons exist on crater wall or rim sections.
- [ ] Side vent or fissure field exists as a subordinate upper-flank feature.
- [ ] At least one stable approach ridge reaches a safe-ish overlook.
- [ ] The full rim is not trivially walkable.
- [ ] Hard blockers are physically readable from terrain shape.
- [ ] Lava-flow/lahar/collapse scars descend from breach/vent terrain.
- [ ] Downstream scars taper into lower terrain/coastal fans.
- [ ] No volcanic feature raises terrain outside the island mask.
- [ ] No active feature creates square-edge cliffs at world bounds.

### Mask acceptance

- [ ] Required masks are emitted with stable names.
- [ ] `HardBlockerMask` has terrain justification.
- [ ] `StableApproachRidgeMask` and `SafeOverlookMask` do not overlap major unstable/hard masks.
- [ ] `LavaCrustMask`, `UnstableCrustMask`, `OpenFissureMask`, `NestedVentPitMask`, and `RimBreachMask` are separable in debug.
- [ ] `CoastalFanTerminationMask` shows fade/merge into sea-level terrain.
- [ ] `NoFeatureBoundaryMask` prevents world-edge volcanic artifacts.

### Collision acceptance

- [ ] UE Landscape collision or terrain-owned collision blocks crater pit, major fissures, collapse drops, and breach sidewalls.
- [ ] Stable approach ridge probes pass.
- [ ] Overlook pads pass slope and area checks.
- [ ] Major hard-block zones cannot be crossed by ordinary traversal probes.
- [ ] Collision evidence is exported in JSON or equivalent.

### Diagnostic acceptance

- [ ] Topographic contour map exported.
- [ ] Slope heatmap exported.
- [ ] Combined mask overlay exported.
- [ ] Hard-blocker overlay exported.
- [ ] Sea-level compliance map exported.
- [ ] Crater and breach cross-sections exported.
- [ ] Numeric validation summary exported.
- [ ] All outputs are developer-only and not player-facing UI.

## Risks And Stop Conditions

### Risks

1. **Perfect crater / theme-park result**
   - Risk: symmetrical bowl with flat lava center.
   - Mitigation: force asymmetry, breach, nested pit, collapse shelves, and partial rim access.

2. **Terrain too dangerous to inspect**
   - Risk: active zone becomes unreachable or unreadable.
   - Mitigation: reserve stable ridges/overlooks before hazard carving.

3. **Hard blockers depend on invisible volumes**
   - Risk: collision works but geometry does not justify it.
   - Mitigation: validate blocker terrain by slope/drop/fissure criteria.

4. **Fissures too thin for collision**
   - Risk: visual cracks are crossed or missed by Landscape collision.
   - Mitigation: minimum fissure width tied to terrain resolution.

5. **Lava/crust depends on future materials**
   - Risk: graybox terrain does not read as hazardous.
   - Mitigation: plate offsets, crack depths, shelf drops, and roughness must be visible in heightfield.

6. **Coastline/sea-level violation**
   - Risk: flow scars create raised strips or cliffs at world bounds.
   - Mitigation: strict island mask clipping and coastal fan taper validation.

7. **Over-large caldera**
   - Risk: crater consumes massif or becomes a full island feature.
   - Mitigation: keep main crater mostly <1 km across unless design explicitly shifts to caldera-scale volcano.

8. **Landscape heightfield limits**
   - Risk: desired overhang/cave-like collapse cannot be represented.
   - Mitigation: use open pits, cracks, steep scarps, and terrain-owned meshes only where justified.

### Stop conditions

Stop implementation and report if:

- the island mask/coastal shelf data is unavailable or ambiguous;
- heightfield resolution is too coarse to represent fissures/blockers at playable scale;
- generated crater cannot fit on the massif without breaking sea-level/coast constraints;
- stable approach ridges cannot be preserved without crossing hard hazard masks;
- UE collision settings make the generated hard blockers unreliable;
- validation cannot produce required slope/mask/cross-section evidence.

## Rejected Or Forbidden Approaches

Reject these approaches:

1. **Tiny decorative cone**
   - Does not meet major active-volcano terrain requirement.

2. **Perfect circular crater**
   - Reads artificial and theme-park-like.

3. **Flat lava pool with no terrain structure**
   - Depends on future visuals and fails terrain-only scope.

4. **Full walkable rim loop**
   - Removes hazard and scale unless explicitly designed as a separate trail system later.

5. **Invisible-wall-first hazard boundary**
   - Visible terrain must explain blockers.

6. **Texture-only lava/crust**
   - Lava/crust must be geometry and masks in this PR.

7. **World-edge volcanic cliffs**
   - Violates island mask/coast requirement.

8. **Raised lava scars outside island mask**
   - Ocean/outside mask must remain below sea level.

9. **Straight cardinal-direction lava channels**
   - Flow scars should follow terrain and breach/vent logic.

10. **Player-facing orientation UI**
    - No map/minimap/compass/GPS/objective/bearing/time/orientation UI in this scope.

11. **VFX/material/audio/gameplay hazard implementation**
    - Reserve semantic masks only; do not implement those systems here.

12. **Huge caldera without design approval**
    - A caldera-scale collapse would alter the island identity and should be a separate design decision.

## Sources And References

- [USGS-Lava-Flows] U.S. Geological Survey, Volcano Hazards Program. “Lava flows destroy everything in their path.” Notes that lava-flow speed depends on lava type/viscosity, ground steepness, sheet/channel/tube form, and eruption rate; basaltic flows can extend tens of kilometers and move faster when confined in channels/tubes on steep slopes.  
  https://www.usgs.gov/programs/VHP/lava-flows-destroy-everything-their-path

- [USGS-Crater-Caldera] U.S. Geological Survey, Yellowstone Volcano Observatory. “Caldera or crater…what’s the difference?” Defines crater as a generic enclosed depression and notes calderas are generally larger, collapse-associated structures, often more than about 1 km across.  
  https://www.usgs.gov/observatories/yvo/news/caldera-or-craterwhats-difference

- [USGS-Lahars] U.S. Geological Survey, Volcano Hazards Program. “Lahars move rapidly down valleys like rivers of concrete.” Describes lahars/debris flows forming from loose volcanic material, rainfall, steep slopes, landslides, and channelized valley flow.  
  https://www.usgs.gov/programs/VHP/lahars-move-rapidly-down-valleys-rivers-concrete

- [USGS-DEMs] U.S. Geological Survey, Volcano Hazards Program. “Digital Elevation Models (DEMs) map changes to a volcano’s topography.” Describes DEMs for measuring lava volume, dome growth, debris-flow thickness, stream-channel shape, and volcanic hazard maps.  
  https://www.usgs.gov/programs/VHP/digital-elevation-models-dems-map-changes-volcanos-topography

- [USGS-Skylights] U.S. Geological Survey / Hawaiian Volcano Observatory. “Volcano Watch — Skylights mark current Kīlauea lava flows.” Describes lava-tube skylights as openings where tube roofs collapse or are pushed up, plus lava-bench collapse hazards near ocean entries.  
  https://www.usgs.gov/news/volcano-watch-skylights-mark-current-kilauea-lava-flows

- [USGS-Halemaumau-Lava-Lake] U.S. Geological Survey media page. “USGS Scientist Talks About Lava Lake in Halema‘uma‘u Crater.” Mentions crustal plates, spatter, collapse scars on crater rim, and hazards near the lake.  
  https://www.usgs.gov/media/videos/usgs-scientist-talks-about-lava-lake-halemaumau-crater

- [NPS-Cinder-Cones] U.S. National Park Service. “Cinder Cones.” Gives ash/cinder angle-of-repose slopes of 25-32° and describes side-vent complexity including lava lakes, spatter deposits, push-ups, and rafted flank pieces.  
  https://www.nps.gov/articles/000/cinder-cones.htm

- [Saville-2022-Lava-Topography] Saville, J. M., Hinton, E. M., & Huppert, H. E. “Predicting Safe Regions Within Lava Flows Over Topography.” *Journal of Geophysical Research: Solid Earth*, 2022. Describes how topographic depressions and mounds influence lava depth, speed, diversion, and protected regions.  
  https://agupubs.onlinelibrary.wiley.com/doi/full/10.1029/2022JB024167

- [Epic-Landscape-Overview] Epic Games. “Landscape Overview.” Documents UE Landscape using a heightfield object for collision and dominant target layers for physical materials.  
  https://dev.epicgames.com/documentation/unreal-engine/landscape-overview

- [Epic-Landscape-Technical] Epic Games. “Landscape Technical Guide.” Documents Landscape heightmap value range and Z-scale conversion for custom terrain heights.  
  https://dev.epicgames.com/documentation/unreal-engine/landscape-technical-guide-in-unreal-engine
