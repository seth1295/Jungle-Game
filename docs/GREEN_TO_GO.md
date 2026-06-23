# Green-To-Go

Green-to-go is the state where this repo is ready to start real UE5 implementation without guessing about platform, engine version, binary asset strategy, or first playable scope.

## Current status

Repo status: green.

Implementation status: not green yet.

The repo has a merged scaffold and remote workflow. The game implementation should not begin until the green-to-go gates pass.

## Green-to-go checklist

The project is green to begin UE5 implementation when all of these are true:

- Linux development target is confirmed.
- UE5 editor launches on the local machine.
- A C++ UE project can build locally.
- Linux package/cook path is understood.
- Packaged Linux smoke launch path is understood.
- UE5.8 vs UE5.7 decision is recorded.
- NVIDIA/Vulkan feature baseline is recorded.
- Lumen/Nanite/VSM stance is recorded.
- Binary asset strategy is recorded.
- First playable slice scope is recorded.
- The next implementation PR has clear allowed and forbidden scope.

## Hard constraints

- Spatial 3D only; no 2D.
- Linux-first development and build target.
- Windows validation can happen later when needed.
- UE5 is the primary engine candidate.
- No VR target.
- No multiplayer target.
- No paid asset or plugin dependency early.
- No giant asset imports before the first slice is proven.
- No gore horror.
- Keep the current lane: 1000 CE dense tropical jungle survival-simulation with light social/anthropological horror.

## Pass condition

Green-to-go passes when Gates 001 and 002 complete, and Gate 003 is ready to create a minimal UE5 C++ project with no unresolved engine/platform blocker.

## Stop condition

Stop before implementation if validation shows UE5.8/UE5.7 cannot run reliably on the local Linux setup, if Vulkan feature support is unclear, or if the first UE project would require large binary/editor-authored assets before the architecture is decided.
