//File:    docs/TODO.md
//Purpose: Deferred cleanup / known-debt backlog. Items here are MORE than a simple fix (they
//         cascade, change behavior, or need a design decision) so they were parked, not done.

# TODO — deferred cleanup

Parked during the 2026-06-26 cleanup pass (simple, no-cascade fixes were done inline; these were not).
Each item says *why* it's deferred. Surfaced by an independent review of the libraries.

## Correctness — address first
- [ ] **Throwing destructor.** `Store/.../persistence/BinaryEventLog.hpp` — `finalize()` throws
      `std::runtime_error` on `ftruncate` failure and is called from `~BinaryEventLog()` (≈:98), so a
      flush failure during destruction risks `std::terminate`. *Deferred:* the fix is a design choice
      (make the dtor `noexcept` and log/swallow, vs. require an explicit `close()` that may throw).
      `int_count_`/`dbl_count_` are also stored but `[[maybe_unused]]` (constructor params that do nothing).

## Dead / inert code — needs care to remove
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

## Not from the review
- [ ] **Port `tools/build_matrix.sh` to the CLI** (code over shell, per project preference) — would let
      it resolve compilers via `compilers.conf` instead of env-overridable hardcoded paths.
