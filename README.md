# jac313

jac313 is a fast event logger for Linux, written in C++, and the test and benchmark harness
built around it.

## What it is for

Anything that has to record a lot of small events quickly and keep them: a robot's position
for replay, a train's location as it moves, an audit log of what happened and when.

The logger is called **Store**. Each event carries exactly the integers and floats you
choose for the job (a robot's x, y, z; a train's position; or nothing at all, just the
fact that something happened), fixed at compile time, so logging an event allocates
nothing. Store writes the events to one or more places at once: a binary log for replay,
jText, HTML, JSON, SQLite or PostgreSQL. Each event's flags decide where it goes.

Store is built on two small libraries that live in this repository too:

- **Qlite**: a SQLite wrapper (Store's SQL output).
- **jText**: a human-readable, self-describing text format (Store's text logs).

## Two versions, side by side

The repository holds two complete copies, one per C++ standard. They are kept apart on
purpose, so a change to one never needs the other to be tested.

| Version | Standard | Contracts |
|---------|----------|-----------|
| [v001](v001/README.md) | C++23 | **Experimental, non-standard contracts that need nothing but C++23**: no compiler contract support (`-fcontracts`, `pre`/`post`) is used. See [Contracts in v001](v001/docs/Contracts.md). |
| [v002](v002/README.md) | C++26 | The C++26 standard contracts (`pre`, `post`, `contract_assert`) where the compiler has them (GCC 16), and plain run-time checks where it does not. |

### Contracts in v001

v001 is where we try contracts written as ordinary C++23 library code, so they can later
be compared with v002's standard C++26 contracts. They come from jac321, a separate
experiment, and are copied into Qlite. Today they guard Qlite's column and bind numbers:
a column number is its own type, checked once when it is made, so reading a column a row
does not have stops the program instead of quietly returning nothing.

```cpp
namespace jc = jac313::Qlite::v001::jac321;

template <class Policy = jc::Abort>          // what a broken contract does, picked at compile time
double percent_of(double part, double whole) {
    jc::check<Policy>(jc::Kind::pre, [&] { return whole > 0; }, "whole > 0");
    return 100.0 * part / whole;
}

st.get(a, b, c);   // 3 values from a 2-column row:
                   // Qlite contract precondition violated: (count >= 3) ... Aborted
```

The full sample, how Qlite uses them and how to turn them off are in
[v001/docs/Contracts.md](v001/docs/Contracts.md). More will be added to v001 as the
experiments prove themselves.

## How to set up a system

There is a lot in here, and it installs a lot of software. I recommend a Linux VM with
practically nothing else on it.

Everything is installed by `bsir.sh` in the version you want. Start it from inside that
directory:

```bash
cd v001 && ./bsir.sh    # C++23
cd v002 && ./bsir.sh    # C++26
```

It asks once whether to install everything, and once for your sudo password. Then it
updates the system and installs:

- the compilers, CMake, valgrind and the libraries the code needs,
- a PostgreSQL server on the same machine (for the PostgreSQL tests),
- **Jac313Runner**, a window for running the tests, and **JTextGui**, a table editor for
  jText files and databases,
- DBeaver, for browsing the results database,
- Google Chrome, for reading the test reports.

When it is done, the Jac313Runner window opens. Run everything as your own user, not with
sudo. `./bsir.sh --no-gui` leaves out the windows (for a server with no screen).

Database passwords live in a `SQL_Secrets` folder beside the repository, never in git.

## Running the tests

In Jac313Runner, tick the tests you want (start with **ctest** and **smoke**) and press
**Start**. From a terminal, inside `v001/` or `v002/`:

```bash
./jac313_test_cli --ctest            # the smallest check
./jac313_test_cli --smoke            # the everyday test
./jac313_test_cli --run-everything   # everything, on both compilers (about 35 minutes)
```

`--run-everything` first clears this machine's earlier results for that version, then
calibrates the batch sizes for this machine, then runs every test, the benchmarks and the
valgrind checks with both GCC and Clang. A machine that passes all of it is marked
**PRESUMED SAFE**: presumed, not proven.

Every run is saved in the results database (`test-summary/results.db`) and ends by writing
report pages. **Reports** shows them in the window; **Open Reports in Chrome** opens them
as linked web pages.

Every compiler warning is treated as an error, in every build.

## Machine numbers

Each machine is recorded as `jac313-001`, `jac313-002`, and so on. Its identity is its
hardware (CPU, cores, RAM, disk type) plus its hostname; the operating system is recorded
with each run, so a machine that boots two systems stays one machine. When several
machines share a PostgreSQL server, the server's registry hands out the numbers, so no two
machines both call themselves `jac313-001`.

You normally never pick a number. A machine that cannot be told apart from one already
recorded (the same hardware under another hostname), or one that was numbered offline and
clashes once the server is reachable, gets the next free number by itself. To choose one
yourself:

- `--assign-new-###` files this machine under that number, in the registry too. It is
  refused, and nothing changes, if another machine holds that number or the server cannot
  be reached.
- `--claim jac313-###` binds this machine to a number already recorded for the same
  hardware.

## Where it has been tested

Fedora 44: the full `--run-everything` passed for both versions from a fresh clone
(2026-11-10). RHEL 9.8, RHEL 10.2 and Linux Mint passed earlier versions and are being run
again. Other Linux systems have not been tested. It does not run on macOS or Windows.

## Results: what to expect

The benchmark has three goals:

1. **A production-like test**, so you know what to expect on your own machine. Its first
   run, the **production run**, writes a binary replay log, a keeper file (jText, HTML or
   JSON) and a database (SQLite or PostgreSQL) at the same time, each event going only
   where its flags send it, the way a real program runs. You pick the keeper file, the
   database and how many events go to each in Jac313Runner (or in `tests/production.jtext`).
2. **A framework for timing contracts**: what they cost, compared across the contract modes
   and between v001's library contracts and v002's standard ones.
3. **An easy benchmark for compiler and CMake builders**: `./jac313_test_cli --bench`.

The production run reports three numbers: how fast your program can log (the hot path),
how long each output takes to get everything to disk, and the whole run from start to exit.
See [Benchmarks](v002/docs/Benchmarks.md).

The results database starts empty; your runs fill it. Performance on my machine does not
equal performance on your machine, which is why the batch sizes are calibrated on each
machine before the big runs.

There are many improvements that could make it faster still. It is already more than fast
enough for most uses, so the public version stops here.

[More information](docs/Overview.md)
