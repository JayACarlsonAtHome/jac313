# Conformance watch-list & baseline

The C++23/26 features this project depends on, and where to re-check them. **We run this by
hand, together, when you're online — no automation, no CI, no GitHub Actions.** To check:
Claude fetches the source URLs below, diffs the live status against this baseline, reports
anything that changed, then we update this file (and note it in `Dev Notification.md`).

> cppreference blocks bots (HTTP 403), so we pull from the upstream **vendor** pages it
> aggregates — those are authoritative anyway.

_Baseline captured: 2026-06-26. GCC/libstdc++ rows marked ~ are from general knowledge and
should be verified on the first real run (the libstdc++ status page didn't parse cleanly)._

## Features we track

| feature | paper | clang / libc++ | gcc / libstdc++ |
|---|---|---|---|
| `import std` (std module) | P2465R3 | **libc++ 19 — Complete** | libstdc++ 15 (the g++-15 pilot) |
| C++ modules (language)    | —       | **Clang 15** | gcc — supported (~11+) |
| `std::print` / `println`  | P2093   | **libc++ 18 — Complete** | libstdc++ ~g++ 14 |
| `std::expected`           | P0323   | **libc++ 16 — Complete** | libstdc++ ~g++ 12 |

## The one we're actually waiting on
**clang + libstdc++ `import std` (recorded in build-times).** v002 CMake now validates
`import std;` on **clang-21 + libstdc++** (reusing GCC's std-module BMI) — see
[Modules.md](../../docs/Modules.md). The **build-times matrix** still records `gcc·istd` only;
`clang·istd = NA` there means "not yet recorded", not "unsupported". **libc++'s own `std`
module** for a native clang+libc++ path remains the standing watch item.

## Sources (fetch these to re-check)
Language and library are **separate pages** — the compiler/language pages do NOT list library
features (`import std`, `std::print`, `std::expected`), so those come from the library pages.

Language (the compiler):
- Clang:  https://clang.llvm.org/cxx_status.html
- GCC:    https://gcc.gnu.org/projects/cxx-status.html

Library (where import std / print / expected actually live):
- libc++ C++23:  https://libcxx.llvm.org/Status/Cxx23.html
- libc++ C++26:  https://libcxx.llvm.org/Status/Cxx26.html
- libstdc++:     https://gcc.gnu.org/onlinedocs/libstdc++/manual/status.html

(cppreference's own pages have both a language and a library section, but it 403s bots — the
vendor pages above are what it aggregates.)

## How to run it
When you're online, say **"check conformance"** — Claude fetches the sources above, diffs each
tracked feature against this baseline, flags any change (e.g. *clang + libstdc++ import std
went from NA → shipped*), updates this file, and adds a `Dev Notification.md` entry.
