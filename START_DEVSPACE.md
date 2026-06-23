# Start DevSpace for Jungle Game

Use this when you want ChatGPT/DevSpace to advertise this project's local skills.

## Terminal 1 — ngrok

```bash
ngrok http --url=basket-enunciate-impotence.ngrok-free.dev 7676
```

## Terminal 2 — DevSpace

```bash
cd "$HOME/projects/new-project" && DEVSPACE_ALLOWED_ROOTS="$HOME/projects" DEVSPACE_SKILLS=1 DEVSPACE_AGENT_DIR="$HOME/.codex" DEVSPACE_SKILL_PATHS="$HOME/projects/new-project/.agents/skills" DEVSPACE_PUBLIC_BASE_URL="https://basket-enunciate-impotence.ngrok-free.dev" devspace serve
```

## Expected output

```text
devspace listening on http://127.0.0.1:7676/mcp
public base url: https://basket-enunciate-impotence.ngrok-free.dev
allowed roots: /home/seth/projects
```

The important pieces are:

```text
DEVSPACE_ALLOWED_ROOTS=/home/seth/projects
DEVSPACE_SKILL_PATHS=/home/seth/projects/new-project/.agents/skills
```

`DEVSPACE_ALLOWED_ROOTS` controls what folders ChatGPT can open. `DEVSPACE_SKILL_PATHS` controls which local skills DevSpace advertises.

## Current boundary

This folder is a git repository connected to the Jungle Game remote. Starting DevSpace does not create commits, PRs, pushes, or merges.
