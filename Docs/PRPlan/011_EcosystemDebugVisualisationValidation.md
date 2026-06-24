# Ecosystem Debug Visualisation and Validation

Roadmap label: PR #11.

## Purpose

This pass makes ecosystem masks and traversal classifications inspectable by developers. It does not add player-facing map, compass, minimap, bearing, GPS, objective marker navigation, HUD clock, explicit time readout, or orientation-confidence feedback.

## Developer-Only Views

The shared debug contract lives in `Source/JungleGame/JungleEcosystemDebugSpec.h` and defines these developer validation views:

- `raw_mask_ranges`
- `traversal_classification`
- `component_border_continuity`
- `first_person_probe`

These views exist to inspect why a terrain or ecosystem classification exists at representative points. They are not gameplay UI and must not become navigation aids.

## Probe Output

A developer probe can record:

- mask name;
- raw normalized mask value;
- traversal class from `FJungleEcosystemMaskSpec`;
- a short reason string for review notes or editor diagnostics.

Raw values remain normalized to 0.0 through 1.0 so validation can catch bad mask ranges before PCG hides bad ecological logic.

## Validation Expectations

1. Run `git diff --check` on touched files.
2. Inspect `git diff --stat` and the full diff.
3. Confirm debug views are developer-only and not player-facing.
4. Confirm raw mask range, traversal classification, component-border continuity, and first-person probe views are represented.
5. Confirm no player map, minimap, compass, bearing readout, GPS marker, objective marker navigation, HUD clock, explicit time readout, orientation-confidence system, hidden orientation meter, or character orientation comment is introduced.
6. Confirm no generated editor or cache output is staged.

## Acceptance

This PR is accepted when reviewers have a stable developer-only debug vocabulary for inspecting ecosystem masks and traversal classifications before later PCG density passes.
