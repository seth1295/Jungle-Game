# SELF-ITERATIVE Runtime Files

`SELF-ITERATIVE/` is repo-owned lifecycle state for the `self-iterative-lifecycle` workflow.

This folder is for numbered runtime files written by the agent after tool-driven diagnosis. It works like PR5 runtime files, but the runtime files are produced inside the repo by the active agent from direct evidence instead of being supplied only by external researcher chats.

## File Shape

Use sequential filenames:

```text
001_<scope>.md
002_<scope>.md
003_<scope>.md
```

Each file must define one scoped implementation unit:

- observed issue
- tool evidence used
- suspected cause
- planned source/tool/repo changes
- validation evidence required
- expected files touched
- PR/check/CodeRabbit expectations
- next-runtime trigger if the result is still wrong

## Execution Rule

Run one numbered runtime file at a time.

After each implementation:

1. inspect the changed code/tooling;
2. run relevant checks/generators;
3. inspect generated evidence with available tools;
4. update `.mex/context/active-memory.md`;
5. PR, CodeRabbit, fix valid findings, merge, and sync when authorized;
6. write the next runtime file only from the new evidence.

Do not overwrite existing terrain preview evidence folders. Create the next numbered preview folder for new evidence.
