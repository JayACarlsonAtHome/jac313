<!-- Project context for AI readers. Jac313Runner's "Copy for AI" puts this file at the top
     of every report it makes. Edit it freely: it is plain Markdown, and the GUI reads it
     fresh each time (gui/ai_context.md under the project root). Keep secrets out of it. -->

### What jac313 is
jac313 is a generic C++ base library that people copy and customize. These tests check its
pieces on real compilers:
- **Store**: a time-series event logger. Many threads record events (an id, a thread id,
  flags, a category, a text payload, a timestamp, and some integer and decimal metrics) into
  memory, and a "sink" can save every event to disk or a database.
- **Qlite**: a thin wrapper over SQLite.
- **jText**: a plain-text table format. Every row starts with a field number, and field 1 is
  always the record id. The result viewer shows data in jText.

Each version folder is the same library at a different C++ standard: **v001 is C++23** and
**v002 is C++26**. Each version is built with two compilers, **gcc 15** and **clang 21**, and
the two must agree.

### The test gates
- **ctest**: unit tests, one program each. PASS or FAIL.
- **smoke**: the Store test matrix. Each test runs with each storage type ("backend"), with
  console output on and off, and checks the stored data afterwards.
- **bench**: speed in events per second. The long-term goal is about 50 million records per
  second; current numbers are well below that, so a low figure is not a test failure.
- **verify-lite / verify**: the same programs under valgrind, which looks for memory errors
  and thread races. "clean" means none were found.

### Storage types (backends)
- **binary**: a compact file in the library's own format.
- **jtext**, **json**, **html**: text files.
- **sql**: an SQLite database file.
- **pg-local**: a PostgreSQL server on the same machine.
- **pg-remote**: the user's own PostgreSQL server on another machine (they choose it; the
  report's "Remote PostgreSQL" line names it). It exists to show the cost of the network: its
  "Ping ms" is the round trip, typically about 100 times the local one on a LAN, more over the
  internet.
- **none / flags / unit**: tests that keep events only in memory or check one feature.

### What the read-back columns mean
After a test saves its events, read-back opens what was saved and compares it with what the
program recorded:
- **Count**: the right number of rows came back, each id exactly once.
- **Checksum**: the totals of every column match, so no value changed.
- **Sample**: chosen rows match field by field ("all" mode compares every row).
- **Rows**: how many rows were saved. Runs are often limited ("Max persisted in a run") to keep
  them small; that is deliberate, not data loss.

### Limits and build options you may see
- **Max runs / Max records / Max persisted**: caps that keep test runs small and spare the disks.
- **Debug-strict**: built with extra compiler warnings. Warnings are expected and are not failures.
- **Debug-hardened**: built with extra runtime safety checks (bounds checks, zeroed variables).
- **[capped]** in bench: a speed run with a row cap; not comparable with full-size runs.

### How to read a result
- PASS on every row means the library built, ran and stored its data correctly.
- A FAIL row says which test and storage type failed; its log (included below when present)
  says why.
- Timing columns (ms, Ping ms, Write ms) describe this machine on this day; compare them
  between rows, not with other machines.
