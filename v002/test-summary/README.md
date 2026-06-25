# Store benchmark results

_Generated from `bench_results.db` by `store_bench --report` — the curated suite_<br>
_(median + low–high band over N runs; durable rates count the flush)._<br>
_Latest run per config per host._

## fedora

| | |
|---|---|
| **Hardware** | Intel(R) Core(TM) Ultra 7 265 · 12 cores · 23 GB · Fedora Linux 44 (Workstation Edition) |
| **Versions** | Store v002.004 · Qlite v002.002 · jText v002.002 |
| **Latest run (UTC)** | 2026-06-25T05:16:45Z |

### Flag-overhead (non-durable) — 10M Events × 10 Runs<br>That's 100M Events per config

| Flags | Median Ops/Sec | Band — Low–High (Ops/Sec) |
|-------|----------------|---------------------------|
| 0 | 22,113,571 | 21.47M – 23.08M |
| 2 | 22,036,357 | 21.55M – 22.36M |
| 4 | 21,767,960 | 21.26M – 22.13M |
| 6 | 22,201,619 | 21.42M – 22.65M |

### Durable — 1M Events × 3 Runs<br>That's 3M Events per config

| Backend | Median Ops/Sec | Band — Low–High (Ops/Sec) |
|---------|----------------|---------------------------|
| jtext | 2,255,453 | 2.11M – 2.32M |
| sql | 1,908,419 | 0.35M – 2.04M |
| binary | 634,895 | 0.62M – 0.67M |

