# jac313 GUI packages (version-independent)

Presenter packages, the jac314 way: the engine tools do the work; a GUI builds
a command line, runs it as a child process, shows the output, and reads the
results file. No shell scripts.

The GUI lives at the project root, outside the version folders, because it serves every
version: it finds the `vNNN/` folders under the root, labels each with the C++ standard
its CMake asks for (`cxx_std_NN`), and runs that version's own `jac313_test_cli`. A new
`v003/` shows up without changing the GUI.

## Quick start: Jac313Runner

Run every step as **your own user**, not with sudo (`bootstrap.sh` asks for sudo itself
when it needs it). `<root>` is the folder that holds `v001/`, `v002/` and `gui/`.

1. **Bootstrap a version with `--gui`.** This builds `jac313_test_cli`, offers the GUI's
   build libraries (GTK3, libnotify, FreeType, Fontconfig, X11; one `[y/N]` sudo step),
   installs U++ from [upp_basefix](https://github.com/JayACarlsonAtHome/upp_basefix) into
   `$HOME/upp_basefix` (U++ pinned to a GitHub commit plus patches: 0 warnings, sanitizer and
   valgrind clean; its own bootstrap lists the system packages and installs them only on a
   `[y/N]`), and builds `gui/out/Jac313Runner` with GCC:
   ```
   cd <root>/v002 && ./bootstrap.sh
   ```
   An existing install with `umk` and `uppsrc` in `$UPP_DIR` (default `$HOME/upp_basefix`) is
   used as it is. A plain `./bootstrap.sh` never touches U++.
2. **Rebuild later:**
   ```
   cd <root>/v002 && ./jac313_test_cli gui-build              # GCC; --method CLANG for clang
   ```
   An upp_basefix install has `GCC.bm` and `CLANG.bm` next to its `umk`, naming the compilers
   by path (gcc-toolset-15 on RHEL), so no toolset shell is needed. Its patch 0001 fixes the
   GCC 15 start-up crash plain U++ 2026.1 had. If a link fails with undefined references, the
   U++ cache holds stale objects: add `--clean` (rebuild everything). Bootstrap retries once
   with `--clean` by itself.
3. **Start it** from a desktop session (Wayland or X11):
   ```
   <root>/gui/out/Jac313Runner
   ```
   Pick the version and gates, then press **Start**. Begin small: ctest + smoke, with
   Max runs 2, Max records 5000 and Max persisted 1000.

**Not as root.** Jac313Runner and `jac313_test_cli` refuse to run as root in a tree a
regular user owns. A root run there leaves root-owned build trees, logs and `/tmp` files
that your later runs cannot overwrite. Valgrind also refuses binaries that have no "other"
execute bit when it runs as root. A tree root owns (a container, CI) runs as usual.
`--allow-root` overrides, and the runner passes it on to every run it starts.

| Path | Role |
|------|------|
| `gui/<Package>/` | U++ package source |
| `gui/out/` | Built binaries, plus `Jac313Runner.json` (saved options); not in git |

## Packages

| Package | What it does |
|---------|--------------|
| **Jac313Runner** | Runs the jac313 test gates through `jac313_test_cli`, streams the run, and shows the results with a jText viewer for each result row's dataset |
| **JTextGui** | The jText table editor, vendored from jText_GUI. Open and save jText, pull PostgreSQL or SQLite and Mergesert, records of up to 999 lines, a Lines box and field box, an Excel-style Records grid (detachable to a second monitor), and Excel (.xlsx) export and import. A blank marker starts as `|#`. Guide: the jText Editor Manual in jText_GUI (`Manual/pdf/jText-Editor-Manual.pdf`). The package carries its own `jText/` library snapshot and does not compile the sibling jText checkout or `vNNN/jText`; it links SQLite, libpq and zlib. |

## Build

From either version, with no shell script:

```
cd v002 && ./jac313_test_cli gui-build      # builds <root>/gui/Jac313Runner -> <root>/gui/out/Jac313Runner
```

umk and its uppsrc come as a pair from one U++ install (the jac314 `gui_build` rule):
`$UPP_DIR`, else `$HOME/upp_basefix`, else `<GFS>/upp-bin/rhel` + `<GFS>/upp`, else `$HOME/u++`.
By hand, with the upp_basefix install:

```
U=$HOME/upp_basefix
$U/umk <root>/gui,$U/uppsrc Jac313Runner GCC -brsH <root>/gui/out/Jac313Runner
$U/umk <root>/gui,$U/uppsrc JTextGui <root>/gui/JTextGui.bm -brsH <root>/gui/out/JTextGui
```

Both packages have `options "-Wformat=2 -Werror=format"` in their `.upp`: a printf-style call
whose arguments do not match its format (such as a 64-bit value given to `%d`) fails the
build. Package options apply to our code, not to uppsrc. The jac313 CMake builds have the
same flags in every build; Jac313Runner's Build box says so ("Always on").

U++ keeps one object cache per build method (an upp_basefix install keeps it in its own
`.config`, apart from any other U++). Build with the same compiler each time. If the compiler changes, rebuild everything
once with `-a`.

### Wayland first (2026-10-16)

Every package's `mainconfig` is `"GUI WAYLAND"`: U++'s GTK backend then tries **Wayland first**
and falls back to X11 only when there is no Wayland session (U++'s default order is X11
first). Keep the flag in any new main package. A build with it lands in U++'s
`GCC.Blitz.Gui.Main.Shared.Wayland` output folder.

## Run

```
gui/out/Jac313Runner [--root R] [--summary FILE [--jtext FILE]] [--allow-root]
```

* `--root R`: the project root (the dir that holds `v001/`, `v002/`, ...). The default is
  the saved root, or else the first dir above the executable that holds a `vNNN/CMakeLists.txt`.
* `--summary FILE`: open straight on the Results screen with this summary JSON.
* `--jtext FILE`: used with `--summary`, shows this file in the viewer.
* The saved version is used when it still exists; otherwise the newest version is selected.
* `--allow-root`: run as root in a tree a regular user owns (refused by default; see Quick start).

Needs a desktop session: Wayland (`WAYLAND_DISPLAY`) first, else X11 (`DISPLAY`).

### Screens

1. **Options + Start.** Choose the version, gates, build, persist targets, max runs / max records in a run / max persisted in a run (0 = not passed),
   read-back and PG clean-up. A read-only box shows the exact command, and it updates as the
   options change. **Save as default** writes `Jac313Runner.json` beside the program.
   **Last results** opens `<root>/<version>/test-summary/runner_last*.json`.
2. **Running.** Shows a progress bar (from `[i/n]`), `pass N fail M` (from PASS / FAIL on
   progress lines), the elapsed time and a live output pane (stdout and stderr together,
   with ANSI colour codes removed). **Stop** sends SIGTERM to the child's process group and
   then SIGKILL after 3 s. The child is started through `/usr/bin/setsid`, so its pid is
   the process group id.
3. **Results.** A header line, filters (test, backend, failures only) and the result rows:
   Test | Backend | Rows | Count | Checksum | Sample | ms | Ping ms | Write ms | Status. A Compiler column appears
   when two summaries are loaded. On the right is the jText viewer with **Table | Raw**,
   a section picker when the document has more than one section, and a validation line.
   The other buttons are **Open file...** (any `.jtext`), **Show log** (the row's `log`),
   **Copy for AI**, **Show AI md**, **Back** and **Quit**.

### Copy for AI
One Markdown document for pasting into any AI ("what am I looking at?", "why did this
fail?"). It goes to the clipboard and is saved beside the summary as `runner_last*_ai.md`.
In order:
- **Project context**, read fresh from `gui/ai_context.md`. Edit that file to change what an AI
  is told about the project; the `<!-- -->` note at its top is left out.
- **What was run**: version, compiler, build type, machine, run id, times, totals, command.
- **Results**: every row, with read-back and PostgreSQL latency.
- **Failures**: status, error and the last 40 lines of each failed row's log (first 10 failures).
- **What the user had open**: the viewer's title, check line and first 80 lines.
- A few questions worth asking.

**Show AI md** shows that saved file in the viewer. It is greyed out until Copy for AI has
written the file for the results on screen, and greys out again when new results load.
If the AI report is open in the viewer when Copy for AI is pressed, the viewer closes it,
the report is rebuilt (never containing the old one) and the new one is reopened in its place.

Headless: `out/Jac313Runner --dump-ai SUMMARY [--jtext FILE]` prints the same Markdown.
`--snap DIR --summary FILE --copy-ai` also presses both buttons and pictures the result.

## Headless checks (no window, no display)

```
out/Jac313Runner --dump-command [--root R]      # the command(s) Screen 1 would run (saved/default options)
out/Jac313Runner --dump-jtext FILE              # columns=N rows=M, column names, first 3 rows (tab-separated)
out/Jac313Runner --dump-summary FILE [--root R] # row count, totals, first 3 rows + their dataset command
out/Jac313Runner --dump-ai FILE [--jtext FILE]  # the "Copy for AI" Markdown
```

Samples to check against are in `Jac313Runner/samples/`:

* `standard_dropdown.jtext`: a compact sentinel row, then per-field rows.
* `compact_rows.jtext`: compact rows, a heredoc template, and both forms of NULL (the 0x1F byte and the literal `\x1F`).
* `per_field.jtext`: per-field rows only.
* `unresolved_include.jtext`: Fields come from an include that does not resolve, so the columns are named 1..N.
* `runner_last.json`: a sample summary for testing Screen 3.

To check the layout with a display, the GUI can write PNGs of the screens and then exit:

```
out/Jac313Runner --snap DIR --summary samples/runner_last.json --jtext samples/standard_dropdown.jtext
out/Jac313Runner --root R --snap DIR --autostart [--autostop-after MS] [--select ROW]
```

The second form presses Start with the saved or default options and waits for the run to
finish, or presses Stop after MS milliseconds. It then takes the pictures and exits. Use it
only with a root whose engine is safe to run.

## Engine contract (what Jac313Runner codes against)

Working dir `<root>/<version>`, executable `<root>/<version>/jac313_test_cli`:

```
jac313_test_cli [--ctest] [--smoke] [--bench] [--verify-lite] [--verify] [--gcc|--clang] [--release] [--strict] [--hardened] [--modules]
                [--size full] --persist <list> [--max-runs N] [--max-records N] [--max-persist-rows N]
                --readback <sample|all>
                --pg-cleanup <keep-last|delete> --summary-json <path>
```

* **Connections** (Screen 1): the PostgreSQL connections file, a jText file kept OUTSIDE every
  git project. The path is editable (and `...` browses); the default is
  `<projects>/SQL_Secrets/jac313_connections.jtext`, i.e. a `SQL_Secrets` folder beside the
  project (`$JAC313_CONNECTIONS` overrides). Only that path is saved in `Jac313Runner.json`.
  **Load** fills the Remote PG fields from the file (also done at start-up); **Save** writes them
  back, keeping the file's other rows (pg-local), creating the folder (0700) and file (0600).
  The status line shows jText's own check (`jac313_test_cli connections --file P`) and turns red
  when the file is inside a git work tree. Runs pass `--connections <file>` when it exists.
* **Command that will run** (Screen 1): the exact `jac313_test_cli` command line(s) the options make.
  **Copy** puts them on the clipboard, each as `cd <version dir> && ...`, to run by hand in a terminal.
* **Remote PG** (Screen 1): Host / Port / Database / User of the user's own server (no password
  field: the password is one line `host:5432:dbname:user:password` in `jac313_pg_remote.pgpass`
  beside the connections file, mode 600; see docs/SQL_Secrets_example). PG local needs nothing here:
  bootstrap sets up its server and `jac313_pg_local.pgpass`. With PG remote
  ticked they become `--pg-remote "host=... dbname=... user=... connect_timeout=10"`; Start refuses
  PG remote without a Host. **Test** runs `jac313_store_dataset --ping pg-remote --conninfo ...` from
  the version's build and shows connect ms, ping ms and the server version, or the error.
* The `--persist` names are `sql` (SQLite), `pg-local`, `pg-remote`, `jtext`, `binary`, `json` and `html`.
* With Compiler **both**, the command runs twice, gcc15 first and then clang. The summaries go to `runner_last_gcc15.json` and `runner_last_clang.json`.
* With one compiler the summary goes to `<root>/<version>/test-summary/runner_last.json`.
* The GUI deletes the summary file before each run, so a stale file cannot pass for a new run.
* Progress lines look like `  [116/172] 006_TS | sqlite | off ... PASS (812 ms)`. Any other line just goes to the output pane.
* Summary JSON: `contract, run_id, host, version, compiler, build_type, started_utc, finished_utc, command,
  totals{pass,fail}, results[]`. Each result has `{gate, test, backend, output_mode, status, ms, rows_written,
  readback{mode,count,checksum,sample,rows_checked}, dataset{backend,base}, log, dataset_cmd[]?}`.
  A missing field shows as `-`.
* **Contract:** every summary carries `"contract": N` (written by the CLI; a summary without
  it counts as 1). This GUI understands contracts 1 to 3 (`kRunnerContract` = 3 in `Model.h`; 2 added
  `--pg-remote` / `--pg-local` and the summary's `pg_remote`, 3 added `--connections`) and refuses
  a newer one with "rebuild the GUI". Any change to the flags above or the summary fields
  bumps the contract in the CLI's `summary.cpp` and in `Model.h` together.
* Viewer data comes from `dataset_cmd` when the row has one. Otherwise it comes from
  `<root>/<version>/<build_dir>/Store/tests/matrix/jac313_store_dataset --backend B --base P --limit 1000`.
  The build dir is `build-gcc15` or `build-clang` for Debug (plus `-strict` / `-hardened`
  when the build type says so), and `build-bench-gcc15` or `build-bench-clang` for Release. The tool prints jText on stdout. Its last stderr line
  `VALIDATION errors=E warnings=W` becomes the validation line.

## jText reader (viewer only)

The reader is small and tolerant. It handles:

* Banners: `=== Section: name ===`, `=== Fields ===` / `=== End Fields ===`, `=== Data ===` / `=== End Data ===`.
  `=== <#include#> Fields: path ===` is resolved relative to the file's folder, and when it cannot be found the columns are named 1..N.
* `-- EOF --` ends the file.
* Template heredocs (`1. <<< !!!TAG!!!` ... `!!!TAG!!!`) are skipped.
* Field `N. #/# name/Type/...  # comment` gives column N the name `name`.
* Data records are separated by blank lines. `1. #|# a|b|c` is a compact row, and `N. #X# value` sets column N.
* The Unit Separator (the 0x1F byte, or the literal `\x1F`) is NULL and shows as `∅`.
