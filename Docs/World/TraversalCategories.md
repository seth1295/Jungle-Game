# Traversal Categories

Created: 2026-06-25

## Purpose

Turn terrain and biome masks into playable movement meaning. The player reads the world; the UI does not solve route choice.

## Categories

### Hard Blocker

A natural feature that stops traversal. It should be visible before the player commits too far.

### Soft Blocker

A natural feature that slows, risks, drains, or discourages traversal. It creates route pressure without becoming a wall.

### Affordance

A natural feature that invites movement or helps orientation. It is useful but imperfect.

### False Affordance

A natural feature that looks useful but worsens, redirects, or naturally terminates.

### Neutral Friction

Ordinary jungle texture that gives movement feel without turning into collision noise.

## Inputs

Classification can use slope, ridge influence, valley influence, wetness, creek distance, coast influence, canopy closure, disturbance gap, biome class, and local obstacle density.

## Rule

Do not draw route lines first. Build terrain and biome conditions that make routes emerge.
