# Fort Kick Ass

A third-person, co-op **shared world** built in Unreal Engine 5 — a place a few friends log into
to mess around in and slowly make their own. Built to be played with friends *and* to learn Unreal
deeply enough to support a games career (networking front-loaded on purpose).

> **Status:** Pre-production. Repo seeded with source-control foundation; Unreal project not yet
> generated (that step happens on a PC with the Editor — see Setup below).

---

## What this repo is

This is the **game repo** — the Unreal project's code and assets. The **planning, design, and
learning docs live separately** in the Conductive vault at
`07_projects/unreal-multiplayer-game/` (README, LEARNING-ROADMAP, ARCHITECTURE, SETUP, design/).
Read those for the full vision and the milestone-by-milestone roadmap.

## The v0.1 goal (Milestone M0)

> **"We can stand in a world together."**
> Two players join the same session (one hosts a listen server, one joins), both run/jump around a
> small level and see each other move smoothly, and one basic interaction replicates (e.g. one player
> opens a door, the other sees it open).

Everything else builds on that proven foundation. See the vault's `README.md` for scope discipline.

## Locked v1 decisions

| Decision | Choice |
|---|---|
| Perspective | Third person |
| Scripting | Hybrid: Blueprints → C++ (core/networked systems in C++) |
| Networking | Listen server first → dedicated server later |
| World | Co-op shared sandbox, survival-flavored first slice |
| Players | 2–4 friends |
| Source control | Git + Git LFS (this repo) |

---

## Setup

**This repo is seeded with `.gitignore` and `.gitattributes` (Git LFS) already in place** — so the
critical "LFS must exist before the first asset commit" rule is satisfied. To create the actual
Unreal project on top of this foundation:

1. **Install the toolchain** — Unreal Engine 5.x (Epic Launcher), Visual Studio 2022 with the
   **"Game development with C++"** workload, Git + Git LFS, and (optional) JetBrains Rider.
2. **Clone this repo**, then run once: `git lfs install`.
3. In Unreal, create a project from the **Third Person** template → **C++**, **Desktop**,
   **With Starter Content**. Generate it into a temp location, then copy the generated project
   files (`*.uproject`, `Config/`, `Source/`, `Content/`) **into this cloned repo folder** so the
   already-committed `.gitattributes` governs the first asset commit.
4. Commit and push:
   ```bash
   git add .
   git commit -m "init: Third Person C++ template"
   git push
   ```
5. **Verify multiplayer:** in the Editor, Play → Number of Players = 2, Net Mode = Play As Listen
   Server. Two windows open — move in one, watch the character move in the other. That's the
   foundation of M0.

Full step-by-step (with the toolchain install notes and the in-Editor multiplayer test) is in the
vault's `SETUP.md`.

---

## Working with friends (short version)

- **Code (C++) merges fine. Assets (`.uasset`/`.umap`) do not** — coordinate who owns an asset;
  use `git lfs lock` for high-traffic files.
- **Pull before you start, push when you stop.** Stale clones cause the worst conflicts.
- Keep every server-side rule behind `HasAuthority()` from day one so the listen → dedicated
  migration stays cheap.

Full workflow: vault `ARCHITECTURE.md` §3.
