# Throughput benchmarks

> **What changed (Store v002.022, results schema 2), and why.** The bench used to time one sink
> at a time. A real program does not run like that: it writes its replay log, a keeper file and a
> database **at once**, and each event goes only where its flags send it. The bench now starts
> with that **production run**, records three numbers for it instead of one, and the report shows
> it first. The single-sink and flag-sweep configs are still there, after it, so you can see what
> each piece costs on its own.

## Why this bench exists

1. **To test in a production-like environment**, so you know what to expect in your own. The
   production run is "a truer representation of how a production machine would actually be
   running": several sinks working at the same time, on the same disk, from the same event stream.
2. **To be the framework for timing future contracts.** The same bench builds and runs under each
   of the four contract modes (`enforce`, `observe`, `quick_enforce`, `ignore`; see
   *Contract modes* below), so the cost of a contract, now or later, is one more run to compare.
3. **To give compiler builders and CMake an easy benchmark.** One command builds Release and runs a
   fixed, recorded suite on real code; see *For compiler and CMake builders* below.

## What it is

Throughput is measured by `store_bench` (`Store/tests/matrix/store_bench.cpp`). Its `--suite`
mode is the curated suite — 16 configs, deliberately small and trustworthy, not the big
functional test matrix. Its job is to give numbers you can stand behind, not coverage.

It is honest by construction:

- The headline is the **median** with a **low–high band** over N runs — never "fastest".
- Every durable run times the **whole process** (fork → exit), so each sink's finalize —
  draining its writer and **syncing bytes to disk** (`fsync` / `msync`) — is inside the clock.
- Verify is **last-run-only**, and the read-back is one extra **untimed** run, so checking
  correctness never taxes the timed runs.

## The production run

Three sinks are attached to one store, one worker thread each, all fed by the same producers:

| Sink | Choices | `PersistMode` | Gets |
|---|---|---|---|
| the replay log | binary (can be turned off) | `All` | every event |
| the keeper file | jtext \| html \| json | `KeeperOnly` | only events flagged `KeeperRecord` (bit 1) |
| the database | sql (SQLite) \| pg-local \| pg-remote | `DatabaseOnly` | only events flagged `DatabaseEntry` (bit 2) |

**The mix.** Event *k* (= thread × events-per-thread + index) gets `KeeperRecord` when
*k* % 100 < `keeper_pct`, and `DatabaseEntry` when (*k* × 37 + 11) % 100 < `database_pct`. The
defaults are 10 % and 10 %. It is deterministic, so the read-back knows each sink's exact subset,
and the two subsets overlap about as much as two independent picks would (1 % of events at 10/10).
Why a mix: most events in a real program only need the replay log; a few are worth keeping in a
readable file, and a few are worth querying later. 10/10 is a starting point — set your own.

**The three numbers** (each the median over the runs):

| Number | What it is | What it tells you |
|---|---|---|
| **hot path ops/sec** | events ÷ the producers' wall time, from the first thread starting to the last one joining, before finalize | how fast your program can log while the sinks keep up in the background |
| **drain ms** (binary / keeper / database) | `Store::drain_time_us(i)`: from attaching that sink to its finalize returning | when each sink has everything on disk; the slowest one is when you may exit |
| **durable total ops/sec** | events ÷ the whole process, start to exit (as for every config) | what `time ./your_program` would show |

A drain time includes the hot-path time (the sink starts draining the moment it is attached), so
it is never smaller than the hot path; the difference is how far that sink lags behind.

**Read-back** (`--readback count|sample|all`) checks each sink separately: the replay log against
every event, the keeper file against exactly the `KeeperRecord` events and the database against
exactly the `DatabaseEntry` events. A sink that holds an event it should not have fails.

### Picking the sinks

- **In the table:** `tests/production.jtext` holds one row: `keeper_sink`, `database_sink`,
  `keeper_pct`, `database_pct`, `binary`. One row serves the smoke, full and bench profiles. Defaults: jtext, sql,
  10, 10, true. `jac313_test_cli --bench` reads it and passes it on.
- **In Jac313Runner:** the *Production* row of the *Persist to* box (Keeper, Database, the two
  percentages, *binary replay log*). It shows the row of the chosen version's file; **Save as
  default** writes it back (through jText's writer), and the bench command passes it as
  `--production`.
- **On the command line:** `jac313_test_cli --bench --production keeper=html,database=pg-local`
  or `jac313_store_bench --production keeper=json,database=sql,keeper_pct=25,database_pct=5,binary=off`
  (any keys left out keep their defaults).

### Compromises (what the production run does not do yet)

- **The database batch is not calibrated**: the replay log and the keeper run at their type's
  calibrated batch (`io_best_fit`); the database (sql, pg-local, pg-remote) always runs at the
  default batch (10,000). `--batch N` gives all three N. The calibration
  (`jac313_store_bench --calibrate`) sweeps binary, jtext, html and json only.
- **The mix is fixed by event index**, not random: the same events are flagged every run, which is
  what makes the read-back exact.
- **The other flag bits route nothing**: only `KeeperRecord` and `DatabaseEntry` are wired (see
  [flags](store/flags.md)); `SendNetwork`, `HotCacheHint` and the rest stay off in this run.
- **pg-local / pg-remote need a reachable server**; without PostgreSQL built in, the production
  run with a PG database is skipped with a note.
- **results.db**: the pair is stored in `parameter.persist` (`jtext+sql`; `jtext+sql/nobinary`
  without the replay log) and the mix in the config name (`production jtext+sql 10/10`); the three
  new figures are `testRun.hot_ops`, `drain_binary_ms`, `drain_keeper_ms`, `drain_database_ms`
  (0 for every other config). Schema 2: an older `results.db` is rebuilt on first open (it holds
  test data only).

## How to run

From `v002/` — the CLI builds Release and drives the suite for you, and records it:

```bash
./jac313_test_cli --bench            # the suite, recorded to results.db; report rendered at the end
./jac313_test_cli --report           # re-render the report pages from results.db
```

`store_bench` *is* the suite — no driver script. From the build directory (where
`jac313_store_bench` was built):

```bash
./jac313_store_bench --suite --dry-run         # print the copy-paste command list
./jac313_store_bench --suite --db results.db   # run the 16 and record each
./jac313_store_bench --threads 8 --events-per-thread 20000 --runs 3 \
    --production keeper=jtext,database=sql --readback sample    # one production run
```

`--db <path>` appends one row per config to `results.db` (written via Qlite) — versions, config
and the full distribution — so re-running **accumulates history**. Every row carries the machine's
**group** (`jac313-###`: one per distinct hardware + OS); `jac313_test_cli --group-id` shows which
group this machine records under (read-only).

`jac313_test_cli --report` writes `test-summary/README.md` and per-area pages; the **bench** page is
per machine, per compiler, in four groups (see *Report* below).

## The 16 configs

The `--dry-run` output, in the order they run: the production run first, then each database
alone, then each file alone (each at **1M** and **10M** events), then the non-durable flag sweep.

```bash
cd <your-build-dir>/Store/tests/matrix          # where jac313_store_bench was built
./jac313_store_bench --threads 50 --events-per-thread  20000 --runs  3 --production keeper=jtext,database=sql,keeper_pct=10,database_pct=10,binary=on  # production jtext+sql 10/10
./jac313_store_bench --threads 50 --events-per-thread 200000 --runs  3 --production keeper=jtext,database=sql,keeper_pct=10,database_pct=10,binary=on  # production jtext+sql 10/10 @10M
./jac313_store_bench --threads 50 --events-per-thread  20000 --runs  3 --persist sql      # durable sql
./jac313_store_bench --threads 50 --events-per-thread 200000 --runs  3 --persist sql      # durable sql @10M
./jac313_store_bench --threads 50 --events-per-thread  20000 --runs  3 --persist binary   # durable binary
./jac313_store_bench --threads 50 --events-per-thread 200000 --runs  3 --persist binary   # durable binary @10M
./jac313_store_bench --threads 50 --events-per-thread  20000 --runs  3 --persist jtext    # durable jtext
./jac313_store_bench --threads 50 --events-per-thread 200000 --runs  3 --persist jtext    # durable jtext @10M
./jac313_store_bench --threads 50 --events-per-thread  20000 --runs  3 --persist html     # durable html
./jac313_store_bench --threads 50 --events-per-thread 200000 --runs  3 --persist html     # durable html @10M
./jac313_store_bench --threads 50 --events-per-thread  20000 --runs  3 --persist json     # durable json
./jac313_store_bench --threads 50 --events-per-thread 200000 --runs  3 --persist json     # durable json @10M
./jac313_store_bench --threads 50 --events-per-thread 200000 --runs 10 --persist none --flag-count 0  # 0 flags, non-durable
./jac313_store_bench --threads 50 --events-per-thread 200000 --runs 10 --persist none --flag-count 2  # 2 flags, non-durable
./jac313_store_bench --threads 50 --events-per-thread 200000 --runs 10 --persist none --flag-count 4  # 4 flags, non-durable
./jac313_store_bench --threads 50 --events-per-thread 200000 --runs 10 --persist none --flag-count 6  # 6 flags, non-durable
```

`--persist-list` (from the CLI's `--persist`) picks which databases and files run alone, and may
add pg-local / pg-remote; they still run databases first, then files.

## Contract modes

Contracts (C++26 pre/post-conditions and assertions; the project's `JAC313_PRE` and friends) can
be evaluated four ways: **enforce** (a violation prints and ends the program; the default),
**observe** (prints and goes on), **quick_enforce** (ends it at once, silently) and **ignore** (not
checked). Which modes a compiler has on each OS is a table: `Setup/contracts.jtext` (native C++26
contracts — GCC 16 on v002 — have all four; elsewhere enforce and ignore).

The everyday gates take `--contracts <mode>` for the ctest and smoke builds. For the bench, build
one Release tree per mode and run the same config in each:

```bash
for m in enforce observe quick_enforce ignore; do
  ./jac313_test_cli configure --release --build-dir build-bench-gcc16-$m --compiler gcc16 --contracts $m
  cmake --build build-bench-gcc16-$m --target jac313_store_bench
  build-bench-gcc16-$m/Store/tests/matrix/jac313_store_bench --threads 50 --events-per-thread 20000 \
      --runs 5 --production keeper=jtext,database=sql
done
```

Compare the hot path, the drain times and the durable total across the four. This is the
framework for timing future contracts: as more of the code gains contracts, the same four runs
show what each mode costs in your environment. **Not yet:** results.db records these runs as plain
`Release` (the mode is not a parameter column yet), so compare the printed lines, or record each
mode into its own `--db` file.

## For compiler and CMake builders

One command, from `v002/` (or `v001/` for C++23):

```bash
./jac313_test_cli --bench            # Release build per compiler (gcc, then clang), then the suite
```

It configures `build-bench-<compiler>` in **Release** through CMake, builds the
`jac313_store_bench` target, runs the 16 configs and records them; the report
(`test-summary/bench/README.md`) shows one column set per compiler, side by side. To compare:

- **Compilers** (gcc vs clang, or two versions of one): the same machine, the same suite; read the
  median and band of each config, and the production run's hot path (codegen of the hot loop)
  against its drain times (I/O and the sink code).
- **CMake** (versions, generators, `--modules` vs headers): `jac313_test_cli configure --release
  --build-dir <dir> --compiler <label> [--modules]`, then `cmake --build <dir> --target
  jac313_store_bench`; the build time is in `jac313_test_cli build-times`, the run time in the
  bench.
- Keep one machine per comparison: throughput is hardware-specific, so the report never mixes
  machines.

## Report

`jac313_test_cli --report` → `test-summary/bench/README.md`: per machine, per compiler (latest run
each), four groups in this order, each split @1M / @10M and sorted by median ops/sec:

1. **Production** — hot path ops/sec · drain ms (binary / keeper / database) · durable median
   ops/sec · band · size.
2. **Databases** — sql, pg-local, pg-remote alone: median · band · size.
3. **Files** — binary, jtext, html, json alone: median · band · size.
4. **Non-durable (flag sweep)** — 0 / 2 / 4 / 6 flags, no sink.

Each run's detail page (`Run_NNN.md`) lists the same, production figures included.

## Sample report

A small production run (8 threads × 20,000 events, 3 runs, jtext + sql, read-back `sample`;
Intel Core Ultra 7 265 · 20 cores · Fedora 44 · gcc 16 Release — your numbers will differ):

```
READBACK backend=binary mode=sample status=PASS expected=160000 found=160000 ... checked=160000
READBACK backend=jtext mode=sample status=PASS expected=16000 found=16000 ... checked=16000 subset=keeper
READBACK backend=sql mode=sample status=PASS expected=16000 found=16000 ... checked=299 subset=database
  Median             : 3,500,175 ops/sec          (durable total)
  Hot path           : 7,153,715 ops/sec (median; producers until they join, before finalize)
  Drain (median)     : binary 27 ms · keeper jtext 27 ms · database sql 26 ms
```

The single-sink and flag-sweep numbers below are an older reference run (Store v002.008, one
machine, threads=50):

Flag-overhead (non-durable) — 10M events × 10 runs per config (median | low–high band):

| Flags | Median Ops/Sec | Band — Low–High (Ops/Sec) |
|-------|----------------|---------------------------|
| 0     | 13,061,789     | 11.11M – 15.92M           |
| 2     | 13,423,641     | 10.45M – 15.89M           |
| 4     | 12,765,779     | 11.25M – 15.71M           |
| 6     | 11,450,885     | 10.75M – 15.56M           |

Durable, one sink alone — 1M events × 3 runs per config (median | low–high band):

| Backend | Median Ops/Sec | Band — Low–High (Ops/Sec) |
|---------|----------------|---------------------------|
| binary  | 2,594,714      | 2.55M – 2.61M             |
| jText   | 2,043,991      | 2.03M – 2.10M             |
| SQL     | 1,456,859      | 1.45M – 1.48M             |

## Takeaways

**Flags are free.** Going 0 → 6 flags is flat within noise: setting flags has no measurable
hot-path cost — which is why routing by flag in the production run costs nothing on the hot path.

**The hot path outruns every sink.** In the production run the producers finish before the sinks
do; how long after is the drain time. A program that logs in bursts gets the hot-path rate; one
that logs flat out for a long time gets the slowest sink's rate.

**Durable ranking, one sink alone (that box): binary > jText > SQL; HTML and JSON sit with the
other text sinks.** Every number counts real durability (binary `msync`s its mmap at finalize, the
text sinks `fsync` their files, SQL commits through SQLite). The text-sink and SQL order depends on
the disk; read the **median + band**, where a one-off fsync stall lands in the band.

For live per-machine numbers, see `test-summary/bench/` after a run.
