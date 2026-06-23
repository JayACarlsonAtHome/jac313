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
<tr><td>rhel-9.8</td><td>clang</td><td>Debug</td><td>modules</td><td>Smoke</td><td>ssd</td><td style="text-align:right">99</td><td>2026-06-23 01:08:58Z</td><td style="text-align:right">16,232 ms (16s)</td><td style="text-align:right">4,708 ms (4s)</td><td style="text-align:right">2,544,529</td><td>jac313_store_005_TS (none/on)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/clang/Debug/ssd/Smoke/modules/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>rhel-9.8</td><td>clang</td><td>Debug</td><td>textual</td><td>Smoke</td><td>ssd</td><td style="text-align:right">100</td><td>2026-06-23 01:09:32Z</td><td style="text-align:right">16,231 ms (16s)</td><td style="text-align:right">-</td><td style="text-align:right">2,873,563</td><td>jac313_store_005_XS (none/on)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/clang/Debug/ssd/Smoke/textual/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
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
<tr><td>rhel-9.8</td><td>clang</td><td>Debug</td><td>modules</td><td>xFull</td><td>ssd</td><td style="text-align:right">107</td><td>2026-06-23 01:13:28Z</td><td style="text-align:right">597,940 ms (9m 57s)</td><td style="text-align:right">4,858 ms (4s)</td><td style="text-align:right">14,068,655</td><td>jac313_store_008_XS (flags/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/clang/Debug/ssd/xFull/modules/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>rhel-9.8</td><td>clang</td><td>Debug</td><td>textual</td><td>xFull</td><td>ssd</td><td style="text-align:right">108</td><td>2026-06-23 01:23:39Z</td><td style="text-align:right">599,097 ms (9m 59s)</td><td style="text-align:right">-</td><td style="text-align:right">14,690,760</td><td>jac313_store_005_XS (none/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/clang/Debug/ssd/xFull/textual/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
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
<tr><td>rhel-9.8</td><td>clang</td><td>Release</td><td>modules</td><td>Smoke</td><td>ssd</td><td style="text-align:right">101</td><td>2026-06-23 01:09:50Z</td><td style="text-align:right">15,883 ms (15s)</td><td style="text-align:right">7,115 ms (7s)</td><td style="text-align:right">4,716,981</td><td>jac313_store_005_XS (none/on)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/clang/Release/ssd/Smoke/modules/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>rhel-9.8</td><td>clang</td><td>Release</td><td>textual</td><td>Smoke</td><td>ssd</td><td style="text-align:right">102</td><td>2026-06-23 01:10:31Z</td><td style="text-align:right">15,832 ms (15s)</td><td style="text-align:right">-</td><td style="text-align:right">4,329,004</td><td>jac313_store_007_XS (none/on)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/clang/Release/ssd/Smoke/textual/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
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
<tr><td>rhel-9.8</td><td>clang</td><td>Release</td><td>modules</td><td>xFull</td><td>ssd</td><td style="text-align:right">109</td><td>2026-06-23 01:33:40Z</td><td style="text-align:right">590,968 ms (9m 50s)</td><td style="text-align:right">7,214 ms (7s)</td><td style="text-align:right">22,128,790</td><td>jac313_store_005_XS (binary/on)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/clang/Release/ssd/xFull/modules/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>rhel-9.8</td><td>clang</td><td>Release</td><td>textual</td><td>xFull</td><td>ssd</td><td style="text-align:right">110</td><td>2026-06-23 01:43:49Z</td><td style="text-align:right">590,990 ms (9m 50s)</td><td style="text-align:right">-</td><td style="text-align:right">23,807,823</td><td>jac313_store_008_XS (flags/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/clang/Release/ssd/xFull/textual/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
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
<tr><td>rhel-9.8</td><td>gcc15</td><td>Debug</td><td>modules</td><td>Smoke</td><td>ssd</td><td style="text-align:right">103</td><td>2026-06-23 01:10:49Z</td><td style="text-align:right">16,232 ms (16s)</td><td style="text-align:right">6,862 ms (6s)</td><td style="text-align:right">3,676,471</td><td>jac313_store_005_XS (none/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/gcc15/Debug/ssd/Smoke/modules/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>rhel-9.8</td><td>gcc15</td><td>Debug</td><td>textual</td><td>Smoke</td><td>ssd</td><td style="text-align:right">104</td><td>2026-06-23 01:11:40Z</td><td style="text-align:right">16,232 ms (16s)</td><td style="text-align:right">-</td><td style="text-align:right">3,225,806</td><td>jac313_store_007_XS (none/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/gcc15/Debug/ssd/Smoke/textual/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
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
<tr><td>rhel-9.8</td><td>gcc15</td><td>Debug</td><td>modules</td><td>xFull</td><td>ssd</td><td style="text-align:right">111</td><td>2026-06-23 01:53:42Z</td><td style="text-align:right">599,147 ms (9m 59s)</td><td style="text-align:right">6,762 ms (6s)</td><td style="text-align:right">15,172,205</td><td>jac313_store_005_XS (sql/on)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/gcc15/Debug/ssd/xFull/modules/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>rhel-9.8</td><td>gcc15</td><td>Debug</td><td>textual</td><td>xFull</td><td>ssd</td><td style="text-align:right">112</td><td>2026-06-23 02:04:10Z</td><td style="text-align:right">599,451 ms (9m 59s)</td><td style="text-align:right">-</td><td style="text-align:right">17,803,415</td><td>jac313_store_008_XS (flags/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/gcc15/Debug/ssd/xFull/textual/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
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
<tr><td>rhel-9.8</td><td>gcc15</td><td>Release</td><td>modules</td><td>Smoke</td><td>ssd</td><td style="text-align:right">105</td><td>2026-06-23 01:11:58Z</td><td style="text-align:right">15,932 ms (15s)</td><td style="text-align:right">11,472 ms (11s)</td><td style="text-align:right">4,444,444</td><td>jac313_store_005_TS (none/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/gcc15/Release/ssd/Smoke/modules/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>rhel-9.8</td><td>gcc15</td><td>Release</td><td>textual</td><td>Smoke</td><td>ssd</td><td style="text-align:right">106</td><td>2026-06-23 01:13:10Z</td><td style="text-align:right">15,881 ms (15s)</td><td style="text-align:right">-</td><td style="text-align:right">5,050,505</td><td>jac313_store_005_TS (none/on)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/gcc15/Release/ssd/Smoke/textual/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
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
<tr><td>rhel-9.8</td><td>gcc15</td><td>Release</td><td>modules</td><td>xFull</td><td>ssd</td><td style="text-align:right">113</td><td>2026-06-23 02:14:11Z</td><td style="text-align:right">591,401 ms (9m 51s)</td><td style="text-align:right">11,873 ms (11s)</td><td style="text-align:right">25,001,250</td><td>jac313_store_008_TS (flags/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/gcc15/Release/ssd/xFull/modules/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>rhel-9.8</td><td>gcc15</td><td>Release</td><td>textual</td><td>xFull</td><td>ssd</td><td style="text-align:right">114</td><td>2026-06-23 02:24:48Z</td><td style="text-align:right">591,447 ms (9m 51s)</td><td style="text-align:right">-</td><td style="text-align:right">24,281,274</td><td>jac313_store_008_XS (flags/off)</td><td style="text-align:right">116</td><td style="text-align:right">0</td><td><a href="test-summary/rhel-9.8/gcc15/Release/ssd/xFull/textual/RUN.md">RUN.md</a></td></tr>
<tr><td colspan="15"><strong>Versions:</strong> Qlite v001.001 · jText v001.001 · Store v001.001</td></tr>
<tr><td colspan="15"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
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

_test_006 reports peak ops/sec as **N/A** — it is a tail-reader concurrency/correctness test, not a throughput benchmark, and emits no ops/sec line._

### almalinux-10.2 · clang (xFull, Debug, run 92)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

_Peak ops/sec is **N/A** for test_006: it is a tail-reader concurrency/correctness stress test (writer/reader timing + hit-miss verification), not a throughput benchmark, so it emits no `Fastest run → … ops/sec` line._

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
| jac313_store_006_TS | off | binary | 35,467 | N/A | 19.81 MiB |
|  |  | jtext | 35,569 | N/A | 20.42 MiB |
|  |  | sql | 34,885 | N/A | 23.51 MiB |
| jac313_store_006_TS | on | binary | 39,388 | N/A | 19.81 MiB |
|  |  | jtext | 39,934 | N/A | 20.42 MiB |
|  |  | sql | 39,379 | N/A | 23.51 MiB |
| jac313_store_006_XS | off | binary | 34,523 | N/A | 19.81 MiB |
|  |  | jtext | 34,873 | N/A | 19.96 MiB |
|  |  | sql | 34,618 | N/A | 22.64 MiB |
| jac313_store_006_XS | on | binary | 38,176 | N/A | 19.81 MiB |
|  |  | jtext | 38,588 | N/A | 19.96 MiB |
|  |  | sql | 38,432 | N/A | 22.64 MiB |
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

_Peak ops/sec is **N/A** for test_006: it is a tail-reader concurrency/correctness stress test (writer/reader timing + hit-miss verification), not a throughput benchmark, so it emits no `Fastest run → … ops/sec` line._

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
| jac313_store_006_TS | off | binary | 34,731 | N/A | 19.81 MiB |
|  |  | jtext | 34,971 | N/A | 20.41 MiB |
|  |  | sql | 35,368 | N/A | 23.50 MiB |
| jac313_store_006_TS | on | binary | 38,627 | N/A | 19.81 MiB |
|  |  | jtext | 38,662 | N/A | 20.40 MiB |
|  |  | sql | 39,258 | N/A | 23.50 MiB |
| jac313_store_006_XS | off | binary | 34,643 | N/A | 19.81 MiB |
|  |  | jtext | 34,588 | N/A | 19.96 MiB |
|  |  | sql | 34,368 | N/A | 22.63 MiB |
| jac313_store_006_XS | on | binary | 38,110 | N/A | 19.81 MiB |
|  |  | jtext | 37,882 | N/A | 19.96 MiB |
|  |  | sql | 38,437 | N/A | 22.63 MiB |
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

_Peak ops/sec is **N/A** for test_006: it is a tail-reader concurrency/correctness stress test (writer/reader timing + hit-miss verification), not a throughput benchmark, so it emits no `Fastest run → … ops/sec` line._

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
| jac313_store_006_TS | off | binary | 35,385 | N/A | 19.81 MiB |
|  |  | jtext | 35,772 | N/A | 20.42 MiB |
|  |  | sql | 35,209 | N/A | 23.50 MiB |
| jac313_store_006_TS | on | binary | 38,979 | N/A | 19.81 MiB |
|  |  | jtext | 39,853 | N/A | 20.42 MiB |
|  |  | sql | 39,159 | N/A | 23.50 MiB |
| jac313_store_006_XS | off | binary | 34,627 | N/A | 19.81 MiB |
|  |  | jtext | 35,099 | N/A | 19.96 MiB |
|  |  | sql | 34,582 | N/A | 22.64 MiB |
| jac313_store_006_XS | on | binary | 38,116 | N/A | 19.81 MiB |
|  |  | jtext | 38,844 | N/A | 19.96 MiB |
|  |  | sql | 38,373 | N/A | 22.64 MiB |
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

_Peak ops/sec is **N/A** for test_006: it is a tail-reader concurrency/correctness stress test (writer/reader timing + hit-miss verification), not a throughput benchmark, so it emits no `Fastest run → … ops/sec` line._

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
| jac313_store_006_TS | off | binary | 35,144 | N/A | 19.81 MiB |
|  |  | jtext | 35,337 | N/A | 20.40 MiB |
|  |  | sql | 34,957 | N/A | 23.50 MiB |
| jac313_store_006_TS | on | binary | 38,962 | N/A | 19.81 MiB |
|  |  | jtext | 38,960 | N/A | 20.40 MiB |
|  |  | sql | 39,253 | N/A | 23.50 MiB |
| jac313_store_006_XS | off | binary | 34,353 | N/A | 19.81 MiB |
|  |  | jtext | 34,179 | N/A | 19.96 MiB |
|  |  | sql | 34,327 | N/A | 22.63 MiB |
| jac313_store_006_XS | on | binary | 37,832 | N/A | 19.81 MiB |
|  |  | jtext | 38,181 | N/A | 19.96 MiB |
|  |  | sql | 38,212 | N/A | 22.63 MiB |
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

_Peak ops/sec is **N/A** for test_006: it is a tail-reader concurrency/correctness stress test (writer/reader timing + hit-miss verification), not a throughput benchmark, so it emits no `Fastest run → … ops/sec` line._

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
| jac313_store_006_TS | off | binary | 34,234 | N/A | 19.81 MiB |
|  |  | jtext | 34,533 | N/A | 20.42 MiB |
|  |  | sql | 34,286 | N/A | 23.50 MiB |
| jac313_store_006_TS | on | binary | 38,061 | N/A | 19.81 MiB |
|  |  | jtext | 38,356 | N/A | 20.42 MiB |
|  |  | sql | 38,407 | N/A | 23.50 MiB |
| jac313_store_006_XS | off | binary | 33,682 | N/A | 19.81 MiB |
|  |  | jtext | 34,161 | N/A | 19.96 MiB |
|  |  | sql | 33,633 | N/A | 22.64 MiB |
| jac313_store_006_XS | on | binary | 37,052 | N/A | 19.81 MiB |
|  |  | jtext | 37,424 | N/A | 19.96 MiB |
|  |  | sql | 37,357 | N/A | 22.64 MiB |
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

_Peak ops/sec is **N/A** for test_006: it is a tail-reader concurrency/correctness stress test (writer/reader timing + hit-miss verification), not a throughput benchmark, so it emits no `Fastest run → … ops/sec` line._

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
| jac313_store_006_TS | off | binary | 34,091 | N/A | 19.81 MiB |
|  |  | jtext | 33,938 | N/A | 20.40 MiB |
|  |  | sql | 33,935 | N/A | 23.51 MiB |
| jac313_store_006_TS | on | binary | 37,856 | N/A | 19.81 MiB |
|  |  | jtext | 37,807 | N/A | 20.40 MiB |
|  |  | sql | 38,161 | N/A | 23.51 MiB |
| jac313_store_006_XS | off | binary | 33,425 | N/A | 19.81 MiB |
|  |  | jtext | 33,278 | N/A | 19.96 MiB |
|  |  | sql | 33,335 | N/A | 22.63 MiB |
| jac313_store_006_XS | on | binary | 36,752 | N/A | 19.81 MiB |
|  |  | jtext | 36,695 | N/A | 19.96 MiB |
|  |  | sql | 37,100 | N/A | 22.63 MiB |
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

_Peak ops/sec is **N/A** for test_006: it is a tail-reader concurrency/correctness stress test (writer/reader timing + hit-miss verification), not a throughput benchmark, so it emits no `Fastest run → … ops/sec` line._

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
| jac313_store_006_TS | off | binary | 34,283 | N/A | 19.81 MiB |
|  |  | jtext | 34,534 | N/A | 20.42 MiB |
|  |  | sql | 34,138 | N/A | 23.50 MiB |
| jac313_store_006_TS | on | binary | 37,757 | N/A | 19.81 MiB |
|  |  | jtext | 38,357 | N/A | 20.42 MiB |
|  |  | sql | 38,357 | N/A | 23.50 MiB |
| jac313_store_006_XS | off | binary | 33,631 | N/A | 19.81 MiB |
|  |  | jtext | 34,179 | N/A | 19.96 MiB |
|  |  | sql | 33,633 | N/A | 22.64 MiB |
| jac313_store_006_XS | on | binary | 37,055 | N/A | 19.81 MiB |
|  |  | jtext | 37,360 | N/A | 19.96 MiB |
|  |  | sql | 37,404 | N/A | 22.64 MiB |
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

_Peak ops/sec is **N/A** for test_006: it is a tail-reader concurrency/correctness stress test (writer/reader timing + hit-miss verification), not a throughput benchmark, so it emits no `Fastest run → … ops/sec` line._

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
| jac313_store_006_TS | off | binary | 34,045 | N/A | 19.81 MiB |
|  |  | jtext | 33,891 | N/A | 20.40 MiB |
|  |  | sql | 33,939 | N/A | 23.49 MiB |
| jac313_store_006_TS | on | binary | 37,668 | N/A | 19.81 MiB |
|  |  | jtext | 37,759 | N/A | 20.40 MiB |
|  |  | sql | 38,060 | N/A | 23.49 MiB |
| jac313_store_006_XS | off | binary | 33,385 | N/A | 19.81 MiB |
|  |  | jtext | 33,344 | N/A | 19.96 MiB |
|  |  | sql | 33,290 | N/A | 22.63 MiB |
| jac313_store_006_XS | on | binary | 36,843 | N/A | 19.81 MiB |
|  |  | jtext | 36,707 | N/A | 19.96 MiB |
|  |  | sql | 36,962 | N/A | 22.63 MiB |
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

_Peak ops/sec is **N/A** for test_006: it is a tail-reader concurrency/correctness stress test (writer/reader timing + hit-miss verification), not a throughput benchmark, so it emits no `Fastest run → … ops/sec` line._

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
| jac313_store_006_TS | off | binary | 34,211 | N/A | 19.81 MiB |
|  |  | jtext | 34,815 | N/A | 20.42 MiB |
|  |  | sql | 34,276 | N/A | 23.46 MiB |
| jac313_store_006_TS | on | binary | 38,078 | N/A | 19.81 MiB |
|  |  | jtext | 38,440 | N/A | 20.42 MiB |
|  |  | sql | 38,539 | N/A | 23.46 MiB |
| jac313_store_006_XS | off | binary | 33,635 | N/A | 19.81 MiB |
|  |  | jtext | 34,254 | N/A | 19.96 MiB |
|  |  | sql | 33,632 | N/A | 22.61 MiB |
| jac313_store_006_XS | on | binary | 37,139 | N/A | 19.81 MiB |
|  |  | jtext | 37,361 | N/A | 19.96 MiB |
|  |  | sql | 37,598 | N/A | 22.61 MiB |
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

_Peak ops/sec is **N/A** for test_006: it is a tail-reader concurrency/correctness stress test (writer/reader timing + hit-miss verification), not a throughput benchmark, so it emits no `Fastest run → … ops/sec` line._

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
| jac313_store_006_TS | off | binary | 33,935 | N/A | 19.81 MiB |
|  |  | jtext | 33,963 | N/A | 20.40 MiB |
|  |  | sql | 33,926 | N/A | 23.47 MiB |
| jac313_store_006_TS | on | binary | 37,704 | N/A | 19.81 MiB |
|  |  | jtext | 37,800 | N/A | 20.39 MiB |
|  |  | sql | 38,290 | N/A | 23.47 MiB |
| jac313_store_006_XS | off | binary | 33,314 | N/A | 19.81 MiB |
|  |  | jtext | 33,403 | N/A | 19.96 MiB |
|  |  | sql | 33,366 | N/A | 22.62 MiB |
| jac313_store_006_XS | on | binary | 36,785 | N/A | 19.81 MiB |
|  |  | jtext | 36,789 | N/A | 19.96 MiB |
|  |  | sql | 37,246 | N/A | 22.62 MiB |
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

_Peak ops/sec is **N/A** for test_006: it is a tail-reader concurrency/correctness stress test (writer/reader timing + hit-miss verification), not a throughput benchmark, so it emits no `Fastest run → … ops/sec` line._

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
| jac313_store_006_TS | off | binary | 34,155 | N/A | 19.81 MiB |
|  |  | jtext | 34,478 | N/A | 20.42 MiB |
|  |  | sql | 34,268 | N/A | 23.48 MiB |
| jac313_store_006_TS | on | binary | 37,986 | N/A | 19.81 MiB |
|  |  | jtext | 38,435 | N/A | 20.41 MiB |
|  |  | sql | 38,534 | N/A | 23.48 MiB |
| jac313_store_006_XS | off | binary | 33,593 | N/A | 19.81 MiB |
|  |  | jtext | 34,050 | N/A | 19.96 MiB |
|  |  | sql | 33,509 | N/A | 22.62 MiB |
| jac313_store_006_XS | on | binary | 37,245 | N/A | 19.81 MiB |
|  |  | jtext | 37,338 | N/A | 19.96 MiB |
|  |  | sql | 37,533 | N/A | 22.62 MiB |
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

_Peak ops/sec is **N/A** for test_006: it is a tail-reader concurrency/correctness stress test (writer/reader timing + hit-miss verification), not a throughput benchmark, so it emits no `Fastest run → … ops/sec` line._

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
| jac313_store_006_TS | off | binary | 33,924 | N/A | 19.81 MiB |
|  |  | jtext | 34,021 | N/A | 20.38 MiB |
|  |  | sql | 33,900 | N/A | 23.47 MiB |
| jac313_store_006_TS | on | binary | 37,753 | N/A | 19.81 MiB |
|  |  | jtext | 37,848 | N/A | 20.38 MiB |
|  |  | sql | 38,164 | N/A | 23.47 MiB |
| jac313_store_006_XS | off | binary | 33,226 | N/A | 19.81 MiB |
|  |  | jtext | 33,377 | N/A | 19.96 MiB |
|  |  | sql | 33,366 | N/A | 22.62 MiB |
| jac313_store_006_XS | on | binary | 36,874 | N/A | 19.81 MiB |
|  |  | jtext | 36,834 | N/A | 19.96 MiB |
|  |  | sql | 37,267 | N/A | 22.62 MiB |
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

### rhel-9.8 · clang (xFull, Debug, run 108)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

_Peak ops/sec is **N/A** for test_006: it is a tail-reader concurrency/correctness stress test (writer/reader timing + hit-miss verification), not a throughput benchmark, so it emits no `Fastest run → … ops/sec` line._

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_005_TS | off | binary | 150 | 11,625,203 | 8.38 MiB |
|  |  | jtext | 350 | 10,765,421 | 7.82 MiB |
|  |  | sql | 150 | 11,258,726 | 7.42 MiB |
| jac313_store_005_TS | on | binary | 150 | 9,507,511 | 8.38 MiB |
|  |  | jtext | 351 | 12,410,027 | 7.82 MiB |
|  |  | sql | 150 | 9,844,458 | 7.42 MiB |
| jac313_store_005_XS | off | binary | 150 | 10,566,357 | 8.38 MiB |
|  |  | jtext | 351 | 12,059,817 | 7.44 MiB |
|  |  | sql | 150 | 11,101,243 | 7.13 MiB |
| jac313_store_005_XS | on | binary | 150 | 11,896,265 | 8.38 MiB |
|  |  | jtext | 401 | 9,546,539 | 7.44 MiB |
|  |  | sql | 150 | 10,548,523 | 7.13 MiB |
| jac313_store_006_TS | off | binary | 33,654 | N/A | 19.81 MiB |
|  |  | jtext | 33,812 | N/A | 20.34 MiB |
|  |  | sql | 33,762 | N/A | 23.39 MiB |
| jac313_store_006_TS | on | binary | 37,530 | N/A | 19.81 MiB |
|  |  | jtext | 37,631 | N/A | 20.33 MiB |
|  |  | sql | 37,561 | N/A | 23.39 MiB |
| jac313_store_006_XS | off | binary | 33,107 | N/A | 19.81 MiB |
|  |  | jtext | 33,212 | N/A | 19.96 MiB |
|  |  | sql | 33,155 | N/A | 22.65 MiB |
| jac313_store_006_XS | on | binary | 36,571 | N/A | 19.81 MiB |
|  |  | jtext | 36,575 | N/A | 19.96 MiB |
|  |  | sql | 36,626 | N/A | 22.65 MiB |
| jac313_store_007_TS | off | binary | 150 | 12,716,175 | 19.82 MiB |
|  |  | jtext | 1,002 | 9,742,790 | 20.37 MiB |
|  |  | sql | 250 | 10,938,525 | 23.52 MiB |
| jac313_store_007_TS | on | binary | 150 | 10,110,201 | 19.82 MiB |
|  |  | jtext | 1,003 | 10,275,380 | 20.37 MiB |
|  |  | sql | 351 | 9,011,445 | 23.52 MiB |
| jac313_store_007_XS | off | binary | 150 | 11,104,942 | 19.82 MiB |
|  |  | jtext | 1,002 | 10,772,380 | 19.98 MiB |
|  |  | sql | 250 | 9,604,303 | 22.66 MiB |
| jac313_store_007_XS | on | binary | 150 | 11,354,604 | 19.82 MiB |
|  |  | jtext | 1,002 | 10,047,222 | 19.98 MiB |
|  |  | sql | 351 | 11,432,491 | 22.66 MiB |

### rhel-9.8 · clang (xFull, Release, run 110)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

_Peak ops/sec is **N/A** for test_006: it is a tail-reader concurrency/correctness stress test (writer/reader timing + hit-miss verification), not a throughput benchmark, so it emits no `Fastest run → … ops/sec` line._

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_005_TS | off | binary | 100 | 19,238,169 | 8.38 MiB |
|  |  | jtext | 100 | 19,700,552 | 7.82 MiB |
|  |  | sql | 100 | 18,796,992 | 7.38 MiB |
| jac313_store_005_TS | on | binary | 100 | 18,730,099 | 8.38 MiB |
|  |  | jtext | 100 | 19,813,751 | 7.82 MiB |
|  |  | sql | 150 | 15,842,839 | 7.38 MiB |
| jac313_store_005_XS | off | binary | 101 | 19,252,984 | 8.38 MiB |
|  |  | jtext | 100 | 14,384,350 | 7.44 MiB |
|  |  | sql | 100 | 15,701,052 | 7.13 MiB |
| jac313_store_005_XS | on | binary | 100 | 18,525,380 | 8.38 MiB |
|  |  | jtext | 100 | 13,585,111 | 7.44 MiB |
|  |  | sql | 150 | 15,681,355 | 7.13 MiB |
| jac313_store_006_TS | off | binary | 33,613 | N/A | 19.81 MiB |
|  |  | jtext | 33,708 | N/A | 20.32 MiB |
|  |  | sql | 33,711 | N/A | 23.29 MiB |
| jac313_store_006_TS | on | binary | 37,424 | N/A | 19.81 MiB |
|  |  | jtext | 37,524 | N/A | 20.32 MiB |
|  |  | sql | 37,522 | N/A | 23.29 MiB |
| jac313_store_006_XS | off | binary | 33,009 | N/A | 19.81 MiB |
|  |  | jtext | 33,107 | N/A | 19.96 MiB |
|  |  | sql | 33,113 | N/A | 22.63 MiB |
| jac313_store_006_XS | on | binary | 36,527 | N/A | 19.81 MiB |
|  |  | jtext | 36,524 | N/A | 19.96 MiB |
|  |  | sql | 36,419 | N/A | 22.63 MiB |
| jac313_store_007_TS | off | binary | 100 | 14,731,880 | 19.82 MiB |
|  |  | jtext | 200 | 20,408,163 | 20.36 MiB |
|  |  | sql | 200 | 20,296,326 | 23.44 MiB |
| jac313_store_007_TS | on | binary | 100 | 17,905,103 | 19.82 MiB |
|  |  | jtext | 200 | 21,819,769 | 20.36 MiB |
|  |  | sql | 250 | 15,076,134 | 23.44 MiB |
| jac313_store_007_XS | off | binary | 100 | 21,724,962 | 19.82 MiB |
|  |  | jtext | 200 | 14,518,002 | 19.98 MiB |
|  |  | sql | 200 | 20,907,380 | 22.64 MiB |
| jac313_store_007_XS | on | binary | 100 | 21,953,897 | 19.82 MiB |
|  |  | jtext | 200 | 16,307,893 | 19.98 MiB |
|  |  | sql | 250 | 19,758,941 | 22.64 MiB |

### rhel-9.8 · gcc15 (xFull, Debug, run 112)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

_Peak ops/sec is **N/A** for test_006: it is a tail-reader concurrency/correctness stress test (writer/reader timing + hit-miss verification), not a throughput benchmark, so it emits no `Fastest run → … ops/sec` line._

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_005_TS | off | binary | 150 | 14,417,532 | 8.38 MiB |
|  |  | jtext | 400 | 11,761,938 | 7.83 MiB |
|  |  | sql | 150 | 11,592,859 | 7.42 MiB |
| jac313_store_005_TS | on | binary | 150 | 9,544,717 | 8.38 MiB |
|  |  | jtext | 401 | 14,064,698 | 7.82 MiB |
|  |  | sql | 200 | 12,825,446 | 7.42 MiB |
| jac313_store_005_XS | off | binary | 150 | 15,728,216 | 8.38 MiB |
|  |  | jtext | 401 | 11,270,145 | 7.44 MiB |
|  |  | sql | 150 | 14,889,815 | 7.13 MiB |
| jac313_store_005_XS | on | binary | 150 | 15,236,934 | 8.38 MiB |
|  |  | jtext | 401 | 12,987,013 | 7.44 MiB |
|  |  | sql | 200 | 15,316,281 | 7.13 MiB |
| jac313_store_006_TS | off | binary | 33,701 | N/A | 19.81 MiB |
|  |  | jtext | 33,809 | N/A | 20.34 MiB |
|  |  | sql | 33,738 | N/A | 23.38 MiB |
| jac313_store_006_TS | on | binary | 37,531 | N/A | 19.81 MiB |
|  |  | jtext | 37,577 | N/A | 20.33 MiB |
|  |  | sql | 37,579 | N/A | 23.38 MiB |
| jac313_store_006_XS | off | binary | 33,087 | N/A | 19.81 MiB |
|  |  | jtext | 33,190 | N/A | 19.96 MiB |
|  |  | sql | 33,162 | N/A | 22.64 MiB |
| jac313_store_006_XS | on | binary | 36,551 | N/A | 19.81 MiB |
|  |  | jtext | 36,649 | N/A | 19.96 MiB |
|  |  | sql | 36,574 | N/A | 22.64 MiB |
| jac313_store_007_TS | off | binary | 150 | 12,068,549 | 19.82 MiB |
|  |  | jtext | 1,103 | 11,732,958 | 20.37 MiB |
|  |  | sql | 250 | 11,457,379 | 23.51 MiB |
| jac313_store_007_TS | on | binary | 150 | 11,448,197 | 19.82 MiB |
|  |  | jtext | 1,103 | 11,186,934 | 20.37 MiB |
|  |  | sql | 300 | 10,688,328 | 23.51 MiB |
| jac313_store_007_XS | off | binary | 150 | 12,846,865 | 19.82 MiB |
|  |  | jtext | 1,103 | 13,238,020 | 19.98 MiB |
|  |  | sql | 250 | 10,671,220 | 22.66 MiB |
| jac313_store_007_XS | on | binary | 150 | 13,789,300 | 19.82 MiB |
|  |  | jtext | 1,103 | 14,202,528 | 19.98 MiB |
|  |  | sql | 300 | 10,525,208 | 22.66 MiB |

### rhel-9.8 · gcc15 (xFull, Release, run 114)

Wall-clock **run ms** (matrix scenario), **peak ops/sec** (005–008 logs only), and on-disk **persist artifact size** per backend.

_Peak ops/sec is **N/A** for test_006: it is a tail-reader concurrency/correctness stress test (writer/reader timing + hit-miss verification), not a throughput benchmark, so it emits no `Fastest run → … ops/sec` line._

| Test | Output | Persist | run ms | Peak ops/sec | Log size |
|------|--------|---------|--------|--------------|----------|
| jac313_store_005_TS | off | binary | 100 | 17,793,594 | 8.38 MiB |
|  |  | jtext | 100 | 14,742,739 | 7.82 MiB |
|  |  | sql | 100 | 13,092,433 | 7.38 MiB |
| jac313_store_005_TS | on | binary | 100 | 19,829,467 | 8.38 MiB |
|  |  | jtext | 100 | 15,339,776 | 7.82 MiB |
|  |  | sql | 150 | 14,300,014 | 7.38 MiB |
| jac313_store_005_XS | off | binary | 100 | 22,182,786 | 8.38 MiB |
|  |  | jtext | 100 | 14,283,674 | 7.44 MiB |
|  |  | sql | 100 | 15,432,099 | 7.13 MiB |
| jac313_store_005_XS | on | binary | 100 | 14,017,382 | 8.38 MiB |
|  |  | jtext | 100 | 24,148,756 | 7.44 MiB |
|  |  | sql | 150 | 14,457,135 | 7.13 MiB |
| jac313_store_006_TS | off | binary | 33,711 | N/A | 19.81 MiB |
|  |  | jtext | 33,716 | N/A | 20.33 MiB |
|  |  | sql | 33,711 | N/A | 23.27 MiB |
| jac313_store_006_TS | on | binary | 37,523 | N/A | 19.81 MiB |
|  |  | jtext | 37,535 | N/A | 20.32 MiB |
|  |  | sql | 37,533 | N/A | 23.27 MiB |
| jac313_store_006_XS | off | binary | 33,107 | N/A | 19.81 MiB |
|  |  | jtext | 33,090 | N/A | 19.96 MiB |
|  |  | sql | 33,086 | N/A | 22.63 MiB |
| jac313_store_006_XS | on | binary | 36,518 | N/A | 19.81 MiB |
|  |  | jtext | 36,573 | N/A | 19.96 MiB |
|  |  | sql | 36,547 | N/A | 22.63 MiB |
| jac313_store_007_TS | off | binary | 100 | 15,363,343 | 19.82 MiB |
|  |  | jtext | 200 | 20,933,640 | 20.36 MiB |
|  |  | sql | 200 | 19,190,175 | 23.43 MiB |
| jac313_store_007_TS | on | binary | 100 | 13,795,006 | 19.82 MiB |
|  |  | jtext | 200 | 15,382,249 | 20.36 MiB |
|  |  | sql | 250 | 14,590,020 | 23.43 MiB |
| jac313_store_007_XS | off | binary | 100 | 14,072,615 | 19.82 MiB |
|  |  | jtext | 200 | 14,751,438 | 19.98 MiB |
|  |  | sql | 200 | 13,478,906 | 22.65 MiB |
| jac313_store_007_XS | on | binary | 100 | 14,092,446 | 19.82 MiB |
|  |  | jtext | 200 | 13,147,515 | 19.98 MiB |
|  |  | sql | 250 | 14,392,631 | 22.65 MiB |


## Top throughput by log category

Peak ops/sec from benchmark tests 005–008 (`Fastest run` in each log). xFull matrix only, grouped by persist log directory.

### binary_logs

<table>
<thead>
<tr><th>#</th><th>Peak ops/sec</th><th>Test</th><th>Persist</th><th>ms</th><th>Log</th></tr>
</thead>
<tbody>
<tr><td>1</td><td style="text-align:right">22,517,451</td><td>jac313_store_007_XS</td><td>binary/off</td><td style="text-align:right">100</td><td><a href="rhel-9.8/gcc15/Release/ssd/xFull/modules/binary_logs/JAC313_STORE_TEST_007_XS/gcc15_binary_off.log">gcc15_binary_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>2</td><td style="text-align:right">22,182,786</td><td>jac313_store_005_XS</td><td>binary/off</td><td style="text-align:right">100</td><td><a href="rhel-9.8/gcc15/Release/ssd/xFull/textual/binary_logs/JAC313_STORE_TEST_005_XS/gcc15_binary_off.log">gcc15_binary_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>3</td><td style="text-align:right">22,128,790</td><td>jac313_store_005_XS</td><td>binary/on</td><td style="text-align:right">100</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/modules/binary_logs/JAC313_STORE_TEST_005_XS/clang_binary_on.log">clang_binary_on.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>4</td><td style="text-align:right">21,953,897</td><td>jac313_store_007_XS</td><td>binary/on</td><td style="text-align:right">100</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/textual/binary_logs/JAC313_STORE_TEST_007_XS/clang_binary_on.log">clang_binary_on.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>5</td><td style="text-align:right">21,724,962</td><td>jac313_store_007_XS</td><td>binary/off</td><td style="text-align:right">100</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/textual/binary_logs/JAC313_STORE_TEST_007_XS/clang_binary_off.log">clang_binary_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>6</td><td style="text-align:right">21,190,930</td><td>jac313_store_007_XS</td><td>binary/on</td><td style="text-align:right">100</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/modules/binary_logs/JAC313_STORE_TEST_007_XS/clang_binary_on.log">clang_binary_on.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>7</td><td style="text-align:right">20,275,750</td><td>jac313_store_007_XS</td><td>binary/off</td><td style="text-align:right">100</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/modules/binary_logs/JAC313_STORE_TEST_007_XS/clang_binary_off.log">clang_binary_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>8</td><td style="text-align:right">19,829,467</td><td>jac313_store_005_TS</td><td>binary/on</td><td style="text-align:right">100</td><td><a href="rhel-9.8/gcc15/Release/ssd/xFull/textual/binary_logs/JAC313_STORE_TEST_005_TS/gcc15_binary_on.log">gcc15_binary_on.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>9</td><td style="text-align:right">19,252,984</td><td>jac313_store_005_XS</td><td>binary/off</td><td style="text-align:right">101</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/textual/binary_logs/JAC313_STORE_TEST_005_XS/clang_binary_off.log">clang_binary_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>10</td><td style="text-align:right">19,238,169</td><td>jac313_store_005_TS</td><td>binary/off</td><td style="text-align:right">100</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/textual/binary_logs/JAC313_STORE_TEST_005_TS/clang_binary_off.log">clang_binary_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
</tbody>
</table>

### inmem_logs

<table>
<thead>
<tr><th>#</th><th>Peak ops/sec</th><th>Test</th><th>Persist</th><th>ms</th><th>Log</th></tr>
</thead>
<tbody>
<tr><td>1</td><td style="text-align:right">23,052,098</td><td>jac313_store_005_XS</td><td>none/on</td><td style="text-align:right">50</td><td><a href="rhel-9.8/gcc15/Release/ssd/xFull/modules/inmem_logs/JAC313_STORE_TEST_005_XS/gcc15_none_on.log">gcc15_none_on.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>2</td><td style="text-align:right">22,261,799</td><td>jac313_store_005_XS</td><td>none/on</td><td style="text-align:right">50</td><td><a href="rhel-9.8/gcc15/Release/ssd/xFull/textual/inmem_logs/JAC313_STORE_TEST_005_XS/gcc15_none_on.log">gcc15_none_on.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>3</td><td style="text-align:right">22,237,047</td><td>jac313_store_005_XS</td><td>none/off</td><td style="text-align:right">50</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/textual/inmem_logs/JAC313_STORE_TEST_005_XS/clang_none_off.log">clang_none_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>4</td><td style="text-align:right">22,128,790</td><td>jac313_store_005_TS</td><td>none/on</td><td style="text-align:right">50</td><td><a href="rhel-9.8/gcc15/Release/ssd/xFull/textual/inmem_logs/JAC313_STORE_TEST_005_TS/gcc15_none_on.log">gcc15_none_on.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>5</td><td style="text-align:right">21,687,270</td><td>jac313_store_007_TS</td><td>none/off</td><td style="text-align:right">50</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/textual/inmem_logs/JAC313_STORE_TEST_007_TS/clang_none_off.log">clang_none_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>6</td><td style="text-align:right">21,547,080</td><td>jac313_store_007_TS</td><td>none/on</td><td style="text-align:right">50</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/textual/inmem_logs/JAC313_STORE_TEST_007_TS/clang_none_on.log">clang_none_on.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>7</td><td style="text-align:right">21,482,277</td><td>jac313_store_007_XS</td><td>none/on</td><td style="text-align:right">50</td><td><a href="rhel-9.8/gcc15/Release/ssd/xFull/modules/inmem_logs/JAC313_STORE_TEST_007_XS/gcc15_none_on.log">gcc15_none_on.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>8</td><td style="text-align:right">21,376,657</td><td>jac313_store_005_TS</td><td>none/off</td><td style="text-align:right">50</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/modules/inmem_logs/JAC313_STORE_TEST_005_TS/clang_none_off.log">clang_none_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>9</td><td style="text-align:right">21,235,931</td><td>jac313_store_005_XS</td><td>none/off</td><td style="text-align:right">50</td><td><a href="rhel-9.8/gcc15/Release/ssd/xFull/textual/inmem_logs/JAC313_STORE_TEST_005_XS/gcc15_none_off.log">gcc15_none_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>10</td><td style="text-align:right">21,021,652</td><td>jac313_store_005_TS</td><td>none/off</td><td style="text-align:right">50</td><td><a href="rhel-9.8/gcc15/Release/ssd/xFull/modules/inmem_logs/JAC313_STORE_TEST_005_TS/gcc15_none_off.log">gcc15_none_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
</tbody>
</table>

### sql_logs

<table>
<thead>
<tr><th>#</th><th>Peak ops/sec</th><th>Test</th><th>Persist</th><th>ms</th><th>Log</th></tr>
</thead>
<tbody>
<tr><td>1</td><td style="text-align:right">21,696,680</td><td>jac313_store_005_XS</td><td>sql/off</td><td style="text-align:right">100</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/modules/sql_logs/JAC313_STORE_TEST_005_XS/clang_sql_off.log">clang_sql_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>2</td><td style="text-align:right">21,445,421</td><td>jac313_store_005_TS</td><td>sql/on</td><td style="text-align:right">150</td><td><a href="rhel-9.8/gcc15/Release/ssd/xFull/modules/sql_logs/JAC313_STORE_TEST_005_TS/gcc15_sql_on.log">gcc15_sql_on.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>3</td><td style="text-align:right">20,907,380</td><td>jac313_store_007_XS</td><td>sql/off</td><td style="text-align:right">200</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/textual/sql_logs/JAC313_STORE_TEST_007_XS/clang_sql_off.log">clang_sql_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>4</td><td style="text-align:right">20,296,326</td><td>jac313_store_007_TS</td><td>sql/off</td><td style="text-align:right">200</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/textual/sql_logs/JAC313_STORE_TEST_007_TS/clang_sql_off.log">clang_sql_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>5</td><td style="text-align:right">19,758,941</td><td>jac313_store_007_XS</td><td>sql/on</td><td style="text-align:right">250</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/textual/sql_logs/JAC313_STORE_TEST_007_XS/clang_sql_on.log">clang_sql_on.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>6</td><td style="text-align:right">19,455,253</td><td>jac313_store_007_TS</td><td>sql/off</td><td style="text-align:right">200</td><td><a href="rhel-9.8/gcc15/Release/ssd/xFull/modules/sql_logs/JAC313_STORE_TEST_007_TS/gcc15_sql_off.log">gcc15_sql_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>7</td><td style="text-align:right">19,190,175</td><td>jac313_store_007_TS</td><td>sql/off</td><td style="text-align:right">200</td><td><a href="rhel-9.8/gcc15/Release/ssd/xFull/textual/sql_logs/JAC313_STORE_TEST_007_TS/gcc15_sql_off.log">gcc15_sql_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>8</td><td style="text-align:right">18,796,992</td><td>jac313_store_005_TS</td><td>sql/off</td><td style="text-align:right">100</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/textual/sql_logs/JAC313_STORE_TEST_005_TS/clang_sql_off.log">clang_sql_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>9</td><td style="text-align:right">17,470,300</td><td>jac313_store_007_XS</td><td>sql/off</td><td style="text-align:right">200</td><td><a href="rhel-9.8/gcc15/Release/ssd/xFull/modules/sql_logs/JAC313_STORE_TEST_007_XS/gcc15_sql_off.log">gcc15_sql_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>10</td><td style="text-align:right">15,842,839</td><td>jac313_store_005_TS</td><td>sql/on</td><td style="text-align:right">150</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/textual/sql_logs/JAC313_STORE_TEST_005_TS/clang_sql_on.log">clang_sql_on.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
</tbody>
</table>

### jText_logs

<table>
<thead>
<tr><th>#</th><th>Peak ops/sec</th><th>Test</th><th>Persist</th><th>ms</th><th>Log</th></tr>
</thead>
<tbody>
<tr><td>1</td><td style="text-align:right">24,148,756</td><td>jac313_store_005_XS</td><td>jtext/on</td><td style="text-align:right">100</td><td><a href="rhel-9.8/gcc15/Release/ssd/xFull/textual/jText_logs/JAC313_STORE_TEST_005_XS/gcc15_jtext_on.log">gcc15_jtext_on.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>2</td><td style="text-align:right">23,364,486</td><td>jac313_store_005_XS</td><td>jtext/off</td><td style="text-align:right">100</td><td><a href="rhel-9.8/gcc15/Release/ssd/xFull/modules/jText_logs/JAC313_STORE_TEST_005_XS/gcc15_jtext_off.log">gcc15_jtext_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>3</td><td style="text-align:right">21,819,769</td><td>jac313_store_007_TS</td><td>jtext/on</td><td style="text-align:right">200</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/textual/jText_logs/JAC313_STORE_TEST_007_TS/clang_jtext_on.log">clang_jtext_on.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>4</td><td style="text-align:right">21,635,656</td><td>jac313_store_007_TS</td><td>jtext/on</td><td style="text-align:right">200</td><td><a href="rhel-9.8/gcc15/Release/ssd/xFull/modules/jText_logs/JAC313_STORE_TEST_007_TS/gcc15_jtext_on.log">gcc15_jtext_on.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>5</td><td style="text-align:right">21,602,938</td><td>jac313_store_007_TS</td><td>jtext/off</td><td style="text-align:right">200</td><td><a href="rhel-9.8/gcc15/Release/ssd/xFull/modules/jText_logs/JAC313_STORE_TEST_007_TS/gcc15_jtext_off.log">gcc15_jtext_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>6</td><td style="text-align:right">20,933,640</td><td>jac313_store_007_TS</td><td>jtext/off</td><td style="text-align:right">200</td><td><a href="rhel-9.8/gcc15/Release/ssd/xFull/textual/jText_logs/JAC313_STORE_TEST_007_TS/gcc15_jtext_off.log">gcc15_jtext_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>7</td><td style="text-align:right">20,487,605</td><td>jac313_store_007_XS</td><td>jtext/off</td><td style="text-align:right">200</td><td><a href="rhel-9.8/gcc15/Release/ssd/xFull/modules/jText_logs/JAC313_STORE_TEST_007_XS/gcc15_jtext_off.log">gcc15_jtext_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>8</td><td style="text-align:right">20,408,163</td><td>jac313_store_007_TS</td><td>jtext/off</td><td style="text-align:right">200</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/textual/jText_logs/JAC313_STORE_TEST_007_TS/clang_jtext_off.log">clang_jtext_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>9</td><td style="text-align:right">19,884,669</td><td>jac313_store_005_XS</td><td>jtext/off</td><td style="text-align:right">100</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/modules/jText_logs/JAC313_STORE_TEST_005_XS/clang_jtext_off.log">clang_jtext_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>10</td><td style="text-align:right">19,813,751</td><td>jac313_store_005_TS</td><td>jtext/on</td><td style="text-align:right">100</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/textual/jText_logs/JAC313_STORE_TEST_005_TS/clang_jtext_on.log">clang_jtext_on.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
</tbody>
</table>

### flags_logs

<table>
<thead>
<tr><th>#</th><th>Peak ops/sec</th><th>Test</th><th>Persist</th><th>ms</th><th>Log</th></tr>
</thead>
<tbody>
<tr><td>1</td><td style="text-align:right">25,001,250</td><td>jac313_store_008_TS</td><td>flags/off</td><td style="text-align:right">651</td><td><a href="rhel-9.8/gcc15/Release/ssd/xFull/modules/flags_logs/JAC313_STORE_TEST_008_TS/gcc15_flags_off.log">gcc15_flags_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>2</td><td style="text-align:right">24,287,761</td><td>jac313_store_008_XS</td><td>flags/off</td><td style="text-align:right">651</td><td><a href="rhel-9.8/gcc15/Release/ssd/xFull/modules/flags_logs/JAC313_STORE_TEST_008_XS/gcc15_flags_off.log">gcc15_flags_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>3</td><td style="text-align:right">24,281,274</td><td>jac313_store_008_XS</td><td>flags/off</td><td style="text-align:right">651</td><td><a href="rhel-9.8/gcc15/Release/ssd/xFull/textual/flags_logs/JAC313_STORE_TEST_008_XS/gcc15_flags_off.log">gcc15_flags_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>4</td><td style="text-align:right">23,807,823</td><td>jac313_store_008_XS</td><td>flags/off</td><td style="text-align:right">651</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/textual/flags_logs/JAC313_STORE_TEST_008_XS/clang_flags_off.log">clang_flags_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>5</td><td style="text-align:right">17,803,415</td><td>jac313_store_008_XS</td><td>flags/off</td><td style="text-align:right">1,102</td><td><a href="rhel-9.8/gcc15/Debug/ssd/xFull/textual/flags_logs/JAC313_STORE_TEST_008_XS/gcc15_flags_off.log">gcc15_flags_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>6</td><td style="text-align:right">17,154,130</td><td>jac313_store_008_TS</td><td>flags/off</td><td style="text-align:right">752</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/modules/flags_logs/JAC313_STORE_TEST_008_TS/clang_flags_off.log">clang_flags_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>7</td><td style="text-align:right">17,152,659</td><td>jac313_store_008_TS</td><td>flags/off</td><td style="text-align:right">651</td><td><a href="rhel-9.8/gcc15/Release/ssd/xFull/textual/flags_logs/JAC313_STORE_TEST_008_TS/gcc15_flags_off.log">gcc15_flags_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>8</td><td style="text-align:right">17,146,188</td><td>jac313_store_008_TS</td><td>flags/off</td><td style="text-align:right">701</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/textual/flags_logs/JAC313_STORE_TEST_008_TS/clang_flags_off.log">clang_flags_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>9</td><td style="text-align:right">16,632,847</td><td>jac313_store_008_TS</td><td>flags/off</td><td style="text-align:right">1,102</td><td><a href="rhel-9.8/gcc15/Debug/ssd/xFull/textual/flags_logs/JAC313_STORE_TEST_008_TS/gcc15_flags_off.log">gcc15_flags_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
<tr><td>10</td><td style="text-align:right">16,444,393</td><td>jac313_store_008_XS</td><td>flags/off</td><td style="text-align:right">701</td><td><a href="rhel-9.8/clang/Release/ssd/xFull/modules/flags_logs/JAC313_STORE_TEST_008_XS/clang_flags_off.log">clang_flags_off.log</a></td></tr>
<tr><td colspan="6"><strong>Hardware:</strong> j5c-base-001 · Red Hat Enterprise Linux 9.8 (Plow) (5.14.0-687.15.1.el9_8.x86_64) · Intel(R) Core(TM) Ultra 7 265 · 62 GB RAM · WD\x20PC\x20SN740\x20SDDQNQD-1T00-1201 (ssd label)</td></tr>
</tbody>
</table>

