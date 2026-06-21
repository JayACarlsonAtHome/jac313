# jac313 test-summary hub

Run index and per-run pages from [`jac313_results.db`](jac313_results.db); log tails (last 50 lines) published at matrix run from captured stdout.
Pipeline docs: [SETUP → Testing](../docs/Setup.md#4-testing) (`release-check`, matrix runs, `--filter`).

<table>
<thead>
<tr><th>OS</th><th>Compiler</th><th>Build</th><th>Modules</th><th>Size</th><th>Disk</th><th>Group</th><th>Run UTC</th><th>Duration</th><th>Build (modules)</th><th>Peak ops/sec</th><th>Peak scenario</th><th>Passed</th><th>Failed</th><th>Run</th></tr>
</thead>
<tbody>
<tr><td>almalinux-10.2</td><td>clang</td><td>Debug</td><td>textual</td><td>Smoke</td><td>x7k</td><td style="text-align:right">84</td><td>2026-06-20 22:49:04Z</td><td style="text-align:right">19,936 ms (19s)</td><td style="text-align:right">-</td><td style="text-align:right">437,828</td><td>jac313_store_005_XS (none/on)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/almalinux-10.2/clang/Debug/x7k/Smoke/textual/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-003 · AlmaLinux 10.2 (Lavender Lion) (6.12.0-211.22.1.el10_2.x86_64_v2) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 122 GB RAM · vda (x7k label)</td></tr>
<tr><td>almalinux-10.2</td><td>clang</td><td>Debug</td><td>modules</td><td>Smoke</td><td>x7k</td><td style="text-align:right">83</td><td>2026-06-20 22:47:01Z</td><td style="text-align:right">20,443 ms (20s)</td><td style="text-align:right">19,716 ms (19s)</td><td style="text-align:right">458,295</td><td>jac313_store_005_TS (none/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/almalinux-10.2/clang/Debug/x7k/Smoke/modules/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-003 · AlmaLinux 10.2 (Lavender Lion) (6.12.0-211.22.1.el10_2.x86_64_v2) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 122 GB RAM · vda (x7k label)</td></tr>
<tr><td>almalinux-10.2</td><td>clang</td><td>Debug</td><td>textual</td><td>xFull</td><td>x7k</td><td style="text-align:right">92</td><td>2026-06-20 23:13:48Z</td><td style="text-align:right">668,661 ms (11m 8s)</td><td style="text-align:right">-</td><td style="text-align:right">2,779,453</td><td>jac313_store_008_XS (flags/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/almalinux-10.2/clang/Debug/x7k/xFull/textual/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-003 · AlmaLinux 10.2 (Lavender Lion) (6.12.0-211.22.1.el10_2.x86_64_v2) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 122 GB RAM · vda (x7k label)</td></tr>
<tr><td>almalinux-10.2</td><td>clang</td><td>Debug</td><td>modules</td><td>xFull</td><td>x7k</td><td style="text-align:right">91</td><td>2026-06-20 23:01:09Z</td><td style="text-align:right">671,426 ms (11m 11s)</td><td style="text-align:right">20,774 ms (20s)</td><td style="text-align:right">2,830,095</td><td>jac313_store_008_XS (flags/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/almalinux-10.2/clang/Debug/x7k/xFull/modules/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-003 · AlmaLinux 10.2 (Lavender Lion) (6.12.0-211.22.1.el10_2.x86_64_v2) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 122 GB RAM · vda (x7k label)</td></tr>
<tr><td>almalinux-10.2</td><td>clang</td><td>Release</td><td>textual</td><td>Smoke</td><td>x7k</td><td style="text-align:right">86</td><td>2026-06-20 22:52:36Z</td><td style="text-align:right">19,746 ms (19s)</td><td style="text-align:right">-</td><td style="text-align:right">650,618</td><td>jac313_store_007_XS (none/on)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/almalinux-10.2/clang/Release/x7k/Smoke/textual/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-003 · AlmaLinux 10.2 (Lavender Lion) (6.12.0-211.22.1.el10_2.x86_64_v2) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 122 GB RAM · vda (x7k label)</td></tr>
<tr><td>almalinux-10.2</td><td>clang</td><td>Release</td><td>modules</td><td>Smoke</td><td>x7k</td><td style="text-align:right">85</td><td>2026-06-20 22:50:03Z</td><td style="text-align:right">19,800 ms (19s)</td><td style="text-align:right">27,955 ms (27s)</td><td style="text-align:right">647,249</td><td>jac313_store_005_XS (none/on)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/almalinux-10.2/clang/Release/x7k/Smoke/modules/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-003 · AlmaLinux 10.2 (Lavender Lion) (6.12.0-211.22.1.el10_2.x86_64_v2) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 122 GB RAM · vda (x7k label)</td></tr>
</tbody>
</table>

<table>
<thead>
<tr><th>OS</th><th>Compiler</th><th>Build</th><th>Modules</th><th>Size</th><th>Disk</th><th>Group</th><th>Run UTC</th><th>Duration</th><th>Build (modules)</th><th>Peak ops/sec</th><th>Peak scenario</th><th>Passed</th><th>Failed</th><th>Run</th></tr>
</thead>
<tbody>
<tr><td>almalinux-10.2</td><td>clang</td><td>Release</td><td>modules</td><td>xFull</td><td>x7k</td><td style="text-align:right">93</td><td>2026-06-20 23:25:35Z</td><td style="text-align:right">632,455 ms (10m 32s)</td><td style="text-align:right">28,403 ms (28s)</td><td style="text-align:right">5,283,681</td><td>jac313_store_008_XS (flags/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/almalinux-10.2/clang/Release/x7k/xFull/modules/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-003 · AlmaLinux 10.2 (Lavender Lion) (6.12.0-211.22.1.el10_2.x86_64_v2) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 122 GB RAM · vda (x7k label)</td></tr>
<tr><td>almalinux-10.2</td><td>clang</td><td>Release</td><td>textual</td><td>xFull</td><td>x7k</td><td style="text-align:right">94</td><td>2026-06-20 23:37:50Z</td><td style="text-align:right">633,841 ms (10m 33s)</td><td style="text-align:right">-</td><td style="text-align:right">5,481,615</td><td>jac313_store_008_XS (flags/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/almalinux-10.2/clang/Release/x7k/xFull/textual/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-003 · AlmaLinux 10.2 (Lavender Lion) (6.12.0-211.22.1.el10_2.x86_64_v2) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 122 GB RAM · vda (x7k label)</td></tr>
<tr><td>almalinux-10.2</td><td>gcc15</td><td>Debug</td><td>modules</td><td>Smoke</td><td>x7k</td><td style="text-align:right">87</td><td>2026-06-20 22:53:33Z</td><td style="text-align:right">23,393 ms (23s)</td><td style="text-align:right">25,014 ms (25s)</td><td style="text-align:right">452,899</td><td>jac313_store_005_TS (none/on)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/almalinux-10.2/gcc15/Debug/x7k/Smoke/modules/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-003 · AlmaLinux 10.2 (Lavender Lion) (6.12.0-211.22.1.el10_2.x86_64_v2) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 122 GB RAM · vda (x7k label)</td></tr>
<tr><td>almalinux-10.2</td><td>gcc15</td><td>Debug</td><td>textual</td><td>Smoke</td><td>x7k</td><td style="text-align:right">88</td><td>2026-06-20 22:55:56Z</td><td style="text-align:right">21,515 ms (21s)</td><td style="text-align:right">-</td><td style="text-align:right">417,188</td><td>jac313_store_005_XS (none/on)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/almalinux-10.2/gcc15/Debug/x7k/Smoke/textual/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-003 · AlmaLinux 10.2 (Lavender Lion) (6.12.0-211.22.1.el10_2.x86_64_v2) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 122 GB RAM · vda (x7k label)</td></tr>
<tr><td>almalinux-10.2</td><td>gcc15</td><td>Debug</td><td>modules</td><td>xFull</td><td>x7k</td><td style="text-align:right">95</td><td>2026-06-20 23:49:00Z</td><td style="text-align:right">667,634 ms (11m 7s)</td><td style="text-align:right">24,233 ms (24s)</td><td style="text-align:right">3,687,397</td><td>jac313_store_008_TS (flags/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/almalinux-10.2/gcc15/Debug/x7k/xFull/modules/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-003 · AlmaLinux 10.2 (Lavender Lion) (6.12.0-211.22.1.el10_2.x86_64_v2) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 122 GB RAM · vda (x7k label)</td></tr>
<tr><td>almalinux-10.2</td><td>gcc15</td><td>Debug</td><td>textual</td><td>xFull</td><td>x7k</td><td style="text-align:right">96</td><td>2026-06-21 00:01:46Z</td><td style="text-align:right">666,425 ms (11m 6s)</td><td style="text-align:right">-</td><td style="text-align:right">3,658,862</td><td>jac313_store_008_TS (flags/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/almalinux-10.2/gcc15/Debug/x7k/xFull/textual/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-003 · AlmaLinux 10.2 (Lavender Lion) (6.12.0-211.22.1.el10_2.x86_64_v2) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 122 GB RAM · vda (x7k label)</td></tr>
</tbody>
</table>

<table>
<thead>
<tr><th>OS</th><th>Compiler</th><th>Build</th><th>Modules</th><th>Size</th><th>Disk</th><th>Group</th><th>Run UTC</th><th>Duration</th><th>Build (modules)</th><th>Peak ops/sec</th><th>Peak scenario</th><th>Passed</th><th>Failed</th><th>Run</th></tr>
</thead>
<tbody>
<tr><td>almalinux-10.2</td><td>gcc15</td><td>Release</td><td>modules</td><td>Smoke</td><td>x7k</td><td style="text-align:right">89</td><td>2026-06-20 22:56:56Z</td><td style="text-align:right">20,105 ms (20s)</td><td style="text-align:right">43,040 ms (43s)</td><td style="text-align:right">667,111</td><td>jac313_store_007_XS (none/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/almalinux-10.2/gcc15/Release/x7k/Smoke/modules/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-003 · AlmaLinux 10.2 (Lavender Lion) (6.12.0-211.22.1.el10_2.x86_64_v2) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 122 GB RAM · vda (x7k label)</td></tr>
<tr><td>almalinux-10.2</td><td>gcc15</td><td>Release</td><td>textual</td><td>Smoke</td><td>x7k</td><td style="text-align:right">90</td><td>2026-06-20 23:00:11Z</td><td style="text-align:right">19,399 ms (19s)</td><td style="text-align:right">-</td><td style="text-align:right">570,451</td><td>jac313_store_007_XS (none/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/almalinux-10.2/gcc15/Release/x7k/Smoke/textual/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-003 · AlmaLinux 10.2 (Lavender Lion) (6.12.0-211.22.1.el10_2.x86_64_v2) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 122 GB RAM · vda (x7k label)</td></tr>
<tr><td>almalinux-10.2</td><td>gcc15</td><td>Release</td><td>modules</td><td>xFull</td><td>x7k</td><td style="text-align:right">97</td><td>2026-06-21 00:13:29Z</td><td style="text-align:right">631,766 ms (10m 31s)</td><td style="text-align:right">42,651 ms (42s)</td><td style="text-align:right">7,562,180</td><td>jac313_store_008_XS (flags/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/almalinux-10.2/gcc15/Release/x7k/xFull/modules/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-003 · AlmaLinux 10.2 (Lavender Lion) (6.12.0-211.22.1.el10_2.x86_64_v2) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 122 GB RAM · vda (x7k label)</td></tr>
<tr><td>almalinux-10.2</td><td>gcc15</td><td>Release</td><td>textual</td><td>xFull</td><td>x7k</td><td style="text-align:right">98</td><td>2026-06-21 00:26:11Z</td><td style="text-align:right">632,794 ms (10m 32s)</td><td style="text-align:right">-</td><td style="text-align:right">5,739,144</td><td>jac313_store_008_XS (flags/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/almalinux-10.2/gcc15/Release/x7k/xFull/textual/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-003 · AlmaLinux 10.2 (Lavender Lion) (6.12.0-211.22.1.el10_2.x86_64_v2) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 122 GB RAM · vda (x7k label)</td></tr>
<tr><td>fedora-44</td><td>clang</td><td>Debug</td><td>modules</td><td>Smoke</td><td>x7k</td><td style="text-align:right">50</td><td>2026-06-20 15:20:59Z</td><td style="text-align:right">21,577 ms (21s)</td><td style="text-align:right">29,255 ms (29s)</td><td style="text-align:right">448,430</td><td>jac313_store_005_XS (none/on)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/fedora-44/clang/Debug/x7k/Smoke/modules/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-005 · Fedora Linux 44 (Server Edition) (6.19.10-300.fc44.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 98 GB RAM · vda (x7k label)</td></tr>
<tr><td>fedora-44</td><td>clang</td><td>Debug</td><td>textual</td><td>Smoke</td><td>x7k</td><td style="text-align:right">52</td><td>2026-06-20 15:35:46Z</td><td style="text-align:right">21,206 ms (21s)</td><td style="text-align:right">-</td><td style="text-align:right">508,647</td><td>jac313_store_005_TS (none/on)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/fedora-44/clang/Debug/x7k/Smoke/textual/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-005 · Fedora Linux 44 (Server Edition) (6.19.10-300.fc44.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 98 GB RAM · vda (x7k label)</td></tr>
</tbody>
</table>

<table>
<thead>
<tr><th>OS</th><th>Compiler</th><th>Build</th><th>Modules</th><th>Size</th><th>Disk</th><th>Group</th><th>Run UTC</th><th>Duration</th><th>Build (modules)</th><th>Peak ops/sec</th><th>Peak scenario</th><th>Passed</th><th>Failed</th><th>Run</th></tr>
</thead>
<tbody>
<tr><td>fedora-44</td><td>clang</td><td>Debug</td><td>modules</td><td>xFull</td><td>x7k</td><td style="text-align:right">51</td><td>2026-06-20 15:23:07Z</td><td style="text-align:right">649,773 ms (10m 49s)</td><td style="text-align:right">23,181 ms (23s)</td><td style="text-align:right">3,530,961</td><td>jac313_store_008_TS (flags/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/fedora-44/clang/Debug/x7k/xFull/modules/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-005 · Fedora Linux 44 (Server Edition) (6.19.10-300.fc44.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 98 GB RAM · vda (x7k label)</td></tr>
<tr><td>fedora-44</td><td>clang</td><td>Debug</td><td>textual</td><td>xFull</td><td>x7k</td><td style="text-align:right">53</td><td>2026-06-20 15:36:44Z</td><td style="text-align:right">648,480 ms (10m 48s)</td><td style="text-align:right">-</td><td style="text-align:right">3,545,697</td><td>jac313_store_008_TS (flags/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/fedora-44/clang/Debug/x7k/xFull/textual/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-005 · Fedora Linux 44 (Server Edition) (6.19.10-300.fc44.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 98 GB RAM · vda (x7k label)</td></tr>
<tr><td>fedora-44</td><td>clang</td><td>Release</td><td>modules</td><td>Smoke</td><td>x7k</td><td style="text-align:right">54</td><td>2026-06-20 15:48:08Z</td><td style="text-align:right">19,396 ms (19s)</td><td style="text-align:right">28,190 ms (28s)</td><td style="text-align:right">749,064</td><td>jac313_store_005_TS (none/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/fedora-44/clang/Release/x7k/Smoke/modules/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-005 · Fedora Linux 44 (Server Edition) (6.19.10-300.fc44.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 98 GB RAM · vda (x7k label)</td></tr>
<tr><td>fedora-44</td><td>clang</td><td>Release</td><td>textual</td><td>Smoke</td><td>x7k</td><td style="text-align:right">56</td><td>2026-06-20 16:02:26Z</td><td style="text-align:right">19,541 ms (19s)</td><td style="text-align:right">-</td><td style="text-align:right">769,823</td><td>jac313_store_007_XS (none/on)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/fedora-44/clang/Release/x7k/Smoke/textual/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-005 · Fedora Linux 44 (Server Edition) (6.19.10-300.fc44.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 98 GB RAM · vda (x7k label)</td></tr>
<tr><td>fedora-44</td><td>clang</td><td>Release</td><td>modules</td><td>xFull</td><td>x7k</td><td style="text-align:right">55</td><td>2026-06-20 15:50:06Z</td><td style="text-align:right">614,059 ms (10m 14s)</td><td style="text-align:right">28,336 ms (28s)</td><td style="text-align:right">5,373,686</td><td>jac313_store_008_XS (flags/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/fedora-44/clang/Release/x7k/xFull/modules/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-005 · Fedora Linux 44 (Server Edition) (6.19.10-300.fc44.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 98 GB RAM · vda (x7k label)</td></tr>
<tr><td>fedora-44</td><td>clang</td><td>Release</td><td>textual</td><td>xFull</td><td>x7k</td><td style="text-align:right">57</td><td>2026-06-20 16:03:23Z</td><td style="text-align:right">615,145 ms (10m 15s)</td><td style="text-align:right">-</td><td style="text-align:right">7,952,097</td><td>jac313_store_008_XS (flags/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/fedora-44/clang/Release/x7k/xFull/textual/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-005 · Fedora Linux 44 (Server Edition) (6.19.10-300.fc44.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 98 GB RAM · vda (x7k label)</td></tr>
</tbody>
</table>

<table>
<thead>
<tr><th>OS</th><th>Compiler</th><th>Build</th><th>Modules</th><th>Size</th><th>Disk</th><th>Group</th><th>Run UTC</th><th>Duration</th><th>Build (modules)</th><th>Peak ops/sec</th><th>Peak scenario</th><th>Passed</th><th>Failed</th><th>Run</th></tr>
</thead>
<tbody>
<tr><td>fedora-44</td><td>gcc16</td><td>Debug</td><td>textual</td><td>Smoke</td><td>x7k</td><td style="text-align:right">61</td><td>2026-06-20 18:38:12Z</td><td style="text-align:right">18,397 ms (18s)</td><td style="text-align:right">-</td><td style="text-align:right">520,833</td><td>jac313_store_005_TS (none/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/fedora-44/gcc16/Debug/x7k/Smoke/textual/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-005 · Fedora Linux 44 (Server Edition) (6.19.10-300.fc44.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 98 GB RAM · vda (x7k label)</td></tr>
<tr><td>fedora-44</td><td>gcc16</td><td>Debug</td><td>modules</td><td>Smoke</td><td>x7k</td><td style="text-align:right">59</td><td>2026-06-20 18:22:36Z</td><td style="text-align:right">21,167 ms (21s)</td><td style="text-align:right">23,589 ms (23s)</td><td style="text-align:right">511,247</td><td>jac313_store_005_XS (none/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/fedora-44/gcc16/Debug/x7k/Smoke/modules/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-005 · Fedora Linux 44 (Server Edition) (6.19.10-300.fc44.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 98 GB RAM · vda (x7k label)</td></tr>
<tr><td>fedora-44</td><td>gcc16</td><td>Debug</td><td>modules</td><td>xFull</td><td>x7k</td><td style="text-align:right">60</td><td>2026-06-20 18:25:12Z</td><td style="text-align:right">645,369 ms (10m 45s)</td><td style="text-align:right">22,569 ms (22s)</td><td style="text-align:right">4,437,384</td><td>jac313_store_008_XS (flags/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/fedora-44/gcc16/Debug/x7k/xFull/modules/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-005 · Fedora Linux 44 (Server Edition) (6.19.10-300.fc44.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 98 GB RAM · vda (x7k label)</td></tr>
<tr><td>fedora-44</td><td>gcc16</td><td>Debug</td><td>textual</td><td>xFull</td><td>x7k</td><td style="text-align:right">62</td><td>2026-06-20 18:39:10Z</td><td style="text-align:right">646,756 ms (10m 46s)</td><td style="text-align:right">-</td><td style="text-align:right">4,514,836</td><td>jac313_store_008_TS (flags/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/fedora-44/gcc16/Debug/x7k/xFull/textual/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-005 · Fedora Linux 44 (Server Edition) (6.19.10-300.fc44.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 98 GB RAM · vda (x7k label)</td></tr>
<tr><td>fedora-44</td><td>gcc16</td><td>Release</td><td>modules</td><td>Smoke</td><td>x7k</td><td style="text-align:right">63</td><td>2026-06-20 18:50:36Z</td><td style="text-align:right">19,399 ms (19s)</td><td style="text-align:right">36,512 ms (36s)</td><td style="text-align:right">687,758</td><td>jac313_store_005_XS (none/on)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/fedora-44/gcc16/Release/x7k/Smoke/modules/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-005 · Fedora Linux 44 (Server Edition) (6.19.10-300.fc44.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 98 GB RAM · vda (x7k label)</td></tr>
<tr><td>fedora-44</td><td>gcc16</td><td>Release</td><td>textual</td><td>Smoke</td><td>x7k</td><td style="text-align:right">65</td><td>2026-06-20 19:07:41Z</td><td style="text-align:right">19,599 ms (19s)</td><td style="text-align:right">-</td><td style="text-align:right">778,816</td><td>jac313_store_005_XS (none/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/fedora-44/gcc16/Release/x7k/Smoke/textual/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-005 · Fedora Linux 44 (Server Edition) (6.19.10-300.fc44.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 98 GB RAM · vda (x7k label)</td></tr>
</tbody>
</table>

<table>
<thead>
<tr><th>OS</th><th>Compiler</th><th>Build</th><th>Modules</th><th>Size</th><th>Disk</th><th>Group</th><th>Run UTC</th><th>Duration</th><th>Build (modules)</th><th>Peak ops/sec</th><th>Peak scenario</th><th>Passed</th><th>Failed</th><th>Run</th></tr>
</thead>
<tbody>
<tr><td>fedora-44</td><td>gcc16</td><td>Release</td><td>modules</td><td>xFull</td><td>x7k</td><td style="text-align:right">64</td><td>2026-06-20 18:53:53Z</td><td style="text-align:right">612,307 ms (10m 12s)</td><td style="text-align:right">36,353 ms (36s)</td><td style="text-align:right">7,873,520</td><td>jac313_store_008_XS (flags/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/fedora-44/gcc16/Release/x7k/xFull/modules/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-005 · Fedora Linux 44 (Server Edition) (6.19.10-300.fc44.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 98 GB RAM · vda (x7k label)</td></tr>
<tr><td>fedora-44</td><td>gcc16</td><td>Release</td><td>textual</td><td>xFull</td><td>x7k</td><td style="text-align:right">66</td><td>2026-06-20 19:08:36Z</td><td style="text-align:right">613,058 ms (10m 13s)</td><td style="text-align:right">-</td><td style="text-align:right">5,944,066</td><td>jac313_store_008_TS (flags/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/fedora-44/gcc16/Release/x7k/xFull/textual/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-005 · Fedora Linux 44 (Server Edition) (6.19.10-300.fc44.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 98 GB RAM · vda (x7k label)</td></tr>
<tr><td>linuxmint-22.3</td><td>clang</td><td>Debug</td><td>modules</td><td>Smoke</td><td>x7k</td><td style="text-align:right">67</td><td>2026-06-20 22:54:29Z</td><td style="text-align:right">22,941 ms (22s)</td><td style="text-align:right">64,604 ms (1m 4s)</td><td style="text-align:right">306,560</td><td>jac313_store_005_TS (none/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/linuxmint-22.3/clang/Debug/x7k/Smoke/modules/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-004 · Linux Mint 22.3 (6.17.0-35-generic) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 31 GB RAM · vda (x7k label)</td></tr>
<tr><td>linuxmint-22.3</td><td>clang</td><td>Debug</td><td>textual</td><td>Smoke</td><td>x7k</td><td style="text-align:right">68</td><td>2026-06-20 22:59:14Z</td><td style="text-align:right">23,187 ms (23s)</td><td style="text-align:right">-</td><td style="text-align:right">343,289</td><td>jac313_store_007_TS (none/on)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/linuxmint-22.3/clang/Debug/x7k/Smoke/textual/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-004 · Linux Mint 22.3 (6.17.0-35-generic) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 31 GB RAM · vda (x7k label)</td></tr>
<tr><td>linuxmint-22.3</td><td>clang</td><td>Debug</td><td>modules</td><td>xFull</td><td>x7k</td><td style="text-align:right">75</td><td>2026-06-20 23:22:29Z</td><td style="text-align:right">662,456 ms (11m 2s)</td><td style="text-align:right">80,510 ms (1m 20s)</td><td style="text-align:right">1,210,742</td><td>jac313_store_005_XS (none/on)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/linuxmint-22.3/clang/Debug/x7k/xFull/modules/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-004 · Linux Mint 22.3 (6.17.0-35-generic) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 31 GB RAM · vda (x7k label)</td></tr>
<tr><td>linuxmint-22.3</td><td>clang</td><td>Debug</td><td>textual</td><td>xFull</td><td>x7k</td><td style="text-align:right">76</td><td>2026-06-20 23:37:03Z</td><td style="text-align:right">660,674 ms (11m)</td><td style="text-align:right">-</td><td style="text-align:right">1,192,323</td><td>jac313_store_008_XS (flags/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/linuxmint-22.3/clang/Debug/x7k/xFull/textual/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-004 · Linux Mint 22.3 (6.17.0-35-generic) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 31 GB RAM · vda (x7k label)</td></tr>
</tbody>
</table>

<table>
<thead>
<tr><th>OS</th><th>Compiler</th><th>Build</th><th>Modules</th><th>Size</th><th>Disk</th><th>Group</th><th>Run UTC</th><th>Duration</th><th>Build (modules)</th><th>Peak ops/sec</th><th>Peak scenario</th><th>Passed</th><th>Failed</th><th>Run</th></tr>
</thead>
<tbody>
<tr><td>linuxmint-22.3</td><td>clang</td><td>Release</td><td>textual</td><td>Smoke</td><td>x7k</td><td style="text-align:right">70</td><td>2026-06-20 23:06:43Z</td><td style="text-align:right">21,644 ms (21s)</td><td style="text-align:right">-</td><td style="text-align:right">600,240</td><td>jac313_store_005_TS (none/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/linuxmint-22.3/clang/Release/x7k/Smoke/textual/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-004 · Linux Mint 22.3 (6.17.0-35-generic) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 31 GB RAM · vda (x7k label)</td></tr>
<tr><td>linuxmint-22.3</td><td>clang</td><td>Release</td><td>modules</td><td>Smoke</td><td>x7k</td><td style="text-align:right">69</td><td>2026-06-20 23:00:30Z</td><td style="text-align:right">21,635 ms (21s)</td><td style="text-align:right">101,634 ms (1m 41s)</td><td style="text-align:right">571,429</td><td>jac313_store_007_TS (none/on)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/linuxmint-22.3/clang/Release/x7k/Smoke/modules/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-004 · Linux Mint 22.3 (6.17.0-35-generic) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 31 GB RAM · vda (x7k label)</td></tr>
<tr><td>linuxmint-22.3</td><td>clang</td><td>Release</td><td>modules</td><td>xFull</td><td>x7k</td><td style="text-align:right">77</td><td>2026-06-20 23:48:56Z</td><td style="text-align:right">618,980 ms (10m 18s)</td><td style="text-align:right">97,025 ms (1m 37s)</td><td style="text-align:right">3,122,073</td><td>jac313_store_005_TS (none/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/linuxmint-22.3/clang/Release/x7k/xFull/modules/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-004 · Linux Mint 22.3 (6.17.0-35-generic) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 31 GB RAM · vda (x7k label)</td></tr>
<tr><td>linuxmint-22.3</td><td>clang</td><td>Release</td><td>textual</td><td>xFull</td><td>x7k</td><td style="text-align:right">78</td><td>2026-06-21 00:03:27Z</td><td style="text-align:right">619,569 ms (10m 19s)</td><td style="text-align:right">-</td><td style="text-align:right">3,258,815</td><td>jac313_store_005_XS (jtext/on)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/linuxmint-22.3/clang/Release/x7k/xFull/textual/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-004 · Linux Mint 22.3 (6.17.0-35-generic) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 31 GB RAM · vda (x7k label)</td></tr>
<tr><td>linuxmint-22.3</td><td>gcc15</td><td>Debug</td><td>modules</td><td>Smoke</td><td>x7k</td><td style="text-align:right">71</td><td>2026-06-20 23:07:56Z</td><td style="text-align:right">24,125 ms (24s)</td><td style="text-align:right">68,207 ms (1m 8s)</td><td style="text-align:right">384,911</td><td>jac313_store_005_XS (none/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/linuxmint-22.3/gcc15/Debug/x7k/Smoke/modules/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-004 · Linux Mint 22.3 (6.17.0-35-generic) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 31 GB RAM · vda (x7k label)</td></tr>
<tr><td>linuxmint-22.3</td><td>gcc15</td><td>Debug</td><td>textual</td><td>Smoke</td><td>x7k</td><td style="text-align:right">72</td><td>2026-06-20 23:12:51Z</td><td style="text-align:right">23,773 ms (23s)</td><td style="text-align:right">-</td><td style="text-align:right">385,356</td><td>jac313_store_007_XS (none/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/linuxmint-22.3/gcc15/Debug/x7k/Smoke/textual/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-004 · Linux Mint 22.3 (6.17.0-35-generic) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 31 GB RAM · vda (x7k label)</td></tr>
</tbody>
</table>

<table>
<thead>
<tr><th>OS</th><th>Compiler</th><th>Build</th><th>Modules</th><th>Size</th><th>Disk</th><th>Group</th><th>Run UTC</th><th>Duration</th><th>Build (modules)</th><th>Peak ops/sec</th><th>Peak scenario</th><th>Passed</th><th>Failed</th><th>Run</th></tr>
</thead>
<tbody>
<tr><td>linuxmint-22.3</td><td>gcc15</td><td>Debug</td><td>modules</td><td>xFull</td><td>x7k</td><td style="text-align:right">79</td><td>2026-06-21 00:14:36Z</td><td style="text-align:right">658,831 ms (10m 58s)</td><td style="text-align:right">68,630 ms (1m 8s)</td><td style="text-align:right">1,414,547</td><td>jac313_store_007_XS (binary/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/linuxmint-22.3/gcc15/Debug/x7k/xFull/modules/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-004 · Linux Mint 22.3 (6.17.0-35-generic) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 31 GB RAM · vda (x7k label)</td></tr>
<tr><td>linuxmint-22.3</td><td>gcc15</td><td>Debug</td><td>textual</td><td>xFull</td><td>x7k</td><td style="text-align:right">80</td><td>2026-06-21 00:28:45Z</td><td style="text-align:right">657,796 ms (10m 57s)</td><td style="text-align:right">-</td><td style="text-align:right">1,488,472</td><td>jac313_store_005_XS (jtext/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/linuxmint-22.3/gcc15/Debug/x7k/xFull/textual/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-004 · Linux Mint 22.3 (6.17.0-35-generic) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 31 GB RAM · vda (x7k label)</td></tr>
<tr><td>linuxmint-22.3</td><td>gcc15</td><td>Release</td><td>modules</td><td>Smoke</td><td>x7k</td><td style="text-align:right">73</td><td>2026-06-20 23:14:12Z</td><td style="text-align:right">22,406 ms (22s)</td><td style="text-align:right">115,876 ms (1m 55s)</td><td style="text-align:right">644,745</td><td>jac313_store_005_XS (none/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/linuxmint-22.3/gcc15/Release/x7k/Smoke/modules/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-004 · Linux Mint 22.3 (6.17.0-35-generic) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 31 GB RAM · vda (x7k label)</td></tr>
<tr><td>linuxmint-22.3</td><td>gcc15</td><td>Release</td><td>textual</td><td>Smoke</td><td>x7k</td><td style="text-align:right">74</td><td>2026-06-20 23:21:09Z</td><td style="text-align:right">22,682 ms (22s)</td><td style="text-align:right">-</td><td style="text-align:right">589,275</td><td>jac313_store_005_XS (none/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/linuxmint-22.3/gcc15/Release/x7k/Smoke/textual/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-004 · Linux Mint 22.3 (6.17.0-35-generic) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 31 GB RAM · vda (x7k label)</td></tr>
<tr><td>linuxmint-22.3</td><td>gcc15</td><td>Release</td><td>modules</td><td>xFull</td><td>x7k</td><td style="text-align:right">81</td><td>2026-06-21 00:40:33Z</td><td style="text-align:right">620,173 ms (10m 20s)</td><td style="text-align:right">107,375 ms (1m 47s)</td><td style="text-align:right">3,234,487</td><td>jac313_store_008_XS (flags/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/linuxmint-22.3/gcc15/Release/x7k/xFull/modules/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-004 · Linux Mint 22.3 (6.17.0-35-generic) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 31 GB RAM · vda (x7k label)</td></tr>
<tr><td>linuxmint-22.3</td><td>gcc15</td><td>Release</td><td>textual</td><td>xFull</td><td>x7k</td><td style="text-align:right">82</td><td>2026-06-21 00:55:21Z</td><td style="text-align:right">618,446 ms (10m 18s)</td><td style="text-align:right">-</td><td style="text-align:right">3,143,962</td><td>jac313_store_005_XS (none/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/linuxmint-22.3/gcc15/Release/x7k/xFull/textual/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-004 · Linux Mint 22.3 (6.17.0-35-generic) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 31 GB RAM · vda (x7k label)</td></tr>
</tbody>
</table>

<table>
<thead>
<tr><th>OS</th><th>Compiler</th><th>Build</th><th>Modules</th><th>Size</th><th>Disk</th><th>Group</th><th>Run UTC</th><th>Duration</th><th>Build (modules)</th><th>Peak ops/sec</th><th>Peak scenario</th><th>Passed</th><th>Failed</th><th>Run</th></tr>
</thead>
<tbody>
<tr><td>rhel-9.8</td><td>clang</td><td>Debug</td><td>modules</td><td>Smoke</td><td>10k</td><td style="text-align:right">2</td><td>2026-06-20 04:34:11Z</td><td style="text-align:right">17,955 ms (17s)</td><td style="text-align:right">10,819 ms (10s)</td><td style="text-align:right">641,026</td><td>jac313_store_005_TS (none/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/clang/Debug/10k/Smoke/modules/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · HUC101812CSS200 (10k label)</td></tr>
<tr><td>rhel-9.8</td><td>clang</td><td>Debug</td><td>textual</td><td>Smoke</td><td>10k</td><td style="text-align:right">3</td><td>2026-06-20 04:35:43Z</td><td style="text-align:right">18,175 ms (18s)</td><td style="text-align:right">-</td><td style="text-align:right">654,450</td><td>jac313_store_007_XS (none/on)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/clang/Debug/10k/Smoke/textual/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · HUC101812CSS200 (10k label)</td></tr>
<tr><td>rhel-9.8</td><td>clang</td><td>Debug</td><td>modules</td><td>Smoke</td><td>ssd</td><td style="text-align:right">34</td><td>2026-06-20 08:28:24Z</td><td style="text-align:right">15,938 ms (15s)</td><td style="text-align:right">12,134 ms (12s)</td><td style="text-align:right">768,049</td><td>jac313_store_005_XS (none/on)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/clang/Debug/ssd/Smoke/modules/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · CT1000BX500SSD1 (ssd label)</td></tr>
<tr><td>rhel-9.8</td><td>clang</td><td>Debug</td><td>textual</td><td>Smoke</td><td>ssd</td><td style="text-align:right">35</td><td>2026-06-20 08:29:27Z</td><td style="text-align:right">16,047 ms (16s)</td><td style="text-align:right">-</td><td style="text-align:right">703,730</td><td>jac313_store_007_XS (none/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/clang/Debug/ssd/Smoke/textual/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · CT1000BX500SSD1 (ssd label)</td></tr>
<tr><td>rhel-9.8</td><td>clang</td><td>Debug</td><td>textual</td><td>Smoke</td><td>x7k</td><td style="text-align:right">19</td><td>2026-06-20 06:32:57Z</td><td style="text-align:right">18,875 ms (18s)</td><td style="text-align:right">-</td><td style="text-align:right">685,401</td><td>jac313_store_005_TS (none/on)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/clang/Debug/x7k/Smoke/textual/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · ST6000NM0095 (x7k label)</td></tr>
<tr><td>rhel-9.8</td><td>clang</td><td>Debug</td><td>modules</td><td>Smoke</td><td>x7k</td><td style="text-align:right">18</td><td>2026-06-20 06:31:21Z</td><td style="text-align:right">19,639 ms (19s)</td><td style="text-align:right">11,071 ms (11s)</td><td style="text-align:right">692,042</td><td>jac313_store_007_XS (none/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/clang/Debug/x7k/Smoke/modules/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · ST6000NM0095 (x7k label)</td></tr>
</tbody>
</table>

<table>
<thead>
<tr><th>OS</th><th>Compiler</th><th>Build</th><th>Modules</th><th>Size</th><th>Disk</th><th>Group</th><th>Run UTC</th><th>Duration</th><th>Build (modules)</th><th>Peak ops/sec</th><th>Peak scenario</th><th>Passed</th><th>Failed</th><th>Run</th></tr>
</thead>
<tbody>
<tr><td>rhel-9.8</td><td>clang</td><td>Debug</td><td>modules</td><td>xFull</td><td>10k</td><td style="text-align:right">10</td><td>2026-06-20 04:45:40Z</td><td style="text-align:right">602,212 ms (10m 2s)</td><td style="text-align:right">10,521 ms (10s)</td><td style="text-align:right">5,387,321</td><td>jac313_store_008_XS (flags/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/clang/Debug/10k/xFull/modules/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · HUC101812CSS200 (10k label)</td></tr>
<tr><td>rhel-9.8</td><td>clang</td><td>Debug</td><td>textual</td><td>xFull</td><td>10k</td><td style="text-align:right">11</td><td>2026-06-20 04:56:42Z</td><td style="text-align:right">601,308 ms (10m 1s)</td><td style="text-align:right">-</td><td style="text-align:right">5,538,049</td><td>jac313_store_008_TS (flags/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/clang/Debug/10k/xFull/textual/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · HUC101812CSS200 (10k label)</td></tr>
<tr><td>rhel-9.8</td><td>clang</td><td>Debug</td><td>modules</td><td>xFull</td><td>ssd</td><td style="text-align:right">42</td><td>2026-06-20 08:36:01Z</td><td style="text-align:right">597,560 ms (9m 57s)</td><td style="text-align:right">10,623 ms (10s)</td><td style="text-align:right">5,335,325</td><td>jac313_store_005_TS (none/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/clang/Debug/ssd/xFull/modules/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · CT1000BX500SSD1 (ssd label)</td></tr>
<tr><td>rhel-9.8</td><td>clang</td><td>Debug</td><td>textual</td><td>xFull</td><td>ssd</td><td style="text-align:right">43</td><td>2026-06-20 08:46:29Z</td><td style="text-align:right">596,246 ms (9m 56s)</td><td style="text-align:right">-</td><td style="text-align:right">5,548,712</td><td>jac313_store_008_XS (flags/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/clang/Debug/ssd/xFull/textual/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · CT1000BX500SSD1 (ssd label)</td></tr>
<tr><td>rhel-9.8</td><td>clang</td><td>Debug</td><td>modules</td><td>xFull</td><td>x7k</td><td style="text-align:right">26</td><td>2026-06-20 06:42:46Z</td><td style="text-align:right">605,515 ms (10m 5s)</td><td style="text-align:right">10,371 ms (10s)</td><td style="text-align:right">5,367,398</td><td>jac313_store_008_XS (flags/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/clang/Debug/x7k/xFull/modules/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · ST6000NM0095 (x7k label)</td></tr>
<tr><td>rhel-9.8</td><td>clang</td><td>Debug</td><td>textual</td><td>xFull</td><td>x7k</td><td style="text-align:right">27</td><td>2026-06-20 06:53:46Z</td><td style="text-align:right">605,352 ms (10m 5s)</td><td style="text-align:right">-</td><td style="text-align:right">5,177,591</td><td>jac313_store_008_XS (flags/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/clang/Debug/x7k/xFull/textual/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · ST6000NM0095 (x7k label)</td></tr>
</tbody>
</table>

<table>
<thead>
<tr><th>OS</th><th>Compiler</th><th>Build</th><th>Modules</th><th>Size</th><th>Disk</th><th>Group</th><th>Run UTC</th><th>Duration</th><th>Build (modules)</th><th>Peak ops/sec</th><th>Peak scenario</th><th>Passed</th><th>Failed</th><th>Run</th></tr>
</thead>
<tbody>
<tr><td>rhel-9.8</td><td>clang</td><td>Release</td><td>modules</td><td>Smoke</td><td>10k</td><td style="text-align:right">4</td><td>2026-06-20 04:36:34Z</td><td style="text-align:right">16,791 ms (16s)</td><td style="text-align:right">14,779 ms (14s)</td><td style="text-align:right">1,218,027</td><td>jac313_store_005_TS (none/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/clang/Release/10k/Smoke/modules/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · HUC101812CSS200 (10k label)</td></tr>
<tr><td>rhel-9.8</td><td>clang</td><td>Release</td><td>textual</td><td>Smoke</td><td>10k</td><td style="text-align:right">5</td><td>2026-06-20 04:38:12Z</td><td style="text-align:right">16,143 ms (16s)</td><td style="text-align:right">-</td><td style="text-align:right">1,118,568</td><td>jac313_store_005_XS (none/on)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/clang/Release/10k/Smoke/textual/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · HUC101812CSS200 (10k label)</td></tr>
<tr><td>rhel-9.8</td><td>clang</td><td>Release</td><td>modules</td><td>Smoke</td><td>ssd</td><td style="text-align:right">36</td><td>2026-06-20 08:29:47Z</td><td style="text-align:right">14,934 ms (14s)</td><td style="text-align:right">16,807 ms (16s)</td><td style="text-align:right">1,173,709</td><td>jac313_store_007_XS (none/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/clang/Release/ssd/Smoke/modules/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · CT1000BX500SSD1 (ssd label)</td></tr>
<tr><td>rhel-9.8</td><td>clang</td><td>Release</td><td>textual</td><td>Smoke</td><td>ssd</td><td style="text-align:right">37</td><td>2026-06-20 08:31:03Z</td><td style="text-align:right">14,832 ms (14s)</td><td style="text-align:right">-</td><td style="text-align:right">1,210,654</td><td>jac313_store_005_XS (none/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/clang/Release/ssd/Smoke/textual/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · CT1000BX500SSD1 (ssd label)</td></tr>
<tr><td>rhel-9.8</td><td>clang</td><td>Release</td><td>modules</td><td>Smoke</td><td>x7k</td><td style="text-align:right">20</td><td>2026-06-20 06:33:46Z</td><td style="text-align:right">18,097 ms (18s)</td><td style="text-align:right">15,031 ms (15s)</td><td style="text-align:right">1,146,789</td><td>jac313_store_005_TS (none/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/clang/Release/x7k/Smoke/modules/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · ST6000NM0095 (x7k label)</td></tr>
<tr><td>rhel-9.8</td><td>clang</td><td>Release</td><td>textual</td><td>Smoke</td><td>x7k</td><td style="text-align:right">21</td><td>2026-06-20 06:35:26Z</td><td style="text-align:right">17,593 ms (17s)</td><td style="text-align:right">-</td><td style="text-align:right">1,084,599</td><td>jac313_store_005_XS (none/on)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/clang/Release/x7k/Smoke/textual/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · ST6000NM0095 (x7k label)</td></tr>
</tbody>
</table>

<table>
<thead>
<tr><th>OS</th><th>Compiler</th><th>Build</th><th>Modules</th><th>Size</th><th>Disk</th><th>Group</th><th>Run UTC</th><th>Duration</th><th>Build (modules)</th><th>Peak ops/sec</th><th>Peak scenario</th><th>Passed</th><th>Failed</th><th>Run</th></tr>
</thead>
<tbody>
<tr><td>rhel-9.8</td><td>clang</td><td>Release</td><td>textual</td><td>xFull</td><td>10k</td><td style="text-align:right">13</td><td>2026-06-20 05:17:51Z</td><td style="text-align:right">571,604 ms (9m 31s)</td><td style="text-align:right">-</td><td style="text-align:right">10,458,610</td><td>jac313_store_008_XS (flags/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/clang/Release/10k/xFull/textual/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · HUC101812CSS200 (10k label)</td></tr>
<tr><td>rhel-9.8</td><td>clang</td><td>Release</td><td>modules</td><td>xFull</td><td>10k</td><td style="text-align:right">12</td><td>2026-06-20 05:07:16Z</td><td style="text-align:right">564,925 ms (9m 24s)</td><td style="text-align:right">15,080 ms (15s)</td><td style="text-align:right">10,887,316</td><td>jac313_store_007_XS (binary/on)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/clang/Release/10k/xFull/modules/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · HUC101812CSS200 (10k label)</td></tr>
<tr><td>rhel-9.8</td><td>clang</td><td>Release</td><td>modules</td><td>xFull</td><td>ssd</td><td style="text-align:right">44</td><td>2026-06-20 08:56:29Z</td><td style="text-align:right">567,036 ms (9m 27s)</td><td style="text-align:right">14,780 ms (14s)</td><td style="text-align:right">13,027,619</td><td>jac313_store_007_XS (none/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/clang/Release/ssd/xFull/modules/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · CT1000BX500SSD1 (ssd label)</td></tr>
<tr><td>rhel-9.8</td><td>clang</td><td>Release</td><td>textual</td><td>xFull</td><td>ssd</td><td style="text-align:right">45</td><td>2026-06-20 09:06:36Z</td><td style="text-align:right">566,466 ms (9m 26s)</td><td style="text-align:right">-</td><td style="text-align:right">12,805,737</td><td>jac313_store_007_XS (none/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/clang/Release/ssd/xFull/textual/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · CT1000BX500SSD1 (ssd label)</td></tr>
<tr><td>rhel-9.8</td><td>clang</td><td>Release</td><td>modules</td><td>xFull</td><td>x7k</td><td style="text-align:right">28</td><td>2026-06-20 07:04:19Z</td><td style="text-align:right">571,400 ms (9m 31s)</td><td style="text-align:right">14,828 ms (14s)</td><td style="text-align:right">11,043,866</td><td>jac313_store_008_XS (flags/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/clang/Release/x7k/xFull/modules/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · ST6000NM0095 (x7k label)</td></tr>
<tr><td>rhel-9.8</td><td>clang</td><td>Release</td><td>textual</td><td>xFull</td><td>x7k</td><td style="text-align:right">29</td><td>2026-06-20 07:14:52Z</td><td style="text-align:right">573,077 ms (9m 33s)</td><td style="text-align:right">-</td><td style="text-align:right">12,743,074</td><td>jac313_store_008_TS (flags/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/clang/Release/x7k/xFull/textual/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · ST6000NM0095 (x7k label)</td></tr>
</tbody>
</table>

<table>
<thead>
<tr><th>OS</th><th>Compiler</th><th>Build</th><th>Modules</th><th>Size</th><th>Disk</th><th>Group</th><th>Run UTC</th><th>Duration</th><th>Build (modules)</th><th>Peak ops/sec</th><th>Peak scenario</th><th>Passed</th><th>Failed</th><th>Run</th></tr>
</thead>
<tbody>
<tr><td>rhel-9.8</td><td>gcc15</td><td>Debug</td><td>modules</td><td>Smoke</td><td>10k</td><td style="text-align:right">6</td><td>2026-06-20 04:38:59Z</td><td style="text-align:right">18,260 ms (18s)</td><td style="text-align:right">14,275 ms (14s)</td><td style="text-align:right">814,996</td><td>jac313_store_005_XS (none/on)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/gcc15/Debug/10k/Smoke/modules/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · HUC101812CSS200 (10k label)</td></tr>
<tr><td>rhel-9.8</td><td>gcc15</td><td>Debug</td><td>textual</td><td>Smoke</td><td>10k</td><td style="text-align:right">7</td><td>2026-06-20 04:41:07Z</td><td style="text-align:right">17,647 ms (17s)</td><td style="text-align:right">-</td><td style="text-align:right">803,213</td><td>jac313_store_007_XS (none/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/gcc15/Debug/10k/Smoke/textual/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · HUC101812CSS200 (10k label)</td></tr>
<tr><td>rhel-9.8</td><td>gcc15</td><td>Debug</td><td>modules</td><td>Smoke</td><td>ssd</td><td style="text-align:right">38</td><td>2026-06-20 08:31:22Z</td><td style="text-align:right">16,388 ms (16s)</td><td style="text-align:right">14,377 ms (14s)</td><td style="text-align:right">884,956</td><td>jac313_store_005_TS (none/on)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/gcc15/Debug/ssd/Smoke/modules/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · CT1000BX500SSD1 (ssd label)</td></tr>
<tr><td>rhel-9.8</td><td>gcc15</td><td>Debug</td><td>textual</td><td>Smoke</td><td>ssd</td><td style="text-align:right">39</td><td>2026-06-20 08:32:58Z</td><td style="text-align:right">16,337 ms (16s)</td><td style="text-align:right">-</td><td style="text-align:right">824,402</td><td>jac313_store_005_XS (none/on)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/gcc15/Debug/ssd/Smoke/textual/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · CT1000BX500SSD1 (ssd label)</td></tr>
<tr><td>rhel-9.8</td><td>gcc15</td><td>Debug</td><td>modules</td><td>Smoke</td><td>x7k</td><td style="text-align:right">22</td><td>2026-06-20 06:36:08Z</td><td style="text-align:right">19,601 ms (19s)</td><td style="text-align:right">14,480 ms (14s)</td><td style="text-align:right">767,460</td><td>jac313_store_005_XS (none/on)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/gcc15/Debug/x7k/Smoke/modules/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · ST6000NM0095 (x7k label)</td></tr>
<tr><td>rhel-9.8</td><td>gcc15</td><td>Debug</td><td>textual</td><td>Smoke</td><td>x7k</td><td style="text-align:right">23</td><td>2026-06-20 06:38:16Z</td><td style="text-align:right">20,314 ms (20s)</td><td style="text-align:right">-</td><td style="text-align:right">807,103</td><td>jac313_store_005_XS (none/on)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/gcc15/Debug/x7k/Smoke/textual/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · ST6000NM0095 (x7k label)</td></tr>
</tbody>
</table>

<table>
<thead>
<tr><th>OS</th><th>Compiler</th><th>Build</th><th>Modules</th><th>Size</th><th>Disk</th><th>Group</th><th>Run UTC</th><th>Duration</th><th>Build (modules)</th><th>Peak ops/sec</th><th>Peak scenario</th><th>Passed</th><th>Failed</th><th>Run</th></tr>
</thead>
<tbody>
<tr><td>rhel-9.8</td><td>gcc15</td><td>Debug</td><td>modules</td><td>xFull</td><td>10k</td><td style="text-align:right">14</td><td>2026-06-20 05:27:57Z</td><td style="text-align:right">607,537 ms (10m 7s)</td><td style="text-align:right">14,529 ms (14s)</td><td style="text-align:right">6,045,584</td><td>jac313_store_005_XS (none/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/gcc15/Debug/10k/xFull/modules/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · HUC101812CSS200 (10k label)</td></tr>
<tr><td>rhel-9.8</td><td>gcc15</td><td>Debug</td><td>textual</td><td>xFull</td><td>10k</td><td style="text-align:right">15</td><td>2026-06-20 05:39:42Z</td><td style="text-align:right">605,446 ms (10m 5s)</td><td style="text-align:right">-</td><td style="text-align:right">6,563,489</td><td>jac313_store_008_TS (flags/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/gcc15/Debug/10k/xFull/textual/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · HUC101812CSS200 (10k label)</td></tr>
<tr><td>rhel-9.8</td><td>gcc15</td><td>Debug</td><td>modules</td><td>xFull</td><td>ssd</td><td style="text-align:right">46</td><td>2026-06-20 09:16:06Z</td><td style="text-align:right">603,647 ms (10m 3s)</td><td style="text-align:right">14,478 ms (14s)</td><td style="text-align:right">6,276,479</td><td>jac313_store_008_TS (flags/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/gcc15/Debug/ssd/xFull/modules/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · CT1000BX500SSD1 (ssd label)</td></tr>
<tr><td>rhel-9.8</td><td>gcc15</td><td>Debug</td><td>textual</td><td>xFull</td><td>ssd</td><td style="text-align:right">47</td><td>2026-06-20 09:27:28Z</td><td style="text-align:right">605,486 ms (10m 5s)</td><td style="text-align:right">-</td><td style="text-align:right">6,264,094</td><td>jac313_store_005_XS (none/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/gcc15/Debug/ssd/xFull/textual/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · CT1000BX500SSD1 (ssd label)</td></tr>
<tr><td>rhel-9.8</td><td>gcc15</td><td>Debug</td><td>modules</td><td>xFull</td><td>x7k</td><td style="text-align:right">30</td><td>2026-06-20 07:24:50Z</td><td style="text-align:right">610,959 ms (10m 10s)</td><td style="text-align:right">14,278 ms (14s)</td><td style="text-align:right">6,744,908</td><td>jac313_store_008_TS (flags/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/gcc15/Debug/x7k/xFull/modules/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · ST6000NM0095 (x7k label)</td></tr>
<tr><td>rhel-9.8</td><td>gcc15</td><td>Debug</td><td>textual</td><td>xFull</td><td>x7k</td><td style="text-align:right">31</td><td>2026-06-20 07:36:38Z</td><td style="text-align:right">608,847 ms (10m 8s)</td><td style="text-align:right">-</td><td style="text-align:right">6,572,418</td><td>jac313_store_008_XS (flags/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/gcc15/Debug/x7k/xFull/textual/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · ST6000NM0095 (x7k label)</td></tr>
</tbody>
</table>

<table>
<thead>
<tr><th>OS</th><th>Compiler</th><th>Build</th><th>Modules</th><th>Size</th><th>Disk</th><th>Group</th><th>Run UTC</th><th>Duration</th><th>Build (modules)</th><th>Peak ops/sec</th><th>Peak scenario</th><th>Passed</th><th>Failed</th><th>Run</th></tr>
</thead>
<tbody>
<tr><td>rhel-9.8</td><td>gcc15</td><td>Release</td><td>modules</td><td>Smoke</td><td>10k</td><td style="text-align:right">8</td><td>2026-06-20 04:41:58Z</td><td style="text-align:right">15,992 ms (15s)</td><td style="text-align:right">25,999 ms (25s)</td><td style="text-align:right">1,210,654</td><td>jac313_store_007_XS (none/on)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/gcc15/Release/10k/Smoke/modules/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · HUC101812CSS200 (10k label)</td></tr>
<tr><td>rhel-9.8</td><td>gcc15</td><td>Release</td><td>textual</td><td>Smoke</td><td>10k</td><td style="text-align:right">9</td><td>2026-06-20 04:44:52Z</td><td style="text-align:right">16,139 ms (16s)</td><td style="text-align:right">-</td><td style="text-align:right">1,230,012</td><td>jac313_store_007_XS (none/on)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/gcc15/Release/10k/Smoke/textual/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · HUC101812CSS200 (10k label)</td></tr>
<tr><td>rhel-9.8</td><td>gcc15</td><td>Release</td><td>modules</td><td>Smoke</td><td>ssd</td><td style="text-align:right">40</td><td>2026-06-20 08:33:18Z</td><td style="text-align:right">14,882 ms (14s)</td><td style="text-align:right">25,647 ms (25s)</td><td style="text-align:right">1,182,033</td><td>jac313_store_007_TS (none/on)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/gcc15/Release/ssd/Smoke/modules/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · CT1000BX500SSD1 (ssd label)</td></tr>
<tr><td>rhel-9.8</td><td>gcc15</td><td>Release</td><td>textual</td><td>Smoke</td><td>ssd</td><td style="text-align:right">41</td><td>2026-06-20 08:35:42Z</td><td style="text-align:right">15,033 ms (15s)</td><td style="text-align:right">-</td><td style="text-align:right">1,349,528</td><td>jac313_store_005_XS (none/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/gcc15/Release/ssd/Smoke/textual/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · CT1000BX500SSD1 (ssd label)</td></tr>
<tr><td>rhel-9.8</td><td>gcc15</td><td>Release</td><td>modules</td><td>Smoke</td><td>x7k</td><td style="text-align:right">24</td><td>2026-06-20 06:39:05Z</td><td style="text-align:right">17,496 ms (17s)</td><td style="text-align:right">26,103 ms (26s)</td><td style="text-align:right">1,150,748</td><td>jac313_store_007_TS (none/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/gcc15/Release/x7k/Smoke/modules/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · ST6000NM0095 (x7k label)</td></tr>
<tr><td>rhel-9.8</td><td>gcc15</td><td>Release</td><td>textual</td><td>Smoke</td><td>x7k</td><td style="text-align:right">25</td><td>2026-06-20 06:42:00Z</td><td style="text-align:right">17,497 ms (17s)</td><td style="text-align:right">-</td><td style="text-align:right">1,170,960</td><td>jac313_store_007_XS (none/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/gcc15/Release/x7k/Smoke/textual/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · ST6000NM0095 (x7k label)</td></tr>
</tbody>
</table>

<table>
<thead>
<tr><th>OS</th><th>Compiler</th><th>Build</th><th>Modules</th><th>Size</th><th>Disk</th><th>Group</th><th>Run UTC</th><th>Duration</th><th>Build (modules)</th><th>Peak ops/sec</th><th>Peak scenario</th><th>Passed</th><th>Failed</th><th>Run</th></tr>
</thead>
<tbody>
<tr><td>rhel-9.8</td><td>gcc15</td><td>Release</td><td>modules</td><td>xFull</td><td>10k</td><td style="text-align:right">16</td><td>2026-06-20 05:50:18Z</td><td style="text-align:right">571,491 ms (9m 31s)</td><td style="text-align:right">25,700 ms (25s)</td><td style="text-align:right">10,577,533</td><td>jac313_store_008_XS (flags/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/gcc15/Release/10k/xFull/modules/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · HUC101812CSS200 (10k label)</td></tr>
<tr><td>rhel-9.8</td><td>gcc15</td><td>Release</td><td>textual</td><td>xFull</td><td>10k</td><td style="text-align:right">17</td><td>2026-06-20 06:02:01Z</td><td style="text-align:right">570,848 ms (9m 30s)</td><td style="text-align:right">-</td><td style="text-align:right">11,085,246</td><td>jac313_store_008_TS (flags/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/gcc15/Release/10k/xFull/textual/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · HUC101812CSS200 (10k label)</td></tr>
<tr><td>rhel-9.8</td><td>gcc15</td><td>Release</td><td>modules</td><td>xFull</td><td>ssd</td><td style="text-align:right">48</td><td>2026-06-20 09:37:36Z</td><td style="text-align:right">569,837 ms (9m 29s)</td><td style="text-align:right">25,751 ms (25s)</td><td style="text-align:right">10,736,526</td><td>jac313_store_008_TS (flags/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/gcc15/Release/ssd/xFull/modules/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · CT1000BX500SSD1 (ssd label)</td></tr>
<tr><td>rhel-9.8</td><td>gcc15</td><td>Release</td><td>textual</td><td>xFull</td><td>ssd</td><td style="text-align:right">49</td><td>2026-06-20 09:49:21Z</td><td style="text-align:right">567,021 ms (9m 27s)</td><td style="text-align:right">-</td><td style="text-align:right">11,087,704</td><td>jac313_store_007_XS (none/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/gcc15/Release/ssd/xFull/textual/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · CT1000BX500SSD1 (ssd label)</td></tr>
<tr><td>rhel-9.8</td><td>gcc15</td><td>Release</td><td>modules</td><td>xFull</td><td>x7k</td><td style="text-align:right">32</td><td>2026-06-20 07:47:19Z</td><td style="text-align:right">573,784 ms (9m 33s)</td><td style="text-align:right">25,898 ms (25s)</td><td style="text-align:right">10,598,385</td><td>jac313_store_008_XS (flags/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/gcc15/Release/x7k/xFull/modules/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · ST6000NM0095 (x7k label)</td></tr>
<tr><td>rhel-9.8</td><td>gcc15</td><td>Release</td><td>textual</td><td>xFull</td><td>x7k</td><td style="text-align:right">33</td><td>2026-06-20 07:59:03Z</td><td style="text-align:right">573,187 ms (9m 33s)</td><td style="text-align:right">-</td><td style="text-align:right">11,534,824</td><td>jac313_store_008_XS (flags/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/gcc15/Release/x7k/xFull/textual/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · ST6000NM0095 (x7k label)</td></tr>
</tbody>
</table>

## Persist backend comparison (latest xFull per compiler & build type)

Benchmark tests 005–008: **run ms**, **peak ops/sec**, and **persist log size** for binary vs jText vs SQL.

### almalinux-10.2 · clang (xFull, Debug, run 92)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_005_TS | off | binary | 725 | 1,973,048 | 8.38 MiB |
|  |  | jtext | 1,530 | 1,771,322 | 7.92 MiB |
|  |  | sql | 723 | 1,511,076 | 7.42 MiB |
| jac313_store_005_TS | on | binary | 818 | 1,555,403 | 8.38 MiB |
|  |  | jtext | 1,573 | 1,693,796 | 7.92 MiB |
|  |  | sql | 1,239 | 1,364,201 | 7.42 MiB |
| jac313_store_005_XS | off | binary | 777 | 1,709,723 | 8.38 MiB |
|  |  | jtext | 1,583 | 1,796,203 | 7.44 MiB |
|  |  | sql | 668 | 1,821,129 | 7.13 MiB |
| jac313_store_005_XS | on | binary | 774 | 1,730,044 | 8.38 MiB |
|  |  | jtext | 1,403 | 1,731,692 | 7.44 MiB |
|  |  | sql | 1,141 | 1,677,486 | 7.13 MiB |
| jac313_store_006_TS | off | binary | 35,467 | - | 19.81 MiB |
|  |  | jtext | 35,569 | - | 20.42 MiB |
|  |  | sql | 34,885 | - | 23.51 MiB |
| jac313_store_006_TS | on | binary | 39,388 | - | 19.81 MiB |
|  |  | jtext | 39,934 | - | 20.42 MiB |
|  |  | sql | 39,379 | - | 23.51 MiB |
| jac313_store_006_XS | off | binary | 34,523 | - | 19.81 MiB |
|  |  | jtext | 34,873 | - | 19.96 MiB |
|  |  | sql | 34,618 | - | 22.64 MiB |
| jac313_store_006_XS | on | binary | 38,176 | - | 19.81 MiB |
|  |  | jtext | 38,588 | - | 19.96 MiB |
|  |  | sql | 38,432 | - | 22.64 MiB |
| jac313_store_007_TS | off | binary | 877 | 1,668,363 | 19.82 MiB |
|  |  | jtext | 3,884 | 1,723,009 | 20.45 MiB |
|  |  | sql | 1,097 | 1,583,707 | 23.51 MiB |
| jac313_store_007_TS | on | binary | 884 | 1,629,593 | 19.82 MiB |
|  |  | jtext | 3,901 | 1,507,136 | 20.45 MiB |
|  |  | sql | 2,063 | 1,567,349 | 23.51 MiB |
| jac313_store_007_XS | off | binary | 832 | 1,669,867 | 19.82 MiB |
|  |  | jtext | 3,276 | 1,807,828 | 19.98 MiB |
|  |  | sql | 1,203 | 1,604,287 | 22.66 MiB |
| jac313_store_007_XS | on | binary | 830 | 1,829,659 | 19.82 MiB |
|  |  | jtext | 3,916 | 1,815,904 | 19.98 MiB |
|  |  | sql | 1,944 | 1,550,027 | 22.66 MiB |

### almalinux-10.2 · clang (xFull, Release, run 94)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_005_TS | off | binary | 410 | 3,096,263 | 8.38 MiB |
|  |  | jtext | 415 | 2,763,347 | 7.91 MiB |
|  |  | sql | 407 | 3,446,256 | 7.42 MiB |
| jac313_store_005_TS | on | binary | 405 | 2,924,575 | 8.38 MiB |
|  |  | jtext | 354 | 2,630,471 | 7.91 MiB |
|  |  | sql | 720 | 2,830,135 | 7.42 MiB |
| jac313_store_005_XS | off | binary | 415 | 2,706,360 | 8.38 MiB |
|  |  | jtext | 411 | 2,928,000 | 7.44 MiB |
|  |  | sql | 410 | 2,977,963 | 7.13 MiB |
| jac313_store_005_XS | on | binary | 411 | 2,577,320 | 8.38 MiB |
|  |  | jtext | 361 | 2,592,823 | 7.44 MiB |
|  |  | sql | 827 | 2,588,461 | 7.13 MiB |
| jac313_store_006_TS | off | binary | 34,731 | - | 19.81 MiB |
|  |  | jtext | 34,971 | - | 20.41 MiB |
|  |  | sql | 35,368 | - | 23.50 MiB |
| jac313_store_006_TS | on | binary | 38,627 | - | 19.81 MiB |
|  |  | jtext | 38,662 | - | 20.40 MiB |
|  |  | sql | 39,258 | - | 23.50 MiB |
| jac313_store_006_XS | off | binary | 34,643 | - | 19.81 MiB |
|  |  | jtext | 34,588 | - | 19.96 MiB |
|  |  | sql | 34,368 | - | 22.63 MiB |
| jac313_store_006_XS | on | binary | 38,110 | - | 19.81 MiB |
|  |  | jtext | 37,882 | - | 19.96 MiB |
|  |  | sql | 38,437 | - | 22.63 MiB |
| jac313_store_007_TS | off | binary | 504 | 2,942,475 | 19.82 MiB |
|  |  | jtext | 897 | 2,884,921 | 20.45 MiB |
|  |  | sql | 792 | 2,780,713 | 23.51 MiB |
| jac313_store_007_TS | on | binary | 569 | 2,521,305 | 19.82 MiB |
|  |  | jtext | 793 | 2,620,064 | 20.45 MiB |
|  |  | sql | 1,744 | 2,885,920 | 23.51 MiB |
| jac313_store_007_XS | off | binary | 512 | 3,441,156 | 19.82 MiB |
|  |  | jtext | 831 | 3,548,616 | 19.98 MiB |
|  |  | sql | 786 | 3,064,664 | 22.64 MiB |
| jac313_store_007_XS | on | binary | 576 | 2,641,450 | 19.82 MiB |
|  |  | jtext | 1,087 | 3,054,181 | 19.98 MiB |
|  |  | sql | 1,625 | 2,662,619 | 22.64 MiB |

### almalinux-10.2 · gcc15 (xFull, Debug, run 96)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_005_TS | off | binary | 717 | 1,978,866 | 8.38 MiB |
|  |  | jtext | 1,631 | 1,898,614 | 7.92 MiB |
|  |  | sql | 669 | 1,962,015 | 7.42 MiB |
| jac313_store_005_TS | on | binary | 662 | 2,172,166 | 8.38 MiB |
|  |  | jtext | 1,630 | 2,035,831 | 7.92 MiB |
|  |  | sql | 1,170 | 2,052,503 | 7.42 MiB |
| jac313_store_005_XS | off | binary | 713 | 1,873,045 | 8.38 MiB |
|  |  | jtext | 1,611 | 1,931,136 | 7.44 MiB |
|  |  | sql | 618 | 2,091,525 | 7.13 MiB |
| jac313_store_005_XS | on | binary | 673 | 2,021,264 | 8.38 MiB |
|  |  | jtext | 1,557 | 2,206,775 | 7.44 MiB |
|  |  | sql | 976 | 2,211,704 | 7.13 MiB |
| jac313_store_006_TS | off | binary | 35,385 | - | 19.81 MiB |
|  |  | jtext | 35,772 | - | 20.42 MiB |
|  |  | sql | 35,209 | - | 23.50 MiB |
| jac313_store_006_TS | on | binary | 38,979 | - | 19.81 MiB |
|  |  | jtext | 39,853 | - | 20.42 MiB |
|  |  | sql | 39,159 | - | 23.50 MiB |
| jac313_store_006_XS | off | binary | 34,627 | - | 19.81 MiB |
|  |  | jtext | 35,099 | - | 19.96 MiB |
|  |  | sql | 34,582 | - | 22.64 MiB |
| jac313_store_006_XS | on | binary | 38,116 | - | 19.81 MiB |
|  |  | jtext | 38,844 | - | 19.96 MiB |
|  |  | sql | 38,373 | - | 22.64 MiB |
| jac313_store_007_TS | off | binary | 825 | 1,835,132 | 19.82 MiB |
|  |  | jtext | 3,801 | 1,902,334 | 20.45 MiB |
|  |  | sql | 1,031 | 1,747,122 | 23.52 MiB |
| jac313_store_007_TS | on | binary | 823 | 1,964,482 | 19.82 MiB |
|  |  | jtext | 3,906 | 1,900,743 | 20.45 MiB |
|  |  | sql | 1,784 | 2,032,644 | 23.52 MiB |
| jac313_store_007_XS | off | binary | 781 | 2,137,117 | 19.82 MiB |
|  |  | jtext | 4,074 | 2,201,916 | 19.98 MiB |
|  |  | sql | 1,034 | 1,876,560 | 22.65 MiB |
| jac313_store_007_XS | on | binary | 820 | 1,759,108 | 19.82 MiB |
|  |  | jtext | 3,795 | 2,231,944 | 19.98 MiB |
|  |  | sql | 1,793 | 2,180,027 | 22.65 MiB |

### almalinux-10.2 · gcc15 (xFull, Release, run 98)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_005_TS | off | binary | 410 | 2,726,653 | 8.38 MiB |
|  |  | jtext | 464 | 2,585,115 | 7.91 MiB |
|  |  | sql | 412 | 2,678,667 | 7.42 MiB |
| jac313_store_005_TS | on | binary | 413 | 3,064,664 | 8.38 MiB |
|  |  | jtext | 414 | 2,881,595 | 7.91 MiB |
|  |  | sql | 837 | 2,632,618 | 7.42 MiB |
| jac313_store_005_XS | off | binary | 407 | 2,574,466 | 8.38 MiB |
|  |  | jtext | 360 | 2,692,805 | 7.44 MiB |
|  |  | sql | 356 | 2,391,544 | 7.13 MiB |
| jac313_store_005_XS | on | binary | 412 | 2,566,669 | 8.38 MiB |
|  |  | jtext | 407 | 2,647,954 | 7.44 MiB |
|  |  | sql | 779 | 3,197,953 | 7.13 MiB |
| jac313_store_006_TS | off | binary | 35,144 | - | 19.81 MiB |
|  |  | jtext | 35,337 | - | 20.40 MiB |
|  |  | sql | 34,957 | - | 23.50 MiB |
| jac313_store_006_TS | on | binary | 38,962 | - | 19.81 MiB |
|  |  | jtext | 38,960 | - | 20.40 MiB |
|  |  | sql | 39,253 | - | 23.50 MiB |
| jac313_store_006_XS | off | binary | 34,353 | - | 19.81 MiB |
|  |  | jtext | 34,179 | - | 19.96 MiB |
|  |  | sql | 34,327 | - | 22.63 MiB |
| jac313_store_006_XS | on | binary | 37,832 | - | 19.81 MiB |
|  |  | jtext | 38,181 | - | 19.96 MiB |
|  |  | sql | 38,212 | - | 22.63 MiB |
| jac313_store_007_TS | off | binary | 514 | 3,066,168 | 19.82 MiB |
|  |  | jtext | 841 | 3,103,470 | 20.44 MiB |
|  |  | sql | 774 | 2,828,854 | 23.50 MiB |
| jac313_store_007_TS | on | binary | 559 | 2,791,503 | 19.82 MiB |
|  |  | jtext | 788 | 3,730,230 | 20.44 MiB |
|  |  | sql | 1,836 | 2,873,481 | 23.50 MiB |
| jac313_store_007_XS | off | binary | 506 | 3,823,946 | 19.82 MiB |
|  |  | jtext | 787 | 3,139,028 | 19.98 MiB |
|  |  | sql | 727 | 3,188,267 | 22.64 MiB |
| jac313_store_007_XS | on | binary | 510 | 3,013,228 | 19.82 MiB |
|  |  | jtext | 1,058 | 3,819,710 | 19.98 MiB |
|  |  | sql | 1,482 | 2,696,726 | 22.64 MiB |

### fedora-44 · clang (xFull, Debug, run 53)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_005_TS | off | binary | 804 | 2,029,797 | 8.38 MiB |
|  |  | jtext | 1,467 | 2,294,683 | 7.92 MiB |
|  |  | sql | 659 | 1,873,326 | 7.42 MiB |
| jac313_store_005_TS | on | binary | 758 | 1,860,257 | 8.38 MiB |
|  |  | jtext | 1,379 | 1,976,324 | 7.92 MiB |
|  |  | sql | 1,007 | 2,061,983 | 7.42 MiB |
| jac313_store_005_XS | off | binary | 761 | 2,482,437 | 8.38 MiB |
|  |  | jtext | 1,488 | 2,122,737 | 7.44 MiB |
|  |  | sql | 603 | 2,210,482 | 7.13 MiB |
| jac313_store_005_XS | on | binary | 755 | 2,126,302 | 8.38 MiB |
|  |  | jtext | 1,221 | 2,378,970 | 7.44 MiB |
|  |  | sql | 1,060 | 2,008,032 | 7.13 MiB |
| jac313_store_006_TS | off | binary | 34,234 | - | 19.81 MiB |
|  |  | jtext | 34,533 | - | 20.42 MiB |
|  |  | sql | 34,286 | - | 23.50 MiB |
| jac313_store_006_TS | on | binary | 38,061 | - | 19.81 MiB |
|  |  | jtext | 38,356 | - | 20.42 MiB |
|  |  | sql | 38,407 | - | 23.50 MiB |
| jac313_store_006_XS | off | binary | 33,682 | - | 19.81 MiB |
|  |  | jtext | 34,161 | - | 19.96 MiB |
|  |  | sql | 33,633 | - | 22.64 MiB |
| jac313_store_006_XS | on | binary | 37,052 | - | 19.81 MiB |
|  |  | jtext | 37,424 | - | 19.96 MiB |
|  |  | sql | 37,357 | - | 22.64 MiB |
| jac313_store_007_TS | off | binary | 820 | 2,029,138 | 19.82 MiB |
|  |  | jtext | 3,467 | 2,120,621 | 20.45 MiB |
|  |  | sql | 962 | 1,994,654 | 23.51 MiB |
| jac313_store_007_TS | on | binary | 861 | 2,002,122 | 19.82 MiB |
|  |  | jtext | 3,923 | 2,003,928 | 20.45 MiB |
|  |  | sql | 1,813 | 2,015,479 | 23.51 MiB |
| jac313_store_007_XS | off | binary | 805 | 2,056,175 | 19.82 MiB |
|  |  | jtext | 3,718 | 2,224,051 | 19.98 MiB |
|  |  | sql | 1,005 | 2,282,323 | 22.66 MiB |
| jac313_store_007_XS | on | binary | 860 | 1,861,365 | 19.82 MiB |
|  |  | jtext | 3,530 | 2,068,081 | 19.98 MiB |
|  |  | sql | 1,861 | 2,051,324 | 22.66 MiB |

### fedora-44 · clang (xFull, Release, run 57)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_005_TS | off | binary | 404 | 4,052,356 | 8.38 MiB |
|  |  | jtext | 355 | 3,503,609 | 7.90 MiB |
|  |  | sql | 455 | 3,182,180 | 7.42 MiB |
| jac313_store_005_TS | on | binary | 455 | 3,520,135 | 8.38 MiB |
|  |  | jtext | 405 | 3,380,320 | 7.90 MiB |
|  |  | sql | 753 | 3,756,997 | 7.42 MiB |
| jac313_store_005_XS | off | binary | 405 | 4,066,859 | 8.38 MiB |
|  |  | jtext | 404 | 3,566,334 | 7.44 MiB |
|  |  | sql | 453 | 3,068,049 | 7.13 MiB |
| jac313_store_005_XS | on | binary | 406 | 3,286,879 | 8.38 MiB |
|  |  | jtext | 353 | 3,454,828 | 7.44 MiB |
|  |  | sql | 806 | 3,449,346 | 7.13 MiB |
| jac313_store_006_TS | off | binary | 34,091 | - | 19.81 MiB |
|  |  | jtext | 33,938 | - | 20.40 MiB |
|  |  | sql | 33,935 | - | 23.51 MiB |
| jac313_store_006_TS | on | binary | 37,856 | - | 19.81 MiB |
|  |  | jtext | 37,807 | - | 20.40 MiB |
|  |  | sql | 38,161 | - | 23.51 MiB |
| jac313_store_006_XS | off | binary | 33,425 | - | 19.81 MiB |
|  |  | jtext | 33,278 | - | 19.96 MiB |
|  |  | sql | 33,335 | - | 22.63 MiB |
| jac313_store_006_XS | on | binary | 36,752 | - | 19.81 MiB |
|  |  | jtext | 36,695 | - | 19.96 MiB |
|  |  | sql | 37,100 | - | 22.63 MiB |
| jac313_store_007_TS | off | binary | 552 | 3,630,159 | 19.82 MiB |
|  |  | jtext | 706 | 3,566,970 | 20.44 MiB |
|  |  | sql | 803 | 3,861,600 | 23.51 MiB |
| jac313_store_007_TS | on | binary | 553 | 3,921,569 | 19.82 MiB |
|  |  | jtext | 803 | 3,605,163 | 20.44 MiB |
|  |  | sql | 1,512 | 4,141,473 | 23.51 MiB |
| jac313_store_007_XS | off | binary | 555 | 4,427,718 | 19.82 MiB |
|  |  | jtext | 703 | 4,298,117 | 19.98 MiB |
|  |  | sql | 705 | 4,771,675 | 22.66 MiB |
| jac313_store_007_XS | on | binary | 512 | 4,966,476 | 19.82 MiB |
|  |  | jtext | 803 | 4,328,630 | 19.98 MiB |
|  |  | sql | 1,508 | 3,853,268 | 22.66 MiB |

### fedora-44 · gcc16 (xFull, Debug, run 62)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_005_TS | off | binary | 656 | 2,651,324 | 8.38 MiB |
|  |  | jtext | 1,608 | 2,512,184 | 7.92 MiB |
|  |  | sql | 554 | 2,412,603 | 7.42 MiB |
| jac313_store_005_TS | on | binary | 703 | 2,342,908 | 8.38 MiB |
|  |  | jtext | 1,507 | 2,531,902 | 7.92 MiB |
|  |  | sql | 953 | 2,536,655 | 7.42 MiB |
| jac313_store_005_XS | off | binary | 654 | 2,698,400 | 8.38 MiB |
|  |  | jtext | 1,457 | 2,805,679 | 7.44 MiB |
|  |  | sql | 553 | 2,748,536 | 7.13 MiB |
| jac313_store_005_XS | on | binary | 602 | 2,767,094 | 8.38 MiB |
|  |  | jtext | 1,455 | 2,912,310 | 7.44 MiB |
|  |  | sql | 1,054 | 2,944,641 | 7.13 MiB |
| jac313_store_006_TS | off | binary | 34,283 | - | 19.81 MiB |
|  |  | jtext | 34,534 | - | 20.42 MiB |
|  |  | sql | 34,138 | - | 23.50 MiB |
| jac313_store_006_TS | on | binary | 37,757 | - | 19.81 MiB |
|  |  | jtext | 38,357 | - | 20.42 MiB |
|  |  | sql | 38,357 | - | 23.50 MiB |
| jac313_store_006_XS | off | binary | 33,631 | - | 19.81 MiB |
|  |  | jtext | 34,179 | - | 19.96 MiB |
|  |  | sql | 33,633 | - | 22.64 MiB |
| jac313_store_006_XS | on | binary | 37,055 | - | 19.81 MiB |
|  |  | jtext | 37,360 | - | 19.96 MiB |
|  |  | sql | 37,404 | - | 22.64 MiB |
| jac313_store_007_TS | off | binary | 703 | 2,636,088 | 19.82 MiB |
|  |  | jtext | 4,070 | 2,318,841 | 20.45 MiB |
|  |  | sql | 957 | 2,359,381 | 23.51 MiB |
| jac313_store_007_TS | on | binary | 767 | 2,240,896 | 19.82 MiB |
|  |  | jtext | 4,015 | 2,659,150 | 20.45 MiB |
|  |  | sql | 1,815 | 2,294,894 | 23.51 MiB |
| jac313_store_007_XS | off | binary | 753 | 2,660,353 | 19.82 MiB |
|  |  | jtext | 3,867 | 2,226,031 | 19.98 MiB |
|  |  | sql | 1,156 | 2,623,983 | 22.66 MiB |
| jac313_store_007_XS | on | binary | 756 | 2,419,023 | 19.82 MiB |
|  |  | jtext | 4,116 | 2,272,314 | 19.98 MiB |
|  |  | sql | 1,817 | 2,595,178 | 22.66 MiB |

### fedora-44 · gcc16 (xFull, Release, run 66)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_005_TS | off | binary | 402 | 3,948,200 | 8.38 MiB |
|  |  | jtext | 355 | 3,210,582 | 7.91 MiB |
|  |  | sql | 405 | 3,464,523 | 7.43 MiB |
| jac313_store_005_TS | on | binary | 454 | 3,285,799 | 8.38 MiB |
|  |  | jtext | 403 | 3,498,583 | 7.90 MiB |
|  |  | sql | 859 | 3,699,593 | 7.43 MiB |
| jac313_store_005_XS | off | binary | 402 | 4,030,145 | 8.38 MiB |
|  |  | jtext | 402 | 4,041,057 | 7.44 MiB |
|  |  | sql | 351 | 4,421,453 | 7.13 MiB |
| jac313_store_005_XS | on | binary | 460 | 3,030,487 | 8.38 MiB |
|  |  | jtext | 402 | 3,733,294 | 7.44 MiB |
|  |  | sql | 758 | 4,332,380 | 7.13 MiB |
| jac313_store_006_TS | off | binary | 34,045 | - | 19.81 MiB |
|  |  | jtext | 33,891 | - | 20.40 MiB |
|  |  | sql | 33,939 | - | 23.49 MiB |
| jac313_store_006_TS | on | binary | 37,668 | - | 19.81 MiB |
|  |  | jtext | 37,759 | - | 20.40 MiB |
|  |  | sql | 38,060 | - | 23.49 MiB |
| jac313_store_006_XS | off | binary | 33,385 | - | 19.81 MiB |
|  |  | jtext | 33,344 | - | 19.96 MiB |
|  |  | sql | 33,290 | - | 22.63 MiB |
| jac313_store_006_XS | on | binary | 36,843 | - | 19.81 MiB |
|  |  | jtext | 36,707 | - | 19.96 MiB |
|  |  | sql | 36,962 | - | 22.63 MiB |
| jac313_store_007_TS | off | binary | 502 | 4,349,150 | 19.82 MiB |
|  |  | jtext | 753 | 3,700,825 | 20.44 MiB |
|  |  | sql | 706 | 3,977,092 | 23.51 MiB |
| jac313_store_007_TS | on | binary | 454 | 4,989,273 | 19.82 MiB |
|  |  | jtext | 753 | 4,298,117 | 20.44 MiB |
|  |  | sql | 1,408 | 3,792,763 | 23.51 MiB |
| jac313_store_007_XS | off | binary | 505 | 4,322,642 | 19.82 MiB |
|  |  | jtext | 654 | 4,136,505 | 19.98 MiB |
|  |  | sql | 709 | 3,698,225 | 22.66 MiB |
| jac313_store_007_XS | on | binary | 507 | 4,190,764 | 19.82 MiB |
|  |  | jtext | 754 | 3,981,526 | 19.98 MiB |
|  |  | sql | 1,407 | 4,537,823 | 22.66 MiB |

### linuxmint-22.3 · clang (xFull, Debug, run 76)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_005_TS | off | binary | 943 | 838,019 | 8.38 MiB |
|  |  | jtext | 1,837 | 882,145 | 7.92 MiB |
|  |  | sql | 849 | 871,019 | 7.41 MiB |
| jac313_store_005_TS | on | binary | 900 | 1,113,747 | 8.38 MiB |
|  |  | jtext | 1,774 | 1,019,742 | 7.92 MiB |
|  |  | sql | 1,319 | 970,836 | 7.41 MiB |
| jac313_store_005_XS | off | binary | 1,014 | 1,181,084 | 8.38 MiB |
|  |  | jtext | 1,648 | 845,294 | 7.44 MiB |
|  |  | sql | 674 | 1,025,736 | 7.12 MiB |
| jac313_store_005_XS | on | binary | 932 | 810,104 | 8.38 MiB |
|  |  | jtext | 1,545 | 931,263 | 7.44 MiB |
|  |  | sql | 1,331 | 1,074,622 | 7.12 MiB |
| jac313_store_006_TS | off | binary | 34,211 | - | 19.81 MiB |
|  |  | jtext | 34,815 | - | 20.42 MiB |
|  |  | sql | 34,276 | - | 23.46 MiB |
| jac313_store_006_TS | on | binary | 38,078 | - | 19.81 MiB |
|  |  | jtext | 38,440 | - | 20.42 MiB |
|  |  | sql | 38,539 | - | 23.46 MiB |
| jac313_store_006_XS | off | binary | 33,635 | - | 19.81 MiB |
|  |  | jtext | 34,254 | - | 19.96 MiB |
|  |  | sql | 33,632 | - | 22.61 MiB |
| jac313_store_006_XS | on | binary | 37,139 | - | 19.81 MiB |
|  |  | jtext | 37,361 | - | 19.96 MiB |
|  |  | sql | 37,598 | - | 22.61 MiB |
| jac313_store_007_TS | off | binary | 1,055 | 855,242 | 19.82 MiB |
|  |  | jtext | 3,431 | 908,785 | 20.45 MiB |
|  |  | sql | 1,132 | 878,464 | 23.48 MiB |
| jac313_store_007_TS | on | binary | 1,102 | 940,858 | 19.82 MiB |
|  |  | jtext | 3,685 | 879,152 | 20.45 MiB |
|  |  | sql | 2,083 | 917,086 | 23.48 MiB |
| jac313_store_007_XS | off | binary | 969 | 1,002,868 | 19.82 MiB |
|  |  | jtext | 3,444 | 1,035,079 | 19.98 MiB |
|  |  | sql | 1,319 | 1,027,137 | 22.62 MiB |
| jac313_store_007_XS | on | binary | 1,011 | 1,045,347 | 19.82 MiB |
|  |  | jtext | 3,867 | 758,921 | 19.98 MiB |
|  |  | sql | 1,976 | 988,875 | 22.62 MiB |

### linuxmint-22.3 · clang (xFull, Release, run 78)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_005_TS | off | binary | 437 | 2,555,453 | 8.38 MiB |
|  |  | jtext | 406 | 2,026,712 | 7.92 MiB |
|  |  | sql | 463 | 2,771,158 | 7.41 MiB |
| jac313_store_005_TS | on | binary | 457 | 2,493,952 | 8.38 MiB |
|  |  | jtext | 455 | 1,952,019 | 7.92 MiB |
|  |  | sql | 860 | 2,328,722 | 7.41 MiB |
| jac313_store_005_XS | off | binary | 469 | 2,097,755 | 8.38 MiB |
|  |  | jtext | 409 | 2,795,404 | 7.44 MiB |
|  |  | sql | 570 | 2,770,544 | 7.12 MiB |
| jac313_store_005_XS | on | binary | 410 | 2,502,377 | 8.38 MiB |
|  |  | jtext | 352 | 3,258,815 | 7.44 MiB |
|  |  | sql | 870 | 2,477,210 | 7.12 MiB |
| jac313_store_006_TS | off | binary | 33,935 | - | 19.81 MiB |
|  |  | jtext | 33,963 | - | 20.40 MiB |
|  |  | sql | 33,926 | - | 23.47 MiB |
| jac313_store_006_TS | on | binary | 37,704 | - | 19.81 MiB |
|  |  | jtext | 37,800 | - | 20.39 MiB |
|  |  | sql | 38,290 | - | 23.47 MiB |
| jac313_store_006_XS | off | binary | 33,314 | - | 19.81 MiB |
|  |  | jtext | 33,403 | - | 19.96 MiB |
|  |  | sql | 33,366 | - | 22.62 MiB |
| jac313_store_006_XS | on | binary | 36,785 | - | 19.81 MiB |
|  |  | jtext | 36,789 | - | 19.96 MiB |
|  |  | sql | 37,246 | - | 22.62 MiB |
| jac313_store_007_TS | off | binary | 615 | 2,079,564 | 19.82 MiB |
|  |  | jtext | 889 | 2,383,790 | 20.45 MiB |
|  |  | sql | 940 | 1,652,046 | 23.48 MiB |
| jac313_store_007_TS | on | binary | 608 | 2,060,369 | 19.82 MiB |
|  |  | jtext | 869 | 2,461,478 | 20.45 MiB |
|  |  | sql | 1,932 | 1,819,836 | 23.48 MiB |
| jac313_store_007_XS | off | binary | 609 | 2,167,458 | 19.82 MiB |
|  |  | jtext | 963 | 1,918,833 | 19.98 MiB |
|  |  | sql | 909 | 2,803,005 | 22.63 MiB |
| jac313_store_007_XS | on | binary | 665 | 1,992,151 | 19.82 MiB |
|  |  | jtext | 909 | 2,131,424 | 19.98 MiB |
|  |  | sql | 1,927 | 2,285,505 | 22.63 MiB |

### linuxmint-22.3 · gcc15 (xFull, Debug, run 80)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_005_TS | off | binary | 830 | 1,281,312 | 8.38 MiB |
|  |  | jtext | 1,756 | 1,016,611 | 7.92 MiB |
|  |  | sql | 789 | 1,209,409 | 7.42 MiB |
| jac313_store_005_TS | on | binary | 786 | 1,028,627 | 8.38 MiB |
|  |  | jtext | 1,877 | 943,361 | 7.92 MiB |
|  |  | sql | 1,286 | 1,096,672 | 7.42 MiB |
| jac313_store_005_XS | off | binary | 813 | 1,090,917 | 8.38 MiB |
|  |  | jtext | 1,786 | 1,488,472 | 7.44 MiB |
|  |  | sql | 738 | 1,358,917 | 7.13 MiB |
| jac313_store_005_XS | on | binary | 793 | 1,133,144 | 8.38 MiB |
|  |  | jtext | 1,746 | 1,070,469 | 7.44 MiB |
|  |  | sql | 1,262 | 1,102,013 | 7.13 MiB |
| jac313_store_006_TS | off | binary | 34,155 | - | 19.81 MiB |
|  |  | jtext | 34,478 | - | 20.42 MiB |
|  |  | sql | 34,268 | - | 23.48 MiB |
| jac313_store_006_TS | on | binary | 37,986 | - | 19.81 MiB |
|  |  | jtext | 38,435 | - | 20.41 MiB |
|  |  | sql | 38,534 | - | 23.48 MiB |
| jac313_store_006_XS | off | binary | 33,593 | - | 19.81 MiB |
|  |  | jtext | 34,050 | - | 19.96 MiB |
|  |  | sql | 33,509 | - | 22.62 MiB |
| jac313_store_006_XS | on | binary | 37,245 | - | 19.81 MiB |
|  |  | jtext | 37,338 | - | 19.96 MiB |
|  |  | sql | 37,533 | - | 22.62 MiB |
| jac313_store_007_TS | off | binary | 919 | 1,303,696 | 19.82 MiB |
|  |  | jtext | 4,121 | 1,160,685 | 20.45 MiB |
|  |  | sql | 1,222 | 1,016,291 | 23.49 MiB |
| jac313_store_007_TS | on | binary | 979 | 986,865 | 19.82 MiB |
|  |  | jtext | 4,030 | 1,099,360 | 20.45 MiB |
|  |  | sql | 2,022 | 1,421,424 | 23.49 MiB |
| jac313_store_007_XS | off | binary | 877 | 1,301,694 | 19.82 MiB |
|  |  | jtext | 4,245 | 1,253,730 | 19.98 MiB |
|  |  | sql | 1,178 | 1,238,344 | 22.63 MiB |
| jac313_store_007_XS | on | binary | 916 | 1,072,927 | 19.82 MiB |
|  |  | jtext | 3,549 | 1,100,631 | 19.98 MiB |
|  |  | sql | 2,211 | 1,276,047 | 22.63 MiB |

### linuxmint-22.3 · gcc15 (xFull, Release, run 82)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_005_TS | off | binary | 460 | 2,532,928 | 8.38 MiB |
|  |  | jtext | 465 | 1,943,295 | 7.92 MiB |
|  |  | sql | 415 | 2,533,506 | 7.41 MiB |
| jac313_store_005_TS | on | binary | 512 | 2,206,434 | 8.38 MiB |
|  |  | jtext | 462 | 2,113,003 | 7.92 MiB |
|  |  | sql | 919 | 1,946,017 | 7.41 MiB |
| jac313_store_005_XS | off | binary | 461 | 2,426,654 | 8.38 MiB |
|  |  | jtext | 458 | 1,969,939 | 7.44 MiB |
|  |  | sql | 411 | 2,475,676 | 7.12 MiB |
| jac313_store_005_XS | on | binary | 509 | 2,562,657 | 8.38 MiB |
|  |  | jtext | 416 | 1,596,857 | 7.44 MiB |
|  |  | sql | 870 | 2,702,995 | 7.12 MiB |
| jac313_store_006_TS | off | binary | 33,924 | - | 19.81 MiB |
|  |  | jtext | 34,021 | - | 20.38 MiB |
|  |  | sql | 33,900 | - | 23.47 MiB |
| jac313_store_006_TS | on | binary | 37,753 | - | 19.81 MiB |
|  |  | jtext | 37,848 | - | 20.38 MiB |
|  |  | sql | 38,164 | - | 23.47 MiB |
| jac313_store_006_XS | off | binary | 33,226 | - | 19.81 MiB |
|  |  | jtext | 33,377 | - | 19.96 MiB |
|  |  | sql | 33,366 | - | 22.62 MiB |
| jac313_store_006_XS | on | binary | 36,874 | - | 19.81 MiB |
|  |  | jtext | 36,834 | - | 19.96 MiB |
|  |  | sql | 37,267 | - | 22.62 MiB |
| jac313_store_007_TS | off | binary | 557 | 2,205,655 | 19.82 MiB |
|  |  | jtext | 821 | 2,214,251 | 20.45 MiB |
|  |  | sql | 709 | 2,225,734 | 23.49 MiB |
| jac313_store_007_TS | on | binary | 615 | 1,989,139 | 19.82 MiB |
|  |  | jtext | 869 | 2,487,438 | 20.45 MiB |
|  |  | sql | 1,777 | 2,676,230 | 23.49 MiB |
| jac313_store_007_XS | off | binary | 577 | 1,670,844 | 19.82 MiB |
|  |  | jtext | 879 | 2,972,917 | 19.98 MiB |
|  |  | sql | 805 | 2,184,121 | 22.61 MiB |
| jac313_store_007_XS | on | binary | 560 | 2,644,803 | 19.82 MiB |
|  |  | jtext | 812 | 2,204,488 | 19.98 MiB |
|  |  | sql | 1,720 | 3,006,615 | 22.61 MiB |

### rhel-9.8 · clang (xFull, Debug, run 43)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_005_TS | off | binary | 451 | 4,448,399 | 8.38 MiB |
|  |  | jtext | 1,503 | 4,535,970 | 7.91 MiB |
|  |  | sql | 451 | 4,475,074 | 7.42 MiB |
| jac313_store_005_TS | on | binary | 501 | 4,668,098 | 8.38 MiB |
|  |  | jtext | 1,202 | 4,402,765 | 7.91 MiB |
|  |  | sql | 551 | 4,397,924 | 7.42 MiB |
| jac313_store_005_XS | off | binary | 451 | 5,120,590 | 8.38 MiB |
|  |  | jtext | 1,302 | 4,593,477 | 7.44 MiB |
|  |  | sql | 451 | 4,586,525 | 7.13 MiB |
| jac313_store_005_XS | on | binary | 501 | 4,576,659 | 8.38 MiB |
|  |  | jtext | 1,052 | 4,470,672 | 7.44 MiB |
|  |  | sql | 551 | 4,511,414 | 7.13 MiB |
| jac313_store_006_TS | off | binary | 32,084 | - | 19.81 MiB |
|  |  | jtext | 32,686 | - | 20.40 MiB |
|  |  | sql | 32,135 | - | 23.48 MiB |
| jac313_store_006_TS | on | binary | 35,599 | - | 19.81 MiB |
|  |  | jtext | 35,950 | - | 20.40 MiB |
|  |  | sql | 35,749 | - | 23.48 MiB |
| jac313_store_006_XS | off | binary | 31,429 | - | 19.81 MiB |
|  |  | jtext | 31,731 | - | 19.96 MiB |
|  |  | sql | 31,482 | - | 22.64 MiB |
| jac313_store_006_XS | on | binary | 34,744 | - | 19.81 MiB |
|  |  | jtext | 35,042 | - | 19.96 MiB |
|  |  | sql | 34,797 | - | 22.64 MiB |
| jac313_store_007_TS | off | binary | 501 | 4,518,549 | 19.82 MiB |
|  |  | jtext | 3,357 | 4,379,434 | 20.45 MiB |
|  |  | sql | 852 | 4,535,970 | 23.51 MiB |
| jac313_store_007_TS | on | binary | 551 | 4,462,891 | 19.82 MiB |
|  |  | jtext | 3,104 | 4,867,838 | 20.44 MiB |
|  |  | sql | 1,152 | 4,342,728 | 23.51 MiB |
| jac313_store_007_XS | off | binary | 451 | 4,767,580 | 19.82 MiB |
|  |  | jtext | 3,006 | 4,419,890 | 19.98 MiB |
|  |  | sql | 751 | 4,492,969 | 22.66 MiB |
| jac313_store_007_XS | on | binary | 451 | 4,643,388 | 19.82 MiB |
|  |  | jtext | 3,057 | 5,212,949 | 19.98 MiB |
|  |  | sql | 952 | 4,643,388 | 22.66 MiB |

### rhel-9.8 · clang (xFull, Release, run 45)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_005_TS | off | binary | 200 | 8,648,275 | 8.38 MiB |
|  |  | jtext | 300 | 7,107,321 | 7.86 MiB |
|  |  | sql | 250 | 6,559,958 | 7.43 MiB |
| jac313_store_005_TS | on | binary | 251 | 6,729,022 | 8.38 MiB |
|  |  | jtext | 300 | 8,231,808 | 7.85 MiB |
|  |  | sql | 401 | 8,522,969 | 7.43 MiB |
| jac313_store_005_XS | off | binary | 200 | 7,728,573 | 8.38 MiB |
|  |  | jtext | 300 | 10,167,768 | 7.44 MiB |
|  |  | sql | 250 | 7,466,587 | 7.13 MiB |
| jac313_store_005_XS | on | binary | 200 | 7,666,360 | 8.38 MiB |
|  |  | jtext | 250 | 7,798,487 | 7.44 MiB |
|  |  | sql | 351 | 7,918,283 | 7.13 MiB |
| jac313_store_006_TS | off | binary | 31,788 | - | 19.81 MiB |
|  |  | jtext | 31,835 | - | 20.36 MiB |
|  |  | sql | 31,833 | - | 23.46 MiB |
| jac313_store_006_TS | on | binary | 35,348 | - | 19.81 MiB |
|  |  | jtext | 35,452 | - | 20.36 MiB |
|  |  | sql | 35,449 | - | 23.46 MiB |
| jac313_store_006_XS | off | binary | 31,231 | - | 19.81 MiB |
|  |  | jtext | 31,280 | - | 19.96 MiB |
|  |  | sql | 30,681 | - | 22.64 MiB |
| jac313_store_006_XS | on | binary | 34,496 | - | 19.81 MiB |
|  |  | jtext | 34,496 | - | 19.96 MiB |
|  |  | sql | 34,054 | - | 22.64 MiB |
| jac313_store_007_TS | off | binary | 250 | 9,328,358 | 19.82 MiB |
|  |  | jtext | 802 | 9,379,983 | 20.39 MiB |
|  |  | sql | 551 | 6,802,258 | 23.52 MiB |
| jac313_store_007_TS | on | binary | 301 | 6,601,096 | 19.82 MiB |
|  |  | jtext | 651 | 8,937,349 | 20.39 MiB |
|  |  | sql | 902 | 6,766,358 | 23.52 MiB |
| jac313_store_007_XS | off | binary | 250 | 8,210,181 | 19.82 MiB |
|  |  | jtext | 752 | 7,802,747 | 19.98 MiB |
|  |  | sql | 601 | 8,067,118 | 22.65 MiB |
| jac313_store_007_XS | on | binary | 250 | 7,961,783 | 19.82 MiB |
|  |  | jtext | 701 | 8,169,267 | 19.98 MiB |
|  |  | sql | 802 | 7,221,259 | 22.65 MiB |

### rhel-9.8 · gcc15 (xFull, Debug, run 47)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_005_TS | off | binary | 451 | 5,064,573 | 8.38 MiB |
|  |  | jtext | 1,403 | 5,443,066 | 7.91 MiB |
|  |  | sql | 451 | 5,601,927 | 7.42 MiB |
| jac313_store_005_TS | on | binary | 451 | 5,652,592 | 8.38 MiB |
|  |  | jtext | 1,403 | 5,307,011 | 7.91 MiB |
|  |  | sql | 501 | 4,345,370 | 7.42 MiB |
| jac313_store_005_XS | off | binary | 501 | 5,006,508 | 8.38 MiB |
|  |  | jtext | 1,252 | 5,288,487 | 7.44 MiB |
|  |  | sql | 451 | 5,564,211 | 7.13 MiB |
| jac313_store_005_XS | on | binary | 451 | 5,242,189 | 8.38 MiB |
|  |  | jtext | 1,454 | 5,928,034 | 7.44 MiB |
|  |  | sql | 501 | 5,402,485 | 7.13 MiB |
| jac313_store_006_TS | off | binary | 32,065 | - | 19.81 MiB |
|  |  | jtext | 32,334 | - | 20.41 MiB |
|  |  | sql | 32,187 | - | 23.47 MiB |
| jac313_store_006_TS | on | binary | 35,701 | - | 19.81 MiB |
|  |  | jtext | 36,275 | - | 20.41 MiB |
|  |  | sql | 35,851 | - | 23.47 MiB |
| jac313_store_006_XS | off | binary | 31,481 | - | 19.81 MiB |
|  |  | jtext | 32,083 | - | 19.96 MiB |
|  |  | sql | 31,482 | - | 22.64 MiB |
| jac313_store_006_XS | on | binary | 34,694 | - | 19.81 MiB |
|  |  | jtext | 34,997 | - | 19.96 MiB |
|  |  | sql | 34,899 | - | 22.64 MiB |
| jac313_store_007_TS | off | binary | 551 | 4,471,472 | 19.82 MiB |
|  |  | jtext | 3,959 | 5,130,047 | 20.44 MiB |
|  |  | sql | 801 | 4,812,783 | 23.51 MiB |
| jac313_store_007_TS | on | binary | 551 | 5,550,930 | 19.82 MiB |
|  |  | jtext | 3,859 | 4,480,688 | 20.44 MiB |
|  |  | sql | 1,252 | 5,112,213 | 23.51 MiB |
| jac313_store_007_XS | off | binary | 501 | 4,877,811 | 19.82 MiB |
|  |  | jtext | 4,408 | 5,182,690 | 19.98 MiB |
|  |  | sql | 852 | 5,669,256 | 22.66 MiB |
| jac313_store_007_XS | on | binary | 451 | 5,141,388 | 19.82 MiB |
|  |  | jtext | 5,061 | 5,400,734 | 19.98 MiB |
|  |  | sql | 1,503 | 5,232,589 | 22.66 MiB |

### rhel-9.8 · gcc15 (xFull, Release, run 49)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_005_TS | off | binary | 201 | 6,659,563 | 8.38 MiB |
|  |  | jtext | 300 | 10,030,090 | 7.85 MiB |
|  |  | sql | 300 | 7,488,954 | 7.42 MiB |
| jac313_store_005_TS | on | binary | 250 | 5,547,542 | 8.38 MiB |
|  |  | jtext | 250 | 7,898,894 | 7.85 MiB |
|  |  | sql | 401 | 7,039,775 | 7.42 MiB |
| jac313_store_005_XS | off | binary | 200 | 7,760,962 | 8.38 MiB |
|  |  | jtext | 300 | 7,810,669 | 7.44 MiB |
|  |  | sql | 300 | 7,574,036 | 7.13 MiB |
| jac313_store_005_XS | on | binary | 200 | 7,930,843 | 8.38 MiB |
|  |  | jtext | 250 | 8,320,852 | 7.44 MiB |
|  |  | sql | 351 | 8,044,405 | 7.13 MiB |
| jac313_store_006_TS | off | binary | 31,737 | - | 19.81 MiB |
|  |  | jtext | 31,785 | - | 20.36 MiB |
|  |  | sql | 31,836 | - | 23.46 MiB |
| jac313_store_006_TS | on | binary | 35,353 | - | 19.81 MiB |
|  |  | jtext | 35,451 | - | 20.36 MiB |
|  |  | sql | 35,501 | - | 23.46 MiB |
| jac313_store_006_XS | off | binary | 31,229 | - | 19.81 MiB |
|  |  | jtext | 31,279 | - | 19.96 MiB |
|  |  | sql | 31,234 | - | 22.64 MiB |
| jac313_store_006_XS | on | binary | 34,497 | - | 19.81 MiB |
|  |  | jtext | 34,493 | - | 19.96 MiB |
|  |  | sql | 34,544 | - | 22.64 MiB |
| jac313_store_007_TS | off | binary | 250 | 8,634,833 | 19.82 MiB |
|  |  | jtext | 752 | 8,664,010 | 20.39 MiB |
|  |  | sql | 601 | 10,217,636 | 23.51 MiB |
| jac313_store_007_TS | on | binary | 250 | 6,077,550 | 19.82 MiB |
|  |  | jtext | 601 | 11,068,069 | 20.38 MiB |
|  |  | sql | 751 | 6,664,001 | 23.51 MiB |
| jac313_store_007_XS | off | binary | 250 | 8,781,173 | 19.82 MiB |
|  |  | jtext | 652 | 9,018,759 | 19.98 MiB |
|  |  | sql | 651 | 8,458,806 | 22.66 MiB |
| jac313_store_007_XS | on | binary | 250 | 7,982,757 | 19.82 MiB |
|  |  | jtext | 651 | 8,213,552 | 19.98 MiB |
|  |  | sql | 752 | 8,557,248 | 22.66 MiB |


## Top throughput by log category

Peak ops/sec from benchmark tests 005–008 (`Fastest run` in each log). xFull matrix only, grouped by persist log directory.

### binary_logs

<table>
<thead>
<tr><th>#</th><th>Peak ops/sec</th><th>Test</th><th>Persist</th><th>ms</th><th>Log</th></tr>
</thead>
<tbody>
<tr><td>1</td><td style="text-align:right">12,716,175</td><td>jac313_store_007_XS</td><td>binary/on</td><td style="text-align:right">250</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/modules/binary_logs/JAC313_STORE_TEST_007_XS/clang_binary_on.log">clang_binary_on.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · CT1000BX500SSD1 (ssd label)</td></tr>
<tr><td>2</td><td style="text-align:right">10,887,316</td><td>jac313_store_007_XS</td><td>binary/on</td><td style="text-align:right">360</td><td><a href="rhel-9.8/clang/Release/10k/xFull/modules/binary_logs/JAC313_STORE_TEST_007_XS/clang_binary_on.log">clang_binary_on.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> jac313-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · HUC101812CSS200 (10k label)</td></tr>
<tr><td>3</td><td style="text-align:right">9,328,358</td><td>jac313_store_007_TS</td><td>binary/off</td><td style="text-align:right">250</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/textual/binary_logs/JAC313_STORE_TEST_007_TS/clang_binary_off.log">clang_binary_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · CT1000BX500SSD1 (ssd label)</td></tr>
<tr><td>4</td><td style="text-align:right">9,143,275</td><td>jac313_store_007_TS</td><td>binary/off</td><td style="text-align:right">250</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/modules/binary_logs/JAC313_STORE_TEST_007_TS/clang_binary_off.log">clang_binary_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · CT1000BX500SSD1 (ssd label)</td></tr>
<tr><td>5</td><td style="text-align:right">9,103,323</td><td>jac313_store_007_TS</td><td>binary/off</td><td style="text-align:right">351</td><td><a href="rhel-9.8/gcc15/Release/x7k/xFull/textual/binary_logs/JAC313_STORE_TEST_007_TS/gcc15_binary_off.log">gcc15_binary_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · ST6000NM0095 (x7k label)</td></tr>
<tr><td>6</td><td style="text-align:right">8,999,280</td><td>jac313_store_007_XS</td><td>binary/on</td><td style="text-align:right">351</td><td><a href="rhel-9.8/gcc15/Release/10k/xFull/textual/binary_logs/JAC313_STORE_TEST_007_XS/gcc15_binary_on.log">gcc15_binary_on.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> jac313-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · HUC101812CSS200 (10k label)</td></tr>
<tr><td>7</td><td style="text-align:right">8,937,349</td><td>jac313_store_007_XS</td><td>binary/off</td><td style="text-align:right">351</td><td><a href="rhel-9.8/gcc15/Release/x7k/xFull/textual/binary_logs/JAC313_STORE_TEST_007_XS/gcc15_binary_off.log">gcc15_binary_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · ST6000NM0095 (x7k label)</td></tr>
<tr><td>8</td><td style="text-align:right">8,906,306</td><td>jac313_store_007_XS</td><td>binary/on</td><td style="text-align:right">301</td><td><a href="rhel-9.8/gcc15/Release/10k/xFull/modules/binary_logs/JAC313_STORE_TEST_007_XS/gcc15_binary_on.log">gcc15_binary_on.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> jac313-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · HUC101812CSS200 (10k label)</td></tr>
<tr><td>9</td><td style="text-align:right">8,837,826</td><td>jac313_store_007_TS</td><td>binary/on</td><td style="text-align:right">351</td><td><a href="rhel-9.8/gcc15/Release/x7k/xFull/textual/binary_logs/JAC313_STORE_TEST_007_TS/gcc15_binary_on.log">gcc15_binary_on.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · ST6000NM0095 (x7k label)</td></tr>
<tr><td>10</td><td style="text-align:right">8,781,173</td><td>jac313_store_007_XS</td><td>binary/off</td><td style="text-align:right">250</td><td><a href="rhel-9.8/gcc15/Release/ssd/xFull/textual/binary_logs/JAC313_STORE_TEST_007_XS/gcc15_binary_off.log">gcc15_binary_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · CT1000BX500SSD1 (ssd label)</td></tr>
</tbody>
</table>

### inmem_logs

<table>
<thead>
<tr><th>#</th><th>Peak ops/sec</th><th>Test</th><th>Persist</th><th>ms</th><th>Log</th></tr>
</thead>
<tbody>
<tr><td>1</td><td style="text-align:right">13,027,619</td><td>jac313_store_007_XS</td><td>none/off</td><td style="text-align:right">100</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/modules/inmem_logs/JAC313_STORE_TEST_007_XS/clang_none_off.log">clang_none_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · CT1000BX500SSD1 (ssd label)</td></tr>
<tr><td>2</td><td style="text-align:right">12,805,737</td><td>jac313_store_007_XS</td><td>none/off</td><td style="text-align:right">100</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/textual/inmem_logs/JAC313_STORE_TEST_007_XS/clang_none_off.log">clang_none_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · CT1000BX500SSD1 (ssd label)</td></tr>
<tr><td>3</td><td style="text-align:right">12,161,012</td><td>jac313_store_007_TS</td><td>none/on</td><td style="text-align:right">100</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/modules/inmem_logs/JAC313_STORE_TEST_007_TS/clang_none_on.log">clang_none_on.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · CT1000BX500SSD1 (ssd label)</td></tr>
<tr><td>4</td><td style="text-align:right">11,087,704</td><td>jac313_store_007_XS</td><td>none/off</td><td style="text-align:right">100</td><td><a href="rhel-9.8/gcc15/Release/ssd/xFull/textual/inmem_logs/JAC313_STORE_TEST_007_XS/gcc15_none_off.log">gcc15_none_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · CT1000BX500SSD1 (ssd label)</td></tr>
<tr><td>5</td><td style="text-align:right">10,463,535</td><td>jac313_store_007_XS</td><td>none/off</td><td style="text-align:right">100</td><td><a href="rhel-9.8/gcc15/Release/ssd/xFull/modules/inmem_logs/JAC313_STORE_TEST_007_XS/gcc15_none_off.log">gcc15_none_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · CT1000BX500SSD1 (ssd label)</td></tr>
<tr><td>6</td><td style="text-align:right">10,214,505</td><td>jac313_store_007_XS</td><td>none/on</td><td style="text-align:right">100</td><td><a href="rhel-9.8/clang/Release/x7k/xFull/modules/inmem_logs/JAC313_STORE_TEST_007_XS/clang_none_on.log">clang_none_on.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · ST6000NM0095 (x7k label)</td></tr>
<tr><td>7</td><td style="text-align:right">10,027,073</td><td>jac313_store_005_XS</td><td>none/on</td><td style="text-align:right">100</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/textual/inmem_logs/JAC313_STORE_TEST_005_XS/clang_none_on.log">clang_none_on.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · CT1000BX500SSD1 (ssd label)</td></tr>
<tr><td>8</td><td style="text-align:right">9,802,960</td><td>jac313_store_005_TS</td><td>none/off</td><td style="text-align:right">100</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/textual/inmem_logs/JAC313_STORE_TEST_005_TS/clang_none_off.log">clang_none_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · CT1000BX500SSD1 (ssd label)</td></tr>
<tr><td>9</td><td style="text-align:right">9,531,980</td><td>jac313_store_007_XS</td><td>none/off</td><td style="text-align:right">100</td><td><a href="rhel-9.8/gcc15/Release/x7k/xFull/modules/inmem_logs/JAC313_STORE_TEST_007_XS/gcc15_none_off.log">gcc15_none_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · ST6000NM0095 (x7k label)</td></tr>
<tr><td>10</td><td style="text-align:right">9,374,707</td><td>jac313_store_007_XS</td><td>none/on</td><td style="text-align:right">100</td><td><a href="rhel-9.8/gcc15/Release/x7k/xFull/modules/inmem_logs/JAC313_STORE_TEST_007_XS/gcc15_none_on.log">gcc15_none_on.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · ST6000NM0095 (x7k label)</td></tr>
</tbody>
</table>

### sql_logs

<table>
<thead>
<tr><th>#</th><th>Peak ops/sec</th><th>Test</th><th>Persist</th><th>ms</th><th>Log</th></tr>
</thead>
<tbody>
<tr><td>1</td><td style="text-align:right">12,459,507</td><td>jac313_store_007_XS</td><td>sql/on</td><td style="text-align:right">701</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/modules/sql_logs/JAC313_STORE_TEST_007_XS/clang_sql_on.log">clang_sql_on.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · CT1000BX500SSD1 (ssd label)</td></tr>
<tr><td>2</td><td style="text-align:right">12,158,055</td><td>jac313_store_007_XS</td><td>sql/off</td><td style="text-align:right">551</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/modules/sql_logs/JAC313_STORE_TEST_007_XS/clang_sql_off.log">clang_sql_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · CT1000BX500SSD1 (ssd label)</td></tr>
<tr><td>3</td><td style="text-align:right">10,410,160</td><td>jac313_store_007_XS</td><td>sql/off</td><td style="text-align:right">852</td><td><a href="rhel-9.8/clang/Release/x7k/xFull/textual/sql_logs/JAC313_STORE_TEST_007_XS/clang_sql_off.log">clang_sql_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · ST6000NM0095 (x7k label)</td></tr>
<tr><td>4</td><td style="text-align:right">10,299,722</td><td>jac313_store_007_TS</td><td>sql/off</td><td style="text-align:right">551</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/modules/sql_logs/JAC313_STORE_TEST_007_TS/clang_sql_off.log">clang_sql_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · CT1000BX500SSD1 (ssd label)</td></tr>
<tr><td>5</td><td style="text-align:right">10,288,066</td><td>jac313_store_007_TS</td><td>sql/off</td><td style="text-align:right">601</td><td><a href="rhel-9.8/clang/Release/x7k/xFull/modules/sql_logs/JAC313_STORE_TEST_007_TS/clang_sql_off.log">clang_sql_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · ST6000NM0095 (x7k label)</td></tr>
<tr><td>6</td><td style="text-align:right">10,217,636</td><td>jac313_store_007_TS</td><td>sql/off</td><td style="text-align:right">601</td><td><a href="rhel-9.8/gcc15/Release/ssd/xFull/textual/sql_logs/JAC313_STORE_TEST_007_TS/gcc15_sql_off.log">gcc15_sql_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · CT1000BX500SSD1 (ssd label)</td></tr>
<tr><td>7</td><td style="text-align:right">9,955,202</td><td>jac313_store_005_TS</td><td>sql/off</td><td style="text-align:right">250</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/modules/sql_logs/JAC313_STORE_TEST_005_TS/clang_sql_off.log">clang_sql_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · CT1000BX500SSD1 (ssd label)</td></tr>
<tr><td>8</td><td style="text-align:right">9,242,144</td><td>jac313_store_005_TS</td><td>sql/on</td><td style="text-align:right">351</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/modules/sql_logs/JAC313_STORE_TEST_005_TS/clang_sql_on.log">clang_sql_on.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · CT1000BX500SSD1 (ssd label)</td></tr>
<tr><td>9</td><td style="text-align:right">8,866,820</td><td>jac313_store_007_TS</td><td>sql/off</td><td style="text-align:right">551</td><td><a href="rhel-9.8/gcc15/Release/ssd/xFull/modules/sql_logs/JAC313_STORE_TEST_007_TS/gcc15_sql_off.log">gcc15_sql_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · CT1000BX500SSD1 (ssd label)</td></tr>
<tr><td>10</td><td style="text-align:right">8,837,045</td><td>jac313_store_007_TS</td><td>sql/on</td><td style="text-align:right">1,253</td><td><a href="rhel-9.8/gcc15/Release/10k/xFull/textual/sql_logs/JAC313_STORE_TEST_007_TS/gcc15_sql_on.log">gcc15_sql_on.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> jac313-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · HUC101812CSS200 (10k label)</td></tr>
</tbody>
</table>

### jText_logs

<table>
<thead>
<tr><th>#</th><th>Peak ops/sec</th><th>Test</th><th>Persist</th><th>ms</th><th>Log</th></tr>
</thead>
<tbody>
<tr><td>1</td><td style="text-align:right">11,880,718</td><td>jac313_store_007_TS</td><td>jtext/on</td><td style="text-align:right">651</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/modules/jText_logs/JAC313_STORE_TEST_007_TS/clang_jtext_on.log">clang_jtext_on.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · CT1000BX500SSD1 (ssd label)</td></tr>
<tr><td>2</td><td style="text-align:right">11,068,069</td><td>jac313_store_007_TS</td><td>jtext/on</td><td style="text-align:right">601</td><td><a href="rhel-9.8/gcc15/Release/ssd/xFull/textual/jText_logs/JAC313_STORE_TEST_007_TS/gcc15_jtext_on.log">gcc15_jtext_on.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · CT1000BX500SSD1 (ssd label)</td></tr>
<tr><td>3</td><td style="text-align:right">10,167,768</td><td>jac313_store_005_XS</td><td>jtext/off</td><td style="text-align:right">300</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/textual/jText_logs/JAC313_STORE_TEST_005_XS/clang_jtext_off.log">clang_jtext_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · CT1000BX500SSD1 (ssd label)</td></tr>
<tr><td>4</td><td style="text-align:right">10,030,090</td><td>jac313_store_005_TS</td><td>jtext/off</td><td style="text-align:right">300</td><td><a href="rhel-9.8/gcc15/Release/ssd/xFull/textual/jText_logs/JAC313_STORE_TEST_005_TS/gcc15_jtext_off.log">gcc15_jtext_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · CT1000BX500SSD1 (ssd label)</td></tr>
<tr><td>5</td><td style="text-align:right">10,005,003</td><td>jac313_store_005_XS</td><td>jtext/off</td><td style="text-align:right">300</td><td><a href="rhel-9.8/gcc15/Release/ssd/xFull/modules/jText_logs/JAC313_STORE_TEST_005_XS/gcc15_jtext_off.log">gcc15_jtext_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · CT1000BX500SSD1 (ssd label)</td></tr>
<tr><td>6</td><td style="text-align:right">9,885,330</td><td>jac313_store_007_TS</td><td>jtext/on</td><td style="text-align:right">952</td><td><a href="rhel-9.8/clang/Release/10k/xFull/textual/jText_logs/JAC313_STORE_TEST_007_TS/clang_jtext_on.log">clang_jtext_on.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> jac313-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · HUC101812CSS200 (10k label)</td></tr>
<tr><td>7</td><td style="text-align:right">9,600,614</td><td>jac313_store_005_TS</td><td>jtext/off</td><td style="text-align:right">250</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/modules/jText_logs/JAC313_STORE_TEST_005_TS/clang_jtext_off.log">clang_jtext_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · CT1000BX500SSD1 (ssd label)</td></tr>
<tr><td>8</td><td style="text-align:right">9,430,404</td><td>jac313_store_007_XS</td><td>jtext/off</td><td style="text-align:right">651</td><td><a href="rhel-9.8/gcc15/Release/x7k/xFull/modules/jText_logs/JAC313_STORE_TEST_007_XS/gcc15_jtext_off.log">gcc15_jtext_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · ST6000NM0095 (x7k label)</td></tr>
<tr><td>9</td><td style="text-align:right">9,379,983</td><td>jac313_store_007_TS</td><td>jtext/off</td><td style="text-align:right">802</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/textual/jText_logs/JAC313_STORE_TEST_007_TS/clang_jtext_off.log">clang_jtext_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · CT1000BX500SSD1 (ssd label)</td></tr>
<tr><td>10</td><td style="text-align:right">9,311,854</td><td>jac313_store_007_TS</td><td>jtext/off</td><td style="text-align:right">651</td><td><a href="rhel-9.8/gcc15/Release/ssd/xFull/modules/jText_logs/JAC313_STORE_TEST_007_TS/gcc15_jtext_off.log">gcc15_jtext_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · CT1000BX500SSD1 (ssd label)</td></tr>
</tbody>
</table>

### flags_logs

<table>
<thead>
<tr><th>#</th><th>Peak ops/sec</th><th>Test</th><th>Persist</th><th>ms</th><th>Log</th></tr>
</thead>
<tbody>
<tr><td>1</td><td style="text-align:right">12,743,074</td><td>jac313_store_008_TS</td><td>flags/off</td><td style="text-align:right">1,757</td><td><a href="rhel-9.8/clang/Release/x7k/xFull/textual/flags_logs/JAC313_STORE_TEST_008_TS/clang_flags_off.log">clang_flags_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · ST6000NM0095 (x7k label)</td></tr>
<tr><td>2</td><td style="text-align:right">11,534,824</td><td>jac313_store_008_XS</td><td>flags/off</td><td style="text-align:right">2,254</td><td><a href="rhel-9.8/gcc15/Release/x7k/xFull/textual/flags_logs/JAC313_STORE_TEST_008_XS/gcc15_flags_off.log">gcc15_flags_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · ST6000NM0095 (x7k label)</td></tr>
<tr><td>3</td><td style="text-align:right">11,085,246</td><td>jac313_store_008_TS</td><td>flags/off</td><td style="text-align:right">1,904</td><td><a href="rhel-9.8/gcc15/Release/10k/xFull/textual/flags_logs/JAC313_STORE_TEST_008_TS/gcc15_flags_off.log">gcc15_flags_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> jac313-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · HUC101812CSS200 (10k label)</td></tr>
<tr><td>4</td><td style="text-align:right">11,071,867</td><td>jac313_store_008_XS</td><td>flags/off</td><td style="text-align:right">1,553</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/modules/flags_logs/JAC313_STORE_TEST_008_XS/clang_flags_off.log">clang_flags_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · CT1000BX500SSD1 (ssd label)</td></tr>
<tr><td>5</td><td style="text-align:right">11,043,866</td><td>jac313_store_008_XS</td><td>flags/off</td><td style="text-align:right">1,861</td><td><a href="rhel-9.8/clang/Release/x7k/xFull/modules/flags_logs/JAC313_STORE_TEST_008_XS/clang_flags_off.log">clang_flags_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · ST6000NM0095 (x7k label)</td></tr>
<tr><td>6</td><td style="text-align:right">10,736,526</td><td>jac313_store_008_TS</td><td>flags/off</td><td style="text-align:right">1,503</td><td><a href="rhel-9.8/gcc15/Release/ssd/xFull/modules/flags_logs/JAC313_STORE_TEST_008_TS/gcc15_flags_off.log">gcc15_flags_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · CT1000BX500SSD1 (ssd label)</td></tr>
<tr><td>7</td><td style="text-align:right">10,598,385</td><td>jac313_store_008_XS</td><td>flags/off</td><td style="text-align:right">2,304</td><td><a href="rhel-9.8/gcc15/Release/x7k/xFull/modules/flags_logs/JAC313_STORE_TEST_008_XS/gcc15_flags_off.log">gcc15_flags_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · ST6000NM0095 (x7k label)</td></tr>
<tr><td>8</td><td style="text-align:right">10,577,533</td><td>jac313_store_008_XS</td><td>flags/off</td><td style="text-align:right">1,954</td><td><a href="rhel-9.8/gcc15/Release/10k/xFull/modules/flags_logs/JAC313_STORE_TEST_008_XS/gcc15_flags_off.log">gcc15_flags_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> jac313-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · HUC101812CSS200 (10k label)</td></tr>
<tr><td>9</td><td style="text-align:right">10,458,610</td><td>jac313_store_008_XS</td><td>flags/off</td><td style="text-align:right">1,954</td><td><a href="rhel-9.8/clang/Release/10k/xFull/textual/flags_logs/JAC313_STORE_TEST_008_XS/clang_flags_off.log">clang_flags_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> jac313-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · HUC101812CSS200 (10k label)</td></tr>
<tr><td>10</td><td style="text-align:right">10,419,923</td><td>jac313_store_008_TS</td><td>flags/off</td><td style="text-align:right">1,653</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/textual/flags_logs/JAC313_STORE_TEST_008_TS/clang_flags_off.log">clang_flags_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> jac313-002 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz · 251 GB RAM · CT1000BX500SSD1 (ssd label)</td></tr>
</tbody>
</table>

