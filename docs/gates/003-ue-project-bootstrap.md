# Gate 003 — UE Project Bootstrap

## Status

planned

## Purpose

Create the minimal UE5 C++ project only after Linux validation and engine version decision are complete.

## Allowed scope

This gate may add the minimal files required for a UE5 C++ project:

- `JungleGame.uproject`
- `Source/JungleGame/`
- `Config/`
- minimal unavoidable starter `Content/` files if the editor requires them
- build/cook/package notes under `docs/`

## Forbidden scope

This gate must not:

- add large binary assets;
- add paid assets;
- add plugin dependencies;
- create a polished level;
- build gameplay systems beyond empty/stub module structure;
- implement Jungle Cell 0 gameplay;
- add multiplayer or VR targets;
- assume Windows packaging is validated.

## Required inputs

- Gate 001 passed.
- Gate 002 passed.
- Selected UE5 version recorded.
- Binary asset policy understood.
- Local UE editor path known.

## Required validation

Run or document:

- UE project generation/open result;
- C++ compile result;
- Linux editor launch result;
- Linux package/cook command or blocker;
- `git status` and diff/stat inspection;
- confirmation that binary assets are minimal and intentional.

## Exit criteria

This gate passes when a minimal UE5 C++ project exists on `main`, builds locally, and does not include asset bloat or gameplay scope creep.

## CodeRabbit handling

Fix valid in-scope project/config/build issues only.

## Next gate

`docs/gates/004-first-playable-slice-plan.md`

## Stop conditions

Stop and report if:

- UE project generation requires manual editor action;
- binary assets cannot be minimized or inspected enough;
- C++ compile fails for reasons outside this gate;
- package/cook validation requires a platform or engine change;
- the generated diff contains unrelated or cache files.
