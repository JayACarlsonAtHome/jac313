//File:    docs/FutureTestAxes.md
//Purpose: Framework-DEV backlog — test-matrix axes we could add. Ideas, not commitments.

# Future test axes (framework backlog)

> **Audience: framework hackers, not library users.** If you just want to *use* jText / Store /
> Qlite, this is not for you — see [ForUsers.md](ForUsers.md). This is a notebook of test
> *dimensions* the harness could grow, kept so the ideas survive past the conversation that produced
> them. Nothing here is planned or scheduled.

The matrix is **data-driven by axis** — a run is `{os, compiler, build_type, modules, size, disk}`,
and the desired set is a cross-join. So a whole new dimension of testing is usually a config-line
addition, not a rewrite. The catch is always the same: cheap to *add*, expensive to *run* (each new
axis **multiplies** the combo count, and each combo is a full build + the scenario set). That's why
the run counter prints `… of N` up front — the price is visible before you spend the hours.

## 1. Optimization-flag matrix — differential UB testing
Expand `Release` from one fixed flag set into variants (`-O2` / `-O3` / `-Os` / `-Ofast` / `-flto` /
`-march=native` / aliasing on-off). Two payoffs:
- **Build breaks** — some flag combos trip the *toolchain* (optimizer ICEs, LTO link fails,
  `-march=native` on a host the binary can't run). A matrix finds which set is unlucky.
- **Code breaks (the prize)** — running the same scenarios at multiple `-O` levels is **differential
  testing** for latent **undefined behavior**. Green at `-O2`, red at `-O3` = the optimizer legally
  exploiting UB (signed overflow, strict-aliasing, uninitialized reads, `-ffast-math` reordering).
  A green/red split on one scenario is a flashing "you have UB right here."

## 2. Sanitizer axis — ASan / UBSan / TSan
The same axis trick aimed at "correct, or just lucky?". **UBSan** turns a mysterious `-O3` failure
from #1 into a file:line diagnostic; **ASan** catches heap/stack/use-after-free faster than valgrind;
**TSan** complements the helgrind/DRD pass on the lock-free path. It would be a build-config axis,
not a one-off harness.
