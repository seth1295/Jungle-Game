# Creek Riparian Wet Valley Research

## Executive Summary

Creeks in **Jungle Game** should behave as **readable but imperfect landform systems**, not painted route lines. In a wet tropical island world, drainage corridors naturally organize terrain: rain falls onto ridges and slopes, concentrates into gullies and headwater channels, joins tributaries, cuts valleys, and exits through creek mouths, floodplains, estuaries, or coast-adjacent wetlands. A watershed is the land area draining rainfall and streams to a common outlet, and stream networks are often structured by ridges, valleys, headwaters, tributaries, confluences, and mouths. ([usgs.gov](https://www.usgs.gov/water-science-school/science/watersheds-and-drainage-basins?utm_source=chatgpt.com))

For implementation, creeks should provide **movement evidence**, **survival pressure**, **sound anchors**, **crossing decisions**, and **false affordances**. The player can use them because they are physically legible: water flows downhill, banks reveal wetness and erosion, valley acoustics carry water sound, frogs and insects cluster in wet microhabitats, and creek mouths imply coast or basin exits. They remain imperfect because banks collapse, vegetation blocks movement, tributaries split, waterfalls trap routes, flood pulses change crossings, and dense canopy limits long sightlines.

The core design rule is: **a creek may help the player reason, but it must never become an explicit navigation UI.** No map, compass, bearing, objective marker, orientation confidence meter, or voiced confirmation should be added. Instead, the game should expose **environmental evidence**: slope, sound gradient, water turbidity, foam, wet rock sheen, bank shape, debris lines, vegetation density, animal sound bands, and crossing geometry.

Immediate PR value should focus on **data masks, authored/procedural creek metadata, placeholder traversal risks, sound anchor placement, and debug/evidence views**. Full hydrology simulation, disease systems, wildlife AI, final water physics, and exact real-world protected-place replication should remain deferred.

## Design Goals For Jungle Game

Creek and wet-valley systems should support these goals:

1. **Creeks as natural structure**
   - Creeks organize the island into catchments, tributary corridors, gullies, wet basins, ridge separations, and coastal exits.
   - Each drainage line should have a source-to-mouth identity: headwater seep, narrow gully, rocky cascade, pool-run sequence, lowland creek, swampy reach, tidal mouth, or waterfall barrier.

2. **Useful but unreliable navigation**
   - Following water downstream should often lead toward lower terrain, larger channels, basins, estuaries, or coast.
   - Following water upstream should often lead toward ridges, slopes, gullies, waterfalls, or dead-end headwater seeps.
   - Tributary splits, bank blockers, flood pulses, and false walkable shelves should prevent creeks from becoming guaranteed routes.

3. **Physical first-person readability**
   - Communicate slope, water direction, depth, crossing risk, wet footing, blockage, and channel scale using visible and audible cues.
   - Avoid UI overlays that interpret those cues for the player.

4. **Tropical specificity**
   - Use dense riparian vegetation, buttress roots, palms, vines, pandanus-like edge forms, lawyer cane / wait-a-while analogues, slippery rocks, mud, leeches, insects, frog choruses, flood debris, and high rainfall response.
   - Avoid temperate assumptions such as open grassy streambanks, gentle meadows, predictable stepping-stone crossings, or clear long-distance sightlines.

5. **Nonviolent unsettling layer**
   - The relict hominin presence may subtly modify creek evidence: blocked bank gaps, moved deadfall, placed stones, disturbed mud, altered sound occlusion, or false crossing invitations.
   - These changes should remain physically plausible and non-magical.

6. **Developer-verifiable systems**
   - Every creek affordance should be inspectable in developer-only evidence views: masks, slope, wetness, flow direction, crossing candidates, sound anchors, risk zones, bank blockers, and route traps.

## Creek And Wet-Valley Landform Principles

A wet tropical creek network should be generated and authored around **catchment logic** rather than arbitrary decorative water splines.

**Core landform hierarchy**

- **Ridge / divide**
  - High ground separating drainage basins.
  - Useful because crossing a ridge often changes water-flow direction and soundscape.
  - Implementation output: `LG_RidgeDivide`, `LG_CatchmentBoundary`.

- **Headwater seep**
  - Small, often discontinuous water emergence on a slope or hollow.
  - May be mostly wet soil, mossy rock, trickle audio, frogs, and saturated leaf litter rather than visible flowing water.
  - Implementation output: `CreekClass_HeadwaterSeep`.

- **Ephemeral gully**
  - Channel that may run strongly after rain and become mud, damp stone, or leaf-choked depression in dry periods.
  - Important for false route logic: it looks like a corridor but may vanish into vegetation or steep slope.
  - Implementation output: `CreekClass_EphemeralGully`.

- **Low-order creek**
  - Narrow channel with visible flow, roots, stones, small pools, and frequent obstructions.
  - Good for movement evidence but often poor for direct travel.
  - Implementation output: `CreekClass_NarrowCreek`.

- **Confluence**
  - Junction where two channels meet.
  - Strong orientation evidence because sound, flow scale, debris, and valley shape change.
  - Risk: confluences can confuse downstream/upstream choice and create route loops.
  - Implementation output: `LG_ConfluenceNode`.

- **Wet valley floor**
  - Broad saturated corridor around a creek, often with mud, thick vegetation, seepage, animal sound, and low visibility.
  - Movement may be possible along edges rather than beside the channel itself.
  - Implementation output: `CreekClass_WetValley`.

- **Cascade / waterfall / plunge pool**
  - Steeper reach where water sound becomes dominant and traversal often shifts from bank walking to detour searching.
  - Useful as a sound landmark and hard blocker.
  - Implementation output: `CreekClass_Cascade`, `CreekClass_Waterfall`.

- **Creek mouth / estuarine edge**
  - Drainage outlet to beach, lagoon, mangrove-like wetland, or tidal inlet.
  - Useful for world-scale orientation but should not guarantee safety.
  - Implementation output: `CreekClass_CreekMouth`, `CreekClass_TidalReach`.

The Wet Tropics of Queensland are described by government and heritage sources as rugged, mountainous, riverine, gorge-and-waterfall-rich tropical rainforest landscapes, which is a useful regional analogue for dense wet tropical island terrain without copying exact protected places. ([dcceew.gov.au](https://www.dcceew.gov.au/parks-heritage/heritage/places/world/wet-tropics?utm_source=chatgpt.com))

**Readable movement structures**

| Structure | Player-readable evidence | Gameplay use | Failure mode |
|---|---|---|---|
| Drainage corridor | Lower ground, damp air, water sound, frogs, darker soil | Find water, infer downhill direction | Corridor blocked by vegetation or cliffs |
| Bank bench | Slightly raised flat strip beside channel | Short movement lane | Ends at undercut bank, deadfall, or vine wall |
| Gravel/stone bar | Exposed stones inside bend or shallow reach | Crossing candidate | Slippery, loose, submerged after rain |
| Root shelf | Exposed buttress/root mat along bank | Footing clue and handhold-like visual | Undercut hollow collapses or blocks direct path |
| Confluence | Louder water, visible merging, debris wedge | Decision node | Player follows wrong tributary |
| Creek mouth | Wider, flatter, brackish/tidal vegetation, sediment fan | Coast/basin evidence | Mangrove/tidal mud trap |
| Waterfall sound cone | Directional roar, mist, cooler air | Landmark | Sound masks other cues and leads to impasse |

**Catchment generation principle**

Creeks should descend from high terrain to outlets through a believable hierarchy:

```text id="54mw1x"
RidgeDivide
  -> HeadwaterSeep
  -> EphemeralGully
  -> NarrowCreek
  -> Confluence
  -> WetValleyReach
  -> Cascade/Pool/Run Sequence
  -> LowlandCreek
  -> CreekMouth / Swamp / Estuary / Coast
```

This does not require full fluid simulation. It requires **consistent terrain drainage metadata**.

Recommended creek/riparian mask names:

- `M_Creek_ChannelCore`
- `M_Creek_WettedBed`
- `M_Creek_ShallowWater`
- `M_Creek_DeepPool`
- `M_Creek_RiffleCascade`
- `M_Creek_BankToe`
- `M_Creek_LowerBank`
- `M_Creek_UpperBankBench`
- `M_Creek_UndercutBank`
- `M_Creek_CollapsibleBank`
- `M_Creek_RiparianDense`
- `M_Creek_RiparianOpenGap`
- `M_Creek_WetValleyFloor`
- `M_Creek_SeepageSlope`
- `M_Creek_MudPatch`
- `M_Creek_DebrisJam`
- `M_Creek_FloodDebrisLine`
- `M_Creek_CrossingCandidate`
- `M_Creek_RouteTrap`
- `M_Creek_HardBlocker`
- `M_Creek_SoundOccluderVegetation`

Recommended catchment metadata:

- `CatchmentId`
- `DrainageOrder`
- `FlowDirectionVector`
- `DownstreamNodeId`
- `UpstreamNodeIds`
- `NearestOutletId`
- `ReachClass`
- `ReachSlope`
- `ValleyConfinement`
- `BankAsymmetry`
- `RecentRainResponse`
- `TidalInfluence`
- `CrossingDensityBudget`

## Creek Traversal Affordance Model

Creek banks should be treated as **dynamic affordance zones** instead of binary walkable/not walkable geometry. A bank’s gameplay role depends on slope, wetness, shape, vegetation, flow scale, and obstruction.

Recommended enum:

```text id="r26w7m"
ECreekBankAffordance
- OpenTravel
- SlowTravel
- CarefulFooting
- SoftBlocker
- HardBlocker
- FalseAffordance
- RouteTrap
- CrossingApproach
- ForcedDetour
```

Recommended bank-side fields:

```text id="hdea3h"
BankSide: Left | Right
BankSlopeDegrees
BankHeightM
BankShape: LowShelf | SlopedMud | RootedToe | Undercut | VerticalCut | BoulderEdge | SwampMargin
SurfaceWetness: Dry | Damp | Wet | Saturated | Flooded
VegetationFriction: Open | Light | Dense | Tangled | Impassable
FootingRisk: Stable | Muddy | SlipperyRock | LooseStone | RootTangle | CollapseEdge
FlowAdjacency: DryBench | ToeWet | InChannel | Overflow
TraversalRole: OpenTravel | SlowTravel | SoftBlocker | HardBlocker | FalseAffordance | RouteTrap
```

**Affordance zones**

1. **Open travel**
   - Rare in dense wet tropical creek margins.
   - Use sparingly for short bank shelves, animal paths, scoured flood benches, or naturally open rock bars.
   - Cues: visible continuous ground, lower vegetation, firm leaf litter, dry roots, stable slope.

2. **Slow travel**
   - Default for many riparian margins.
   - Cues: ankle-high roots, wet leaves, scattered vines, uneven stones, shallow mud.
   - Mechanics: reduce movement speed, increase footstep noise, increase slip stumble chance, increase leech/mud nuisance tags.

3. **Careful footing**
   - Used near wet rocks, root mats, shallow water, and steep bank toes.
   - Cues: dark glossy rock, algae-like sheen, water film, uneven root web, rushing nearby water.
   - Mechanics: camera bob disruption, footstep splash, temporary speed cap, no health system required.

4. **Soft blocker**
   - Player can push through but with high time/noise cost.
   - Cues: dense saplings, palm fronds, hanging vines, lawyer cane/wait-a-while analogues, deadfall.
   - Mechanics: slow movement, snag animation/audio, increased disorientation risk through obscured sightlines.

5. **Hard blocker**
   - Natural impassable barrier without invisible walls where possible.
   - Cues: vertical bank, deep pool, waterfall, boulder choke, flood-swollen channel, dense thorn/vine wall, cliff root mass.
   - Mechanics: physical collision, forced detour, no UI message.

6. **False affordance**
   - Looks passable from a distance but fails on closer inspection.
   - Cues: bright open patch beyond water, apparent low bank that is actually undercut, stones that disappear under glare, vine gap that closes into thorns.
   - Mechanics: player invests approach time, then must reassess.

7. **Route trap**
   - A corridor that permits entry but becomes costly or dead-ended.
   - Examples:
     - bank shelf narrows against cliff and deep water;
     - gully rises into dense wait-a-while;
     - creek leads to waterfall lip;
     - mudflat leads into tidal mangrove-like roots;
     - post-rain channel becomes uncrossable behind the player.
   - Use carefully: route traps should feel ecological, not designer-cheap.

**Bank shape rules**

- **Low shelf + damp leaf litter + light vegetation** → `SlowTravel` or `CrossingApproach`.
- **Sloped mud + saturated surface + no roots** → `CarefulFooting` or `SoftBlocker`.
- **Rooted toe + moderate slope + exposed buttress roots** → `SlowTravel`, but with trip/root snag risk.
- **Undercut bank + overhanging roots + dark hollow below** → `FalseAffordance` or `HardBlocker`.
- **Vertical cut bank + deep outside bend** → `HardBlocker`.
- **Boulder edge + wet rock sheen + cascade sound** → `CarefulFooting`, possibly `ForcedDetour`.
- **Swamp margin + standing water + dense palms/vines** → `RouteTrap` or `SoftBlocker`.

Riparian vegetation can stabilize banks by binding soil with roots and reducing erosion, but this does not mean every vegetated bank is safe or walkable. Queensland WetlandInfo notes that riparian roots bind banks and can allow undercut banks to persist, while Australian streambank guidance also warns that bank erosion and channel change can still occur even in vegetated systems. ([wetlandinfo.detsi.qld.gov.au](https://wetlandinfo.detsi.qld.gov.au/wetlands/ecology/components/biota/flora/flora-structural/riparian-vegetation.html?utm_source=chatgpt.com))

## Crossing Candidate Model

Crossings should be represented as **candidates with evidence and risk**, not as guaranteed interaction points.

Recommended struct name:

```text id="wukv33"
FCreekCrossingCandidate
```

Recommended fields:

```text id="q3rqk0"
CandidateId
CreekReachId
CatchmentId
WorldLocation
EntryBankSide
ExitBankSide
ApproachVector
ExitVector
FlowDirectionVector
ChannelWidthM
WaterDepthEstimateM
FlowSpeedClass: Still | Slow | Moderate | Fast | FloodPulse
BedSurface: SandMud | Gravel | Cobble | Boulder | Bedrock | RootMat | Debris
BankEntryShape: LowShelf | SlopedMud | RootStep | CutBank | BoulderStep | VegetationGap
BankExitShape: LowShelf | SlopedMud | RootStep | CutBank | BoulderStep | VegetationGap
VisibilityClass: Clear | PartiallyOccluded | Glare | FoamObscured | RainObscured | NightObscured
FootingRisk: Low | Medium | High | Unsafe
WaterRisk: Low | Medium | High | Unsafe
VegetationRisk: None | Snag | DenseExit | WaitAWhile | Deadfall
RecentRainState: DryBaseline | Wet | Rising | Flooded | Falling
AcousticMasking: None | Moderate | Heavy
PlayerReadableEvidence: array<RouteEvidenceTag>
ValidationStatus: Untested | AutoValid | NeedsDesignerReview | Rejected
DebugReason
```

Recommended enum:

```text id="w6ux3z"
ECreekCrossingType
- DryRockHop
- ShallowWade
- RootStep
- FallenLog
- GravelBar
- BoulderChoke
- NarrowPinch
- PoolEdgeSkirt
- UnsafeFlooded
- FalseCrossing
```

Recommended route evidence tags:

```text id="wypajz"
RouteEvidence_WaterDirection
RouteEvidence_ShallowRiffle
RouteEvidence_ExposedStone
RouteEvidence_LowOppositeBank
RouteEvidence_DryDebrisLine
RouteEvidence_AnimalTrackGap
RouteEvidence_RootStep
RouteEvidence_SiltShelf
RouteEvidence_FoamLineFastFlow
RouteEvidence_DeepPoolDarkWater
RouteEvidence_UndercutExitBank
RouteEvidence_RainSwollen
RouteEvidence_SlipperyRockSheen
RouteEvidence_DenseExitVegetation
RouteEvidence_DebrisJamBlock
RouteEvidence_SoundMasksDepth
```

**Crossing scoring concept**

Do not expose a crossing score to the player. Use it only for generation, validation, and debug.

```text id="5qh4ec"
CrossingSuitability =
  + low width
  + low depth
  + slow flow
  + stable bed
  + low entry bank
  + low exit bank
  + visible exit
  + low vegetation friction
  - recent rain rise
  - deep pool
  - foam / turbulence
  - undercut bank
  - slippery rock
  - deadfall jam
  - acoustic masking
```

Suggested internal categories:

| Internal category | Meaning | Player-facing expression |
|---|---|---|
| `ValidPrimaryCrossing` | Safe-ish intended crossing | Clear shallow riffle, low banks, visible opposite exit |
| `ValidRiskyCrossing` | Possible but punishing | Wet rocks, moderate flow, awkward exit |
| `FalseCrossing` | Appears usable, fails on approach | Opposite bank undercut, hidden deep pool, dense exit |
| `BlockedCrossing` | Natural hard no | flood, waterfall, deep channel, vertical bank |
| `ChangedCrossing` | State-dependent | safe before rain, unsafe after rain |
| `RelictModifiedCrossing` | Plausibly altered | moved branch, placed stone, disturbed mud, blocked gap |

**First-person crossing cues**

- **Water direction**
  - Floating leaves, foam drift, ripples wrapping around stones, angled grass, wet streaks on rock.
  - Sound should be louder downstream of cascades and at confluences, but local terrain can mislead.

- **Depth**
  - Darker pools, smoother surface, lack of visible bed, submerged roots, abrupt color transition.
  - In tropical forest, tannin-stained or sediment-laden water should reduce depth certainty.

- **Flow risk**
  - Foam lines, turbulent riffles, water curling around stones, debris movement, sound pitch/volume, rain intensity.
  - Floodwater should hide hazards and move debris. Queensland flood safety guidance warns against entering floodwaters because they can move quickly and hide debris, currents, and displaced animals. ([detsi.qld.gov.au](https://www.detsi.qld.gov.au/our-department/news-media/down-to-earth/queensland-flooding-safety-advice?utm_source=chatgpt.com))

- **Footing**
  - Glossy wet rocks, algae-like film, unstable loose stones, root tangles, mud deformation, audible suction.
  - Use animation/audio discomfort instead of UI warnings.

- **Exit quality**
  - The opposite bank matters as much as the near bank.
  - Good crossing: low opposite shelf, visible gap, stable root/stone step.
  - Bad crossing: vertical bank, dense vegetation, undercut hollow, deadfall pile.

**Implementation rule**

A crossing candidate is valid only if all three parts are plausible:

```text id="nzx94p"
Approach plausible + Channel traversal plausible + Exit plausible
```

A beautiful shallow riffle with an impossible exit is not a valid crossing. It is a `FalseCrossing` or `RouteTrap`.

## Water-Sound And Acoustic Evidence

Water sound should be one of the strongest non-UI orientation systems in Jungle Game. The player should learn to interpret sound gradients, not receive explicit direction.

Unreal supports distance-based attenuation for sound sources, Ambient Sound Actors, Audio Volumes, Ambient Zones, Sound Classes, and low-pass/filter behavior through audio volumes and attenuation settings. ([dev.epicgames.com](https://dev.epicgames.com/documentation/unreal-engine/ambient-sound-actor-user-guide-in-unreal-engine?utm_source=chatgpt.com))

Recommended sound anchor categories:

```text id="nmbmgr"
SA_Creek_Trickle
SA_Creek_Riffle
SA_Creek_Run
SA_Creek_Rapid
SA_Creek_Cascade
SA_Creek_Waterfall
SA_Creek_DeepPoolLap
SA_Creek_DebrisJam
SA_Creek_Confluence
SA_Creek_MouthSurfMix
SA_Riparian_FrogBand
SA_Riparian_InsectWetEdge
SA_Rain_CanopyDrip
SA_Rain_LeafImpact
SA_Valley_MuffledDistantWater
SA_Valley_ReverbWetGully
SA_Flood_RisingChannel
```

**Acoustic orientation principles**

1. **Gradient, not marker**
   - Water sound should increase, decrease, broaden, or sharpen as the player moves.
   - Do not add an audio ping, objective tone, or compass-like stereo cue.

2. **Source scale**
   - Trickle: close, delicate, high-frequency detail, easily masked.
   - Riffle/run: mid-distance texture, useful for finding channels.
   - Cascade/waterfall: broad, loud, low-to-mid roar, can dominate a valley and hide other cues.
   - Confluence: layered sounds from two directions, often confusing but informative.
   - Creek mouth: water plus surf/tidal wash, lower terrain, wider acoustic field.

3. **Dense forest attenuation**
   - Vegetation, trunks, leaf litter, ground softness, humidity, terrain, and ambient sound all affect propagation. Forest acoustics literature describes attenuation through foliage, trunks, bark, ground effects, scattering, and meteorological variables; passive acoustic studies also emphasize that detection distance varies with frequency, habitat attenuation, and ambient sound level. ([oro.open.ac.uk](https://oro.open.ac.uk/47314/1/IOA2016-Attenborough.pdf?utm_source=chatgpt.com))

4. **Valley acoustics**
   - Narrow wet gullies should create muffled, occluded, reverberant water.
   - Broad valley floors should create wider, less directional water beds.
   - Waterfalls should be audible through vegetation but directionally ambiguous near cliffs.

5. **Rain interaction**
   - Light rain adds leaf impacts and drip points, making water sources harder to isolate.
   - Heavy rain should flatten acoustic contrast: everything becomes wet, loud, and close.
   - Post-rain should increase creek volume, deepen pools, add debris movement, and activate ephemeral gullies.

6. **Biophony as wetness evidence**
   - Frogs and insects can imply wet zones, pools, seepage, night conditions, or recent rain.
   - Avoid species-specific simulation unless later needed. Use bands: `FrogBand_Pool`, `FrogBand_Seep`, `InsectBand_WetEdge`, `InsectBand_DryRidge`.
   - Frog/insect density may guide the player toward wetness, but not necessarily toward safe water or safe crossings.

**Sound masking states**

Recommended enum:

```text id="qe5njl"
EAcousticMaskState
- ClearDirectional
- VegetationMuffled
- ValleyReverb
- RainMasked
- WaterfallDominant
- ConfluenceConfused
- NightBiophonyDense
```

**Audio implementation notes**

- Place sound anchors along creek reach nodes rather than one continuous sound everywhere.
- Use reach class to choose sound category:
  - `HeadwaterSeep` → `SA_Creek_Trickle`, `SA_Riparian_FrogBand`
  - `NarrowCreek` → `SA_Creek_Riffle`, `SA_Creek_Run`
  - `Cascade` → `SA_Creek_Cascade`, `SA_Valley_ReverbWetGully`
  - `Waterfall` → `SA_Creek_Waterfall`, local mist/drip layer
  - `Confluence` → two or more directional water beds plus merged downstream anchor
  - `CreekMouth` → `SA_Creek_MouthSurfMix`
- Use Audio Volumes in wet gullies and behind dense riparian vegetation to apply muffling or reverb-like transitions.
- Use sound attenuation shapes that match valley geometry where practical.
- Do not overdo stereo precision. In dense forest, audio should be useful but sometimes deceptive.

## Tropical Riparian Vegetation And Friction

Riparian zones should be among the densest and most physically costly parts of the jungle. They are attractive because they provide water, sound, cooler air, animal signs, and navigable drainage structure. They are dangerous because they concentrate mud, insects, leeches, unstable banks, dense vegetation, deadfall, and flood energy.

Recommended biome outputs:

```text id="1uz0d9"
Biome_CreekChannel
Biome_WettedBed
Biome_RockCascade
Biome_DeepPool
Biome_RiparianLowerBank
Biome_RiparianUpperBank
Biome_WetValleyFloor
Biome_SeepageSlope
Biome_DensePalmGully
Biome_VineTangle
Biome_FloodScourBench
Biome_DebrisJam
Biome_CreekMouthSwamp
Biome_TidalMudEdge
```

Recommended vegetation/friction layers:

```text id="p1g4bq"
Veg_Riparian_FernDense
Veg_Riparian_PalmJuvenile
Veg_Riparian_PandanusEdge
Veg_Riparian_WaitAWhile
Veg_Riparian_RootMat
Veg_Riparian_ButtressRoot
Veg_Riparian_VineCurtain
Veg_Riparian_Deadfall
Veg_Riparian_CanopyDrip
Veg_Riparian_OpenScourGap
```

Wait-a-while / lawyer cane analogues are especially useful because they are tropical, visually legible, and traversal-relevant. Queensland wetland profiles list lawyer cane / wait-a-while among shrubs, vines, epiphytes, and herbs in coastal palm swamp communities, and botanical references describe Calamus species as climbing palms with hooked tendrils or spines. ([wetlandinfo.detsi.qld.gov.au](https://wetlandinfo.detsi.qld.gov.au/resources/static/pdf/resources/fact-sheets/profiles/new-profiles/29113-04-palm-swamps-web.pdf?utm_source=chatgpt.com))

**Vegetation roles**

| Vegetation form | Gameplay role | Implementation output |
|---|---|---|
| Ferns / wet herbs | Slow movement, wetness cue | `VegFriction_LightWet` |
| Palm juveniles | Visual clutter, soft blocker | `VegFriction_DensePalm` |
| Pandanus-like edge | Bank marker, sharp silhouette | `VegFriction_EdgeDense` |
| Wait-a-while analogue | Snag hazard, route trap | `VegFriction_SnagHigh` |
| Buttress roots | footing cue, cover, root shelf | `Footing_RootTangle` |
| Vine curtain | false opening, occlusion | `Visibility_PartialOcclusion` |
| Deadfall | blocker/crossing object | `Obstacle_DebrisJam` |
| Scour gap | short open lane | `Traversal_OpenTravel` |

**Placeholder nuisance/risk concepts**

Do not build a full health or disease system now. Use lightweight status tags, animation/audio, and local traversal penalties.

Recommended placeholder risk tags:

```text id="u04tbi"
Risk_LeechExposure
Risk_TickExposure
Risk_MudSuction
Risk_SlipperyRock
Risk_RootTrip
Risk_WaitAWhileSnag
Risk_BankCollapse
Risk_DeadfallBlock
Risk_FloodPulse
Risk_RainSwollenCrossing
Risk_AudioMasking
Risk_LowVisibility
```

Suggested current implementation effects:

| Risk | Now | Deferred |
|---|---|---|
| Leeches | local nuisance tag, inspect/remove animation later, wet-zone exposure counter | disease, blood loss, medical system |
| Ticks | exposure tag in dense vegetation | disease system |
| Mud | movement slowdown, suction sound, footprint/deformation decal | stamina/boots system |
| Slippery rocks | stumble chance, careful footing state | injury system |
| Deadfall | physical blocker, climbing/crouch detour marker | dynamic treefall simulation |
| Wait-a-while | snag audio, movement interruption, camera tug | complex cloth/plant interaction |
| Bank collapse | local crumble decal/audio, forced retreat, unstable edge flag | full terrain deformation |
| Flooding | crossing state changes, sound/visual intensity | physically simulated flood hydraulics |

Queensland park guidance notes that leeches are common in rainforest, particularly after rain, and Wet Tropics hazard material also treats leeches as a real wet-forest nuisance. ([parks.qld.gov.au](https://parks.qld.gov.au/parks/lamington/about/faq?utm_source=chatgpt.com))

## Weather And Night Interaction

Weather and night should alter creek evidence without becoming a UI timer or explicit state label.

**Rain states**

Recommended enum:

```text id="lrhveh"
ERiparianRainState
- DryBaseline
- LightRain
- HeavyRain
- StormBurst
- PostRainRising
- PostRainFalling
```

**Rain effects**

- `DryBaseline`
  - Some headwater seeps may be faint or silent.
  - Crossings are most available.
  - Mud still exists in shaded wet valleys.

- `LightRain`
  - Leaf impacts and canopy drips appear.
  - Frogs/insects increase around wet edges.
  - Rock sheen becomes more common.

- `HeavyRain`
  - Audio masking increases.
  - Small gullies activate.
  - Shallow crossings become less readable.
  - Flood debris may move.

- `StormBurst`
  - Crossing candidates can flip to `UnsafeFlooded`.
  - Creek sound becomes broad and less directional.
  - Bank collapse risk increases on saturated cut banks.
  - Player should be encouraged by environment to wait, detour, or seek higher ground, without UI warnings.

- `PostRainRising`
  - Water may continue rising after rain due to catchment response.
  - This is especially important for tropical steep catchments where runoff can concentrate rapidly.
  - Use stronger flow audio, floating debris, and new brown/turbid water.

- `PostRainFalling`
  - Debris lines, wet marks, foam residue, and fresh silt reveal recent water height.
  - Crossings may re-open but remain slippery.

**Night effects**

Night should strengthen sound and reduce visual crossing certainty:

- water direction more audible but less visually confirmed;
- frogs/insects become stronger wetness cues;
- shiny wet surfaces can mislead under moonlight or weak light;
- deep pools become unreadable dark patches;
- valley acoustics can create false source direction;
- relict hominin environmental changes become more unsettling because the player cannot verify them at range.

Recommended night evidence tags:

```text id="e2v23o"
RouteEvidence_NightWaterSound
RouteEvidence_FrogWetnessBand
RouteEvidence_InsectEdgeBand
RouteEvidence_MoonlitWetRock
RouteEvidence_DarkPoolUncertain
RouteEvidence_RainMuffledDirection
```

Forbidden drift:

- no HUD clock;
- no “it is too late to cross” message;
- no character comment saying “I’m going the right way”;
- no explicit orientation confidence indicator.

## UE5 Implementation Guidance

Use UE5 systems pragmatically. The goal is **believable creek evidence**, not final hydrology.

Epic’s Water system supports spline-based rivers, lakes, and oceans interacting with Landscape terrain; Water Body actors and Water Zones define water surfaces and rendering regions. Landscape Edit Layers support non-destructive heightmap and weightmap workflows, and PCG supports procedural content generation for biomes and world-building tools. ([dev.epicgames.com](https://dev.epicgames.com/documentation/unreal-engine/water-system-in-unreal-engine?utm_source=chatgpt.com))

**Recommended implementation layers**

1. **Terrain drainage metadata**
   - Generate or author catchments, flow lines, confluences, and outlet nodes.
   - Do not require runtime hydrology.
   - Store creek reach data in data assets or world partition-compatible actors.

2. **Creek spline / reach actors**
   - Use splines for channel alignment and water body placement where useful.
   - Separate visual water from gameplay metadata.
   - A dry gully may have no water mesh but still has creek metadata.

3. **Riparian masks**
   - Use landscape weightmaps, RVT, PCG attributes, or generated spatial data.
   - Runtime Virtual Texturing is useful for large-area landscape shading and decal-like/spline-conforming material effects. ([dev.epicgames.com](https://dev.epicgames.com/documentation/unreal-engine/runtime-virtual-texturing-in-unreal-engine?utm_source=chatgpt.com))

4. **Physical materials**
   - Assign physical materials/surface types for wet mud, wet rock, dry leaf litter, root mat, shallow water, and deep water edges.
   - Unreal Physical Materials can be used to identify surface types for gameplay/audio responses such as footsteps. ([dev.epicgames.com](https://dev.epicgames.com/documentation/unreal-engine/artist-11-add-footstep-sounds-to-a-character?utm_source=chatgpt.com))

5. **PCG biome spawning**
   - Spawn riparian vegetation based on masks:
     - high wetness + low slope → ferns/palms/mud;
     - bank toe + roots → exposed roots;
     - dense edge + route trap → wait-a-while analogue;
     - flood scour bench → sparse open ground;
     - debris jam → deadfall/branch piles.
   - PCG data should carry attributes such as `Wetness`, `Slope`, `BankZone`, `FlowOrder`, `FloodDisturbance`, and `TraversalFriction`.

6. **Crossing candidates**
   - Auto-generate candidates at narrow, shallow, low-bank reach segments.
   - Then reject candidates with bad exit banks, high vegetation friction, deep pools, or flood state.
   - Mark uncertain cases `NeedsDesignerReview`.

7. **Sound anchors**
   - Place anchors based on reach class and slope.
   - Add special anchors at confluences, waterfalls, cascades, debris jams, creek mouths, and wet valley basins.
   - Use Audio Volumes / Ambient Zones for wet gullies, waterfall mist zones, and dense vegetation muffling.

8. **Weather state hooks**
   - Change crossing state, sound mix, wetness masks, mud/slip risk, and ephemeral gully activation based on rain state.
   - Do not simulate full water volume yet.

**Suggested data assets / structs**

```text id="4x1xua"
UCreekReachDataAsset
FCreekReachRuntimeState
FCreekCrossingCandidate
FRiparianBiomeOutput
FWaterSoundAnchor
FRouteEvidencePoint
FWetValleyRiskZone
```

**Suggested enums**

```text id="ry4j8g"
ECreekReachClass
- HeadwaterSeep
- EphemeralGully
- NarrowCreek
- WetValley
- Cascade
- Waterfall
- LowlandCreek
- CreekMouth
- TidalReach

ECreekBankAffordance
- OpenTravel
- SlowTravel
- CarefulFooting
- SoftBlocker
- HardBlocker
- FalseAffordance
- RouteTrap
- CrossingApproach
- ForcedDetour

ECrossingValidity
- ValidPrimary
- ValidRisky
- FalseCrossing
- Blocked
- WeatherChanged
- NeedsReview

EWaterSoundAnchorType
- Trickle
- Riffle
- Run
- Rapid
- Cascade
- Waterfall
- DeepPool
- DebrisJam
- Confluence
- CreekMouth
- FrogWetBand
- InsectWetBand
- RainDrip
- ValleyMuffle
```

**Implement now**

- Creek reach metadata.
- Catchment IDs and downstream links.
- Riparian/wetness/bank masks.
- Crossing candidate struct and debug visualization.
- Basic bank affordance classification.
- Placeholder risk tags.
- Sound anchor categories and placement rules.
- Rain state affecting crossing validity and audio mix.
- Developer-only evidence views.

**Defer**

- Full hydrodynamic simulation.
- Runtime erosion and terrain deformation.
- Full disease, wound, parasite, or medical system.
- Full wildlife AI.
- Complex plant physics for every vine.
- Perfect water depth physics.
- Exact protected-place replication.
- Any player-facing navigation UI.

## Validation Points And Evidence Plan

Developer-only validation should prove that creek logic creates believable, usable, imperfect movement evidence.

Recommended validation point types:

```text id="7qhz2h"
VP_CreekReachContinuity
VP_FlowDirectionReadable
VP_CatchmentOutletResolved
VP_BankAffordanceBalanced
VP_CrossingCandidateValid
VP_CrossingExitPlausible
VP_FalseCrossingIntentional
VP_RouteTrapHasEscape
VP_SoundAnchorAudible
VP_SoundMaskingPlausible
VP_RainStateChangesCrossing
VP_RiparianFrictionTropical
VP_DebugEvidenceComplete
```

Recommended debug view names:

```text id="7uc7pc"
DBG_Creek_FlowDirection
DBG_Creek_CatchmentId
DBG_Creek_ReachClass
DBG_Creek_StreamOrder
DBG_Creek_BankAffordance
DBG_Creek_Wetness
DBG_Creek_RiparianMask
DBG_Creek_CrossingCandidates
DBG_Creek_CrossingValidity
DBG_Creek_RouteEvidence
DBG_Creek_RouteTraps
DBG_Creek_RiskZones
DBG_Creek_SoundAnchors
DBG_Creek_AcousticMasking
DBG_Creek_RainResponse
DBG_Creek_BlockerReasons
DBG_Creek_PlayerTraversalHeat
```

**Evidence requirements**

- Every creek reach should have a downstream direction.
- Every confluence should merge into a larger or equal-order downstream reach.
- Every crossing candidate should show:
  - approach bank;
  - channel segment;
  - exit bank;
  - validity;
  - rejection reason if invalid.
- Every hard blocker should have a physical reason:
  - slope;
  - bank height;
  - water depth;
  - vegetation density;
  - deadfall;
  - waterfall;
  - flood state.
- Every route trap should have:
  - entry affordance;
  - trap cause;
  - minimum escape/detour route unless deliberately unreachable for world-boundary reasons.
- Sound anchors should visualize attenuation radius, occlusion/muffle zones, and reach association.
- Rain state debug should show which crossings changed and why.

**Validation walk tests**

1. **Downstream reasoning test**
   - Start player in mid-slope wet valley.
   - Player can infer downstream from water movement, valley slope, sound, and debris.
   - No UI direction.

2. **Tributary confusion test**
   - At a confluence, both branches are plausible.
   - One branch narrows into headwater gully; the other continues to larger creek.
   - Evidence is present but not explicit.

3. **False crossing test**
   - A visible shallow section appears crossable.
   - Close evidence reveals undercut exit bank or dense exit vegetation.
   - Player can identify failure without a warning prompt.

4. **Rain-swollen crossing test**
   - A previously valid crossing becomes unsafe after heavy rain.
   - Evidence: louder water, foam, debris, brown/turbid water, submerged stones, stronger current.

5. **Sound landmark test**
   - A waterfall is audible before visible.
   - Player can use it as a landmark but may be misled by valley reflection/occlusion.

6. **Wet valley friction test**
   - Moving directly along the creek is sometimes slower than contouring slightly above the bank.
   - This prevents “always follow the waterline” behavior.

## Risks, Pitfalls, And Forbidden Drift

**Major design risks**

1. **Creeks become roads**
   - If banks are too open and continuous, players will treat creeks as roads.
   - Mitigation: alternate open banks, blocked banks, steep outside bends, dense vegetation, deadfall, and detours.

2. **Creeks become useless**
   - If every bank is blocked, players stop reading hydrology.
   - Mitigation: provide intermittent bank shelves, crossing clues, confluences, sound gradients, and visible drainage logic.

3. **Crossings become UI objects**
   - If every crossing has the same visual pattern, it becomes an objective marker.
   - Mitigation: vary crossing types and evidence combinations.

4. **Audio becomes compass**
   - Overly precise stereo water direction can function like a bearing.
   - Mitigation: use occlusion, vegetation muffling, valley reflection, rainfall masking, and multiple water sources.

5. **Tropical setting drifts temperate**
   - Open banks, clear gravel streams, dry meadows, and easy stepping stones feel wrong if overused.
   - Mitigation: emphasize wet roots, mud, dense vegetation, shade, insects, leeches, palms, vines, sudden rain, and poor visibility.

6. **Rain state becomes unfair**
   - Changing crossings after rain can feel punitive if evidence is weak.
   - Mitigation: show strong sensory changes: sound, water color, foam, debris, submerged stones, wet bank marks.

7. **False affordances become cheap tricks**
   - A false route must be physically inspectable and learnable.
   - Mitigation: expose close-range evidence before hard failure.

8. **Relict hominin changes become magic**
   - Environmental changes must remain plausible: moved deadfall, blocked gap, disturbed mud, stones shifted, sound occluded by placed vegetation.
   - Avoid supernatural water control or impossible terrain changes.

**Forbidden drift**

Do not add:

- player-facing map;
- minimap;
- compass;
- bearing number;
- GPS;
- objective marker;
- HUD clock;
- explicit time readout;
- orientation confidence UI;
- hidden orientation meter;
- character comments that confirm orientation;
- modern survival navigation tools;
- combat, gore, chase, monster framing, or fantasy magic;
- final hydrology simulation as a PR requirement;
- full wildlife AI;
- full disease system;
- exact protected-place replication.

## PR-Ready Recommendations

**Recommended PR scope**

Implement a creek/riparian metadata and debug foundation, not final water simulation.

**Add data definitions**

- `UCreekReachDataAsset`
- `FCreekReachRuntimeState`
- `FCreekCrossingCandidate`
- `FRiparianBiomeOutput`
- `FWaterSoundAnchor`
- `FRouteEvidencePoint`
- `FWetValleyRiskZone`

**Add enums**

- `ECreekReachClass`
- `ECreekBankAffordance`
- `ECrossingValidity`
- `ECreekCrossingType`
- `EWaterSoundAnchorType`
- `EAcousticMaskState`
- `ERiparianRainState`

**Add masks**

Minimum mask set:

```text id="xha1k3"
M_Creek_ChannelCore
M_Creek_WettedBed
M_Creek_BankToe
M_Creek_UpperBankBench
M_Creek_RiparianDense
M_Creek_WetValleyFloor
M_Creek_MudPatch
M_Creek_DebrisJam
M_Creek_CrossingCandidate
M_Creek_HardBlocker
M_Creek_RouteTrap
```

**Add crossing candidate fields**

Minimum fields:

```text id="4wu7mc"
CandidateId
CreekReachId
WorldLocation
EntryBankSide
FlowDirectionVector
ChannelWidthM
WaterDepthEstimateM
FlowSpeedClass
BedSurface
BankEntryShape
BankExitShape
VisibilityClass
FootingRisk
WaterRisk
VegetationRisk
RecentRainState
PlayerReadableEvidence
ValidationStatus
DebugReason
```

**Add route evidence names**

Minimum set:

```text id="0ntd41"
RouteEvidence_WaterDirection
RouteEvidence_ShallowRiffle
RouteEvidence_ExposedStone
RouteEvidence_LowOppositeBank
RouteEvidence_DeepPoolDarkWater
RouteEvidence_UndercutExitBank
RouteEvidence_RainSwollen
RouteEvidence_SlipperyRockSheen
RouteEvidence_DenseExitVegetation
RouteEvidence_DebrisJamBlock
```

**Add sound anchor categories**

Minimum set:

```text id="c0tvgs"
SA_Creek_Trickle
SA_Creek_Riffle
SA_Creek_Run
SA_Creek_Cascade
SA_Creek_Waterfall
SA_Creek_Confluence
SA_Riparian_FrogBand
SA_Riparian_InsectWetEdge
SA_Rain_CanopyDrip
SA_Valley_MuffledDistantWater
```

**Add debug views**

Minimum set:

```text id="bq8fue"
DBG_Creek_FlowDirection
DBG_Creek_ReachClass
DBG_Creek_BankAffordance
DBG_Creek_Wetness
DBG_Creek_CrossingCandidates
DBG_Creek_CrossingValidity
DBG_Creek_RouteEvidence
DBG_Creek_RouteTraps
DBG_Creek_SoundAnchors
DBG_Creek_RainResponse
```

**Acceptance criteria**

- Creek reaches have catchment ID, reach class, flow direction, and downstream link.
- Banks classify into affordance states using slope, wetness, vegetation, and bank shape.
- Crossing candidates include approach, channel, and exit fields.
- Invalid crossings expose developer-only rejection reasons.
- Rain state can change crossing validity.
- Sound anchors exist as data categories tied to creek reach classes.
- Debug views can prove all major creek traversal decisions.
- No player-facing navigation UI is introduced.

## Sources

- U.S. Geological Survey — “Watersheds and Drainage Basins”
  https://www.usgs.gov/water-science-school/science/watersheds-and-drainage-basins

- U.S. Geological Survey — “Rivers, Streams, and Creeks”
  https://www.usgs.gov/water-science-school/science/rivers-streams-and-creeks

- U.S. Geological Survey — “Watershed Boundary Dataset”
  https://www.usgs.gov/national-hydrography/watershed-boundary-dataset

- Australian Government Department of Climate Change, Energy, the Environment and Water — “Wet Tropics of Queensland”
  https://www.dcceew.gov.au/parks-heritage/heritage/places/world/wet-tropics

- UNESCO World Heritage Centre — “Wet Tropics of Queensland”
  https://whc.unesco.org/en/list/486/

- Queensland WetlandInfo — “Riparian vegetation”
  https://wetlandinfo.detsi.qld.gov.au/wetlands/ecology/components/biota/flora/flora-structural/riparian-vegetation.html

- Cooperative Research Centre for Catchment Hydrology / eWater archive — Abernethy and Rutherfurd, “Guidelines for Stabilising Streambanks with Riparian Vegetation”
  https://www.ewater.org.au/archive/crcch/archive/pubs/pdfs/technical199910.pdf

- Queensland Government / Great Barrier Reef catchment science — Bartley et al., “Stream bank management in the Great Barrier Reef catchments”
  https://apps.information.qld.gov.au/StoryMaps/GBR_PaddockCatchmentScience/assets/Bartleyetal_2015_GBR_streambank.pdf

- Queensland WetlandInfo — “Wetland Management Profile: Coastal Palm Swamps”
  https://wetlandinfo.detsi.qld.gov.au/resources/static/pdf/resources/fact-sheets/profiles/new-profiles/29113-04-palm-swamps-web.pdf

- Lucid Central Rainforest Key — “Calamus moti”
  https://apps.lucidcentral.org/rainforest/text/entities/calamus_moti.htm

- Wet Tropics Management Authority — “Hazards in the Wet Tropics”
  https://www.wettropics.gov.au/rainforest_explorer/Resources/Documents/TropicalTopics/HazardsintheWetTropics.pdf

- Queensland Parks and Wildlife Service — Lamington National Park FAQ, leeches guidance
  https://parks.qld.gov.au/parks/lamington/about/faq

- Queensland Government — “If it’s flooded, forget it”
  https://www.getready.qld.gov.au/FloodedForgetIt

- Queensland Government — “Safety tips for Queensland flooding: what you need to know”
  https://www.detsi.qld.gov.au/our-department/news-media/down-to-earth/queensland-flooding-safety-advice

- FAO — “Forests and Water”
  https://www.fao.org/4/i0410e/i0410e.pdf

- Attenborough et al. — “Sound Propagation Through Forests and Tree Belts”
  https://oro.open.ac.uk/47314/1/IOA2016-Attenborough.pdf

- Haupert, Sèbe, and Sueur — “Physics-based model to predict the acoustic detection distance of terrestrial autonomous recording units over the diel cycle and across seasons”
  https://arxiv.org/abs/2211.16077

- Epic Games Documentation — “Water System in Unreal Engine”
  https://dev.epicgames.com/documentation/unreal-engine/water-system-in-unreal-engine

- Epic Games Documentation — “Water Body Actors in Unreal Engine”
  https://dev.epicgames.com/documentation/unreal-engine/water-body-actors-in-unreal-engine

- Epic Games Documentation — “Water Meshing System and Surface Rendering”
  https://dev.epicgames.com/documentation/unreal-engine/water-meshing-system-and-surface-rendering-in-unreal-engine

- Epic Games Documentation — “Landscape Edit Layers in Unreal Engine”
  https://dev.epicgames.com/documentation/unreal-engine/landscape-edit-layers-in-unreal-engine

- Epic Games Documentation — “Runtime Virtual Texturing”
  https://dev.epicgames.com/documentation/unreal-engine/runtime-virtual-texturing-in-unreal-engine

- Epic Games Documentation — “Procedural Content Generation Overview”
  https://dev.epicgames.com/documentation/unreal-engine/procedural-content-generation-overview

- Epic Games Documentation — “Ambient Sound Actor User Guide”
  https://dev.epicgames.com/documentation/unreal-engine/ambient-sound-actor-user-guide-in-unreal-engine

- Epic Games Documentation — “Ambient Zones in Unreal Engine”
  https://dev.epicgames.com/documentation/unreal-engine/ambient-zones-in-unreal-engine

- Epic Games Documentation — “Audio Volumes in Unreal Engine”
  https://dev.epicgames.com/documentation/unreal-engine/audio-volumes-in-unreal-engine

- Epic Games Documentation — “Sound Attenuation in Unreal Engine”
  https://dev.epicgames.com/documentation/unreal-engine/sound-attenuation-in-unreal-engine

- Epic Games Documentation — “Add Footstep Sounds to a Character”
  https://dev.epicgames.com/documentation/unreal-engine/artist-11-add-footstep-sounds-to-a-character
