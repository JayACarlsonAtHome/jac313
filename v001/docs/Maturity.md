# Maturity — an honest self-assessment

Where jac313 actually sits, on a 12-level scale, and why. Like the rest of these docs this
is candid (and AI-assisted): the goal is an accurate read, not a flattering one. The
[main README](../README.md) is the overview.

## The scale

Twelve rungs in three bands — **local**, **professional**, **business**. The level *names*
are the yardstick; the one-line glosses are this doc's reading of what each rung implies.

| # | Level | Roughly means |
|---|-------|---------------|
| 1 | Local throwaway project | A scratch experiment; works once on the author's machine. No tests, no docs. |
| 2 | Local project | Runs reliably for the author; minimal structure and notes. |
| 3 | Good local project | Clean layout, a README, reproducible build, some tests. |
| 4 | Hardened local project | Solid tests, handles edge cases/errors, documented — still single-author scope. |
| 5 | Start of professional work | Versioned/namespaced API, real test discipline, multi-config builds; readable by others. |
| 6 | Professional work | Comprehensive tests + docs, multi-compiler/platform, deliberate design; a peer could maintain it. |
| 7 | **Very good professional work** | Strong test *infrastructure*, memory/thread-safety gating, candid docs, measured engineering choices. High craft. |
| 8 | Hardened professional work | + fuzzing, sanitizers, security review, performance gating — defends against adversarial/edge input. |
| 9 | Start of business quality | CI/CD, versioned releases, multiple contributors, a review process; a team can depend on it. |
| 10 | Business quality | Supported releases, compatibility guarantees, broad CI matrix, issue/PR workflow, external consumers. |
| 11 | Very good business quality | Security/supply-chain scanning, signed/reproducible releases, mature governance, SLAs. |
| 12 | Tier-1 business quality | The gold standard: rigorous everything, long-term support, formal/audited process. |

## Where jac313 sits: **~7**, judged as a solo private tool

The honest split is **craft vs. structure**, and they're at different rungs.

**The craft reaches 7–8.** What earns it:
- a custom C++ test orchestrator (`jac313_test_cli`) — not shell scripts;
- **RunIdentity**: one definition driving DB keys, the results path, and rendered pages, so they can't drift;
- a multi-compiler × build-type × persist-backend matrix, smoke and full tiers;
- a measured `import std` module experiment (recorded, not hand-waved);
- and a **valgrind memcheck + helgrind/DRD pre-push gate** ([cleancheck](Memory-And-Concurrency.md)) that already found and fixed a real data race.

**The structure is what caps it.** No CI, no releases, one author, results committed into the repo. That's a 5–6 shape. The composite lands at a defensible **7 — "very good professional *solo* work."**

## Why it isn't higher — and why that's a choice, not a gap

The rungs above 7 split cleanly:

- **Rung 8 (hardened professional) is reachable** and on the roadmap: security review + fuzzing (the jText parser is the natural target), optionally ASan/UBSan alongside valgrind. **Deferred for cost/budget, not priority.**
- **Rungs 9+ (business quality) require organizational scaffolding** — CI/CD, versioned releases, multiple contributors, external consumers. These are **deliberate non-goals** here.

On the CI point specifically, because it's the usual "why not higher" reflex: for a **solo** project the value of *unbypassable, runs-in-a-clean-room* CI is low — the person who'd skip the check is the same person who wrote it. The enforcement that matters is "the gate runs before code leaves my machine," and that's exactly what the installed **pre-push `verify-lite` hook** does. Paid GitHub Actions isn't justified for one developer on their own boxes, so **the local hook *is* the CI** — a correct cost/benefit call, not a missing feature. (Free options exist if that ever changes — Actions is free on public repos, and self-hosted runners are free on private ones — but they solve problems a solo project doesn't have.)

## The honest caveats

- It's **AI-assisted** work (see the README author's note) — the craft is real, but so are the limits.
- The thread/memory gate runs at **smoke scale on gcc-15 Debug**, a representative subset — a tripwire, not an exhaustive sweep.
- **No external validation**: one author, no second reviewer, no consumers in the wild.

## Bottom line

**7 is the chosen ceiling, with 8 optional via the hardening backlog — not a deficit to fix.**
The gap to 9+ is organizational scaffolding this project deliberately doesn't buy. Judged for
what it actually is — a solo, private, single-developer tool — it's very good professional work
that runs its own pre-push memory/thread-safety gate and ships browsable real sample output and
logs. That's the rung, honestly.

Part of the [jac313](../README.md) monorepo.
