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
- Durable runs time the **whole process** (fork→exit), so each backend's finalize —
  draining its writer and **syncing bytes to disk** (`fsync`/`msync`) — lands inside the clock.
- Verify is **last-run-only**, so checking correctness never taxes the timed
  runs.

## How to run

The easy way, from `v001/` — the runner builds Release and drives the suite for you:

```bash
./jac313_test_cli --bench            # run the curated suite — numbers to stdout, NOT recorded
./jac313_test_cli --bench --report   # record → test-summary/results.db + render the report
```

`--bench --report` auto-resolves the machine label (see *group identity* below) — no manual
`host_label.local`. Everything underneath is `store_bench`; drive it directly when you want one
config, `--dry-run`, or `--anonymize`.

`store_bench` *is* the suite — no driver script. From the build directory
(where `jac313_store_bench` was built):

```bash
./jac313_store_bench --suite --dry-run         # print the copy-paste command list
./jac313_store_bench --suite --db results.db   # run the curated 10 and record each
./jac313_test_cli --report                     # render the comparison pages from results.db
```

`--db <path>` appends one row per config to a SQLite database (written via
Qlite) — versions, config, and the full distribution — so re-running
**accumulates history**. Every row carries a **`group_id`**: one per distinct
**hardware + OS** identity `(cpu, cores, ram_gb, os)` — the container for a machine
running an OS (externally it renders as **`Run_NNN`**). A re-run on the same
machine reuses its group; a new machine, or a new OS on the same box, gets the next group.

**Group identity / host label (auto).** The group is resolved by a plain compare:
this machine's `(cpu, cores, ram_gb, os)` against the recorded groups — same
hardware+OS reuses its group, anything new gets the next number. The recorded
`host` is the anonymized **`jac313-<group_id>`**, so no real hostname ever enters
the DB and no manual label step is needed. A `host_label.local` /
`$JAC313_HOST_LABEL` override still pins a specific label. Use
`jac313_test_cli --group-id` to preview the existing groups and which one this
machine resolves to before recording (read-only).

`jac313_test_cli --report` reads `results.db` back and **writes files** (no stdout redirect): a
top-level **`test-summary/README.md`** index linking to per-area **compiler-comparison** pages —
`compiler/`, `ctest/`, `smoke/`, `bench/`, `verify/`, `build/`. The **bench** page is head-to-head
per compiler — median ops/sec · low–high band · output size, clang ↔ gcc side by side, numbers
locale-formatted. Re-run `--report` any time to refresh the pages. Pass `--jtext-ver vX.Y` on the
`store_bench --suite` call so the recorded row carries the jText version. Each config is also a
standalone `store_bench` command (see below) for running one in isolation.

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
Host:     Intel Core Ultra 7 265 · 20 cores · 62 GB · RHEL 9.8 · Release · threads=50
Versions: Store v001.005 · Qlite v001.006 · jText v001.005
```

Flag-overhead (non-durable) — 10M Events × 10 Runs = 100M Events per config (median | low–high band):

| Flags | Median Ops/Sec | Band — Low–High (Ops/Sec) |
|-------|----------------|---------------------------|
| 0     | 12,638,618     | 11.29M – 15.57M           |
| 2     | 11,370,100     | 10.50M – 15.72M           |
| 4     | 11,620,606     | 11.13M – 15.97M           |
| 6     | 15,043,162     | 11.32M – 15.93M           |

Durable — 1M Events × 3 Runs = 3M Events per config (median | low–high band):

| Backend | Median Ops/Sec | Band — Low–High (Ops/Sec) |
|---------|----------------|---------------------------|
| binary  | 2,568,970      | 2.57M – 2.60M             |
| jText   | 2,053,047      | 2.02M – 2.09M             |
| SQL     | 1,492,332      | 1.48M – 1.51M             |

_Representative (jac313-004, gcc15, 3 runs; each backend fsync'd to disk inside the clock — the
jText/SQL order is disk-dependent). Live per-machine numbers are in the rendered
`test-summary/bench/` report and refresh on each `--run-everything`._

## Takeaways

**Flags are free.** Going 0→6 flags is flat within ~2%, and the deltas don't
even trend — it's pure noise.<br>
Setting flags has no measurable hot-path cost.

**Durable ranking (this box): binary (~2.6M) > jText (~2.0M) > SQL (~1.5M).** A durable run
times the whole process (fork→exit) with each backend's flush-to-disk inside the clock, so
every number counts real durability: binary `msync(MS_SYNC)`s its mmap at finalize, jText
`fsync`s each of its three files, and SQL commits transactionally through SQLite. Binary wins
as the most compact path (a packed `mmap` append; async `msync` per batch, one blocking `msync`
at finalize). The jText and SQL numbers are closer and depend on the machine's disk —
text-formatting-plus-three-`fsync`s vs SQLite's per-commit sync — so treat their order as
hardware-dependent, not fixed (a slower-disk representative measured them the other way around).
Read the **median + band**: a one-off fsync stall lands in the band, where median+band beats both
the average (which the stall drags down) and "fastest" (which hides it).

For what each number means, see the Quick Start in this README or the live reports in `test-summary/bench/`.
