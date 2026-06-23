# UE5 Engine Version Decision Research

Gate 002 is blocked pending Gate 001 local Linux evidence.

Recommended candidate: UE 5.8.
Fallback candidate: UE 5.7.

## Executive Summary

Gate 002 should stay blocked until Gate 001 records OS, kernel, glibc, NVIDIA driver, Vulkan summary, atomic-int64 support, Unreal Editor path/version, and editor launch evidence.

UE 5.8 is the first candidate because Epic's current Linux documentation is UE 5.8 and its recommended Linux hardware class includes GeForce 2080, 8 GB+ graphics RAM, 32 GB RAM, Vulkan, and NVIDIA 570+ drivers. UE 5.7 remains the fallback if 5.8 fails local Linux validation.

This is research/scope only. It does not approve project creation, code, assets, plugins, or implementation.

## Required Inputs From Gate 001

| Evidence | Required | Reason |
|---|---:|---|
| OS version | Yes | Ubuntu 26.04 differs from Epic's Ubuntu 22.04 recommendation. |
| Kernel/glibc | Yes | Epic documents Linux kernel/glibc minimums. |
| NVIDIA driver | Yes | Epic documents NVIDIA 570+ for Vulkan. |
| Vulkan summary | Yes | Linux rendering path. |
| `VK_KHR_shader_atomic_int64` | Yes | Required before relying on Nanite/VSM. |
| UE editor path/version | Yes | Version decision must match a real install. |
| Editor launch result | Yes | Without launch, the decision is theoretical. |

## UE 5.8 Candidate

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

Policy: choose UE 5.8 only if editor launch and basic Linux workflow validate.

## UE 5.7 Fallback

Strengths:

- Lower fresh-release risk.
- Epic's Linux requirements page includes a UE 5.7 toolchain/version-history row.
- Still viable for C++ first-person prototype work.

Risks:

- Still needs the same Linux/Vulkan/NVIDIA validation.
- May lack useful 5.8 terrain/vegetation/lighting improvements.
- Requires extra work if 5.7 is not already installed.

Policy: use UE 5.7 only if UE 5.8 fails or is unavailable. Do not fallback just because UE 5.8 is new if UE 5.8 works locally.

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
| Current docs | Strong | Older fallback | Favors 5.8 if local path works |
| Fresh-release risk | Higher | Lower | Favors 5.7 if 5.8 fails |
| Linux requirements clarity | Strong | Good via version history | Neutral |
| RTX 2080 viability | Must profile | Must profile | Neutral |
| Jungle tooling | Better long-term candidate | Still viable | Slight 5.8 advantage |
| First playable needs | Enough | Enough | Either works if validated |

## Fallback Triggers

Fallback from UE 5.8 to UE 5.7 if:

- UE 5.8 editor cannot launch locally.
- UE 5.8 build/cook path fails for engine-version reasons.
- UE 5.8 Vulkan/NVIDIA behavior is broken.
- UE 5.8 is not installed and 5.7 is the available local path.
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

- Gate 002 status: **Blocked pending Gate 001 evidence.**
- Recommended version: **UE 5.8 provisional candidate.**
- Fallback: **UE 5.7.**
- Next gate: **Gate 003 only after version selected.**

## Sources

- Epic Games — Linux Development Requirements for Unreal Engine 5.8: https://dev.epicgames.com/documentation/unreal-engine/linux-development-requirements-for-unreal-engine
- Epic Games — Linux Development Quickstart for Unreal Engine 5.8: https://dev.epicgames.com/documentation/unreal-engine/linux-development-quickstart-for-unreal-engine
- Epic Games — PCG Overview: https://dev.epicgames.com/documentation/unreal-engine/procedural-content-generation-overview
- Epic Games — World Partition: https://dev.epicgames.com/documentation/unreal-engine/world-partition-in-unreal-engine
- Epic Games — Nanite Virtualized Geometry: https://dev.epicgames.com/documentation/unreal-engine/nanite-virtualized-geometry-in-unreal-engine
- Epic Games — Lumen Global Illumination and Reflections: https://dev.epicgames.com/documentation/unreal-engine/lumen-global-illumination-and-reflections-in-unreal-engine
