# Handoffs

Fresh DevSpace chats start without the prior chat context. Use this folder for tracked Markdown handoffs that let the next chat continue safely.

## When to create a handoff

Create a handoff when either condition is true:

1. The fourth PR in a sequential four-PR planning batch is complete.
2. Work stops early because of a blocker, external dependency, tool/context limit, or user pause and the next chat will need repo state.

## Execution model

- Plan up to four PRs ahead.
- Execute one PR at a time.
- Do not run normal UE implementation PRs in parallel.
- After the fourth PR in a batch, write a handoff before starting the next batch.

## Naming

Prefer:

```text
handoffs/YYYY-MM-DD-prNN-prMM-short-topic.md
```

For incomplete batches, include `interim` in the file name:

```text
handoffs/YYYY-MM-DD-interim-prNN-short-topic.md
```

## Required contents

Each handoff should be readable by a fresh DevSpace chat with no local conversation context. Include:

- repo path, branch, and PR state;
- exact user authority relevant to the current workflow;
- four-PR batch just completed or current incomplete batch status;
- files changed and files intentionally left alone;
- validations/checks run and their result;
- known warnings, deferred risks, and blockers;
- generated/cache/config files to avoid;
- current game/design direction needed for the next batch;
- next four proposed PRs, if known;
- exact next action for the next DevSpace chat.

## Do not include

- secrets, tokens, credentials, raw auth files, or private system/developer prompts;
- generated cache dumps or large logs;
- unverified claims about PR/merge status.
