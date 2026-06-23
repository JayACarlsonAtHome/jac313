//File:    docs/HowToReadResults.md
//Date:    2026-06-22
//Purpose: How to read the throughput numbers in test-summary/ without fooling yourself

# How to read the results

The matrix records a lot of `ops/sec` numbers. Some of them are easy to misread in a way
that *flatters* the project — and once you see how the tests are built, you can't unsee it.
This page is the decoder ring: what each number actually measures, which ones are real
throughput, which ones aren't, and why.

See also: [Results.md](Results.md) (the recorded numbers) · [RunAllTests.md](RunAllTests.md)
(how to produce them) · [Architecture.md](Architecture.md) (what Store is).

---

## TL;DR — the one thing to remember

> **"Peak ops/sec" is the fastest of N runs, and persistence is only attached on the *last*
> run. So for every persist backend, the peak is measured during a run that wrote nothing to
> disk. It is the in-memory hot-path speed wearing the backend's label.**

Concretely, two very different numbers come out of the same scenario:

| Number | What it is | Honest range (gcc15, Release, SSD) |
|--------|------------|------------------------------------|
| **In-memory ceiling** | hot loop, no sink attached | **~15–25M ops/sec** |
| **Durable sustained** | hot loop throttled by the disk it's writing to | **~2.4–2.7M ops/sec** |

Both are real. The mistake is quoting the first one as if it were the second.

---

## How a throughput test actually runs

`test_005` / `test_007` / `test_008` don't do one run — they do **N runs of the same
workload** (full tier: 3 runs). By deliberate design, **only the last run attaches
persistence**; the earlier runs are pure in-memory. The test prints this itself:

> *"Persistence will only be attached on the last run (previous runs measure clean hot path;
> only final run produces persist artifacts)."*

So for a `jtext` scenario the timeline is:

- **Run 1** — in-memory only, jText writes nothing → fast
- **Run 2** — in-memory only, jText writes nothing → fast
- **Run 3** — jText actually writes to disk → slow

The runner then reports:

```
Fastest run : ...  → 24,148,756 ops/sec   ← run 1 or 2: in-memory, nothing written
Slowest run : ...  →  2,466,152 ops/sec   ← run 3: the run that ACTUALLY wrote jText
Average     : ...  →  6,055,101 ops/sec
```

Only the **Slowest run** reflects the named backend. The headline **Peak** (= Fastest) is the
in-memory run. (Mechanics: `Store/tests/matrix/test_005_TS.cpp` — timer at lines 47/95,
`attach_persistence` on the last run only at ~line 187.)

---

## Reading a RUN.md scenario row

```
| jac313_store_005_XS | off | jtext | 100 | 24,148,756 | 20.40 MiB |
                                            ^^^^^^^^^^^^
                                            in-memory peak, NOT jText throughput
```

- **run ms** — wall-clock for the scenario (this *does* include the persist run, so it's a
  fair relative cost signal across backends).
- **Peak ops/sec** — the clean in-memory run. **Same workload → same number regardless of
  backend.** Do not read this as the backend's write speed.
- **Log size** — the on-disk artifact the persist run produced. Real.

To get the backend's actual durable rate, open the linked log and read the **Slowest run**
line.

---

## Why the leaderboard looks absurd (and is)

`matrix top` / `v_top_throughput` ranks scenarios by Peak. Because Peak is the clean run, the
list is effectively ranked by *"whose in-memory run got luckiest"* — and it cheerfully puts a
**jText persist scenario at #1 with 24M ops/sec**, which is self-evidently impossible for a
text-serializing backend. It's not a bug in jText; it's the metric measuring the wrong run.

### The numbers are also noisy

All four backends of one test run the *identical* in-memory code on their fast runs, yet the
recorded peaks scatter across two clean bands:

```
~22–24M band:  none_off 21.2M · none_on 22.3M · binary_off 22.2M · jtext_on 24.1M
~14–15M band:  binary_on 14.0M · jtext_off 14.3M · sql_off 15.4M · sql_on 14.5M
```

Same workload, same code. The split is **CPU turbo/frequency state** — a run that caught
turbo lands ~22–24M, one at base clock lands ~14M. Since the metric keeps only the *single
fastest* of ~2 clean runs, each scenario's number is just "which band did my luckiest sample
fall in." Treat any single Peak as ±40%; trust the **band**, not the digits.

---

## The persist numbers are honest (here's why)

It's tempting to assume the slow persist run is dragged down by setup/teardown. It isn't —
the timer excludes both:

- **Setup is free.** `attach_persistence` (buffer allocation + writer thread spawn) happens
  *before* the timer starts.
- **Teardown is free.** The timer stops the instant the worker threads join; the writer's
  final flush happens *after* that.

So why is the persist run still ~6× slower than clean? **Backpressure.** The double buffer is
bounded — `DoubleBufferedWriter(sink, 10'000)`. Push 1,000,000 events through a 10,000-slot
buffer and it fills immediately; from then on the hot loop **blocks waiting for the disk to
drain** before it can continue. That blocking is inside the timed loop. So ~2.7M is the genuine
**sustained rate at which events can reach disk** — the right thing to measure.

### Why selective persistence doesn't rescue it

`test_008` only persists 20,000 of 1,000,000 events (flag routing — "keepers"). You'd expect
it to fly. It doesn't:

```
008 flags  —   2% to disk  → 2,785,050 ops/sec
005 binary — 100% to disk  → 2,667,947 ops/sec
005 none   —   0% to disk  → 15,370,427 ops/sec
```

A **50× cut in disk writes bought ~4%.** The cliff isn't *how much* you write — it's *whether
a sink is attached at all*, because every event pays the per-event routing/enqueue tax on the
persist run whether or not it ends up durable. In today's Store, "keep 1 in 100" still costs
~2.8M, not 24M. (If the flag check moved onto the hot path *ahead* of the writer enqueue, so
unkept events skipped the sink entirely, selective persistence could genuinely approach the
in-memory ceiling — that's a real `ts_store` optimization, not a reporting tweak.)

### The trap to avoid

You *could* make persist look fast: size the buffer to 10,000,000 so it never blocks, let
`save_event` be a cheap enqueue, and let the whole disk write happen at teardown — *outside
the timer*. You'd "measure" ~20M durable. That number is fiction: it just hides the real disk
work in an untimed teardown. The bounded buffer is what keeps the measurement honest.

---

## Tiers and special cases

### Smoke is not a throughput measurement

Smoke = ~1,000 events (10 threads × 100), 1 run. At that size **thread spin-up dominates the
timer**, so smoke "ops/sec" (often sub-1M) is a setup-cost artifact, not throughput. Smoke
exists to prove correctness fast. **Read throughput only from the `full` / `xFull` tier**,
where every scenario clears ~8.8M and setup is negligible.

### test_006 has no ops/sec (by design)

`test_006` is a tail-reader concurrency/correctness stress test — it measures writer/reader
timing and hit/miss verification, not throughput, and emits no `Fastest run → … ops/sec`
line. Its Peak cell is rendered **N/A** on purpose. A blank there is correct, not missing data.

---

## Quick reference

| You see… | It means… | Trust it as throughput? |
|----------|-----------|--------------------------|
| Peak ops/sec on a `none` scenario | in-memory hot-path ceiling | Yes (±band noise) |
| Peak ops/sec on `binary`/`jtext`/`sql` | the **clean** run, not the backend | **No** — read the log's Slowest run |
| Slowest run (persist scenario) | durable sustained rate, backpressure-bound | Yes — this is the real durable number |
| Any smoke-tier ops/sec | setup-dominated | No |
| test_006 Peak = N/A | correctness test, no throughput | N/A by design |
| `matrix top` ranking | ranked by clean-run luck | No — in-memory + noise |

**The two true stories:** in-memory hot path ~15–25M ops/sec, and durable-to-disk ~2.4–2.7M
ops/sec across binary ≳ jText ≳ SQL. Anything claiming a persist backend writes tens of
millions of events per second is reading the clean run by mistake.
