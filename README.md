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

The results database has been scrubbed. When you run it, the database is rebuilt.

Performance on my machine does not equal performance on your machine. Some steps are tuned before the big runs.

Claude says there are a lot of improvements that could make this faster. That is true. This is more than fast enough for most users, so my public version is going to stop here.

[More information](docs/Overview.md)
