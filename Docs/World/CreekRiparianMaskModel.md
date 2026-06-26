# Creek Riparian Mask Model

Created: 2026-06-25

## Purpose

Creeks are primary world-navigation and survival-pressure structures. The creek model must create readable terrain evidence without becoming an explicit route marker.

## Inputs

- flow accumulation;
- creek distance;
- wetness;
- slope;
- valley influence;
- canopy closure;
- obstacle density;
- bank height;
- crossing width proxy.

## Output Bands

### Channel Core

Water path or wet drainage center. Usually hard to cross unless the local crossing band says otherwise.

### Wet Bank

Muddy, cluttered, slippery, or dense edge zone. Often soft friction.

### Bank Affordance

A usable edge, shelf, dry side, or animal-like movement line near water. Useful but not always safe.

### Crossing Candidate

A local place where slope, width, bank height, and clutter imply possible crossing.

### Riparian Density

Vegetation band influenced by water. Can create sound cues, visual contrast, soft friction, and concealment.

### Water Sound Anchor

Developer/runtime marker for where water noise can support orientation. Must stay diegetic.

## Rules

Creek routes should vary. A creek bank may help, slow, block, or mislead depending on local terrain.

Crossings must be inferred from world evidence, not shown as objective markers.

## Review

Review points should include creek head, small channel, wet bank, usable bank, crossing candidate, difficult crossing, and gorge-compressed creek.
