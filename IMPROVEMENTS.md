# Improvement Plan — fortkickass

> Generated 2026-07-02 from a full 12-repo portfolio audit (Claude Code session).
> Companion career report: ConductiveOS vault, `09_personal/2026-07-02-life-audit-and-career-plan.md`.

**What this is:** A 2-day-old but rigorously documented Unreal Engine 5.5 C++ co-op sandbox prototype that front-loads server-authoritative multiplayer — replicated door, pickups, per-player resources, Server-RPC building, and a shared day/night clock, all verified across two clients on a listen server.

**Stack:** Unreal Engine 5.5, C++ (UE gameplay framework), Unreal replication (property replication, RepNotify, Server RPCs), Enhanced Input, Blueprints (hybrid C++/BP), Git LFS, UnrealBuildTool (C# build rules) · **Maturity:** prototype
**Size:** ~850 lines C++/C# in Source/ (~380 lines custom networking code beyond the UE Third Person template; 21 source files) plus ~540 binary Content assets in LFS

## What's genuinely good here

- The server-authority architecture is genuinely correct for its scope: every state mutation is behind HasAuthority(), state lives in the right framework class, and the one client→server pathway is a proper Server RPC — the foundational pattern is right, which is the hard part to retrofit
- Exceptional documentation-to-code ratio: README + DEVLOG explain what exists, why it exists, what was deliberately deferred, and how to verify it — better docs than most professional repos
- Correct handling of real replication subtleties (RepNotify not firing locally, late-join state via ApplyDoorState() in BeginPlay, fixed debug-message keys to avoid spam) shows understanding, not tutorial-copying
- Disciplined git hygiene: LFS before assets, conventional commits, template code left surgically untouched (raw pointers in FortKickassCharacter.h are template legacy; all new classes use TObjectPtr) — consistent with the vault's stated Karpathy 'surgical changes' principle
- Honest scoping in writing: 'Direct-IP / LAN only for now; Steam friend invites come later (Phase 5)' — deferrals are documented, not hidden


## Issues found

- ServerPlaceBuildable_Implementation (Source/FortKickass/FortKickassCharacter.cpp:110) trusts the client-supplied Location/Rotation with no server-side range validation, no resource cost, and no rate limiting — a cheat vector that contradicts the repo's own server-authority thesis
- Gather and build loops are not economically connected: AFortPlayerState::ResourceCount is incremented by pickups but never spent; building is free, so the survival slice has no actual survival pressure
- AFortGameState replicates TimeOfDay as a float mutated every server tick (FortGameState.cpp Tick) — bandwidth-naive; should derive time client-side from a replicated epoch + GetServerWorldTimeSeconds()
- AInteractableDoor toggles on every pawn overlap-begin (InteractableDoor.cpp:46) — walking through re-toggles it, it can close on an occupant, and any pawn (not just players) triggers it
- No CI, no build-verification script, no tests — nothing guarantees a commit compiles (.github/ absent)
- No screenshots, GIFs, or video anywhere — the repo's core claim (replication across two clients) has zero visual evidence for anyone who won't install UE 5.5
- Repo carries full StarterContent plus duplicate UE4/UE5 mannequin sets (~540 Content files; SKM_Manny.uasset is a 34.5MB LFS object) for a prototype that renders an engine cube — LFS bloat
- All UI is GEngine->AddOnScreenDebugMessage (FortPlayerState.cpp:40, FortGameState.cpp:55) — fine for Day 1 but undemoable; no UMG HUD
- Legacy BindKey(EKeys::B) for Build (FortKickassCharacter.cpp:97) coexists with Enhanced Input — acknowledged shortcut, unremappable
- History is a single 2-day burst (2026-06-30/07-01) with one non-conventional 'setup' commit (1a0de41); proves velocity, not sustained delivery


## Ranked improvements

### 1. Validate the build Server RPC and connect it to the resource economy  `impact 5/5 · effort S`

**Why:** ServerPlaceBuildable_Implementation (FortKickassCharacter.cpp:110) spawns whatever transform the client sends — no range check, no cost, no rate limit. A modified client could spawn unlimited blocks anywhere. This directly contradicts the repo's own 'server decides' thesis, and it's the first thing a multiplayer-studio interviewer would probe. Building is also free, so the gather loop (APickupItem → AFortPlayerState::ResourceCount) is economically decorative.

**How:** In ServerPlaceBuildable_Implementation: (1) recompute the spawn point server-side from GetActorLocation()/GetActorForwardVector() instead of trusting the client's Location/Rotation (or clamp distance to ~200uu); (2) fetch GetPlayerState<AFortPlayerState>() and require ResourceCount >= BuildCost, deducting via a new server-only SpendResource(int32) that mirrors AddResource; (3) reject and optionally notify via a Client RPC. Update DEVLOG.md with a 'Day 2: closing the trust gap' entry — the writeup is worth as much as the fix.

**Career angle:** Server-side validation of client RPCs is the canonical multiplayer-gameplay interview topic; showing the vulnerability found and fixed (with a devlog entry) turns a weakness into the repo's best talking point.

### 2. Add two-client GIFs/screenshots to the README  `impact 5/5 · effort S`

**Why:** This is a game repo whose entire claim is 'replication verified across two clients' — and there is zero visual evidence. Nobody evaluating him will install UE 5.5 to check. A 10-second GIF of the door opening in both listen-server windows proves the whole M0 claim instantly.

**How:** Run PIE with Number of Players = 2 / Net Mode = Listen Server, capture: door toggle in both windows, B-key block placement appearing on both, the day/night clock in sync, resource count incrementing. Save to docs/media/, embed 2-3 GIFs at the top of README.md under the status blockquote.

**Career angle:** Pure portfolio move — converts the repo from 'trust me' to 'see for yourself' for recruiters and hiring managers who spend 30 seconds per repo.

### 3. Replicate the day/night clock efficiently instead of per-tick float replication  `impact 4/5 · effort S`

**Why:** AFortGameState::Tick mutates the Replicated TimeOfDay every server frame, so the property is dirty every net update — the textbook bandwidth-naive pattern. AGameStateBase already replicates a synchronized server clock.

**How:** Replace the replicated per-tick TimeOfDay with a replicated epoch: store DayStartServerTime (set once on the server) + DayLengthSeconds, and have GetTimeOfDay() derive the hour on every machine from GetServerWorldTimeSeconds(). Keep IsNight() and the BlueprintPure API identical; delete the DOREPLIFETIME(TimeOfDay) entry in FortGameState.cpp:17 and the server mutation in Tick.

**Career angle:** Distinguishes 'knows the replication API' from 'thinks about bandwidth' — the exact line between junior and mid-level UE network programmers, and a great DEVLOG entry.

### 4. Sustain the DEVLOG cadence with a Day 2+ milestone (dedicated server or persistence)  `impact 4/5 · effort L`

**Why:** The single-burst history is the repo's biggest career weakness: it currently proves fast learning, not follow-through. The DEVLOG's own 'Next' section (interact UI, inventory, dedicated server) is the roadmap; one more milestone with the same documentation quality doubles the repo's evidentiary value.

**How:** Pick the dedicated-server build (the DEVLOG's stated payoff for the HasAuthority discipline): package a Linux/Win64 dedicated server target (add FortKickassServer.Target.cs), verify two clients joining it, and write the 'Day N — listen to dedicated' DEVLOG entry. Alternatively, ABuildable persistence (save/load placed blocks) pairs with the ronin-survivor map-spec pattern.

**Career angle:** Turns 'did a UE tutorial-plus sprint' into 'stood up a dedicated multiplayer server' — a line that survives on a resume and directly supports networked-gameplay and even backend/infra job narratives.

### 5. Add a build-verification script and CI stub  `impact 3/5 · effort M`

**Why:** There is no .github/workflows and no way to know a commit even compiles without opening the editor. Full UE builds in hosted CI are impractical without engine containers, but a checked-in build script plus a documented pre-push habit closes most of the gap.

**How:** Add Scripts/build.bat (or .ps1) wrapping Engine\Build\BatchFiles\Build.bat FortKickassEditor Win64 Development -project=FortKickass.uproject, document it in README 'Verify a change' (mirroring ronin-survivor's excellent npm-test section), and add .github/workflows/build.yml targeting a self-hosted runner (or Epic's EULA-gated container) marked as the dedicated-server milestone's infra step.

**Career angle:** CI-mindedness on a UE project is rare in hobby repos and signals production-team readiness; even the honest 'here's why hosted CI is hard for UE and what I did instead' writeup is a strong systems-thinking artifact.

### 6. Press-to-interact door with occupant safety  `impact 3/5 · effort M`

**Why:** AInteractableDoor::OnTriggerBeginOverlap toggles on every pawn overlap-begin: walking through can slam it, re-entering re-toggles, and any pawn (including future AI) triggers it. The header itself calls press-to-use 'a clean upgrade once the netcode is trusted' — the netcode is now trusted.

**How:** Add an Interact InputAction (Enhanced Input, replacing the pattern used by the legacy EKeys::B bind at FortKickassCharacter.cpp:97) that traces for an IInteractable interface and calls a ServerInteract RPC; AInteractableDoor implements the interface and keeps HasAuthority + RepNotify exactly as-is. This also generalizes interaction for every future object.

**Career angle:** Interface-driven interaction systems are a standard gameplay-programming portfolio pattern; moves the codebase from 'demo per concept' toward 'small architecture'.

### 7. Migrate the Build key to Enhanced Input  `impact 2/5 · effort S`

**Why:** FortKickassCharacter.cpp:97 uses legacy PlayerInputComponent->BindKey(EKeys::B) alongside Enhanced Input — acknowledged in a comment as a shortcut, but it's the one inconsistency in an otherwise idiomatic UE5 codebase and it's unremappable.

**How:** Create IA_Build + add it to the existing DefaultMappingContext, add a UInputAction* BuildAction UPROPERTY next to JumpAction/MoveAction in FortKickassCharacter.h, and bind it in SetupPlayerInputComponent like the others. Fold into the press-to-interact work if done together.

**Career angle:** Small, but 'consistent modern input system' is the kind of detail UE reviewers notice when skimming a sample.

### 8. Prune unused StarterContent / duplicate mannequins from LFS  `impact 2/5 · effort S`

**Why:** The repo ships full StarterContent plus both UE4 and UE5 mannequin sets (~540 assets; the SKM_Manny skeletal mesh alone is a 34.5MB LFS object) for a prototype that uses an engine cube and one map. This bloats LFS quota/clone time and buries the 21 files that matter.

**How:** In-editor, use Asset Audit / Reference Viewer against Content/ThirdPerson/Maps/ThirdPersonMap.umap, delete unreferenced StarterContent packs and the Mannequin_UE4 tree, then commit. Note the prune in DEVLOG (LFS hygiene is part of the repo's stated story).

**Career angle:** Minor, but repo weight is part of first impressions and LFS bandwidth costs real money on GitHub free tier.


## Skills this repo proves (for hiring managers)

- Unreal Engine 5 replication fundamentals, used correctly: DOREPLIFETIME + RepNotify (including handling the subtlety that RepNotify doesn't fire on the mutating machine — see AInteractableDoor::OnTriggerBeginOverlap and AFortPlayerState::AddResource), HasAuthority() gates on every state change, replicated actor spawn/destroy, and a UFUNCTION(Server, Reliable) RPC
- UE Gameplay Framework fluency — put each piece of state in its canonical home: per-player data on a custom APlayerState, shared session state on a custom AGameStateBase, world actors replicated, GameMode wiring classes together (FortKickassGameMode.cpp)
- Modern UE5 C++ idioms in all new code: TObjectPtr<> properties, forward declarations in headers, PrimaryActorTick disabled where unneeded, BlueprintPure/BlueprintCallable API surface for Blueprint hybrid workflow (UFortNetworkLibrary, AFortGameState getters)
- Binary-asset source control done right: Git LFS .gitattributes committed before the first asset (verified as pointer files), Epic-style .gitignore, asset-lock workflow documented in README
- Technical writing that teaches — DEVLOG.md maps every replication concept to the class that demonstrates it and explains tradeoffs (overlap-vs-press interact, listen-vs-dedicated server) in plain language
- Milestone-driven scoping: an explicit M0 definition of done ('we can stand in a world together'), deliberate deferrals written down, and a locked-decisions table — product-thinking discipline rare in hobby game repos


## Career signals

- Documentation is hiring-manager grade for a 1-day-old repo: README has a status line, a systems-to-networking-concepts table, and locked-decisions table; DEVLOG.md narrates *why* each networking choice was made — this is the communication skill senior engineers are paid for
- Process maturity beyond hobbyist norm: Git LFS attributes committed BEFORE the first binary asset (verified — .uasset files are LFS pointers), correct UE .gitignore, conventional commit messages (feat:/docs:/chore:) with scoped bodies
- Learning velocity signal: went from empty repo to a working server-authoritative co-op loop (door, pickup, PlayerState, Server RPC build, GameState clock) verified across two clients in effectively one day (all 9 commits 2026-06-30 to 2026-07-01)
- Activity pattern is a single burst, not sustained delivery — the repo proves fast onboarding to UE5 netcode, not yet shipped-game endurance; a hiring manager will ask 'what happened after Day 1?'
- No CI, no automated tests, no screenshots/video — the repo currently requires a reader to compile UE5 to see anything, which most recruiters/managers will never do
- States the right principle everywhere ('client requests, server decides') and mostly follows it — but the one Server RPC trusts a client-supplied transform, the exact gap a multiplayer-studio interviewer would probe
- Explicitly framed as deliberate career preparation ('networking front-loaded on purpose... rarest, hardest-to-fake gameplay-programming skill') — self-aware skill-market positioning


## Monetization angles

- Not directly monetizable as a product (it's a friends-only sandbox by design); its value is career capital toward UE gameplay/network programming roles, which command top-of-market game-industry salaries
- The DEVLOG already reads like course material — a 'UE5 multiplayer from zero' written/video tutorial series (blog, YouTube, or paid course on the mattdanusergrant.com brand) is the most realistic income path; server-authoritative netcode is a chronically under-taught topic
- Long-term: the co-op survival-sandbox skeleton is a legitimate foundation for a small Steam co-op title (2-4 player niche co-op games are a proven indie category), but that is many milestones away
- Contract/freelance signal: 'can set up UE5 project with correct LFS, replication architecture, and docs' is billable consulting for small studios spinning up multiplayer prototypes


## Standout artifacts to show off

- DEVLOG.md — a Day 1 narrative that maps every UE replication concept (RepNotify, HasAuthority, Server RPC, PlayerState vs GameState) to the exact class demonstrating it; reads like teaching material and doubles as interview prep
- Source/FortKickass/InteractableDoor.h/.cpp — a textbook-clean minimal replicated actor: RepNotify pattern with the fires-only-on-clients subtlety explicitly handled and commented ('the client requests, the server decides, the result replicates back')
- README.md 'What works so far' table — each system mapped to its class and the networking concept it proves; a hiring manager gets the whole repo in 20 seconds
- Source/FortKickass/FortGameState.h + FortPlayerState.h — correct GameState-vs-PlayerState split for shared vs per-player replicated data, with Blueprint-facing pure getters
- .gitattributes + DEVLOG 'Source control before assets' section — evidence of knowing the UE/LFS failure mode and preventing it before it happened


## Cross-repo connections

- mattdanusergrant (personal site): publish DEVLOG.md as a blog/devlog series ('UE5 multiplayer replication from zero, Day 1') with two-client GIFs — the vault already has a case-study-forge pipeline for site-styled writeups; this is the highest-leverage hook since the DEVLOG is already written in publishable teaching voice
- ConductiveOS: README already points to planning docs in the vault at 07_projects/unreal-multiplayer-game/ (LEARNING-ROADMAP, ARCHITECTURE, SETUP) — formalize this as the showcase example of the vault's Builder-avatar workflow producing real shipped code; a session-note trail linking vault plans to fortkickass commits is a unique 'AI-assisted engineering process' portfolio piece
- ronin-survivor: direct design cross-pollination — ronin's authored-map spec + mapbuilder.html grid-painter pattern is a ready-made model for fortkickass world/base persistence (serialize placed ABuildable actors to a v2-style spec); conversely, port fortkickass's DEVLOG discipline back to ronin-survivor which only has design-docs/TODO.md
- mustdesigngames: the survival-slice scoping decisions (overlap-vs-press interact, listen-server-first, M0 definition of done) are ready-made game-design-process content for a design blog/brand
- Shared 'server-authoritative state' expertise transfers to any future multiplayer or agent-sync project across the portfolio — e.g., if jabberjawbreaker or keepingcadence ever need real-time shared state, the authority/replication mental model (and the written explanations of it) carry over
- 10_game-asset-library in ConductiveOS: as fortkickass replaces engine-cube placeholders with real art, route assets through the vault's shared game-asset library convention rather than ad-hoc imports


#LLM-generated
