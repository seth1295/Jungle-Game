# UE5 Linux Validation Research

## Executive Summary

Gate 001 now has local evidence that Seth's Linux workstation can launch the selected UE5 source editor path. The local UE source checkout at `/mnt/ue5/UnrealEngine` reports UE 5.8.0, has the required editor and `ShaderCompileWorker` binaries, and `UnrealEditor` reached engine initialization with the OpenWorld template loaded.

Epic's UE 5.8 Linux documentation gives this gate a concrete target: Ubuntu 22.04 is the recommended Linux OS, 32 GB RAM is recommended, GeForce 2080 and 8 GB+ graphics RAM are named as the recommended GPU class, and Vulkan on NVIDIA is documented with a 570+ driver target. Epic also documents two Linux editor paths: a precompiled installed build and a source build, with the editor launched from `Engine/Binaries/Linux/UnrealEditor`.

Seth's machine has recorded local evidence for the required Gate 001 decision points: Ubuntu 26.04, kernel/glibc, RTX 2080 8 GB, NVIDIA 595.71.05, Vulkan 1.4.341, `VK_KHR_shader_atomic_int64`, UE workspace storage, Epic source access, UE 5.8 source checkout, editor launch, shader compilation, and template map load.

This document is validation scope only. It does not add a UE project, game code, or assets. Cook/package validation remains deferred until a UE project exists.

## Gate 001 Goal

Validate that the local Linux environment can support Unreal Engine development for Jungle Game before any Unreal project files are added.

Gate 001 should answer:

- What OS, kernel, and glibc version is the machine actually running?
- What NVIDIA driver is installed?
- What Vulkan version and device features are exposed?
- Is `VK_KHR_shader_atomic_int64` available for Nanite / Virtual Shadow Maps related paths?
- Is a UE editor install available?
- Can the editor launch?
- Is the Linux build/cook/package path understood enough to proceed to Gate 002?

## Local Validation Snapshot — 2026-06-24 AEST

Current Gate 001 light status: **Green for editor/source validation; cook/package deferred until project bootstrap**.

Green evidence recorded:

| Evidence | Result | Status |
|---|---|---|
| OS | Ubuntu 26.04 Resolute Raccoon | Recorded / warning because newer than Epic's Ubuntu 22.04 recommendation |
| Kernel | 7.0.0-22-generic | Recorded |
| glibc | 2.43 | Recorded / above Epic minimum |
| GPU | NVIDIA GeForce RTX 2080 | Recorded |
| GPU memory | 8192 MiB | Recorded / matches Epic's 8 GB+ recommendation class |
| NVIDIA driver | 595.71.05 | Recorded / above Epic's 570+ target |
| Vulkan instance | 1.4.341 | Recorded |
| Vulkan NVIDIA device | RTX 2080, NVIDIA driver 595.71.05 | Recorded |
| Vulkan atomic int64 extension | `VK_KHR_shader_atomic_int64` present | Recorded / green for Nanite/VSM prerequisite tracking |
| Core tools | `gcc`, `g++`, `cmake`, `make`, `git` present | Recorded |
| Clang 18 | `clang-18` and `clang++-18` installed and verified | Recorded / green |
| Clang 20 | `clang-20` and `clang++-20` installed and verified | Recorded / useful for UE5.7/UE5.8 fallback comparison |
| Ninja | `ninja` installed and verified | Recorded / green |
| Linker/debug tools | `lld-18`, `llvm-config-18`, `lldb-18`, `gdb`, `valgrind`, `ccache`, `mold` installed and verified | Recorded / green |
| Vulkan shader tools | `glslangValidator`, `spirv-val`, Vulkan validation layers installed and verified | Recorded / green |
| Source/archive helpers | `git-lfs`, `curl`, `wget`, `rsync`, `unzip`, `zip`, `7zip`, `patchelf` installed or available | Recorded / green |

Resolved / deferred evidence:

| Evidence | Result | Gate impact |
|---|---|---|
| UE editor path | `/mnt/ue5/UnrealEngine/Engine/Binaries/Linux/UnrealEditor` exists and is executable | Recorded / green |
| UE source version | `Engine/Build/Build.version` reports MajorVersion `5`, MinorVersion `8`, PatchVersion `0`; source checkout branch `release`, commit `7deeb413d` | Recorded / green for Gate 002 decision |
| `ShaderCompileWorker` | Initially missing at launch, then built; `/mnt/ue5/UnrealEngine/Engine/Binaries/Linux/ShaderCompileWorker` exists and is executable | Resolved blocker |
| Editor launch | `UnrealEditor` process confirmed running as PID `27998`; log shows `Engine is initialized. Leaving FEngineLoop::Init()` | Recorded / green |
| Template map load | Editor loaded `../../../Engine/Content/Maps/Templates/OpenWorld.umap` as template | Recorded / green |
| Map check | `Map check complete: 0 Error(s), 0 Warning(s)` | Recorded / green |
| Shader compilation | `Shaders Compiled: 6,802`; `Jobs assigned 6,802, completed 6,802 (100%)` | Recorded / green |
| Vulkan editor path | Log records `LogVulkanRHI` creating a Vulkan swapchain with `VK_PRESENT_MODE_MAILBOX_KHR` | Recorded / green for editor Vulkan startup |
| CEF / embedded browser GPU process | CEF GPU acceleration repeatedly logs GLX/ANGLE/EGL initialization failures and GPU process exits | Warning; not a core editor launch blocker unless browser-backed UI becomes required or unstable |
| Local Zen DDC | Zen local readiness timed out/fell back earlier, while local DDC path remained writable | Warning/fallback; not a launch blocker |
| Disk space | Root filesystem reported by `df -h` as 255G size, 188G used, 55G available, 78% used; `df` available space excludes reserved filesystem blocks and is rounded | Warning / improved, but not the preferred full UE install, Derived Data Cache, build, or package workflow target |
| Windows storage partition | `/dev/sda1`, NTFS, label `WIN_STORAGE` | Recorded / Windows storage requirement restored |
| UE workspace partition | `/dev/sda2`, ext4, label `UE5_WORKSPACE`, mounted at `/mnt/ue5`; after UE source checkout, `/mnt/ue5/UnrealEngine` uses about 160 GB and `/mnt/ue5` has about 459 GB free | Recorded / green for Linux UE source/build/cache/project workspace |
| UE workspace folders | `/mnt/ue5/UnrealEngine`, `/mnt/ue5/UE-Downloads`, `/mnt/ue5/UE-Installs`, `/mnt/ue5/DerivedDataCache`, and `/mnt/ue5/JungleGame` created | Recorded / staging path available |
| Heroic/Legendary Epic access | Heroic bundled Legendary is authenticated enough to list 5 Epic library items | Recorded / Unreal Engine not present in listed library |
| EpicGames/UnrealEngine GitHub access | `gh repo view EpicGames/UnrealEngine` resolves with `viewerPermission: READ` and default branch `release` | Recorded / source access green |
| Linux cook/package smoke | Not tested | Deferred until Gate 003 creates a project |

Gate 001 conclusion from this snapshot:

- GPU, NVIDIA driver, Vulkan, atomic-int64, Clang 18, Clang 20, Ninja, LLDB 18, linker tooling, Vulkan shader tooling, and source/archive helpers are **green**.
- UE workspace storage and Epic source access are **green**.
- UE 5.8 source editor availability, `ShaderCompileWorker`, editor launch, OpenWorld template load, map check, shader compilation, and Vulkan swapchain creation are **green**.
- CEF/ANGLE embedded browser GPU-process failures are a **warning**, not a core editor launch blocker.
- Local Zen DDC timeout/fallback is a **warning**, not a core editor launch blocker.
- Root disk headroom is improved but `/mnt/ue5` should remain the preferred UE install/build/cache/project target.
- Linux cook/package validation is **deferred** until Gate 003 creates a minimal UE project.
- Gate 001 has enough evidence to pass and activate Gate 002.

## Sources and Confidence

| Source type | Confidence | Notes |
|---|---:|---|
| Epic UE 5.8 Linux Development Requirements | High | Primary source for Linux OS, hardware, driver, Vulkan, and rendering feature requirements. |
| Epic UE 5.8 Linux Quickstart | High | Primary source for installed/source build path and editor launch path. |
| Epic Build Operations docs | High | Primary source for BuildCookRun/cook/package terminology. |
| Local validation command list | Medium | Commands are read-only checks, but actual availability depends on local packages. |
| Ubuntu 26.04 risk assessment | Medium | Based on mismatch with Epic's documented Ubuntu 22.04 development baseline; must be validated locally. |

## Official UE5 Linux Requirements

Epic's UE 5.8 Linux Development Requirements page lists the recommended Linux development hardware as Ubuntu 22.04, 32 GB RAM, GeForce 2080, 8 GB+ graphics RAM, and Vulkan with NVIDIA 570+ drivers.

Epic's minimum software requirements for running the engine/editor include Rocky Linux 8 / Red Hat Linux 8 or newer, Linux kernel 4.18 or newer, and glibc 2.28 or newer. Epic also warns that older glibc versions can make editor/game startup very slow because of `dlopen`, with glibc 2.35+ avoiding that slow path.

For engine development, Epic documents Ubuntu 22.04 / Rocky Linux 8 and clang/toolchain requirements. The exact toolchain depends on UE minor version; Epic's version history lists UE 5.7 with clang-20.1.8 and toolchain v26, while the general UE 5.8 Linux requirements page still separately mentions clang 18.1.0 in the recommended software section. Gate 001 should therefore record the UE version and toolchain rather than assuming one compiler globally.

## Local Target Baseline

### Verified measurements

| Item | Known value | Gate 001 status |
|---|---|---|
| OS | Ubuntu 26.04 Resolute Raccoon | Recorded / warning because newer than Epic's Ubuntu 22.04 recommendation |
| Kernel | 7.0.0-22-generic | Recorded |
| glibc | 2.43 | Recorded / above Epic minimum |
| GPU | NVIDIA RTX 2080 8 GB | Recorded / matches Epic's GeForce 2080 / 8 GB recommendation class |
| NVIDIA driver | 595.71.05 | Recorded / above Epic's 570+ target |
| Vulkan instance | 1.4.341 | Recorded |
| Vulkan device | NVIDIA GeForce RTX 2080 | Recorded |
| Vulkan atomic int64 | `VK_KHR_shader_atomic_int64` present | Recorded |
| Disk | Root filesystem reported by `df -h` as 255G size, 188G used, 55G available, 78% used; `df` available space excludes reserved filesystem blocks and is rounded | Warning / improved, but still not preferred for full UE install/build/cache |
| Windows storage | `/dev/sda1`, NTFS, label `WIN_STORAGE` | Recorded / Windows storage requirement restored |
| UE workspace | `/dev/sda2`, ext4, label `UE5_WORKSPACE`, mounted at `/mnt/ue5`, 652 GB total and 619 GB free | Recorded / green for Linux UE source/build/cache/project workspace |
| UE workspace folders | `/mnt/ue5/UnrealEngine`, `/mnt/ue5/UE-Downloads`, `/mnt/ue5/UE-Installs`, `/mnt/ue5/DerivedDataCache`, `/mnt/ue5/JungleGame` | Recorded |
| Epic source access | `EpicGames/UnrealEngine` resolves with read access and default branch `release` | Recorded / green for source access |
| UE editor | `/mnt/ue5/UnrealEngine/Engine/Binaries/Linux/UnrealEditor`; process confirmed running as PID `27998` | Recorded / green |

### Known project / hardware context not remeasured in this gate snapshot

| Item | Known value | Gate 001 status |
|---|---|---|
| CPU | Ryzen 9 5950X | Plausible / above minimum; not remeasured in this snapshot |
| RAM | 32 GB | Matches Epic's recommended RAM; not remeasured in this snapshot |
| Target RHI | Vulkan | NVIDIA Vulkan path recorded |
| Development stance | Linux-first | Confirmed project constraint |
| Windows stance | Later secondary validation only | Not a substitute for Gate 001 |

## Ubuntu 26.04 Compatibility Risk

Ubuntu 26.04 is newer than Epic's documented/recommended Ubuntu 22.04 development environment. That is not automatically a blocker, but it is a **warning** until local editor launch, toolchain, driver, and package/cook paths are tested.

Risk areas:

- Newer system libraries may expose compatibility issues not covered by Epic's tested baseline.
- UE's native toolchain/sysroot can reduce some compiler/libc risk, but the editor still runs on the local Linux desktop stack.
- GNOME/Wayland/NVIDIA/Vulkan behavior should be validated by actually launching the editor and opening a trivial project later.

Gate 001 should classify Ubuntu 26.04 as pass only after editor launch and basic UE workflow validation. It is a warning while untested. It becomes a blocker only if editor launch, Vulkan device creation, C++ build, or packaging fails because of OS/toolchain/library issues.

## NVIDIA Driver Validation Scope

Epic documents Vulkan on NVIDIA as requiring/recommending driver 570+ for UE 5.8 Linux development. Gate 001 must record the actual driver version from Seth's machine.

Read-only local check to run later:

- `nvidia-smi`

What matters:

- Driver version.
- GPU name.
- VRAM amount.
- Whether the RTX 2080 is visible and healthy.

Classification:

| Result | Classification |
|---|---|
| NVIDIA driver 570+ and RTX 2080 visible | Pass |
| NVIDIA driver below 570 but editor/Vulkan may still run | Warning / likely driver-update candidate |
| NVIDIA command unavailable or GPU missing | Blocker |
| Nouveau/incorrect driver path | Blocker for UE5 Linux validation |

## Vulkan Validation Scope

UE on Linux uses Vulkan RHI for modern rendering paths. Epic's docs explicitly list Vulkan for Linux RHI and warn that Vulkan on Linux is not friendly to low VRAM compared with other backends, recommending a dedicated GPU with high VRAM.

Read-only local checks to run later:

- `vulkaninfo --summary`
- search the `vulkaninfo` output for `VK_KHR_shader_atomic_int64`
- search the `vulkaninfo` output for `shaderAtomicInt64`

What matters:

- Vulkan instance version.
- GPU/device selected.
- Driver info.
- Whether the device is NVIDIA RTX 2080, not a fallback software/CPU device.
- Extension and feature availability.

Classification:

| Result | Classification |
|---|---|
| Vulkan reports RTX 2080 and sane driver | Pass |
| `vulkaninfo` missing only | Warning / manual package needed |
| Vulkan reports software rendering | Blocker |
| Vulkan cannot enumerate the NVIDIA device | Blocker |

## Required Vulkan Feature/Extension Checklist

Epic's UE 5.8 Linux requirements page lists Nanite Virtualized Geometry and Virtual Shadow Maps as requiring a GPU that supports `VK_KHR_shader_atomic_int64`, with SM6 enabled and current graphics drivers.

| Feature / extension | Why it matters | Gate 001 status requirement |
|---|---|---|
| `VK_KHR_shader_atomic_int64` | Epic-listed requirement for Nanite/VSM Vulkan path | Must record present/missing |
| Vulkan API version | Confirms modern Vulkan stack | Must record |
| NVIDIA driver version | Epic documents NVIDIA 570+ | Must record |
| SM6 setting | Project-level setting later; cannot be proven before project exists | Document as later Gate 003/005 item |
| RTX hardware ray tracing | Lumen HWRT requires RTX-class GPU and SM6; RTX 2080 qualifies by class | Optional / not required for Gate 001 pass |

## RTX 2080 8 GB Viability

RTX 2080 8 GB is a viable baseline for validation and first playable target class because Epic's UE 5.8 Linux recommended hardware specifically names GeForce 2080 and 8 GB+ graphics RAM.

However, Jungle Game has special performance risks:

- Dense foliage can create overdraw and shadow cost.
- Vulkan on Linux is explicitly called out by Epic as VRAM-sensitive.
- Lumen, Nanite, Virtual Shadow Maps, Niagara rain/fire effects, and high-density PCG foliage can stack cost.
- First playable should use placeholder/simple assets and must not start with large photoreal asset packs.

Gate 001 conclusion:

- RTX 2080 8 GB is **not a blocker**.
- RTX 2080 8 GB is a **performance budget warning**.
- First playable should validate conservative Lumen/Nanite/VSM usage, not assume maxed UE5 rendering.

## UE5 Editor Availability / Install Path Scope

Epic's Linux Quickstart documents two Linux setup paths:

1. Precompiled installed build.
2. Source build.

For installed builds, Epic documents downloading a Linux archive, unzipping it, and launching `Engine/Binaries/Linux/UnrealEditor`.

Gate 001 should search only for availability/read-only evidence, not install UE automatically.

Read-only local check to run later:

- find a file named `UnrealEditor` under likely Unreal install folders.

Classification:

| Result | Classification |
|---|---|
| UE editor binary found and version known | Pass candidate |
| UE not installed | Blocked on manual install/download, but not project failure |
| Source tree exists but build missing | Warning / source build required |
| Editor launches and reaches project browser | Strong pass evidence |

## Build, Cook, and Package Validation Scope

Epic's Build Operations docs explain that UAT/BuildCookRun covers build, cook, stage, package, deploy, and run stages. Epic's Linux Quickstart gives example BuildCookRun and Turnkey forms, but Gate 001 should not run them before a UE project exists.

Gate 001 should document:

- Where the UE root is.
- Whether RunUAT exists.
- Whether a future project can use Linux packaging.
- That actual cook/package validation belongs after project bootstrap.

Do not hardcode local paths until UE root and project path are known.

## First-Slice Rendering Feature Stance

| Feature | Gate 001 stance |
|---|---|
| Vulkan RHI | Required to validate |
| NVIDIA 570+ | Required target from Epic docs |
| `VK_KHR_shader_atomic_int64` | Required to record before relying on Nanite/VSM |
| Lumen | Viable candidate, but must be tested later |
| Lumen HWRT | Optional; do not require it for first playable |
| Nanite | Candidate for rocks/trunks/geometry; do not assume foliage is free |
| Virtual Shadow Maps | Candidate if extension/driver path supports it |
| PCG | Defer until Gate 004/005 scope; not needed for Gate 001 pass |
| World Partition | Defer; not required for compact first playable |

## Safe Read-Only Local Checks Seth May Run Later

These are examples for manual validation. They are not commands this document has run.

| Check | What it proves | Pass signal |
|---|---|---|
| `cat /etc/os-release` | OS name/version | Ubuntu version recorded |
| `uname -a` | Kernel version | Kernel newer than Epic's minimum and later editor launch succeeds |
| `ldd --version` | glibc version | 2.28+ minimum, 2.35+ preferred for startup performance |
| `nvidia-smi` | NVIDIA GPU/driver | RTX 2080 visible, driver 570+ preferred |
| `vulkaninfo --summary` | Vulkan device/driver summary | NVIDIA RTX 2080 selected, no software renderer |
| search Vulkan output for `VK_KHR_shader_atomic_int64` | Vulkan extension availability | Present before relying on Nanite/VSM |
| search Vulkan output for `shaderAtomicInt64` | Vulkan feature exposure | True/present before relying on Nanite/VSM |
| find `UnrealEditor` | UE editor availability | Linux editor binary found and version recorded |

## Pass / Warning / Blocker Table

| Area | Pass | Warning | Blocker |
|---|---|---|---|
| OS | OS/version/kernel/glibc recorded and editor later launches | Ubuntu 26.04 differs from Epic's Ubuntu 22.04 recommendation | OS/library issue prevents editor launch |
| NVIDIA | RTX 2080 visible, driver 570+ | Driver below Epic's UE 5.8 NVIDIA target | NVIDIA device unavailable |
| Vulkan | Vulkan reports RTX 2080 device | Vulkan tooling missing package | Vulkan cannot enumerate NVIDIA GPU |
| Atomic int64 | `VK_KHR_shader_atomic_int64` present | Not checked yet | Missing if Nanite/VSM required |
| UE editor | Editor binary found, launches, reaches `FEngineLoop::Init()`, loads OpenWorld template, and completes map check | CEF/browser-backed UI GPU-process warnings | No UE install or editor launch failure |
| Build/cook | Future path documented and deferred until Gate 003 project exists | Not runnable until project exists | Toolchain blocks even trivial project later |
| Scope hygiene | Docs only | Extra notes need cleanup | `.uproject`, `Source/`, `Config/`, `Content/`, binaries, generated files added |

## Fallback Triggers

Gate 001 should trigger a fallback/blocked path if:

- UE 5.8 editor stops launching on the local Linux environment.
- NVIDIA driver cannot reach Epic's recommended 570+ path or Vulkan fails.
- `VK_KHR_shader_atomic_int64` is missing and the project intends to rely on Nanite/VSM immediately.
- Ubuntu 26.04 causes editor/toolchain/library failures.
- UE is not installed and requires manual Epic account/download/source-build action.

Fallback does not mean abandoning UE. It means Gate 002 must compare UE 5.8 against UE 5.7 or a more stable installed/source-build path using real evidence.

## What Gate 001 Should Write Into The Repo

Recommended final repo document:

```text
docs/UE5_LINUX_VALIDATION.md
```

The PR should include:

- The known local baseline.
- Evidence table with unknowns marked as not collected.
- Safe read-only validation checks.
- Pass/warning/blocker table.
- Fallback triggers.
- Explicit no-UE-project boundary.

The PR may mark Gate 001 passed because real local Linux, UE source, editor launch, shader compilation, and template map evidence has been inserted.

## PR Scope Recommendation

Recommended target file: `docs/UE5_LINUX_VALIDATION.md`

Allowed PR changes:

- Add/update Linux validation research docs.
- Add/update safe read-only validation checklist.
- Update Gate 001 status only if the evidence supports it.
- Update active next gate only if Gate 001 has enough evidence to pass.

Forbidden PR changes:

- No UE project creation.
- No `.uproject`.
- No `Source/` changes.
- No `Config/` changes.
- No `Content/` changes.
- No code changes.
- No binary assets.
- No engine installation.
- No plugin or asset-pack dependency.

Gate recommendation:

- Gate 001 status: **Passed by local evidence.**
- Next active gate: **Gate 002 — Engine Version Decision.**
- Blockers: none for editor/source validation at time of recording.
- Warnings: Ubuntu 26.04 is newer than Epic's Ubuntu 22.04 baseline; RTX 2080 8 GB is viable but tight for dense jungle/Lumen/Nanite/VSM; CEF/ANGLE embedded-browser GPU process logs GLX/EGL/Vulkan initialization failures; Local Zen DDC timed out and fell back earlier.
- Deferred checks: Linux cook/package smoke remains deferred until a UE project exists under Gate 003.

## Sources

- [S1] Epic Games, **Linux Development Requirements for Unreal Engine 5.8** — https://dev.epicgames.com/documentation/unreal-engine/linux-development-requirements-for-unreal-engine
- [S2] Epic Games, **Linux Development Quickstart for Unreal Engine 5.8** — https://dev.epicgames.com/documentation/unreal-engine/linux-development-quickstart-for-unreal-engine
- [S3] Epic Games, **Build Operations: Cooking, Packaging, Deploying, and Running Projects** — https://dev.epicgames.com/documentation/unreal-engine/build-operations-cooking-packaging-deploying-and-running-projects-in-unreal-engine
- [S4] Epic Games, **Nanite Virtualized Geometry** — https://dev.epicgames.com/documentation/unreal-engine/nanite-virtualized-geometry-in-unreal-engine
- [S5] Epic Games, **Lumen Global Illumination and Reflections** — https://dev.epicgames.com/documentation/unreal-engine/lumen-global-illumination-and-reflections-in-unreal-engine
