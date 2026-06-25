# Memory & concurrency validation (valgrind)

A record of a focused validation pass over the Store hot path and persistence sinks using
**valgrind** — memcheck for memory safety, and helgrind + DRD for data races. The
[main README](../README.md) is the overview; this documents what was run, what it found,
what was fixed, and — honestly — what it does **not** cover. Like the rest of jac313, this
is AI-assisted work; the findings below were verified by running the tools, not asserted.

---

## Why

Store advertises a **lock-free in-memory hot path** and a set of pluggable persistence
sinks (binary, jText, SQL, flag-routing). Both are concurrency-shaped code — exactly where
memory and threading bugs hide and where unit tests rarely catch them. memcheck answers
"is every byte allocated, freed, and accessed legally?"; helgrind and DRD answer "do the
threads coordinate correctly?" — two different questions, two different tools.

## Method

- **valgrind 3.22.0**, gcc-15 **Debug** binaries (symbols, no optimization — the right
  target for these tools).
- Binaries were driven **directly** with a `/tmp` working directory so nothing in the repo
  was touched — not the tracked results DB (`test-summary/bench_results.db`), not the
  committed `persist_*` sample artifacts at the repo root.
- The matrix tests are the real concurrency surface (each spins multiple writer threads);
  they were exercised on both the **lock-free path** (`--persist=none`) and the
  **mutex+condvar persistence path** (`--persist=binary|jtext|sql`).

```bash
# one-time
sudo apt-get install -y valgrind

# memcheck a binary (cwd = scratch dir, so persist output stays out of the repo)
cd /tmp/scratch
valgrind --tool=memcheck --leak-check=full --track-origins=yes \
  <build>/Store/tests/matrix/jac313_store_001_TS --no-interactive --test-size=smoke --persist=binary --base-name=/tmp/scratch/m

# helgrind / DRD
valgrind --tool=helgrind <bin> --no-interactive --test-size=smoke --persist=none --base-name=/tmp/scratch/h
valgrind --tool=drd      <bin> --no-interactive --test-size=smoke --persist=none --base-name=/tmp/scratch/d
```

## memcheck: clean

22 binaries — the two unit tests, the six example smokes (incl. `binary_to_jtext`, which
exercises the `mmap` `BinaryEventLogReader`), all eight matrix tests on the binary sink,
matrix 001 across jText/SQL/none, plus `flags` and `metric_view`:

> **22/22 clean — 0 errors, 0 leaks.**

Verified to be real instrumentation, not no-ops: e.g. `binary_to_jtext` ran 129 allocs /
129 frees ("all blocks freed"); a matrix test ran 5 threads × 20 events with payloads
validated. The memory surface — multithreaded hot path, binary sink + `mmap` reader, jText
sink, SQL/SQLite sink, flag routing, metric view — is memcheck-clean at smoke scale.

## The lock-free boundary (an architectural finding)

The whole Store has **exactly one mutex + condition variable**, both in
`DoubleBufferedWriter`. The in-memory core never locks:

| Path | Synchronization | Lock-free? |
|------|-----------------|------------|
| In-memory only (`--persist=none`) | atomics only — `fetch_add` id, atomic epoch, unique-slot write | **Yes** |
| With persistence | every event calls `submit_event()` → takes `buffer_mutex_` | **No** — one mutex acquisition per event |

So "lock-free" holds for the in-memory store; persistence bolts a mutex-guarded
double-buffered writer onto the producer side. This is a reasonable design — the lock is
held only to append into a buffer (microseconds), while a background worker does the slow
file/SQL I/O outside the lock. The trade-off is that producer threads contend on one mutex
per event, which serializes submission at high thread counts versus a truly lock-free queue.

## Findings

helgrind and DRD **agreed on every result** (DRD words it "Conflicting load", helgrind
"data race"; same addresses). Two issues surfaced — and notably, the lock-free path was
clean on 001/002/004/005/007, so this was not a blanket "tools can't see atomics" artifact.

### 1. A real data race in the tail-reader tests (003, 006) — **fixed**

Tests 003/006 run a reader thread that calls `store.select()` concurrently with writers (a
streaming read-while-write pattern). 200 errors from 2 contexts, confirmed by both tools.

Root cause was in the **test harness**, not the store core. Writers reserve a slot, fill
it, then publish a completion count:

```cpp
size_t pos = log_stream_write_pos.fetch_add(1, relaxed);  // reserve
log_stream_array[pos] = id;                               // fill   (race)
total_written.fetch_add(1, release);                      // publish
```

…but the reader gated its readable region on `log_stream_write_pos` — the **reservation**
counter, bumped *before* the slot is filled — so it read array slots (and fed their ids
into `store.select()`) before the writer had written them. A genuine torn read, plus a
downstream read of an in-flight store row.

**Fix — per-slot atomic publication.** `log_stream_array` is now
`std::array<std::atomic<size_t>>`; the writer release-stores `id + 1` (0 = not yet
published), and the reader acquire-loads, stopping at the first empty slot. Each slot's
write is now paired with its read, and that release/acquire transitively orders the
writer's `save_event` before the reader's `select()` — so no in-flight row is ever read.

This also confirmed a contract: **`store.select()` has no internal synchronization** between
a slot's write and its read — it is only safe on *completed* ids. The store is fine; the
test was violating that contract.

### 2. A benign "dubious" warning in the writer shutdown — **cleaned up**

`DoubleBufferedWriter::stop()` called `cv_.notify_one()` *after* releasing the mutex. The
predicate (`stop_requested_`) is set under the lock and the worker re-checks it under the
lock, so this was the correct, idiomatic notify-without-lock pattern — no lost wakeup. But
the thread checkers flag it ("dubious: associated lock not held") because they can't prove
the predicate was published under lock. Moved the `notify_one()` inside the lock scope —
silences the warning, no behavior change.

## Why a residual flag remains (and the annotation)

Neither helgrind nor DRD models **C++11 `std::atomic` acquire/release as happens-before** —
they only track pthread primitives. So after the fix above, the (now correct) lock-free
producer→consumer handoff still drew a spurious race on the payload row: the tools see the
`save_event` write and the `select()` read of the same row with only atomic synchronization
between them, which they ignore.

The handoff is annotated with valgrind's `ANNOTATE_HAPPENS_BEFORE` / `ANNOTATE_HAPPENS_AFTER`
on the atomic slot, gated behind the **`JAC313_STORE_HELGRIND_ANNOTATE`** CMake option
(default **OFF**). Default builds carry no valgrind header dependency — the macros fall back
to no-ops. Build a separate tree with the option ON to teach the tools the edge:

```bash
cmake -G Ninja -S . -B build-hg -DCMAKE_CXX_COMPILER=g++-15 \
  -DJAC313_BUILD_STORE_TESTS=ON -DJAC313_STORE_HELGRIND_ANNOTATE=ON
cmake --build build-hg --target jac313_store_003_TS jac313_store_006_TS
valgrind --tool=helgrind build-hg/Store/tests/matrix/jac313_store_003_TS --no-interactive --test-size=smoke --persist=none
```

## Results

| Subject | Before | After |
|---------|--------|-------|
| memcheck (22 binaries) | clean | clean |
| 003 / 006 (helgrind & DRD) | 200 err, 2 contexts | **0** (code fix + annotation) |
| Writer dubious-notify (008 + binary/jText/SQL) | 1 err each | **0** |
| Lock-free path 001/002/004/005/007 | 0 | 0 |
| `ctest` (gcc-15 Debug) | 30/30 | **30/30** |
| 003/006 functional (native) | 100 hits / 0 misses | 100 hits / 0 misses |

The real data race is fixed and verified; the cosmetic dubious-notify is fixed; and the
tool blind-spot on lock-free atomics is annotated away. (Commit `ee0058c`.)

## The gate (`cleancheck`)

Having found a real race, the rule is: re-run these tools before every push. That's a
`jac313_test_cli` subcommand and an installed pre-push hook — not a manual ritual to
remember.

```bash
jac313_test_cli cleancheck            # auto-detect compiler
jac313_test_cli cleancheck --gcc15    # pick a toolchain
```

`cleancheck` self-builds a dedicated **annotated** Debug tree
(`-DJAC313_STORE_HELGRIND_ANNOTATE=ON`, so helgrind/DRD see the lock-free handoff), then
runs the representative set at **smoke scale**: memcheck over the full memory surface, and
helgrind + DRD over the multithreaded tests (003/006 lock-free + 001 across the
persistence sinks). It runs each binary from a `/tmp` scratch dir — nothing in the repo is
touched, no results DB, no committed artifact. Output is verdict-first: a per-check
clean/error line, a `CLEANCHECK PASS`/`FAIL` summary, and a **non-zero exit on any error**.
If valgrind isn't installed it stops with install instructions (it's a hard prerequisite).

**Enforcement.** `bootstrap.sh` installs `tools/hooks/pre-push` into `.git/hooks/pre-push`
(git hooks are local and not version-controlled, so they're installed per clone). It runs
`cleancheck` before every push and blocks a dirty one. Bypass a single push with
`git push --no-verify`.

## Honest limits — what this does **not** cover

- **Smoke scale only** (e.g. 5 threads × 20 events). Scale-dependent races — ring-buffer
  wraparound, buffer growth, persist spillover — would not necessarily surface here. A
  full-matrix run under these tools would be far slower but more thorough.
- **gcc-15 Debug only** — no clang, no Release. Optimized builds can reorder in ways Debug
  does not.
- **A representative subset**, not every test × every sink combination. Every *code path*
  (hot path, each sink, each reader) was hit; the full combinatorial grid was not.
- **The gate is smoke + representative, by design.** `cleancheck` (above) runs before every
  push, but at smoke scale and over a representative subset — not the full matrix, not every
  test × sink. It's a fast tripwire, not an exhaustive sweep; a deeper periodic run (full
  scale, more scenarios) is still a manual exercise.
- **Local-only enforcement.** The pre-push hook is local and bypassable (`--no-verify`);
  there's no server-side check (no CI, by project choice).
- The annotations make the **tools** agree with the C++ memory model; they are not a
  substitute for the model. The correctness argument is the release/acquire pairing, not
  the annotation.

Part of the [jac313](../README.md) monorepo.
