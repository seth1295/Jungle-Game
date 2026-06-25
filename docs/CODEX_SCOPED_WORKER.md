# Codex Scoped Worker Workflow

Codex is allowed as a scoped local worker for this repo when the user explicitly authorizes it.

Codex is not a merge gate. It is a tool for long uninterrupted local work when DevSpace or ChatGPT tool calls are too fragmented.

## Repo

`/home/seth/projects/new-project`

## Allowed use

Codex may be used for scoped implementation, build/package/test loops, UE packaged visual capture, local screenshot workflows, second-pass review, and repair tasks.

## Boundaries

Codex must not touch secrets, credentials, auth files, browser profiles, SSH keys, API keys, global machine config, package caches, unrelated repos, generated screenshots, packaged builds, DerivedDataCache, Intermediate, or unrelated Saved output.

Codex must not merge unless the current prompt explicitly delegates merge and all normal repo gates pass.

## High-permission local mode

The user may authorize Codex high-permission local mode for a precise scoped task. Before using it, check the installed Codex CLI help and official Codex CLI docs for the exact flags supported by that install.

The project skill `.agents/skills/codex-scoped-worker/SKILL.md` records the approved command shapes and prompt clauses.

## Prompt requirements

Every Codex scoped-worker prompt must include the repo path, exact scope, boundaries, required checks, final diff inspection, changed-file report, commands run, results, and blockers.

## UE visual capture tie-in

For packaged visibility validation, Codex should run:

`CAPTURE_VIEW=1 bash scripts/ue5-linux-run-packaged-playable.sh`

That appends the game-side `-JungleVisualSmoke` flag. Runtime code sets a deterministic Cell 0 camera, requests a high-resolution screenshot, and exits.

Screenshots are generated under:

`PackagedBuilds/PR20Smoke/Linux/JungleGame/Saved/Screenshots/Linux/`

Screenshots are validation evidence only and must not be committed.
