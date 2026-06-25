# Store benchmark results

_Generated from `bench_results.db` by `store_bench --report` — the curated suite_<br>
_(median + low–high band over N runs; timed **end-to-end** — store buffer allocation through flush, front edge included)._<br>
_Latest run per config, per host, per compiler._

## fedora

| | |
|---|---|
| **Hardware** | Intel(R) Core(TM) Ultra 7 265 · 12 cores · 23 GB · Fedora Linux 44 (Workstation Edition) |
| **Versions** | Store v002.004 · Qlite v002.002 · jText v002.002 |
| **Latest run (UTC)** | 2026-06-25T06:48:03Z |

### Flag-overhead (non-durable) — 10M Events × 10 Runs<br>That's 100M Events per config

<table><tr>
<td>

**clang 22.1**

| Flags | Median Ops/Sec | Band (Low–High) |
|---|--:|---|
| 6 | 6,348,787 | 6.26M – 6.40M |
| 0 | 6,347,767 | 6.29M – 6.39M |
| 4 | 6,327,631 | 6.31M – 6.38M |
| 2 | 6,281,774 | 6.09M – 6.35M |

</td>
<td>

**gcc 16.1**

| Flags | Median Ops/Sec | Band (Low–High) |
|---|--:|---|
| 0 | 5,959,689 | 5.81M – 6.08M |
| 4 | 5,927,795 | 5.81M – 6.00M |
| 6 | 5,904,765 | 5.83M – 5.99M |
| 2 | 5,848,404 | 5.76M – 5.96M |

</td>
</tr></table>

### Durable — 1M Events × 3 Runs<br>That's 3M Events per config

<table><tr>
<td>

**clang 22.1**

| Backend | Median Ops/Sec | Band (Low–High) |
|---|--:|---|
| jtext | 1,947,363 | 1.89M – 1.97M |
| binary | 557,341 | 0.54M – 0.68M |
| sql | 287,917 | 0.28M – 0.31M |

</td>
<td>

**gcc 16.1**

| Backend | Median Ops/Sec | Band (Low–High) |
|---|--:|---|
| jtext | 1,844,198 | 1.74M – 1.92M |
| binary | 604,010 | 0.60M – 0.67M |
| sql | 301,278 | 0.29M – 0.30M |

</td>
</tr></table>

### Persisted size — jText vs SQL vs binary (same across compilers)

| Backend | On-disk size |
|---|--:|
| binary | 83.8 MB |
| jtext | 83.0 MB |
| sql | 76.1 MB |

