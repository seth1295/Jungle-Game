# UE Project Bootstrap Research

## Executive Summary

Gate 003 should create the first minimal UE C++ project **only after** Gate 001 validates Linux and Gate 002 selects a UE version. This research file defines the future bootstrap PR scope; it does not create the project.

Epic's project creation docs show that Unreal projects are created from the Project Browser, with an implementation choice of Blueprint or C++. For Jungle Game, the future bootstrap must use C++ as the implementation lane. Epic's modules docs explain that projects have a primary module by default, that modules live under `Source/`, that modules use `.Build.cs`, and that `.uproject` contains the module list. Epic's UBT docs explain that UnrealBuildTool builds from target/module files, not IDE project files.

The bootstrap PR should be brutally small: `.uproject`, minimal `Source/`, minimal `Config/`, and only unavoidable minimal `Content/` if the editor requires it. It should not add gameplay systems, large assets, plugins, paid dependencies, generated build output, or binary clutter.

## Gate 003 Goal

Define the minimal UE C++ project bootstrap scope for Jungle Game.

Gate 003 should answer:

- Which UE version is selected?
- Where is the local editor/engine root?
- What minimal files are expected in a C++ UE project?
- Which generated/cache files are forbidden?
- What build/cook/smoke validation must be documented?
- What exact diff shape is acceptable?

## Required Inputs From Gates 001 and 002

Gate 003 is blocked until:

| Input | Required? | Reason |
|---|---:|---|
| Linux OS/driver/Vulkan evidence | Yes | Proves the editor path is viable. |
| UE editor path/version | Yes | Bootstrap must match selected UE. |
| Engine version decision | Yes | Prevents generating files under the wrong UE version. |
| Basic editor launch result | Yes | Project creation needs a functioning editor path. |
| Binary asset policy | Yes | UE maps/assets are binary and must be intentionally minimal. |

## Minimal UE5 C++ Project Structure

Expected future bootstrap project shape:

```text
JungleGame.uproject
Source/
  JungleGame/
    JungleGame.Build.cs
    JungleGame.cpp
    JungleGame.h
Config/
  DefaultEngine.ini
  DefaultGame.ini
  DefaultInput.ini        if needed
Content/                  only if unavoidable/minimal
```

The exact generated files depend on the selected template and UE version. The future implementation worker must inspect the generated output before staging anything.

## Files Expected In The Future Bootstrap PR

Allowed in the future bootstrap PR:

- One `.uproject` file.
- One primary runtime module under `Source/JungleGame/` or the selected module name.
- Minimal UE target/module files required by the generated C++ project.
- Minimal `Config/` files required for project defaults.
- Minimal `Content/` only if required by the template/editor and explicitly reviewed.
- Docs describing build/cook/package smoke result.

## Files Forbidden From The Future Bootstrap PR

Forbidden:

```text
Binaries/
Intermediate/
Saved/
DerivedDataCache/
.vs/
.idea/
packaged builds
shader caches
local editor logs
crash dumps
large imported assets
paid plugin files
Marketplace or Fab asset dumps
screenshots/videos unless explicitly requested
```

Also forbidden:

- Gameplay systems.
- Inventory/crafting/survival systems.
- Watcher AI.
- Level art pass.
- Full jungle map.
- Heavy Blueprint architecture.

## Generated / Cache / Intermediate File Risks

UE creates many local files that should usually stay untracked. The bootstrap PR must inspect `git diff --stat` and file paths before staging.

Blocker examples:

- Generated build folders staged.
- Local logs staged.
- Packaged build output staged.
- Large binary assets staged without review.
- Editor cache folders staged.

## Linux Project Generation Options

Options:

1. **Editor Project Browser**
   - Pros: official, safest for initial project generation.
   - Cons: binary/editor-authored outputs need careful diff inspection.

2. **Template/generation through installed UE tooling**
   - Pros: repeatable if known.
   - Cons: exact command depends on UE path/version and should not be invented before Gate 001/002.

3. **Manual hand-written project files**
   - Pros: minimal text diff.
   - Cons: risky if UE version conventions are wrong.

Recommendation:

- Prefer editor-created minimal C++ project after the selected UE version is known.
- Inspect every generated file before staging.
- Do not commit generated build/cache output.

## Build Validation Scope

Future bootstrap PR should record:

- UE version.
- Editor path.
- Project opens.
- C++ compile result.
- Any warnings.
- Whether the project can launch in editor.

Do not hardcode local paths in final repo docs unless they are intentionally local-only notes.

## Cook / Package Validation Scope

Epic's Build Operations docs explain that BuildCookRun stages include build, cook, stage, package, deploy, and run. Epic's Linux Quickstart shows that Linux packaging can be tested from the editor or command line, but the exact command depends on engine root and project path.

Gate 003 should at least document the intended Linux cook/package path. If packaging fails for environment reasons, record it as a blocker or warning depending on severity.

## Binary Asset Policy

Policy:

- Keep binary assets to the minimum required to open the project.
- Prefer text/config/code where possible.
- Do not add asset packs.
- Do not add paid plugins/assets.
- Do not add large `Content/` trees.
- If a `.umap` exists, it must be named and intentional.

## Blueprint Policy

- C++ owns important runtime state later.
- Blueprint is acceptable only as a thin editor/placement layer.
- Bootstrap should not create gameplay Blueprints unless the chosen template forces a minimal one.
- Any Blueprint asset must be named, justified, and reviewed as binary content.

## Gitignore Requirements

The repo `.gitignore` should exclude UE generated output before project files are committed.

Must cover:

- `Binaries/`
- `Intermediate/`
- `Saved/`
- `DerivedDataCache/`
- local IDE/project-generated clutter unless intentionally tracked
- package/stage output
- logs/crash dumps

## Pass / Warning / Blocker Table

| Area | Pass | Warning | Blocker |
|---|---|---|---|
| UE version | Gate 002 selected version | Version selected but not locally opened | No version decision |
| Project creation | Minimal C++ project opens | Editor generated extra reviewed assets | Project cannot open |
| Source | Minimal primary module only | Extra template code needs review | Gameplay systems added |
| Config | Minimal defaults only | Template defaults noisy | Incorrect platform/default map config blocks launch |
| Content | Empty/minimal reviewed assets | Small binary files unavoidable | Asset pack/import dump |
| Build | C++ compile passes | Nonfatal warnings | Compile fails |
| Cook/package | Path documented or passes | Package not yet run | Build/cook impossible due environment |
| Diff hygiene | Intended files only | Naming cleanup needed | Generated/cache/build output staged |

## Bootstrap PR Checklist

Future Gate 003 bootstrap PR should:

- Start from clean `main`.
- Confirm Gate 001 and Gate 002 passed.
- Use selected UE version.
- Create minimal C++ project.
- Inspect generated file tree.
- Remove generated/cache/build output from tracking.
- Run or document C++ build/editor launch.
- Document package/cook path or blocker.
- Keep binary assets minimal.
- Run CodeRabbit.
- Fix only in-scope findings.

## PR Scope Recommendation

Recommended target file: `docs/UE_PROJECT_BOOTSTRAP_RESEARCH.md`

Allowed PR changes for the **research/scope PR**:

- Add/update bootstrap research docs.
- Add/update minimal UE project structure checklist.
- Add/update generated-file/binary-asset policy.
- Add/update Gate 003 scope notes.

Forbidden PR changes for the **research/scope PR**:

- No `.uproject` creation in the research PR.
- No `Source/` changes in the research PR.
- No `Config/` changes in the research PR.
- No `Content/` changes in the research PR.
- No code changes in the research PR.
- No binary assets in the research PR.
- No engine-generated files.

Gate recommendation:

- Gate 003 status: **Blocked until Gate 001/002 pass; research scope ready.**
- Is bootstrap implementation ready?: **No, not until engine version is selected and editor path is known.**
- Next active gate: **Gate 003 after Gate 002 passes.**
- Blockers: missing engine version decision; missing local UE editor launch evidence.
- Warnings: binary assets and generated folders can easily pollute the first UE PR.
- Manual evidence required: selected UE version, editor path, build/cook method.

## Sources

- Epic Games — Creating a New Project in Unreal Engine: https://dev.epicgames.com/documentation/unreal-engine/creating-a-new-project-in-unreal-engine
- Epic Games — Unreal Engine Modules: https://dev.epicgames.com/documentation/unreal-engine/unreal-engine-modules
- Epic Games — UnrealBuildTool: https://dev.epicgames.com/documentation/unreal-engine/unreal-build-tool-in-unreal-engine
- Epic Games — Linux Development Quickstart: https://dev.epicgames.com/documentation/unreal-engine/linux-development-quickstart-for-unreal-engine
- Epic Games — Build Operations: https://dev.epicgames.com/documentation/unreal-engine/build-operations-cooking-packaging-deploying-and-running-projects-in-unreal-engine
