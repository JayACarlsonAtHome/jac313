# jac313

## Why would I need this?

Robot audit logging of moments for replay. Train movements by location. Anything that needs data stored quickly.

jac313 is a Linux C++ store for those records. Each event holds the integers and floats you fixed for the job — a robot's position, a train's place, or just the fact that something happened — and the store writes them out as a binary log, jText, HTML, JSON, or SQL. v001 is the C++23 line. v002 is the C++26 line.

## How to set up a system

There is a lot in here, and it installs a lot of stuff. I recommend installing it on a Linux VM with practically nothing on it.

Everything you need is installed by running `v001/bsir.sh` or `v002/bsir.sh`, depending on which version you want. The script has to be started from inside that directory:

```bash
cd v001 && ./bsir.sh    # C++23
cd v002 && ./bsir.sh    # C++26
```

It asks once whether to install everything, and once for your sudo password. Then it installs:

- the compilers and libraries the code needs,
- a PostgreSQL server on the same machine,
- Jac313Runner, a window for running the tests,
- DBeaver, for browsing the results database,
- Google Chrome, for reading the test reports.

When it is done, the Jac313Runner window opens. Run it as your own user, not with sudo.

## Running the tests

In Jac313Runner, tick the tests you want (start with **ctest** and **smoke**) and press **Start**.

Every run is saved in the results database and ends by writing report pages. **Reports** shows them in the window. **Open Reports in Chrome** opens them as web pages that link to each other.

## Where it has been tested

This version has been tested on Fedora 44 and RHEL 9.8. Before this version it was also tested on Linux Mint and RHEL 10.2. I may do those again soon. Other Linux versions have not been tested.

## Results — what to expect

The benchmark has three goals:

1. Give you something to test in a production-like environment, so you know what to expect in your own. Its first run, the **production run**, writes a binary replay log, a keeper file (jText, HTML or JSON) and a database (SQLite or PostgreSQL) at the same time, each event going only where its flags send it — a truer representation of how a production machine would actually be running. You pick the keeper file, the database and how many events go to each in Jac313Runner (or in `tests/production.jtext`; one row serves the smoke, full and bench profiles).
2. Be the framework for timing future contracts, and how the four contract modes (enforce, observe, quick_enforce, ignore) affect your environment.
3. Give compiler builders and CMake a good, easy way to benchmark their products: `./jac313_test_cli --bench`.

The production run reports three numbers: how fast your program can log (the hot path), how long each sink takes to get everything to disk (its drain time), and the whole run from start to exit. This is new; before, the bench only timed one sink at a time, which is not how a real program runs. See [Benchmarks](v002/docs/Benchmarks.md).

The results database has been scrubbed. When you run it, the database is rebuilt.

Performance on my machine does not equal performance on your machine. Some steps are tuned before the big runs.

Claude says there are a lot of improvements that could make this faster. That is true. This is more than fast enough for most users, so my public version is going to stop here.

[More information](docs/Overview.md)
