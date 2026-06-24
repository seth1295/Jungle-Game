# UE5 Engine Version Decision Research

Gate 002 is complete based on Gate 001 local Linux evidence.

Selected version: UE 5.8.
Fallback candidate: UE 5.7.

## Executive Summary

Gate 001 now records OS, kernel, glibc, NVIDIA driver, Vulkan summary, atomic-int64 support, Unreal Editor path/version, editor launch evidence, shader compilation completion, and OpenWorld template load evidence.

UE 5.8 is selected because the local UE source checkout at `/mnt/ue5/UnrealEngine` reports UE 5.8.0, the source branch is `release` at commit `7deeb413d`, the Linux editor launches, `ShaderCompileWorker` is built and executable, shaders compile to completion, and the editor reaches template map load with a clean map check. UE 5.7 remains the fallback only if UE 5.8 later fails project bootstrap, compile, cook/package, or Vulkan/editor stability validation for engine-version reasons.

This is version-decision scope only. It does not approve game implementation, assets, plugins, or broad feature scope. It does approve Gate 003 using the selected UE 5.8 source path to create a minimal C++ project.

## Required Inputs From Gate 001

| Evidence | Required | Reason |
|---|---:|---|
| OS version | Yes | Ubuntu 26.04 differs from Epic's Ubuntu 22.04 recommendation. |
| Kernel/glibc | Yes | Epic documents Linux kernel/glibc minimums. |
| NVIDIA driver | Yes | Epic documents NVIDIA 570+ for Vulkan. |
| Vulkan summary | Yes | Linux rendering path. |
| `VK_KHR_shader_atomic_int64` | Yes | Required before relying on Nanite/VSM. |
| UE editor path/version | Yes | `/mnt/ue5/UnrealEngine`, UE 5.8.0, branch `release`, commit `7deeb413d`. |
| Editor launch result | Yes | `UnrealEditor` launches, reaches `FEngineLoop::Init()`, loads OpenWorld template, completes clean map check. |

## UE 5.8 Selection

Decision: **selected for the first implementation phase**.

Local selected path:

| Item | Value |
|---|---|
| UE root | `/mnt/ue5/UnrealEngine` |
| Version file | UE 5.8.0 |
| Source branch | `release` |
| Source commit | `7deeb413d` |
| Editor binary | `Engine/Binaries/Linux/UnrealEditor` |
| Shader worker | `Engine/Binaries/Linux/ShaderCompileWorker` |
| Editor launch | Passed Gate 001 local validation |
| Cook/package | Deferred until Gate 003 project exists |

Strengths:

- Current Epic docs are UE 5.8.
- Linux recommended hardware matches Seth's broad GPU/RAM class.
- Newer terrain, vegetation, lighting, and shader-iteration work is directionally useful for Jungle Game.
- Good first candidate if local validation passes.

Risks:

- Fresh-version risk.
- Ubuntu 26.04 is not Epic's documented recommended Linux dev OS.
- NVIDIA/Vulkan must prove clean locally.
- New UE 5.8 systems should not be mandatory for Cell 0.

Policy: use UE 5.8 for Gate 003 minimal project bootstrap. Do not require UE 5.8-only experimental systems for the first playable slice. Treat CEF/ANGLE embedded-browser GPU-process failures and Zen DDC fallback as warnings unless they block project bootstrap or editor stability.

## UE 5.7 Fallback

Strengths:

- Lower fresh-release risk.
- Epic's Linux requirements page includes a UE 5.7 toolchain/version-history row.
- Still viable for C++ first-person prototype work.

Risks:

- Still needs the same Linux/Vulkan/NVIDIA validation.
- May lack useful 5.8 terrain/vegetation/lighting improvements.
- Requires extra work if 5.7 is not already installed.

Policy: use UE 5.7 only if UE 5.8 fails project bootstrap, compile, cook/package, or Vulkan/editor stability validation for engine-version reasons. Do not fallback just because UE 5.8 is new; UE 5.8 works locally enough to proceed.

## Feature Policy For First Playable

| Feature | Policy |
|---|---|
| C++ project | Required later, after Gate 003. |
| Lumen | Candidate, performance-gated. |
| Lumen hardware ray tracing | Optional, not required. |
| Nanite | Candidate for rocks/trunks/geometry if Vulkan atomic int64 is present. |
| Virtual Shadow Maps | Candidate if driver/Vulkan path passes. |
| PCG | Useful later, not required for first Cell 0. |
| World Partition | Deferred; compact Cell 0 does not need it. |
| Data Layers | Deferred unless Gate 004 requires it. |
| Niagara | Minimal placeholders allowed later. |
| MetaSounds | Deferred or minimal. |
| Chaos | Minimal only; no rope/destruction simulation. |
| Watcher AI framework | Deferred; no full watcher AI. |

## Decision Matrix

| Criterion | UE 5.8 | UE 5.7 | Effect |
|---|---|---|---|
| Current docs | Strong | Older fallback | Favors 5.8 |
| Local availability | Source checkout exists and launches | Not installed/tested locally | Strongly favors 5.8 |
| Fresh-release risk | Higher | Lower | Keep 5.7 fallback policy |
| Linux requirements clarity | Strong | Good via version history | Neutral |
| RTX 2080 viability | Locally validated enough for editor path; must still profile game content | Must profile if used | Favors current 5.8 path |
| Jungle tooling | Better long-term candidate | Still viable | Slight 5.8 advantage |
| First playable needs | Enough and locally available | Enough but not needed yet | Favors 5.8 |

## Fallback Triggers

Fallback from UE 5.8 to UE 5.7 if:

- UE 5.8 editor stops launching locally.
- UE 5.8 minimal C++ project bootstrap fails for engine-version reasons.
- UE 5.8 build/cook/package path fails for engine-version reasons.
- UE 5.8 Vulkan/NVIDIA behavior becomes a core editor/project blocker.
- UE 5.8 source checkout becomes unavailable or corrupted and 5.7 is the available local path.
- 5.8-only feature temptation delays minimal bootstrap.

## PR Scope Recommendation

Recommended target file: `docs/UE_VERSION_DECISION.md`

Allowed PR changes:

- Add/update engine version decision docs.
- Add/update UE 5.8 vs UE 5.7 matrix.
- Update Gate 002 status only if evidence supports it.

Forbidden PR changes:

- No UE project creation.
- No `.uproject`.
- No `Source/` changes.
- No `Config/` changes.
- No `Content/` changes.
- No code changes.
- No binary assets.
- No plugin or asset-pack dependency.
- No game implementation.

Gate recommendation:

- Gate 002 status: **Passed.**
- Selected version: **UE 5.8 using `/mnt/ue5/UnrealEngine`.**
- Fallback: **UE 5.7 only on concrete project/bootstrap/build/cook/editor-stability failure.**
- Next gate: **Gate 003 — UE Project Bootstrap.**

## Sources

- Epic Games — Linux Development Requirements for Unreal Engine 5.8: https://dev.epicgames.com/documentation/unreal-engine/linux-development-requirements-for-unreal-engine
- Epic Games — Linux Development Quickstart for Unreal Engine 5.8: https://dev.epicgames.com/documentation/unreal-engine/linux-development-quickstart-for-unreal-engine
- Epic Games — PCG Overview: https://dev.epicgames.com/documentation/unreal-engine/procedural-content-generation-overview
- Epic Games — World Partition: https://dev.epicgames.com/documentation/unreal-engine/world-partition-in-unreal-engine
- Epic Games — Nanite Virtualized Geometry: https://dev.epicgames.com/documentation/unreal-engine/nanite-virtualized-geometry-in-unreal-engine
- Epic Games — Lumen Global Illumination and Reflections: https://dev.epicgames.com/documentation/unreal-engine/lumen-global-illumination-and-reflections-in-unreal-engine
