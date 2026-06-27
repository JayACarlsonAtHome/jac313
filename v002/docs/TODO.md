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
      `write_build_pages` each rebuild the same `comps` / `inlist` / `col_for` compiler-column setup.
      A shared "column scaffold" helper would dedup ~4 lines × 3 (low priority; the cell logic differs
      per type, so it needs parameterizing). Otherwise the report code reviewed clean.

## Not from the review
- [x] **Port `tools/build_matrix.sh` to the CLI** — DONE 2026-06-27. Folded into `jac313_test_cli
      build-times`: `.ninja_log` per-target timing, group-scoped upsert, capture-on-build hook,
      host-scoped report (per `jac313-###` with hardware), gap-filled per host, wired into the
      pre-push hook (no-op when complete). `build_matrix.sh` + the `resolve-compiler` /
      `record-build-test` shims deleted.
