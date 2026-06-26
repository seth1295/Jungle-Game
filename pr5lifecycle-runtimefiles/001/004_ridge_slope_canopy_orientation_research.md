# Ridge Slope Canopy Orientation Research

## Executive Summary

Jungle Game should treat orientation as an **environment-reading problem**, not a UI problem. The player should learn direction through repeated, imperfect evidence: ridge lines that hold elevation, spurs that branch downhill, saddles that create recognizable passes, creek flow that trends toward lower land and eventually coast, canopy gaps that expose fragments of sky, sun and shadow that vary by time/season, and night cues that appear only through narrow sky windows.

The strongest no-map orientation design is a **layered cue model**:

1. **Terrain backbone**: ridges, spurs, saddles, gorges, creek junctions, slope breaks, and mountain silhouettes.
2. **Canopy/sky evidence**: canopy closure, treefall gaps, cyclone scars, skyline slots, emergent crowns, and brief horizon glimpses.
3. **Light/weather evidence**: sun angle, shadow direction, diffuse overcast light, fog distance falloff, wet surfaces, moonlight, and stars.
4. **Traversal pressure**: fatigue, footing risk, slope feel, wind exposure, vegetation density, drainage, and acoustic directionality.

The system must never answer “am I oriented?” for the player. Recovery should come from **designed redundancy**, not feedback: ridges reconnect to landmarks, creeks converge, saddles open skyline windows, coast/ocean sound grows stronger near drainage exits, and weather occasionally degrades then later reveals evidence. Topographic map principles support this approach because contour-derived terrain features such as ridges, valleys, saddles, draws/gullies, spurs, and slope steepness correspond to persistent landform patterns that players can learn visually and kinesthetically even without seeing a map. ([pubs.usgs.gov](https://pubs.usgs.gov/gip/TopographicMapSymbols/topomapsymbols.pdf?utm_source=chatgpt.com))

For implementation, build a developer-only evidence layer over terrain and atmosphere systems: **orientation cue volumes, canopy masks, terrain-form masks, weather degradation states, and debug view modes**. These must remain invisible to players. The player-facing game should expose only the world: landform shape, vegetation, light, shadow, sound, water, and motion cost.

## Design Goals For Jungle Game

### Core goals

- Make navigation feel **natural, embodied, and fallible**.
- Support no-map orientation through **landform memory**, not UI.
- Preserve dense tropical jungle readability without making it behave like a temperate open woodland.
- Let canopy and weather alternately **hide, filter, distort, and reveal** orientation evidence.
- Avoid explicit confirmation. The game should never say “you are going north,” “you are lost,” or “this is the correct ridge.”
- Make recovery possible through ecological and geomorphic logic: water flows downhill, ridges divide drainage, saddles connect high ground, canopy gaps admit light, exposed ridges carry more wind, gullies concentrate water and sound.

### Required player-facing absence

Forbidden player-facing aids:

- map;
- minimap;
- compass;
- bearing number/readout;
- GPS;
- objective marker navigation;
- HUD clock;
- explicit time readout;
- orientation confidence UI;
- hidden orientation meter;
- character comments that tell the player whether they are oriented.

### Positive design target

The intended player question should be:

> “What does this place tell me?”

Not:

> “What does the UI say?”

### Tropical adaptation

Dense rainforest should not be implemented as a generic green maze. Tropical rainforest structure is vertically layered, with emergents, canopy, understory, and forest floor; the canopy forms a dense roof that blocks much sunlight, rainfall, and wind, producing darker, stiller, more humid understory conditions. ([wettropics.gov.au](https://www.wettropics.gov.au/rainforest-structure?utm_source=chatgpt.com))

For Jungle Game this means:

- direct sun evidence is **rare but powerful**;
- sky windows are **valuable navigational events**;
- ground-level visibility is often short;
- horizon cues are normally occluded except at ridges, gaps, beaches, cliffs, river mouths, landslides, treefall zones, and saddles;
- vegetation transition should be tied to topography, moisture, disturbance, and exposure, not random decoration.

## Ridge And Slope Orientation Principles

### Terrain features as orientation anchors

Use terrain as the most reliable orientation substrate. The player cannot see the map, but the world can still be built from readable map-like landforms.

Recommended terrain feature categories:

| Category | Player-readable evidence | Orientation role |
|---|---|---|
| Main ridge | long elevated crest, falling ground on both sides, more wind, occasional canopy breaks | stable route spine; divides drainage basins |
| Spur | smaller descending ridge branching from main ridge | descent/ascent route; can mislead if confused with main ridge |
| Saddle | dip between higher points, pass-like crossing, two uphill and two downhill tendencies | landmark, route decision point, skyline window opportunity |
| Gully/draw | small incised drainage, damp footing, water after rain, enclosed sound | descent cue, creek-finding cue, occlusion/degradation zone |
| Valley/creek corridor | lower ground, flowing water, denser humidity, sediment/debris | downstream route, but often slow and visually confined |
| Slope break | transition from steep to gentle ground, terrace, bench, cliff base | memory anchor; route contouring opportunity |
| Peak/knoll | local high point, radial falloff, emergent silhouette potential | broad orientation reset if visible |
| Gorge | steep enclosed drainage, echoing water, narrow sky | strong linear guide but poor horizon/sky cue |
| Coastal escarpment | ocean sound, salt wind, abrupt vegetation transition | macro-orientation toward island edge |

Topographic references define contour lines as equal-elevation lines and use spacing to indicate steepness; close contours mean steeper slopes, while terrain-feature patterns identify ridges, valleys, saddles, spurs, and gullies. Those principles should be translated into player-facing geometry rather than UI. ([pubs.usgs.gov](https://pubs.usgs.gov/gip/TopographicMapSymbols/topomapsymbols.pdf?utm_source=chatgpt.com))

### Ridge logic

Ridges should function as **macro-navigation rails** without becoming roads.

Implementation guidance:

- Main ridges should have persistent direction over hundreds of meters.
- Spurs should branch from ridges at believable angles and descend toward gullies or creek systems.
- Ridge crests should be somewhat more readable than random jungle:
  - slightly thinner understory in places;
  - more wind movement;
  - occasional exposed roots/rock;
  - intermittent skyline windows;
  - drier leaf litter after rain than valley bottoms;
  - more audible canopy movement.
- Avoid making ridges clean paths. They should still be rough, rooty, tangled, and interrupted.

Recommended mask names:

```text
M_Terrain_MainRidgeMask
M_Terrain_SpurMask
M_Terrain_SaddleMask
M_Terrain_SlopeBreakMask
M_Terrain_GullyMask
M_Terrain_GorgeMask
M_Terrain_ValleyFloorMask
M_Terrain_CoastalEscarpmentMask
M_Terrain_SkylineExposureMask
```

### Spur logic

Spurs are excellent orientation tools because they are readable but ambiguous.

Design use:

- A spur gives the player a descent line from ridge to water.
- Multiple spurs should look related but not identical.
- Some spurs should terminate at:
  - creek junctions;
  - rock shelves;
  - dense vine thickets;
  - landslip scars;
  - coastal views;
  - old disturbed clearings.
- Spurs should help the player infer “I left the main ridge and am descending,” but not “I am facing exact direction X.”

Risk:

- If every spur is equally traversable, the player will treat them as roads.
- If no spur is readable, jungle becomes random.

Recommendation:

- Author **primary spurs** as deliberate route candidates.
- Generate or sculpt **secondary spurs** for ecological realism and misdirection.
- Use vegetation and footing to differentiate them subtly.

Suggested data tag:

```text
TerrainForm.PrimarySpur
TerrainForm.SecondarySpur
TerrainForm.FalseSpur
TerrainForm.TerminalSpur
```

### Saddle logic

Saddles should be high-value orientation landmarks. A saddle naturally compresses route choice: two ways rise, two ways fall. It can also create sky exposure between higher shoulders.

Implementation guidance:

- Saddles should often provide:
  - a lateral breeze;
  - a short sky slot;
  - visible opposing slope shape;
  - different vegetation on each side due to drainage/exposure;
  - water sounds from two possible descent directions after rain.
- Use saddles as places where players can re-evaluate without UI.
- Do not put explicit markers in saddles. The saddle itself is the clue.

Recommended mask names:

```text
M_Terrain_SaddleFloorMask
M_Terrain_SaddleShoulderMask
M_Terrain_SaddleSkyWindowMask
M_Terrain_PassWindExposureMask
```

### Mountain silhouettes and skyline windows

In dense jungle, mountain silhouettes should not be constantly visible. They become meaningful because they are scarce.

Use them through:

- ridge-top gaps;
- saddle windows;
- landslide scars;
- creek bends with open sky;
- beach/estuary exits;
- storm-damaged canopy;
- large emergent tree gaps;
- cliff bases looking upward.

Player function:

- compare a remembered silhouette to a new glimpse;
- infer whether they are on the same side of a ridge;
- use a peak notch, double summit, or cliff face as a long-range anchor;
- recover macro-orientation after being enclosed in gullies.

Implementation guidance:

- Create a small set of **distinctive but plausible skyline anchors**:
  - split peak;
  - slanted ridge wall;
  - saddle notch;
  - volcanic-looking knoll if geologically justified;
  - cliff band;
  - coastal headland silhouette.
- Do not overuse iconic “landmark mountains.” The world should feel natural, not theme-parked.

Suggested named assets/volumes:

```text
BP_SkylineAnchorVolume
BP_SkylineWindowVolume
DA_SkylineAnchorProfile
M_Terrain_SkylineWindowMask
```

### Slope breaks and benches

Slope breaks are excellent no-map memory anchors because players feel them in movement.

Examples:

- steep climb easing onto bench;
- bench dropping into gorge;
- root terrace before a gully;
- boulder shelf beside creek;
- ridge shoulder flattening before saddle;
- wet clay transition at lower slope.

Implementation guidance:

- Add traversal and audio change at slope breaks:
  - footstep material change;
  - breath/exertion change;
  - slip risk change;
  - vegetation density change;
  - insect/frog/water ambience change.
- Make slope breaks repeatable enough for memory: “I crossed a flat shoulder before dropping into the wet gully.”

Recommended mask names:

```text
M_Terrain_UpperSlopeMask
M_Terrain_MidSlopeMask
M_Terrain_LowerSlopeMask
M_Terrain_BenchMask
M_Terrain_ToeSlopeMask
M_Terrain_SlopeBreakMask
```

## Canopy Gap And Sky Window Evidence

### Canopy closure model

Canopy closure should be a core orientation variable. It controls:

- direct sun visibility;
- shadow sharpness;
- sky visibility;
- star/moon visibility;
- rain interception;
- wind penetration;
- fog/mist feel;
- understory brightness;
- vegetation density;
- long-distance visibility.

Tropical rainforest canopy is structurally layered, and the dense canopy roof can strongly reduce light and air movement below it. This supports a design where orientation evidence is usually filtered and intermittent, not absent everywhere and not constantly available. ([wettropics.gov.au](https://www.wettropics.gov.au/rainforest-structure?utm_source=chatgpt.com))

Recommended canopy mask names:

```text
M_Canopy_ClosedMask
M_Canopy_FilteredMask
M_Canopy_GapMask
M_Canopy_SkyWindowMask
M_Canopy_EmergentMask
M_Canopy_StormDamageMask
M_Canopy_LianaCurtainMask
M_Canopy_GorgeSlotMask
M_Canopy_RidgeThinMask
```

### Canopy gaps as navigational events

Canopy gaps should not be decorative only. They should be evidence nodes.

Factual basis:

- Canopy gaps increase resource availability at the forest floor, including light, water, and nutrients.
- Tropical forest gap borders can be gradual rather than clean, and increased forest-floor light is one of the ways gaps are detected.
- Cyclone disturbance in northeast Australian rainforest has been observed to increase canopy openness and direct/diffuse/total light availability in the understory. ([besjournals.onlinelibrary.wiley.com](https://besjournals.onlinelibrary.wiley.com/doi/10.1111/1365-2745.14320?utm_source=chatgpt.com))

Design translation:

| Gap type | Likely cause | Orientation value | Player-facing evidence |
|---|---|---|---|
| Single-tree fall | old tree fall, root plate, branch collapse | short sky glimpse, local memory anchor | bright patch, fallen trunk, changed vegetation |
| Multi-tree fall | storm/cyclone disturbance | stronger sky/sun cue, vegetation transition | dense regrowth, tangled debris, larger light pool |
| Ridge-thin canopy | exposure, shallow soil, wind | repeated skyline/sun glimpses | moving crowns, drier ground, broken shadows |
| Gorge slot | narrow opening above steep walls | linear sky strip, poor horizon | vertical light shaft, echoing water |
| Creek bend gap | bank erosion/treefall | water-flow cue plus sky cue | wet sediment, exposed roots, louder water |
| Coastal gap | salt/wind/disturbance | macro recovery toward coast | brighter sky, ocean sound, salt-stunted vegetation |

### Sky windows

A sky window is any visible patch of sky large enough to expose useful information: sun location, cloud motion, moon/stars, silhouette, or weather change.

Recommended sky-window classes:

```text
SkyWindow.None
SkyWindow.PinHole
SkyWindow.FilteredCanopy
SkyWindow.NarrowSlot
SkyWindow.LocalGap
SkyWindow.RidgeWindow
SkyWindow.SaddleWindow
SkyWindow.CoastalWindow
```

Suggested implementation variables:

```text
SkyVisibleFraction
DirectSunVisibleFraction
HorizonVisibleFraction
SilhouetteVisibleFraction
MoonStarVisibleFraction
CanopyOcclusionFraction
```

Player-facing cue behavior:

- **PinHole**: tiny brightness cue; no reliable direction.
- **FilteredCanopy**: moving mottled light; weak sun/shadow inference.
- **NarrowSlot**: possible sun/moon track cue, but little horizon.
- **LocalGap**: useful sun/shadow and cloud cue.
- **RidgeWindow**: likely skyline/silhouette cue.
- **SaddleWindow**: route decision plus sky cue.
- **CoastalWindow**: macro recovery cue with ocean light/sound.

### Canopy gap visual language

Use consistent but non-explicit visual grammar:

- Closed canopy:
  - low contrast;
  - green-filtered light;
  - soft/no shadows;
  - still humid ambience;
  - short view distance.
- Filtered canopy:
  - mottled light;
  - moving leaf shadows;
  - occasional sun shafts;
  - limited directional confidence.
- True gap:
  - brighter exposure;
  - stronger color temperature shift;
  - visible cloud/sun/moon/sky;
  - more rain reaching ground;
  - different seedlings/regrowth;
  - possible fallen tree/root plate.
- Ridge window:
  - wind movement;
  - crown thinning;
  - skyline silhouette.
- Gorge slot:
  - vertical light;
  - echo/reverb;
  - wet rock;
  - limited lateral escape.

## Sun, Shadow, Night, And Weather Cue Model

### Sun and shadow

Sun cues should be useful but imperfect.

Relevant factual basis:

- Solar position is described by azimuth and elevation; NOAA’s solar position tools calculate those values for a given location, date, and time.
- Celestial navigation uses observed sun, moon, stars, and horizon, but precise position-fixing requires instruments and tabulated data; Jungle Game should not imply exact naked-eye precision under dense canopy. ([gml.noaa.gov](https://gml.noaa.gov/grad/solcalc/azel.html?utm_source=chatgpt.com))

Design guidance:

- Use a configured world latitude and season model.
- Do not assume “midday sun always north” unless the island latitude/season guarantees it. In the tropics, the sun’s apparent path can shift north or south of zenith depending on latitude and time of year.
- Player-facing sun evidence should come from:
  - visible sun disk through gap;
  - bright side of clouds;
  - sun shafts;
  - shadow direction on open ground;
  - warmer/cooler edge lighting;
  - repeated morning/evening light angle memory.
- Dense canopy should break shadows into unreliable fragments.
- Overcast should remove sharp shadows but preserve some diffuse brightness gradient.

Recommended enum:

```text
enum class EJG_SolarCueState
{
    None,
    DiffuseBrightnessGradient,
    FilteredCanopyMottle,
    PartialSunDisk,
    DirectSunAndShadow,
    LowAngleRidgeLight,
    StormSuppressed
};
```

### Shadow quality states

Recommended shadow cue degradation:

```text
ShadowCue.None
ShadowCue.DiffuseOnly
ShadowCue.MottledUnreliable
ShadowCue.ShortLocalReliable
ShadowCue.LongDirectionalReliable
ShadowCue.DistortedByTerrain
```

Rules:

- Closed canopy + overcast = `DiffuseOnly` or `None`.
- Closed canopy + direct sun = `MottledUnreliable`.
- Gap + direct sun = `ShortLocalReliable`.
- Ridge/shore + low sun = `LongDirectionalReliable`.
- Gorge/steep slope = `DistortedByTerrain`.

### Night cues

Night should not simply be “same jungle but darker.” It should change the available evidence.

Night orientation sources:

- moonlit canopy gaps;
- stars through sky windows;
- ridge silhouettes against sky glow;
- ocean surf direction;
- creek sound;
- insect/frog acoustic zones;
- wet rock/ground reflectance;
- firefly/bioluminescence-like cues only if ecologically justified and not magical;
- memory of terrain crossed in daylight.

The Moon’s phase changes how much of the visible disk is illuminated by sunlight; moonlight availability should vary by phase, cloud, canopy, and sky-window exposure. ([spaceplace.nasa.gov](https://spaceplace.nasa.gov/moon-phases/?utm_source=chatgpt.com))

Recommended night cue states:

```text
NightCue.BlackCanopy
NightCue.MoonFilteredCanopy
NightCue.MoonGapVisible
NightCue.StarWindowVisible
NightCue.RidgeSilhouetteVisible
NightCue.CoastalSkyGlowVisible
NightCue.StormNightSuppressed
```

Design notes:

- Stars should be visible only through meaningful gaps/windows.
- Moonlight should create orientation recovery moments in gaps, ridges, beaches, and creek openings.
- Avoid making constellations a reliable compass unless the project later implements a historically/geographically appropriate sky model.
- Night movement should increase dependence on sound, slope, and memory.

### Weather cue model

Weather is both a degradation system and a reveal system.

Weather effects:

| Weather condition | Degrades | Reveals |
|---|---|---|
| Clear direct sun | none; may increase contrast | shadows, sun shafts, mountain silhouette |
| Broken cloud | consistency of shadow | moving light patches, cloud direction |
| Overcast | direct sun/shadow | diffuse brightness, humidity, sound distance |
| Light rain | visibility, footing | drainage lines, leaf drip, creek direction |
| Heavy rain | visibility, sound localization, footing | runoff paths, gullies, flood direction |
| Fog/mist | horizon, silhouette, distance | depth layering, ridge/valley humidity pattern |
| Storm | sun, stars, sound clarity | wind exposure, canopy damage, drainage intensity |
| Post-rain clearing | ground stability, glare | fresh tracks, runoff, stronger stream sounds, temporary sky breaks |

Fog is formally associated with reduced visibility; the Met Office defines fog as cloud at ground level that reduces visibility below 1000 m, and dense fog below 100 m. The Bureau of Meteorology similarly defines fog as suspended droplets causing horizontal visibility below 1000 m and notes that fog can obscure ground features and impair visual navigation. ([weather.metoffice.gov.uk](https://weather.metoffice.gov.uk/learn-about/weather/types-of-weather/fog?utm_source=chatgpt.com))

Recommended enum:

```text
enum class EJG_WeatherCueState
{
    Clear,
    BrokenCloud,
    OvercastBright,
    OvercastDark,
    LightRain,
    HeavyRain,
    Mist,
    DenseFog,
    Storm,
    PostRainClearing
};
```

## Cue Degradation Rules

### Core degradation principle

Do not randomly remove cues. Degrade them according to **physical cause**:

- canopy closure blocks sky/sun/stars;
- terrain occlusion blocks horizon/silhouette;
- fog/rain reduces distance and contrast;
- gullies/gorges restrict sky angle and sound direction;
- night removes color/detail but can preserve silhouettes and sound;
- wet ground changes footing, reflectance, and drainage evidence.

The player should be able to infer why cues are missing.

### Cue degradation states

Recommended shared cue state enum:

```text
enum class EJG_CueDegradationState
{
    ClearReadable,
    FilteredReadable,
    Intermittent,
    Ambiguous,
    Occluded,
    Distorted,
    Suppressed
};
```

Definitions:

| State | Meaning | Example |
|---|---|---|
| `ClearReadable` | strong cue visible/audible enough to use | sun shadow in large gap |
| `FilteredReadable` | cue weakened but still interpretable | mottled sun through thin canopy |
| `Intermittent` | appears in bursts | skyline glimpses while moving along ridge |
| `Ambiguous` | multiple interpretations plausible | spur mistaken for main ridge |
| `Occluded` | physically blocked | mountain behind ridge/canopy |
| `Distorted` | cue present but warped by terrain/weather | echoing creek in gorge |
| `Suppressed` | weather/night/canopy removes cue temporarily | heavy rain drowns surf direction |

### Degradation matrix

| Cue category | Closed canopy | Gully/gorge | Rain | Fog/mist | Night |
|---|---:|---:|---:|---:|---:|
| Ridge shape | medium | high degradation if enclosed | low-medium | medium | high unless silhouette |
| Spur/valley slope | low | low | medium footing effect | low-medium | medium |
| Creek flow | low | low | stronger but noisier | low | medium |
| Skyline silhouette | high | high | medium | high | medium if backlit |
| Sun/shadow | high | medium-high | medium-high | high | high |
| Stars/moon | high | high | high if cloud | high | available only through windows |
| Ocean sound | low-medium | distorted | suppressed by rain | low | stronger relative to visuals |
| Vegetation transition | low | low | low | low | medium |
| Wind exposure | low under canopy | medium | storm-enhanced | low | low-medium |

### Avoid unreadable failure states

A dense jungle can become frustrating if every cue is suppressed at once. Use a **cue floor**:

- If sky and sun are suppressed, preserve slope, water, vegetation, sound, or terrain texture.
- If sound is suppressed by rain, strengthen runoff and wet-ground evidence.
- If fog removes horizon, preserve nearby landform shape and vegetation transition.
- If night removes visual detail, preserve creek sound, slope feel, and large silhouette contrast in gaps.
- If a gorge removes horizon and sky, make its linear drainage logic and exit transitions readable.

Suggested rule:

```text
At every validation point, at least two diegetic orientation cue categories should be readable, filtered-readable, or intentionally recoverable within a short traversal distance.
```

This is a developer validation rule only, not a player-facing system.

## Traversal Pressure And Recovery Design

### Slope feel

Slope should be felt through movement, not read from UI.

Implementation cues:

- uphill:
  - slower acceleration;
  - heavier breathing;
  - footstep strain;
  - more stamina drain;
  - camera bob subtly changes;
  - climbing over roots/rocks;
- downhill:
  - braking steps;
  - increased slip risk;
  - loose leaf/soil slide;
  - more knee/ankle stress audio/animation;
  - faster but riskier descent;
- side-slope:
  - uneven gait;
  - higher fatigue over time;
  - foot placement noise;
  - increased stumble chance on wet clay/roots.

Walking/running energy cost changes with slope; extreme uphill and downhill grades alter locomotion cost, and uneven terrain further complicates effort. Use this as a game-feel foundation rather than a simulator target. ([pubmed.ncbi.nlm.nih.gov](https://pubmed.ncbi.nlm.nih.gov/12183501/?utm_source=chatgpt.com))

Recommended movement tags:

```text
TraversalSlope.Flat
TraversalSlope.GentleUp
TraversalSlope.SteepUp
TraversalSlope.GentleDown
TraversalSlope.SteepDown
TraversalSlope.SideSlope
TraversalSlope.RootStep
TraversalSlope.WetClay
TraversalSlope.LooseLeaf
TraversalSlope.RockShelf
```

### Fatigue pressure

Fatigue should influence route decisions without becoming a navigation UI.

Design use:

- Players may choose ridge travel for clarity but pay climb cost.
- Players may choose creek travel for water/linear guidance but pay speed, wet footing, and gorge risk.
- Players may contour across slope to preserve elevation but pay side-slope fatigue.
- Players may climb for sky/silhouette recovery but expose themselves to wind/rain and stamina loss.

Do not display “best route.” Let physical pressure teach route tradeoffs.

### Footing risk

Footing should reinforce terrain identity.

Examples:

| Terrain | Footing risk | Orientation role |
|---|---|---|
| Ridge crest | roots, rocks, windfall | keeps route slow but readable |
| Spur | leaf litter, steep descent | descent line with risk |
| Saddle | mud, leaf accumulation, pass-through animal routes | decision landmark |
| Gully | wet clay, hidden rocks, debris | water-finding but enclosed |
| Gorge | slick rock, narrow ledges | strong route but dangerous |
| Creek bed | slippery stones, flood debris | linear route, flow cue |
| Lower slope | saturated soil, dense vegetation | transition to drainage/valley |
| Coastal fringe | sand, salt-stunted vegetation, surf noise | macro edge cue |

### Wind exposure

Wind should be a subtle orientation layer.

Topographic position affects biophysical conditions such as hydrology, erosion/deposition, wind exposure, and habitat suitability; ridges, valleys, upper slopes, lower slopes, and flats should therefore support different environmental signals. ([jennessent.com](https://www.jennessent.com/downloads/TPI-poster-TNC_18x22.pdf?utm_source=chatgpt.com))

Implementation guidance:

- Ridge crests:
  - stronger canopy movement;
  - audible leaf rush;
  - cooler/drier feeling;
  - more broken branches.
- Saddles:
  - cross-breeze;
  - funnelled wind;
  - moving mist.
- Gullies:
  - still air;
  - damp sound;
  - echo/reverb.
- Coast-facing slopes:
  - salt wind;
  - distant surf;
  - windward vegetation lean if ecologically appropriate.

Recommended mask:

```text
M_Exposure_WindRidgeMask
M_Exposure_SaddleFunnelMask
M_Exposure_StillGullyMask
M_Exposure_CoastalWindMask
```

### Sound

Sound should support imperfect orientation, especially when visuals degrade.

Sound categories:

```text
SoundCue.CreekFlow
SoundCue.WaterfallLowFreq
SoundCue.OceanSurf
SoundCue.CanopyWind
SoundCue.RainOnCanopy
SoundCue.RainOnOpenGround
SoundCue.InsectZone
SoundCue.FrogWetZone
SoundCue.BirdCanopyEdge
SoundCue.GorgeEcho
```

Rules:

- Creek sound should grow when descending toward drainage but may echo in gorges.
- Surf sound should be low-frequency, intermittent, and weather-dependent.
- Heavy rain should suppress distant sound but strengthen local drainage.
- Dense vegetation should muffle directionality.
- Saddles and ridges can carry distant sound briefly.
- Avoid making sound a perfect beacon.

### Vegetation transition

Vegetation should support orientation through ecological patterning.

Use transitions between:

- ridge-thinner canopy;
- lower-slope dense palms/vines;
- creek-margin wet vegetation;
- gorge ferns/moss;
- disturbance regrowth in canopy gaps;
- coastal littoral/salt-wind influence;
- storm-damaged patches;
- saddle pass vegetation.

Topography affects abiotic conditions that influence tropical forest structure and dynamics; use topographic position as a vegetation driver rather than uniform foliage scatter. ([besjournals.onlinelibrary.wiley.com](https://besjournals.onlinelibrary.wiley.com/doi/10.1111/1365-2745.13261?utm_source=chatgpt.com))

Recommended vegetation masks:

```text
M_Veg_RidgeDryThinMask
M_Veg_UpperSlopeMixedMask
M_Veg_LowerSlopeWetDenseMask
M_Veg_GullyFernMask
M_Veg_CreekMarginMask
M_Veg_CanopyGapRegrowthMask
M_Veg_CoastalSaltWindMask
M_Veg_StormDamageRegrowthMask
```

### Recovery design without feedback

Recovery should be possible through world structure:

- Creeks converge downhill; following flow eventually gives macro direction toward larger drainage or coast. USGS explains that runoff and small creeks flow downhill, merge into larger streams/rivers, and generally continue toward lakes or seas unless otherwise blocked. ([usgs.gov](https://www.usgs.gov/water-science-school/science/rivers-streams-and-creeks?utm_source=chatgpt.com))
- Main ridges divide drainage and provide intermittent skyline/sky windows.
- Saddles are repeatable decision landmarks.
- Coast/ocean sound provides a broad macro cue near island edges.
- Storm gaps and treefalls create temporary/rare sky evidence.
- Re-entering a known terrain form should feel recognizable by slope, vegetation, sound, and light.

Forbidden recovery methods:

- “You seem lost” character lines.
- UI compass pulse.
- Objective marker flicker.
- Map auto-sketching.
- Hidden orientation score affecting dialogue.
- Camera steering toward correct route.
- Artificially highlighted “right” path.

Allowed recovery methods:

- brighter sky through a real gap;
- increasing creek volume downhill;
- surf sound carried by wind;
- visible mountain notch from saddle;
- ridge crest wind and silhouette;
- distinctive treefall remembered by player;
- vegetation transition indicating creek/coast/ridge;
- animal movement patterns only if natural and non-directive.

## UE5 Implementation Guidance

### System architecture

Implement orientation as a world evidence layer composed of authored and procedural data. No player-facing widgets should consume this data.

Suggested high-level components:

```text
BP_JGOrientationCueVolume
BP_JGCanopyGapVolume
BP_JGSkyWindowVolume
BP_JGTerrainFormVolume
BP_JGWeatherCueController
BP_JGTraversalSurfaceCueVolume
DA_JGOrientationCueProfile
DA_JGSkylineAnchorProfile
DA_JGCanopyCueProfile
```

Suggested subsystem names:

```text
UJGTerrainEvidenceSubsystem
UJGCanopyEvidenceSubsystem
UJGWeatherCueSubsystem
UJGTraversalCueSubsystem
UJGDeveloperEvidenceViewSubsystem
```

### Orientation cue categories

Recommended enum:

```text
enum class EJG_OrientationCueCategory
{
    TerrainBackbone,
    DrainageFlow,
    RidgeSpurSaddle,
    SlopeFeel,
    SkylineSilhouette,
    CanopyGap,
    SkyWindow,
    SolarShadow,
    NightCelestial,
    WeatherAtmosphere,
    AcousticDirection,
    VegetationTransition,
    TraversalRisk,
    CoastalMacroCue
};
```

### Terrain form enum

```text
enum class EJG_TerrainForm
{
    Unknown,
    MainRidge,
    Spur,
    Saddle,
    UpperSlope,
    MidSlope,
    LowerSlope,
    SlopeBreak,
    Bench,
    Gully,
    Gorge,
    ValleyFloor,
    CreekCorridor,
    CreekJunction,
    WaterfallApproach,
    CoastalEscarpment,
    BeachExit,
    Headland
};
```

### Canopy state enum

```text
enum class EJG_CanopyCueState
{
    Closed,
    DenseFiltered,
    BrokenFiltered,
    LocalGap,
    StormGap,
    RidgeThin,
    SaddleWindow,
    GorgeSlot,
    CoastalOpen
};
```

### Data asset sketch

Pseudocode only:

```text
DA_JGOrientationCueProfile
- CueCategory
- TerrainForm
- CanopyCueState
- WeatherCueState
- DayNightAvailability
- MinReadableDistance
- MaxReadableDistance
- OcclusionSensitivity
- WeatherSensitivity
- DebugColor
- EvidenceTags
- ScreenshotValidationTags
```

### Landscape and material guidance

Use landscape materials and runtime/editor masks to support cue readability:

- slope-driven material variation:
  - wet clay lower slopes;
  - exposed roots on ridges;
  - slick rock in gorges;
  - sediment near creek junctions;
- vegetation layer masks:
  - ridge dry/thin;
  - lower-slope wet/dense;
  - gap regrowth;
  - creek margin;
- canopy masks:
  - direct sun blockers;
  - sky visibility fraction;
  - rain interception fraction;
  - wind exposure fraction.

Unreal landscape materials support named landscape layers and painted blend weights, making them suitable for explicit terrain/vegetation masks such as ridge, gully, wet-slope, and creek-margin layers. ([dev.epicgames.com](https://dev.epicgames.com/documentation/unreal-engine/landscape-materials-in-unreal-engine?utm_source=chatgpt.com))

### Lighting, sky, fog, and atmosphere

UE5 systems relevant to this spec:

- Directional Light for sun/moon direction.
- Sky Atmosphere for sky scattering.
- Sky Light for ambient contribution.
- Exponential Height Fog and Volumetric Fog for mist, gorge haze, rain/fog degradation.
- Volumetric Clouds for cloud occlusion and broken-light states.
- Lumen for dynamic global illumination where project performance allows.

Epic documentation covers environmental lighting with fog, clouds, sky, and atmosphere; Sky Atmosphere; Exponential Height Fog integration; and Lumen dynamic global illumination. ([dev.epicgames.com](https://dev.epicgames.com/documentation/unreal-engine/environmental-light-with-fog-clouds-sky-and-atmosphere-in-unreal-engine?utm_source=chatgpt.com))

Implementation recommendation:

```text
Use a single authoritative time/weather controller for:
- sun angle;
- moon angle if implemented;
- cloud coverage;
- fog density;
- rain intensity;
- skylight intensity;
- canopy wetness;
- shadow cue state.
```

Do not expose this as a HUD clock or compass.

### Weather degradation implementation

Recommended scalar fields:

```text
WeatherVisibilityMeters
FogDensityNormalized
RainIntensityNormalized
CloudCoverNormalized
DirectSunTransmission
DiffuseSkyTransmission
GroundWetnessNormalized
DistantSoundSuppression
CanopyDripIntensity
RunoffIntensity
```

Recommended rule:

```text
CueReadability = BaseCueStrength
               * TerrainExposureFactor
               * CanopyTransmissionFactor
               * WeatherVisibilityFactor
               * TimeOfDayFactor
               * LocalOcclusionFactor
```

This formula is developer-only. It should not become an orientation meter. It is for validation/debugging of whether cues are present and degraded correctly.

### What should be implemented now

For PR5 Batch 001 scale, implement foundations only:

1. Terrain-form naming convention.
2. Canopy/sky-window mask naming convention.
3. Cue category enum/data asset design.
4. Weather degradation state enum.
5. Developer-only debug view specification.
6. Validation screenshot plan.
7. Initial authored validation points:
   - ridge crest;
   - spur descent;
   - saddle;
   - gully;
   - gorge;
   - canopy gap;
   - creek junction;
   - coastal recovery view.

### What should be deferred

Defer:

- complete astronomy simulation;
- historical star navigation modeling;
- full ecological succession simulation;
- fully procedural hydrology;
- complex AI route inference;
- dynamic treefall simulation;
- long-term storm damage simulation;
- any player-facing orientation confidence system;
- any auto-map/sketch/map substitute.

## Developer-Only Debug And Evidence Views

### Debug view principles

Debug views are allowed only for developers. They must prove the world contains sufficient orientation evidence and that weather/canopy degrade it physically.

Never ship these as player-facing features.

### Required debug view names

Recommended view modes:

```text
DBG_OrientationCue_All
DBG_OrientationCue_TerrainBackbone
DBG_OrientationCue_RidgeSpurSaddle
DBG_OrientationCue_DrainageFlow
DBG_OrientationCue_CanopyClosure
DBG_OrientationCue_SkyWindow
DBG_OrientationCue_SolarShadow
DBG_OrientationCue_NightSky
DBG_OrientationCue_WeatherDegradation
DBG_OrientationCue_AcousticReach
DBG_OrientationCue_VegetationTransition
DBG_OrientationCue_TraversalPressure
DBG_OrientationCue_CueFloor
DBG_OrientationCue_ForbiddenUIAudit
```

### Debug overlays

Developer-only overlays should show:

| Debug overlay | Shows | Purpose |
|---|---|---|
| `DBG_TerrainFormMask` | ridge/spur/saddle/gully/gorge/creek/coast masks | prove terrain evidence exists |
| `DBG_CanopyTransmission` | sky/sun/rain transmission | prove canopy is not uniform |
| `DBG_SkyWindowRays` | sky visibility cones/windows | validate sun/moon/star opportunities |
| `DBG_SolarCueState` | current sun/shadow cue state | validate time/weather/canopy interaction |
| `DBG_WeatherVisibility` | fog/rain visibility bands | validate degradation distance |
| `DBG_AcousticCueReach` | creek/surf/waterfall audibility | validate non-visual recovery |
| `DBG_TraversalCost` | fatigue/footing/slope risk | validate route pressure |
| `DBG_CueRedundancy` | count of readable cue categories | ensure no unreadable dead zones |
| `DBG_ScreenshotValidationPoints` | capture locations/angles | support PR evidence |
| `DBG_NoPlayerFacingOrientation` | audit of forbidden UI systems | prevent design drift |

### Developer evidence fields

For each validation point, log:

```text
ValidationPointId
TerrainForm
CanopyCueState
SkyWindowClass
WeatherCueState
TimeOfDayBucket
ReadableCueCategories
SuppressedCueCategories
DominantRecoveryCue
ForbiddenUIUsed=false
ScreenshotRequired=true/false
```

### Cue floor debug

The cue floor view should not compute “player orientation.” It should only count available environmental evidence categories.

Example:

```text
ReadableCueCategories:
- TerrainBackbone
- DrainageFlow
- AcousticDirection

SuppressedCueCategories:
- SolarShadow
- SkylineSilhouette
- NightCelestial

CueFloorPass:
- true if >= 2 categories readable or recoverable nearby
```

This proves the world is not unreadable while avoiding hidden orientation scoring.

## Validation Points And Screenshot Plan

### Validation point types

Recommended validation point types:

```text
VP_RidgeCrest_Longitudinal
VP_RidgeCrest_CrossSlope
VP_SpurBranch_Decision
VP_Saddle_PassWindow
VP_SlopeBreak_Bench
VP_Gully_Enclosed
VP_Gorge_SkySlot
VP_CreekFlow_Downstream
VP_CreekJunction_Confluence
VP_CanopyGap_LocalSun
VP_CanopyGap_StormDamage
VP_SkylineWindow_MountainSilhouette
VP_CoastalSound_Approach
VP_CoastalExit_MacroRecovery
VP_NightMoonGap
VP_NightStarWindow
VP_FogRidge_Degraded
VP_HeavyRain_Runoff
VP_PostRain_ClearingReveal
```

### Screenshot/evidence view names

Recommended capture names:

```text
EV_RidgeCrest_PlayerView_Clear
EV_RidgeCrest_DebugTerrainMask
EV_RidgeCrest_DebugSkyWindow
EV_SpurBranch_PlayerView
EV_SpurBranch_DebugTerrainForm
EV_Saddle_PlayerView_SkylineWindow
EV_Saddle_DebugWindExposure
EV_Gully_PlayerView_Occluded
EV_Gully_DebugDrainageFlow
EV_Gorge_PlayerView_SkySlot
EV_Gorge_DebugWeatherOcclusion
EV_CanopyGap_PlayerView_SunPatch
EV_CanopyGap_DebugCanopyTransmission
EV_CreekJunction_PlayerView
EV_CreekJunction_DebugAcousticReach
EV_Fog_PlayerView_Degraded
EV_Fog_DebugVisibilityBands
EV_Night_PlayerView_MoonGap
EV_Night_DebugNightSkyWindow
EV_CoastalExit_PlayerView_Recovery
EV_CoastalExit_DebugCueRedundancy
```

### Required screenshot angles

For each key validation point, capture:

1. **Player-eye forward view**
   - Shows what the player actually sees.
   - No debug overlay.
2. **Player-eye 90-degree side view**
   - Shows slope/canopy/occlusion relationship.
3. **Slightly elevated developer view**
   - Shows landform shape without turning into player map evidence.
4. **Debug terrain-form view**
   - Shows ridge/spur/saddle/gully masks.
5. **Debug canopy/sky-window view**
   - Shows canopy transmission and sky access.
6. **Weather-degraded comparison**
   - Same angle in clear, overcast, rain/fog, and night where applicable.
7. **Cue redundancy view**
   - Shows developer-only readable cue categories.

### Validation scenarios

#### Scenario A: Ridge recovery

- Start in enclosed lower slope.
- Climb toward ridge.
- Player should notice:
  - slope increase;
  - drier/rockier ground;
  - wind/canopy movement;
  - intermittent sky windows;
  - possible mountain silhouette.
- Debug must show:
  - `M_Terrain_MainRidgeMask`;
  - `M_Canopy_RidgeThinMask`;
  - `DBG_OrientationCue_CueFloor` passing.

#### Scenario B: Spur misread

- Player follows a spur thinking it is the main ridge.
- Evidence should eventually reveal descent:
  - slope trend downward;
  - creek sound increases;
  - canopy closes;
  - valley humidity/vegetation increases.
- Debug must show:
  - `TerrainForm.Spur`;
  - `DrainageFlow` increasing downhill;
  - no UI correction.

#### Scenario C: Saddle decision

- Player reaches saddle between two high points.
- Evidence:
  - two uphill shoulders;
  - two downhill exits;
  - wind funnel;
  - sky window;
  - maybe distant silhouette.
- Debug must show:
  - `M_Terrain_SaddleMask`;
  - `M_Terrain_SaddleSkyWindowMask`;
  - at least three cue categories readable.

#### Scenario D: Heavy rain degradation

- Same route during heavy rain.
- Evidence degraded:
  - sun/shadow suppressed;
  - distant sound suppressed;
  - footing risk increased;
  - runoff/drainage strengthened.
- Debug must show:
  - `WeatherCueState.HeavyRain`;
  - `SolarShadow.Suppressed`;
  - `DrainageFlow.FilteredReadable` or stronger;
  - cue floor pass.

#### Scenario E: Fogged ridge

- Ridge with normal skyline visibility becomes fogged.
- Evidence degraded:
  - mountain silhouette hidden;
  - horizon distance reduced;
  - near ridge shape and wind remain.
- Debug must show:
  - `SkylineSilhouette.Occluded`;
  - `TerrainBackbone.FilteredReadable`;
  - `WeatherVisibilityMeters` reduced.

#### Scenario F: Night canopy gap

- Player reaches canopy gap at night.
- Evidence:
  - moon/star glimpse if weather allows;
  - silhouette contrast;
  - acoustic cues stronger relative to visual.
- Debug must show:
  - `NightCue.MoonGapVisible` or `NightCue.StarWindowVisible`;
  - cloud/canopy state;
  - no explicit time readout.

## Risks, Pitfalls, And Forbidden Drift

### Major risks

1. **Uniform jungle maze**
   - Risk: every location looks equally dense.
   - Fix: terrain-form masks, canopy variation, vegetation transitions, sound zones.

2. **Over-readable theme-park navigation**
   - Risk: every ridge is a path, every gap frames the goal.
   - Fix: ambiguous spurs, partial silhouettes, weather suppression, natural clutter.

3. **Hidden compass by another name**
   - Risk: debug cue values become a player-facing confidence system.
   - Fix: strict separation between developer-only evidence and player-facing world.

4. **Temperate forest assumptions**
   - Risk: too much open sightline, too many clear shadows, dry leaf visibility everywhere.
   - Fix: closed canopy, humid understory, dense vegetation, filtered sun, tropical disturbance gaps.

5. **Weather as pure frustration**
   - Risk: rain/fog only remove information.
   - Fix: weather also reveals runoff, wetness, sound changes, canopy drip, and post-rain clearing.

6. **Perfect natural navigation**
   - Risk: sun/stars/vegetation always give reliable direction.
   - Fix: make cues partial, seasonal, occluded, and cross-check dependent.

7. **Modern survival-tool drift**
   - Risk: adding watch, compass, GPS, map sketch, coordinates.
   - Fix: forbid all modern explicit navigation tools.

8. **Character explanation drift**
   - Risk: protagonist says “the creek flows east” or “I’m turned around.”
   - Fix: use breath, exertion, hesitation, and environmental reaction only; no orientation verdicts.

### Forbidden drift list

Do not add:

- player map;
- minimap;
- compass;
- bearing readout;
- GPS;
- objective marker;
- HUD clock;
- explicit time readout;
- orientation confidence meter;
- player-facing “lost” state;
- auto-drawn terrain memory map;
- magic trail glow;
- animal guide path;
- chase/monster framing;
- combat navigation pressure;
- modern survival instruments;
- dialogue that confirms correct/incorrect orientation.

### Acceptable ambiguity

The player may be unsure. That is intended.

The design fails only if:

- uncertainty becomes random;
- cues are absent rather than degraded;
- recovery depends on UI;
- weather removes every cue category;
- terrain forms are not physically consistent.

## PR-Ready Recommendations

### Implement now

1. Add naming conventions for terrain/canopy/weather orientation evidence.
2. Define enums:
   - `EJG_OrientationCueCategory`;
   - `EJG_TerrainForm`;
   - `EJG_CanopyCueState`;
   - `EJG_CueDegradationState`;
   - `EJG_WeatherCueState`;
   - `EJG_SolarCueState`.
3. Add data asset spec:
   - `DA_JGOrientationCueProfile`;
   - `DA_JGSkylineAnchorProfile`;
   - `DA_JGCanopyCueProfile`.
4. Add developer-only validation-point spec:
   - `BP_JGOrientationValidationPoint`.
5. Add debug view names:
   - `DBG_OrientationCue_All`;
   - `DBG_OrientationCue_CanopyClosure`;
   - `DBG_OrientationCue_SkyWindow`;
   - `DBG_OrientationCue_WeatherDegradation`;
   - `DBG_OrientationCue_CueFloor`.
6. Add first validation point list:
   - ridge crest;
   - spur branch;
   - saddle;
   - gully;
   - gorge;
   - canopy gap;
   - creek junction;
   - coastal exit;
   - fogged ridge;
   - night sky window.

### Defer

1. Full astronomy simulation.
2. Detailed historical sky authenticity.
3. Dynamic long-term canopy succession.
4. Procedural storm damage.
5. Advanced hydrological simulation.
6. Player-facing navigation aids.
7. Any hidden orientation meter.

### Recommended file/data names

```text
DA_JGOrientationCueProfile
DA_JGCanopyCueProfile
DA_JGSkylineAnchorProfile
DA_JGWeatherDegradationProfile
DA_JGTraversalPressureProfile

BP_JGOrientationCueVolume
BP_JGTerrainFormVolume
BP_JGCanopyGapVolume
BP_JGSkyWindowVolume
BP_JGOrientationValidationPoint

M_Terrain_MainRidgeMask
M_Terrain_SpurMask
M_Terrain_SaddleMask
M_Terrain_GullyMask
M_Terrain_GorgeMask
M_Terrain_SlopeBreakMask
M_Canopy_ClosedMask
M_Canopy_GapMask
M_Canopy_SkyWindowMask
M_Canopy_RidgeThinMask
M_Veg_CreekMarginMask
M_Veg_CanopyGapRegrowthMask
M_Exposure_WindRidgeMask
```

### PR acceptance criteria

A PR based on this document should be considered aligned if it:

- adds no player-facing navigation UI;
- defines terrain/canopy/weather cue categories clearly;
- separates developer debug evidence from player experience;
- includes cue degradation states;
- includes validation point and screenshot naming;
- treats tropical canopy as dense, layered, and cue-filtering;
- makes recovery possible through environmental redundancy;
- avoids modern tools, magic, monster/chase framing, and explicit orientation feedback.

## Sources

- U.S. Geological Survey — “Topographic Map Symbols”
  URL: https://pubs.usgs.gov/gip/TopographicMapSymbols/topomapsymbols.pdf

- Geoscience Australia — “Map Reading Guide”
  URL: https://www.ga.gov.au/bigobj/GA7194.pdf

- Natural Resources Canada — “Topographic Maps: The basics”
  URL: https://natural-resources.canada.ca/sites/nrcan/files/earthsciences/pdf/topo101/pdf/mapping_basics_e.pdf

- ACT Orienteering — “Lesson 5: Introduction to Contours”
  URL: https://act.orienteering.asn.au/resources/skills/advanced-skills/lesson-5-introduction-to-contours/

- U.S. Geological Survey — “Rivers, Streams, and Creeks”
  URL: https://www.usgs.gov/water-science-school/science/rivers-streams-and-creeks

- U.S. Geological Survey — “Streamflow and the Water Cycle”
  URL: https://www.usgs.gov/water-science-school/science/streamflow-and-water-cycle

- Wet Tropics Management Authority — “Rainforest structure”
  URL: https://www.wettropics.gov.au/rainforest-structure

- National Geographic Education — “Rainforest”
  URL: https://education.nationalgeographic.org/resource/rain-forest/

- Smithsonian Tropical Research Institute — “Understory”
  URL: https://striresearch.si.edu/rainforest/home/understory/

- Australian Government Department of Climate Change, Energy, the Environment and Water — “Wet Tropics of Queensland”
  URL: https://www.dcceew.gov.au/parks-heritage/heritage/places/world/wet-tropics

- UNESCO World Heritage Centre — “Wet Tropics of Queensland”
  URL: https://whc.unesco.org/en/list/486/

- Krüger et al. — “Gap expansion is the dominant driver of canopy openings in a tropical forest”
  URL: https://besjournals.onlinelibrary.wiley.com/doi/10.1111/1365-2745.14320

- van der Meer et al. — “Canopy dynamics of a tropical rain forest in French Guiana”
  URL: https://edepot.wur.nl/205963

- Turton — “Understorey light environments in a north-east Australian rain forest before and after a tropical cyclone”
  URL: https://www.cambridge.org/core/journals/journal-of-tropical-ecology/article/understorey-light-environments-in-a-northeast-australian-rain-forest-before-and-after-a-tropical-cyclone/730ED01021D4DBCB28904DBC4782D6F7

- NOAA Global Monitoring Laboratory — “Solar Position Calculator”
  URL: https://gml.noaa.gov/grad/solcalc/azel.html

- U.S. Naval Observatory — “Celestial Navigation Data for Assumed Position and Time”
  URL: https://aa.usno.navy.mil/data/celnav

- UNSW Sydney — “Ask an Expert: What is Celestial Navigation?”
  URL: https://www.unsw.edu.au/news/2022/10/ask-an-expert--what-is-celestial-navigation--

- NASA Space Place — “What Are the Moon’s Phases?”
  URL: https://spaceplace.nasa.gov/moon-phases/

- U.S. Naval Observatory — “Phases of the Moon and Percent of the Moon Illuminated”
  URL: https://aa.usno.navy.mil/faq/moon_phases

- Met Office — “What is fog?”
  URL: https://weather.metoffice.gov.uk/learn-about/weather/types-of-weather/fog

- Australian Bureau of Meteorology — “Fog: Hazardous Weather Phenomena”
  URL: https://www.bom.gov.au/aviation/data/education/hwp-fog.pdf

- Minetti et al. — “Energy cost of walking and running at extreme uphill and downhill slopes”
  URL: https://pubmed.ncbi.nlm.nih.gov/12183501/

- Jessup et al. — “Uphill and Downhill With Varying Grades”
  URL: https://espace.library.uq.edu.au/view/UQ%3A471b3ac/japplphysiol_00349_2023.pdf

- Muscarella et al. — “Effects of topography on tropical forest structure depend on climate context”
  URL: https://research.fs.usda.gov/download/treesearch/61542.pdf

- Weiss — “Topographic Position and Landforms Analysis”
  URL: https://www.jennessent.com/downloads/TPI-poster-TNC_18x22.pdf

- Epic Games — “Environmental Light with Fog, Clouds, Sky and Atmosphere in Unreal Engine”
  URL: https://dev.epicgames.com/documentation/unreal-engine/environmental-light-with-fog-clouds-sky-and-atmosphere-in-unreal-engine

- Epic Games — “Sky Atmosphere Component in Unreal Engine”
  URL: https://dev.epicgames.com/documentation/unreal-engine/sky-atmosphere-component-in-unreal-engine

- Epic Games — “Lumen Global Illumination and Reflections in Unreal Engine”
  URL: https://dev.epicgames.com/documentation/unreal-engine/lumen-global-illumination-and-reflections-in-unreal-engine

- Epic Games — “Landscape Materials in Unreal Engine”
  URL: https://dev.epicgames.com/documentation/unreal-engine/landscape-materials-in-unreal-engine
