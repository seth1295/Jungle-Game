# World-First Terrain/Biome Sprint Target

Created: 2026-06-25
Project: Jungle Game
Mode: internal implementation target for the active worker

## Non-Negotiable Operating Rule

Do not stop at discussion. Continue implementation until a hard blocker prevents file inspection and file edits. User prompts are steering input, not permission to stop and summarize.

Use available forward paths in this priority order:

1. DevSpace `read` / `edit` / `write` for repo work.
2. Workspace-agent / Codex repo-worker when available as a first-class tool.
3. DevSpace `bash` only for final validation gates, package/test/build/git inspection, or read-only discovery when necessary.
4. If one path is blocked, route around it and continue with another useful in-scope task.

Every 5 PR-slices, create a handoff and continue immediately.

## 3-Hour End-State Target

The game should be back on a world-first implementation spine:

- terrain/map/world systems before Cell 0 polish;
- biome and traversal systems as the next long implementation run;
- source-authored specs/debug hooks where binary/editor mutation is unsafe;
- packaged/visual evidence paths preserved for later validation;
- no player-facing map, minimap, compass, bearing, GPS, objective marker, HUD clock, explicit time readout, or orientation-confidence UI.

The desired end of the sprint is not a tiny cleanup. The desired end is a repo state where Jungle Game clearly moves toward a shippable terrain/biome survival-navigation game.

## PR-Slice Cadence

Assume a target pace of about 10 small PR-slices per hour. In 3 hours, plan around 30 scoped slices with handoffs after slices 5, 10, 15, 20, 25, and 30.

A PR-slice means one coherent, reviewable repo step. It may be docs, source spec, validation hook, debug hook, code scaffold, or package/visual evidence work, but it must advance the world-first implementation track.

## Hour 1: Terrain Spine

Goal: lock the world terrain vocabulary and implementation runway.

1. Reset active roadmap to world-first terrain/biome focus.
2. Record 5-PR handoff cadence in workflow memory/docs.
3. Harden terrain source spec: height, slope, aspect, ridge, valley, flow, coast distance.
4. Harden macro island landform spec: mountain spine, basins, creek heads, coastline classes.
5. Define terrain validation points: coast, ridge, creek, lowland, gorge, saddle.
6. Handoff 1 and continue.
7. Define biome mask taxonomy: terrain-derived mask names and value ranges.
8. Define traversal categories: hard blocker, soft blocker, affordance, false affordance.
9. Define creek/riparian mask model: wetness, bank affordance, crossing candidates, water-sound anchors.
10. Define ridge/slope mask model: skyline, sky windows, fatigue slope bands, route pressure.

## Hour 2: Biome Spine

Goal: move biome implementation from roadmap text into source-authored specs and debug hooks.

11. Handoff 2 and continue.
12. Extend PCG biome framework spec with named outputs and density budgets.
13. Harden creek/riparian/wet-valley ecosystem source spec.
14. Harden ridge/slope/mountain ecosystem source spec.
15. Harden lowland/alluvial/gorge ecosystem source spec.
16. Handoff 3 and continue.
17. Expand ecosystem debug categories and developer-only validation descriptions.
18. Add no-map/no-compass validation guard expectations.
19. Define world validation runner expectations without committing generated output.
20. Define multi-angle world visual evidence plan beyond Cell 0.

## Hour 3: Gameplay Coupling

Goal: connect terrain and biomes to traversal, orientation, danger, and shippable game direction.

21. Handoff 4 and continue.
22. Align navigation pillars with terrain-first cues: sun, ridge, creek, coast, sound, sky windows.
23. Align lost-state gameplay with world evidence and degraded certainty, not UI rescue.
24. Align day/night/weather danger with terrain and biome masks.
25. Define first playable world route classes: creek follow, ridge walk, basin crossing, gorge compression, coast false affordance.
26. Handoff 5 and continue.
27. Add generated-world evidence checklist for screenshots/logs/diff review.
28. Check Codex/workspace-agent status; use it if available, bypass if blocked.
29. Draft next 30 PR-slices for actual generation, masks, debug views, PCG outputs, and packaged evidence.
30. Handoff 6 and continue into the next implementation batch.

## Shippable Direction

A player should eventually be dropped into a large FNQ-inspired island jungle where the world itself provides the game:

- terrain form;
- creek systems;
- ridges, saddles, gullies, basins, coastlines;
- biome resistance and affordances;
- sound, light, canopy, weather, and night pressure;
- no explicit navigation UI rescue.

The sprint is successful when repo state, docs, source specs, and validation hooks all point in this direction without drifting back into Cell 0-only polish.

## Stop Conditions

Only stop for a true hard blocker:

- DevSpace workspace cannot be opened;
- file reads and file writes are both unavailable;
- safety boundary prevents continuing any in-scope implementation task;
- repo state is too ambiguous to safely modify and cannot be inspected.

Bash/test/git blockage alone is not a stop condition. It is a validation blocker; continue with read/edit/write tasks and record validation as pending.
