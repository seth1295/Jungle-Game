# Full-Size World Authority Research

## Executive Summary

Jungle Game should establish full-size world authority before any further full-world terrain work by making one project-owned UE level asset the single authoritative loaded world for editor review and packaged Linux startup.

**Recommended authoritative world asset:**

```text
/Game/JungleGame/Maps/WLD_JungleIsland_Full
```

This asset should be a project-owned World Partition persistent level. It may begin as a sparse authority shell, coarse terrain blockout, or migrated duplicate of an Unreal `Open World`/`Empty Open World` starting level, but it must not remain named or treated as a generic template map. From this point forward, all full-world terrain, biome, traversal, creek/riparian, ridge/slope/canopy, weather, ecology, and validation evidence should target this asset or clearly declared data owned by this asset.

The implementation PR should be narrow. It should not attempt to finish the island. It should establish:

- the authoritative full-size world asset path;
- default editor and packaged startup behavior;
- packaging/cook inclusion for the authoritative map;
- retirement or quarantine of template `OpenWorld` usage;
- a transitional status for Cell 0;
- a developer-only review spawn registry;
- logs and validation evidence proving the expected world loaded.

The key rule is simple: **Cell 0 and template OpenWorld content may remain useful review or migration material, but neither may define, fake, or launch as the full world.**

## Assumptions And Constraints

### Project assumptions

- Project: **Jungle Game**.
- Engine: UE5.
- Target platform priority: Linux-first editor/build workflow.
- Genre: first-person jungle survival-simulation.
- Fictional date/setting: around 1000 CE on a dense tropical island jungle.
- Existing PR5 contracts define terrain, biome, traversal, creek/riparian, ridge/slope/canopy, and validation vocabulary.
- The visible packaged build still resembles an older Cell 0 slice.
- The next implementation step is authority establishment, not full terrain completion.

### Navigation and UI constraints

The implementation must not add or imply any player-facing:

- map;
- minimap;
- compass;
- bearing readout;
- GPS marker;
- objective marker;
- objective arrow;
- HUD clock;
- explicit time display;
- orientation confidence meter;
- hidden orientation meter;
- character comments that tell the player whether they are oriented;
- modern navigation tools.

Developer-only editor/debug facilities are allowed only when explicitly isolated from player-facing runtime.

### Naming assumptions

The exact project content root may differ. If the repository already uses a stable root such as `/Game/JungleGame`, `/Game/Jungle`, or `/Game/Maps`, preserve the repository convention. If there is no established convention, use:

```text
/Game/JungleGame/Maps/WLD_JungleIsland_Full
```

Do not use:

```text
/Game/Maps/OpenWorld
/Game/OpenWorld
/Game/Maps/Cell0
/Game/Maps/TestMap
```

as the authoritative full-size world path.

## UE5 World / Map Authority Concepts

### Levels and default maps

In UE, a map/level is the loaded world asset that defines runtime world state. UE project settings expose **Game Default Map** and **Editor Startup Map** controls. Epic documents the Game Default Map as the level players load into, with Editor Startup Map available for editor workflow convenience. [UE-DefaultLevel] Epic's project settings reference also identifies `Game Default Map` as the map loaded by default when no other map is loaded, and `Editor Startup Map` as the map loaded when the editor starts if set. [UE-ProjectSettings]

For packaged builds, Epic states that the **Game Default Map** is the map that loads when a packaged project launches and that projects should set one before packaging. Without a Game Default Map, a blank project may launch into a black screen; template projects can mask this by supplying their own template default. [UE-Packaging]

### Startup map loading and command-line overrides

Epic's running documentation states that the engine attempts to load a default map at startup from project configuration unless overridden. [UE-Running] Epic also documents executable syntax where a URL parameter can choose a specific map on startup, for example passing a map path before flags. [UE-CommandLine]

For Jungle Game this means:

- the authoritative full-size map must be configured as the default;
- command-line map overrides may be used for developer review only;
- validation logs must record whether a command-line map override was present;
- packaged startup acceptance must be tested without a command-line map override.

### World Partition and open-world templates

World Partition is the UE5 system intended for large, streamable worlds. Epic documents the Open World default map type as a starting point for large open-world maps with World Partition, One File Per Actor, Data Layers, and HLOD enabled, and notes that the default sample map contains a sample 2 km x 2 km landscape and outdoor lighting setup. [UE-WorldPartition]

Epic also documents level templates including `Open World` and `Empty Open World`; `Open World` includes sample content and World Partition, while `Empty Open World` uses World Partition without content. [UE-WorkingLevels]

For Jungle Game this means template maps are acceptable as raw starting material, but **authority begins only when the project owns a named, intentional world asset**. A map still named `OpenWorld` is not an acceptable authority asset.

### PlayerStart and spawn control

Epic documents `Player Start` actors as runtime spawn locations. If no PlayerStart exists, the player starts at world coordinates `0,0,0`; multiple PlayerStarts can exist and can be selected through Blueprint or C++. [UE-PlayerStart]

For Jungle Game this supports developer-only review spawns, but these spawns must remain invisible to the player and must not become navigation UI.

### Packaging map inclusion

Epic's packaging settings include a list of maps to include in packaged builds when no other command-line map list is specified. [UE-PackagingSettings] For World Partition maps, Epic's World Partition documentation describes cooking World Partition maps through the cook workflow. [UE-WorldPartition]

For Jungle Game this means the authoritative world must be both the default startup map and explicitly included in packaging/cook configuration where the project already constrains cooked maps.

## Recommended World Authority Model For Jungle Game

### Authority definition

A world asset has **full-size world authority** only when all of these are true:

1. It is a project-owned level asset with a stable path.
2. It is the configured Game Default Map.
3. It is the configured Editor Startup Map, unless the project intentionally chooses a separate editor bootstrap level that immediately opens/loads the authoritative map.
4. It is included in packaged/cooked map configuration.
5. It contains or references a developer-only authority marker/data record.
6. Packaged Linux builds launch into it by default.
7. Logs prove the loaded world matches the expected authority record.
8. Legacy Cell 0 and template OpenWorld assets are not allowed to satisfy authority checks.

### Recommended authoritative asset

Use this as the default convention:

```text
/Game/JungleGame/Maps/WLD_JungleIsland_Full
```

On disk this corresponds to a `.umap` asset under the project `Content` directory:

```text
Content/JungleGame/Maps/WLD_JungleIsland_Full.umap
```

Recommended naming semantics:

| Element | Meaning |
|---|---|
| `WLD_` | persistent playable world / level authority asset |
| `JungleIsland` | project-specific island world identity |
| `Full` | full-size world state, not Cell 0, test map, or slice |
| no `OpenWorld` | avoids template ownership ambiguity |
| no `Cell0` | prevents legacy slice from pretending to be full world |

### Recommended authority marker

The map should contain or reference one developer-only authority marker. This can be a non-rendering actor, data asset, subsystem configuration, or lightweight Blueprint/C++ validation object.

Recommended schema sketch:

```text
WorldAuthorityId: JG_FULL_WORLD_001
WorldAssetPath: /Game/JungleGame/Maps/WLD_JungleIsland_Full
AuthorityPurpose: FullSizePlayableWorld
AuthorityStatus: EstablishedShell
AuthorityBatch: PR5_BATCH_002_PROMPT_001
LegacyCell0Policy: ReviewAnchorOnly
TemplateOpenWorldPolicy: RetiredAsRuntimeDefault
PlayerFacingNavigationUiAllowed: false
```

`AuthorityStatus` should begin as `EstablishedShell` or `AuthorityShell`, not `CompleteWorld`. The PR is proving ownership and startup behavior, not claiming finished terrain/ecology.

### Ownership rule

Only `WLD_JungleIsland_Full` may be treated as the authoritative full-size world. Any other level must be classified as one of:

- `LegacyValidationSlice`;
- `ReviewAnchor`;
- `TemplateSource`;
- `Prototype`;
- `MigrationSource`;
- `AutomatedTestMap`;
- `DeveloperOnlyMap`.

No other level may be called "the full map", "the world", "the island", or "the packaged default" in logs, comments, docs, PR description, or config.

## Default Map And Packaged Launch Strategy

### Config target

The implementation PR should update `Config/DefaultEngine.ini` through Project Settings or direct config edit so the default map settings point at the authoritative asset.

Recommended config sketch:

```ini
[/Script/EngineSettings.GameMapsSettings]
EditorStartupMap=/Game/JungleGame/Maps/WLD_JungleIsland_Full
GameDefaultMap=/Game/JungleGame/Maps/WLD_JungleIsland_Full
ServerDefaultMap=/Game/JungleGame/Maps/WLD_JungleIsland_Full
```

If the project uses a main menu entry map, that should be deferred unless it already exists and is intentionally the shipped entry. For this PR, the safest authority move is to launch directly into `WLD_JungleIsland_Full`.

### Packaging/cook inclusion

If the project uses an explicit map cook list, add the authoritative map.

Recommended config sketch:

```ini
[/Script/UnrealEd.ProjectPackagingSettings]
+MapsToCook=(FilePath="/Game/JungleGame/Maps/WLD_JungleIsland_Full")
```

If the project currently cooks all maps, this PR should still add a validation check that the authoritative map is cookable and appears in staged packaged output logs. It should not rely on "cook everything" as the only proof of authority.

### Packaged launch behavior

Packaged Linux builds should launch into the authoritative world when run without a map override:

```text
./JungleGame.sh
```

The validation run must not use:

```text
./JungleGame.sh /Game/JungleGame/Maps/WLD_JungleIsland_Full
```

as the primary proof, because that only proves the command line can force the map. Command-line startup is useful as a secondary diagnostic, not default launch authority.

### Editor startup behavior

The editor should open `WLD_JungleIsland_Full` by default unless there is a strong performance reason to use a bootstrap/editor utility level. If an editor bootstrap is used, it must log and open/load the authoritative map immediately, and acceptance criteria must still prove `WLD_JungleIsland_Full` is the map under review.

## Template OpenWorld / Legacy Slice Handling

### OpenWorld policy

The UE `Open World` template is a source template, not a project identity. Epic explicitly describes it as a starting point and notes that it includes sample terrain and lighting. [UE-WorldPartition]

Recommended handling:

1. If an existing map named `OpenWorld` is currently the default, duplicate or migrate it to:

   ```text
   /Game/JungleGame/Maps/WLD_JungleIsland_Full
   ```

2. Remove `OpenWorld` from:
   - `GameDefaultMap`;
   - `EditorStartupMap`;
   - `ServerDefaultMap`;
   - explicit `MapsToCook`, unless preserved as a developer-only comparison map.

3. Rename or classify the old asset as one of:

   ```text
   /Game/JungleGame/Maps/_Template/TMP_OpenWorld_Source
   /Game/JungleGame/Maps/_Deprecated/DEP_OpenWorld_TemplateSource
   ```

4. Fix redirectors after asset moves/renames.
5. Add validation that no authoritative config path contains `OpenWorld`.

### OpenWorld wrapping alternative

If the team cannot rename or move the existing OpenWorld asset in one PR because of binary asset churn, the PR may temporarily wrap it with an explicit authority record only if all these conditions are met:

- the asset is referenced by a project-owned path;
- the PR adds a stop condition requiring rename/migration in the next terrain PR;
- logs call it `TemplateSourceWrapped`, not final authority;
- the packaged Game Default Map does not remain `/Game/Maps/OpenWorld`.

This is a fallback only. Preferred implementation is to create or rename to `WLD_JungleIsland_Full`.

### Legacy slice policy

A visible old slice can be retained for review, but it must stop defining the shipped world. Any old slice should be renamed or documented as:

```text
/Game/JungleGame/Maps/Review/LV_Review_Cell0_Legacy
```

or, if the asset is too expensive to move immediately:

```text
Cell0Status: LegacyValidationSlice
Cell0RuntimeDefaultAllowed: false
Cell0MayDefineFullWorldScale: false
```

## Cell 0 Transition Strategy

Cell 0 should be treated as a **temporary review anchor and legacy validation slice**, not as the full world.

### Recommended status

```text
Cell0Status: ReviewAnchorOnly
Cell0Purpose: Preserve known playable/visual baseline while full world authority is established
Cell0MayLaunchByDefault: false
Cell0MayDefineIslandScale: false
Cell0MaySatisfyFullWorldValidation: false
FutureMigration: Place selected Cell0 content into authoritative world region after world scale/origin are stable
```

### Why not future placed location yet

Cell 0 can later become a placed location inside the island, but doing that now risks letting an old slice dictate full-world origin, scale, terrain style, traversal density, or ecology. The authority PR should avoid that. It should establish where the full world lives first, then future terrain work can decide whether Cell 0 content is migrated into a specific bay, creek valley, ridge approach, or inland camp region.

### How to prevent Cell 0 from faking the whole world

The PR should add explicit checks and evidence:

- `GameDefaultMap` is not Cell 0.
- `EditorStartupMap` is not Cell 0.
- Packaged launch logs identify `WLD_JungleIsland_Full`.
- Authority marker ID exists only in `WLD_JungleIsland_Full`.
- Cell 0 logs, if opened manually, report `AuthorityStatus=LegacyValidationSlice`.
- Validation scripts fail if Cell 0 satisfies `FullSizeWorldAuthority`.

### Review use allowed

Cell 0 can remain useful for:

- before/after visual comparison;
- player controller sanity checks;
- movement, foliage, lighting, and interaction regression checks;
- old packaged slice comparison;
- short-form PR review anchors.

Cell 0 must not be used as proof that the full-size world exists or launches.

## Developer-Only Review Spawn Registry

### Purpose

Developer-only review spawns should let reviewers enter representative regions of the authoritative world without adding player-facing navigation assistance. These spawns are test/review conveniences, not diegetic tools.

### Representation

Use one of these approaches:

1. `PlayerStart` actors with explicit developer-only tags.
2. A `DA_ReviewSpawnRegistry` data asset listing named transforms.
3. A lightweight world authority subsystem that resolves review spawn names in non-shipping builds.

Recommended minimum for the authority PR:

```text
/Game/JungleGame/Data/DA_ReviewSpawnRegistry_FullWorld
```

with entries:

```text
SP_Review_DefaultBeach_001
SP_Review_RidgeLine_001
SP_Review_CanopyEdge_001
SP_Review_WetValley_001
SP_Review_CreekCrossing_001
SP_Review_Cell0Comparison_001
```

Early placeholders are acceptable if the full terrain is not yet built, but their purpose must be clear. Coordinates may be temporary.

### Spawn naming rules

| Prefix | Meaning |
|---|---|
| `SP_Review_` | developer/reviewer spawn only |
| `SP_Game_` | real player start candidate |
| `SP_Test_` | automated/test-only spawn |
| `SP_Legacy_` | old slice or migration comparison spawn |

Avoid names that imply navigation hints to players, such as:

- `SP_NorthObjective`;
- `SP_GoToCreek`;
- `SP_FindCamp`;
- `SP_CorrectDirection`;
- `SP_MapCenter`.

### Runtime access rule

Review spawn selection may be available through:

```text
-JungleReviewSpawn=SP_Review_RidgeLine_001
```

or PIE/development-only settings.

It must not appear in:

- player HUD;
- pause menu;
- settings menu;
- map screen;
- minimap;
- compass;
- marker system;
- character dialogue;
- gameplay hints;
- save/load UI;
- shipping runtime UI.

### Shipping behavior

In Shipping builds:

- ignore `-JungleReviewSpawn`, or compile the feature out;
- always use the normal player start path;
- do not print player-facing spawn labels;
- retain only non-sensitive authority logs needed for diagnostics.

## Logging And Validation Evidence

### Required startup log category

Create a dedicated log category or equivalent structured log prefix:

```text
LogJungleWorldAuthority
```

### Required log lines

On editor PIE, standalone, and packaged Linux startup, emit structured evidence similar to:

```text
LogJungleWorldAuthority: Display: AuthorityCheck Begin BuildConfig=Development Platform=Linux IsEditor=false IsPIE=false
LogJungleWorldAuthority: Display: LoadedWorld Package=/Game/JungleGame/Maps/WLD_JungleIsland_Full Name=WLD_JungleIsland_Full
LogJungleWorldAuthority: Display: ExpectedAuthority Id=JG_FULL_WORLD_001 Asset=/Game/JungleGame/Maps/WLD_JungleIsland_Full Status=EstablishedShell
LogJungleWorldAuthority: Display: DefaultMaps GameDefaultMap=/Game/JungleGame/Maps/WLD_JungleIsland_Full EditorStartupMap=/Game/JungleGame/Maps/WLD_JungleIsland_Full ServerDefaultMap=/Game/JungleGame/Maps/WLD_JungleIsland_Full
LogJungleWorldAuthority: Display: WorldPartition Enabled=true StreamingEnabled=<true|false> OneFilePerActor=<true|false> DataLayers=<count-or-unavailable>
LogJungleWorldAuthority: Display: ReviewSpawn Selected=<none|SP_Review_DefaultBeach_001> Source=<Default|CommandLine|PIE|Disabled>
LogJungleWorldAuthority: Display: LegacyCell0 Status=ReviewAnchorOnly RuntimeDefaultAllowed=false
LogJungleWorldAuthority: Display: TemplateOpenWorld Status=RetiredAsRuntimeDefault RuntimeDefaultAllowed=false
LogJungleWorldAuthority: Display: AuthorityCheck PASS
```

On failure:

```text
LogJungleWorldAuthority: Error: AuthorityCheck FAILED Reason=LoadedWorldMismatch Expected=/Game/JungleGame/Maps/WLD_JungleIsland_Full Actual=/Game/JungleGame/Maps/Cell0
```

or:

```text
LogJungleWorldAuthority: Error: AuthorityCheck FAILED Reason=TemplateOpenWorldDefault Actual=/Game/Maps/OpenWorld
```

### Evidence fields

The PR should capture evidence for:

- UE version;
- platform;
- build configuration;
- editor vs packaged;
- PIE vs standalone vs packaged;
- loaded world package path;
- expected authoritative asset path;
- authority ID;
- authority status;
- configured Game Default Map;
- configured Editor Startup Map;
- configured Server Default Map;
- map command-line override present/absent;
- selected review spawn name and source;
- Cell 0 policy;
- OpenWorld policy;
- PASS/FAIL result.

### Validation commands

The implementation PR should include repeatable validation steps for:

```text
Editor open:
- launch Unreal Editor on Linux;
- confirm EditorStartupMap resolves to WLD_JungleIsland_Full;
- run PIE or Standalone;
- capture LogJungleWorldAuthority PASS.

Packaged Linux launch:
- package/cook Linux build;
- run packaged executable with no map URL argument;
- capture LogJungleWorldAuthority PASS;
- confirm LoadedWorld Package=/Game/JungleGame/Maps/WLD_JungleIsland_Full.

Negative check:
- manually open Cell 0 or OpenWorld in editor;
- confirm the authority check does not report FullSizeWorldAuthority PASS.
```

### Validation must not rely on visuals alone

A screenshot that "looks larger" is not sufficient. Authority must be proven by asset path, default config, packaging/cook inclusion, and startup logs.

## Implementation-Sized PR Scope

### In scope

One narrow implementation PR should do the following:

1. Create or rename the authoritative full world map asset:

   ```text
   /Game/JungleGame/Maps/WLD_JungleIsland_Full
   ```

2. Configure `GameDefaultMap`, `EditorStartupMap`, and preferably `ServerDefaultMap` to the authoritative asset.
3. Add the authoritative map to packaging/cook map inclusion if the project uses explicit map lists.
4. Add a developer-only world authority marker or data record.
5. Add minimal startup logging under `LogJungleWorldAuthority`.
6. Add a developer-only review spawn registry or tagged PlayerStart set.
7. Reclassify Cell 0 as `ReviewAnchorOnly` / `LegacyValidationSlice`.
8. Reclassify template `OpenWorld` as `TemplateSource` / `RetiredAsRuntimeDefault`.
9. Add simple validation instructions or scripts that inspect config and packaged logs.

### Out of scope

This PR should not:

- generate the final island terrain;
- implement final biome placement;
- implement player navigation UI;
- add a map/minimap/compass/bearing/objective marker;
- solve weather simulation;
- solve ecology simulation;
- migrate all Cell 0 content into the full island;
- implement full save/load;
- add combat/chase/gore systems;
- claim the full world is content-complete.

### Recommended PR title

```text
Establish full-size world authority and packaged startup map
```

### Recommended PR body evidence checklist

```text
- [ ] Authoritative map asset exists at /Game/JungleGame/Maps/WLD_JungleIsland_Full
- [ ] GameDefaultMap points to authoritative map
- [ ] EditorStartupMap points to authoritative map
- [ ] Packaged Linux build launches with no map override
- [ ] Startup log shows AuthorityCheck PASS
- [ ] Startup log shows LoadedWorld Package=/Game/JungleGame/Maps/WLD_JungleIsland_Full
- [ ] Cell 0 is classified as ReviewAnchorOnly / LegacyValidationSlice
- [ ] OpenWorld is classified as TemplateSource / RetiredAsRuntimeDefault
- [ ] No player-facing navigation UI added
```

## Files And Areas Likely Touched

Exact files depend on the existing repository, but likely areas include:

```text
Config/DefaultEngine.ini
Config/DefaultGame.ini
Config/DefaultEditor.ini
Config/DefaultEditorPerProjectUserSettings.ini        # avoid committing user-specific changes unless already project-approved
Content/JungleGame/Maps/WLD_JungleIsland_Full.umap
Content/JungleGame/Maps/Review/LV_Review_Cell0_Legacy.umap
Content/JungleGame/Maps/_Template/TMP_OpenWorld_Source.umap
Content/JungleGame/Data/DA_WorldAuthority_FullWorld.uasset
Content/JungleGame/Data/DA_ReviewSpawnRegistry_FullWorld.uasset
Source/JungleGame/Public/World/JungleWorldAuthority*.h
Source/JungleGame/Private/World/JungleWorldAuthority*.cpp
Source/JungleGame/Private/Game/JungleGameMode*.cpp
Tests/WorldAuthority/*
Docs/Validation/WorldAuthority/*
```

### Notes on binary assets

UE `.umap` and `.uasset` files are binary. The PR should keep binary churn small:

- prefer creating one new authoritative map asset;
- avoid unnecessary resaves of unrelated maps;
- avoid moving many content assets unless required;
- fix redirectors only for assets intentionally moved;
- document binary asset changes in the PR summary.

### Config caution

Do not commit local machine/user preference files unless the project already tracks them intentionally. Keep authority in project config, source, content assets, or project-owned docs.

## Acceptance Criteria

Full-size world authority is established when all criteria below pass.

### Asset authority

- `WLD_JungleIsland_Full.umap` exists at the agreed project-owned path.
- The asset is under source control.
- The asset is not named `OpenWorld`, `Cell0`, `Test`, `Prototype`, or `Example`.
- The asset contains or references authority ID `JG_FULL_WORLD_001`.

### Default startup

- `GameDefaultMap` resolves to `/Game/JungleGame/Maps/WLD_JungleIsland_Full`.
- `EditorStartupMap` resolves to `/Game/JungleGame/Maps/WLD_JungleIsland_Full`, or an explicitly documented editor bootstrap immediately opens it.
- `ServerDefaultMap` is set consistently if the project has server/dedicated-server configuration.

### Packaging

- The authoritative map is included in cook/package configuration when explicit map lists are used.
- Packaged Linux launch without a map URL argument loads the authoritative map.
- Packaged logs include `LogJungleWorldAuthority: ... AuthorityCheck PASS`.

### Legacy handling

- Cell 0 is not the packaged default.
- Cell 0 cannot satisfy `FullSizeWorldAuthority`.
- Template `OpenWorld` is not the packaged default.
- Any retained template OpenWorld asset is clearly marked as template/source/deprecated/dev-only.

### Review spawns

- Developer-only review spawns exist or are explicitly stubbed with a data registry.
- Review spawn access is not available through player-facing UI.
- Shipping build behavior ignores or disables review-spawn selection.

### Forbidden UI

The PR adds none of the following player-facing systems:

- map;
- minimap;
- compass;
- bearing;
- GPS;
- objective marker;
- objective arrow;
- HUD clock;
- explicit time display;
- orientation confidence meter;
- hidden orientation meter;
- character orientation comments.

### Evidence

The PR includes logs or artifacts proving:

- editor startup map;
- packaged Linux startup map;
- loaded world package path;
- default map config;
- Cell 0 status;
- OpenWorld status;
- review spawn status;
- authority PASS/FAIL.

## Risks And Stop Conditions

### Risks

| Risk | Impact | Mitigation |
|---|---|---|
| Existing template map remains default | Packaged build still launches old/full-fake world | Validate config and logs for exact asset path |
| Cell 0 remains visually dominant | Reviewers mistake old slice for full-world progress | Rename/classify Cell 0 and log policy |
| Command-line map override hides broken default | False pass during packaged testing | Require no-override packaged launch proof |
| Binary asset churn is too large | Review becomes unsafe/noisy | Limit scope to authority map, registry, config |
| Redirectors break references | Load failures or stale asset paths | Fix redirectors and validate cooked launch |
| World Partition map not cooked | Packaged load failure | Add map to packaging settings and run packaged test |
| Debug spawn leaks into player UI | Violates survival/navigation design | Compile out/disable in Shipping and avoid UI surfaces |
| Editor-only World Partition minimap confused with gameplay minimap | Scope misunderstanding | Document it as editor-only and never expose runtime UI |

### Stop conditions

Stop and report a blocker if any of these occur:

- the implementation cannot create or save the authoritative map asset;
- `GameDefaultMap` cannot be changed without breaking startup;
- packaged Linux build cannot launch far enough to prove loaded world path;
- `WLD_JungleIsland_Full` fails to cook/package;
- Cell 0 is still required as the packaged default for unrelated systems;
- template `OpenWorld` cannot be distinguished from project-owned world content;
- authority logs cannot identify the loaded world package path;
- any requested implementation would add forbidden player-facing navigation UI.

## Rejected Or Forbidden Approaches

### Rejected: "Just keep using OpenWorld"

Keeping `OpenWorld` as the authoritative map is rejected. Epic describes it as a starting-point template with sample content. [UE-WorldPartition] Jungle Game needs a project-owned identity path.

### Rejected: "Cell 0 is the full world until replaced"

Cell 0 can be a review anchor, but it cannot define the full island, full terrain authority, or packaged default. This would let legacy playable-slice constraints distort full-world terrain design.

### Rejected: "Visual scale screenshot proves authority"

A visual check is insufficient. Authority requires exact map asset path, default config, cook/package inclusion, and logs.

### Rejected: "Command-line launch proves packaged default"

Launching with a map URL argument proves override behavior, not default packaged behavior. Packaged acceptance must launch without a map override.

### Rejected: player-facing navigation aids

Do not add, suggest, or leave hooks for player-facing map, minimap, compass, bearing, GPS, objective markers, objective arrows, HUD clocks, explicit time display, orientation confidence meters, hidden orientation meters, or character comments that tell the player whether they are oriented.

### Rejected: hidden orientation meter

A hidden meter still creates a system that tracks player orientation confidence and risks affecting content, hints, audio, dialogue, or difficulty. It is specifically forbidden.

### Allowed with isolation: editor-only World Partition review tools

World Partition editor tools, region loading, and editor-only minimap generation may be used internally by developers if they remain editor/debug facilities and are never exposed as player-facing UI. Epic documents World Partition editor region loading and editor minimap generation as editor workflow aids. [UE-WorldPartition]

## Sources And References

- [UE-DefaultLevel] Epic Developer Community, **Changing the Default Level of an Unreal Engine Project**. <https://dev.epicgames.com/documentation/unreal-engine/changing-the-default-level-of-an-unreal-engine-project>
- [UE-ProjectSettings] Epic Developer Community, **Project Section of the Unreal Engine Project Settings**. <https://dev.epicgames.com/documentation/unreal-engine/project-section-of-the-unreal-engine-project-settings>
- [UE-Packaging] Epic Developer Community, **Packaging Your Project**. <https://dev.epicgames.com/documentation/unreal-engine/packaging-your-project>
- [UE-Running] Epic Developer Community, **Running Unreal Engine**. <https://dev.epicgames.com/documentation/unreal-engine/running-unreal-engine>
- [UE-CommandLine] Epic Developer Community, **Command-Line Arguments in Unreal Engine**. <https://dev.epicgames.com/documentation/unreal-engine/command-line-arguments-in-unreal-engine>
- [UE-WorldPartition] Epic Developer Community, **World Partition in Unreal Engine**. <https://dev.epicgames.com/documentation/en-us/unreal-engine/world-partition-in-unreal-engine>
- [UE-WorkingLevels] Epic Developer Community, **Working with Levels in Unreal Engine**. <https://dev.epicgames.com/documentation/unreal-engine/working-with-levels-in-unreal-engine>
- [UE-PlayerStart] Epic Developer Community, **Player Start Actor in Unreal Engine**. <https://dev.epicgames.com/documentation/en-us/unreal-engine/player-start-actor-in-unreal-engine>
- [UE-PackagingSettings] Epic Developer Community, **UProjectPackagingSettings API Reference**. <https://dev.epicgames.com/documentation/unreal-engine/API/Developer/DeveloperToolSettings/UProjectPackagingSettings>
