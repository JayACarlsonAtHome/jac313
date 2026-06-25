//File:    docs/Benchmarks.md
//Date:    2026-06-24
//Purpose: How to run the throughput benchmarks and read a reference run

# Throughput benchmarks

## What it is

Throughput is measured by the `store_bench` instrument (in
`Store/tests/matrix/`); the curated suite is its own built-in `--suite` mode —
no driver script. It is a *curated* 10-config suite — deliberately small and
trustworthy, not the big functional test matrix. Its job is to give numbers
you can stand behind, not coverage.

It is honest by construction:

- The headline is the **median** with a **low–high band** over N runs — never
  "fastest" or "peak".
- For durable persistence the flush happens **inside** the timed region, so the
  cost of getting bytes to disk is counted.
- Verify is **last-run-only**, so checking correctness never taxes the timed
  runs.

## How to run

`store_bench` *is* the suite — no driver script. From the build directory
(where `jac313_store_bench` was built):

```bash
./jac313_store_bench --suite --dry-run         # print the copy-paste command list
./jac313_store_bench --suite --db results.db   # run the curated 10 and record each
./jac313_store_bench --clear  --db results.db  # wipe THIS host+OS's rows (re-measure clean); other machines/OSes untouched
./jac313_store_bench --report --db results.db  # write the index + per-run pages from the DB
```

`--db <path>` appends one row per config to a SQLite database (written via
Qlite) — host, versions, config, and the full distribution — so re-running
**accumulates history**. Every row carries a **`group_id`**: one per
**(host, os)** — the container for a machine running an OS (externally it
renders as **`Run_NNN`**). A re-run on the same machine+OS reuses its group; a
new machine, or a new OS on the same box, gets the next group.

`--report` reads the DB back and **writes files** (no stdout redirect): a
summary **`README.md`** — one row per group (`group_id` · HW details ·
`Run_NNN` link · max ops/sec, so you can see at a glance which machines to use
or ignore) — plus one **`Run_NNN.md`** detail page per group (the full
clang-vs-gcc tables). Pages land next to the DB by default, or in `--out <dir>`;
a `Run_NNN.md` whose group was cleared is pruned automatically. Pass
`--jtext-ver vX.Y` so the recorded row carries the jText version. Each config is
also a standalone `store_bench` command (see below) for running one in isolation.

## The 10 configs

This is the `--dry-run` output (build dir shown as a placeholder): 4 non-durable flag
steps, then the durable backends at **1M** and again at **10M** (the scaling grid).

```bash
cd <your-build-dir>/Store/tests/matrix          # where jac313_store_bench was built
./jac313_store_bench --threads 50 --events-per-thread 200000 --runs 10 --persist none --flag-count 0  # 0 flags, non-durable
./jac313_store_bench --threads 50 --events-per-thread 200000 --runs 10 --persist none --flag-count 2  # 2 flags, non-durable
./jac313_store_bench --threads 50 --events-per-thread 200000 --runs 10 --persist none --flag-count 4  # 4 flags, non-durable
./jac313_store_bench --threads 50 --events-per-thread 200000 --runs 10 --persist none --flag-count 6  # 6 flags, non-durable
./jac313_store_bench --threads 50 --events-per-thread  20000 --runs  3 --persist jtext                 # durable jtext  (1M)
./jac313_store_bench --threads 50 --events-per-thread  20000 --runs  3 --persist sql                   # durable sql    (1M)
./jac313_store_bench --threads 50 --events-per-thread  20000 --runs  3 --persist binary                # durable binary (1M)
./jac313_store_bench --threads 50 --events-per-thread 200000 --runs  3 --persist jtext                 # durable jtext  @10M
./jac313_store_bench --threads 50 --events-per-thread 200000 --runs  3 --persist sql                   # durable sql    @10M
./jac313_store_bench --threads 50 --events-per-thread 200000 --runs  3 --persist binary                # durable binary @10M
```

## Sample report

A reference run (one machine — your numbers will differ):

```
Host:     Intel Core Ultra 7 265 · 12 cores · 23 GB · Fedora Linux 44 · Release · threads=50
Versions: Store v002.004 · Qlite v002.002 · jText v002.002
```

Flag-overhead (non-durable) — 10M Events × 10 Runs = 100M Events per config:

| Flags | Median Ops/Sec | σ    |
|-------|----------------|------|
| 0     | 23,329,003     | 1.6% |
| 2     | 23,253,779     | 1.0% |
| 4     | 23,425,087     | 1.2% |
| 6     | 23,832,126     | 1.8% |

Durable — 1M Events × 3 Runs = 3M Events per config (median | low–high band):

| Backend | Median Ops/Sec | Band — Low–High (Ops/Sec) |
|---------|----------------|---------------------------|
| jText   | 2,418,614      | 2.05M – 2.47M             |
| SQL     | 1,991,445      | 0.34M – 2.05M             |
| binary  | 641,263        | 0.52M – 0.70M             |

## Takeaways

**Flags are free.** Going 0→6 flags is flat within ~2%, and the deltas don't
even trend — it's pure noise.<br>
Setting flags has no measurable hot-path cost.

**The honest durable ranking is jText ≈ SQL (~2.1M) > binary (~0.64M).**
Binary's old buffered number (~2.7M) was hiding the truth: the ops/sec used to stop
*before* the `msync`, so the flush wasn't counted. Moving the flush inside the
clock exposed binary as actually the slowest. Note the SQL row's low of ~0.34M —
a real fsync stall. The median (1.99M) ignores it while the band still reports
it, which is exactly why median+band beats average (which the stall would drag
down) and "fastest" (which would hide it).

For what each number means, see [HowToReadResults.md](HowToReadResults.md).
