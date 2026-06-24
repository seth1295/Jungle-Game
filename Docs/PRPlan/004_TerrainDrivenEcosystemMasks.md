# Terrain-Driven Ecosystem Masks

## Purpose

Ecosystem classification should come from layered masks. Each mask represents terrain, hydrology, substrate, canopy, disturbance, visibility, or danger information that PCG and gameplay systems can consume.

## Required Masks

### Height

Height separates lowland, basin, slope, upland, mountain shoulder, and high-ridge contexts.

Use:

- macro ecosystem classification;
- temperature/fog/cloud exposure proxy;
- landmark silhouette planning;
- movement-cost scaling.

### Slope

Slope controls traversal difficulty, erosion exposure, soil depth proxy, drainage, and vegetation structure.

Use:

- steep slope forest;
- cliff/gorge exclusion;
- landslip or exposed root microhabitat;
- player fatigue/risk modifiers.

### Aspect

Aspect is slope direction. It can act as a sunlight, drying, wind, and exposure proxy.

Use:

- wetter sheltered slopes;
- drier exposed slopes;
- moss/fern density;
- sun-navigation readability.

### Creek Distance

Distance from mapped or generated creeks controls riparian influence.

Use:

- creek bank vegetation;
- water sound;
- crossing probability;
- humidity/wetness;
- night risk and creek-following cues.

### Flow Accumulation / Wetness

Flow accumulation and wetness estimate where water collects, even away from visible creek lines. Wetness can be approximated from upslope contributing area and slope, similar to topographic wetness index concepts that combine contributing area and slope to estimate topographic control on hydrological processes ([Sørensen, Zinko, and Seibert 2006, Hydrology and Earth System Sciences](https://hess.copernicus.org/articles/10/101/2006/)).

Use:

- seeps;
- boggy patches;
- fern/gully intensity;
- leech/insect/audio zones;
- wet valley classification.

### Ridge / Valley Score

Ridge/valley score separates convex high lines from concave drainage lines.

Use:

- ridge navigation affordances;
- mountain silhouettes;
- valley enclosure;
- gorge corridors;
- high-ground visibility checks.

### Soil / Geology / Land-Zone Proxy

Queensland regional ecosystems are associated with geology, landform, and soil, and Queensland Herbarium mapping uses geology and soil mapping as source inputs ([Queensland regional ecosystems](https://www.qld.gov.au/environment/plants-animals/plants/ecosystems/about), [Queensland Herbarium mapping](https://www.qld.gov.au/environment/plants-animals/plants/herbarium/mapping-ecosystems)).

The game does not need real geology data for the fictional world, but it needs a substrate proxy.

Use:

- volcanic-soil inspired patches;
- granitic ridge proxy;
- alluvial valley proxy;
- shallow rocky slope proxy;
- ecosystem variation that is not explained by height alone.

### Canopy Density

Canopy density controls light, visibility, rainfall interception, sound dampening, and player orientation.

Use:

- dense rainforest;
- canopy gaps;
- creek-edge openings;
- ridge visibility;
- night readability.

### Disturbance / Regrowth

Disturbance and regrowth represent storm damage, treefall, landslip, flood disturbance, old camp impacts, animal/hominin alteration, or natural gaps.

Use:

- movement openings;
- unusual clue placement;
- regrowth thickets;
- landmark exceptions;
- visual variety without arbitrary biome painting.

### Visibility Density

Visibility density is gameplay-facing occlusion and sightline information. It may be derived from canopy, understory, slope, and PCG clutter density.

Use:

- navigation difficulty;
- lost-state escalation;
- watcher cue concealment;
- line-of-sight validation for landmarks;
- performance budgets.

### Night Danger

Night danger is a gameplay mask derived from terrain and ecology.

Inputs:

- distance from camp;
- creek proximity;
- slope;
- wetness;
- canopy density;
- visibility density;
- known landmarks;
- sky visibility;
- star/moon visibility;
- sun/shadow cue visibility before nightfall;
- weather state;
- time of night.

Use:

- danger audio;
- traversal risk;
- lost-state pressure;
- safe/unsafe camp-adjacent zones;
- loss or weakening of sun, shadow, star, moon, and skyline cues;
- first-playable night validation.

### Traversal Blocker / Affordance

Traversal masks classify how the terrain invites, resists, blocks, or misleads movement.

Inputs:

- slope;
- local relief;
- cliff/water obstacle tests;
- creek distance;
- wetness;
- ridge/valley score;
- canopy and understory density;
- deadfall/disturbance/regrowth;
- soil/mud proxy;
- animal corridor or old-disturbance proxy where available.

Classes:

- hard blockers: cliffs, extreme slopes, water obstacles, dense impassable vegetation;
- soft blockers: steep fatigue slopes, mud, deadfall, thick regrowth;
- affordance zones: ridges, spurs, saddles, open understory, creek edges, animal corridors;
- false affordances: gullies, dry creek beds, disturbed regrowth, misleading clearings.

Use:

- player movement resistance and risk;
- PCG density and obstruction placement;
- lost-state evaluation;
- first-person route-discovery validation;
- debug-only traversal studies.

## Classification Approach

Ecosystem classifications should be weighted combinations, not single-threshold switches.

Example:

```text
Riparian Wet Valley =
  high creek distance influence
  + high flow accumulation/wetness
  + low to mid height
  + concave valley score
  + high canopy density
  + alluvial or wet substrate proxy
```

```text
Ridge Forest =
  high ridge score
  + mid/high height
  + moderate to steep slope shoulder
  + lower wetness
  + aspect exposure modifier
  + broken canopy or wind-exposure modifier
```

```text
Gorge Forest =
  high slope
  + strong valley score
  + creek proximity
  + shadow/shelter aspect
  + high wetness
  + constrained visibility
```

```text
Traversal Affordance =
  ridge/spur/saddle score
  + moderate slope
  + low obstruction density
  + readable landmark or creek relationship
  - cliff/water hard blockers
  - thick regrowth soft blockers
```

```text
False Affordance =
  apparent openness or drainage-line continuity
  + high gully/dry-creek score
  + rising wetness or enclosure
  + poor landmark visibility
  + increasing soft blockers
```

## Classification Rules

- Every primary ecosystem must have a terrain/hydrology reason.
- Classification must support overlapping influence, especially near creeks, slopes, and ridges.
- Traversal classification must be terrain-first; do not encode authored route lines as the primary input.
- Debug output must show both raw masks and final classification.
- PCG must consume masks through stable names, not ad hoc per-graph guesses.
- Player-facing ecology must be readable in first person.

## Validation Expectations

PR #10 and PR #11 should validate:

- each required mask exists or has a documented placeholder;
- raw mask ranges are inspectable;
- classifications explain themselves from input masks;
- ecosystem boundaries cross component borders naturally;
- traversal hard blockers, soft blockers, affordance zones, and false affordances are inspectable;
- debug views do not require player-facing map/compass UI;
- no final gameplay balance is claimed from masks alone.
