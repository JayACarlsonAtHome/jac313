# Store — Persistence (the sinks)

Where logged events go for durability — the `IEventSink` interface, four backends, the
double-buffered drain, and how [jText](../../jText/README.md) and
[Qlite](../../Qlite/README.md) are *used by* Store.

← [Store docs index](../../Store/README.md) · [main README](../../README.md) ·
related: [logging](logging.md) · [bitmaps/flags](bitmaps.md) · [reporting](reporting.md)

---

## The interface

`IEventSink` (`persistence/EventSink.hpp`) — every backend implements:

```cpp
virtual void write_batch(std::span<const PersistedEvent> batch) = 0;
virtual void flush() = 0;
virtual void finalize() = 0;     // close files, commit transactions
```

A `PersistedEvent` is the flat record: ids, flags, category, payload, `timestamp_us`, and the
int/double metric arrays.

## Double-buffered drain

`DoubleBufferedWriter` (`persistence/DoubleBufferedWriter.hpp`) sits between the
[hot path](logging.md) and a sink: the logger appends to an active buffer; when it fills (default
**10k** events) the buffers swap and a worker thread drains the full one to the sink and flushes.
Amortized O(1) per event, and the logger never blocks on sink I/O.

---

## The backends

| Sink | Format | Uses | Notes |
|------|--------|------|-------|
| **Binary** (`BinaryEventSink` / `BinaryEventLog`) | length-prefixed records in an **mmap**-backed file (`posix_fallocate` pre-sizes; `ftruncate`+remap on grow) | POSIX | fastest (memcpy into the mapping); read back with `BinaryEventLogReader` |
| **jText** (`JTextEventSink` / `JTextSplitEventLog`) | three `.jtext` files: **main** + **`_Ints`** + **`_Floats`** | [jac313::jText](../../jText/README.md) `JTextWriter` | human-readable; metrics split out so slow-changing events don't bloat metric tables |
| **HTML** (`HtmlEventSink` / `HtmlSplitEventLog`) | three `.html` files: **main** + **`_Ints`** + **`_Floats`** | — (no external deps) | browser-viewable tables; writer only (no reader) |
| **JSON** (`JsonEventSink` / `JsonSplitEventLog`) | three NDJSON files: **main** + **`_Ints`** + **`_Floats`** (one object per line) | — (no external deps) | machine-readable text; read back with `JsonEventLogReader` |
| **SQL** (`SqlEventSink`) | three tables: **main** + **`_ints`** + **`_floats`** (dynamic metric columns), `INSERT OR IGNORE` in transactions | [jac313::Qlite](../../Qlite/README.md) `Sqlite` | queryable; optional debug `.sql` of textual INSERTs |
| **Flag-routing** (`FlagRoutingEventSink`) | composite — multiplexes a batch across two inner sinks by per-event flags | — | the file-vs-SQL split; see [bitmaps](bitmaps.md) |

**Binary record layout:** 5× `uint64_t` (ids/flags/timestamp), then length-prefixed category,
payload, int array, double array. A `uint32_t` length prefix on each record lets the reader find
boundaries.

**How the sibling packages plug in:** Store calls jText (`JTextWriter`) **only** from the jText
sink and Qlite (`Sqlite`) **only** from the SQL sink — both are pay-for-what-you-use. Use just
the binary sink and neither sibling is invoked.

---

## Strengths

- **Pick your trade-off per sink:** speed (binary), readability (jText / HTML / JSON), queryability (SQL) — or
  several at once via flag routing.
- **Non-blocking** logging regardless of sink speed (double-buffered).
- **Clean `IEventSink` seam** — adding a backend is implementing three methods.

## Weaknesses / footguns

- **Binary is POSIX-only** (`mmap`) and **unversioned** — a layout change silently breaks old
  `.bin` files (no format-version field), and a malformed record halts the reader (no recovery).
- **SQLite writes are slow** relative to binary (disk + WAL); the schema (metric column count) is
  baked at sink creation — you can't widen `_ints`/`_floats` later without recreating the sink.
- **Metrics live in separate tables/files** — reconstructing "an event with all its metrics"
  means a join (`_ints` / `_floats` back to main).
- **jText is a build dependency of the jText sink** even if you only ever use binary.

> Persistence is the densest part of Store and where most of the real engineering lives (mmap
> growth, batched draining, the binary codec). It's the prime candidate for an AI-assisted deep
> read against your durability/throughput requirements.

---

## JSON split layout

The JSON sink writes **NDJSON** — one JSON object per line after the standard `//` metadata
header — in three files (main, `_Ints`, `_Floats`) so the comparison to jText stays fair.
Read back with `JsonEventLogReader` (`next(JsonRecord&)`), mirroring the binary reader pattern.


