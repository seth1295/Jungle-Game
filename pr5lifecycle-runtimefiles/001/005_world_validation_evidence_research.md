# World Validation Evidence Research

## Executive Summary

Jungle Game needs a **developer-only world validation evidence workflow** that proves the island is physically readable, traversable, streamable, and playable without introducing any player-facing navigation aids. The evidence should answer one core question per PR: **can a reviewer see, from compact artifacts and notes, that the world system changed in a controlled, readable, runtime-safe way?**

For UE5 large worlds, validation should combine:

- **Editor-only diagnostic views** for terrain channels, biome masks, PCG outputs, traversal resistance, route classes, streaming cells, component seams, and weather/night modifiers.
- **First-person screenshot sets** captured from the player’s eye height, with no HUD, no compass, no map, no coordinate readout, and no orientation feedback.
- **Runtime package smoke evidence** proving that cooked/package execution still loads the relevant map, streams cells, spawns the player, preserves traversal behavior, and does not expose debug systems.
- **Repo hygiene proof** showing that screenshots, logs, packaged builds, caches, generated PCG output, and editor junk were kept out of source control.

UE5 supports the relevant validation foundations: World Partition subdivides a single persistent level into streamable grid cells loaded by streaming sources; Landscape is component-based and has built-in layer debug visualization; PCG has node-level debug rendering and inspection; UAT/BuildCookRun handles cook/package/stage/run operations; Data Validation can run editor and command-line asset validation; and Automation tests define smoke tests as fast checks rather than long traversal passes.

The recommended workflow is: **commit source and concise Markdown validation notes only; keep generated evidence external; link or identify artifacts by stable manifest entries; and require each world-system PR to include minimum evidence for the systems it touched.**

## Design Goals For Jungle Game

The evidence workflow must serve Jungle Game’s specific design constraints, not generic open-world tooling.

### Primary validation goals

1. **World-first implementation**
   - Terrain form, biome placement, vegetation density, traversal resistance, and route readability should be implemented and validated before feature-heavy survival layers.
   - Validation should prove that the world is playable from the ground, not merely attractive from editor flycam or top-down views.

2. **No-map readability**
   - The player should navigate by coastlines, creeks, ridges, saddles, basins, gorges, canopy gaps, slope breaks, vegetation transitions, weather, sound, and memory.
   - Developer evidence may use overlays, masks, top-down debug captures, and coordinates, but these must never become player-facing navigation systems.

3. **Physically and ecologically normal world**
   - Terrain and biome transitions should read as natural tropical island logic: wet lowlands, creek corridors, ridges, dense canopy, exposed coast, slope breaks, erosion-like paths, and believable obstruction.
   - The unsettling relict hominin presence should be validated through environmental changes and observation spaces, not monster, chase, combat, gore, fantasy, or explicit supernatural framing.

4. **Reviewable validation**
   - Every world-system PR should include a small, structured Markdown evidence note that CodeRabbit and human reviewers can scan quickly.
   - The note should summarize what was tested, where evidence lives, what passed, what failed, and what is intentionally deferred.

5. **No source bloat**
   - Generated screenshots, videos, logs, cooked builds, package outputs, Derived Data Cache, Intermediate files, Saved files, and transient PCG/cache output must not be committed.
   - UE’s Derived Data Cache is designed to store large regenerable derived data outside source control.

## Evidence Categories

Use consistent evidence category names so PR notes, debug widgets, artifact manifests, and future automation can align.

| Evidence Category | Suggested ID | What It Proves | Minimum Artifact Type |
|---|---:|---|---|
| Terrain form | `WV_TerrainForm` | Landforms are readable, scaled, continuous, and physically plausible. | Editor debug screenshot plus first-person screenshot. |
| Terrain continuity | `WV_TerrainContinuity` | Landscape component seams, slope transitions, collision, and streaming boundaries do not create visual or traversal breaks. | Seam overlay screenshot plus cross-boundary runtime note. |
| Biome mask output | `WV_BiomeMask` | Biome masks produce expected dominant and blended regions. | Mask overlay screenshot and sample values. |
| PCG generation output | `WV_PCGOutput` | Vegetation/rocks/debris/spawned environment assets respect biome, slope, wetness, density, and exclusion rules. | PCG debug screenshot and count/range summary. |
| Traversal resistance | `WV_TraversalResistance` | Terrain and vegetation impose intended movement cost without invisible unfairness. | Traversal heatmap and first-person affordance screenshots. |
| Route class evidence | `WV_RouteClass` | Natural route types exist and are legible: coast, creek bed, ridge line, saddle pass, animal track, blocked route, false affordance. | Route overlay and player-view decision-point screenshot. |
| No-map readability | `WV_NoMapReadability` | A player can infer local direction and route options without UI navigation aids. | HUD-free first-person screenshot set. |
| Weather/night impact | `WV_WeatherNightImpact` | Rain, fog, wetness, and darkness degrade readability without destroying fair navigation. | Matched clear/day versus adverse/night screenshots. |
| World Partition continuity | `WV_WorldPartitionContinuity` | Streamed cells load/unload without visible holes, collision loss, missing PCG output, or discontinuous biome/traversal data. | Cell-boundary pass note and editor cell/debug capture. |
| Package smoke | `WV_PackageSmoke` | Cooked/package build starts, loads map, spawns player, traverses validation route, and hides developer debug views. | Short smoke note with build type, map, route, result. |
| Repo hygiene | `WV_RepoHygiene` | Generated output stayed out of git, and committed files are intentional. | `git status --short` summary and committed-file list. |

### Evidence should prove these concrete claims

#### Terrain form

Evidence should show:

- Major landforms exist and are readable from ground level: coast, creek, ridge, saddle, basin, gorge, lowland, canopy gap, slope break.
- Height, slope, and curvature-like transitions produce believable movement choices.
- Landscape component boundaries do not create seams, cracks, collision steps, texture discontinuities, or sudden foliage density changes.
- Terrain is not validated only from top-down editor views.

UE Landscape is component-based; components drive rendering, visibility calculation, collision, and LOD behavior, so terrain evidence must include component and seam checks, not just visual beauty shots.

#### Biome outputs

Evidence should show:

- Dominant biome class at sample points.
- Mask weights and transition bands.
- No hard square/tile boundaries unless intentionally hidden by terrain or vegetation.
- PCG output density matches the masks.
- Exclusion zones work around creeks, cliffs, beaches, rock shelves, canopy gaps, and traversal corridors.

#### Traversal resistance

Evidence should show:

- Traversal resistance matches what the player sees.
- Dense vegetation, mud, steep slope, rock, water, and deadfall slow or block the player consistently.
- Walkable-looking areas are not unfairly blocked.
- Blocked or slow areas have visible cause: dense roots, steepness, water depth, unstable rock, thorny vegetation, fallen trunks, etc.
- False affordances are intentional and readable, not accidental collision bugs.

#### Route classes

Evidence should show route classes as world logic, not UI logic:

- `CoastRoute`
- `CreekRoute`
- `RidgeRoute`
- `SaddlePass`
- `BasinExit`
- `GorgeRoute`
- `LowlandPush`
- `CanopyGapRoute`
- `SlopeBreakTraverse`
- `FalseAffordance`
- `BlockedRoute`

Routes should be validated from first-person decision points, because the player cannot use a map, minimap, compass, bearing readout, GPS, objective marker, HUD clock, orientation confidence UI, hidden orientation meter, or character comments that explain orientation.

#### Visual readability

Evidence should show:

- Landmark silhouettes.
- Vegetation density gradients.
- Light direction and canopy gaps.
- Creek flow direction, bank shape, and sound corridor where implemented.
- Ridge/saddle visibility through partial occlusion.
- Coast/creek/ridge route choices without explicit UI.
- Weather and night conditions that reduce confidence but do not collapse readability into random wandering.

## Developer-Only Debug Views

All debug views must be **developer-only**. They may exist in editor builds or explicitly gated development builds, but they must not ship as player-facing UI, diegetic tools, hidden systems, or character feedback.

PCG-specific debug should use UE’s PCG debug rendering and node inspection where possible; Epic documents node debug rendering, node enable/disable, and inspection of generated point attributes. The PCG Debug node is documented as editor-only and does not execute in non-editor builds.

### Required debug view groups

#### Terrain channel views

| Debug View Name | Purpose | Notes |
|---|---|---|
| `DBG_TerrainHeightBands` | Shows height bands and major elevation zones. | Useful for proving coast/ridge/basin/gorge form. |
| `DBG_TerrainSlope` | Shows slope ranges used by traversal and biome rules. | Must align with movement slowdown/blocking rules. |
| `DBG_TerrainSlopeBreaks` | Highlights abrupt slope transitions. | Critical for readable route choice. |
| `DBG_TerrainDrainageBias` | Shows likely creek/drainage corridors if generated or derived. | May be approximate; label uncertainty if heuristic. |
| `DBG_TerrainLandformClass` | Shows classified coast, ridge, saddle, basin, gorge, lowland, slope break. | Developer-only classification, not player UI. |
| `DBG_LandscapeComponentGrid` | Shows component edges and section/cell continuity. | UE Landscape has built-in component visualization concepts and layer debug support. |
| `DBG_ComponentSeamRisk` | Highlights component or tile edges with discontinuity risk. | Use for terrain height, material, foliage, collision, and traversal seams. |

#### Biome and mask views

| Debug View Name | Purpose | Notes |
|---|---|---|
| `DBG_BiomeDominantClass` | Shows the winning biome per sampled area. | Use stable class colors in editor only. |
| `DBG_BiomeMaskWeights` | Shows numeric/gradient mask values for selected biome. | Should support coast, lowland, riparian, ridge, gorge, basin, canopy gap. |
| `DBG_BiomeTransitionBands` | Shows blend zones between biomes. | Proves transitions are not hard tile artifacts. |
| `DBG_WetnessMask` | Shows wet ground, creek influence, basin wetness, post-rain wetness. | Useful for movement and material validation. |
| `DBG_CanopyDensity` | Shows canopy closure/opening. | Needed for visibility, night readability, and canopy gap evidence. |
| `DBG_PCGDensity` | Shows point/asset spawn density. | Use PCG debug rendering and inspect attributes. |
| `DBG_PCGSpawnClass` | Shows generated asset class/type distribution. | Proves biome output is not just mask math. |
| `DBG_PCGExclusionZones` | Shows where PCG is suppressed. | Must protect paths, water, steep rock, player spawn, validation routes. |

#### Traversal and route views

| Debug View Name | Purpose | Notes |
|---|---|---|
| `DBG_TraversalResistance` | Shows continuous movement cost/resistance. | Should be sampled from the same data used by movement/traversal logic. |
| `DBG_TraversalClass` | Shows discrete class: open, slow vegetation, steep slow, wet slow, scramble, blocked. | Must match visible affordances. |
| `DBG_RouteClass` | Shows route classification. | Route evidence only; never player-facing. |
| `DBG_RouteCostGradient` | Shows low-to-high route cost around a decision area. | Useful for detecting accidental funnels or unfair dead ends. |
| `DBG_RouteCandidateGraph` | Shows candidate route links between natural features. | Editor-only; avoid turning this into a game map. |
| `DBG_FalseAffordanceMarkers` | Shows intentionally misleading-looking but readable blocked/slow routes. | Must be justified by visual obstruction, not invisible collision. |
| `DBG_PlayerStepProbe` | Shows recent player movement samples, resistance, and blocking causes. | Development/editor only; useful for smoke route notes. |

#### Weather, night, and readability views

| Debug View Name | Purpose | Notes |
|---|---|---|
| `DBG_NightVisibilityBands` | Shows estimated visibility bands under night lighting. | Do not expose as player confidence UI. |
| `DBG_RainVisibilityImpact` | Shows rain/fog/particle visibility reduction zones. | Compare to first-person screenshots. |
| `DBG_WetTraversalDelta` | Shows traversal changes caused by rain/wetness. | Useful for creek banks, basins, mud, slopes. |
| `DBG_LandmarkSilhouetteCheck` | Highlights visible ridge/coast/canopy-gap landmarks from validation viewpoints. | Editor-only screenshot planning tool. |
| `DBG_ReadabilityOcclusion` | Shows whether vegetation fully hides intended route cues. | Should not become objective marker logic. |

#### World Partition and streaming continuity views

| Debug View Name | Purpose | Notes |
|---|---|---|
| `DBG_WorldPartitionCellState` | Shows loaded/visible/nearby streaming cells during validation. | World Partition uses streamable grid cells loaded by streaming sources. |
| `DBG_StreamingBoundaryContinuity` | Shows route and biome continuity across cell boundaries. | Required for large-world PRs touching streaming or generated content. |
| `DBG_CellEdgePCGContinuity` | Shows PCG density/class continuity across World Partition cell edges. | Especially important for vegetation and route corridors. |
| `DBG_CellEdgeTraversalContinuity` | Shows traversal class/cost continuity across cell edges. | Prevents hidden walls, sudden movement changes, or missing collision. |
| `DBG_HLODContext` | Shows far-view simplification context if HLOD is relevant. | HLOD build automation can be handled by World Partition builder commandlets when needed. |

### Debug gating rules

Debug views should be implemented behind one of these gates:

- `WITH_EDITOR` editor-only systems.
- Editor Utility Widget or editor subsystem.
- Non-shipping development console flag that is stripped or disabled in shipping.
- Dedicated validation map or validation data layer that is never player-facing.

Forbidden debug exposure:

- No in-game map.
- No minimap.
- No compass.
- No bearing number.
- No GPS coordinate display.
- No objective marker.
- No HUD clock.
- No explicit time readout.
- No orientation confidence meter.
- No hidden orientation system that comments on whether the player is oriented.
- No character bark such as “I’m going north,” “I’m lost,” “this is the right way,” or equivalent.

## First-Person Screenshot View Plan

The first-person screenshot plan should prove **ground-level readability**. Each screenshot should be captured from approximately player eye height with normal player FOV and no HUD/debug overlays unless the artifact is explicitly a debug comparison shot.

### Naming convention

Use deterministic names:

```text
FPV_<LandformOrCase>_<ViewIntent>_<Condition>_<Sequence>.png
```

Examples:

```text
FPV_Coast_ExitFromJungle_ClearDay_01.png
FPV_Creek_DownstreamDecision_Rain_02.png
FPV_Ridge_LineReadable_Night_01.png
FPV_FalseAffordance_BlockedByVegetation_ClearDay_01.png
```

For debug comparisons:

```text
DBG_<ViewName>_<Area>_<Condition>_<Sequence>.png
```

Example:

```text
DBG_TraversalResistance_CreekBank_Rain_01.png
```

### Capture rules

- Capture from the player camera, not editor flycam, for player-readability evidence.
- Use the same FOV as intended gameplay.
- Disable player-facing HUD.
- Do not include coordinate overlays, compass overlays, map overlays, debug labels, or orientation hints in player-view evidence.
- Record the map name, validation area ID, condition, approximate camera bookmark ID, and commit hash in the Markdown evidence note.
- Capture at least one clear daytime shot and one degraded-condition shot when the PR touches weather, night, visibility, wetness, lighting, or traversal.
- For each decision point, capture:
  1. **Approach view**: what the player sees before choosing.
  2. **Decision view**: the point where route choice is made.
  3. **Confirmation/reversal view**: what confirms route, slowdown, blockage, or false affordance.

### Required viewpoint set

| View Name | Screenshot IDs | What To Prove | Failure Signs |
|---|---|---|---|
| Coast | `FPV_Coast_ApproachFromJungle`, `FPV_Coast_AlongShoreRoute`, `FPV_Coast_ReentryChoice` | Shoreline is recognizable, coast route is readable, jungle reentry is visible without a marker. | Coast looks like random bright edge; reentry invisible; route only obvious from top-down. |
| Creek | `FPV_Creek_UpstreamRead`, `FPV_Creek_DownstreamDecision`, `FPV_Creek_BankCrossing` | Creek corridor direction, banks, crossing/slowing rules, and riparian biome transition are readable. | Water edge collision mismatch; bank looks walkable but blocks; creek hidden by vegetation. |
| Ridge | `FPV_Ridge_LineReadable`, `FPV_Ridge_SideDrop`, `FPV_Ridge_ViewWindow` | Ridge line acts as natural route and landmark. | Crest indistinct; side drop unreadable; vegetation erases ridge cue. |
| Saddle | `FPV_Saddle_Approach`, `FPV_Saddle_PassDecision`, `FPV_Saddle_ExitView` | Saddle reads as a pass between high areas. | Looks like a flat random clearing; route cue exists only in debug overlay. |
| Basin | `FPV_Basin_Entry`, `FPV_Basin_Interior`, `FPV_Basin_ExitCue` | Basin feels enclosed; exit options are readable but not marked. | Player sees no exit cue; basin reads as arbitrary flat terrain. |
| Gorge | `FPV_Gorge_Entry`, `FPV_Gorge_NarrowRoute`, `FPV_Gorge_BlockOrExit` | Gorge walls, restricted path, wetness, and route class are visible. | Invisible walls; over-darkness; no clear distinction between blocked and passable rock. |
| Lowland | `FPV_Lowland_DensePush`, `FPV_Lowland_SubtleRoute`, `FPV_Lowland_LostRisk` | Dense lowland is slow but not unreadable noise. | All directions look identical; route depends on UI; vegetation collision unfair. |
| Canopy gap | `FPV_CanopyGap_LightCue`, `FPV_CanopyGap_RouteChoice`, `FPV_CanopyGap_FalseCue` | Light gaps provide environmental cue without becoming objective markers. | Gap looks like magical beacon; gap always means correct route; no false cases. |
| Slope break | `FPV_SlopeBreak_Approach`, `FPV_SlopeBreak_TraversalChange`, `FPV_SlopeBreak_SafeAlternative` | Movement transition is visually fair before slowdown/blockage. | Sudden slowdown without visible terrain change; cliff/slope scale unreadable. |
| False affordance | `FPV_FalseAffordance_ApparentRoute`, `FPV_FalseAffordance_CloseRead`, `FPV_FalseAffordance_AlternativeCue` | Misleading route is fair: player can inspect and understand why it fails or costs more. | Invisible blocker; arbitrary collision; no alternative cue; feels like designer trickery. |

### Condition matrix

Minimum condition captures:

| PR Type | Required Conditions |
|---|---|
| Terrain-only | Clear day, no debug HUD. |
| Biome/PCG | Clear day plus debug biome/PCG comparison. |
| Traversal | Clear day plus movement/resistance debug comparison. |
| Weather or wetness | Clear day and rain/wet condition from same viewpoint. |
| Night/lighting | Clear day and night/dusk from same viewpoint. |
| Streaming/World Partition | First-person cross-boundary route plus editor cell-state/debug capture. |
| No-map readability | At least three connected first-person views showing approach, decision, and confirmation without UI aids. |

## Package And Runtime Smoke Evidence

Editor validation and package validation serve different purposes.

### Editor-only validation

Editor validation should prove:

- Terrain and biome data are correct.
- Debug overlays match source masks and generated output.
- PCG graph assumptions are inspectable.
- Data layers, landscape components, and World Partition cells are continuous.
- Developer views are useful for diagnosis.
- Validation notes can be produced without committing generated artifacts.

Recommended editor-only checks:

- Open validation map.
- Toggle `DBG_TerrainHeightBands`.
- Toggle `DBG_TerrainSlope`.
- Toggle `DBG_BiomeDominantClass`.
- Toggle `DBG_BiomeMaskWeights`.
- Toggle `DBG_PCGDensity`.
- Inspect representative PCG nodes and attributes.
- Toggle `DBG_TraversalResistance`.
- Toggle `DBG_RouteClass`.
- Toggle `DBG_WorldPartitionCellState`.
- Cross at least one landscape component seam and one World Partition cell boundary.
- Capture required first-person screenshots with debug disabled.
- Capture matching debug screenshots only where they explain the system changed.

UE’s Data Validation system can validate assets in editor and via command line, including project-wide validation from `Tools > Validate Data...` and command-line `-run=DataValidation`.

### Package/runtime smoke validation

Package validation should prove:

- The cooked/package build starts.
- The intended map loads.
- The player spawns correctly.
- The tested route is traversable in runtime.
- World Partition streaming works during movement.
- Debug overlays are not exposed.
- No player-facing map, compass, GPS, objective marker, HUD clock, explicit time readout, or orientation confidence UI appears.
- Weather/night behavior works in runtime if the PR touches those systems.
- There are no obvious missing materials, missing PCG output, blocking collision regressions, or streaming holes.

UE packaging uses build, cook, stage, package, deploy, and run stages through UAT/BuildCookRun or editor platform tools. Package validation should therefore record the build configuration and whether the evidence came from editor, PIE, standalone, Development package, or Shipping package.

### Package smoke expectations

For world-system PRs, use this minimum package evidence:

```text
Package smoke target: Linux Development package
Map: <ValidationMapName>
Route: <ValidationRouteID>
Duration: <short route duration or checkpoint count>
Result: PASS / FAIL
Debug exposed: NO
No-map guard passed: YES / NO
Streaming issue observed: YES / NO
Weather/night included: YES / NO / Not applicable
```

Do not overlabel long traversal routes as UE “Smoke” automation tests. UE’s Automation Test Framework defines smoke tests as a speed promise intended to complete within one second; long world walks are better described as **runtime smoke routes**, **manual smoke passes**, or **functional validation routes**.

### Package validation now versus later

Implement now:

- Linux Development package smoke route.
- Runtime no-debug-exposure check.
- One representative validation map.
- One short route per affected system.
- Manual evidence notes.

Defer:

- Full Shipping package matrix.
- Automated screenshot comparison baselines.
- Large-scale performance captures.
- Multi-platform smoke.
- Full overnight traversal soak tests.
- Formal CI/CD expansion beyond the small evidence workflow.

## Generated Output And Repo Hygiene Rules

Validation evidence must be reviewable without becoming source bloat.

### Commit these

Commit only:

- Source code.
- Editor-only validation tools.
- Small config files.
- Small data assets required for validation logic.
- Markdown validation notes.
- Templates.
- `.gitignore` / `.p4ignore` hygiene updates.
- Small deterministic text summaries if they are genuinely useful and stable.

### Do not commit these

Do not commit:

- Screenshots.
- Videos.
- Packaged builds.
- Cooked output.
- Staged output.
- Logs.
- Derived Data Cache.
- Intermediate files.
- Saved files.
- Editor autosaves.
- Crash dumps.
- Generated PCG caches unless they are deliberate authored assets and explicitly reviewed.
- Large binary validation artifacts.
- Local-only path references as the only evidence location.

Epic’s DDC documentation states derived data is large, regenerable, and kept in DDC instead of source control; GitHub’s UnrealEngine `.gitignore` template also ignores `Saved/*`, `Intermediate/*`, and `DerivedDataCache/*`.

### Recommended local artifact layout

Keep validation artifacts outside the repo, or in an ignored top-level folder if local convenience is necessary.

Preferred external layout:

```text
<external-artifact-root>/
  JungleGame/
    PR005_Batch001_File005/
      <commit-short-sha>/
        screenshots/
        debug/
        package_smoke/
        logs_local_only/
        manifest.md
```

Acceptable ignored repo-local layout:

```text
/ValidationArtifacts/
/LocalValidationOutput/
/PackagedBuilds/
/Saved/
/Intermediate/
/DerivedDataCache/
```

The PR should not depend on a private machine path. If artifacts are referenced, the validation note should include stable artifact names, hashes, or upload IDs where available.

### Suggested ignore rules

Use project-specific ignored folders for validation output:

```gitignore
# Jungle Game local validation artifacts
/ValidationArtifacts/
/LocalValidationOutput/
/PackagedBuilds/
/Screenshots/
/Videos/
/Logs/

# Unreal generated folders
/Saved/
/Intermediate/
/DerivedDataCache/

# Common transient evidence files
*.log
*.tmp
*.mp4
*.mov
*.mkv
*.avi
*.webm
*.crash
```

Avoid blindly ignoring UE’s `/Build/` folder unless the project has confirmed that no required icons, platform metadata, or packaging config live there. Prefer `/PackagedBuilds/` or an external build output path for cooked/package artifacts.

### Required repo hygiene evidence per PR

Each world-system PR should include:

```text
Repo hygiene:
- Generated screenshots/videos/logs/builds committed: NO
- Saved/Intermediate/DDC committed: NO
- Package output committed: NO
- Artifact manifest included in PR note: YES/NO
- git status checked before final commit: YES
- git diff --stat reviewed: YES
```

The validation note should list intentionally committed files by category, not paste giant logs.

## UE5 Implementation Guidance

### Recommended validation architecture

Use a small developer-only validation layer, not production gameplay UI.

Suggested module names:

```text
JungleWorldValidation
JungleWorldValidationEditor
```

Suggested asset/data names:

```text
DA_WorldValidationViewSet
DA_WorldValidationRouteSet
DA_WorldValidationBiomeSamples
DA_WorldValidationSmokeRoutes
MI_Debug_TerrainHeightBands
MI_Debug_TerrainSlope
MI_Debug_BiomeMaskWeights
MI_Debug_TraversalResistance
```

Suggested runtime/editor actors:

```text
BP_WorldValidationCameraBookmark
BP_WorldValidationRouteProbe
BP_WorldValidationBiomeSamplePoint
BP_WorldValidationStreamingBoundaryProbe
BP_WorldValidationScreenshotAnchor
```

These should be editor-only or validation-only where possible.

### Suggested enum/data naming

Pseudocode only:

```cpp
enum class EJGWorldValidationCategory : uint8
{
    TerrainForm,
    TerrainContinuity,
    BiomeMask,
    PCGOutput,
    TraversalResistance,
    RouteClass,
    NoMapReadability,
    WeatherNightImpact,
    WorldPartitionContinuity,
    PackageSmoke,
    RepoHygiene
};

enum class EJGTerrainLandformClass : uint8
{
    Coast,
    Creek,
    Ridge,
    Saddle,
    Basin,
    Gorge,
    Lowland,
    CanopyGap,
    SlopeBreak
};

enum class EJGTraversalClass : uint8
{
    Open,
    SlowVegetation,
    WetSlow,
    SteepSlow,
    Scramble,
    Blocked,
    FalseAffordance
};

enum class EJGRouteClass : uint8
{
    CoastRoute,
    CreekRoute,
    RidgeRoute,
    SaddlePass,
    BasinExit,
    GorgeRoute,
    LowlandPush,
    CanopyGapRoute,
    SlopeBreakTraverse,
    BlockedRoute,
    FalseAffordance
};

enum class EJGBiomeClass : uint8
{
    Coast,
    RiparianCreek,
    LowlandRainforest,
    RidgeForest,
    BasinWetland,
    Gorge,
    CanopyGap,
    DisturbedObservationArea
};
```

### Debug view implementation principles

Use:

- Debug materials for terrain and masks.
- Material Parameter Collections for selected debug channel where useful.
- Editor Utility Widgets for toggling validation views.
- Validation-only camera bookmarks.
- PCG debug rendering and node inspection for PCG point attributes.
- Data Validation rules for assets that must not leak into runtime.
- World Partition editor/debug views for cell and streaming continuity.
- Visual Logger only for development/editor spatial debugging where route probes, movement samples, or streaming events need scrub-friendly review.

UE Visual Logger captures actor state, log messages, and debug shapes for live or after-the-fact review in editor; this is useful for spatial problems such as movement and routes, but Visual Logger files should be treated as generated local artifacts, not committed source.

### World Partition guidance

World Partition validation should check:

- Cells load as the player approaches.
- Cells unload without affecting visible nearby continuity.
- Terrain, PCG output, traversal, and route classes remain consistent at cell edges.
- Validation routes cross at least one cell boundary when the PR touches large-world content.
- HLOD, navigation data, foliage grid, RVT, or PCG World Partition builders are used only when relevant to the PR.

Epic documents World Partition builder commandlets for HLODs, foliage, navigation data, PCG, RVT, and related builder operations. Use these as project needs mature, not as mandatory overhead for every early PR.

### PCG guidance

PCG validation should include:

- Debug rendering at meaningful graph stages.
- Inspection of point attributes for sampled nodes.
- Sanity checks for expected density or attribute ranges where appropriate.
- Stable seed recording when generation affects screenshots or route evidence.
- Clear distinction between authored source assets and generated/transient output.

Useful PCG attributes to expose in debug/evidence:

```text
BiomeClass
BiomeMaskWeight
Slope
Wetness
CanopyDensity
TraversalResistance
TraversalClass
RouteClass
SpawnDensity
ExclusionReason
ValidationAreaID
```

### What to implement now

Implement now:

1. Evidence category names.
2. Debug view naming convention.
3. First-person validation viewpoint list.
4. Markdown validation note template.
5. Repo hygiene ignore rules.
6. One validation map or validation area.
7. One validation route crossing multiple landform/biome/traversal cases.
8. Basic debug overlays:
   - `DBG_TerrainSlope`
   - `DBG_BiomeDominantClass`
   - `DBG_BiomeMaskWeights`
   - `DBG_TraversalResistance`
   - `DBG_RouteClass`
   - `DBG_WorldPartitionCellState`
9. Linux Development package smoke note.
10. No-map guard check.

Defer:

1. Automated screenshot comparison.
2. Full route graph scoring.
3. Full biome statistical reports.
4. Full weather/night readability metrics.
5. World Partition builder automation for every map.
6. CI artifact upload redesign.
7. Shipping-package matrix.
8. Long soak tests.
9. Player study instrumentation.
10. Advanced replay capture.

## Per-PR Validation Checklist

Each world-system PR should include a Markdown validation note with the relevant checklist items.

### Universal checklist

```text
Universal world validation:
[ ] Scope of world-system change is described.
[ ] Evidence categories touched are listed.
[ ] First-person readability evidence is included where visual/playability changed.
[ ] Debug evidence is included where data/masks/classes changed.
[ ] Package/runtime smoke was run if runtime behavior changed.
[ ] No player-facing map/compass/GPS/objective marker/HUD clock/orientation UI was added.
[ ] No debug view is accessible in normal player flow.
[ ] Generated screenshots/videos/logs/builds/caches were not committed.
[ ] git diff --stat was reviewed.
[ ] Known limitations and deferred work are listed.
```

### Terrain PR checklist

```text
Terrain validation:
[ ] `WV_TerrainForm` evidence included.
[ ] `DBG_TerrainHeightBands` or equivalent included if height/form changed.
[ ] `DBG_TerrainSlope` included if slope/traversal changed.
[ ] Component seam or boundary continuity checked.
[ ] First-person screenshots include at least one affected landform.
[ ] Collision/traversal mismatch checked from player view.
```

### Biome/PCG PR checklist

```text
Biome/PCG validation:
[ ] `WV_BiomeMask` evidence included.
[ ] `WV_PCGOutput` evidence included if generated assets changed.
[ ] Dominant biome and mask weights checked.
[ ] Transition bands checked.
[ ] PCG density/class output checked.
[ ] PCG exclusion zones checked for creeks, slopes, route corridors, and blocked areas.
[ ] Seed or generation settings recorded if relevant.
```

### Traversal PR checklist

```text
Traversal validation:
[ ] `WV_TraversalResistance` evidence included.
[ ] `WV_RouteClass` evidence included if routes changed.
[ ] Traversal class matches visible affordance.
[ ] Slow/blocked cases have visible cause.
[ ] At least one false affordance case checked if changed.
[ ] At least one player route was walked in runtime or PIE.
[ ] No invisible blocker introduced.
```

### Weather/night PR checklist

```text
Weather/night validation:
[ ] `WV_WeatherNightImpact` evidence included.
[ ] Matched clear/day and adverse/night screenshots included.
[ ] Route remains readable without UI aids.
[ ] Wetness changes traversal only where visually justified.
[ ] Darkness/rain/fog does not hide all route cues.
[ ] No explicit time readout or orientation helper was added.
```

### World Partition/streaming PR checklist

```text
World Partition validation:
[ ] `WV_WorldPartitionContinuity` evidence included.
[ ] Cell boundary crossing checked.
[ ] Terrain seam checked.
[ ] Biome/PCG continuity checked where relevant.
[ ] Traversal continuity checked where relevant.
[ ] Runtime package or standalone pass checked if streaming behavior changed.
[ ] Missing actor/foliage/material/collision issue checked.
```

### Package/smoke PR checklist

```text
Package smoke:
[ ] Build configuration recorded.
[ ] Platform recorded.
[ ] Map recorded.
[ ] Route/checkpoints recorded.
[ ] Player spawn checked.
[ ] Debug exposure checked.
[ ] No-map guard checked.
[ ] Critical runtime issue result recorded.
```

## Review-Friendly Evidence Note Template

Use this template in PR descriptions or a committed Markdown validation note such as:

```text
Docs/Validation/PR005_world_validation_evidence.md
```

Template:

```markdown
# Validation Evidence — PR <number>: <short title>

## Scope

Changed systems:
- <Terrain / biome / traversal / route / weather / streaming / package / docs>

Evidence categories:
- `WV_<Category>`
- `WV_<Category>`

Commit tested:
- `<commit-sha>`

UE version:
- `<UE version>`

Platform:
- `<Linux editor / Linux Development package / other>`

Map / validation area:
- `<map name>`
- `<validation area IDs>`

Generation seed/settings:
- `<seed/settings or Not applicable>`

## Evidence Manifest

Generated artifacts are not committed.

| Artifact ID | Category | Location / Upload ID | What It Proves | Result |
|---|---|---|---|---|
| `<artifact-name>` | `WV_TerrainForm` | `<external location or artifact ID>` | `<short claim>` | PASS/FAIL |
| `<artifact-name>` | `WV_NoMapReadability` | `<external location or artifact ID>` | `<short claim>` | PASS/FAIL |

## Editor Validation

Checks run:
- [ ] `DBG_TerrainHeightBands`
- [ ] `DBG_TerrainSlope`
- [ ] `DBG_BiomeDominantClass`
- [ ] `DBG_BiomeMaskWeights`
- [ ] `DBG_PCGDensity`
- [ ] `DBG_TraversalResistance`
- [ ] `DBG_RouteClass`
- [ ] `DBG_WorldPartitionCellState`

Findings:
- PASS:
  - `<specific pass>`
- FAIL:
  - `<specific fail or None>`
- Notes:
  - `<short notes>`

## First-Person Readability Validation

HUD/debug disabled:
- YES / NO

Captured viewpoints:
- `<FPV_Coast_...>`
- `<FPV_Creek_...>`
- `<FPV_Ridge_...>`

Readability result:
- PASS / FAIL / PARTIAL

Notes:
- `<what was readable from player eye height>`
- `<what remains ambiguous>`

## Package / Runtime Smoke

Required for this PR:
- YES / NO

Build:
- `<Linux Development package / Standalone / PIE only>`

Route:
- `<ValidationRouteID>`

Result:
- PASS / FAIL / NOT RUN

Observed issues:
- `<None or issue summary>`

Debug/player-facing guard:
- Map/minimap exposed: YES / NO
- Compass/bearing/GPS exposed: YES / NO
- Objective marker exposed: YES / NO
- HUD clock/time readout exposed: YES / NO
- Orientation confidence/commentary exposed: YES / NO
- Developer debug accessible in normal play: YES / NO

## Repo Hygiene

Generated output committed:
- Screenshots: NO
- Videos: NO
- Logs: NO
- Packaged builds: NO
- Saved/Intermediate/DDC: NO
- Generated cache/editor junk: NO

Committed file categories:
- `<Source>`
- `<Config>`
- `<Docs>`
- `<Validation template>`

Diff/stat reviewed:
- YES / NO

## Deferred Work

- `<Deferred item with reason>`
- `<Deferred item with reason>`

## Reviewer Notes

- `<short, direct notes for CodeRabbit/human review>`
- `<known limitation>`
- `<specific files worth reviewing>`
```

### CodeRabbit-friendly structure rules

- Keep evidence notes short and headed.
- Use tables for artifact manifests.
- Do not paste huge logs.
- Do not attach binaries to the repo.
- Do not include machine-only paths as the only artifact reference.
- State PASS/FAIL/PARTIAL explicitly.
- State whether package smoke was required and whether it ran.
- State no-map guard result explicitly.
- Put deferred work in its own section so reviewers do not confuse it with missed acceptance criteria.

## Risks, Pitfalls, And Forbidden Drift

### Validation risks

1. **Top-down false confidence**
   - A route may look readable in a debug map but not from first-person jungle density.
   - Mitigation: require first-person approach/decision/confirmation screenshots.

2. **Debug overlay becomes design crutch**
   - Developers may tune routes until the overlay is clean while the player view remains unreadable.
   - Mitigation: every route-class change needs HUD-free player-view evidence.

3. **Invisible traversal unfairness**
   - Collision or resistance may block a route that looks passable.
   - Mitigation: every blocked/slow class needs visible cause.

4. **Weather/night over-collapse**
   - Rain, fog, or darkness may erase all readable cues.
   - Mitigation: matched day/adverse screenshots from the same viewpoints.

5. **World Partition edge bugs**
   - Biome, PCG, traversal, or collision data may break at streaming/component boundaries.
   - Mitigation: require seam/cell-boundary evidence for affected systems.

6. **Generated output source bloat**
   - Screenshots, package builds, logs, DDC, and Saved files can quickly bury source review.
   - Mitigation: ignore generated folders and commit only concise Markdown evidence notes.

7. **PCG nondeterminism**
   - Screenshots and evidence may not reproduce if seeds/settings are not recorded.
   - Mitigation: record seed, graph name, validation area, and commit hash.

8. **Editor-only success, package failure**
   - PCG/debug/editor systems may work in PIE but fail or disappear incorrectly in cooked builds.
   - Mitigation: runtime smoke package for runtime-affecting PRs.

### Forbidden drift

Do not introduce:

- Player-facing map.
- Minimap.
- Compass.
- Bearing number/readout.
- GPS.
- Objective marker navigation.
- HUD clock.
- Explicit time readout.
- Orientation confidence UI.
- Hidden orientation meter.
- Character comments that tell the player whether they are oriented.
- Modern navigation UI disguised as survival tools.
- Combat/gore/chase/monster framing.
- Fantasy magic explanation for world changes.
- Validation that requires committed screenshots/videos/build outputs/log dumps.
- Validation that depends only on private local paths.
- Broad CI/CD redesign unrelated to this evidence workflow.
- Debug assets accessible through normal player input.
- First-person screenshots captured from editor flycam and presented as player evidence.

## PR-Ready Recommendations

### Implement immediately

1. **Add this evidence taxonomy**
   - Use `WV_*` evidence category IDs.
   - Use `DBG_*` debug view IDs.
   - Use `FPV_*` screenshot IDs.

2. **Create a Markdown validation note template**
   - Place it under project docs.
   - Keep it source-controlled.
   - Require it for world-system PRs.

3. **Add repo hygiene rules**
   - Ignore local validation artifacts.
   - Ignore package output folders.
   - Ensure `Saved/`, `Intermediate/`, and `DerivedDataCache/` remain untracked.

4. **Implement core developer-only debug views**
   - `DBG_TerrainSlope`
   - `DBG_BiomeDominantClass`
   - `DBG_BiomeMaskWeights`
   - `DBG_TraversalResistance`
   - `DBG_RouteClass`
   - `DBG_WorldPartitionCellState`

5. **Define first-person validation camera anchors**
   - Coast.
   - Creek.
   - Ridge.
   - Saddle.
   - Basin.
   - Gorge.
   - Lowland.
   - Canopy gap.
   - Slope break.
   - False affordance.

6. **Create one validation route**
   - It should cross at least three landform/biome classes.
   - It should include at least one slow traversal area.
   - It should include at least one route decision point.
   - It should include one false affordance or blocked-route case.
   - It should cross a component seam or World Partition cell boundary if available.

7. **Require package smoke for runtime-affecting PRs**
   - Linux Development package is enough initially.
   - Record map, route, result, and no-debug-exposure result.

8. **Keep generated evidence external**
   - PR note should include a manifest.
   - Commit only the concise Markdown evidence note, not artifacts.

### Defer until the world systems stabilize

1. Automated screenshot comparison.
2. Full statistical biome report generation.
3. Full route graph analysis.
4. Long traversal soak tests.
5. Multi-platform package matrix.
6. Shipping package validation for every PR.
7. CI artifact publishing.
8. Visual regression baselines.
9. Formal player navigation study.
10. Advanced replay-based evidence capture.

### Minimum acceptance evidence by PR type

| PR Type | Minimum Evidence Required |
|---|---|
| Terrain form | `WV_TerrainForm`, terrain debug view, first-person landform screenshot, seam check. |
| Biome masks | `WV_BiomeMask`, dominant/mask debug views, transition screenshot, sample value note. |
| PCG output | `WV_PCGOutput`, PCG density/class debug view, exclusion check, seed/settings note. |
| Traversal | `WV_TraversalResistance`, traversal debug view, first-person affordance screenshot, walked route note. |
| Route classes | `WV_RouteClass`, route debug view, first-person decision screenshots. |
| Weather/night | `WV_WeatherNightImpact`, matched clear/adverse screenshots, no-map readability note. |
| Streaming | `WV_WorldPartitionContinuity`, cell/boundary debug evidence, cross-boundary runtime note. |
| Runtime/package | `WV_PackageSmoke`, build/map/route result, no-debug-exposure result. |
| Hygiene-only | `WV_RepoHygiene`, ignored output check, diff/stat summary. |

## Sources

- [S1] Epic Games. “World Partition in Unreal Engine.”
  URL: https://dev.epicgames.com/documentation/unreal-engine/world-partition-in-unreal-engine
  Used for: World Partition grid cells, streaming sources, large-world streaming model.

- [S2] Epic Games. “Landscape Technical Guide in Unreal Engine.”
  URL: https://dev.epicgames.com/documentation/unreal-engine/landscape-technical-guide-in-unreal-engine
  Used for: Landscape components, section/LOD implications, component visualization, layer debug visualizer, heightmap sizing.

- [S3] Epic Games. “Procedural Content Generation Overview.”
  URL: https://dev.epicgames.com/documentation/unreal-engine/procedural-content-generation-overview
  Used for: PCG graph workflow, debug rendering, node enable/disable, node inspection, point attributes.

- [S4] Epic Games. “Procedural Content Generation Framework Node Reference.”
  URL: https://dev.epicgames.com/documentation/unreal-engine/procedural-content-generation-framework-node-reference-in-unreal-engine
  Used for: PCG Debug node, Sanity Check Point Data, Print String behavior, editor/non-editor debug distinction.

- [S5] Epic Games. “Build Operations: Cook, Package, Deploy, and Run.”
  URL: https://dev.epicgames.com/documentation/unreal-engine/build-operations-cooking-packaging-deploying-and-running-projects-in-unreal-engine
  Used for: UAT/BuildCookRun stages: build, cook, stage, package, deploy, run.

- [S6] Epic Games. “Data Validation in Unreal Engine.”
  URL: https://dev.epicgames.com/documentation/unreal-engine/data-validation-in-unreal-engine
  Used for: editor and command-line asset validation, validation rules, CI-compatible commandlet usage.

- [S7] Epic Games. “Automation Test Framework in Unreal Engine.”
  URL: https://dev.epicgames.com/documentation/unreal-engine/automation-test-framework-in-unreal-engine
  Used for: smoke test meaning, automation categories, screenshot comparison reference.

- [S8] Epic Games. “Visual Logger in Unreal Engine.”
  URL: https://dev.epicgames.com/documentation/unreal-engine/visual-logger-in-unreal-engine
  Used for: actor state capture, debug shapes, log review, spatial gameplay debugging.

- [S9] Epic Games. “World Partition Builder Commandlet Reference.”
  URL: https://dev.epicgames.com/documentation/unreal-engine/world-partition-builder-commandlet-reference
  Used for: HLOD, foliage, navigation data, PCG, RVT, and other World Partition builder commandlets.

- [S10] Epic Games. “Using Derived Data Cache in Unreal Engine.”
  URL: https://dev.epicgames.com/documentation/unreal-engine/using-derived-data-cache-in-unreal-engine
  Used for: DDC as large, regenerable derived data that should not be checked into source control.

- [S11] GitHub. “UnrealEngine.gitignore.”
  URL: https://github.com/github/gitignore/blob/main/UnrealEngine.gitignore
  Used for: common Unreal ignore patterns including `Saved/*`, `Intermediate/*`, `DerivedDataCache/*`, binaries, generated project files, and build-output handling.
