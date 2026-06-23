# Gate 002 — Engine Version Decision

## Status

planned

## Purpose

Choose the UE5 version for the first implementation phase based on Linux validation results.

## Allowed scope

This gate may:

- compare UE5.8 and UE5.7 against validation results;
- document the chosen engine version;
- document fallback criteria;
- document rendering feature stance for the RTX 2080 baseline;
- add or update engine decision docs under `docs/`;
- update gate status and next gate pointer when complete.

## Forbidden scope

This gate must not:

- create a UE project;
- add game code;
- add assets;
- change first playable scope beyond engine-relevant constraints;
- assume Linux-to-Windows packaging is proven;
- depend on experimental UE5.8 features unless explicitly accepted.

## Required inputs

- Gate 001 validation results.
- UE5.8 Linux status.
- UE5.7 fallback status if tested.
- RTX 2080 Vulkan feature baseline.
- Known constraints around Lumen, Nanite, Virtual Shadow Maps, and hardware ray tracing.

## Required validation

The decision document must answer:

- Which UE5 version is selected?
- Why was it selected?
- What would trigger fallback?
- What rendering features are allowed in the first slice?
- What features are optional or experimental?
- What features are forbidden until later profiling?

## Exit criteria

This gate passes when the engine version and fallback policy are recorded clearly enough for Gate 003 to create a minimal UE C++ project without guessing.

## CodeRabbit handling

Fix valid in-scope documentation consistency issues only.

## Next gate

`docs/gates/003-ue-project-bootstrap.md`

## Stop conditions

Stop and report if:

- Gate 001 did not produce enough data;
- UE5.8 and UE5.7 both appear unsuitable;
- driver or Vulkan support blocks required features;
- the decision would require accepting an unstable experimental feature as mandatory.
