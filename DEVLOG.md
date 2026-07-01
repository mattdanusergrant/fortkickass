# Fort Kickass — Dev Log

A running log of what got built and *why*, focused on the networking decisions.
The whole project front-loads multiplayer on purpose — server-authoritative
replication is the rarest, hardest-to-fake gameplay-programming skill, so it's
where the learning starts, not where it's bolted on later.

---

## Day 1 — From empty repo to a networked survival slice

Started the day with an empty repo and a fresh Unreal 5 Third Person (C++)
project. Ended it with a small but complete **co-op shared-world loop** —
gather, build, and a shared day/night clock — all server-authoritative and
replicating cleanly across two clients.

### Source control before assets
Set up `.gitattributes` (Git LFS) and `.gitignore` **before** the first asset
commit — Unreal projects are gigabytes of binary `.uasset`/`.umap` files that
Git can't diff or merge, so LFS has to be in place first or the binaries commit
as broken raw blobs. Verified with `git lfs ls-files` that the meshes and maps
are tracked as LFS pointers, not history bloat.

### M0 — "two players in a world, one interaction replicating"
The first milestone was deliberately tiny: prove the networking foundation.

- **`AInteractableDoor`** — the smallest complete demonstration of Unreal's
  replication toolkit. A `bReplicates` actor with a `ReplicatedUsing` bool
  (`DOREPLIFETIME` + a `RepNotify`). A pawn overlapping the trigger toggles the
  door — but only on the server (`HasAuthority()` gate). The state replicates
  down and every machine swings the mesh in its `OnRep`. Ran it with 2 players
  on a listen server: the door opens in **both** windows. The golden rule made
  concrete — *the client requests, the server decides, the result replicates back.*

### The survival slice
With the foundation proven, built the world-concept loop:

- **Gather — `APickupItem`.** Walk into it; the **server** consumes it and it
  disappears for everyone (replicated `Destroy`). Server authority over world state.
- **Count — `AFortPlayerState`.** Collecting credits a **replicated
  `ResourceCount`** on a custom `PlayerState` (the Gameplay Framework's home for
  per-player replicated data). A `RepNotify` prints the count on screen — no HUD
  needed yet, but you can watch gathering actually count.
- **Build — Server RPC.** Press `B`: the controlling client calls a
  `UFUNCTION(Server, Reliable)` on its Character; the **server** spawns a
  replicated `ABuildable` in front of the player, and it appears in everyone's
  world. This adds the Server RPC — the last core piece of the toolkit.
- **Pressure — `AFortGameState`.** A replicated day/night clock. The **server**
  advances `TimeOfDay` each tick and replicates it, so the whole session shares
  one clock (`GameState` = replicated shared state, the counterpart to
  `PlayerState`). Gives gathering and building a rhythm.

### Replication toolkit covered (day one)
| Concept | Used in |
|---|---|
| Replicated variable + `RepNotify` | Door, PlayerState |
| `HasAuthority()` server-authority gate | Door, Pickup, PlayerState |
| Replicated actor spawn / destroy | Buildable (spawn), Pickup (destroy) |
| Server RPC | Build system |
| PlayerState (per-player replicated data) | Resource count |
| GameState (shared replicated data) | Day/night clock |

### Design notes / deliberate scoping
- **Overlap-to-interact** over press-to-interact for the first pass — proves
  server authority without dragging in the input system on day one. Press-to-use
  is a clean upgrade once the netcode is trusted.
- **Hybrid C++/Blueprint, C++ first for networked systems** — the studio pattern.
  Everything above is C++; cosmetic/glue stays in Blueprints.
- **Listen server first.** Zero infra, plays tonight, and the code is identical
  to a dedicated server as long as every server rule stays behind
  `HasAuthority()` — which keeps the eventual dedicated-server migration cheap.

### Next
Wire the Server RPC to a real interact/build UI, credit resources into an
inventory, and start on a dedicated-server build so the world can persist.
