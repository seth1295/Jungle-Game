# Tropical Biome Masks And Traversal Research

## Executive Summary

This document specifies a developer-only mask system for **Jungle Game** that converts tropical island terrain, hydrology, canopy, vegetation structure, and acoustic conditions into implementation-ready traversal and readability data.

The core recommendation is to treat dense jungle as a **structured field of readable friction**, not as random foliage collision. Traversal resistance should emerge from layered masks:

- terrain shape;
- wetness and drainage;
- vegetation structure;
- canopy closure and sky access;
- disturbance gaps;
- footing risk;
- sound masking;
- visibility loss;
- ecological tells;
- night-specific risk pressure.

The player should never receive map, compass, bearing, objective marker, hidden orientation-confidence, or clock-like UI from these systems. Masks are strictly **world-generation, AI, audio, weather, validation, and developer-debug inputs**. The player reads the same logic through landform, light, sound, water, plant density, animal absence/presence, mud, roots, slope, canopy gaps, and coast/ridge/valley transitions.

Rainforest traversal should be readable because the world is internally consistent. A shaded valley should sound wetter, smell visually muddier, hold denser low vegetation, reduce sightlines, increase insect and amphibian cues, mask distant movement, and make footing worse. A ridge should usually be drier, more wind-exposed, more aurally open, more navigable along its spine, and better for sky-window orientation. A canopy gap should invite movement with light and visibility, but may also carry vine tangles, fallen timber, regrowth, slick debris, and misleading route temptation.

Foundational ecology constraints: tropical rainforest is dense, moisture-dependent vegetation with shade-tolerant understory beneath near-closed canopy; Australian rainforest/vine-thicket classifications commonly use closed continuous canopy, with foliage cover typically exceeding 70%; terrain variables such as elevation, slope, and aspect are standard predictors of hydrology, geomorphology, and ecological heterogeneity; and the topographic wetness index is commonly defined from upslope contributing area and slope. ([qld.gov.au](https://www.qld.gov.au/environment/plants-animals/habitats/rainforests?utm_source=chatgpt.com))

## Design Goals For Jungle Game

### Primary Design Goals

1. **World-first navigation**
   - The player navigates by terrain, water, light, canopy, sound, coast exposure, slope, and ecological transition.
   - No player-facing map, minimap, compass, GPS, bearing display, objective marker, HUD clock, explicit time readout, orientation-confidence UI, or character orientation callouts.

2. **Readable traversal friction**
   - Vegetation should communicate movement cost before contact.
   - A player should see, hear, and infer why a route is slow, noisy, risky, misleading, or promising.

3. **Ecological normality**
   - Dense wet tropical jungle should feel physically and ecologically plausible.
   - The unsettling hominin presence should exploit environmental logic rather than replace it with fantasy rules.

4. **Mask-driven systems**
   - Use masks to drive PCG placement, traversal cost, audio propagation, visibility, weather pressure, animal cues, environmental evidence, and validation.
   - Masks should be inspectable by developers but invisible as navigation aids to players.

5. **Soft ambiguity**
   - A route can be tempting but costly.
   - A light gap can be real relief, a deadfall trap, or a liana tangle.
   - A valley can provide water and acoustic cover while worsening orientation and footing.

### Non-Goals

Do not build:

- player-facing maps, minimaps, compasses, GPS, bearing numbers, objective markers, HUD clocks, explicit time readouts, orientation-confidence displays, or hidden orientation meters;
- modern survival-tool dependencies;
- combat, chase, monster, gore, or fantasy-magic traversal logic;
- generic pine-forest assumptions;
- final species-accurate asset-pack lists;
- full ecological simulation beyond what is needed for traversal, readability, PCG, and validation.

## Recommended Mask Families

All masks should use normalized `float` values in `[0.0, 1.0]` unless otherwise stated. Binary masks should still be authored as floats where possible so later systems can blend rather than branch. Recommended prefix: `M_` for masks, `TR_` for traversal categories, and `DBG_` for debug views.

### Terrain-Derived Masks

| Mask Name | Contract | Meaning | Primary Uses |
|---|---:|---|---|
| `M_ElevationBand` | `0..1` | Relative elevation from coast/sea-level to island high points | biome bands, temperature/moisture hints, cloud/fog pressure |
| `M_SlopeBand` | `0..1` | Normalized movement slope severity | traversal cost, blocker detection, erosion/wetness |
| `M_AspectExposure` | `0..1` | Exposure from aspect, prevailing wind, sun, salt spray | coast-facing stress, drier slopes, canopy deformation |
| `M_TopographicWetness` | `0..1` | Terrain tendency to collect water | mud, streams, leeches/insects, wet footing, sound dampening |
| `M_RidgeInfluence` | `0..1` | Ridge/spur proximity and strength | dry travel corridors, skyline/sky-window cues, wind audio |
| `M_ValleyInfluence` | `0..1` | valley/gully/channel influence | water, mud, dense understory, sound masking, low visibility |
| `M_CoastInfluence` | `0..1` | salt, wind, tidal, surf, and littoral transition influence | coast navigation cues, windshear canopy, mangrove/littoral edge |
| `M_StreamInfluence` | `0..1` | proximity to streams, seeps, waterfall drainage | water access, wet rock, acoustic masking, bank obstruction |
| `M_LandslipDebris` | `0..1` | unstable slope/debris/fallen-material likelihood | rough footing, blocked gullies, false shortcuts |
| `M_MicroReliefRoughness` | `0..1` | small-scale surface unevenness | ankle risk, root mats, boulders, buttress roots |

### Ecology-Derived Masks

| Mask Name | Contract | Meaning | Primary Uses |
|---|---:|---|---|
| `M_CanopyClosure` | `0..1` | overhead canopy cover / sky occlusion | light, temperature, rain interception, visibility, orientation |
| `M_UnderstoryDensity` | `0..1` | density of shrubs, saplings, palms, vines, ferns, broad leaves | movement friction, sightline loss, sound rustle |
| `M_LianaTangle` | `0..1` | woody vine / hanging vine obstruction | false affordance, snagging, noise, blocked gaps |
| `M_RootButtressComplexity` | `0..1` | buttress roots, exposed roots, stilt/prop-root obstacles | footing, narrow passability, visual landmarks |
| `M_FallenTimberLoad` | `0..1` | logs, branches, deadfall, cyclone/treefall debris | hard/soft blockers, climb-over cost |
| `M_DisturbanceGap` | `0..1` | canopy gap, treefall, storm damage, landslip opening | light, regrowth, false openings, route temptation |
| `M_RegrowthThicket` | `0..1` | dense young growth after disturbance | high resistance despite visual brightness |
| `M_PalmOrPandanusBarrier` | `0..1` | spiny/sword-leaf/palm-like obstruction class | painful-looking friction, blocked side routes |
| `M_GroundLayerTangle` | `0..1` | leaf litter, ferns, seedlings, low vines, rotten debris | low visibility at foot level, noise, slipping |
| `M_EcosystemTellStrength` | `0..1` | how strongly local ecology communicates terrain state | environmental readability, animal/insect cues |

Rainforest structure should support multiple vertical layers, vines, shrubs, epiphytes, and a closely spaced canopy. Canopy gaps are not merely open space: they increase light and microclimate exposure, trigger succession, and often favor fast-growing vines or regrowth, making them ideal for false-affordance design. ([wettropics.gov.au](https://www.wettropics.gov.au/rainforest-structure?utm_source=chatgpt.com))

### Perception And Pressure Masks

| Mask Name | Contract | Meaning | Primary Uses |
|---|---:|---|---|
| `M_VisibilityOcclusion` | `0..1` | ground-level sightline blockage | AI perception, player readability, evidence concealment |
| `M_SoundPropagation` | `0..1` | how far non-local sound carries | distant cues, relict evidence, animal calls |
| `M_SoundMasking` | `0..1` | local ambient masking from rain, surf, insects, water, wind | concealment, uncertainty, tension |
| `M_SkyWindow` | `0..1` | usable glimpse of sky through canopy | non-UI orientation, time-of-day feel, weather signs |
| `M_LightShaftContrast` | `0..1` | visible directional light / sunfleck contrast | route temptation, landmarking, mood |
| `M_FootingRisk` | `0..1` | slip/trip/ankle-risk likelihood | movement speed, noise, animation response |
| `M_NightDangerPressure` | `0..1` | night-specific movement uncertainty and sensory pressure | non-combat tension, audio/visibility balance |
| `M_EvidenceRetention` | `0..1` | likelihood tracks, broken vegetation, mud marks, prints remain readable | environmental investigation |
| `M_EvidenceConfusion` | `0..1` | likelihood evidence is overwritten by rain, leaf fall, animals, water | ambiguity and delayed interpretation |

Forest sound propagation differs from open environments because microclimate, vegetation density, and forest structure alter scattering, absorption, and ambient masking; detection distance also varies with frequency, distance, ambient sound level, diel cycle, and habitat attenuation. This supports separate `M_SoundPropagation` and `M_SoundMasking` rather than one generic “hearing” mask. ([ftp-public.abmi.ca](https://ftp-public.abmi.ca/home/publications/documents/471_Yip_etal_2017_SoundAttenuationinForestandRoadsideEnvironments_ABMI.pdf?utm_source=chatgpt.com))

## Traversal Resistance Model

### Core Concept

Traversal resistance should be a composed value, not a direct copy of foliage density.

Recommended normalized output:

```text
TR_Resistance = 0.0 easy movement
TR_Resistance = 1.0 impassable or effectively impassable
```

Recommended developer-only category enum:

```text
EJungleTraversalClass
- TR_NeutralFriction
- TR_Affordance
- TR_SoftBlocker
- TR_HardBlocker
- TR_FalseAffordance
- TR_HazardFriction
- TR_EvidenceSensitive
```

Recommended resistance struct names:

```text
FJungleTraversalSample
FJungleBiomeMaskSample
FJungleTraversalMaskWeights
FJunglePCGMaskPoint
```

### Traversal Categories

| Category | Resistance Range | Meaning | Example |
|---|---:|---|---|
| `TR_Affordance` | `0.00..0.25` | easier-than-average route | ridge spine, animal path, dry open understory |
| `TR_NeutralFriction` | `0.20..0.45` | normal jungle movement | moderate roots, leaf litter, partial understory |
| `TR_HazardFriction` | `0.35..0.65` | passable but risky/noisy | slick bank, wet rocks, buttress-root maze |
| `TR_SoftBlocker` | `0.55..0.80` | possible but slow, costly, readable | dense thicket, vine curtain, fallen branches |
| `TR_FalseAffordance` | visible openness high, actual resistance `0.60..0.90` | visually tempting but costly | bright canopy gap filled with lianas/regrowth |
| `TR_HardBlocker` | `0.85..1.00` | should route around | cliff, deep mud, mangrove root wall, dense deadfall |

### Placeholder Formula

Use this only as a pre-data contract. Tune later from playtest and authored biome passes.

```text
TR_Resistance =
    0.20 * M_SlopeBand
  + 0.16 * M_UnderstoryDensity
  + 0.12 * M_LianaTangle
  + 0.12 * M_FootingRisk
  + 0.10 * M_TopographicWetness
  + 0.08 * M_FallenTimberLoad
  + 0.08 * M_RootButtressComplexity
  + 0.06 * M_GroundLayerTangle
  + 0.04 * M_VisibilityOcclusion
  + 0.04 * M_RegrowthThicket
```

Then apply modifiers:

```text
if M_RidgeInfluence > 0.70 and M_SlopeBand < 0.45:
    TR_Resistance -= 0.10

if M_StreamInfluence > 0.65 and M_TopographicWetness > 0.65:
    TR_Resistance += 0.12

if M_DisturbanceGap > 0.60 and (M_LianaTangle > 0.55 or M_RegrowthThicket > 0.55):
    classify as TR_FalseAffordance

if M_SlopeBand > 0.88 or M_FootingRisk > 0.90:
    classify as TR_HardBlocker unless explicitly authored as climb/scramble route
```

Clamp final value to `[0.0, 1.0]`.

### Placeholder Thresholds

#### Slope

| `M_SlopeBand` | Approx. Slope | Traversal Meaning |
|---:|---:|---|
| `0.00..0.20` | `0..5°` | easy ground unless wet/muddy |
| `0.20..0.40` | `5..15°` | normal walkable slope |
| `0.40..0.60` | `15..25°` | tiring, route-selective |
| `0.60..0.80` | `25..35°` | scramble risk, wet slip risk |
| `0.80..1.00` | `35°+` | blocker candidate |

#### Wetness

| `M_TopographicWetness` | Meaning | Traversal Use |
|---:|---|---|
| `0.00..0.20` | dry exposed ground | better footing, louder leaf litter |
| `0.20..0.45` | normal moist rainforest floor | default |
| `0.45..0.65` | damp/soft | mild slip, track retention |
| `0.65..0.85` | muddy/seep-prone | slow, noisy, evidence-rich |
| `0.85..1.00` | saturated/swamp/channel | hard blocker or special route |

Topographic wetness should be computed or approximated from upslope contributing area and slope. The exact GIS method can be deferred, but the gameplay contract should preserve the idea that lower, flatter, convergent positions collect more water than steep convex positions. ([data.csiro.au](https://data.csiro.au/collection/csiro%3A5588?utm_source=chatgpt.com))

#### Canopy And Sky

| Mask | Range | Meaning | Use |
|---|---:|---|---|
| `M_CanopyClosure` | `0.00..0.30` | open or heavily disturbed | sky cue, heat/light, regrowth |
| `M_CanopyClosure` | `0.30..0.70` | broken/variable canopy | readable transition |
| `M_CanopyClosure` | `0.70..1.00` | closed rainforest canopy | low sky, dim floor, acoustic enclosure |
| `M_SkyWindow` | `0.00..0.20` | no useful sky | orientation-poor |
| `M_SkyWindow` | `0.20..0.55` | intermittent sky flecks | subtle non-UI cue |
| `M_SkyWindow` | `0.55..1.00` | clear overhead gap | route temptation and weather cue |

Closed rainforest canopy should be treated as normal, not exceptional. Australian rainforest/vine-thicket guidance describes closed and continuous canopy as a defining feature, with foliage cover typically exceeding 70%. ([dcceew.gov.au](https://www.dcceew.gov.au/sites/default/files/documents/mvg1-nvis-rainforests-and-vine-thickets.pdf?utm_source=chatgpt.com))

#### Visibility

| `M_VisibilityOcclusion` | Meaning | Approx. Design Target |
|---:|---|---|
| `0.00..0.25` | open sightlines | player can read landform ahead |
| `0.25..0.50` | partial occlusion | normal jungle uncertainty |
| `0.50..0.75` | heavy occlusion | short route planning only |
| `0.75..1.00` | visual wall | blocker, concealment, or intentional reveal gate |

#### Sound

| Mask | Range | Meaning |
|---|---:|---|
| `M_SoundPropagation` | `0.00..0.30` | sound dies quickly; dense/wet/noisy enclosure |
| `M_SoundPropagation` | `0.30..0.70` | ordinary forest propagation |
| `M_SoundPropagation` | `0.70..1.00` | ridge/coast/open corridor sound carries |
| `M_SoundMasking` | `0.00..0.30` | quiet enough for small cues |
| `M_SoundMasking` | `0.30..0.70` | insects/rain/leaves/water obscure detail |
| `M_SoundMasking` | `0.70..1.00` | surf, waterfall, storm, heavy rain, cicada wall |

### Hard Blocker, Soft Blocker, Affordance, False Affordance, Neutral Friction

#### Hard Blockers

Use hard blockers sparingly and make them physically legible.

Candidate conditions:

- `M_SlopeBand >= 0.88`;
- `M_TopographicWetness >= 0.90` plus `M_MicroReliefRoughness >= 0.60`;
- `M_FallenTimberLoad >= 0.85`;
- `M_LianaTangle >= 0.90` and `M_VisibilityOcclusion >= 0.75`;
- mangrove/tidal root mesh with deep mud;
- cliff, ravine, waterfall drop, impassable boulder choke.

Readable cues:

- vertical rock, exposed roots, deep shadowed mud, water sheen, fallen trunks stacked across slope, tangled vine wall, root spikes, audible sucking mud, water pooling.

#### Soft Blockers

Soft blockers are passable but intentionally unattractive.

Candidate conditions:

- `TR_Resistance >= 0.55 && TR_Resistance < 0.85`;
- dense regrowth after a gap;
- steep wet bank;
- root-buttress maze;
- fern/palm/pandanus-like dense leaf barriers;
- leaf-litter slope hiding uneven ground;
- narrow gully with fallen branches.

Readable cues:

- body-height foliage, visible snagging vines, forced crouch-like compression, wet leaf shine, branch clutter, dark ground, insect swarms, muffled audio.

#### Affordances

Affordances are naturally readable routes, not UI paths.

Candidate conditions:

- `TR_Resistance <= 0.25`;
- `M_RidgeInfluence >= 0.65 && M_SlopeBand <= 0.45`;
- animal trail-like vegetation depression;
- dry stream terrace above the wet channel;
- coast strand just inside salt spray if not mangrove/tidal;
- open understory beneath mature closed canopy;
- fallen log used as a short bridge only when stable and readable.

Readable cues:

- repeated gaps at foot height, consistent ground texture, fewer snagging vines, firm soil, audible openness, filtered light line, visible terrain continuation.

#### False Affordances

False affordances are essential for Jungle Game because they allow the world to test the player without explicit UI.

Candidate condition:

```text
M_LightShaftContrast >= 0.60
or M_SkyWindow >= 0.55
or M_VisibilityOcclusion <= 0.35
```

combined with:

```text
M_LianaTangle >= 0.55
or M_RegrowthThicket >= 0.55
or M_FootingRisk >= 0.65
or M_FallenTimberLoad >= 0.60
```

Examples:

- bright gap filled with lianas;
- streambed that looks navigable but becomes slick rock and root undercut;
- dry-looking leaf mat over mud;
- ridge shortcut that ends in a steep landslip scar;
- beach access that funnels into mangrove roots or tidal mud;
- animal path that is too low/narrow for human movement.

#### Neutral Friction

Neutral friction is default rainforest movement.

Candidate conditions:

- no dominant blocker;
- moderate understory;
- moderate root and leaf litter;
- no strong route cue;
- no strong visibility or sound advantage.

Neutral friction should still feel alive: small detours around roots, brushing leaves, changing footstep sound, occasional branch push, but not constant collision frustration.

## Tropical Vegetation And Terrain Inputs

### Vegetation Inputs

Use functional vegetation classes instead of final species lists.

| Input Class | Suggested Mask | Traversal Effect | Readability |
|---|---|---|---|
| Mature closed-canopy trees | `M_CanopyClosure` | low ground light, often clearer understory than gaps | dim, tall trunks, large roots |
| Buttress/root systems | `M_RootButtressComplexity` | trip, detour, narrow passage | obvious radial root walls |
| Lianas / woody vines | `M_LianaTangle` | snagging, visual curtain, false gap closure | hanging/looping vertical lines |
| Palm-like dense leaf barriers | `M_PalmOrPandanusBarrier` | painful-looking side blocker | spiky/sword leaves |
| Ferns / broad-leaf low plants | `M_GroundLayerTangle` | foot visibility loss, wet brushing | shin-to-waist green mass |
| Regrowth saplings | `M_RegrowthThicket` | dense obstruction in bright gaps | thin stems, chaotic repetition |
| Fallen logs / dead branches | `M_FallenTimberLoad` | climb-over, route block, noise | horizontal clutter |
| Epiphyte-heavy branches | visual/ecological tell | branch fall risk, humidity cue | mossy, dripping, heavy limbs |
| Mangrove/prop-root class | coast/wetland blocker | mud/root maze | roots above mud/water |

Rainforest trees often use shallow roots to exploit nutrients and moisture near the surface, and buttress roots are common adaptations in rainforest settings. These are not just visual landmarks; they are credible sources of micro-obstacles, foot placement decisions, and non-random path deflection. ([nationalparks.nsw.gov.au](https://www.nationalparks.nsw.gov.au/~/media/E8073DC1918348D3AD95B4650C8FE915.ashx?utm_source=chatgpt.com))

### Terrain Inputs

| Input | Suggested Mask | Traversal Effect | Readability |
|---|---|---|---|
| Ridge/spur | `M_RidgeInfluence` | dry corridor, better sky/sound | crest line, wind, thinner wet cues |
| Valley/gully | `M_ValleyInfluence` | wet, dark, noisy, constrained | water sound, steep sides, dense growth |
| Stream bank | `M_StreamInfluence` | water access but slick/undercut | stones, mud, exposed roots |
| Slope | `M_SlopeBand` | energy cost and slip risk | body angle, runoff lines |
| Aspect exposure | `M_AspectExposure` | wind/salt/sun pressure | canopy lean, drier leaves, surf/wind |
| Coast edge | `M_CoastInfluence` | strong orientation cue, possible root/mud blocker | surf, salt wind, littoral canopy |
| Landslip/treefall scar | `M_LandslipDebris`, `M_DisturbanceGap` | false opening, unstable ground | exposed soil, bright gap |
| Swale/seep | `M_TopographicWetness` | mud, insects, evidence retention | glossy leaves, pooling, darker soil |
| Boulder/rock outcrop | `M_MicroReliefRoughness` | detour, scramble, dry landmark | exposed stone, sound reflection |

### Coast And Littoral Inputs

Coastal rainforest and littoral transitions should differ from inland rainforest. Coastal processes such as salt spray, tidal inundation, storm tide, onshore wind, and unstable maritime substrates shape littoral rainforest; exposed coastal canopy may show windshear and lower/variable heights. Mangrove transitions add muddy, waterlogged substrates and complex aerial/prop roots. ([nesplandscapes.edu.au](https://nesplandscapes.edu.au/wp-content/uploads/2017/02/Mapping-to-underpin-management-of-tropical-littoral_rainforest_Report_Feb2017_WEB.pdf?utm_source=chatgpt.com))

Recommended coast-specific masks:

| Mask | Use |
|---|---|
| `M_CoastInfluence` | broad coastal exposure gradient |
| `M_SaltWindExposure` | canopy deformation, drier/stunted edge, audible wind |
| `M_TidalMudRisk` | hard blocker or special low-speed route |
| `M_MangroveRootBarrier` | root maze blocker, visibility loss |
| `M_SurfSoundMasking` | strong audio masking near shore |
| `M_LittoralCanopyCompression` | lower wind-shaped canopy and edge density |

## False Affordances And Readable Friction

### Design Principle

A false affordance should never feel like arbitrary punishment. It should be visually tempting for a reason and costly for a reason.

Good false affordance formula:

```text
temptation cue + hidden-but-readable cost + recoverable decision
```

Bad false affordance formula:

```text
open-looking route + invisible collision + no warning + forced backtracking
```

### Recommended False Affordance Types

#### 1. Bright Gap, Vine Trap

- High `M_SkyWindow`
- High `M_LightShaftContrast`
- High `M_LianaTangle`
- High `M_RegrowthThicket`
- Medium/high `M_FallenTimberLoad`

Player read:

- “That looks open.”
- Closer read: hanging vines, dense saplings, branch clutter.
- Outcome: slow/noisy route or dead end.

#### 2. Stream Corridor Temptation

- High `M_StreamInfluence`
- High `M_SoundMasking`
- Medium/high `M_TopographicWetness`
- Medium/high `M_FootingRisk`
- Variable `M_VisibilityOcclusion`

Player read:

- “Watercourse might lead somewhere.”
- Closer read: slick rock, undercut banks, tangled roots.
- Outcome: useful orientation and water cue, but slow and noisy.

#### 3. Dry Leaf Mat Over Soft Ground

- Medium `M_CanopyClosure`
- Medium/high `M_TopographicWetness`
- High `M_GroundLayerTangle`
- High `M_EvidenceRetention`
- Medium/high `M_EvidenceConfusion`

Player read:

- “Looks flat.”
- Closer read: darker leaf litter, shine, insects, soft footstep sound.
- Outcome: mud, prints, slow movement.

#### 4. Ridge Shortcut Break

- High `M_RidgeInfluence`
- Low/medium `M_TopographicWetness`
- Rising `M_SlopeBand`
- High `M_LandslipDebris` at break point

Player read:

- “Ridge is easy.”
- Closer read: exposed soil, root tears, sudden canopy gap.
- Outcome: detour, landmark, possible evidence point.

#### 5. Coast Exit Trap

- High `M_CoastInfluence`
- High `M_SurfSoundMasking`
- High `M_TidalMudRisk` or `M_MangroveRootBarrier`
- Low/medium `M_SkyWindow`

Player read:

- “Follow coast.”
- Closer read: root lattice, mud, tidal channels, insect/shorebird change.
- Outcome: coast is a macro-orientation anchor, not always a walkable path.

### Readable Friction Rules

1. **Friction must appear before collision.**
   - Use visible density, leaf shape, vine lines, wetness shine, ground clutter, and footstep sound.

2. **Collision should match silhouette.**
   - A thin leaf should not stop the player.
   - A root wall, fallen trunk, dense vine rope, or trunk cluster can.

3. **Vegetation density should have gradients.**
   - Avoid isolated invisible blockers.
   - Build edges: sparse fringe → medium obstruction → full thicket.

4. **Every hard blocker should imply a natural reason.**
   - cliff, root wall, deep mud, deadfall, dense regrowth, mangrove mesh, boulder choke.

5. **Affordances should not be too clean.**
   - An easy route can still brush leaves, bend around roots, and obscure long-range view.

## Developer-Only Debug View Requirements

### Debug View Names

Recommended debug modes:

```text
DBG_Mask_ElevationBand
DBG_Mask_SlopeBand
DBG_Mask_AspectExposure
DBG_Mask_TopographicWetness
DBG_Mask_RidgeValley
DBG_Mask_CoastInfluence
DBG_Mask_CanopyClosure
DBG_Mask_DisturbanceGap
DBG_Mask_VisibilityOcclusion
DBG_Mask_SoundPropagation
DBG_Mask_SoundMasking
DBG_Mask_SkyWindow
DBG_Mask_FootingRisk
DBG_Mask_TraversalResistance
DBG_Mask_TraversalClass
DBG_Mask_FalseAffordance
DBG_Mask_EvidenceRetention
DBG_Mask_NightDangerPressure
```

### Debug View Rules

Developer debug views may show:

- normalized mask heatmaps;
- sampled PCG point values;
- per-cell traversal class;
- threshold bands;
- blocker/affordance classification;
- mask source contributions;
- validation warnings;
- PCG spawn influence;
- audio-zone influence;
- visibility ray samples;
- before/after composition values.

Developer debug views must not become:

- player map;
- minimap;
- compass substitute;
- route planner;
- bearing indicator;
- explicit time/orientation tool;
- confidence meter;
- hidden player-facing accessibility layer that reveals orientation correctness.

### Editor-Only Enforcement

Recommended technical constraints:

1. All debug materials, widgets, and overlays live under editor/development-only folders or modules.
2. Runtime shipping builds strip or disable debug commands.
3. No player input binding should toggle mask views.
4. Debug screenshots should be marked development-only.
5. No mask texture should be exposed to player-facing UI materials.
6. Any in-world debug coloration must require editor/dev console access.
7. Debug labels should use internal mask names, not player-readable navigation language.

### Debug Visualization Style

Use intentionally technical labels:

Good:

```text
DBG_Mask_RidgeValley
M_TopographicWetness = 0.74
TR_Class = TR_SoftBlocker
```

Avoid:

```text
You are lost
Good path
Bad path
North
Safe route
Objective direction
Time left before dark
```

Unreal PCG supports point data with built-in properties and user-defined attributes, and PCG generation can be partitioned or hierarchical for larger worlds. This makes mask attributes suitable as developer-facing PCG inputs, filters, and debug data, while keeping them separate from player UI. ([dev.epicgames.com](https://dev.epicgames.com/documentation/unreal-engine/procedural-content-generation-overview?utm_source=chatgpt.com))

## UE5 PCG And Data Contract Guidance

### Recommended Data Contract

Use one authoritative landscape/biome sampling contract that PCG, audio, AI, traversal validation, and evidence systems can consume.

Suggested struct:

```text
FJungleBiomeMaskSample
- FVector WorldPosition
- float M_ElevationBand
- float M_SlopeBand
- float M_AspectExposure
- float M_TopographicWetness
- float M_RidgeInfluence
- float M_ValleyInfluence
- float M_CoastInfluence
- float M_StreamInfluence
- float M_CanopyClosure
- float M_DisturbanceGap
- float M_UnderstoryDensity
- float M_LianaTangle
- float M_RootButtressComplexity
- float M_FallenTimberLoad
- float M_RegrowthThicket
- float M_VisibilityOcclusion
- float M_SoundPropagation
- float M_SoundMasking
- float M_SkyWindow
- float M_FootingRisk
- float M_EvidenceRetention
- float M_EvidenceConfusion
- float M_NightDangerPressure
- float TR_Resistance
- EJungleTraversalClass TR_Class
```

### PCG Input Contracts

PCG graphs should accept:

| Input | Type | Notes |
|---|---|---|
| Heightfield / Landscape | sampled raster or landscape query | source for elevation, slope, aspect |
| Hydrology approximation | raster/spline/derived mask | streams, wetness, valleys |
| Coastline distance field | raster/spline distance | surf, salt, littoral transitions |
| Canopy field | authored/procedural raster or point cloud | closure, sky windows, light shafts |
| Disturbance field | authored mask + procedural noise | treefall, storm gaps, landslips |
| Vegetation class weights | attribute set / data table | functional classes, not final species |
| Traversal override volumes | editor-authored volumes | hand-tune key routes/blockers |
| Evidence zones | volumes/points | prints, broken vegetation, moved objects |

### PCG Output Contracts

PCG graphs should output:

| Output | Type | Use |
|---|---|---|
| `TR_Resistance` | float attribute | movement/audio/AI/evidence cost |
| `TR_Class` | enum/int attribute | debug and rule branching |
| `Spawn_DenseUnderstory` | density | shrubs, broad leaves, ferns |
| `Spawn_LianaCurtain` | density | vine obstruction |
| `Spawn_ButtressRoot` | density | root landmarks/obstacles |
| `Spawn_FallenTimber` | density | deadfall blockers |
| `Spawn_RegrowthThicket` | density | disturbed gap friction |
| `Spawn_SkyGap` | density or negative canopy | light/sky opening |
| `Audio_SoundMasking` | float | water, rain, insect, surf mix |
| `Visibility_MaxMeters` | float | perception tuning and validation |
| `Evidence_Retention` | float | track/trace persistence |
| `Evidence_Confusion` | float | decay/overwrite tendency |

### Attribute Naming Rules

Use stable, explicit names:

Good:

```text
M_TopographicWetness
M_RidgeInfluence
M_VisibilityOcclusion
TR_Resistance
TR_Class
Spawn_LianaCurtain
Audio_SoundMasking
Evidence_Retention
```

Avoid:

```text
Wet
JungleAmount
Badness
Path
Scary
PlayerLost
NorthHint
```

### Resolution Guidance

Use multiple mask resolutions.

| Resolution Class | Approx. Cell Size | Use |
|---|---:|---|
| Macro | `32..128 m` | elevation bands, coast influence, broad biome transitions |
| Meso | `8..32 m` | ridge/valley, wetness, canopy closure, disturbance |
| Micro | `1..8 m` | roots, deadfall, footing, final vegetation friction |
| Runtime local | `0.25..2 m` | collision-adjacent validation, footstep surface, close evidence |

### Implement Now

Implement now:

1. Mask naming contract.
2. `FJungleBiomeMaskSample` equivalent data shape.
3. Placeholder normalized masks for:
   - `M_ElevationBand`
   - `M_SlopeBand`
   - `M_TopographicWetness`
   - `M_RidgeInfluence`
   - `M_ValleyInfluence`
   - `M_CoastInfluence`
   - `M_CanopyClosure`
   - `M_UnderstoryDensity`
   - `M_DisturbanceGap`
   - `M_VisibilityOcclusion`
   - `M_SkyWindow`
   - `M_FootingRisk`
   - `TR_Resistance`
   - `TR_Class`
4. Developer-only debug heatmaps.
5. PCG attribute passthrough and validation reports.
6. Simple false-affordance classifier.

### Defer

Defer:

1. Final species lists.
2. Full hydrological simulation.
3. True ecological succession simulation.
4. Complex animal-path generation.
5. Full acoustic physics.
6. ML/remote-sensing-derived canopy classification.
7. Fine-grained phenology/seasonality.
8. Final traversal animation responses.
9. Runtime dynamic regrowth.
10. Full weather erosion feedback.

## Validation Points And Sample Tables

### Validation Principles

A mask is valid only if it produces consistent world behavior.

Developer validation should answer:

1. Are ridges generally more legible than gullies?
2. Are valleys wetter, darker, louder, and more obstructed?
3. Do canopy gaps increase light without always becoming easy paths?
4. Are hard blockers visually obvious before impact?
5. Does dense vegetation create readable resistance instead of random collision?
6. Do sound-masking zones match water, rain, insects, surf, or wind?
7. Do sky windows appear as world cues, not UI substitutes?
8. Do coast and ridge features help orientation without compass-like readouts?
9. Are false affordances recoverable, not punitive?
10. Are debug views unavailable in player-facing builds?

### Sample Mask Composition Table

| Location Type | Key Mask Signature | Expected Traversal |
|---|---|---|
| Mature interior rainforest | high canopy, medium understory, medium wetness | neutral friction, low sky, short sightlines |
| Ridge spine | high ridge, lower wetness, moderate canopy breaks | affordance or neutral friction |
| Steep ridge side | high slope, root complexity, lower wetness | hazard friction or soft blocker |
| Valley floor | high wetness, high valley, high sound masking | slow, muddy, evidence-retaining |
| Stream bank | high stream, high footing risk | useful orientation, risky movement |
| Canopy gap | high sky window, high disturbance | affordance only if low regrowth/liana |
| Liana gap | high sky, high liana, high regrowth | false affordance |
| Mangrove edge | high coast, high tidal mud, high root barrier | hard blocker or special slow route |
| Littoral forest edge | high coast, salt wind, variable canopy | macro-orientation cue, mixed friction |
| Landslip scar | high disturbance, high slope/debris | false shortcut or blocker |

### Placeholder Category Table

| Category | Required Conditions | Developer Validation |
|---|---|---|
| `TR_Affordance` | low resistance, visible continuation | should look naturally easier |
| `TR_NeutralFriction` | moderate resistance, no dominant cue | should feel like default jungle |
| `TR_HazardFriction` | medium resistance plus high footing/noise risk | should look risky before entry |
| `TR_SoftBlocker` | high resistance but not impossible | should be passable but unattractive |
| `TR_HardBlocker` | extreme slope/mud/root/deadfall | should be visibly impassable |
| `TR_FalseAffordance` | tempting visibility/light plus hidden friction | should invite inspection, not blind punishment |
| `TR_EvidenceSensitive` | high evidence retention/confusion relevance | should support tracks, broken plants, prints |

### Debug Validation Reports

Recommended report names:

```text
JungleMaskCoverageReport
JungleTraversalClassReport
JungleFalseAffordanceReport
JungleHardBlockerContinuityReport
JungleSkyWindowDistributionReport
JungleSoundMaskingZoneReport
JungleEvidenceRetentionReport
JungleForbiddenUIDriftReport
```

### Minimum Validation Metrics

| Metric | Target |
|---|---|
| hard blockers with clear visual source | `>= 95%` |
| false affordances with visible close-range warning | `>= 90%` |
| affordance routes interrupted by arbitrary collision | `<= 5%` |
| player-facing UI references to orientation masks | `0` |
| debug mask materials included in shipping UI | `0` |
| valley cells with wetness/audio mismatch | investigate if `> 20%` |
| ridge cells classified as saturated without local reason | investigate if `> 10%` |
| canopy gaps classified easy despite high liana/regrowth | investigate if `> 15%` |

## Risks, Pitfalls, And Forbidden Drift

### Risks

1. **Random collision noise**
   - Risk: foliage becomes frustrating because collision does not match visible structure.
   - Mitigation: collision only on believable blockers: trunks, roots, dense vine masses, fallen timber, thick leaf barriers.

2. **Debug view leakage**
   - Risk: mask overlays become player navigation tools.
   - Mitigation: editor-only modules, stripped shipping commands, no player input bindings.

3. **Over-clean paths**
   - Risk: world starts to look like a theme-park trail network.
   - Mitigation: affordances should be subtle, broken, and environmentally justified.

4. **Every gap becomes a path**
   - Risk: canopy openings falsely teach the player that bright equals safe/easy.
   - Mitigation: disturbance gaps often include lianas, deadfall, regrowth, slick exposed soil.

5. **Generic forest assumptions**
   - Risk: using temperate/pine forest logic.
   - Mitigation: prioritize closed canopy, lianas, buttress roots, wet leaf litter, rapid regrowth, storm gaps, mangrove/littoral transitions, dense understory variation.

6. **Hydrology as decoration**
   - Risk: water and mud do not influence traversal, sound, evidence, or vegetation.
   - Mitigation: `M_TopographicWetness`, `M_StreamInfluence`, and `M_ValleyInfluence` must affect multiple systems.

7. **Orientation UI drift**
   - Risk: debug concepts become character callouts or hidden meters.
   - Mitigation: player reads only diegetic cues: sky gaps, ridge lines, water flow, surf sound, light, wind, vegetation transition.

### Forbidden Drift

Do not add or imply:

- player map;
- minimap;
- compass;
- bearing readout;
- GPS;
- objective marker navigation;
- HUD clock;
- explicit time readout;
- orientation-confidence UI;
- hidden orientation meter;
- character comments saying the player is oriented or lost;
- modern survival tools as required navigation systems;
- combat/chase/monster traversal framing;
- fantasy-magic biome behavior;
- protected-place replication;
- culture-coded “tribal jungle” stereotypes.

## PR-Ready Recommendations

### Implement In PR5 Batch 001 Scope

1. Add this document as:

```text
002_tropical_biome_masks_traversal_research.md
```

2. Establish the initial mask vocabulary:

```text
M_ElevationBand
M_SlopeBand
M_AspectExposure
M_TopographicWetness
M_RidgeInfluence
M_ValleyInfluence
M_CoastInfluence
M_StreamInfluence
M_CanopyClosure
M_UnderstoryDensity
M_LianaTangle
M_RootButtressComplexity
M_FallenTimberLoad
M_DisturbanceGap
M_RegrowthThicket
M_VisibilityOcclusion
M_SoundPropagation
M_SoundMasking
M_SkyWindow
M_FootingRisk
M_EcosystemTellStrength
M_EvidenceRetention
M_EvidenceConfusion
M_NightDangerPressure
TR_Resistance
TR_Class
```

3. Establish traversal categories:

```text
TR_Affordance
TR_NeutralFriction
TR_HazardFriction
TR_SoftBlocker
TR_HardBlocker
TR_FalseAffordance
TR_EvidenceSensitive
```

4. Add placeholder normalized contracts:
   - all masks are `float` in `[0.0, 1.0]`;
   - `0.0` means absent/minimum influence;
   - `1.0` means maximum influence;
   - enum/int classes are derived from masks and never hand-authored as player UI.

5. Implement developer-only debug view names from this document.

6. Add validation report stubs:
   - `JungleMaskCoverageReport`
   - `JungleTraversalClassReport`
   - `JungleFalseAffordanceReport`
   - `JungleForbiddenUIDriftReport`

7. Add explicit forbidden UI guard language to implementation notes:
   - masks may drive world, PCG, AI, audio, weather, evidence, and debug;
   - masks must not drive player-facing navigation UI.

### Defer Beyond This PR

Defer final tuning of:

- exact biome thresholds;
- final vegetation species;
- full hydrology;
- acoustic propagation model;
- evidence decay;
- animal behavior;
- animation-specific traversal responses;
- weather-driven runtime mask changes;
- handcrafted hero-area overrides.

### Acceptance Criteria

This research/spec document is PR-ready if it:

- uses the exact required top-level structure;
- gives concrete mask names and contracts;
- defines traversal categories and placeholder thresholds;
- distinguishes hard blockers, soft blockers, affordances, false affordances, and neutral friction;
- treats tropical vegetation as readable traversal friction;
- includes developer-only debug guidance;
- includes UE5 PCG data-contract guidance;
- includes validation points and sample tables;
- forbids player-facing navigation UI drift;
- cites real sources.

## Sources

- Queensland Government — “Rainforests”
  URL: https://www.qld.gov.au/environment/plants-animals/habitats/rainforests

- Queensland Government WetlandInfo — “Rainforest”
  URL: https://wetlandinfo.detsi.qld.gov.au/wetlands/ecology/processes-systems/nitrogen-concept-model/rainforest/

- Wet Tropics Management Authority — “Rainforest structure”
  URL: https://www.wettropics.gov.au/rainforest-structure

- Australian Government Department of Climate Change, Energy, the Environment and Water — “NVIS Fact sheet MVG 1: Rainforests and vine thickets”
  URL: https://www.dcceew.gov.au/sites/default/files/documents/mvg1-nvis-rainforests-and-vine-thickets.pdf

- NSW National Parks and Wildlife Service — “Rainforest plant adaptations”
  URL: https://www.nationalparks.nsw.gov.au/~/media/E8073DC1918348D3AD95B4650C8FE915.ashx

- Australian National Botanic Gardens — “Rainforest: surviving the wet and dark”
  URL: https://visit.anbg.gov.au/static/2b2c6b812484fc84312c46b098e22f30/anbg-document-rainforest-gully-teacher-notes.pdf

- National Geographic Education — “Rainforest”
  URL: https://education.nationalgeographic.org/resource/rain-forest/

- Schnitzer, S. A. and Bongers, F. — “The Ecology of Lianas and Their Role in Forests”
  URL: https://epublications.marquette.edu/cgi/viewcontent.cgi?article=1743&context=bio_fac

- Schnitzer, S. A., Dalling, J. W., and Carson, W. P. — “The impact of lianas on tree regeneration in tropical forest canopy gaps”
  URL: https://besjournals.onlinelibrary.wiley.com/doi/10.1046/j.1365-2745.2000.00489.x

- NSW National Parks and Wildlife Service — “Under the Canopy: A guide to the rainforests of NSW”
  URL: https://www.nationalparks.nsw.gov.au/-/media/visitor/files/pdf/education/rainforests-of-nsw/under-the-canopy-a-guide-to-the-rainforests-of-nsw.pdf

- Amatulli, G. et al. — “A suite of global, cross-scale topographic variables for environmental and biodiversity modeling”
  URL: https://www.nature.com/articles/sdata201840

- Sørensen, R., Zinko, U., and Seibert, J. — “On the calculation of the topographic wetness index”
  URL: https://hess.copernicus.org/articles/10/101/2006/hess-10-101-2006.pdf

- CSIRO Data Access Portal — “Topographic Wetness Index derived from 1 second SRTM DEM-H”
  URL: https://data.csiro.au/collection/csiro%3A5588

- TERN Soil and Landscape Grid of Australia — “Landscape Attribute Details”
  URL: https://esoil.io/TERNLandscapes/Public/Pages/SLGA/ProductDetails-LandscapeAttributes.html

- Department of Climate Change, Energy, the Environment and Water — “Littoral rainforests and coastal vine thickets of eastern Australia”
  URL: https://www.dcceew.gov.au/sites/default/files/documents/littoral-rainforest.pdf

- NESP Tropical Water Quality Hub — “Mapping to underpin management of tropical littoral rainforest”
  URL: https://nesplandscapes.edu.au/wp-content/uploads/2017/02/Mapping-to-underpin-management-of-tropical-littoral_rainforest_Report_Feb2017_WEB.pdf

- Queensland Government WetlandInfo — “Mangroves”
  URL: https://wetlandinfo.detsi.qld.gov.au/wetlands/ecology/components/biota/flora/flora-structural/mangroves/

- NOAA Ocean Service — “What is a mangrove forest?”
  URL: https://oceanservice.noaa.gov/facts/mangroves.html

- Yip, D. A. et al. — “Sound attenuation in forest and roadside environments”
  URL: https://ftp-public.abmi.ca/home/publications/documents/471_Yip_etal_2017_SoundAttenuationinForestandRoadsideEnvironments_ABMI.pdf

- Haupert, S., Sèbe, F., and Sueur, J. — “Physics-based model to predict the acoustic detection distance of terrestrial autonomous recording units over the diel cycle and across seasons”
  URL: https://arxiv.org/abs/2211.16077

- Epic Games — “Procedural Content Generation Overview”
  URL: https://dev.epicgames.com/documentation/unreal-engine/procedural-content-generation-overview

- Epic Games — “Procedural Content Generation Framework Data Types Reference”
  URL: https://dev.epicgames.com/documentation/unreal-engine/procedural-content-generation-framework-data-types-reference-in-unreal-engine

- Epic Games — “Using PCG Generation Modes in Unreal Engine”
  URL: https://dev.epicgames.com/documentation/unreal-engine/using-pcg-generation-modes-in-unreal-engine

- Epic Games — “Procedural Content Generation Framework Node Reference”
  URL: https://dev.epicgames.com/documentation/unreal-engine/procedural-content-generation-framework-node-reference-in-unreal-engine
