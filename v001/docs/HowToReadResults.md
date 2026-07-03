//File:    docs/HowToReadResults.md
//Date:    2026-06-22
//Purpose: How to read the throughput numbers in test-summary/ without fooling yourself

# How to read the results

Current throughput and test results live in the per-version `test-summary/` directories
(e.g. `v001/test-summary/`). The top-level `test-summary/README.md` (in the shared results
hub) links to per-machine safeness summaries and detailed reports for ctest, smoke, bench,
verify, etc. Throughput lives in the `bench/` reports.

**Results are per-machine** (hardware + OS determines the `jac313-###` group). Numbers
vary significantly by CPU, cores, RAM, and disk type/speed.

## The short version

- **Look at `test-summary/bench/README.md`** (per version) for throughput.
- Each machine has sections for:
  - Non-durable (flag sweep: 0/2/4/6 flags)
  - Durable at 1M events
  - Durable at 10M events
- Per compiler: **median ops/sec** + low–high band + output size.
- Always read the **median + band**. Different machines show different absolutes.

Hardware matters a lot. Treat numbers as specific to that machine's config, not universal.

See [Benchmarks.md](Benchmarks.md) for how to run and produce these reports.

---

Throughput numbers mislead in **both** directions when you read them wrong. Some look
misleadingly *good* — a buffered durable run flattering a backend that hadn't yet paid for the
disk write. Others look misleadingly *bad* — a single fsync stall dragging an *average* down
far below the rate the backend actually sustains. The fix for both is the same: report the
**median with a low–high band** over N runs, and put the disk flush **inside the timed
region**. This page is the decoder ring: what each number measures, why median+band is the
honest headline, and what the legacy per-test logs do and don't tell you.

See also: [Benchmarks.md](Benchmarks.md) (how to run the suite and read its report) ·
[Results.md](Results.md) (the recorded numbers) · [Architecture.md](Architecture.md) (what
Store is).

---

## Where throughput numbers come from

Throughput is measured by the dedicated `store_bench` (in `Store/tests/matrix/`).
The easy way from the version root:

```
./jac313_test_cli --bench --report
```

This runs the curated **10-config suite** (4 non-durable flag counts at 10M events × 10 runs;
durable jText/SQL/binary at 1M events × 3 runs; and again at 10M events × 3 runs).

Reports appear in `test-summary/bench/README.md` (and per-run .md files). See
[Benchmarks.md](Benchmarks.md) for running details and the `--dry-run` command list.

Each config headline in the report is **median ops/sec + low–high band** across its runs.
The `test-summary/bench/README.md` organizes by machine, with tables for non-durable,
durable@1M, and durable@10M, plus per-compiler columns for median/band/size.

---

## The one thing to remember

> **The headline metric is the MEDIAN over N runs, reported with a low–high band — not the
> fastest run, not the average.** "Fastest of N" (peak / best-of-N) is lucky-max: it quietly
> reports whichever run caught CPU turbo and skipped every stall. The average is the opposite
> failure: one outlier drags it. The median lands on the rate the backend *actually sustains*,
> and the band tells you honestly how spread the runs were.

Concretely, two very different numbers come out of the suite:

| Number | What it is | Notes |
|--------|------------|-------|
| **In-memory ceiling** (non-durable, low flags) | hot loop, no sink attached | Often 4–9M+ ops/sec depending on hardware; σ low. |
| **Durable sustained** | hot loop throttled by disk flush | Typically 0.4–1.2M ops/sec (varies by backend, event scale, and machine). |

Both are real. The mistake is quoting a single lucky run (or peak) as the steady rate. Always use the median + band from the reports.

---

## Why median + band beats fastest and average

The old reports led with **"Peak ops/sec = fastest of N runs."** That is lucky-max: it keeps
only the single run that caught turbo and dodged every fsync stall, so it systematically
overstates what you'll actually get. The obvious alternative — the **average** — fails the
other way: a single bad run drags it.

An earlier SQL run makes this concrete — the *shape* is the lesson, not the absolute rate (which
the fsync fixes above have since lowered). Across its runs, one fsync-stalled run dropped far
below the rest:

- **Average** got dragged down — the stall poisons the headline even though most runs were fine.
- **Fastest** would have hidden the stall entirely, reporting only the luckiest run.
- **Median held** at the rate that run actually sustained — and the **band** reported the low
  *honestly*, as a low edge, instead of burying it (average) or cherry-picking it away (fastest).

That is the whole argument: a one-off stall belongs **in the band**, visible, not averaged
into the headline and not discarded from it.

---

## Durable now flushes inside the clock — for *every* backend

Durable timing does a **flush-and-wait inside the timed region**: the clock counts bytes
**actually written to disk**, not bytes buffered into a queue or the OS page cache. Getting this
right took **two** passes, because the same bug bit two backends in turn:

1. **binary** first looked *fastest* at ~2.7M ops/sec — but the timer stopped before its
   `msync`. Once the final `MS_SYNC` was counted, binary fell to **~0.6M ops/sec**.
2. **jText** then looked fastest at ~2.4M — but its `finalize()` only stream-flushed to the OS
   page cache; **no `fsync` ever ran**. Once `finalize()` fsyncs each of its three split files,
   jText falls to **~0.26M ops/sec** — the *slowest* of the three (text formatting *plus* three
   files to sync). See Bloopers #7.

The honest order (backend-dependent, see current reports):

> binary often edges out the others; jText can be slower due to formatting + multiple files. Exact order and absolutes vary by hardware and event scale — check the live `test-summary/bench/` tables.

Each "fastest" claim was a measurement artifact — bytes in a buffer, not on a platter.

### The trap this closes

You *could* still make durable look fast: size the buffer so it never blocks, let `save_event`
be a cheap enqueue, and let the disk write happen *after* the timer stops. You'd "measure" tens
of millions durable. That number is fiction — it hides the real disk work outside the clock,
which is exactly the buffered-binary mistake above. Flushing inside the timed region is what
keeps the durable number honest.

### Why selective persistence doesn't rescue durable

The flag-routing configs only persist a small fraction of events ("keepers"), yet they don't
fly: the cliff isn't *how much* you write — it's *whether a sink is attached at all*, because
every event pays the per-event routing/enqueue tax whether or not it ends up durable. (If the
flag check moved onto the hot path *ahead* of the writer enqueue, so unkept events skipped the
sink entirely, selective persistence could genuinely approach the in-memory ceiling — that's a
real `ts_store` optimization, not a reporting tweak.)

---

## Legacy: the per-test logs and "test_006 has no ops/sec"

Older docs sourced throughput from per-test logs — `test_005` / `test_007` / `test_008` — and
noted that **`test_006` emits no `ops/sec`**. That distinction is now **legacy**. Throughput
comes from `store_bench`, not from per-test logs, so you no longer read a
backend's rate out of a test's "Slowest run" line.

The functional side of those tests is unchanged. `test_006` is still a tail-reader
concurrency/correctness stress test — writer/reader timing and hit/miss verification, **not**
throughput. So "test_006 has no ops/sec" remains true; it just isn't a gap any more, because no
test is the throughput source. Tail-reader correctness stays covered; throughput lives in the
bench suite.

> The **functional/correctness matrix still exists and is unchanged** — 116 scenarios × 16
> compiler/build/modules/size combos = **1,856**. Only *throughput benchmarking* moved out of
> it into `store_bench --suite`. So 1,856 / 116 / 115 is still accurate **for the functional matrix** —
> just don't read throughput numbers out of it.

---

## Quick reference

| You see… | It means… | Trust it as throughput? |
|----------|-----------|--------------------------|
| Median ops/sec (in bench report) | the rate the config actually sustains | **Yes** — headline for that config + machine |
| The low–high band | run-to-run spread (incl. stalls) | Yes — read the full band |
| "Fastest"/"peak"/"best of N" | lucky-max (turbo + no stall) | **No** — overstates steady rate |
| "Average" with wide band | one outlier dragged it | No — prefer median |
| Non-durable (low flags) median | hot-path ceiling (no sink) | Yes (varies by hardware) |
| Durable median (flush in-clock) | bytes actually on disk | Yes — real sustained durable |
| Old per-test "ops/sec" or `test_006` no ops/sec | legacy functional logs | N/A for throughput (now in bench reports) |

See the per-machine tables in `test-summary/bench/README.md` (and linked Run_*.md) for current
data. Numbers are hardware-specific — compare within the same machine. The core advice remains:
use **median + band**, and durable numbers include the actual disk flush cost.

---

## The short version (again)

Look at `test-summary/bench/README.md` (under the version's test-summary). It shows, per
machine and per compiler:

- Non-durable flag configs: median + band (often several million ops/sec).
- Durable @1M and @10M: median + band + output size (typically hundreds of K to low millions
  ops/sec, varying by backend and hardware).

Always prefer the median + band over any peak/average. Hardware (CPU + disk) dominates the
absolute numbers. See [Benchmarks.md](Benchmarks.md) for how the reports are produced.
