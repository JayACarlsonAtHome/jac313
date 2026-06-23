# jac313 test-summary hub

> **Reading the numbers? Start with [How to read the results](../docs/HowToReadResults.md).** "Peak ops/sec" is the in-memory hot path, *not* a persist backend's write speed (in-memory ~15–25M; durable ~2.4–2.7M).

Run index and per-run pages from [`jac313_results.db`](jac313_results.db); log tails (last 50 lines) published at matrix run from captured stdout.
Pipeline docs: [SETUP → Testing](../docs/Setup.md#4-testing) (`release-check`, matrix runs, `--filter`).

<table>
<thead>
<tr><th>OS</th><th>Compiler</th><th>Build</th><th>Modules</th><th>Size</th><th>Disk</th><th>Group</th><th>Run UTC</th><th>Duration</th><th>Build (modules)</th><th>Peak ops/sec</th><th>Peak scenario</th><th>Passed</th><th>Failed</th><th>Run</th></tr>
</thead>
<tbody>
<tr><td>rhel-9.8</td><td>clang</td><td>Debug</td><td>modules</td><td>Smoke</td><td>ssd</td><td style="text-align:right">16</td><td>2026-06-23 05:10:39Z</td><td style="text-align:right">16,232 ms (16s)</td><td style="text-align:right">4,912 ms (4s)</td><td style="text-align:right">3,636,364</td><td>jac313_store_005_XS (none/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/clang/Debug/ssd/Smoke/modules/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v002.001 · jText v002.001 · Store v002.003</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>rhel-9.8</td><td>clang</td><td>Debug</td><td>textual</td><td>Smoke</td><td>ssd</td><td style="text-align:right">17</td><td>2026-06-23 05:11:08Z</td><td style="text-align:right">16,232 ms (16s)</td><td style="text-align:right">-</td><td style="text-align:right">3,389,831</td><td>jac313_store_007_XS (none/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/clang/Debug/ssd/Smoke/textual/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v002.001 · jText v002.001 · Store v002.003</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>rhel-9.8</td><td>clang</td><td>Debug</td><td>modules</td><td>xFull</td><td>ssd</td><td style="text-align:right">25</td><td>2026-06-23 05:15:41Z</td><td style="text-align:right">598,881 ms (9m 58s)</td><td style="text-align:right">5,060 ms (5s)</td><td style="text-align:right">14,766,686</td><td>jac313_store_008_XS (flags/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/clang/Debug/ssd/xFull/modules/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v002.001 · jText v002.001 · Store v002.003</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>rhel-9.8</td><td>clang</td><td>Debug</td><td>textual</td><td>xFull</td><td>ssd</td><td style="text-align:right">26</td><td>2026-06-23 05:25:54Z</td><td style="text-align:right">598,517 ms (9m 58s)</td><td style="text-align:right">-</td><td style="text-align:right">13,997,956</td><td>jac313_store_008_XS (flags/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/clang/Debug/ssd/xFull/textual/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v002.001 · jText v002.001 · Store v002.003</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>rhel-9.8</td><td>clang</td><td>Release</td><td>modules</td><td>Smoke</td><td>ssd</td><td style="text-align:right">18</td><td>2026-06-23 05:11:28Z</td><td style="text-align:right">15,982 ms (15s)</td><td style="text-align:right">7,564 ms (7s)</td><td style="text-align:right">4,950,495</td><td>jac313_store_005_XS (none/on)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/clang/Release/ssd/Smoke/modules/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v002.001 · jText v002.001 · Store v002.003</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>rhel-9.8</td><td>clang</td><td>Release</td><td>textual</td><td>Smoke</td><td>ssd</td><td style="text-align:right">19</td><td>2026-06-23 05:12:03Z</td><td style="text-align:right">16,034 ms (16s)</td><td style="text-align:right">-</td><td style="text-align:right">4,032,258</td><td>jac313_store_005_TS (none/on)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/clang/Release/ssd/Smoke/textual/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v002.001 · jText v002.001 · Store v002.003</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
</tbody>
</table>

<table>
<thead>
<tr><th>OS</th><th>Compiler</th><th>Build</th><th>Modules</th><th>Size</th><th>Disk</th><th>Group</th><th>Run UTC</th><th>Duration</th><th>Build (modules)</th><th>Peak ops/sec</th><th>Peak scenario</th><th>Passed</th><th>Failed</th><th>Run</th></tr>
</thead>
<tbody>
<tr><td>rhel-9.8</td><td>clang</td><td>Release</td><td>modules</td><td>xFull</td><td>ssd</td><td style="text-align:right">27</td><td>2026-06-23 05:35:54Z</td><td style="text-align:right">591,386 ms (9m 51s)</td><td style="text-align:right">7,413 ms (7s)</td><td style="text-align:right">23,364,486</td><td>jac313_store_005_XS (jtext/on)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/clang/Release/ssd/xFull/modules/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v002.001 · jText v002.001 · Store v002.003</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>rhel-9.8</td><td>clang</td><td>Release</td><td>textual</td><td>xFull</td><td>ssd</td><td style="text-align:right">28</td><td>2026-06-23 05:46:04Z</td><td style="text-align:right">590,714 ms (9m 50s)</td><td style="text-align:right">-</td><td style="text-align:right">25,357,541</td><td>jac313_store_008_XS (flags/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/clang/Release/ssd/xFull/textual/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v002.001 · jText v002.001 · Store v002.003</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>rhel-9.8</td><td>gcc15</td><td>Debug</td><td>modules</td><td>Smoke</td><td>ssd</td><td style="text-align:right">21</td><td>2026-06-23 05:13:09Z</td><td style="text-align:right">16,232 ms (16s)</td><td style="text-align:right">6,611 ms (6s)</td><td style="text-align:right">3,436,426</td><td>jac313_store_005_TS (none/on)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/gcc15/Debug/ssd/Smoke/modules/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v002.001 · jText v002.001 · Store v002.003</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>rhel-9.8</td><td>gcc15</td><td>Debug</td><td>textual</td><td>Smoke</td><td>ssd</td><td style="text-align:right">22</td><td>2026-06-23 05:13:54Z</td><td style="text-align:right">16,232 ms (16s)</td><td style="text-align:right">-</td><td style="text-align:right">3,597,122</td><td>jac313_store_007_XS (none/on)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/gcc15/Debug/ssd/Smoke/textual/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v002.001 · jText v002.001 · Store v002.003</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>rhel-9.8</td><td>gcc15</td><td>Debug</td><td>modules</td><td>xFull</td><td>ssd</td><td style="text-align:right">29</td><td>2026-06-23 05:55:57Z</td><td style="text-align:right">599,189 ms (9m 59s)</td><td style="text-align:right">6,712 ms (6s)</td><td style="text-align:right">14,583,637</td><td>jac313_store_005_TS (binary/on)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/gcc15/Debug/ssd/xFull/modules/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v002.001 · jText v002.001 · Store v002.003</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>rhel-9.8</td><td>gcc15</td><td>Debug</td><td>textual</td><td>xFull</td><td>ssd</td><td style="text-align:right">30</td><td>2026-06-23 06:06:25Z</td><td style="text-align:right">599,038 ms (9m 59s)</td><td style="text-align:right">-</td><td style="text-align:right">15,540,982</td><td>jac313_store_008_XS (flags/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/gcc15/Debug/ssd/xFull/textual/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v002.001 · jText v002.001 · Store v002.003</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
</tbody>
</table>

<table>
<thead>
<tr><th>OS</th><th>Compiler</th><th>Build</th><th>Modules</th><th>Size</th><th>Disk</th><th>Group</th><th>Run UTC</th><th>Duration</th><th>Build (modules)</th><th>Peak ops/sec</th><th>Peak scenario</th><th>Passed</th><th>Failed</th><th>Run</th></tr>
</thead>
<tbody>
<tr><td>rhel-9.8</td><td>gcc15</td><td>Release</td><td>modules</td><td>Smoke</td><td>ssd</td><td style="text-align:right">23</td><td>2026-06-23 05:14:16Z</td><td style="text-align:right">16,232 ms (16s)</td><td style="text-align:right">11,821 ms (11s)</td><td style="text-align:right">4,672,897</td><td>jac313_store_005_TS (none/on)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/gcc15/Release/ssd/Smoke/modules/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v002.001 · jText v002.001 · Store v002.003</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>rhel-9.8</td><td>gcc15</td><td>Release</td><td>textual</td><td>Smoke</td><td>ssd</td><td style="text-align:right">24</td><td>2026-06-23 05:15:17Z</td><td style="text-align:right">16,232 ms (16s)</td><td style="text-align:right">-</td><td style="text-align:right">4,385,965</td><td>jac313_store_005_TS (none/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/gcc15/Release/ssd/Smoke/textual/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v002.001 · jText v002.001 · Store v002.003</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>rhel-9.8</td><td>gcc15</td><td>Release</td><td>modules</td><td>xFull</td><td>ssd</td><td style="text-align:right">31</td><td>2026-06-23 06:16:26Z</td><td style="text-align:right">590,890 ms (9m 50s)</td><td style="text-align:right">12,274 ms (12s)</td><td style="text-align:right">24,844,720</td><td>jac313_store_008_TS (flags/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/gcc15/Release/ssd/xFull/modules/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v002.001 · jText v002.001 · Store v002.003</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>rhel-9.8</td><td>gcc15</td><td>Release</td><td>textual</td><td>xFull</td><td>ssd</td><td style="text-align:right">32</td><td>2026-06-23 06:27:02Z</td><td style="text-align:right">589,074 ms (9m 49s)</td><td style="text-align:right">-</td><td style="text-align:right">24,631,755</td><td>jac313_store_008_TS (flags/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/gcc15/Release/ssd/xFull/textual/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v002.001 · jText v002.001 · Store v002.003</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
</tbody>
</table>

## Persist backend comparison (latest xFull per compiler & build type)

Benchmark tests 005–008: **run ms**, **peak ops/sec**, and **persist log size** for binary vs jText vs SQL.

_test_006 reports peak ops/sec as **N/A** — it is a tail-reader concurrency/correctness test, not a throughput benchmark, and emits no ops/sec line._

### rhel-9.8 · clang (xFull, Debug, run 26)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

_Peak ops/sec is **N/A** for test_006: it is a tail-reader concurrency/correctness stress test (writer/reader timing + hit-miss verification), not a throughput benchmark, so it emits no `Fastest run → … ops/sec` line._

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_005_TS | off | binary | 150 | 9,957,184 | 8.38 MiB |
|  |  | jtext | 401 | 9,494,873 | 7.83 MiB |
|  |  | sql | 150 | 11,940,299 | 7.42 MiB |
| jac313_store_005_TS | on | binary | 150 | 10,014,020 | 8.38 MiB |
|  |  | jtext | 350 | 12,306,178 | 7.82 MiB |
|  |  | sql | 150 | 11,052,166 | 7.42 MiB |
| jac313_store_005_XS | off | binary | 150 | 11,691,804 | 8.38 MiB |
|  |  | jtext | 350 | 11,060,723 | 7.44 MiB |
|  |  | sql | 150 | 13,397,642 | 7.13 MiB |
| jac313_store_005_XS | on | binary | 150 | 9,009,821 | 8.38 MiB |
|  |  | jtext | 350 | 10,501,995 | 7.44 MiB |
|  |  | sql | 150 | 11,361,054 | 7.13 MiB |
| jac313_store_006_TS | off | binary | 33,716 | N/A | 19.81 MiB |
|  |  | jtext | 33,738 | N/A | 20.34 MiB |
|  |  | sql | 33,742 | N/A | 23.40 MiB |
| jac313_store_006_TS | on | binary | 37,505 | N/A | 19.81 MiB |
|  |  | jtext | 37,577 | N/A | 20.33 MiB |
|  |  | sql | 37,561 | N/A | 23.40 MiB |
| jac313_store_006_XS | off | binary | 33,108 | N/A | 19.81 MiB |
|  |  | jtext | 33,153 | N/A | 19.96 MiB |
|  |  | sql | 33,156 | N/A | 22.64 MiB |
| jac313_store_006_XS | on | binary | 36,516 | N/A | 19.81 MiB |
|  |  | jtext | 36,622 | N/A | 19.96 MiB |
|  |  | sql | 36,464 | N/A | 22.64 MiB |
| jac313_store_007_TS | off | binary | 150 | 9,811,617 | 19.82 MiB |
|  |  | jtext | 1,053 | 12,259,409 | 20.36 MiB |
|  |  | sql | 250 | 9,083,477 | 23.52 MiB |
| jac313_store_007_TS | on | binary | 150 | 10,751,532 | 19.82 MiB |
|  |  | jtext | 1,052 | 9,759,906 | 20.36 MiB |
|  |  | sql | 250 | 10,187,449 | 23.52 MiB |
| jac313_store_007_XS | off | binary | 150 | 9,574,876 | 19.82 MiB |
|  |  | jtext | 1,002 | 11,381,744 | 19.98 MiB |
|  |  | sql | 250 | 9,362,419 | 22.66 MiB |
| jac313_store_007_XS | on | binary | 150 | 9,970,090 | 19.82 MiB |
|  |  | jtext | 1,002 | 10,312,468 | 19.98 MiB |
|  |  | sql | 250 | 10,907,504 | 22.66 MiB |

### rhel-9.8 · clang (xFull, Release, run 28)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

_Peak ops/sec is **N/A** for test_006: it is a tail-reader concurrency/correctness stress test (writer/reader timing + hit-miss verification), not a throughput benchmark, so it emits no `Fastest run → … ops/sec` line._

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_005_TS | off | binary | 100 | 19,278,967 | 8.38 MiB |
|  |  | jtext | 100 | 14,775,414 | 7.82 MiB |
|  |  | sql | 100 | 14,480,162 | 7.38 MiB |
| jac313_store_005_TS | on | binary | 100 | 15,062,509 | 8.38 MiB |
|  |  | jtext | 100 | 19,305,019 | 7.82 MiB |
|  |  | sql | 100 | 22,217,285 | 7.38 MiB |
| jac313_store_005_XS | off | binary | 100 | 16,147,263 | 8.38 MiB |
|  |  | jtext | 100 | 17,571,604 | 7.44 MiB |
|  |  | sql | 100 | 22,716,947 | 7.13 MiB |
| jac313_store_005_XS | on | binary | 100 | 19,719,976 | 8.38 MiB |
|  |  | jtext | 100 | 15,615,240 | 7.44 MiB |
|  |  | sql | 100 | 23,073,373 | 7.13 MiB |
| jac313_store_006_TS | off | binary | 33,607 | N/A | 19.81 MiB |
|  |  | jtext | 33,712 | N/A | 20.32 MiB |
|  |  | sql | 33,687 | N/A | 23.29 MiB |
| jac313_store_006_TS | on | binary | 37,444 | N/A | 19.81 MiB |
|  |  | jtext | 37,515 | N/A | 20.32 MiB |
|  |  | sql | 37,513 | N/A | 23.29 MiB |
| jac313_store_006_XS | off | binary | 33,081 | N/A | 19.81 MiB |
|  |  | jtext | 33,085 | N/A | 19.96 MiB |
|  |  | sql | 32,866 | N/A | 22.64 MiB |
| jac313_store_006_XS | on | binary | 36,543 | N/A | 19.81 MiB |
|  |  | jtext | 36,501 | N/A | 19.96 MiB |
|  |  | sql | 36,285 | N/A | 22.64 MiB |
| jac313_store_007_TS | off | binary | 100 | 17,614,937 | 19.82 MiB |
|  |  | jtext | 200 | 19,964,065 | 20.36 MiB |
|  |  | sql | 200 | 13,827,434 | 23.44 MiB |
| jac313_store_007_TS | on | binary | 100 | 17,522,341 | 19.82 MiB |
|  |  | jtext | 200 | 20,185,709 | 20.36 MiB |
|  |  | sql | 200 | 14,080,541 | 23.44 MiB |
| jac313_store_007_XS | off | binary | 100 | 15,460,730 | 19.82 MiB |
|  |  | jtext | 200 | 18,047,284 | 19.98 MiB |
|  |  | sql | 200 | 20,048,115 | 22.64 MiB |
| jac313_store_007_XS | on | binary | 100 | 14,775,414 | 19.82 MiB |
|  |  | jtext | 200 | 14,434,180 | 19.98 MiB |
|  |  | sql | 200 | 14,585,764 | 22.64 MiB |

### rhel-9.8 · gcc15 (xFull, Debug, run 30)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

_Peak ops/sec is **N/A** for test_006: it is a tail-reader concurrency/correctness stress test (writer/reader timing + hit-miss verification), not a throughput benchmark, so it emits no `Fastest run → … ops/sec` line._

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_005_TS | off | binary | 150 | 11,760,555 | 8.38 MiB |
|  |  | jtext | 401 | 14,084,507 | 7.83 MiB |
|  |  | sql | 150 | 10,036,130 | 7.42 MiB |
| jac313_store_005_TS | on | binary | 150 | 9,891,197 | 8.38 MiB |
|  |  | jtext | 401 | 13,802,622 | 7.83 MiB |
|  |  | sql | 150 | 13,025,922 | 7.42 MiB |
| jac313_store_005_XS | off | binary | 150 | 11,409,013 | 8.38 MiB |
|  |  | jtext | 401 | 10,864,841 | 7.44 MiB |
|  |  | sql | 150 | 11,304,544 | 7.13 MiB |
| jac313_store_005_XS | on | binary | 150 | 13,071,895 | 8.38 MiB |
|  |  | jtext | 400 | 11,708,231 | 7.44 MiB |
|  |  | sql | 150 | 10,274,324 | 7.13 MiB |
| jac313_store_006_TS | off | binary | 33,688 | N/A | 19.81 MiB |
|  |  | jtext | 33,793 | N/A | 20.34 MiB |
|  |  | sql | 33,732 | N/A | 23.40 MiB |
| jac313_store_006_TS | on | binary | 37,500 | N/A | 19.81 MiB |
|  |  | jtext | 37,601 | N/A | 20.34 MiB |
|  |  | sql | 37,549 | N/A | 23.40 MiB |
| jac313_store_006_XS | off | binary | 33,099 | N/A | 19.81 MiB |
|  |  | jtext | 33,154 | N/A | 19.96 MiB |
|  |  | sql | 33,160 | N/A | 22.64 MiB |
| jac313_store_006_XS | on | binary | 36,509 | N/A | 19.81 MiB |
|  |  | jtext | 36,622 | N/A | 19.96 MiB |
|  |  | sql | 36,577 | N/A | 22.64 MiB |
| jac313_store_007_TS | off | binary | 150 | 13,070,187 | 19.82 MiB |
|  |  | jtext | 1,052 | 10,048,232 | 20.37 MiB |
|  |  | sql | 250 | 11,973,180 | 23.51 MiB |
| jac313_store_007_TS | on | binary | 150 | 12,285,012 | 19.82 MiB |
|  |  | jtext | 1,103 | 11,294,330 | 20.37 MiB |
|  |  | sql | 300 | 11,784,115 | 23.51 MiB |
| jac313_store_007_XS | off | binary | 150 | 12,928,248 | 19.82 MiB |
|  |  | jtext | 1,153 | 10,623,606 | 19.98 MiB |
|  |  | sql | 250 | 10,975,744 | 22.66 MiB |
| jac313_store_007_XS | on | binary | 150 | 11,879,306 | 19.82 MiB |
|  |  | jtext | 1,103 | 11,139,579 | 19.98 MiB |
|  |  | sql | 300 | 12,186,205 | 22.66 MiB |

### rhel-9.8 · gcc15 (xFull, Release, run 32)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

_Peak ops/sec is **N/A** for test_006: it is a tail-reader concurrency/correctness stress test (writer/reader timing + hit-miss verification), not a throughput benchmark, so it emits no `Fastest run → … ops/sec` line._

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_005_TS | off | binary | 100 | 20,052,136 | 8.38 MiB |
|  |  | jtext | 100 | 14,019,347 | 7.82 MiB |
|  |  | sql | 100 | 14,731,880 | 7.38 MiB |
| jac313_store_005_TS | on | binary | 100 | 18,175,209 | 8.38 MiB |
|  |  | jtext | 100 | 15,232,292 | 7.82 MiB |
|  |  | sql | 150 | 14,021,312 | 7.38 MiB |
| jac313_store_005_XS | off | binary | 100 | 15,176,810 | 8.38 MiB |
|  |  | jtext | 100 | 17,975,912 | 7.44 MiB |
|  |  | sql | 100 | 14,210,601 | 7.13 MiB |
| jac313_store_005_XS | on | binary | 100 | 21,570,319 | 8.38 MiB |
|  |  | jtext | 100 | 13,902,405 | 7.44 MiB |
|  |  | sql | 150 | 15,211,439 | 7.13 MiB |
| jac313_store_006_TS | off | binary | 33,692 | N/A | 19.81 MiB |
|  |  | jtext | 33,680 | N/A | 20.33 MiB |
|  |  | sql | 33,625 | N/A | 23.30 MiB |
| jac313_store_006_TS | on | binary | 37,149 | N/A | 19.81 MiB |
|  |  | jtext | 37,462 | N/A | 20.32 MiB |
|  |  | sql | 36,781 | N/A | 23.30 MiB |
| jac313_store_006_XS | off | binary | 33,047 | N/A | 19.81 MiB |
|  |  | jtext | 32,944 | N/A | 19.96 MiB |
|  |  | sql | 33,041 | N/A | 22.63 MiB |
| jac313_store_006_XS | on | binary | 36,496 | N/A | 19.81 MiB |
|  |  | jtext | 36,400 | N/A | 19.96 MiB |
|  |  | sql | 36,518 | N/A | 22.63 MiB |
| jac313_store_007_TS | off | binary | 100 | 13,987,970 | 19.82 MiB |
|  |  | jtext | 200 | 14,675,668 | 20.36 MiB |
|  |  | sql | 200 | 21,244,954 | 23.45 MiB |
| jac313_store_007_TS | on | binary | 100 | 17,780,939 | 19.82 MiB |
|  |  | jtext | 200 | 20,165,356 | 20.36 MiB |
|  |  | sql | 250 | 15,762,926 | 23.45 MiB |
| jac313_store_007_XS | off | binary | 100 | 20,644,096 | 19.82 MiB |
|  |  | jtext | 200 | 14,744,913 | 19.98 MiB |
|  |  | sql | 200 | 14,803,849 | 22.64 MiB |
| jac313_store_007_XS | on | binary | 100 | 14,082,524 | 19.82 MiB |
|  |  | jtext | 200 | 14,566,642 | 19.98 MiB |
|  |  | sql | 250 | 18,351,991 | 22.64 MiB |


## Top throughput by log category

Peak ops/sec from benchmark tests 005–008 (`Fastest run` in each log). xFull matrix only, grouped by persist log directory.

### binary_logs

<table>
<thead>
<tr><th>#</th><th>Peak ops/sec</th><th>Test</th><th>Persist</th><th>ms</th><th>Log</th></tr>
</thead>
<tbody>
<tr><td>1</td><td style="text-align:right">22,899,015</td><td>jac313_store_007_XS</td><td>binary/on</td><td style="text-align:right">100</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/modules/binary_logs/JAC313_STORE_TEST_007_XS/clang_binary_on.log">clang_binary_on.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>2</td><td style="text-align:right">22,639,801</td><td>jac313_store_005_XS</td><td>binary/on</td><td style="text-align:right">100</td><td><a href="rhel-9.8/gcc15/Release/ssd/xFull/modules/binary_logs/JAC313_STORE_TEST_005_XS/gcc15_binary_on.log">gcc15_binary_on.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>3</td><td style="text-align:right">21,810,251</td><td>jac313_store_005_TS</td><td>binary/on</td><td style="text-align:right">100</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/modules/binary_logs/JAC313_STORE_TEST_005_TS/clang_binary_on.log">clang_binary_on.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>4</td><td style="text-align:right">21,570,319</td><td>jac313_store_005_XS</td><td>binary/on</td><td style="text-align:right">100</td><td><a href="rhel-9.8/gcc15/Release/ssd/xFull/textual/binary_logs/JAC313_STORE_TEST_005_XS/gcc15_binary_on.log">gcc15_binary_on.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>5</td><td style="text-align:right">21,533,161</td><td>jac313_store_007_TS</td><td>binary/off</td><td style="text-align:right">100</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/modules/binary_logs/JAC313_STORE_TEST_007_TS/clang_binary_off.log">clang_binary_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>6</td><td style="text-align:right">20,733,983</td><td>jac313_store_007_XS</td><td>binary/on</td><td style="text-align:right">100</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/textual/binary_logs/JAC313_STORE_TEST_007_XS/clang_binary_on.log">clang_binary_on.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>7</td><td style="text-align:right">20,644,096</td><td>jac313_store_007_XS</td><td>binary/off</td><td style="text-align:right">100</td><td><a href="rhel-9.8/gcc15/Release/ssd/xFull/textual/binary_logs/JAC313_STORE_TEST_007_XS/gcc15_binary_off.log">gcc15_binary_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>8</td><td style="text-align:right">20,521,239</td><td>jac313_store_007_TS</td><td>binary/off</td><td style="text-align:right">100</td><td><a href="rhel-9.8/gcc15/Release/ssd/xFull/modules/binary_logs/JAC313_STORE_TEST_007_TS/gcc15_binary_off.log">gcc15_binary_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>9</td><td style="text-align:right">20,296,326</td><td>jac313_store_005_XS</td><td>binary/off</td><td style="text-align:right">100</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/modules/binary_logs/JAC313_STORE_TEST_005_XS/clang_binary_off.log">clang_binary_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>10</td><td style="text-align:right">20,177,563</td><td>jac313_store_005_XS</td><td>binary/off</td><td style="text-align:right">100</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/textual/binary_logs/JAC313_STORE_TEST_005_XS/clang_binary_off.log">clang_binary_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
</tbody>
</table>

### inmem_logs

<table>
<thead>
<tr><th>#</th><th>Peak ops/sec</th><th>Test</th><th>Persist</th><th>ms</th><th>Log</th></tr>
</thead>
<tbody>
<tr><td>1</td><td style="text-align:right">23,803,856</td><td>jac313_store_007_TS</td><td>none/on</td><td style="text-align:right">50</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/modules/inmem_logs/JAC313_STORE_TEST_007_TS/clang_none_on.log">clang_none_on.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>2</td><td style="text-align:right">22,794,620</td><td>jac313_store_007_TS</td><td>none/off</td><td style="text-align:right">50</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/textual/inmem_logs/JAC313_STORE_TEST_007_TS/clang_none_off.log">clang_none_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>3</td><td style="text-align:right">22,573,363</td><td>jac313_store_005_TS</td><td>none/off</td><td style="text-align:right">50</td><td><a href="rhel-9.8/gcc15/Release/ssd/xFull/modules/inmem_logs/JAC313_STORE_TEST_005_TS/gcc15_none_off.log">gcc15_none_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>4</td><td style="text-align:right">22,456,771</td><td>jac313_store_007_TS</td><td>none/off</td><td style="text-align:right">50</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/textual/inmem_logs/JAC313_STORE_TEST_007_TS/clang_none_off.log">clang_none_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>5</td><td style="text-align:right">22,366,361</td><td>jac313_store_007_XS</td><td>none/off</td><td style="text-align:right">50</td><td><a href="rhel-9.8/gcc15/Release/ssd/xFull/modules/inmem_logs/JAC313_STORE_TEST_007_XS/gcc15_none_off.log">gcc15_none_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>6</td><td style="text-align:right">21,417,862</td><td>jac313_store_005_XS</td><td>none/on</td><td style="text-align:right">50</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/textual/inmem_logs/JAC313_STORE_TEST_005_XS/clang_none_on.log">clang_none_on.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>7</td><td style="text-align:right">21,413,276</td><td>jac313_store_005_TS</td><td>none/on</td><td style="text-align:right">50</td><td><a href="rhel-9.8/gcc15/Release/ssd/xFull/modules/inmem_logs/JAC313_STORE_TEST_005_TS/gcc15_none_on.log">gcc15_none_on.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>8</td><td style="text-align:right">21,272,070</td><td>jac313_store_007_TS</td><td>none/off</td><td style="text-align:right">50</td><td><a href="rhel-9.8/gcc15/Release/ssd/xFull/modules/inmem_logs/JAC313_STORE_TEST_007_TS/gcc15_none_off.log">gcc15_none_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>9</td><td style="text-align:right">21,034,918</td><td>jac313_store_007_TS</td><td>none/off</td><td style="text-align:right">50</td><td><a href="rhel-9.8/gcc15/Release/ssd/xFull/textual/inmem_logs/JAC313_STORE_TEST_007_TS/gcc15_none_off.log">gcc15_none_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>10</td><td style="text-align:right">20,942,408</td><td>jac313_store_005_TS</td><td>none/on</td><td style="text-align:right">50</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/modules/inmem_logs/JAC313_STORE_TEST_005_TS/clang_none_on.log">clang_none_on.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
</tbody>
</table>

### sql_logs

<table>
<thead>
<tr><th>#</th><th>Peak ops/sec</th><th>Test</th><th>Persist</th><th>ms</th><th>Log</th></tr>
</thead>
<tbody>
<tr><td>1</td><td style="text-align:right">23,073,373</td><td>jac313_store_005_XS</td><td>sql/on</td><td style="text-align:right">100</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/textual/sql_logs/JAC313_STORE_TEST_005_XS/clang_sql_on.log">clang_sql_on.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>2</td><td style="text-align:right">22,716,947</td><td>jac313_store_005_XS</td><td>sql/off</td><td style="text-align:right">100</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/textual/sql_logs/JAC313_STORE_TEST_005_XS/clang_sql_off.log">clang_sql_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>3</td><td style="text-align:right">22,217,285</td><td>jac313_store_005_TS</td><td>sql/on</td><td style="text-align:right">100</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/textual/sql_logs/JAC313_STORE_TEST_005_TS/clang_sql_on.log">clang_sql_on.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>4</td><td style="text-align:right">21,244,954</td><td>jac313_store_007_TS</td><td>sql/off</td><td style="text-align:right">200</td><td><a href="rhel-9.8/gcc15/Release/ssd/xFull/textual/sql_logs/JAC313_STORE_TEST_007_TS/gcc15_sql_off.log">gcc15_sql_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>5</td><td style="text-align:right">21,101,498</td><td>jac313_store_005_XS</td><td>sql/off</td><td style="text-align:right">100</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/modules/sql_logs/JAC313_STORE_TEST_005_XS/clang_sql_off.log">clang_sql_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>6</td><td style="text-align:right">20,946,795</td><td>jac313_store_007_XS</td><td>sql/off</td><td style="text-align:right">200</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/modules/sql_logs/JAC313_STORE_TEST_007_XS/clang_sql_off.log">clang_sql_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>7</td><td style="text-align:right">20,656,889</td><td>jac313_store_005_TS</td><td>sql/off</td><td style="text-align:right">100</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/modules/sql_logs/JAC313_STORE_TEST_005_TS/clang_sql_off.log">clang_sql_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>8</td><td style="text-align:right">20,631,318</td><td>jac313_store_005_TS</td><td>sql/off</td><td style="text-align:right">100</td><td><a href="rhel-9.8/gcc15/Release/ssd/xFull/modules/sql_logs/JAC313_STORE_TEST_005_TS/gcc15_sql_off.log">gcc15_sql_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>9</td><td style="text-align:right">20,145,044</td><td>jac313_store_007_XS</td><td>sql/on</td><td style="text-align:right">250</td><td><a href="rhel-9.8/gcc15/Release/ssd/xFull/modules/sql_logs/JAC313_STORE_TEST_007_XS/gcc15_sql_on.log">gcc15_sql_on.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>10</td><td style="text-align:right">20,048,115</td><td>jac313_store_007_XS</td><td>sql/off</td><td style="text-align:right">200</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/textual/sql_logs/JAC313_STORE_TEST_007_XS/clang_sql_off.log">clang_sql_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
</tbody>
</table>

### jText_logs

<table>
<thead>
<tr><th>#</th><th>Peak ops/sec</th><th>Test</th><th>Persist</th><th>ms</th><th>Log</th></tr>
</thead>
<tbody>
<tr><td>1</td><td style="text-align:right">23,364,486</td><td>jac313_store_005_XS</td><td>jtext/on</td><td style="text-align:right">100</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/modules/jText_logs/JAC313_STORE_TEST_005_XS/clang_jtext_on.log">clang_jtext_on.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>2</td><td style="text-align:right">22,967,386</td><td>jac313_store_005_XS</td><td>jtext/off</td><td style="text-align:right">100</td><td><a href="rhel-9.8/gcc15/Release/ssd/xFull/modules/jText_logs/JAC313_STORE_TEST_005_XS/gcc15_jtext_off.log">gcc15_jtext_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>3</td><td style="text-align:right">21,829,295</td><td>jac313_store_005_TS</td><td>jtext/on</td><td style="text-align:right">100</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/textual/jText_logs/JAC313_STORE_TEST_005_TS/clang_jtext_on.log">clang_jtext_on.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>4</td><td style="text-align:right">21,654,396</td><td>jac313_store_005_TS</td><td>jtext/off</td><td style="text-align:right">100</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/textual/jText_logs/JAC313_STORE_TEST_005_TS/clang_jtext_off.log">clang_jtext_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>5</td><td style="text-align:right">21,533,161</td><td>jac313_store_005_XS</td><td>jtext/on</td><td style="text-align:right">100</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/textual/jText_logs/JAC313_STORE_TEST_005_XS/clang_jtext_on.log">clang_jtext_on.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>6</td><td style="text-align:right">21,150,592</td><td>jac313_store_007_XS</td><td>jtext/off</td><td style="text-align:right">200</td><td><a href="rhel-9.8/gcc15/Release/ssd/xFull/modules/jText_logs/JAC313_STORE_TEST_007_XS/gcc15_jtext_off.log">gcc15_jtext_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>7</td><td style="text-align:right">20,933,640</td><td>jac313_store_005_TS</td><td>jtext/off</td><td style="text-align:right">100</td><td><a href="rhel-9.8/gcc15/Release/ssd/xFull/modules/jText_logs/JAC313_STORE_TEST_005_TS/gcc15_jtext_off.log">gcc15_jtext_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>8</td><td style="text-align:right">20,521,239</td><td>jac313_store_007_TS</td><td>jtext/off</td><td style="text-align:right">200</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/textual/jText_logs/JAC313_STORE_TEST_007_TS/clang_jtext_off.log">clang_jtext_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>9</td><td style="text-align:right">20,517,029</td><td>jac313_store_007_TS</td><td>jtext/on</td><td style="text-align:right">200</td><td><a href="rhel-9.8/gcc15/Release/ssd/xFull/modules/jText_logs/JAC313_STORE_TEST_007_TS/gcc15_jtext_on.log">gcc15_jtext_on.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>10</td><td style="text-align:right">20,222,447</td><td>jac313_store_007_XS</td><td>jtext/on</td><td style="text-align:right">200</td><td><a href="rhel-9.8/gcc15/Release/ssd/xFull/modules/jText_logs/JAC313_STORE_TEST_007_XS/gcc15_jtext_on.log">gcc15_jtext_on.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
</tbody>
</table>

### flags_logs

<table>
<thead>
<tr><th>#</th><th>Peak ops/sec</th><th>Test</th><th>Persist</th><th>ms</th><th>Log</th></tr>
</thead>
<tbody>
<tr><td>1</td><td style="text-align:right">25,357,541</td><td>jac313_store_008_XS</td><td>flags/off</td><td style="text-align:right">652</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/textual/flags_logs/JAC313_STORE_TEST_008_XS/clang_flags_off.log">clang_flags_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>2</td><td style="text-align:right">24,870,054</td><td>jac313_store_008_XS</td><td>flags/off</td><td style="text-align:right">651</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/textual/flags_logs/JAC313_STORE_TEST_008_XS/clang_flags_off.log">clang_flags_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>3</td><td style="text-align:right">24,844,720</td><td>jac313_store_008_TS</td><td>flags/off</td><td style="text-align:right">651</td><td><a href="rhel-9.8/gcc15/Release/ssd/xFull/modules/flags_logs/JAC313_STORE_TEST_008_TS/gcc15_flags_off.log">gcc15_flags_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>4</td><td style="text-align:right">24,818,823</td><td>jac313_store_008_XS</td><td>flags/off</td><td style="text-align:right">701</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/modules/flags_logs/JAC313_STORE_TEST_008_XS/clang_flags_off.log">clang_flags_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>5</td><td style="text-align:right">24,631,755</td><td>jac313_store_008_TS</td><td>flags/off</td><td style="text-align:right">651</td><td><a href="rhel-9.8/gcc15/Release/ssd/xFull/textual/flags_logs/JAC313_STORE_TEST_008_TS/gcc15_flags_off.log">gcc15_flags_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>6</td><td style="text-align:right">24,608,721</td><td>jac313_store_008_XS</td><td>flags/off</td><td style="text-align:right">651</td><td><a href="rhel-9.8/gcc15/Release/ssd/xFull/textual/flags_logs/JAC313_STORE_TEST_008_XS/gcc15_flags_off.log">gcc15_flags_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>7</td><td style="text-align:right">23,937,762</td><td>jac313_store_008_TS</td><td>flags/off</td><td style="text-align:right">652</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/textual/flags_logs/JAC313_STORE_TEST_008_TS/clang_flags_off.log">clang_flags_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>8</td><td style="text-align:right">23,147,077</td><td>jac313_store_008_TS</td><td>flags/off</td><td style="text-align:right">651</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/modules/flags_logs/JAC313_STORE_TEST_008_TS/clang_flags_off.log">clang_flags_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>9</td><td style="text-align:right">17,407,652</td><td>jac313_store_008_TS</td><td>flags/off</td><td style="text-align:right">701</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/modules/flags_logs/JAC313_STORE_TEST_008_TS/clang_flags_off.log">clang_flags_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>10</td><td style="text-align:right">17,332,224</td><td>jac313_store_008_XS</td><td>flags/off</td><td style="text-align:right">1,102</td><td><a href="rhel-9.8/gcc15/Debug/ssd/xFull/modules/flags_logs/JAC313_STORE_TEST_008_XS/gcc15_flags_off.log">gcc15_flags_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
</tbody>
</table>

