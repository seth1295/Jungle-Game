# PR 024 — World-First Terrain/Biome Batch Reset

## Goal

Reset the active implementation lane back to world-first terrain, biome, traversal, and environmental gameplay systems. This PR-slice exists to prevent drift into Cell 0-only polish while the large-world foundation still needs terrain and biome depth.

## Direction

Jungle Game should treat the island world as the game surface. The player should eventually navigate by landform, water, canopy, sound, weather, and biome resistance rather than interface rescue.

Current active implementation priority:

1. macro island terrain;
2. terrain-derived masks;
3. biome and traversal-resistance systems;
4. developer-only debug validation;
5. packaged/visual evidence;
6. gameplay coupling: orientation, lost-state, night/weather danger.

Cell 0 work is allowed only when it directly supports world validation, packaged evidence, spawn safety, or later placement inside the world.

## Hard Guards

Do not add player-facing map, minimap, compass, bearing number/readout, GPS marker, objective marker navigation, HUD clock, explicit time readout, orientation-confidence UI, hidden orientation meter, or character comments that tell the player whether they are oriented.

Do not add modern navigation tools, combat systems, multiplayer systems, fantasy magic, gore-horror framing, or binary asset churn for this slice.

## Handoff Cadence

Every 5 PR-slices in this world-first run must produce a tracked handoff under `handoffs/`, then implementation continues immediately.

## First 30 PR-Slice Spine

### Terrain Spine

1. World-first reset and gate note.
2. Sprint target/handoff cadence.
3. Terrain source data contract.
4. Macro island landform contract.
5. Terrain validation points.
6. Handoff 1.
7. Biome mask taxonomy.
8. Traversal category model.
9. Creek/riparian mask model.
10. Ridge/slope mask model.

### Biome Spine

11. Handoff 2.
12. PCG biome output contract.
13. Creek/riparian ecosystem pass contract.
14. Ridge/slope/mountain ecosystem pass contract.
15. Lowland/alluvial/gorge ecosystem pass contract.
16. Handoff 3.
17. Developer-only ecosystem debug categories.
18. Forbidden navigation UI validation guard.
19. World validation runner expectations.
20. Multi-angle world visual evidence plan.

### Gameplay Coupling

21. Handoff 4.
22. Navigation pillars terrain coupling.
23. Lost-state terrain/biome coupling.
24. Day/night/weather terrain/biome coupling.
25. First playable world route classes.
26. Handoff 5.
27. Generated-world evidence checklist.
28. Workspace-agent/Codex worker status checkpoint.
29. Next 30 PR-slice implementation runway.
30. Handoff 6 and continue.

## Acceptance Criteria

- Active roadmap clearly prioritizes world terrain and biomes.
- Handoff cadence is every 5 PR-slices.
- Cell 0 polish is no longer the next default target.
- No forbidden navigation/time/orientation UI is introduced.
- The next implementation slice can proceed without asking for direction.
