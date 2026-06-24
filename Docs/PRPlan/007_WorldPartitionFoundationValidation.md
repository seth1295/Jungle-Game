# 16x16 World Partition Landscape Foundation

Roadmap label: PR #7.

## Foundation Target

The island foundation targets an 8129 x 8129 landscape heightmap at 2 m XY scale.

- Landscape vertices: 8129 x 8129
- Landscape quads per side: 8128
- XY scale: 2 m per quad
- Target side length: 16.256 km
- Boundary framing: island-first; ocean is the natural world boundary

The constants are recorded in `Source/JungleGame/JungleWorldPartitionSpec.h` so tooling can validate against the same dimensions without relying on an unreviewable binary map asset.

## World Partition Baseline

The first authored large-world level should be created as a World Partition map with One File Per Actor. Data Layers are reserved for later ecosystem, debug, validation, and authored-location placement passes. HLOD should be considered only after terrain and ecosystem readability are validated.

Authored locations such as Cell 0, shelters, intro sequences, and story anchors are placed inside the world later.

## Validation

1. Run `git diff --check` on touched files.
2. Inspect `git diff --stat` and the full diff.
3. Run a C++ compile or Unreal commandlet smoke check when the local UE installation is available.
4. Confirm no generated editor/cache output is staged.

## Acceptance

This PR is accepted when the repo has a clean, reviewable source-of-truth for the 16.256 km island foundation and future terrain import/tooling PRs can target the same constants.
