# PR5 Lifecycle Workflow

## Purpose

`pr5lifecycle` is a repeatable research-to-implementation workflow for Jungle Game.

It lets the project use five independent no-context researcher AI chats to create high-quality Markdown research/spec documents, then uses those documents as source inputs for five sequential implementation PRs.

The workflow exists because some systems need stronger research and design grounding before code is written, especially world-first terrain, biome, traversal, navigation, weather, ecology, validation, and UE5 implementation systems.

## Summary

```text
pr5lifecycle-setup
→ create five researcher prompt files

external researcher AI step
→ user sends each prompt to an independent no-context researcher chat
→ each researcher returns one Markdown research/spec document

user places runtime docs
→ pr5lifecycle-runtimefiles/<batch>/

pr5lifecycle-run
→ consume one runtime file per PR
→ implement PR
→ validate
→ update .mex
→ commit/push/open PR
→ CodeRabbit
→ fix valid in-scope findings
→ merge
→ sync main
→ continue to next runtime file
```

## Folder Layout

The lifecycle uses two root folders:

```text
pr5lifecycle-precursor/
pr5lifecycle-runtimefiles/
```

### `pr5lifecycle-precursor/`

This folder contains prompt files created during `pr5lifecycle-setup`.

These files are not research docs. They are prompts for independent researcher AI chats.

Use numbered batch folders:

```text
pr5lifecycle-precursor/
  001/
    001_<topic>_prompt.md
    002_<topic>_prompt.md
    003_<topic>_prompt.md
    004_<topic>_prompt.md
    005_<topic>_prompt.md

  002/
    001_<topic>_prompt.md
    002_<topic>_prompt.md
    003_<topic>_prompt.md
    004_<topic>_prompt.md
    005_<topic>_prompt.md
```

### `pr5lifecycle-runtimefiles/`

This folder contains the actual researcher-produced Markdown files.

These files are the runtime inputs consumed during `pr5lifecycle-run`.

Use matching numbered batch folders:

```text
pr5lifecycle-runtimefiles/
  001/
    001_<topic>_research.md
    002_<topic>_research.md
    003_<topic>_research.md
    004_<topic>_research.md
    005_<topic>_research.md

  002/
    001_<topic>_research.md
    002_<topic>_research.md
    003_<topic>_research.md
    004_<topic>_research.md
    005_<topic>_research.md
```

## Naming Rules

Batch folders must be zero-padded three-digit numbers.

```text
001
002
003
004
005
006
007
008
009
010
011
```

Each batch contains exactly five prompt files and, later, exactly five runtime research files.

Prompt files:

```text
001_<topic>_prompt.md
002_<topic>_prompt.md
003_<topic>_prompt.md
004_<topic>_prompt.md
005_<topic>_prompt.md
```

Runtime research files:

```text
001_<topic>_research.md
002_<topic>_research.md
003_<topic>_research.md
004_<topic>_research.md
005_<topic>_research.md
```

The numeric prefix controls PR order during `pr5lifecycle-run`.

## Authority Order

Runtime research files are implementation inputs, not absolute authority.

Authority order is:

```text
1. Current user instruction
2. AGENTS.md
3. .mex routed memory and active-memory
4. Active gate / PR workflow docs
5. Current codebase
6. Active pr5lifecycle runtime research file
```

If a runtime research file conflicts with project rules, the conflicting portion must be rejected, trimmed, or deferred.

Examples of rejected drift:

```text
player-facing map
minimap
compass
bearing readout
GPS
objective marker
HUD clock
explicit time display
orientation confidence UI
modern navigation tools
combat/chase/gore framing
asset-pack dumping
unreviewable binary churn
```

## Phase 1 — `pr5lifecycle-setup`

`pr5lifecycle-setup` creates the five independent researcher prompt files.

### Setup Inputs

Before creating setup prompts, read:

```text
AGENTS.md
.mex/ROUTER.md
.mex/context/active-memory.md
.mex/context/project.md
.mex/context/pr-gates.md
docs/PR_WORKFLOW.md
relevant current source/docs for the target lane
```

### Setup Output

Create:

```text
pr5lifecycle-precursor/<batch>/
```

Then write five standalone Markdown prompt files.

Each prompt must be usable in a completely fresh no-context researcher AI chat.

Each prompt must include:

```text
project context
current game direction
exact research target
expected output file name
required Markdown structure
factuality requirements
implementation usefulness requirements
forbidden scope
citation/source expectations when applicable
how the result will be used in the repo
```

### Setup Prompt Quality Bar

Each setup prompt must be:

```text
standalone
project-specific
narrow enough to produce useful implementation guidance
research factual
not vibes-only
clear about forbidden drift
clear about expected Markdown output
clear that the researcher must not write production code unless asked for pseudocode/spec examples
clear that output must be a Markdown research/spec document
```

### Setup Completion Report

After `pr5lifecycle-setup`, report:

```text
batch number
folder created
five prompt files created
intended runtime folder
what each prompt is for
next user action
```

The next user action is:

```text
Send each prompt file to an independent researcher AI chat.
Place returned Markdown research/spec docs into pr5lifecycle-runtimefiles/<batch>/.
```

## Phase 2 — External Researcher AI Step

The user sends each prompt from:

```text
pr5lifecycle-precursor/<batch>/
```

to an independent researcher AI chat.

Each researcher AI returns one Markdown document.

The user places those returned documents into:

```text
pr5lifecycle-runtimefiles/<batch>/
```

The researcher AI does not edit the repo directly.

The researcher AI does not create PRs.

The researcher AI does not decide project authority.

The researcher AI produces research/spec input only.

## Phase 3 — `pr5lifecycle-run`

`pr5lifecycle-run` consumes the five runtime files and turns them into five sequential implementation PRs.

Each runtime file maps to exactly one implementation PR unless the user explicitly changes the plan.

Example:

```text
pr5lifecycle-runtimefiles/001/001_macro_island_terrain_navigation_research.md
→ PR 1

pr5lifecycle-runtimefiles/001/002_tropical_biome_masks_traversal_research.md
→ PR 2

pr5lifecycle-runtimefiles/001/003_creek_riparian_wet_valley_research.md
→ PR 3

pr5lifecycle-runtimefiles/001/004_ridge_slope_canopy_orientation_research.md
→ PR 4

pr5lifecycle-runtimefiles/001/005_world_validation_evidence_research.md
→ PR 5
```

## Runtime File Intake

Before implementing each PR, read the active runtime file and classify its contents.

Classification categories:

```text
accepted
accepted with trimming
deferred
rejected
conflicts with project rules
requires user decision
```

The active runtime file must be reconciled against:

```text
AGENTS.md
.mex/ROUTER.md
.mex/context/active-memory.md
.mex/context/project.md
.mex/context/pr-gates.md
docs/PR_WORKFLOW.md
current source/docs
current dirty workspace state
```

If the runtime file contains conflicts, record the conflict and continue only with accepted in-scope content.

## Per-PR Runtime Cycle

For each runtime file `001` through `005`, run this cycle.

### 1. Read active runtime file

```text
pr5lifecycle-runtimefiles/<batch>/<number>_<topic>_research.md
```

### 2. Define PR scope

Create a narrow PR scope from the runtime file.

The scope must include:

```text
PR title
runtime input file
accepted findings
rejected/deferred findings
allowed files/areas
forbidden files/areas
validation expectations
stop conditions
```

### 3. Start from clean updated main

Before lifecycle work:

```text
sync main
confirm clean status
create scoped branch
```

Do not start implementation from a dirty unrelated branch unless the user explicitly approves a recovery path.

### 4. Implement through DevSpace

Use:

```text
devspace.read
devspace.edit
devspace.write
```

Use bash only for:

```text
git status/log/diff
read-only discovery
build/test/package validation
```

Do not use bash redirection, heredocs, generated scripts, `tee`, or `sed -i` to modify project files through DevSpace.

### 5. Validate

Required validation depends on PR scope.

Minimum validation:

```text
git diff --stat
inspect intended diff
UE build or compile validation when source changes
docs consistency check when docs change
no generated/cache/build junk staged
```

If visual or packaged state matters:

```text
package smoke
UE-native screenshot capture
visual evidence inspection
runtime/headless smoke
```

### 6. Fix valid in-scope failures

If validation fails:

```text
fix only in-scope failures
rerun validation
record unresolved blockers
do not expand into adjacent PR scope
```

### 7. Update `.mex`

Every PR must update:

```text
.mex/context/active-memory.md
```

The update must record:

```text
completed PR/runtime file
batch number
branch name
commit/PR number when known
validation performed and result
accepted/deferred/rejected research findings
next runtime file
blockers/warnings
whether batch handoff is due
```

This is a hard lifecycle gate.

### 8. Commit

After validation passes and `.mex` is updated:

```text
inspect diff/stat
stage intended files only
commit coherent PR slice
```

Do not stage:

```text
Binaries/
Intermediate/
Saved/
DerivedDataCache/
packaged builds
logs
cache files
temporary screenshots/videos
secrets
local auth files
unrelated generated output
```

### 9. Push and open ready PR

Push branch and open a ready PR against `main`.

Do not open a draft PR unless requested.

If using manual CodeRabbit loop, include this in the PR body:

```text
@coderabbitai ignore
```

### 10. CodeRabbit loop

Run CodeRabbit.

Inspect findings against current files.

Fix only findings that are:

```text
valid
in-scope
not stale
not speculative
not duplicate
not contrary to project direction
```

After fixes:

```text
rerun validation
commit fixes
rerun or re-check CodeRabbit as needed
```

### 11. Merge

Merge only when all gates pass.

Merge requirements:

```text
scope matches runtime file and user request
diff inspected
no unrelated changes
no generated/cache junk
validation passes
.mex updated
CodeRabbit clean or remaining findings classified non-blocking
repo guard/gates pass if applicable
user has authorized merge/lifecycle
```

### 12. Sync main

After merge:

```text
checkout/sync main
confirm clean status
confirm .mex reflects landed state
continue to next runtime file
```

## Batch Completion

After all five runtime files are consumed and all five PRs are merged:

Update `.mex/context/active-memory.md` with batch completion.

Record:

```text
batch number
all five runtime files
all five PRs
validation summary
merged branches/commits
deferred findings
rejected findings
active blockers
next recommended pr5lifecycle-setup batch
```

Create or update a handoff if required by the project’s handoff cadence.

A batch is complete only when:

```text
all five runtime files were consumed
all five implementation PRs were merged or explicitly skipped/deferred
.mex records final state
local main is synced clean
next action is clear
```

## Stop Conditions

Stop and report if:

```text
DevSpace cannot open the workspace
runtime files are missing
runtime files are clearly not the expected batch
repo state is dirty in unrelated ways and cannot be safely separated
active gate conflicts with runtime plan
required validation fails and the fix is outside scope
research file asks for forbidden project direction
binary/generated junk cannot be avoided
merge/review requires user authorization that has not been given
```

## Current Project-Specific Hard Guards

For Jungle Game, every `pr5lifecycle` batch must preserve:

```text
first-person 3D UE5/Linux-first survival simulation
dense tropical jungle
world-first island terrain direction
physical/ecological normality
nonviolent relict hominin presence
observed/tested/evaluated tone
no gore-horror framing
no modern-current-day tool framing
no generic pine woodland drift
```

Forbidden navigation/time/orientation UI:

```text
no player map
no minimap
no compass
no bearing number/readout
no GPS marker
no objective marker navigation
no HUD clock
no explicit time readout
no orientation confidence system
no hidden orientation meter
no character comments that tell the player whether they are oriented
```

## Current Recommended Use

For the current Jungle Game state, the first `pr5lifecycle` batch targets the world-first implementation lane:

```text
001_macro_island_terrain_navigation_prompt.md
002_tropical_biome_masks_traversal_prompt.md
003_creek_riparian_wet_valley_prompt.md
004_ridge_slope_canopy_orientation_prompt.md
005_world_validation_evidence_prompt.md
```

Batch 001 prompt 001 includes InfiniteDiffusion / Terrain Diffusion as a required terrain-generation research reference. It should be treated as emerging architecture inspiration for seed-consistent, lazy/unbounded, learned terrain generation, not as an immediate project dependency.

The corresponding runtime files should become:

```text
001_macro_island_terrain_navigation_research.md
002_tropical_biome_masks_traversal_research.md
003_creek_riparian_wet_valley_research.md
004_ridge_slope_canopy_orientation_research.md
005_world_validation_evidence_research.md
```

Each runtime file becomes one sequential PR during `pr5lifecycle-run`.

## Short Command Phrase Definitions

### `pr5lifecycle-setup`

Means:

```text
Create the next numbered pr5lifecycle-precursor/<batch>/ folder and write five standalone researcher prompt Markdown files.
```

### `pr5lifecycle-run`

Means:

```text
Read pr5lifecycle-runtimefiles/<batch>/ and consume files 001 through 005 as five sequential implementation PRs with validation, .mex updates, CodeRabbit, merge, and sync between each PR.
```

### `pr5lifecycle`

Means the whole system:

```text
setup prompts
external research docs
runtime PR implementation queue
five sequential PRs
batch completion memory/handoff
```

## Final Reporting Format

Every `pr5lifecycle` operation reports:

```text
workspace path
mode
batch number
phase: setup or run
files created/read/changed
current runtime file if applicable
checks run
CodeRabbit status if applicable
merge status if applicable
.mex update status
blockers
warnings
next action
```
