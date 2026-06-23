# Gate 001 — UE5 Linux Validation

## Status

active

## Purpose

Validate that the local Linux environment can support UE5 development for Jungle Game before any UE project files are added.

## Allowed scope

This gate may:

- document local system validation results;
- document UE5 installation options;
- document Vulkan/NVIDIA feature checks;
- document editor launch/build/cook/package validation commands;
- add or update validation notes under `docs/`;
- update this gate's status and next gate pointer when complete.

## Forbidden scope

This gate must not:

- create a UE project;
- add `.uproject`, `Source/`, `Config/`, or `Content/` files;
- add binary assets;
- choose the final engine version unless Gate 002 is also explicitly activated;
- install paid assets or plugins;
- change game design scope beyond validation notes.

## Required inputs

- `docs/PR_WORKFLOW.md`
- `docs/GREEN_TO_GO.md`
- local OS details
- NVIDIA driver details
- Vulkan feature details
- available UE5 editor/install path, if present

## Required validation

Record results for:

- OS/distro and kernel;
- NVIDIA driver version;
- Vulkan version;
- `VK_KHR_shader_atomic_int64` availability;
- UE5 editor availability or install path;
- editor launch result, if available;
- C++ project build feasibility, if available;
- Linux package/cook smoke path, if available.

## Exit criteria

This gate passes when the repo contains a clear Linux validation record showing whether UE5.8 can proceed or whether testing must fall back to UE5.7.

## CodeRabbit handling

Fix valid in-scope documentation or command issues only.

## Next gate

`docs/gates/002-engine-version-decision.md`

## Stop conditions

Stop and report if:

- UE is not installed and installation requires user action;
- driver/Vulkan support is missing or ambiguous;
- Ubuntu version/toolchain issues need manual intervention;
- validation requires rebooting into Windows;
- the required commands would expose sensitive local data.
