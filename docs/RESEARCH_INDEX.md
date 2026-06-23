# Research Index

This index points to the permanent research and scope documents that support the Jungle Game gate workflow.

These documents are not implementation patches. They define evidence, constraints, risks, and PR scope boundaries for future gates.

## Gate research docs

| Gate | Document | Purpose | Status |
|---|---|---|---|
| Gate 001 | `docs/UE5_LINUX_VALIDATION.md` | Linux, NVIDIA, Vulkan, and UE editor validation scope. | Active / pending local evidence |
| Gate 002 | `docs/UE_VERSION_DECISION.md` | UE 5.8 vs UE 5.7 decision logic and fallback policy. | Blocked pending Gate 001 evidence |
| Gate 003 | `docs/UE_PROJECT_BOOTSTRAP_RESEARCH.md` | Minimal UE C++ project bootstrap scope and generated-file policy. | Blocked pending Gates 001–002 |
| Gate 004 | `docs/FIRST_PLAYABLE_SLICE_RESEARCH.md` | Jungle Cell 0 first playable planning and boundaries. | Reference-ready |
| Gate 005 | `docs/JUNGLE_CELL_0_IMPLEMENTATION_SCOPE.md` | Future first playable implementation scope and anti-scope-creep rules. | Reference-ready |

## Related workflow docs

- `docs/PR_WORKFLOW.md` — PR-by-PR gate workflow.
- `docs/GREEN_TO_GO.md` — green-to-go criteria.
- `docs/gates/` — individual gate files and current gate chain.

## Current gate stance

The active gate remains Gate 001:

```text
docs/gates/001-ue5-linux-validation.md
```

The project should not create UE project files until Gate 001 and Gate 002 have enough evidence to pass.
