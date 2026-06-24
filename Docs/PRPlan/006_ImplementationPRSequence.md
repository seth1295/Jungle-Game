# Implementation PR Sequence

## Global Rules For Every Implementation PR

Every PR in this sequence must preserve the hard navigation rule:

- no player map;
- no minimap;
- no compass;
- no bearing number/readout;
- no GPS marker;
- no objective marker navigation;
- no HUD clock;
- no explicit time readout;
- no orientation confidence system, hidden orientation meter, character comments, or UI-like feedback that tells the player whether they are oriented.

Every PR must also preserve the world-framing rule: the island world is primary, ocean is the natural boundary, and authored locations such as Cell 0, shelters, intro sequences, and story anchors are placed inside the world later rather than used to define or fake the world.

## PR #7: 16x16 World Partition Landscape Foundation

PR title: `16x16 World Partition landscape foundation`

Goal: Create the large-world island foundation for a 16 km x 16 km rainforest/mountain target using World Partition.

Why it exists: The project needs the streaming world container before terrain, ecosystem masks, PCG, or navigation systems can be implemented.

Allowed scope: World Partition level foundation, Landscape sizing setup, ocean-boundary assumption, OFPA/HLOD/Data Layer baseline configuration, validation docs.

Forbidden scope: Gameplay, ecosystem art pass, PCG density pass, final assets, invisible-wall-first island boundary, player map/minimap/compass/bearing/GPS/objective-marker/HUD-clock/explicit-time UI, generated/cache junk, unrelated Cell 0 changes.

Likely files/areas touched: `Content/` world/map files when unavoidable, `Config/` only if required for World Partition defaults, validation docs under `docs/` or `Docs/`.

Implementation notes: Prefer the 8129 x 8129 heightmap target at 2 m scale, producing 16.256 km per side. Record component/subsection settings from Epic's Landscape Technical Guide.

Validation expectations: Level opens or commandlet-validates; dimensions are documented; World Partition debug can be inspected; diff excludes generated/cache output.

Acceptance criteria: A clean large-world island foundation exists and is ready for terrain tooling.

Stop/ask conditions: Engine/editor blocks world creation; landscape size must change; generated binary output cannot be reviewed; project settings need broad changes.

## PR #8: Landscape Generation/Import/Tooling Scaffold

PR title: `Landscape generation/import/tooling scaffold`

Goal: Add a repeatable way to create or import the large landscape height data.

Why it exists: Terrain must be reproducible; hand-editing a one-off landscape is not enough for future ecosystem masks.

Allowed scope: Tooling scaffold, import notes, deterministic source heightmap workflow, validation docs.

Forbidden scope: Final terrain art, PCG population, gameplay, packaged builds, large arbitrary binary imports without explanation.

Likely files/areas touched: tooling scripts if approved by repo pattern, docs, source heightmap artifacts only if explicitly justified.

Implementation notes: The tooling should output height, slope, aspect, flow, ridge/valley, and mask-ready data or define how later PRs will derive it.

Validation expectations: Tool or import path can be rerun; output dimensions match the PR #7 landscape; no editor/cache junk is staged.

Acceptance criteria: The team can recreate the landscape input path from documented steps.

Stop/ask conditions: Tooling requires external paid tools/plugins; heightmap size changes; binary source files become too large for review.

## PR #9: Macro Terrain: Island Mountains, Ridges, Creek Systems, Basins, Coastline

PR title: `Macro terrain: island mountains, ridges, creek systems, basins, coastline`

Goal: Shape the first readable large-world terrain and traversal-affordance pass.

Why it exists: Navigation and ecology depend on actual landform, not flat terrain with painted biomes.

Allowed scope: Macro heightfield, island coastline, ocean boundary, beach, mangrove, cliff coast, rocky shore, creek mouth, exposed headland, difficult coastal terrain, mountain spine, ridges, spurs, saddles, valleys, gullies, slope breaks, creeks, basins, terrain traversal study, terrain validation.

Forbidden scope: Final foliage, final assets, authored trail network, arbitrary steep noise, constant sliding traversal, survival gameplay expansion, player map/minimap/compass/bearing/GPS/objective-marker/HUD-clock/explicit-time UI, full Cell 0 integration.

Likely files/areas touched: landscape source data, generated/imported landscape files, docs.

Implementation notes: Creek flow, ridge orientation, mountain silhouettes, spurs, saddles, slope breaks, gullies, basin structure, and coastline should be legible from first-person traversal. Do not draw authored route lines first; build the terrain conditions that allow routes to emerge. Mountains should create landmarks, catchments, ridge systems, creek heads, basins, and traversal pressure without becoming arbitrary steep noise or an impossible sliding simulator.

Validation expectations: Height/slope/creek/coast debug evidence; hard blocker, soft blocker, affordance-zone, and false-affordance study; mountain catchment/ridge/creek-head review; first-person landmark screenshots; no square ecosystem assumptions.

Acceptance criteria: The terrain can support discovered traversal through creek, ridge, spur, saddle, slope-break, basin, coast, and gully logic, plus later ecosystem masks.

Stop/ask conditions: Macro terrain cannot fit performance/editor limits; terrain requires protected-place replication; Cell 0 placement needs design decision.

## PR #10: Ecosystem Mask System

PR title: `Ecosystem mask system`

Goal: Implement the terrain-derived mask data path.

Why it exists: PCG and gameplay need stable ecosystem inputs rather than ad hoc painted regions.

Allowed scope: Height, slope, aspect, creek distance, coast/ocean distance, wetness, ridge/valley, land-zone proxy, canopy, disturbance, player-made trail mark persistence, sound propagation, visibility, sky window, footing/injury risk, ecosystem tells, night-danger, and traversal blocker/affordance mask definitions.

Forbidden scope: Final PCG density, final art, player map/minimap/compass/bearing/GPS/objective-marker/HUD-clock/explicit-time UI, unrelated survival systems.

Likely files/areas touched: mask tooling/data, debug docs, possibly source code or editor utility assets if approved.

Implementation notes: Masks must be world-space and cross Landscape Component boundaries naturally. Traversal masks must distinguish hard blockers, soft blockers, affordance zones, and false affordances. Wait-a-while must be represented as a soft-blocker/terrain-friction concept.

Validation expectations: Raw mask ranges inspectable; classification inputs documented; traversal categories inspectable; coast/ocean and sky-window masks inspectable; mask continuity across component borders checked.

Acceptance criteria: Named masks exist or have deterministic placeholders ready for PR #11 debug visualisation.

Stop/ask conditions: Mask generation requires a new external dependency; data storage format is unclear; masks align to technical grid cells.

## PR #11: Ecosystem Debug Visualisation And Validation

PR title: `Ecosystem debug visualisation and validation`

Goal: Make ecosystem masks and classifications inspectable.

Why it exists: Without debug views, PCG will hide bad ecological logic.

Allowed scope: Debug overlays, editor-only views, validation commands, mask summary docs.

Forbidden scope: Player-facing map, compass, minimap, bearing readout, GPS marker, objective marker navigation, HUD clock, explicit time readout, orientation confidence UI, final gameplay UI, PCG art density.

Likely files/areas touched: debug materials/tools, editor-only assets, docs.

Implementation notes: Debug views can show top-down masks for developers, but must not become player navigation aids. Do not implement orientation confidence, hidden orientation meters, or character comments that tell the player whether they are oriented.

Validation expectations: Screenshots or documented outputs for each mask; component-border continuity check; first-person classification check.

Acceptance criteria: A reviewer can inspect why an ecosystem classification exists at representative points.

Stop/ask conditions: Debug tooling requires runtime player UI; overlays cannot distinguish raw masks from final classification.

## PR #12: PCG Biome Framework

PR title: `PCG biome framework`

Goal: Create the PCG framework that consumes masks and emits controlled placeholder ecosystem and traversal-resistance content.

Why it exists: Later ecology passes need stable graphs and data interfaces before asset density increases.

Allowed scope: PCG graph framework, placeholder spawn rules, island edge framework, terrain-friction rules, density budgets, debug outputs.

Forbidden scope: Final foliage art, asset-pack dump, full performance pass, unrelated gameplay.

Likely files/areas touched: PCG assets, content placeholders, docs, possibly config if PCG plugin enabling is required.

Implementation notes: Use named mask inputs. Keep output deterministic and region-scoped. PCG should respond to terrain-derived affordances and blockers rather than drawing primary trails. It should support valid future semi-random world-location spawns rather than assuming all structure radiates from camp.

Validation expectations: PCG outputs can be regenerated; hard/soft blocker and affordance behavior is visible; wait-a-while friction is represented as readable plant friction, not collision noise; World Partition load/unload still works; dense test area performance is noted.

Acceptance criteria: Creek, ridge, slope, lowland, and gorge passes can be added without rebuilding the framework.

Stop/ask conditions: PCG requires unapproved plugins; output creates unreviewable binary sprawl; streaming breaks.

## PR #13: Creek/Riparian/Wet Valley Ecosystem Pass

PR title: `Creek/riparian/wet valley ecosystem pass`

Goal: Implement the first creek and wet-valley ecosystem content and traversal-affordance pass.

Why it exists: Creeks are primary navigation and survival-pressure structures.

Allowed scope: Riparian PCG, creek-edge clutter, wet valley density, water-sound anchors, crossing candidates, wet-zone ecosystem tells, leech/tick placeholder risk notes.

Forbidden scope: Final water simulation, final audio mix, full wildlife AI, player map/minimap/compass/bearing/GPS/objective-marker/HUD-clock/explicit-time UI.

Likely files/areas touched: PCG graphs/assets, creek/riparian content, docs.

Implementation notes: Creek influence should vary by channel size, valley width, slope, and wetness. Creek banks may become affordance zones, mud/deadfall/wait-a-while soft blockers, water hard blockers, or false affordances depending on local conditions. Frogs, insects, waterfalls, and muffled rain-state water sounds are navigation evidence.

Validation expectations: Creek bands follow channels; crossings are legible; player can infer creek route logic; streaming/performance noted.

Acceptance criteria: A player can use creeks as imperfect terrain-driven navigation structures without being handed explicit authored route lines.

Stop/ask conditions: Creek zones align to grid cells; density blocks traversal; water feature scope expands beyond first playable.

## PR #14: Ridge/Slope/Mountain Ecosystem Pass

PR title: `Ridge/slope/mountain ecosystem pass`

Goal: Implement upland, ridge, mountain shoulder, and steep-slope ecosystem content.

Why it exists: Ridges and mountain silhouettes are core no-map navigation cues.

Allowed scope: Ridge/slope PCG, exposed/sheltered aspect variation, skyline landmarks, sky windows, slope traversal readability, terrain fatigue rules.

Forbidden scope: Climbing system, final mountain art, full weather simulation, unrelated enemy/gameplay systems, any player-facing navigation/time/orientation UI forbidden by the global rules.

Likely files/areas touched: PCG graphs/assets, landmark placeholders, docs.

Implementation notes: Preserve sightline windows and silhouette anchors. Avoid filling every ridge with uniform dense foliage. Ridges, spurs, saddles, slope breaks, open canopy, wind, and sky windows should create discovered movement affordances. Steep slopes should create fatigue, time loss, noise, injury risk, and route reconsideration rather than constant physics sliding.

Validation expectations: Ridge affordances are readable; mountain silhouettes survive HLOD checks; slope difficulty is visible; traversal does not feel like fighting collision.

Acceptance criteria: A player can use ridges, spurs, saddles, slope breaks, and mountains to orient without compass, map, minimap, bearing, GPS, objective-marker, HUD-clock, or explicit-time UI.

Stop/ask conditions: Landmarks become artificial signage; HLOD erases navigation silhouettes; slope content traps the player.

## PR #15: Lowland/Alluvial/Gorge Ecosystem Pass

PR title: `Lowland/alluvial/gorge ecosystem pass`

Goal: Implement lowland, alluvial, gorge, and transition ecosystem content.

Why it exists: The world needs basin movement structure, floodplain contrast, and compressed gorge navigation.

Allowed scope: Lowland/alluvial/gorge PCG, island-edge/coastal transition density, mangrove/creek-mouth/cliff-coast placeholders, transition density, constrained corridor readability, validation docs.

Forbidden scope: Final cinematics, full hydrology simulation, full wildlife ecology, unrelated Cell 0 polish, any player-facing navigation/time/orientation UI forbidden by the global rules.

Likely files/areas touched: PCG graphs/assets, gorge/lowland placeholders, docs.

Implementation notes: Gorges should compress sightlines and sound. Alluvial zones should feel wetter/flatter through structure, not arbitrary swamp vibes. Gullies, dry creek beds, old scars, regrowth gaps, misleading clearings, and beach routes that end in cliffs or mangroves can be false affordances.

Validation expectations: Transitions read in first person; gorge corridors are navigable; performance in dense lowlands is noted.

Acceptance criteria: Lowland, alluvial, and gorge areas have distinct navigation meaning.

Stop/ask conditions: Dense lowlands fail performance; gorge geometry creates unrecoverable player traps; ecosystem logic becomes arbitrary.

## PR #16: Navigation Pillars: Sun, Sky, Sound, Coast, Creeks, Ridges, Landmarks

PR title: `Navigation pillars: sun, sky, sound, coast, creeks, ridges, landmarks`

Goal: Implement the non-map navigation and terrain-first traversal cue set.

Why it exists: The player needs world-readable orientation tools instead of UI map/compass aids.

Allowed scope: Sun-position readability, shadow direction/length, light colour, creek-flow cues, slope cues, coastline/ocean cues, ridge/spur/saddle/mountain landmarks, sky windows, stars/moon/southern-sky logic, environmental sound cue hooks, traversal-affordance cue hooks, player-made trail mark plan.

Forbidden scope: Player map, compass, minimap, bearing number/readout, GPS marker, HUD clock, explicit time readout, objective marker navigation, objective arrow, modern navigation tools.

Likely files/areas touched: gameplay source, materials/VFX placeholders, landmark actors, docs.

Implementation notes: Cues should be diegetic and imperfect. Camp smoke can be a visible anchor only if a camp exists and only under appropriate weather/line-of-sight conditions. Traversal cues should expose landform affordances rather than prescribe path lines. The player may estimate time from world presentation, but no explicit clock is allowed. Do not add orientation confidence, character orientation comments, hidden meters, or UI-like feedback.

Validation expectations: Player can orient from selected test points using environmental cues; canopy, cloud, gorge terrain, and night can degrade sun/shadow cues; no player-facing map, compass, minimap, bearing readout, GPS marker, objective marker navigation, HUD clock, explicit time readout, orientation confidence system, hidden orientation meter, or character orientation comments exist.

Acceptance criteria: Navigation relies on sun, shadows, light colour, slope, creek flow, coastline/ocean, ridge, mountain silhouette, landmark, vegetation/ecosystem transition, sound, weather, sky windows, stars, moon, and player interpretation.

Stop/ask conditions: Design pressure pushes toward UI navigation; cues are unreadable in first person; shelter/fire/smoke cues become globally visible magic.

## PR #17: No-Map/No-Compass Lost-State Gameplay Loop

PR title: `No-map/no-compass lost-state gameplay loop`

Goal: Implement first-pass lost-state rules and recovery pressure.

Why it exists: Getting lost is gameplay, not a missing feature.

Allowed scope: Lost-state detection/proxy, distance-from-known-safety pressure, traversal uncertainty, recovery cues, player-made trail mark interactions, debug-only tools.

Forbidden scope: Player map, compass, minimap, bearing readout, GPS marker, HUD clock, explicit time readout, objective marker navigation, quest marker, orientation confidence system, character comments that tell the player whether they are oriented, full survival stat stack.

Likely files/areas touched: gameplay source, debug tools, docs.

Implementation notes: Lost-state should escalate pressure through darkness, rain, distance from known safety, visibility, slope, terrain fatigue, weather-muffled sound, erased trail marks, and traversal uncertainty. The world provides evidence; the player interprets it.

Validation expectations: Player can become disoriented, recover through environment reading, and experience escalating pressure without UI rescue.

Acceptance criteria: The loop supports "I am lost, but the world gives me clues."

Stop/ask conditions: Lost-state becomes arbitrary punishment; recovery requires hidden debug knowledge; no-map/no-compass rule is challenged.

## PR #18: Day/Night/Weather Danger Prototype

PR title: `Day/night/weather danger prototype`

Goal: Prototype time and weather as survival-navigation pressure.

Why it exists: Night must change the rules, and weather must affect traversal confidence.

Allowed scope: Day/night cycle prototype, rain/fog/visibility modifiers, sun/shadow cue degradation, star/moon/sky visibility logic, weather attacks navigation, sound muffling, creek swelling, trail-mark degradation, night danger mask consumption, known-safety rules.

Forbidden scope: Final weather simulation, explicit HUD clock, explicit time readout, player map/minimap/compass/bearing/GPS/objective-marker UI, full damage/death system, horror chase system, combat.

Likely files/areas touched: gameplay source, lighting/weather placeholders, docs.

Implementation notes: Night should remove sun-based direction, reduce landmark visibility, change sound priorities, increase creek/slope risk, and make known shelters or fire/smoke more important where they exist. Rough direction may come from stars through canopy breaks, southern-sky orientation appropriate to FNQ/southern hemisphere, moonlight/moon position, ocean sound/coastline direction, mountain/ridge silhouettes against sky glow, creek flow, slope, and terrain memory. Dense canopy, clouds, rain, gullies, gorges, and mountain occlusion should reduce or remove sky navigation.

Validation expectations: The same terrain feels materially different by day, rain, and night; the player estimates time from world presentation only; night remains dangerous because sun-based direction is gone, not because the world is unreadable.

Acceptance criteria: Night is a serious danger state without becoming an instant-fail state.

Stop/ask conditions: Prototype requires final art/audio; weather breaks performance; night becomes unreadable rather than dangerous.

## PR #19: Cell 0 First Playable Integration Inside The Large World

PR title: `Cell 0 first playable integration inside the large world`

Goal: Move or integrate Cell 0 as a real location inside the large-world first playable.

Why it exists: The current Cell 0 blockout must become part of the world, not a separate proof-of-concept island.

Allowed scope: Cell 0 placement inside the island, terrain-affordance integration, authored shelter/story-anchor placement rules, Cell 0 loop preservation, smoke/landmark relationship where applicable, validation docs.

Forbidden scope: Full story expansion, final art pass, unrelated mechanics, player map/minimap/compass/bearing/GPS/objective-marker/HUD-clock/explicit-time UI.

Likely files/areas touched: Cell 0 actors/content, large-world level actors, gameplay integration code, docs.

Implementation notes: Preserve the ember/rain/crossing/evidence/watcher loop while making movement choices depend on real terrain/ecology. Cell 0 must be placed inside the world later; it must not define or fake the whole world structure.

Validation expectations: Cell 0 remains playable as a placed world location; entering and leaving it through surrounding terrain works; streaming does not break Cell 0 state.

Acceptance criteria: Cell 0 is a first-playable location inside the large world.

Stop/ask conditions: Integration breaks the existing slice; binary asset churn becomes too broad; scope expands to full narrative content.

## PR #20: Packaged Large-World First Playable Smoke, Performance Checks, And Release Notes

PR title: `Packaged large-world first playable smoke, performance checks, and release notes`

Goal: Produce packaged first-playable evidence and release notes.

Why it exists: The roadmap ends with a runnable first playable, not only editor evidence.

Allowed scope: Package/cook fixes, smoke testing, performance checks, release notes, blocker documentation.

Forbidden scope: New major gameplay systems, final art expansion, unrelated refactors, any player-facing navigation/time/orientation UI forbidden by the global rules, merge without explicit authorization.

Likely files/areas touched: docs, packaging notes, narrowly scoped config or source fixes if package-blocking.

Implementation notes: Fix only package/smoke blockers that are in scope. Defer large feature improvements.

Validation expectations: Package/cook result; launch smoke; representative FPS/performance notes; World Partition/HLOD/Data Layer checks; clean diff.

Acceptance criteria: A packaged large-world first playable can be launched or a precise external blocker is documented.

Stop/ask conditions: Packaging requires broad engine/config changes; performance failure needs redesign; generated output pollutes the diff.
