# User Enhancements

> **Status: ideas, not promises.** Nothing here is planned, scheduled, or in progress. This is a
> notebook of "the framework could clearly do X" thoughts, written down so they survive past the
> conversation that produced them. If you came looking for features that exist, see
> [Architecture.md](Architecture.md) and [RunAllTests.md](RunAllTests.md) instead.

The point of the test matrix is that it's **data-driven by axis**: a run is identified by
`{os, compiler, build_type, modules, size, disk}`, and the desired set is just a cross-join in
`v_desired_matrix`. That shape means whole new dimensions of testing are *config-line* additions,
not rewrites. Below are the ones worth remembering.

---

## 1. The optimization-flag matrix 🎚️

**The pitch.** Today `build_type` is `{Debug, Release}`, and Release means one fixed set of flags.
But "Release" is a *family*, and the interesting bugs hide in the differences between its members.
Add an axis (or expand Release into variants):

| Variant | Flags | What it stresses |
|---|---|---|
| `Release` (baseline) | `-O2` | the everyday build |
| `O3` | `-O3` | aggressive inlining / vectorization |
| `Os` | `-Os` | size-tuned codegen |
| `Ofast` | `-Ofast` / `-ffast-math` | float reassociation, relaxed IEEE |
| `LTO` | `-flto` | whole-program optimization, cross-TU inlining |
| `native` | `-march=native` | ISA-specific codegen (AVX-512, etc.) |
| `noalias` | `-fno-strict-aliasing` (and its inverse) | type-punning assumptions |

**Why it earns its keep — two distinct failure classes:**

- **Build breaks.** Some flag combinations trip the *toolchain*, not your code: optimizer ICEs,
  LTO link failures, `-march=native` on a host the binary later can't run. These are the same
  flavor as the `clang-scan-deps` phantom (compiler-side, not yours) — and a matrix is exactly how you find out *which* flag set
  is the unlucky one.

- **Code breaks — the prize.** Running the *same* scenarios at multiple `-O` levels is
  **differential testing**, one of the cheapest, highest-yield ways to flush out latent
  **undefined behavior**. Code that's green at `-O0`/`-O2` and red at `-O3` is the optimizer
  *legally* exploiting UB you didn't know you had: a signed overflow it assumed couldn't happen,
  a strict-aliasing violation it punished, an uninitialized read it constant-folded into nonsense,
  a float identity `-ffast-math` reordered out from under you. **A green `-O2` and a red `-O3` on
  one scenario is a flashing sign that says "you have UB right here."** No sanitizer required to
  *notice* — though see #2 for catching it red-handed.

**How it slots in.** A CTE row in `v_desired_matrix`, a build-config entry mapping the variant to
its flags, and `RunIdentity` already carries it so results don't collide. `run-all` gap-fill and
idempotency just work. It is, genuinely, a few lines.

**The honest catch — cost.** It's a **combo multiplier.** Seven Release variants × the existing
16 combos × 116 scenarios = **~9,280 scenario-runs** per host — and *that* number is legitimate,
not a bug. Which is the whole reason the [run-all counter](RunAllTests.md) now prints `… of N`
on every line: the price is shown up front, so you choose to spend the hours with eyes open
instead of being ambushed by a four-digit total at hour two. (See the history of benchmark-matrix over-reporting for why we care so
much about that distinction.)

---

## 2. A sanitizer axis (ASan / UBSan / TSan) 🧪

The same axis trick, pointed at the question "is my code *actually* correct, or just lucky?"

- **UBSan** turns the latent UB from #1 from a mysterious `-O3` failure into a precise file:line
  diagnostic — the two ideas are natural partners (differential `-O` *finds* the smell, UBSan
  *names* it).
- **ASan** catches the heap/stack overflows and use-after-frees that the existing valgrind
  `verify-lite` gate covers more slowly.
- **TSan** complements the helgrind/DRD pass on the lock-free hot path.

`Maturity.md` lists sanitizer runs as currently out-of-scope; this is the shape they'd take if
that ever changed — a build-config axis, not a special one-off harness.

---

## 3. Why these are cheap to *add* and expensive to *run*

The recurring theme: the data-driven `RunIdentity` + `v_desired_matrix` design makes new test
dimensions nearly free to *express*, but every axis you add **multiplies** the combo count, and
each combo is a full build + 116 scenarios. The framework removed the hard part (orchestration,
gap-fill, result storage, rendering); what's left is the irreducible cost of actually compiling
and running everything N ways. The guardrail counter and the combo-level `--fail-fast` exist so
that cost is **visible and bounded** — you see the total before it grinds, and a systemic break
stops the sweep early instead of failing all N combos over hours.

---

*Again: none of the above is committed work. It's here so that "the framework could obviously do
this" doesn't evaporate the moment the terminal scrolls past it.*
