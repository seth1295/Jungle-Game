# Full Terrain Math Acceptance And Handoff Research

## Executive Summary

This document defines the final terrain-only acceptance standard for the Jungle Game Batch 005 / 006 terrain math lane. The current known state is that the deterministic 97.536 km volcanic island preview has working coast and scale gates, while the interior terrain fails because it still reads as a single procedural radial volcano. The acceptance target is therefore not merely “pretty terrain”; it is reproducible evidence that the terrain generator now produces a multi-source, multi-process island landform with believable hydrology, ridge/valley structure, volcanic features that are subordinate to the broader landform, and runtime import evidence in UE5.

The hard gates are non-negotiable:

```text
world_size_m: 97536.0 target
shoreline_error_max_m: 0 or effectively 0
beach_continuity_pct: 100%
ocean_below_sea_pct: 100%
square_edge_ocean_violations: 0
outer world edge remains ocean/container terrain
```

The additional hard acceptance gate is qualitative and quantitative: the terrain must no longer be one-feature radial volcano terrain. A reviewer must be able to disable the volcanic subsystem and still see a coherent island with independent ridge systems, valley networks, catchments, coast/shore continuity, and a non-radial elevation structure. Volcanic features may exist, but they cannot own the terrain foundation.

The acceptance package must include source identity, deterministic run identity, preview images, manifest metrics, hydrology/catchment analysis, ridge/valley graph evidence, feature grammar evidence, volcano-disabled proof, calibration summary, UE5 runtime terrain shell evidence, and project-state handoff notes. The manifest and previews must be written under `Images/TerrainPreview/` so the next agent can inspect output evidence without reconstructing context.

The standard below intentionally separates completed Batch 004 work from new Batch 005 / 006 terrain replacement work. Batch 004 may be recorded as the historical coast/scale milestone. Batch 005 / 006 must not be marked complete until the new terrain foundation passes the acceptance evidence described here.

## Terrain-Only Scope Boundary

This document is strictly about terrain acceptance, validation evidence, project state, and handoff. It does not authorize work on:

- foliage, ecology, biomes, or vegetation placement;
- weather, climate, time of day, atmosphere, or audio;
- NPCs, animals, survival loops, crafting, combat, AI, or gameplay systems;
- buildings, roads, settlements, caves as gameplay spaces, or props;
- final materials, shaders, decals, water shading, or art polish;
- UI, map, compass, minimap, GPS, objective markers, or player-facing overlays.

Allowed terrain-only outputs are heightfields, masks, terrain shell actors, debug previews, validation renders, derived rasters, graph/manifest files, and documentation that proves acceptance. Temporary debug colors, contour overlays, hydrology overlays, and slope/aspect visualizations are allowed only as evidence images. They must not be presented as player-facing UI or final art.

## Batch 004 Historical Failure Summary

Batch 004 should be described as a completed historical coast/scale preview milestone, not as a completed terrain foundation. Its value is that it established a deterministic island footprint and proved the outer terrain containment constraints:

- the preview targets a 97.536 km world;
- the coastline and square-edge ocean/container constraints are known acceptance targets;
- the edge remains ocean/container terrain;
- the preview pipeline can produce inspectable terrain images and metrics.

The failure is that the interior terrain is still dominated by a radial procedural volcano. This means the terrain reads as one synthetic cone or one volcano-owned landform instead of a terrain system. The visible symptoms likely include:

- elevation falling primarily as a function of distance from a central cone;
- ridges radiating outward from one dominant volcanic center;
- valleys reading as radial grooves rather than catchment-owned drainage networks;
- coastline success masking an interior morphology failure;
- volcanic subsystem output being indistinguishable from the base terrain foundation.

Required project-state wording:

```text
Batch 004: coast/scale preview gate completed; interior terrain rejected because it is still dominated by a one-feature radial volcano.
Batch 005/006: active replacement terrain math lane; acceptance requires source identity, hydrology, ridge/valley graph, volcanic subsystem isolation, feature grammar, calibration, preview, manifest, and runtime terrain shell evidence.
```

Forbidden project-state wording:

```text
Batch 004 completed terrain math.
Terrain accepted because the island scale and coast are correct.
Volcano terrain accepted pending foliage/material polish.
```

## Batch 005 / 006 Replacement Goal

The replacement goal is to turn the island from a radial-volcano height function into a reproducible terrain system. The system may still be volcanic in origin, but the accepted terrain must read as a mature island landform shaped by interacting terrain processes, not a centered cone with noise applied.

The generator should be evaluated as a pipeline with separable stages:

1. **Source terrain identity / source morphology** — clear source inputs, seeds, source hashes, or reference terrain descriptors.
2. **Base island support / coastline preservation** — the known coastline/scale gates remain intact.
3. **Macro relief foundation** — large-scale non-radial elevation domains and mountain massing.
4. **Hydrology and catchment layout** — flow direction, accumulation, drainage basins, outlets, and true sinks.
5. **Ridge/valley graph generation** — explicit landform graph evidence rather than unstructured noise.
6. **Volcanic subsystem** — localized volcanic edits, cones, vents, caldera/rift/lava/collapse features that remain subordinate.
7. **Feature grammar / multi-scale detail** — rules that attach terrain detail to landform context rather than applying uniform noise.
8. **Calibration and validation** — metrics, image evidence, source comparison, and failure flags.
9. **Runtime terrain shell** — UE5 import/runtime evidence matching the accepted height data.

A successful Batch 005 / 006 result must pass both **data acceptance** and **visual acceptance**. Numeric metrics can fail a build even when screenshots look good. Screenshots can fail a build even when metrics pass if the terrain still reads as radial, volcano-owned, smeared, or procedurally noisy.

## Required Source Evidence

Every accepted terrain run must produce a source evidence block in the manifest. The block must answer: what terrain identity did this run derive from, what code/data produced it, and can it be reproduced exactly?

Minimum source evidence:

```json
{
  "source_terrain": {
    "identity": "jungle_game_batch_005_006_source_v1",
    "source_type": "synthetic_reference_pipeline | real_dem_reference | hybrid",
    "source_description": "Short human-readable identity of the terrain source and intended morphology.",
    "source_files": [
      {
        "path": "relative/path/to/source_or_config.ext",
        "sha256": "...",
        "role": "height_reference | coast_mask | calibration_profile | generator_config"
      }
    ],
    "license_or_provenance": "Internal generated source / public DEM source / other explicit provenance.",
    "seed": 0,
    "coordinate_space": {
      "world_size_m": 97536.0,
      "height_units": "meters",
      "horizontal_units": "meters",
      "origin": "documented project terrain origin"
    },
    "transform_history": [
      "crop",
      "resample",
      "normalize",
      "coast-fit",
      "hydrology-condition"
    ],
    "determinism": {
      "code_commit": "git sha",
      "generator_version": "terrain_math_v005_006",
      "config_sha256": "...",
      "output_height_sha256": "..."
    }
  }
}
```

If a real DEM, satellite-derived DEM, or reference raster is used for calibration, the source must record its dataset name, download/access date, projection if applicable, transform steps, resampling method, nodata handling, and hash. GDAL is appropriate for raster translation and processing evidence because it provides a common raster model and command-line utilities for geospatial data workflows.[^gdal]

If the source is synthetic only, the source identity must still be explicit. “Generated from seed” is not enough. The source evidence must state the landform intent, grammar version, stage list, parameter file, seed, and output hashes.

Minimum source stop conditions:

- missing source hash;
- missing seed/config identity;
- no distinction between source terrain, generator edits, and preview/render outputs;
- source terrain is secretly the old radial volcano output;
- source identity cannot reproduce the same heightfield hash from a clean run.

## Required Preview Evidence

The acceptance run must create an output folder under `Images/TerrainPreview/`:

```text
Images/TerrainPreview/005_full_terrain_math_acceptance/
```

Required files:

```text
Images/TerrainPreview/005_full_terrain_math_acceptance/
  005_acceptance_manifest.json
  005_acceptance_summary.md
  005_acceptance_log.txt
  005_project_state_patch.md
  previews/
    005_height_overview.png
    005_slope_map.png
    005_aspect_map.png
    005_curvature_or_relief_map.png
    005_coast_gate_overlay.png
    005_beach_continuity_overlay.png
    005_hydrology_flow_accumulation.png
    005_catchments_colored.png
    005_ridge_valley_graph.png
    005_landform_regions.png
    005_volcanic_features_enabled.png
    005_volcanic_features_disabled.png
    005_enabled_disabled_delta.png
    005_runtime_ue5_landscape_shell_overview.png
    005_runtime_ue5_landscape_shell_low_angle.png
    005_runtime_ue5_landscape_shell_debug_height.png
  data/
    005_height_raw_or_r16.*
    005_height_normalized_preview.*
    005_coast_mask.*
    005_land_mask.*
    005_beach_mask.*
    005_flow_direction.*
    005_flow_accumulation.*
    005_catchments.*
    005_ridge_graph.*
    005_valley_graph.*
    005_feature_regions.*
```

The preview set must include both top-down diagnostic views and low-angle terrain shell views. Top-down maps prove metrics and structure. Low-angle UE5 shell renders prove whether the terrain reads as an actual explorable landform. Epic documents UE Landscapes as component-based outdoor terrain built from heightmap dimensions and height data, so the runtime preview must prove the accepted heightfield enters the actual Landscape pathway or the project’s terrain-shell equivalent rather than remaining as an offline image only.[^epic-landscape]

For high-quality rendered evidence, Unreal’s Movie Render Queue is suitable when configured for offline output image sequences and controlled render settings.[^epic-mrq] However, MRQ polish must not conceal failed morphology. Debug overlays and raw height/slope/catchment maps remain primary evidence.

Required preview captions in `005_acceptance_summary.md`:

- what each image proves;
- whether the image is generator-space, validation-space, or UE5 runtime-space;
- seed/config/run id;
- whether volcano subsystem is enabled or disabled;
- whether the image is allowed as acceptance evidence or only supplementary.

Preview stop conditions:

- no volcano-disabled preview;
- no hydrology/catchment preview;
- only beauty shots, no diagnostic maps;
- UE5 screenshot uses a different heightfield than the manifest hash;
- preview images are overwritten without preserving run id or manifest hash.

## Required Manifest Metrics

The final manifest must be a deterministic machine-readable JSON file. It should be strict enough that a later acceptance script can fail the run without reinterpreting screenshots.

Required manifest path:

```text
Images/TerrainPreview/005_full_terrain_math_acceptance/005_acceptance_manifest.json
```

Required top-level schema:

```json
{
  "schema_version": "jungle_terrain_acceptance_manifest.v1",
  "project": "Jungle Game",
  "batch": "005_006_full_terrain_math_acceptance",
  "run": {
    "run_id": "YYYYMMDD_HHMMSS_seedNNNN_commitSHORTSHA",
    "timestamp_utc": "2026-06-27T00:00:00Z",
    "timezone_note": "timestamps in UTC; handoff date may be Australia/Brisbane local",
    "git_commit": "full git sha",
    "git_branch": "branch name",
    "working_tree_clean": true,
    "generator_command": "exact command used",
    "validator_command": "exact command used",
    "ue5_command_or_editor_steps": "exact shell/editor automation path where available",
    "seed": 0,
    "deterministic_replay_hash": "sha256 of replay-critical config"
  },
  "source_terrain": {},
  "world": {},
  "coast_gates": {},
  "morphology_gates": {},
  "hydrology": {},
  "ridge_valley_graph": {},
  "volcanic_subsystem": {},
  "feature_grammar": {},
  "calibration": {},
  "runtime_terrain_shell": {},
  "artifacts": {},
  "acceptance": {
    "overall_status": "PASS | FAIL",
    "blocking_failures": [],
    "warnings": [],
    "reviewer_notes": []
  }
}
```

Required world metrics:

```json
{
  "world": {
    "world_size_m_target": 97536.0,
    "world_size_m_actual": 97536.0,
    "cell_size_m": 0.0,
    "height_min_m": 0.0,
    "height_max_m": 0.0,
    "sea_level_m": 0.0,
    "land_cell_count": 0,
    "ocean_cell_count": 0,
    "land_area_km2": 0.0,
    "ocean_area_km2": 0.0,
    "heightfield_resolution": [0, 0],
    "heightfield_format": "r16 | png16 | exr | npy | other",
    "heightfield_sha256": "..."
  }
}
```

Required artifact hashes:

```json
{
  "artifacts": {
    "manifest_sha256": "filled after write or stored separately",
    "summary_md": "Images/TerrainPreview/005_full_terrain_math_acceptance/005_acceptance_summary.md",
    "height_raw_sha256": "...",
    "coast_mask_sha256": "...",
    "flow_accumulation_sha256": "...",
    "catchments_sha256": "...",
    "ridge_graph_sha256": "...",
    "valley_graph_sha256": "...",
    "ue5_runtime_preview_images": [
      {
        "path": "previews/005_runtime_ue5_landscape_shell_overview.png",
        "sha256": "..."
      }
    ]
  }
}
```

Recommended pass/fail enum:

```text
PASS         hard gate passed
FAIL         hard gate failed
WARN         non-blocking evidence issue
NOT_RUN      required test did not run; blocks final acceptance unless explicitly optional
N/A          not applicable; must include reason
```

Manifest stop conditions:

- missing `overall_status`;
- any required hard gate reports `NOT_RUN`;
- any artifact listed without a hash;
- generated run cannot be replayed from manifest command/config;
- final manifest lacks both visual and numeric evidence.

## Coast And Scale Hard Gates

The coast/scale gates are inherited from the known working preview and remain hard blockers. They must pass in the new terrain foundation without regression.

Required coast gate manifest block:

```json
{
  "coast_gates": {
    "world_size_m": {
      "target": 97536.0,
      "actual": 97536.0,
      "tolerance_m": 0.0,
      "status": "PASS"
    },
    "shoreline_error_max_m": {
      "target_max": 0.0,
      "actual": 0.0,
      "effective_zero_tolerance_m": 0.0,
      "status": "PASS"
    },
    "beach_continuity_pct": {
      "target_min": 100.0,
      "actual": 100.0,
      "status": "PASS"
    },
    "ocean_below_sea_pct": {
      "target_min": 100.0,
      "actual": 100.0,
      "status": "PASS"
    },
    "square_edge_ocean_violations": {
      "target": 0,
      "actual": 0,
      "status": "PASS"
    },
    "outer_world_edge": {
      "target": "ocean/container terrain",
      "actual": "ocean/container terrain",
      "status": "PASS"
    }
  }
}
```

Acceptance rules:

- The 97.536 km target must be reported in meters and not rounded to “about 100 km.”
- Coastline validation must run on the same heightfield used by hydrology and UE5 shell evidence.
- Beach continuity must be evaluated from the coast/beach mask, not eyeballed from a render.
- Ocean-below-sea must test all ocean cells, including corners and square world edges.
- Edge containment must remain ocean/container terrain even if the interior terrain foundation changes.

Stop conditions:

- any coast gate regresses while fixing interior terrain;
- coast mask and heightfield hashes do not match the same run;
- world size is achieved only by UE scale tricks while source metrics remain wrong;
- beach continuity is less than 100%;
- any square-edge ocean violation exists.

## Morphology Hard Gates

Morphology acceptance proves the island is no longer one-feature radial volcano terrain. This is the central Batch 005 / 006 acceptance requirement.

Required morphology manifest block:

```json
{
  "morphology_gates": {
    "radial_dominance_r2": {
      "description": "R^2 of elevation explained by distance from rejected volcano/radial center over land cells.",
      "target_max": 0.35,
      "actual": 0.0,
      "status": "PASS"
    },
    "azimuthal_relief_uniformity": {
      "description": "How uniformly relief radiates from one center; lower is less cone-like.",
      "target_max": 0.45,
      "actual": 0.0,
      "status": "PASS"
    },
    "single_peak_landform_dominance_pct": {
      "description": "Percent of land area morphologically owned by the largest cone/peak domain.",
      "target_max": 20.0,
      "actual": 0.0,
      "status": "PASS"
    },
    "independent_macro_landform_count": {
      "description": "Distinct macro landform regions not explainable as one volcano slope.",
      "target_min": 4,
      "actual": 0,
      "status": "PASS"
    },
    "volcano_disabled_base_still_valid": {
      "description": "Base terrain without volcano subsystem still has coherent relief, catchments, ridges, valleys, and coast gates.",
      "target": true,
      "actual": true,
      "status": "PASS"
    },
    "visual_radial_rejection_review": {
      "reviewers": ["agent_or_human_reviewer"],
      "status": "PASS",
      "notes": "Must state why terrain no longer reads as a radial cone."
    }
  }
}
```

Recommended morphology calculations:

1. Fit elevation over land cells against distance to the old volcano center and report `radial_dominance_r2`.
2. Repeat against the highest peak and any detected volcanic center.
3. Compare elevation profiles by azimuth bins. A cone-like surface has similar downslope behavior in many directions; accepted terrain should show anisotropy driven by catchments, ridges, basins, and coastal constraints.
4. Segment macro landform regions by relief, slope, curvature, catchment boundaries, and ridge/valley graphs.
5. Detect dominant peaks/cones and calculate ownership domains.
6. Run all tests twice: volcano enabled and volcano disabled.

The target values above are acceptance defaults. If later calibration proves a threshold too strict or too lax, the threshold may be changed only with a source-calibration note and before final acceptance. Threshold changes made after seeing a failed terrain run are not valid unless clearly documented as a new calibration pass.

Visual hard fail examples:

- one central cone with radial grooves;
- volcano-disabled terrain becomes flat, empty, or incoherent;
- catchments are merely radial spokes;
- ridge graph is a star centered on the volcano;
- volcano feature mask covers most of the island;
- noise hides the cone but does not create true landform structure.

## Hydrology / Catchment Acceptance

Hydrology acceptance proves that terrain structure supports believable drainage. The terrain does not need real-world survey accuracy, but it must produce coherent flow directions, accumulation, basins, outlets, and non-radial catchment layout.

USGS hydro-conditioned DEM guidance is useful here because it frames hydrologic validity around continuous flow, downstream monotonicity, drainage to outlet points or true sinks, and derivatives such as watershed boundaries, flow direction, and flow accumulation.[^usgs-hydro] DEM preprocessing methods such as Priority-Flood are also relevant because they are designed to remove or resolve depressions so every cell can drain or contribute to watershed labeling.[^priority-flood]

Required hydrology manifest block:

```json
{
  "hydrology": {
    "flow_direction_method": "D8 | D-infinity | MFD | project-specific",
    "depression_handling": "priority_flood | breach | fill | explicit_true_sinks | none",
    "flat_resolution_method": "documented method",
    "flow_direction_hash": "...",
    "flow_accumulation_hash": "...",
    "catchment_hash": "...",
    "catchment_count_total": 0,
    "catchment_count_major": 0,
    "major_catchment_min_area_km2": 0.0,
    "largest_catchment_area_pct_of_land": 0.0,
    "outlet_count": 0,
    "coastal_outlet_count": 0,
    "interior_true_sink_count": 0,
    "unresolved_sink_count": 0,
    "flow_to_ocean_pct": 0.0,
    "radial_spoke_catchment_score": 0.0,
    "status": "PASS"
  }
}
```

Minimum hydrology gates:

| Gate | Target |
| --- | --- |
| `unresolved_sink_count` | `0`, unless explicitly documented true sinks are intentional and tiny |
| `flow_to_ocean_pct` | high enough to prove island drainage reaches ocean; recommended `>= 99%` excluding accepted true sinks |
| `catchment_count_major` | recommended `>= 6` major coastal catchments |
| `largest_catchment_area_pct_of_land` | recommended `<= 35%` unless source calibration justifies otherwise |
| `radial_spoke_catchment_score` | must not indicate a pure star pattern from one volcano center |
| coastal outlets | distributed around coastline, not clustered at one artificial break |

Hydrology preview requirements:

- flow accumulation map;
- colored catchment map;
- outlet markers;
- unresolved sink markers if any;
- overlay showing catchments are not just radial spokes from the rejected volcano center;
- volcano-enabled and volcano-disabled catchment comparison.

Hydrology stop conditions:

- flow accumulation cannot be generated;
- catchment map is not derived from the accepted heightfield;
- rivers/valleys are hand-drawn separately from terrain slopes without reconciliation;
- hydrology only works after modifying the coastline hard gates;
- catchments remain radial spokes.

## Ridge / Valley / Landform Acceptance

Ridge/valley acceptance proves that the island has a landform graph, not just noise over a coast mask. The generator may use a graph-first approach, a terrain-first extraction approach, or both, but the final evidence must expose the graph and show it matches the heightfield.

Procedural terrain literature supports graph- or process-based approaches for terrain detail because they provide controllable structure rather than merely adding random noise. For example, fluvial erosion terrain work has used graph representations to improve terrain variation and designer control.[^erosion-graph]

Required ridge/valley manifest block:

```json
{
  "ridge_valley_graph": {
    "method": "generated_graph | extracted_graph | hybrid",
    "ridge_graph_path": "Images/TerrainPreview/005_full_terrain_math_acceptance/data/005_ridge_graph.*",
    "valley_graph_path": "Images/TerrainPreview/005_full_terrain_math_acceptance/data/005_valley_graph.*",
    "ridge_node_count": 0,
    "ridge_edge_count": 0,
    "valley_node_count": 0,
    "valley_edge_count": 0,
    "ridge_valley_alignment_score": 0.0,
    "ridge_radial_star_score": 0.0,
    "valley_radial_star_score": 0.0,
    "branching_complexity_index": 0.0,
    "major_ridge_system_count": 0,
    "major_valley_system_count": 0,
    "graph_heightfield_consistency": "PASS | FAIL",
    "status": "PASS"
  }
}
```

Minimum gates:

- at least several major ridge systems independent of the old volcano center;
- valleys must align with low-curvature / convergent flow regions;
- ridges must align with divides between catchments;
- graph edges must not form a simple radial star;
- graph evidence must include visual overlay and machine-readable data;
- volcano-disabled graph must remain coherent.

Recommended graph exports:

```text
005_ridge_graph.geojson
005_valley_graph.geojson
005_landform_regions.geojson
005_graph_metrics.json
```

If GeoJSON is not appropriate for internal coordinates, use JSON with explicit coordinate units:

```json
{
  "coordinate_units": "meters",
  "origin": "terrain local origin",
  "nodes": [{ "id": "r1", "x_m": 0.0, "y_m": 0.0, "z_m": 0.0, "kind": "ridge" }],
  "edges": [{ "id": "e1", "a": "r1", "b": "r2", "kind": "ridge_spine" }]
}
```

Ridge/valley stop conditions:

- no graph export;
- graph export exists but is not overlaid on the accepted heightfield;
- graph is a radial star;
- valleys climb uphill or cross ridges without explanation;
- graph only exists as debug art, not data.

## Volcanic Subsystem Acceptance

Volcanic subsystem acceptance proves two things at once:

1. the island can still support volcanic identity where desired;
2. the volcano subsystem no longer owns the entire terrain.

The volcanic subsystem must be isolated, toggleable, and measurable. It may add cones, vents, caldera/collapse features, rifts, lava flows, ash plains, lava benches, flank scars, or crater rims. It must not be the base height function for the whole island.

Required volcanic subsystem manifest block:

```json
{
  "volcanic_subsystem": {
    "enabled": true,
    "toggle_name": "enable_volcanic_subsystem",
    "disabled_run_id": "matching run id for volcano-disabled proof",
    "feature_count_total": 0,
    "feature_types": {
      "cones": 0,
      "vents": 0,
      "caldera_or_collapse": 0,
      "rifts": 0,
      "lava_flows": 0,
      "benches": 0,
      "scarps": 0
    },
    "modified_land_area_pct": 0.0,
    "max_height_delta_m": 0.0,
    "mean_abs_height_delta_m": 0.0,
    "coast_delta_max_m": 0.0,
    "catchment_delta_summary": "short summary",
    "volcano_ownership_score": 0.0,
    "base_terrain_without_volcano_status": "PASS",
    "status": "PASS"
  }
}
```

Recommended gates:

| Gate | Target |
| --- | --- |
| `base_terrain_without_volcano_status` | `PASS` |
| `coast_delta_max_m` | `0` or effectively `0` unless deliberately allowed and still coast-gate-valid |
| `modified_land_area_pct` | recommended `<= 20%` for localized volcanic edits; justify any higher value |
| `volcano_ownership_score` | must be low enough to prove volcano is a subsystem, not the base terrain |
| disabled preview | required |
| enabled-disabled delta map | required |

Required proof images:

```text
previews/005_volcanic_features_enabled.png
previews/005_volcanic_features_disabled.png
previews/005_enabled_disabled_delta.png
previews/005_landform_regions.png
```

Stop conditions:

- disabling volcano removes most terrain identity;
- volcano disabled output has no meaningful catchments/ridges/valleys;
- enabled-disabled delta shows most of the island was volcano-authored;
- a single cone still visually dominates the island;
- volcanic feature masks are not exported.

## Feature Grammar Acceptance

Feature grammar acceptance proves multi-scale detail is placed by landform rules rather than uniform noise or post-hoc texture. The feature grammar may operate on height, slope, curvature, flow accumulation, catchments, ridges, valleys, volcanic masks, coastal masks, and distance-to-shore.

A feature grammar is acceptable only if it is explicit, deterministic, and tied to terrain context.

Required feature grammar manifest block:

```json
{
  "feature_grammar": {
    "grammar_version": "terrain_feature_grammar_v1",
    "rules_hash": "sha256",
    "rule_count": 0,
    "feature_layers": [
      "coast_beach_transition",
      "major_ridge_spines",
      "secondary_spurs",
      "valley_floors",
      "incised_channels",
      "colluvial_slopes",
      "volcanic_flows",
      "cliffs_or_scarps"
    ],
    "context_inputs": [
      "height",
      "slope",
      "curvature",
      "flow_accumulation",
      "catchments",
      "ridge_graph",
      "valley_graph",
      "volcanic_mask",
      "coast_mask"
    ],
    "uniform_noise_area_pct": 0.0,
    "contextual_feature_area_pct": 0.0,
    "invalid_feature_overlap_count": 0,
    "status": "PASS"
  }
}
```

Required rule documentation example:

```json
{
  "rule_id": "secondary_spurs_from_major_ridges_v1",
  "inputs": ["ridge_graph", "slope", "catchment_divides"],
  "operation": "spawn tapered spur relief from ridge nodes with catchment-aware termination",
  "constraints": ["must not cross valley floor", "must taper before beach mask"],
  "outputs": ["spur_height_delta", "spur_mask"],
  "determinism": "seeded stable ordering by graph id"
}
```

Minimum gates:

- rule list exported;
- each rule has inputs, outputs, constraints, and deterministic ordering;
- detail features attach to landform context;
- feature masks are exported or inspectable;
- grammar cannot overwrite coast gates;
- grammar cannot hide radial failure with high-frequency noise.

Stop conditions:

- terrain detail is mostly uniform fractal/noise displacement;
- rules are not reproducible;
- features are generated after validation and not included in manifest metrics;
- feature masks show invalid overlaps, such as channels crossing ridge crests without terrain support;
- grammar acts as visual camouflage for an unchanged radial base.

## Calibration Acceptance

Calibration acceptance explains why the thresholds and metrics are credible. The terrain does not need to match a single real island perfectly, but it must be calibrated against believable terrain structure or against clearly stated project-specific morphology goals.

Calibration can use:

- reference DEM slope/aspect/curvature distributions;
- island-scale catchment count and outlet distribution targets;
- ridge/valley branching distributions;
- volcanic feature size and area ratios;
- hand-authored target ranges agreed before final run;
- comparison against previous failed Batch 004 metrics.

Required calibration manifest block:

```json
{
  "calibration": {
    "calibration_version": "terrain_calibration_v1",
    "calibration_sources": [
      {
        "name": "reference_or_internal_target",
        "type": "DEM | internal_target | previous_failed_run",
        "sha256": "...",
        "role": "slope_distribution | catchment_targets | radial_failure_baseline"
      }
    ],
    "metrics": {
      "slope_histogram_distance": 0.0,
      "aspect_distribution_notes": "...",
      "curvature_distribution_notes": "...",
      "hypsometric_curve_distance": 0.0,
      "catchment_distribution_notes": "...",
      "radial_failure_baseline_delta": 0.0
    },
    "threshold_changes": [
      {
        "metric": "radial_dominance_r2",
        "old": 0.35,
        "new": 0.35,
        "reason": "No change"
      }
    ],
    "status": "PASS"
  }
}
```

DEM analysis should use documented raster operations and reproducible preprocessing. GDAL can support raster format translation and processing evidence, while hydrologic conditioning should follow documented flow-direction and watershed principles rather than visual interpretation alone.[^gdal][^usgs-hydro]

Calibration must include the old failure baseline if the old Batch 004 output is available:

```text
old_radial_dominance_r2: measured from rejected Batch 004 terrain
new_radial_dominance_r2: measured from Batch 005/006 candidate
required_delta: new must be materially lower and under target_max
```

Stop conditions:

- thresholds are invented after the run to justify passing;
- calibration sources are not identified or hashed;
- no comparison to the rejected radial failure exists when old output is available;
- terrain only passes metrics because measurements exclude obvious failed regions;
- calibration is described only with aesthetic language.

## Runtime Terrain Shell Acceptance

Runtime terrain shell acceptance proves the validated terrain can enter UE5 as terrain data and still read correctly at runtime/editor preview scale. This is not final art acceptance. It is a terrain data parity check.

Epic’s Landscape documentation identifies valid heightmap dimensions, Landscape Components, Z-scale conversion, and import formats such as 16-bit grayscale PNG and 16-bit R16 heightmaps.[^epic-landscape] The acceptance evidence must therefore record the heightmap format, dimensions, Z scale, and component settings used to preview the terrain shell.

Required runtime manifest block:

```json
{
  "runtime_terrain_shell": {
    "engine": "Unreal Engine 5",
    "platform_target": "Linux-first",
    "height_source_path": "Images/TerrainPreview/005_full_terrain_math_acceptance/data/005_height_raw_or_r16.*",
    "height_source_sha256": "...",
    "ue_landscape_or_shell_actor": "Landscape | ProceduralMesh | RuntimeVirtualHeightfieldMesh | project-specific shell",
    "heightmap_resolution": [0, 0],
    "landscape_components": "documented if using Landscape",
    "section_size": "documented if using Landscape",
    "z_scale": 0.0,
    "xy_scale_m": 0.0,
    "sea_level_alignment_m": 0.0,
    "world_size_m_actual_in_ue": 97536.0,
    "runtime_preview_images": [
      "previews/005_runtime_ue5_landscape_shell_overview.png",
      "previews/005_runtime_ue5_landscape_shell_low_angle.png",
      "previews/005_runtime_ue5_landscape_shell_debug_height.png"
    ],
    "automation_or_manual_steps": "exact command/steps",
    "status": "PASS"
  }
}
```

Minimum runtime evidence:

- imported shell uses the same height hash as the accepted manifest;
- UE scale reproduces the 97.536 km target;
- sea level alignment is documented;
- Z-scale conversion is documented;
- low-angle preview shows landform readability without foliage/material camouflage;
- debug height/slope/contour material or overlay exists for review;
- no UI/map/GPS/objective overlay work is introduced.

Unreal’s Automation Test Framework and Functional Testing pathway can be used for repeatable editor or runtime checks where appropriate; the important part is that runtime evidence is reproducible and not a one-off screenshot with unknown import settings.[^epic-automation]

Stop conditions:

- UE preview is not generated from the accepted heightfield;
- heightmap dimensions/import settings are undocumented;
- runtime preview is only a beauty render with final materials;
- world scale differs from accepted manifest;
- runtime shell introduces unrelated gameplay/UI/material scope.

## Handoff File Requirements

The final handoff must give the next agent or developer enough information to resume without guessing. Required handoff files:

```text
Docs/Terrain/005_full_terrain_math_acceptance_handoff.md
Images/TerrainPreview/005_full_terrain_math_acceptance/005_acceptance_manifest.json
Images/TerrainPreview/005_full_terrain_math_acceptance/005_acceptance_summary.md
Images/TerrainPreview/005_full_terrain_math_acceptance/005_acceptance_log.txt
Images/TerrainPreview/005_full_terrain_math_acceptance/005_project_state_patch.md
```

If the repository uses a different docs path, keep the file basename exactly:

```text
005_full_terrain_math_acceptance_handoff.md
```

The handoff Markdown must include:

- current git commit and branch;
- exact generator command;
- exact validator command;
- exact UE5 import/runtime shell command or manual steps;
- manifest path and hash;
- preview folder path;
- PASS/FAIL status for each hard gate;
- list of known warnings;
- exact language to update project state;
- next recommended implementation prompt only if all gates pass or clear recovery prompt if gates fail.

Required `005_project_state_patch.md` language:

```markdown
## Terrain Math State

Batch 004 remains accepted only as the historical coast/scale preview milestone. It is not the accepted interior terrain foundation.

Batch 005 / 006 terrain math acceptance status: PASS|FAIL.

Evidence folder: `Images/TerrainPreview/005_full_terrain_math_acceptance/`
Manifest: `Images/TerrainPreview/005_full_terrain_math_acceptance/005_acceptance_manifest.json`

Current result summary:
- world size target: 97,536.0 m
- coast hard gates: PASS|FAIL
- morphology radial rejection: PASS|FAIL
- hydrology/catchments: PASS|FAIL
- ridge/valley graph: PASS|FAIL
- volcanic subsystem isolation: PASS|FAIL
- feature grammar: PASS|FAIL
- calibration: PASS|FAIL
- runtime terrain shell: PASS|FAIL

Do not mark Batch 005 / 006 complete unless all required hard gates and evidence gates pass.
```

Required handoff naming for failure states:

```text
005_full_terrain_math_acceptance_handoff_FAIL.md
005_full_terrain_math_acceptance_recovery_notes.md
```

Required handoff naming for passing states:

```text
005_full_terrain_math_acceptance_handoff_PASS.md
```

## Recovery Instructions

Use this recovery order if acceptance fails or if a future agent starts with no context.

1. **Open the latest manifest first.**
   - Path: `Images/TerrainPreview/005_full_terrain_math_acceptance/005_acceptance_manifest.json`
   - If missing, acceptance has not been completed.

2. **Check hard coast gates before interior debugging.**
   - If coast/scale regressed, recover coast gates before evaluating morphology.
   - Do not accept an interior improvement that breaks shoreline, beach, ocean, or edge containment gates.

3. **Check radial rejection metrics and images.**
   - Review `005_height_overview.png`, `005_slope_map.png`, `005_catchments_colored.png`, and `005_ridge_valley_graph.png`.
   - If the terrain still reads as a cone, fail immediately.

4. **Disable volcanic subsystem.**
   - Compare `005_volcanic_features_disabled.png` to `005_volcanic_features_enabled.png`.
   - If the disabled version collapses into flat/empty terrain, the base foundation is not accepted.

5. **Inspect hydrology.**
   - Flow accumulation, catchments, and outlets must be derived from the same heightfield hash.
   - If catchments are radial spokes, return to terrain foundation/graph generation, not feature grammar polish.

6. **Inspect ridge/valley graph.**
   - If the graph is missing, not overlaid, or star-shaped, fail.

7. **Inspect feature grammar.**
   - If detail is just noise, fail or downgrade to pre-acceptance.
   - Do not let feature grammar hide base morphology failure.

8. **Inspect UE5 runtime shell.**
   - Confirm heightfield hash, world scale, Z scale, sea level, and debug images.
   - If runtime shell differs from validated data, fail runtime acceptance.

9. **Update handoff state honestly.**
   - Use PASS only when all required gates pass.
   - Use FAIL with exact stop condition otherwise.

Recovery prompts must not ask for foliage, materials, weather, gameplay, map UI, or art polish. The correct recovery target is the terrain math foundation.

## Files And Areas Likely Touched

Exact repository paths may differ, but implementation is likely to touch these areas:

```text
Source/...
  Terrain generator code
  Terrain validation code
  Runtime terrain shell/import code

Content/...
  Temporary terrain shell level or debug assets only
  Debug materials for height/slope/catchment visualization only

Config/...
  Terrain generator config
  Validation thresholds
  UE5 runtime shell import settings if stored in config

Scripts/ or Tools/...
  Terrain generation CLI
  Acceptance validation CLI
  Preview image generation
  Manifest hashing/reporting

Tests/...
  Determinism tests
  Coast gate tests
  Hydrology/catchment tests
  Morphology/radial rejection tests
  Runtime shell smoke/automation tests if practical

Docs/Terrain/...
  Handoff file
  Acceptance summary
  Project state patch

Images/TerrainPreview/005_full_terrain_math_acceptance/...
  Manifest
  Preview images
  Derived validation rasters/graphs
  Logs
```

Implementation should avoid unrelated edits. If a future PR includes foliage, ecology, gameplay, final material polish, UI, map, compass, minimap, GPS, objective marker, or non-terrain work, the scope should be rejected or split.

## Acceptance Criteria

A final Batch 005 / 006 terrain math acceptance pass requires all of the following:

### 1. Source and reproducibility

- Source terrain identity documented.
- Generator seed/config/commit recorded.
- All key inputs and outputs hashed.
- Clean deterministic replay produces the same heightfield hash.
- Source evidence distinguishes source terrain, generator stages, validation rasters, preview images, and UE5 runtime shell.

### 2. Coast and scale

- `world_size_m` target: `97536.0`.
- `shoreline_error_max_m`: `0` or effectively `0` with explicit tolerance.
- `beach_continuity_pct`: `100%`.
- `ocean_below_sea_pct`: `100%`.
- `square_edge_ocean_violations`: `0`.
- Outer world edge remains ocean/container terrain.

### 3. Morphology

- Terrain no longer reads as one-feature radial volcano.
- Radial dominance metrics pass.
- Independent macro landform regions exist.
- Volcano-disabled terrain still passes base structure review.
- Visual review explicitly rejects radial/cone ownership.

### 4. Hydrology/catchments

- Flow direction and accumulation generated from accepted heightfield.
- Catchment map exported and previewed.
- Outlets and unresolved sinks reported.
- Catchments are not radial spokes from one center.
- Hydrology remains coherent when volcano is disabled.

### 5. Ridge/valley graph

- Ridge and valley graphs exported as data.
- Graph overlays align with height/slope/catchment evidence.
- Graphs are not star-shaped around a volcano center.
- Major ridge/valley system counts and graph consistency metrics pass.

### 6. Volcanic subsystem

- Volcanic subsystem is toggleable.
- Enabled/disabled/delta previews exist.
- Base terrain remains valid without volcanic subsystem.
- Volcanic modifications are localized or explicitly justified.
- Volcano does not control coast, macro terrain, catchments, or graph structure.

### 7. Feature grammar

- Rule set exported.
- Rules are deterministic and context-bound.
- Feature masks/layers are inspectable.
- Noise is subordinate to landform grammar.
- Grammar does not overwrite hard coast gates.

### 8. Calibration

- Calibration sources or internal target ranges documented.
- Thresholds recorded before final acceptance.
- Old Batch 004 radial failure baseline compared if available.
- Slope/aspect/curvature/hypsometry/catchment metrics summarized.

### 9. Runtime shell

- UE5 terrain shell generated from accepted height hash.
- Heightmap format, dimensions, XY scale, Z scale, and sea level documented.
- Runtime preview images exported.
- Runtime view still reads as non-radial terrain without art camouflage.

### 10. Handoff

- Manifest, summary, log, previews, source data, and handoff files present.
- Project-state language is updated with PASS/FAIL honesty.
- Stop conditions and next steps are explicit.

## Risks And Stop Conditions

Hard stop immediately if any of these occur:

- coast/scale hard gate regression;
- no manifest or no machine-readable metrics;
- missing volcano-disabled proof;
- volcano-disabled terrain is invalid, empty, or incoherent;
- radial dominance still visually or numerically fails;
- catchments are radial spokes;
- ridge/valley graph is missing or star-shaped;
- hydrology cannot derive from the accepted heightfield;
- feature grammar is mostly noise camouflage;
- UE5 runtime shell uses a different heightfield;
- final report claims success with only screenshots and no metrics;
- project state says Batch 005 / 006 complete before all hard evidence passes.

Risk classifications:

| Risk | Classification | Required action |
| --- | --- | --- |
| Coast gate regression | Blocker | Restore coast gates before further acceptance. |
| Radial volcano still visible | Blocker | Rework base terrain foundation. |
| Missing source hash | Blocker | Add reproducibility evidence. |
| Hydrology unresolved sinks | Blocker unless true sinks documented | Fix DEM conditioning or document true sinks. |
| UE runtime shell missing | Blocker for final acceptance | Generate runtime shell evidence. |
| Preview image missing but metric present | Warning or blocker depending on image | Required evidence images block final acceptance. |
| Minor calibration threshold note | Warning | Document before PASS. |
| Beauty render absent | Non-blocking | Diagnostic evidence is more important than polish. |

## Rejected Or Forbidden Approaches

The following approaches must be rejected for Batch 005 / 006 acceptance:

- keeping the old radial volcano and adding noise;
- hiding radial terrain with foliage, materials, fog, weather, or lighting;
- moving the coastline to compensate for terrain interior changes;
- claiming coast/scale success as full terrain success;
- using a single volcano as the macro height function;
- hand-sculpting screenshots without reproducible generator state;
- generating rivers or valleys visually without flow-consistent terrain;
- exporting graphs that do not align with height/slope/catchments;
- using unseeded randomness;
- changing validation thresholds after seeing failure without calibration notes;
- accepting UE5 runtime screenshots that cannot be traced to the manifest height hash;
- adding UI, map, compass, minimap, GPS, objective markers, foliage, ecology, gameplay, buildings, or final materials as part of this terrain acceptance scope.

## Sources And References

[^gdal]: GDAL contributors, “GDAL documentation,” Open Source Geospatial Foundation. GDAL provides a common raster/vector data model and command-line utilities for geospatial data translation and processing. Accessed 2026-06-27. <https://gdal.org/en/stable/>

[^usgs-hydro]: U.S. Geological Survey, “Elevation-Derived Hydrography Data Acquisition Specifications: Hydrologically Conditioned and Enforced DEM.” The specification describes hydrologically conditioned DEM outcomes including flow direction, flow accumulation, watershed boundaries, downstream monotonicity, outlets, and true sinks. Accessed 2026-06-27. <https://www.usgs.gov/ngp-standards-and-specifications/elevation-derived-hydrography-data-acquisition-specifications-15>

[^priority-flood]: Richard Barnes, Clarence Lehman, and David Mulla, “Priority-Flood: An Optimal Depression-Filling and Watershed-Labeling Algorithm for Digital Elevation Models,” arXiv, 2015. The paper describes depression filling and watershed labeling for DEMs so cells can drain through a resolved surface. <https://arxiv.org/abs/1511.04463>

[^flat-drainage]: Richard Barnes, Clarence Lehman, and David Mulla, “An Efficient Assignment of Drainage Direction Over Flat Surfaces in Raster Digital Elevation Models,” arXiv, 2015. The paper addresses drainage direction assignment over flat DEM regions. <https://arxiv.org/abs/1511.04433>

[^erosion-graph]: Fong Yuan Lim, Yu Wei Tan, and Anand Bhojan, “Visually Improved Erosion Algorithm for the Procedural Generation of Tile-based Terrain,” arXiv, 2022. The paper discusses procedural terrain generation using a graph representation of fluvial erosion with height constraints and rainfall/erosion parameters. <https://arxiv.org/abs/2210.14496>

[^terrain-style]: Fabio Merizzi, “Procedural terrain generation with style transfer,” arXiv, 2024. The paper discusses procedural noise combined with real-world height-map style reference and numerical evaluation of morphology. <https://arxiv.org/abs/2403.08782>

[^epic-landscape]: Epic Games, “Landscape Technical Guide in Unreal Engine,” Unreal Engine 5.8 Documentation. The guide documents Landscape Components, valid heightmap dimensions, heightmap Z scale, and supported heightmap import formats. Accessed 2026-06-27. <https://dev.epicgames.com/documentation/unreal-engine/landscape-technical-guide-in-unreal-engine>

[^epic-automation]: Epic Games, “Automation Test Framework in Unreal Engine,” Unreal Engine 5.8 Documentation. The guide describes Unreal’s automation and functional testing framework for repeatable tests. Accessed 2026-06-27. <https://dev.epicgames.com/documentation/unreal-engine/automation-test-framework-in-unreal-engine>

[^epic-mrq]: Epic Games, “Rendering High Quality Frames with Movie Render Queue in Unreal Engine,” Unreal Engine 5.8 Documentation. The guide describes Movie Render Queue output for high-quality image sequences and controlled offline render settings. Accessed 2026-06-27. <https://dev.epicgames.com/documentation/unreal-engine/rendering-high-quality-frames-with-movie-render-queue-in-unreal-engine>
