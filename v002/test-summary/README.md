# jac313 v002 — test-summary

This version's results world starts empty. The test runner generates this hub, the
per-`(os, compiler, build_type, disk, size)` result pages, and `jac313_results.db`
here on its first run:

```bash
cd ..            # the v002 root
./bootstrap.sh
# then drive the matrix; results + summaries are written under v002/ only.
```

v002 begins as a faithful copy of v001 and is validated for path isolation before any
ISO C++26 work begins. It does **not** inherit v001's recorded results — it produces
its own.
