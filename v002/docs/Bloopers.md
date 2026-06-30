//File:    docs/Bloopers.md
//Purpose: The mistakes, owned and explained. Every clean git log hides a messier true story — this is ours.

# Bloopers

A reference repo shows you the polished result. This page shows you the bloopers reel —
the bugs we shipped to ourselves, the hours we donated to the void, and the small mercies
of catching them before anyone else did. We're laughing *with* us. (We're the only ones here.)

---

## 1. 1,856 Tests, Not One You'd Quote 📊

**The headline act, redux.** We had a benchmark matrix of magnificent scale —
`{compiler} × {build_type} × {modules} × {size} × {persist} × {output on/off}`, **1,856
scenarios**, the better part of an afternoon to run. It was *thorough*. It also produced
numbers nobody could trust, and we'd stopped noticing.

The leaderboard quoted the **fastest of N runs** as the headline — so every figure you
cited was, by construction, the luckiest one. Durable persistence only attached on the
*last* run, so the "durable" rows smeared two in-memory runs with one real one. Log files
were full of raw ANSI escape bytes — pretty in a terminal, garbage in an editor or on
GitHub. And the durable board ranked **binary fastest**… because the clock stopped *before*
the `msync`. Binary's headline "2.7M ops/sec" was buffered, never written; its real rate
was **~640K**. The ranking was exactly backwards, and we'd been reading it for weeks.

Maximum testing, minimum signal: more scenarios than a person can hold in their head, each
reporting a figure quietly engineered to flatter.

**The fix (today):** stop measuring *everything badly*, start measuring *a few things
honestly*. The 1,856-combo matrix collapsed to a curated **store_bench suite**
(`store_bench --suite`) — 7 configs then, 10 today (a 10M durable-scaling grid was added since). The headline became the **median** with a low–high band over N runs — an
fsync stall now shows up in the band instead of being hidden or cherry-picked. Durable
timing moved the flush **inside** the clock, which is what finally caught the 4× gap binary had been hiding.
Verify left the hot loop. Total runtime: **~90 seconds**, not an afternoon.

**Moral:** a test suite's job is *trustworthy numbers*, not *many numbers*. If you can't say
what a figure means in one sentence — "median throughput, flush included, over 10 runs" —
more scenarios won't save you, they'll bury the problem deeper. Measure less, measure honestly.

---

## 2. The Non-Distinct Clusterf‑‑k 🏆

**The headline act.** The test matrix is `{compiler} × {build_type} × {modules} × {size}` —
**16 combos** (8 if smoke-only). One day a "full matrix" run took ~2 hours and nobody blinked.
Then someone said the eight most useful words in debugging:

> *"I don't remember a smoke test taking this long."*

It wasn't. It was running **158 smoke combos** instead of **8**, and **86 full** instead of **16**.

**The cause** was one missing keyword. `missing_matrix_combos` builds the run list with:

```sql
SELECT 
    d.compiler
  , d.build_type
  , d.modules
  , d.size_label
FROM v_desired_matrix d
LEFT JOIN runs r ON (… matching identity …)
WHERE (? OR r.id IS NULL)        -- ? = --force
```

Without `--force`, `r.id IS NULL` keeps only the *missing* combos — one row each. Correct.
With `--force`, the filter goes away… and the **`LEFT JOIN runs` emits one row per historical
run of that combo.** No `DISTINCT`. So a combo that had been run 22 times got scheduled **22
times.** Eight desired combos, fanned across an ever-growing pile of run history, = 158.

And it **compounded**: every `--force` sweep recorded more runs, which inflated the *next*
`--force` count. We watched it climb **8 → 9 → 39 → 158** across the session like a sourdough
starter nobody asked for.

**The fix** was, with apologies for the anticlimax, the word `DISTINCT`:

```sql
SELECT DISTINCT 
    d.compiler
  , d.build_type
  , d.modules
  , d.size_label
FROM v_desired_matrix d
LEFT JOIN runs r ON (… matching identity …)
WHERE r.id IS NULL;
```

**Moral:** a `LEFT JOIN` against an append-only history table without `DISTINCT` is a loaded
gun pointed at your wall-clock. Also: trust the human who says "this feels slow." They're
usually right, and they pay the electricity bill.

---

## 3. `localhost.localdomain`: The Sequel Nobody Wanted

We scrub machine hostnames to stable `jac313-NNN` labels so committed results don't leak the
box they ran on. We did this carefully for v002, set a `host_label.local`, verified it — gold star.

Then we ran the v001 matrix and **leaked `localhost.localdomain` into six committed pages**,
because v001 never got its own `host_label.local`. We fixed the *symptom* in one version and
strolled right past the *source* in the other.

It was caught **mid-ingest** by a review pass that noticed the v001 run table climbing in real
time with the wrong hostname. Close one.

**Moral:** when you fix a leak, fix it in *every* version that shares the plumbing — not just
the one you're looking at.

---

## 4. The clang-scan-deps Phantom 👻

One combo in a full run died with:

```
clang-scan-deps: … Assertion `!isAnnotation() && "Annotation tokens have no length field"' failed.
PLEASE submit a bug report to https://github.com/llvm/llvm-project/issues/
```

Panic: did the jText port break a build? We chased it. Re-ran the combo: **passed.** Wiped the
build dir and rebuilt from scratch (full parallel scan storm): **passed.** Rebuilt incrementally
on a dirty tree: **passed.** It refused to reproduce.

Verdict: a **non-deterministic crash inside the LLVM module scanner itself** — not our code, not
our build, just the C++23-modules tooling being haunted (exactly the "quirks & curses" our own
`Modules.md` warns about). We spent real time proving a negative, which is the most thankless
kind of correct.

**Moral:** when a *tool* assert-crashes on valid input and won't reproduce, it's a heisenbug in
the tool. Confirm, shrug, log it, move on.

---

## 5. Death by a Thousand `--force`

Side effect of Blooper #2: every `--force` sweep *also* appended duplicate run records. By the
time we looked, v002 had **78 runs across 32 identities** (some cells recorded 10×) and v001 had
**112 across 96**. The "top throughput" leaderboards were quietly padded with the same scenario
listed over and over — every *number* was real, but the *volume* was theatre.

Deduped to one-run-per-identity (v002 → 32, v001 → 96) via `v_latest_runs`, cascade-cleaning the
orphaned scenario rows, and re-rendered. The bug fed on the history; the history fed the bug.

**Moral:** an append-only results table needs either a "latest wins" read path *or* discipline.
Ideally both.

---

## 6. Friendly Fire (bonus blooper) 🔫🦶

While cleaning up the above, a status-check command ran:

```
pkill -f "jac313_test_cli matrix run-all"
```

…which matched **its own command line** (the pattern was right there in the args) and
SIGTERM'd the shell running it. Exit code 144, very confusing, briefly believed the machine
was cursed. Switched to `pkill -x <exact-name>`. Also discovered the harness politely blocks
foreground `sleep`, which turned a couple of "wait and poll" one-liners into modern art.

**Moral:** `pkill -f` is a footgun when your pattern describes the process you're typing it in.

---

## 7. The jText Sequel: Same Flush, Different Backend 🎬

Blooper #1 caught binary's "fastest durable" number stopping the clock *before* the `msync`. We
fixed binary, moved the flush inside the clock, wrote it up, felt good. Then the durable board
crowned a new champion: **jText at ~2.4M ops/sec**, beating fsync'd binary and SQL by 3–5×.

A *text* format — formatting every event to human-readable lines across three split files —
out-throughputting a packed binary log on *durable* writes. That's not a result, that's a smell.
Someone said the quiet part out loud: *"are these numbers real?"*

They were real measurements of the wrong thing. `JTextSplitEventLog::finalize()` finalized the
jText writers and flushed the `std::ofstream` — into the **OS page cache**. We grepped the entire
jText tree for `fsync`/`fdatasync`/`msync`: **zero hits.** jText's "durable" path never forced a
single byte to the platter. The 2.4M was the speed of writing to RAM the kernel hadn't flushed
yet — *exactly* binary's bug from #1, hiding in a different backend the whole time we were
congratulating ourselves on fixing it.

**The fix:** `finalize()` now flushes and `fsync`s each of the three split files, inside the bench
clock. jText @1M dropped **~2,283,663 → ~255,744 ops/sec — a ~9× correction** — and the durable
ranking *inverted*: **binary ~0.6M > SQL ~0.46M > jText ~0.26M.** jText isn't the fastest durable
backend; it's the **slowest** (text formatting *plus* three files to sync). The number we'd have
bragged about was fiction.

**Moral:** when you fix a measurement bug, fix it **everywhere the measurement is taken** — not
just the backend that happened to be on top when you looked. And when a counterintuitive number
makes you want to celebrate, *that's* the moment to ask "is this real?" before you cry for joy.
The disk does not care how fast you wrote to its cache.

---

## The silver lining

Blooper #2 is *why* the run-all counter now shouts `… of 1,856` (full) / `… of 928` (smoke) on
every line and in the final summary. The expected total is now in your face at all times — so
the next time the number drifts toward four digits, it gets caught in the first ten seconds,
not at hour two. The best guardrails are paid for in hours already lost.
