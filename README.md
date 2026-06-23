# Jungle Game

A serious 3D game project scaffold for a UE5/Linux-first jungle survival-simulation.

## Current design direction

The current lead concept is a first-person survival-simulation set around **1000 CE** in a dense tropical jungle. The jungle follows normal physical and ecological rules, while the player gradually uncovers a quiet relict hominin presence that observes, tests boundaries, and changes the player's environment from distance.

Internal tagline:

> You are not being hunted. You are being evaluated.

This is still in research/design mode. The final game idea is not locked.

## Current repo state

- Local path: `/home/seth/projects/new-project`
- Remote: `https://github.com/seth1295/Jungle-Game.git`
- Primary dev/build target: Linux
- Intended engine: Unreal Engine 5
- Candidate engine path: UE5.8 first, UE5.7 fallback after Linux/Vulkan validation
- Current folder contents: workflow scaffold plus project brief

## Hard creative constraints

- Spatial 3D only; no 2D.
- No gore horror.
- Light environmental/social horror is allowed.
- Do not force all genres together.
- Current genre lane: survival-simulation with nonviolent social/anthropological horror.
- DevSpace is only the implementation mechanism, not the subject of the game.

## Implementation stance

Early implementation should be code-driven because UE `.uasset` and `.umap` files are binary and difficult to review through text-only tooling.

Preferred early architecture:

- C++ core gameplay/simulation systems
- minimal Blueprint layer
- text-based config/data where possible
- procedural/generated first playable slice
- minimal unavoidable binary assets
- Linux Vulkan baseline
- scalable Lumen/Nanite settings
- no VR target
- no multiplayer target
- no paid/plugin asset dependency early

## First playable research target

Working slice name:

> Jungle Cell 0: Embers

The first playable should prove:

- dense jungle traversal
- fire/ember survival pressure
- camp persistence
- bridge or crossing state changes
- weather/rain state
- evidence of hands
- distant nonviolent watcher presence

## DevSpace usage

Open this folder directly from ChatGPT/DevSpace:

```text
/home/seth/projects/new-project
```

The active DevSpace server must be started with the parent projects directory allowed:

```bash
DEVSPACE_ALLOWED_ROOTS="$HOME/projects"
```

To advertise this project's own skills, start DevSpace with:

```bash
DEVSPACE_SKILL_PATHS="$HOME/projects/new-project/.agents/skills"
```

Full startup instructions are in `START_DEVSPACE.md`.

## Research and gate docs

Start here for the permanent research set:

```text
docs/RESEARCH_INDEX.md
```

Current core docs:

```text
docs/UE5_LINUX_VALIDATION.md
docs/UE_VERSION_DECISION.md
docs/UE_PROJECT_BOOTSTRAP_RESEARCH.md
docs/FIRST_PLAYABLE_SLICE_RESEARCH.md
docs/JUNGLE_CELL_0_IMPLEMENTATION_SCOPE.md
docs/PR_WORKFLOW.md
docs/GREEN_TO_GO.md
docs/gates/
```

## Included workflow files

```text
AGENTS.md
START_DEVSPACE.md
DEVSPACE_CONTEXT.md
.agents/skills/devspace-local-workspace/SKILL.md
.agents/skills/local-git-bootstrap/SKILL.md
.agents/skills/standard-pr-workflow/SKILL.md
.agents/skills/manual-coderabbit-loop/SKILL.md
.agents/skills/project-memory-maintenance/SKILL.md
.mex/ROUTER.md
.mex/context/active-memory.md
.mex/context/project.md
.mex/patterns/INDEX.md
.mex/patterns/local-devspace-workflow.md
relict_jungle_project_brief.docx
```

## Workflow rule

Do not commit, push, open PRs, call repo-host tools, or merge unless the user explicitly asks for that lifecycle.
