# Store log viewer — planned project

**Status:** parked — design captured now; implementation starts **sometime in the near future** (not in the current Store persistence/bench pass).

← [Store docs index](../../Store/README.md) · [reporting](reporting.md) · [persistence](persistence.md)

---

## The insight

Store's multi-threaded hot path is working: events are claimed, written, and flushed to durable
sinks at scale (1M / 10M bench configs, real ops/sec numbers). What we **do not** have yet is a
way to **browse** those captures interactively after the run — only:

- **Console streaming** while tests execute (`prod.print()`, matrix logs with ANSI color), and
- **External tools** per format (gedit/vim on jText, **any browser on HTML**, `sqlite3` on SQL,
  `jq` line-by-line on JSON).

Two formats are **instantly human-readable** without a custom app (see §9): **jText** in an editor,
**HTML** in a browser — open the file and scroll. What they still lack is **structured filtering**
(category, relative time range) and comfortable **10M-row** navigation; grep and browser Find do
not replace that. **Binary** and **JSON** additionally have no instant human view at all.

This document plans a **wxWidgets desktop log viewer** as a separate package in the jac313 tree,
not a Store-core feature.

---

## Goals

1. **Three panes** — Events (main), Ints, Floats — virtual lists aligned to the split persistence
   layout. Scroll **independently** or **lock by `id`** so all three move together on the same record.
2. **Filters** (all supported formats — jText and HTML included)
   - **Category** — dropdown populated from capture metadata (NET, DB, UI, …).
   - **Relative time range** — two text boxes (low / high), in microseconds or a normalized
     offset from capture start (TBD in UI spec).
   - Grep and browser Find are poor substitutes at 1M+ rows; structured filters are a main reason
     to open jText/HTML in the viewer, not just binary/JSON.
3. **Load screen** — pick a capture set (format + base path); viewer loads the right reader(s).
4. **Fast open at scale** — must not require a full pre-scan of 10M rows just to size containers
   or paint the first frame.
5. **C++23** for all viewer code (deliberately **not** C++26 — isolated from Store's latest line).
6. **Same 6-way build matrix** as the rest of jac313: gcc/clang × headers / modules / import-std.

## Non-goals (v1)

- Replacing jText/vim or the browser for **unfiltered** peeks at small smoke logs (those stay
  one double-click away). The viewer **complements** them when you need category/time filters or
  joined metrics at scale.
- Editing or re-writing captures (read-only viewer).
- Real-time tail-follow of a live Store run (batch load only in v1).
- Web or Electron UI (wxWidgets desktop only for this plan).

---

## Functional design

### 1. Main window — three-pane log browser

Matches the persistence **split layout**: main events, integer metrics, and float metrics are
**three separate virtual lists**. Each pane can scroll on its own, or **lock together by
`id`** (record id) so all three stay aligned on the same event row.

```
┌─ Filters: Category [All ▼]  Time low [____] high [____]  [✓] Link by id ┐
├─────────────────────────────────────────────────────────────────────────┤
│ Events (main)          │ Ints                    │ Floats              │
│ id thr cat payload …   │ id  int0 int1 int2 …    │ id  dbl0 dbl1 …     │
│ ─────────────────────  │ ───────────────────────  │ ─────────────────── │
│ 0  4  GFX  Not Set…    │ 0   0    1    2   …     │ 0   0.0  1.0  …    │
│ 1  6  DB   Trace…      │ 1   100  101  102 …     │ 1   10.5 11.5 …    │
│ 2  4  GFX  Debug…      │ 2   200  201  202 …     │ 2   20.5 21.5 …    │
│   (linked or independent scroll per pane)                              │
└────────────────────────┴─────────────────────────┴─────────────────────┘
│ status: events 1M/1M   ints 1M/1M   floats 1M/1M   format=json         │
└─────────────────────────────────────────────────────────────────────────┘
```

| Pane | Columns | Scroll |
|------|---------|--------|
| **Events** | `id`, `thread_id`, `per_thread_event_id`, `category`, `payload` (truncated), `timestamp_us`, flags summary | Virtual list |
| **Ints** | `id`, `int0` … `int{N-1}` (width from sidecar) | Virtual list |
| **Floats** | `id`, `dbl0` … `dbl{M-1}` (width from sidecar) | Virtual list |

**Shared chrome (above the panes)**

| Control | Behavior |
|---------|----------|
| Category dropdown | `All` + categories from sidecar. Filters the **Events** pane row set; Ints/Floats panes show rows whose `id` is in the filtered event set (row count shrinks with filter). |
| Time low / high | Two text inputs on event `timestamp_us` (or relative µs from sidecar `t_min`). Same propagation to Ints/Floats via `id`. |
| **Link by id** | Toggle (toolbar checkbox). **Off (default):** each pane scrolls independently. **On:** panes stay aligned on the same **`id`** — scrolling, keyboard navigation, or “go to id” in any pane moves all three so that record id shares the same viewport row (split-table join while browsing). |
| Go to id | Text box + button (always available): jump all panes to a given `id`; respects Link state (always aligns all three when invoked). |
| Status bar | Per-pane filtered/total row counts, current linked `id` when locked, load path, format, schema widths. |

**Row linkage:** all three tables share **`id`** as the join key (same as split files on disk).
Independent scroll is for free exploration of one table; **Link by id** is for inspecting one
event and its metrics together — the normal “read a record” workflow across the split files.

Thread identity lives in the **Events** pane only (`thread_id` column). Sorting by `id` or
`timestamp_us` in Events; Ints/Floats stay sorted by `id` to match file order.

If a capture has `int_metric_count == 0` or `dbl_metric_count == 0` (e.g. bench JSON), hide
or collapse the empty pane(s).

### 2. Load screen (first dialog or File → Open)

```
┌─────────────────────────────────────────────┐
│  Open Store capture                         │
│                                             │
│  Format:  [ Binary ▼ | jText | HTML |      │
│            JSON | SQL ]                     │
│                                             │
│  Base path: [ /path/to/persist_______ ] […] │
│                                             │
│  Detected set:                              │
│    ✓ persist.bin          (sidecar OK)      │
│    ✓ persist_Ints.bin                       │
│    ✓ persist_Floats.bin                     │
│                                             │
│  Rows: 10,000,000   Int cols: 9   Dbl: 6   │
│  Time: 0 .. 4,521,883 µs                    │
│  Categories: 5                              │
│                                             │
│            [ Cancel ]  [ Load ]             │
└─────────────────────────────────────────────┘
```

**Format → loader mapping**

| Format | Files expected | Loader backend |
|--------|----------------|----------------|
| Binary | `base.bin` [+ split metric bins if used] | `BinaryEventLogReader` |
| jText | `base.jtext`, `base_Ints.jtext`, `base_Floats.jtext` | jText reader or text fallback |
| HTML | `base.html`, `base_Ints.html`, `base_Floats.html` | HTML table parser (viewer-only) |
| JSON | `base.json`, `base_Ints.json`, `base_Floats.json` | `JsonEventLogReader` |
| SQL | `base` SQLite DB or `.sql` dump | Qlite + schema introspection |

The load screen validates presence of the sidecar (§5). Without it, offer **slow scan** (progress
bar, optional) or refuse 1M+ opens.

### 3. Paged viewer model (not a full reload)

The viewer **never** loads an entire capture into RAM. Opening a file means holding a **file
handle** (and mmap where appropriate), reading the **sidecar** for dimensions, then serving
rows on demand through a **page cache**.

Mirror the shape Store already uses when **writing**:

- `PersistedEvent` / `JsonRecord` field layout: ids, flags, category, payload, timestamp,
  `vector<int64_t>`, `vector<double>` with widths fixed per capture.
- **Do not** assume compile-time `ts_store_config` — widths come from the **sidecar** so one
  viewer binary can open any capture.

**Page model**

| Piece | Role |
|-------|------|
| `CaptureSession` | Owns open files + sidecar; one per loaded capture set |
| `PageSource` | Format adapter: seek to row range, decode N records |
| `PageCache` | LRU of decoded pages (key = table + page_index) |
| Virtual list | Asks for visible row indices → cache → `PageSource` on miss |

**User-controlled page size**

- Settings / toolbar: **Records per page** (e.g. 100 / 500 / 1 000 / 5 000 — persisted in config).
- Scroll and keyboard navigation move by **row index**; the list only materializes the current
  page(s) plus a small read-ahead window.
- “Go to id” and filters change the **logical row set**; paging applies on the filtered view.

**Seek strategy (per format)**

1. **Sidecar** — instant `row_count`, time bounds, category list, `data_offset` (no full scan).
2. **Binary** — record-length prefixes + mmap; `row_index → file_offset` via sidecar sparse index
   or sequential skip within a page boundary.
3. **JSON / HTML** — sidecar sparse `line_offsets` every N rows; page read = seek + decode M lines.
4. **jText / HTML / JSON / SQL** — same page contract; adapter-specific seek (line sections / `LIMIT/OFFSET`).

Pre-scanning 10M lines to *discover* row count is **out of scope** — that is what the sidecar
fixes. Reloading the whole store on filter change is also **out of scope** — filters narrow the
index; pages refresh from the file handle.

---

## Sidecar index file (§5 — prerequisite)

Each capture base name gets a companion index written at **`finalize()`** time (sinks + binary
log), same moment as today's `//File:` headers:

**Proposed name:** `{base}.jac313idx` (JSON sidecar for v1 — easy to debug in vim; binary
index v2 if size matters).

**Minimum fields:**

```json
{
  "format_version": 1,
  "store_schema": "ts_store",
  "base_name": "bench_json_10M",
  "created": "2026-07-09",
  "main": {
    "path": "bench_json_10M.json",
    "row_count": 10000000,
    "data_offset": 142,
    "time_min_us": 0,
    "time_max_us": 4521883
  },
  "ints": { "path": "...", "row_count": 0, "col_count": 0 },
  "floats": { "path": "...", "row_count": 0, "col_count": 0 },
  "categories": ["DB", "GFX", "NET", "SYS", "UI"],
  "int_metric_count": 0,
  "dbl_metric_count": 0
}
```

**Optional v1.1:** `line_offsets` sparse index every N rows for JSON/HTML O(1) seek; binary
already has record-length prefixes for sequential scan.

**Migration:** old captures without `.jac313idx` → viewer offers one-time background index build
(same cost as today's full readback); new captures always ship with index.

---

## Why this is a large standalone project (§6)

| Workstream | Scope |
|------------|--------|
| Sidecar spec + sink changes | Touch every persistence backend (binary, jText, HTML, JSON, SQL) in Store v001/v002 |
| Index builders | Backfill tool for legacy files |
| Viewer core (C++23) | Format adapters, virtual row model, filter engine, no wx dependency |
| wxWidgets UI | Load dialog, virtual list, filters, threading for load progress |
| CMake / 6 builds | New top-level package `LogViewer/` or `tools/jac313_log_viewer/` |
| Tests | Golden sidecar files, round-trip open on smoke + 1M samples (10M in CI optional) |

Estimate: **multi-week** effort, not a weekend. Store bench/reporting work does not block on it.

---

## Language and build constraints (§7–8)

- **C++23 only** for the viewer package — keeps wx + UI code stable while Store continues on
  C++26 where needed.
- **Six compile modes** (same policy as Qlite/jText/Store):

  | Compiler | Headers | Modules | import-std |
  |----------|---------|---------|------------|
  | gcc16    | ✓       | ✓       | ✓          |
  | clang    | ✓       | ✓       | ✓          |

- Viewer links **Store readers** (binary, JSON) and **Qlite** for SQL; jText/HTML parsers may
  live in viewer only to avoid pulling wx into Store.

---

## jText and HTML vs the viewer (§9 — the honest trade-off)

| Concern | jText | HTML | JSON / binary |
|---------|-------|------|---------------|
| Open 100-event smoke log | gedit — instant | browser — instant | need tools / viewer |
| Open 10M capture | Heavy in editor; still usable | browser struggles (GB table) | opaque or line-at-a-time |
| Schema in file | Self-describing `//` + jText sections | `//` header + HTML tables | sidecar + NDJSON / binary |
| Split metrics | `_Ints` / `_Floats` tabs in editor | companion `.html` files | companion files / tables |
| Filter by category / time | grep / editor search (awkward at scale) | browser find (no real filter) | same pain without viewer |
| Viewer adds here | category dropdown + time range on `.jtext` | same filters on `.html` | only practical browse path |
| Dependencies | jText lib (write path) | none (write path) | wxWidgets + index pipeline |

**Conclusion:** **jText and HTML win for instant, unfiltered reading** — gedit or a browser, no
custom app. That stays compelling. The viewer is **not** replacing that; it adds what raw files
lack: **category and relative-time filters**, **virtual scroll at 10M**, and **three independently
scrollable panes** (events, ints, floats) mirroring the split files — across **every** format,
including jText and HTML. Binary and JSON still *need*
the viewer to be readable at all; jText and HTML *benefit* from it when the capture is large or
you are digging for a thread/category/time window instead of reading top-to-bottom.

---

## Phased roadmap

### Phase 0 — Spec freeze (short)
- Finalize `.jac313idx` JSON schema.
- Decide time filter units (absolute µs vs relative).
- wxWidgets version pin in `Setup/`.

### Phase 1 — Sidecar write path (Store)
- `write_sidecar_index()` in `PersistCommon.hpp`.
- Call from all sinks at `finalize()`.
- Smoke + 1M matrix tests assert sidecar exists and matches row counts.
- **No viewer yet** — proves index correctness with existing readers.

### Phase 2 — Viewer core library (no UI)
- `jac313::LogViewer::CaptureIndex` — load sidecar.
- `PageSource` + `PageCache` — file handle, seek, decode N records (user page size).
- `VirtualEventSource` — row index → page fetch; JSON/binary implementations first.
- Filter: category + time range on the logical row set (re-page, not full reload).
- CLI stub: `jac313_log_viewer --page 0 --page-size 500 --filter category=NET` for headless tests.

### Phase 3 — wxWidgets shell
- Load screen (§2).
- Three-pane layout: Events / Ints / Floats — independent scroll + **Link by id** lock (§1).
- Shared filter bar (category + time range) and go-to-id control.
- Progress dialog for index backfill on legacy files.

### Phase 4 — Remaining formats
- HTML table parser (reuse split layout).
- jText loader (leverage jText reader or line scan).
- SQL via Qlite.

### Phase 5 — Build matrix + polish
- Six-way CMake targets.
- Docs + sample capture with sidecar in `docs/samples/`.
- Optional: link from `test-summary` “open in viewer” (far future).

---

## Open questions

1. **Package name / path:** `LogViewer/` sibling to Store, or `tools/jac313_log_viewer/`?
2. **wxWidgets packaging:** system wx vs bundled — follow Fedora/RHEL baseline?
3. **10M in viewer:** mmap binary only for v1, or require line-offset index for JSON/HTML?
4. **v001 parity:** viewer reads both v001 and v002 captures, or v002-only?
5. **License:** wxWidgets LGPL — confirm acceptable for jac313 distribution model.

---

## Immediate next step (when we start)

**Phase 1 only:** add `.jac313idx` generation to JSON + binary sinks on finalize; validate against
`bench_json_1M.json` / `bench_json_10M.json` without building UI. Viewer waits until sidecar is
trustworthy.

---

## References

- [reporting.md](reporting.md) — today's read paths (jText, HTML, JSON, SQL, binary reader).
- [persistence.md](persistence.md) — sink layout (split main / ints / floats).
- Bench captures (local, gitignored): `v002/bench-json-output/bench_json_{1M,10M}.json`.
- `JsonEventLogReader` — proves round-trip read of NDJSON + `//` header today (~3.6 s / 1M,
  ~36 s / 10M full scan); sidecar removes the need to scan for **metadata**.