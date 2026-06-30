//File:    docs/HowToReadResults.md
//Date:    2026-06-22
//Purpose: How to read the throughput numbers in test-summary/ without fooling yourself

# How to read the results

> **v002 note:** v002 has now recorded its **own** battery (rhel-9.8/ssd, both gcc15 and
> clang at C++26 — see [Results.md](Results.md)), and the figures land where this page says
> they should: in-memory Release median ~25M, durable ~0.25–0.6M (binary ~0.6M > SQL ~0.46M >
> jText ~0.26M, all fsync'd to disk — see Bloopers #7). The numbers below read as v002's own; the methodology is
> version-independent.

## The short version

Don't want the details? On this machine (gcc15, Release, SSD):

- **In-memory logging:** ~**24M** events/sec — *median*, with a low–high band
- **Logging to disk (durable):** ~**0.25–0.6M** events/sec — **binary ~0.6M > SQL ~0.46M > jText
  ~0.26M**, every backend fsync'd (jText was the last buffered one — see Bloopers #7)

Both **depend greatly on hardware** (CPU clock + disk speed) — an older Xeon with a 7200-rpm
HDD lands nearer ~3–5M in-memory. Treat these as ballpark, not guarantees.

That's the whole story. Everything below is *why*, and how to avoid misreading the numbers.

> To run the benchmarks and see the report format, see [Benchmarks.md](Benchmarks.md).

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

Throughput is now measured by a dedicated instrument, **`store_bench`**, driven by
**`store_bench --suite`** (both in `Store/tests/matrix/`). Run it from the repo root:

```
./jac313_store_bench --suite
```

It is a **curated 10-config suite**, not a giant matrix:

- a **non-durable flag sweep** — 0 / 2 / 4 / 6 flags, 10M events × 10 runs,
- the **durable backends** — jText / SQL / binary, 1M events × 3 runs, and
- the **durable backends again at 10M** — the scaling grid (jText / SQL / binary, 10M × 3 runs).

Each config's headline is the **median ops/sec** with a **low–high band** across its runs.
Verify runs **last-run-only** — correctness is not throughput, so it stays out of the hot loop.

---

## The one thing to remember

> **The headline metric is the MEDIAN over N runs, reported with a low–high band — not the
> fastest run, not the average.** "Fastest of N" (peak / best-of-N) is lucky-max: it quietly
> reports whichever run caught CPU turbo and skipped every stall. The average is the opposite
> failure: one outlier drags it. The median lands on the rate the backend *actually sustains*,
> and the band tells you honestly how spread the runs were.

Concretely, two very different numbers come out of the suite:

| Number | What it is | Honest range (gcc15, Release, SSD) |
|--------|------------|------------------------------------|
| **In-memory ceiling** | hot loop, no sink attached | **~15–25M ops/sec** (reference HW hit ~23M, σ ~1–2%) |
| **Durable sustained** | hot loop throttled by the disk it flushes to | **~0.25–0.6M ops/sec** (binary > SQL > jText) |

Both are real. The mistake is quoting a single lucky run as if it were the steady rate.

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

The honest order, every backend forced to disk:

> **binary ~0.6M&nbsp;&nbsp;>&nbsp;&nbsp;SQL ~0.46M&nbsp;&nbsp;>&nbsp;&nbsp;jText ~0.26M**

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
comes from `store_bench --suite` / `store_bench`, not from per-test logs, so you no longer read a
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
| Median ops/sec (bench config) | the rate the config actually sustains | **Yes** — this is the headline |
| The low–high band | run-to-run spread, incl. any one-off stall | Yes — read it, don't ignore it |
| "Fastest"/"peak"/"best of N" | lucky-max: turbo + no stall | **No** — overstates the steady rate |
| "Average" with a wide band | one outlier may have dragged it | No — prefer the median |
| In-memory (non-durable) median | hot-path ceiling, ~15–25M | Yes (±HW/turbo noise) |
| Durable median, flush in-clock | bytes actually written to disk | Yes — the real durable number |
| `test_006` / no `ops/sec` | correctness test; **legacy** non-source | N/A — throughput is in the store_bench suite |

**The two true stories:** in-memory hot path ~15–25M ops/sec, and durable-to-disk ~0.25–0.6M
ops/sec with the honest order **binary ~0.6M > SQL ~0.46M > jText ~0.26M** (both binary's old
~2.7M and jText's old ~2.4M were buffered measurements, not flushes — see Bloopers #7). Read the
**median + band** the bench suite prints; treat any "fastest"/"peak" figure as lucky-max, and any
average with a wide band as outlier-dragged.

---

## The short version (again)

On this machine (gcc15, Release, SSD):

- **In-memory logging:** ~**24M** events/sec — *median*, with a low–high band
- **Logging to disk (durable):** ~**0.25–0.6M** events/sec — **binary ~0.6M > SQL ~0.46M > jText
  ~0.26M**, every backend fsync'd (see Bloopers #7)

Both **depend greatly on hardware** (CPU clock + disk speed) — an older Xeon with a 7200-rpm
HDD lands nearer ~3–5M in-memory. Treat these as ballpark, not guarantees.
