//File:    docs/TODO.md
//Purpose: Deferred cleanup / known-debt backlog. Items here are MORE than a simple fix (they
//         cascade, change behavior, or need a design decision) so they were parked, not done.

# TODO — deferred cleanup

Parked during the 2026-06-26 cleanup pass (simple, no-cascade fixes were done inline; these were not).
Each item says *why* it's deferred. Surfaced by an independent review of the libraries.

## Dead / inert code — needs care to remove
- [ ] **BinaryEventLog dead ctor params.** `int_count_`/`dbl_count_` are stored but `[[maybe_unused]]`
      (constructor params that do nothing). *Deferred:* removing them changes the constructor signature
      (cascade to call sites).
      *(The throwing-destructor risk in this file was FIXED 2026-06-26: `~BinaryEventLog()` is now
      `noexcept` and swallows+logs; `finalize()` releases the fd even when ftruncate fails, and throws
      only after cleanup so an explicit caller still learns of it. Verified textual + import-std.)*
- [ ] **Dead enum kinds (jText).** 4 declared-but-never-emitted kinds overstate the taxonomy:
      `file_error_kind::{unknown_section_state, multiline_sentinel_empty_body, unterminated_section}`
      (`section.hpp`) and `issue_kind::field_position_out_of_range` (`validator.hpp`). *Deferred:*
      removing enum values can trip exhaustive `switch`/`to_string` coverage — verify each before cutting.
- [ ] **Inert source-line tracking (jText).** `validation_issue::line_no` and `record::source_line_no`
      are hard-coded to `0` at every call site (core.hpp ≈:269 "not tracked at this layer yet", ≈:553
      "future work"). *Decision needed:* wire it through, or drop the fields. Not a simple fix.
- [ ] **Possibly-dead flags API.** `ts_store_flags.hpp` `get_severity_mask_from_index` /
      `set_severity_from_index` have no in-tree callers. Confirm they're intended public API; if not, remove.

## Duplication / cleanup — refactors (cascade across files)
- [ ] **`get_all_ids()` 0..N-1 builder duplicated** verbatim in `core.hpp`, `printing.hpp`,
      `sorting.hpp` → factor into one helper.
- [ ] **EventSink copy-paste + needless copies.** `JTextEventSink.hpp` / `BinaryEventSink.hpp` both
      copy `e.int_metrics`/`e.dbl_metrics` into locals before passing to a `const&` param, and their
      `write_batch` loops are near-identical → dedup + pass by ref.
- [ ] **printing.hpp:** throwaway `bResult1..bResult4` booleans (≈:180-195) → inline the conditions.
- [ ] **verify_checks.hpp:** the per-row success / "ONE OR MORE CORRUPTED" print sits inside the row
      loop, so a clean run prints a full success block for *every* row (millions at full size); also a
      sloppy local name `utf8_xactualx_len`.

## Low / review
- [ ] **ansi_colors.hpp:** defines `operator<<(std::ostream&, std::string_view)`, duplicating the
      standard C++17 inserter — latent ambiguity risk. Confirm removable (currently compiles).
- [ ] **Qlite `Sqlite.hpp` doc note** "Header-only friendly" vs. the actual include-fragment + module
      design — minor doc drift to reconcile.
- [ ] **CLI report pivot scaffold** (`main.cpp`) — `write_type_pages` / `write_bench_pages` /
      `write_build_pages` now each repeat the SAME host-scoping skeleton: groups query, per-host
      hardware header, group-scoped `comps` query, `inlist` / `col_for`. The duplication GREW with the
      2026-06-27 host-scoping (deficiency #1) — a shared "per-host section + column scaffold" helper
      would dedup ~15 lines × 3. Low priority; the cell logic differs per type, so it needs parameterizing.

## Not from the review
- [x] **Port `tools/build_matrix.sh` to the CLI** — DONE 2026-06-27. Folded into `jac313_test_cli
      build-times`: `.ninja_log` per-target timing, group-scoped upsert, capture-on-build hook,
      host-scoped report (per `jac313-###` with hardware), gap-filled per host, wired into the
      pre-push hook (no-op when complete). `build_matrix.sh` + the `resolve-compiler` /
      `record-build-test` shims deleted.

## Reporting deficiency list
North star: reports should *yield a conclusion*, not hand over facts to derive one from.
- [x] **#1 Hardware spec in reports** — DONE 2026-06-27. `compiler-build-times`, `bench`, AND
      `ctest`/`smoke`/`verify`/`verify-lite` are now host-scoped: one section per `jac313-###` with its
      cpu/cores/ram/os header and only that host's compiler columns. Un-conflates hosts; throughput and
      compile times are now interpretable.
- [x] **#5 build-times report 404'd on GitHub** — DONE. Was rendered into `test-summary/build/`, which
      the `**/build/` .gitignore rule (for CMake trees) swallowed; moved to `compiler-build-times/`.
- [ ] **#2 No end-of-run summary for `--bench --report`** — prints each config's tail but never rolls
      up. Need (a) a consolidated all-tests block + (b) a one-line headline verdict (a "N passed,
      M failed … *** ALL PASSED ***" rollup like the build-times gate's end-of-run summary).
- [ ] **#3 `--report` is overloaded** — standalone = render-only; WITH `--bench` it also gates whether
      data is recorded (passes `--db`). Plain `--bench` persists nothing. Same flag, two jobs.
- [ ] **#4 verify-lite pollutes `results.db`** — the pre-push valgrind gate records its run into the
      tracked DB, dirtying the tree on every push (discarded by hand each time). Forward doc says it's
      "verdict-only, no DB" — behavior drifted. Gate runs shouldn't write the results table.
- [ ] **#6 verify / verify-lite per-run detail pages render EMPTY** — `Run_NNN.md` shows the header
      with zero scenario rows even though the aggregate page references it with values.
- [ ] **#7 committed `results.db` drifts ahead of rendered pages** — gates record runs but don't
      re-render, so a commit can ship a DB with runs the pages don't show. Render should follow a DB
      write (or the gate shouldn't write — see #4).
