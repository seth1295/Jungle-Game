# Full-Size Terrain In-Game Shell Research

## Executive Summary

Jungle Game needs one narrow PR that turns the deterministic full-size terrain source into a rough, visible, packaged UE5 world shell. The goal is not final art, final ecology, or a navigational user interface. The goal is a packaged Linux build that clearly loads a full-size island/world state instead of the old Cell 0 slice.

The recommended implementation is a dedicated World Partition map containing a UE Landscape imported from the deterministic terrain source, a simple natural ocean/island-boundary placeholder, a minimal shell terrain material, developer-only review spawn anchors, and a packaged-build screenshot/log validation path. UE5 World Partition is appropriate because it stores the world in a single persistent level while subdividing it into streamable grid cells loaded around streaming sources such as the player.[^ue-world-partition] UE Landscape is appropriate because it is designed for large outdoor terrains, integrates with World Partition, supports large heightmaps, and is substantially more memory-efficient than equivalent static mesh terrain at the same vertex density.[^ue-landscape-overview]

The PR should start with **8 developer-only review points**:

1. `Review_Coast_North` — visible shoreline and ocean boundary.
2. `Review_Coast_CreekMouth` — creek/valley meeting the sea.
3. `Review_Ridge_Spine` — long ridge orientation and sky-window check.
4. `Review_Creek_Valley` — navigable low valley/creek corridor.
5. `Review_Basin_Lowland` — basin or flat lowland readability.
6. `Review_Highland_Shoulder` — transition from jungle lowland toward high ground.
7. `Review_Mountain_Lookout` — high point with visible world scale.
8. `Review_Cell0_CandidateOnly` — future placement reference, not the default spawn and not a Cell 0 level load.

The validation proof should be concrete: packaged Linux launch logs must print map name, terrain-source hash, landscape resolution, world bounds in meters, sea level, chosen player start, and review point count. Screenshot output must include one screenshot per review point, with filenames containing the review point name and terrain-source hash. This directly addresses the risk that the packaged game still looks like Cell 0 or accidentally spawns into empty sky/ocean.

## Assumptions And Constraints

### Project assumptions

- The project already has deterministic source-level contracts for terrain, biome masks, traversal categories, creeks/riparian systems, ridges/slopes/canopy orientation, and validation evidence.
- The visible packaged build still resembles an older Cell 0 playable slice.
- The next implementation PR must make the actual loaded UE world/level/landscape state visible in game.
- “Map state” means the loaded UE map/level/landscape state, not a player-facing map interface.
- The deterministic terrain source is already available or can be produced by existing project tooling before the PR imports or realizes it in UE.

### Hard constraints

The PR must not add any player-facing navigation aid. It must not include a map, minimap, compass, bearing number, GPS marker, objective marker, objective arrow, HUD clock, explicit time display, orientation confidence meter, hidden orientation meter, or character comments that tell the player whether they are oriented.

The PR must not attempt final jungle art/content density. It should avoid asset-pack dumping, broad binary churn, and any terrain shell that is visually indistinguishable from the old Cell 0 slice.

### Engine/version assumptions

- Treat UE documentation cited here as UE5-era guidance. The cited official pages are current UE5.8 docs at time of research, but the implementation PR should verify the exact project engine version before applying version-sensitive settings.
- Use standard UE Landscape and World Partition first. Do not introduce experimental terrain tech for this PR unless the existing project already depends on it.
- Use Linux-first validation: a packaged Linux build must launch, load the shell map, write logs, and generate screenshots.

## UE5 Landscape / World Partition Realization Strategy

### Recommended map shape

Create a dedicated map such as:

```text
/Game/Maps/L_WorldShell
```

This map should be distinct from the old Cell 0 map and should be the packaged review/default map for this PR. The map should contain:

- one imported or generated UE Landscape representing the full deterministic world extent;
- a simple placeholder ocean/natural boundary plane or water body;
- developer-only review point actors;
- one safe default player start;
- minimal shell lighting/skydome/atmosphere only as needed for visibility;
- no final foliage fill, no final prop dressing, and no player-facing navigation UI.

World Partition should be enabled for the full-size map. Epic describes World Partition as an automatic data-management and distance-based streaming system for large worlds; it stores the world in a single persistent level, separates it into grid cells, and loads/unloads cells around streaming sources such as the player.[^ue-world-partition] That model fits the shell because the PR needs a single reviewable full-size world state without reintroducing old sublevel/cell assumptions.

### Landscape sizing policy

Use UE recommended Landscape dimensions rather than arbitrary heightmap sizes. Epic’s Landscape Technical Guide lists recommended sizes that maximize area while minimizing component count, including `8129 x 8129`, `4033 x 4033`, `2017 x 2017`, and smaller sizes.[^ue-landscape-technical]

Recommended policy for this PR:

- **Preferred if source/detail budget allows:** `8129 x 8129` height samples.
  - At 1 meter per vertex, this gives roughly an 8.128 km square terrain extent.
  - At 2 meters per vertex, this gives roughly a 16.256 km square terrain extent with lower ground sampling density.
- **Safer first-shell fallback:** `4033 x 4033` height samples.
  - At 1 meter per vertex, this gives roughly a 4.032 km square extent.
  - At 2 meters per vertex, this gives roughly an 8.064 km square extent.
- **Do not shrink the world to Cell 0 scale.** If the actual world target is larger than the first-shell resolution supports at 1 m/vertex, preserve world extent by increasing XY scale rather than silently importing a small slice.

The implementation should log the actual chosen resolution and scale so reviewers can prove the shell is full-size.

Example log contract:

```text
[JungleWorldShell] Map=/Game/Maps/L_WorldShell
[JungleWorldShell] TerrainSource=FullSizeDeterministicTerrain v003 sha256=<hash>
[JungleWorldShell] LandscapeResolution=8129x8129 XYScaleCm=100 WorldExtentMeters=8128x8128
[JungleWorldShell] SeaLevelZCm=0 ReviewPointCount=8 DefaultStart=Review_Basin_Lowland
[JungleWorldShell] Cell0Status=NotLoaded FuturePlacementOnly=true
```

### Why not static mesh terrain for the shell

Do not realize the full-size shell as one or many static mesh terrain chunks unless a hard engine/project constraint forces it. Epic’s Landscape Overview states that Landscape is generally a better choice for large terrains than static meshes and notes that Landscapes use 4 bytes per vertex for vertex data while static meshes require substantially more vertex memory for equivalent data.[^ue-landscape-overview] Landscape also integrates natively with World Partition into streamable Landscape Proxies.[^ue-landscape-overview]

Static mesh terrain remains acceptable only for tiny helper meshes such as a simple ocean plane, far boundary cards, temporary creek ribbons, or debug-only visual aids that are not player-facing navigation tools.

## Recommended In-Game Shell For Jungle Game

### Shell content target

The shell should visually establish five macro regions from the deterministic terrain source:

- **Coast:** visible land/ocean relationship and natural island edge.
- **Ridge:** raised terrain spine visible from near and medium distance.
- **Creek/valley:** readable low channel or depression; water can be placeholder, but the landform must be visible.
- **Basin/lowland:** flatter lower terrain that can act as a safe starting area.
- **Highland/mountain:** high point or shoulder that proves vertical range and distant extent.

The shell must be rough but unmistakably full-size. A reviewer should be able to launch the packaged build, switch between review points, and immediately see that the loaded world is no longer just the old Cell 0 slice.

### Minimal visual language

Use a deliberately simple visual hierarchy:

- terrain base material: neutral, non-final material with slope/height variation;
- optional low-cost color bands from existing biome/elevation masks if they already exist;
- placeholder creek/riparian visibility: narrow darker/greener strip or simple spline/ribbon only where it helps confirm landform alignment;
- placeholder ocean: single plane/water body at sea level with plain material;
- lighting: enough directional/skylight/atmosphere for screenshots, not final mood or weather.

Do not add dense foliage to “sell” the jungle yet. This PR validates world realization, scale, boundaries, review points, and packaged visibility. Final jungle density can come later after the shell is landed and screenshot-proven.

### Map/default behavior

For this PR, the packaged review build should load `L_WorldShell` by default or provide a clearly documented packaged launch command that opens it. Because the research target says the packaged game should clearly stop looking like only the old Cell 0 slice, the cleanest implementation is to point the development packaged build’s game default map at `L_WorldShell` for the branch.

If changing the default game map conflicts with active project policy, then acceptance must require an explicit packaged launch command that names the shell map and logs the map load. A build that only shows `L_Cell0` by default is not sufficient for this PR.

## Terrain Import Or Runtime Realization Path

### Preferred path: deterministic source to cooked Landscape asset

Use a deterministic offline/editor-time realization path:

1. Generate or locate the full-size deterministic height source.
2. Normalize it to a UE-supported Landscape import format.
3. Import it into `L_WorldShell` as a UE Landscape.
4. Commit only the minimal resulting assets required for the packaged build.
5. Store a source manifest with hash, dimensions, sea level, scale, and review-point coordinates.

Epic’s Landscape documentation supports importing heightmaps as 16-bit grayscale PNG, 8-bit grayscale R8, 16-bit grayscale R16, and RAW with a JSON sidecar defining width, height, and bit depth.[^ue-landscape-technical] Unreal’s heightmap import/export docs also describe export options including single-file or tiled heightmap behavior, with tiled export requiring World Partition.[^ue-heightmap-import-export]

Recommended source bundle:

```text
/SourceData/Terrain/FullSizeShell/terrain_height.r16
/SourceData/Terrain/FullSizeShell/terrain_manifest.json
/SourceData/Terrain/FullSizeShell/terrain_masks_optional/
```

Recommended manifest sketch:

```json
{
  "id": "FullSizeTerrainShell003",
  "source_hash_sha256": "<hash>",
  "height_resolution": [8129, 8129],
  "xy_scale_cm": 100,
  "z_scale_cm": 100,
  "world_extent_m": [8128, 8128],
  "sea_level_z_cm": 0,
  "height_min_cm": -1200,
  "height_max_cm": 145000,
  "coordinate_origin": "centered_world_origin",
  "review_points": [
    {"name": "Review_Coast_North", "purpose": "shoreline boundary"},
    {"name": "Review_Coast_CreekMouth", "purpose": "creek to ocean transition"},
    {"name": "Review_Ridge_Spine", "purpose": "ridge orientation landform"},
    {"name": "Review_Creek_Valley", "purpose": "valley/creek corridor"},
    {"name": "Review_Basin_Lowland", "purpose": "safe lowland default"},
    {"name": "Review_Highland_Shoulder", "purpose": "lowland to highland transition"},
    {"name": "Review_Mountain_Lookout", "purpose": "vertical range and world scale"},
    {"name": "Review_Cell0_CandidateOnly", "purpose": "future Cell 0 placement reference only"}
  ]
}
```

### Runtime generation should be avoided for this PR

Do not make the packaged game generate the terrain at runtime as the primary implementation. Runtime generation increases launch risk, determinism risk, packaging risk, and validation complexity. The packaged build should load cooked map/landscape assets and prove that the actual UE world state is present.

Runtime helper code is acceptable only for:

- selecting a safe spawn from known anchors;
- running spawn sanity traces;
- printing world-shell evidence logs;
- moving the player/camera between developer-only review points;
- issuing screenshot commands.

### Avoiding unreviewable binary churn

UE map and asset changes are partly binary. Keep the binary surface intentionally small:

- one new shell map;
- one Landscape asset/proxy set created from deterministic source;
- one minimal material instance or material;
- one optional review-point Blueprint/C++ actor;
- one manifest data asset or JSON source manifest;
- no mass foliage instances;
- no broad asset-pack imports;
- no generated screenshots committed unless the repository already tracks validation evidence artifacts deliberately.

If the landscape import generates hundreds or thousands of asset files because of partition/proxy settings, the PR should stop and either adjust the import strategy or document why the generated file count is unavoidable and bounded. Reviewability is part of the acceptance gate.

## Ocean / Island Boundary Placeholder Strategy

### Objective

The boundary must make the shell read as an island or natural terrain extent, not as an endless test plane. It should be visible from coast and highland review points, but it must not become a final water-art task.

### Recommended first implementation

Use the lightest viable option:

- a large static mesh plane or simple grid plane centered on the island;
- Z at `sea_level_z_cm` from the terrain manifest;
- material named clearly as placeholder, such as `M_OceanBoundary_Placeholder`;
- no final wave simulation requirement;
- no heavy water plugin enablement unless the project already uses UE Water.

The plane should extend beyond the landscape bounds enough that screenshots from high points do not reveal a tiny square. Suggested first value:

```text
OceanPlaneExtent = LandscapeExtent * 1.5 to 2.0
```

The terrain should still define the playable landform. The ocean plane is a visual/natural boundary placeholder, not a navigation UI.

### If UE Water is already in use

If the project already has UE Water enabled and working in packaged Linux builds, a `WaterBodyOcean` or equivalent can be used as long as it remains placeholder-scoped. Do not enable new plugins, create new water systems, or fight water-body shoreline polish in this PR unless the simple plane fails a core acceptance criterion.

### Boundary acceptance

From `Review_Coast_North` and `Review_Coast_CreekMouth`, a screenshot must show land meeting ocean or water boundary. From `Review_Mountain_Lookout`, a screenshot should show enough surrounding low terrain/coast/water to prove macro scale. A screenshot that shows only sky or only ocean fails.

## Developer-Only Review Spawn Points

### Count and names

Start with exactly **8 review points**. More points add review overhead; fewer points risk missing key terrain categories. The first batch should use the following names and purposes:

| Name | Terrain role | Purpose |
|---|---:|---|
| `Review_Coast_North` | Coast | Confirms natural island boundary and visible large lateral terrain extent. |
| `Review_Coast_CreekMouth` | Coast + creek | Confirms creek/valley landform reaches the boundary. |
| `Review_Ridge_Spine` | Ridge | Confirms ridge/slope/canopy-orientation terrain source is represented spatially. |
| `Review_Creek_Valley` | Creek/valley | Confirms low channel readability and rough traversal corridor. |
| `Review_Basin_Lowland` | Basin/lowland | Safe default spawn candidate and lowland readability. |
| `Review_Highland_Shoulder` | Highland transition | Confirms vertical transition from lowland to upland. |
| `Review_Mountain_Lookout` | High point | Confirms height range and visible world scale. |
| `Review_Cell0_CandidateOnly` | Future placement reference | Confirms future Cell 0 relation without loading or recreating old Cell 0. |

### Actor representation

Use a simple developer-only actor or data record. Recommended fields:

```text
Name
Purpose
ExpectedTerrainClass
WorldLocation
YawDegreesForScreenshot
FallbackRadiusMeters
RequiresLandscapeHit=true
RequiresAboveSeaLevel=true/false depending on coast use
AllowDefaultPlayerStart=true/false
```

Only `Review_Basin_Lowland` or another explicitly safe lowland point should be allowed as the default player start. `Review_Cell0_CandidateOnly` must not be the default player start.

### Review-point access

Add a developer-only path that works in packaged Development builds, such as:

```text
Jungle.Shell.ListReviewPoints
Jungle.Shell.GoToReviewPoint Review_Creek_Valley
Jungle.Shell.CaptureReviewScreenshots
```

These commands must not surface as player-facing UI. They should be console/log/test hooks only. The implementation can use UE command-line arguments because UE executables accept command-line URL parameters and arguments, and Epic documents passing arguments from the command line as `<EXECUTABLE> [URL_PARAMETERS] [ARGUMENTS]`.[^ue-command-line]

## Safe Spawn And Traversal Sanity Rules

### Default spawn rules

The default player start should use a known safe review point, preferably `Review_Basin_Lowland`, after validation. Do not spawn at a random world origin unless the terrain manifest proves origin is safe.

At BeginPlay or validation startup, run these checks for the chosen default spawn:

1. **Landscape hit check:** trace downward from above the candidate point and require a landscape/terrain hit.
2. **Ocean-only rejection:** reject if the only hit is the ocean plane/water placeholder.
3. **Sky-only rejection:** reject if no ground hit is found within a bounded trace distance.
4. **Capsule clearance:** ensure the player capsule is placed above the terrain and not embedded.
5. **Slope check:** reject if local slope exceeds a rough walkable threshold for the player controller.
6. **Sea-level check:** for default spawn, require terrain Z to be above sea level by a small margin.
7. **Local movement probe:** test a small radial set of points around the spawn to ensure the player is not isolated on a spike.

If the default point fails, the build should try known fallback points in deterministic order and log the selected fallback. If all safe candidates fail, the packaged validation must fail loudly instead of silently spawning into sky/ocean.

Example failure log:

```text
[JungleWorldShell][ERROR] Safe spawn failed: Review_Basin_Lowland no landscape hit; rejected ocean-only fallback.
[JungleWorldShell][ERROR] No valid default PlayerStart found for L_WorldShell. Failing shell validation.
```

### Rough traversal sanity

This PR does not need final collision polish, but it must support basic walking at review points. Minimum sanity:

- landscape collision enabled;
- player capsule starts above terrain;
- no forced fall-through at default spawn;
- no immediate spawn death/fall loop;
- 20–50 meter walking area around default spawn;
- at least one creek/valley point can be walked for a short distance without getting trapped by placeholder geometry;
- ocean boundary does not need final swimming, but should not be the default spawn.

### What not to add

Do not add an orientation confidence meter, hidden compass, player comments, GPS-like coordinates, objective arrow, or any other tool that tells the player where they are. Developer commands and logs are acceptable only as validation/debug paths.

## Cell 0 Separation / Future Placement Notes

### Core rule

The full-size world shell and future Cell 0 placement must remain separate concepts. The shell PR should prove the full island/world exists. It should not attempt to finish, dress, or merge the old Cell 0 slice into the new world.

### Recommended separation

Use naming and asset boundaries that make the distinction explicit:

```text
/Game/Maps/L_WorldShell
/Game/Maps/Legacy/L_Cell0_OldSlice        # existing if applicable
/Game/WorldShell/DA_WorldShellManifest
/Game/WorldShell/ReviewPoints/*
```

`Review_Cell0_CandidateOnly` is only a future placement reference. It can be a coordinate or actor in the shell map, but it should not load Cell 0 content, should not copy Cell 0 geometry, and should not make Cell 0 the default experience.

### Evidence requirement

Packaged logs should state:

```text
[JungleWorldShell] Cell0Status=NotLoaded FuturePlacementOnly=true
```

This proves the reviewer is looking at the full-size shell, not the legacy Cell 0 map.

## Multi-Point Screenshot Validation

### Validation objective

Screenshots must show that the packaged Linux build can load the full-size shell and view representative terrain categories. The validation should be repeatable enough for PR review without relying on a human manually walking for 20 minutes.

### Screenshot set

Generate at least one screenshot from each review point:

```text
003_shell_01_Review_Coast_North_<hash>.png
003_shell_02_Review_Coast_CreekMouth_<hash>.png
003_shell_03_Review_Ridge_Spine_<hash>.png
003_shell_04_Review_Creek_Valley_<hash>.png
003_shell_05_Review_Basin_Lowland_<hash>.png
003_shell_06_Review_Highland_Shoulder_<hash>.png
003_shell_07_Review_Mountain_Lookout_<hash>.png
003_shell_08_Review_Cell0_CandidateOnly_<hash>.png
```

Screenshots should be saved under `Saved/Screenshots/Linux` or the platform-equivalent screenshot folder. Epic documents the standard `Shot` console command and the `HighResShot` console command, including filename and resolution parameters.[^ue-screenshots] The console commands reference also documents `HighResShot` usage with explicit resolution arguments.[^ue-console-commands]

### Recommended packaged validation command shape

The project should provide a command or automation path equivalent to:

```bash
./JungleGame.sh /Game/Maps/L_WorldShell -windowed -ResX=1600 -ResY=900 -log -ExecCmds="Jungle.Shell.CaptureReviewScreenshots,Quit"
```

Epic documents `-ExecCmds` as a command-line argument for executing console commands at startup.[^ue-execcmds] If this exact command sequencing is fragile in the project version, implement a single project console command such as `Jungle.Shell.CaptureReviewScreenshots` that internally handles teleport, wait frames, capture, and quit.

### Screenshot pass/fail rules

Pass:

- all 8 screenshots exist after packaged Linux run;
- filenames contain review point names and terrain hash;
- at least one screenshot shows coast/ocean boundary;
- at least one screenshot shows ridge/highland vertical structure;
- at least one screenshot shows creek/valley/basin landform;
- no screenshot is only sky, only ocean, black frame, or old Cell 0 view;
- logs identify the map as `L_WorldShell` and print full-size world bounds.

Fail:

- packaged build opens old Cell 0 by default without explicit shell launch proof;
- default spawn starts in sky/ocean or falls through;
- screenshots cannot be generated from packaged Linux build;
- screenshot evidence does not show large terrain extent;
- review points are editor-only and unavailable to packaged Development validation.

## Performance And Streaming Sanity Checks

### Minimum checks

The PR should not chase optimization, but it must prove the shell is not catastrophically broken.

Required packaged Development checks:

- launch does not crash;
- map load completes;
- player can stand at default spawn;
- review point teleport/capture command completes;
- logs show world bounds and review point count;
- screenshots capture terrain rather than blank frames;
- no obvious runaway asset loading loop;
- no massive final foliage draw cost introduced.

### World Partition sanity

Because World Partition streams grid cells based on streaming sources such as the player,[^ue-world-partition] the validation should include at least one highland or distant point after loading so streaming is exercised beyond the default spawn. For this shell PR, the goal is not perfect streaming tuning; the goal is to avoid an implementation that only works because every relevant actor is accidentally always loaded in editor but missing in packaged runtime.

Suggested log evidence:

```text
[JungleWorldShell] RuntimeWorldPartition=true
[JungleWorldShell] CurrentReviewPoint=Review_Mountain_Lookout
[JungleWorldShell] VisibleTerrain=true LandscapeHit=true
[JungleWorldShell] ScreenshotSaved=<path>
```

### Performance thresholds

Use practical soft thresholds rather than hard final-game targets:

- packaged Development build should remain interactive at default spawn on the target development machine;
- screenshot command should complete without timeout;
- no broad asset pack or dense foliage should be present;
- if the landscape resolution is too heavy, prefer a lower sample density that preserves world extent over shrinking the island into a Cell 0-scale map.

### Linux packaging relevance

Packaging matters because packaging compiles code and cooks assets into a standalone target-platform build.[^ue-packaging] Epic’s Linux quickstart documents both Editor packaging via Linux > Package Project and command-line BuildCookRun through `RunUAT`.[^ue-linux-quickstart] This PR’s acceptance should therefore require a packaged Linux run, not only PIE/editor screenshots.

## Implementation-Sized PR Scope

### In scope

A narrow implementation PR should include only:

1. Dedicated full-size world shell map.
2. UE Landscape realized from deterministic full-size terrain source.
3. Minimal placeholder terrain material.
4. Simple ocean/island-boundary placeholder.
5. Eight developer-only review points.
6. Safe default player start and spawn validation.
7. Packaged Linux screenshot/log validation command.
8. Brief developer documentation for launching and validating the shell.

### Out of scope

Do not include:

- final jungle foliage density;
- final creek/water simulation;
- final biome art pass;
- AI, survival mechanics, inventory, crafting, or animals;
- player-facing map/navigation UI;
- old Cell 0 dressing migration;
- broad asset packs;
- massive generated screenshots committed to source unless explicitly required by project evidence policy.

### Suggested PR title

```text
PR5 003: Full-size terrain shell visible in packaged build
```

### Suggested PR description checklist

Use three short PR-body sections without adding unrelated implementation narrative:

- What changed:
  - Added `L_WorldShell` full-size terrain shell map.
  - Imported deterministic full-size terrain source as UE Landscape.
  - Added placeholder ocean boundary.
  - Added 8 developer-only review points.
  - Added packaged Linux screenshot/log validation path.
- Evidence:
  - Packaged Linux launch log showing map, source hash, resolution, world bounds, sea level, and review point count.
  - 8 screenshots from review points.
  - Default safe spawn validation pass.
  - Diff/stat inspected for generated asset churn.
- Explicitly not included:
  - No player-facing map, compass, objective marker, or navigation UI.
  - No final foliage/art density.
  - No Cell 0 placement/migration beyond future placement marker.

## Files And Areas Likely Touched

Exact paths will depend on the current repository layout, but likely touched areas are:

```text
Content/Maps/L_WorldShell.umap
Content/WorldShell/DA_WorldShellManifest.uasset
Content/WorldShell/M_TerrainShell_Placeholder.uasset
Content/WorldShell/M_OceanBoundary_Placeholder.uasset
Content/WorldShell/BP_WorldShellReviewPoint.uasset
Content/WorldShell/BP_WorldShellOceanBoundary.uasset
Source/JungleGame/Public/WorldShell/JungleWorldShellSubsystem.h
Source/JungleGame/Private/WorldShell/JungleWorldShellSubsystem.cpp
Source/JungleGame/Private/WorldShell/JungleWorldShellConsoleCommands.cpp
Config/DefaultEngine.ini
Docs/Validation/WorldShell003.md
SourceData/Terrain/FullSizeShell/terrain_manifest.json
SourceData/Terrain/FullSizeShell/terrain_height.r16
```

### Config touch caution

If `DefaultEngine.ini` changes the default map, the PR must clearly call it out. That change is likely desirable for the review branch, because acceptance depends on the packaged game no longer presenting the old Cell 0 slice. Still, it should be isolated and easy to revert or adjust when the next placement PR lands.

### Binary churn caution

The PR must include a diff/stat review. If the import produces a very large number of World Partition actor files or large landscape proxy assets, the PR description should explain:

- why those files are required;
- which deterministic source produced them;
- how to regenerate them;
- why no unrelated generated content is included.

## Acceptance Criteria

The implementation PR is acceptable only if all of the following are true:

1. **Packaged Linux build loads the shell.** A packaged Linux build launches `L_WorldShell` by default or through a documented explicit launch command.
2. **The shell is visually full-size.** Screenshots clearly show large terrain extent and cannot be mistaken for the old Cell 0 slice.
3. **Terrain is deterministic.** Logs and/or manifest include terrain source ID, hash, resolution, scale, bounds, and sea level.
4. **Ocean/boundary exists.** At least one coast screenshot shows land meeting an ocean or natural island-boundary placeholder.
5. **Eight review points exist.** All required review points are present, named, and usable in packaged Development validation.
6. **Default spawn is safe.** The player does not spawn in empty sky, ocean-only space, below terrain, or inside collision.
7. **Representative landforms are visible.** Coast, ridge, creek/valley, basin/lowland, and highland/mountain views are represented in screenshots.
8. **Cell 0 remains separate.** Logs or docs make clear that Cell 0 is not loaded and only has a future placement reference.
9. **No forbidden navigation aids.** No player-facing map, minimap, compass, bearing, GPS, objective marker, orientation meter, HUD clock, or orientation-telling character comments are added.
10. **No final-art scope creep.** No dense final foliage pass, broad asset-pack dump, or large unrelated art import is included.
11. **Reviewable asset churn.** Binary/generated changes are bounded and explained.
12. **Validation evidence exists.** Logs and 8 screenshots are produced from a packaged Linux run.

## Risks And Stop Conditions

### Main risks

| Risk | Why it matters | Mitigation |
|---|---|---|
| Landscape import silently uses wrong scale | The world may look full-size in editor but represent a shrunken slice. | Log resolution, XY scale, Z scale, and world extent in meters. |
| Spawn lands in sky/ocean | Packaged review appears broken or misleading. | Require landscape hit, sea-level, slope, and capsule clearance checks. |
| Shell still resembles Cell 0 | PR fails the core target. | Use full-size map, ocean boundary, highland/coast/ridge screenshots, and Cell0Status logs. |
| Binary churn becomes unreviewable | Review cannot prove scope or source determinism. | Limit assets, include manifest/hash, avoid foliage packs, inspect diff/stat. |
| World Partition actors missing in packaged build | Editor works but packaged build fails. | Validate in packaged Linux build, not PIE only. |
| Placeholder ocean hides terrain | Screenshots become ocean-only or boundary masks scale. | Use review-point screenshot pass/fail rules. |
| Terrain dimensions exceed machine budget | Packaged build becomes non-interactive. | Preserve world extent with lower sample density instead of overcommitting resolution. |

### Stop conditions

Stop and report instead of merging if:

- the deterministic full-size terrain source cannot be identified or hashed;
- the only available terrain is the old Cell 0 slice;
- packaged Linux build cannot load the shell map;
- default spawn cannot be validated against actual landscape collision;
- screenshots from review points are blank, sky-only, ocean-only, or Cell 0-only;
- the PR requires broad generated binary churn that cannot be explained from deterministic source;
- achieving the shell requires adding forbidden player-facing navigation aids;
- the implementation begins turning into final art/foliage/content placement instead of shell realization.

## Rejected Or Forbidden Approaches

Reject the following approaches for this PR:

- **Player-facing map or minimap:** directly forbidden and contrary to terrain-first navigation.
- **Compass, bearing, GPS, objective marker, or objective arrow:** directly forbidden.
- **HUD clock or explicit time display:** directly forbidden.
- **Orientation confidence UI or hidden orientation meter:** directly forbidden, even if framed as debug-assisted player feedback.
- **Character comments that tell the player they are oriented/disoriented:** directly forbidden.
- **Final jungle dressing pass:** not needed to prove world-shell load state.
- **Asset-pack dumping:** creates broad unreviewable churn and obscures terrain validation.
- **Static mesh full-world terrain as first choice:** weaker fit for UE large terrain than Landscape and risks memory/review issues.
- **Runtime-only terrain generation in packaged build:** raises determinism and validation risk for this PR.
- **Old Cell 0 as default packaged experience:** fails the research target.
- **A visually indistinguishable larger empty Cell 0 clone:** fails the goal of a clear full-size island/world shell.

## Sources And References

[^ue-world-partition]: Epic Games, “World Partition in Unreal Engine,” UE5.8 documentation. Key points used: World Partition stores a world in a single persistent level, divides it into grid cells, and streams cells based on streaming sources such as the player. https://dev.epicgames.com/documentation/unreal-engine/world-partition-in-unreal-engine

[^ue-landscape-technical]: Epic Games, “Landscape Technical Guide in Unreal Engine,” UE5.8 documentation. Key points used: recommended Landscape sizes and supported import formats including 16-bit grayscale PNG, R8, R16, and RAW with JSON sidecar. https://dev.epicgames.com/documentation/unreal-engine/landscape-technical-guide-in-unreal-engine

[^ue-landscape-overview]: Epic Games, “Landscape Overview,” UE5.8 documentation. Key points used: Landscape supports large terrain sizes, integrates natively with World Partition, uses Landscape Proxies, and is memory-efficient compared with static mesh terrain. https://dev.epicgames.com/documentation/unreal-engine/landscape-overview

[^ue-heightmap-import-export]: Epic Games, “Importing and Exporting Landscape Heightmaps in Unreal Engine,” UE5.8 documentation. Key points used: heightmap import/export behavior, Y-axis flip, import resolution, and World Partition tiled export note. https://dev.epicgames.com/documentation/unreal-engine/importing-and-exporting-landscape-heightmaps-in-unreal-engine

[^ue-packaging]: Epic Games, “Packaging Your Project,” UE5.8 documentation. Key points used: packaging converts a project into a standalone executable/application for target platforms and includes build, cook, stage, and package operations. https://dev.epicgames.com/documentation/unreal-engine/packaging-your-project

[^ue-linux-quickstart]: Epic Games, “Linux Development Quickstart for Unreal Engine,” UE5.8 documentation. Key points used: Linux project packaging through the Editor and command-line BuildCookRun through `RunUAT`. https://dev.epicgames.com/documentation/unreal-engine/linux-development-quickstart-for-unreal-engine

[^ue-screenshots]: Epic Games, “Taking Screenshots in Unreal Engine,” UE5.8 documentation. Key points used: `Shot`, `HighResShot`, screenshot save behavior, and filename/resolution parameters. https://dev.epicgames.com/documentation/unreal-engine/taking-screenshots-in-unreal-engine

[^ue-console-commands]: Epic Games, “Unreal Engine Console Commands Reference,” UE5.8 documentation. Key point used: `HighResShot` command usage with explicit resolution arguments. https://dev.epicgames.com/documentation/unreal-engine/unreal-engine-console-commands-reference

[^ue-command-line]: Epic Games, “Command-Line Arguments in Unreal Engine,” UE5.8 documentation. Key point used: executable command-line syntax with URL parameters and arguments. https://dev.epicgames.com/documentation/unreal-engine/command-line-arguments-in-unreal-engine

[^ue-execcmds]: Epic Games, “Unreal Engine Command-Line Arguments Reference,” UE5.8 documentation. Key point used: `-ExecCmds` executes specified console commands at startup. https://dev.epicgames.com/documentation/unreal-engine/unreal-engine-command-line-arguments-reference
