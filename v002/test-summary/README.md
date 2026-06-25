# Store benchmark results

_Generated from `bench_results.db` by `store_bench --report` — the curated suite_<br>
_(median + low–high band over N runs; durable rates count the flush)._<br>
_Latest run per config, per host, per compiler._

## fedora

| | |
|---|---|
| **Hardware** | Intel(R) Core(TM) Ultra 7 265 · 12 cores · 23 GB · Fedora Linux 44 (Workstation Edition) |
| **Versions** | Store v002.004 · Qlite v002.002 · jText v002.002 |
| **Latest run (UTC)** | 2026-06-25T06:38:18Z |

### Flag-overhead (non-durable) — 10M Events × 10 Runs<br>That's 100M Events per config

<table><tr>
<td>

**clang 22.1**

| Flags | Median Ops/Sec | Band (Low–High) |
|---|--:|---|
| 0 | 16,713,294 | 16.42M – 17.25M |
| 2 | 16,759,969 | 16.37M – 17.00M |
| 4 | 16,980,490 | 16.23M – 17.20M |
| 6 | 16,392,153 | 15.77M – 17.21M |

</td>
<td>

**gcc 16.1**

| Flags | Median Ops/Sec | Band (Low–High) |
|---|--:|---|
| 0 | 13,790,231 | 13.58M – 13.96M |
| 2 | 13,752,616 | 13.71M – 13.92M |
| 4 | 13,722,773 | 13.65M – 13.79M |
| 6 | 13,773,645 | 13.69M – 13.83M |

</td>
</tr></table>

### Durable — 1M Events × 3 Runs<br>That's 3M Events per config

<table><tr>
<td>

**clang 22.1**

| Backend | Median Ops/Sec | Band (Low–High) |
|---|--:|---|
| jtext | 2,210,448 | 2.20M – 2.36M |
| sql | 319,451 | 0.30M – 0.36M |
| binary | 602,833 | 0.50M – 0.73M |

</td>
<td>

**gcc 16.1**

| Backend | Median Ops/Sec | Band (Low–High) |
|---|--:|---|
| jtext | 2,417,263 | 2.28M – 2.46M |
| sql | 334,912 | 0.33M – 0.34M |
| binary | 714,753 | 0.57M – 0.75M |

</td>
</tr></table>

### Persisted size — jText vs SQL vs binary (same across compilers)

| Backend | On-disk size |
|---|--:|
| jtext | 83.0 MB |
| sql | 76.1 MB |
| binary | 83.8 MB |

