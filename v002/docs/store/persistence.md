# Store — Persistence (the sinks)

Where logged events go for durability — the `IEventSink` interface, the backends, the
persistence workers (one per attached sink) that read the rows in place, and how [jText](../../jText/README.md) and
[Qlite](../../jText/README.md) (brought in by jText) are *used by* Store.

← [Store docs index](../../Store/README.md) · [main README](../../README.md) ·
related: [logging](logging.md) · [bitmaps/flags](bitmaps.md) · [reporting](reporting.md)

---

## The interface

`IEventSink` (`persistence/EventSink.hpp`) — every backend implements:

```cpp
virtual void write_batch(std::span<const PersistedEvent> batch) = 0;
virtual void flush() = 0;
virtual void finalize() = 0;     // close files, commit transactions
virtual std::string_view name() const;   // for diagnostics and per-sink report lines
```

A `PersistedEvent` is the flat record: ids, flags, category, payload, `timestamp_us`, and the
int/double metric arrays.

## The persistence workers (a sliding view, one per sink)

`ts_store::attach_persistence(sink, batch_size = 10'000)` starts one worker thread for that sink
(`impl_details/persist_worker.hpp`) and returns the worker's index. The rows are preallocated and
never overwritten within a run, so the worker reads them **in place**: the [hot path](logging.md)'s
last statement marks its row ready (a release store) and that is all it does for persistence — no
copy, no queue, no lock. The worker keeps a watermark (the next id not yet persisted), takes the
contiguous run of ready rows from there (at most `batch_size`, capped by `max_persist_rows`),
fills one reusable batch of views into those rows, hands it to the sink and advances; with nothing ready it
waits ~1 ms. A sink exception latches that worker's failure; its watermark keeps moving, so no
producer ever waits on a sink.

**Several sinks at once.** Call `attach_persistence` once per sink, before or during a run (events
saved before a sink was attached are not given to it). A jText keeper file, a SQLite database and
the binary replay log can all persist the same run, each at its own pace: every worker has its
own watermark, start id, batch and thread, and they share nothing but the read-only rows and the
store's atomics (`next_id_`, `max_persist_rows_`, the ready epoch). Routing needs no router: each
sink keeps the events its `PersistMode` takes (`All`, `KeeperOnly` = `KeeperRecord` bit 1,
`DatabaseOnly` = `DatabaseEntry` bit 2), so a `KeeperOnly` file sink beside a `DatabaseOnly` SQL
sink splits the events by flag; see [bitmaps](bitmaps.md).

**Each sink must own its own file or connection.** A sink is called from its worker's thread
only, so it needs no locks — but two sinks writing one file interleave their records, and a
database connection (a libpq `PGconn` included) is not thread-safe. Give every sink its own base
name / database file / connection.

| Call | Meaning |
|------|---------|
| `attach_persistence(sink, batch)` | add a worker for `sink`; returns its index |
| `flush_persistence()` | every worker writes what is ready and flushes its sink |
| `finalize_persistence()` | every worker writes every saved row, finalizes its sink, stops; all are finalized even if one reports an error, then the errors of all are thrown together (one line per sink) |
| `persistence_failed()` / `persistence_failed(i)` | any sink failed / sink `i` failed |
| `persisted_rows()` / `persisted_rows(i)` | the fewest rows any sink has taken / rows sink `i` has taken |
| `drain_time_us(i)` | microseconds from attaching sink `i` to its finalize returning (0 before) |
| `persistence_sink_name(i)` | sink `i`'s `IEventSink::name()`, for one report line per sink |
| `persistence_count()` | sinks attached |

`clear()` is refused once any event was saved since any worker was attached (the ids would
repeat in that sink's output).

---

## The backends

| Sink | Format | Uses | Notes |
|------|--------|------|-------|
| **Binary** (`BinaryEventSink` / `BinaryEventLog`) | length-prefixed records in an **mmap**-backed file (`posix_fallocate` pre-sizes; `ftruncate`+remap on grow) | POSIX | fastest (memcpy into the mapping); read back with `BinaryEventLogReader` |
| **jText** (`JTextEventSink` / `JTextSplitEventLog`) | three `.jtext` files: **main** + **`_Ints`** + **`_Floats`** | [jac313::jText](../../jText/README.md) `JTextWriter` | human-readable; metrics split out so slow-changing events don't bloat metric tables |
| **HTML** (`HtmlEventSink` / `HtmlSplitEventLog`) | three `.html` files: **main** + **`_Ints`** + **`_Floats`** | — (no external deps) | browser-viewable tables; writer only (no reader) |
| **JSON** (`JsonEventSink` / `JsonSplitEventLog`) | three NDJSON files: **main** + **`_Ints`** + **`_Floats`** (one object per line) | — (no external deps) | machine-readable text; read back with `JsonEventLogReader` |
| **SQL** (`SqlEventSink`) | three tables: **main** + **`_ints`** + **`_floats`** (dynamic metric columns), `INSERT OR IGNORE` in transactions | [jac313::Qlite](../../jText/README.md) `Sqlite` | queryable; optional debug `.sql` of textual INSERTs |

**Binary record layout:** 5× `uint64_t` (ids/flags/timestamp), then length-prefixed category,
payload, int array, double array. A `uint32_t` length prefix on each record lets the reader find
boundaries.

**jText rows without strings:** `JTextSplitEventLog` formats each number with `std::to_chars` into
buffers it owns and hands every field (category and payload too) to `JTextWriter::append_row_views`
as a view, so a row makes no string. Given `JTextRowLimits` (`jtext_row_limits_for<Config>()`: the
Config's bounded category and payload sizes), it reserves the main writer's row buffer once for the
longest one-line row (`jtext_row_bytes`); the `_Ints` / `_Floats` buffers are always reserved, as
their rows hold numbers only. A multiline payload makes a longer row, which simply grows the buffer.

**How the sibling packages plug in:** Store calls jText (`JTextWriter`) **only** from the jText
sink and Qlite (`Sqlite`) **only** from the SQL sink — both are pay-for-what-you-use. Use just
the binary sink and neither sibling is invoked.

---

## Strengths

- **Pick your trade-off per sink:** speed (binary), readability (jText / HTML / JSON), queryability (SQL)
  — or several at once, one worker each, routed by each sink's `PersistMode`.
- **Non-blocking** logging regardless of sink speed (the worker reads the rows in place).
- **Clean `IEventSink` seam** — adding a backend is implementing three methods.

## Weaknesses / footguns

- **Binary is POSIX-only** (`mmap`) and **unversioned** — a layout change silently breaks old
  `.bin` files (no format-version field), and a malformed record halts the reader (no recovery).
- **SQLite writes are slow** relative to binary (disk + WAL); the schema (metric column count) is
  baked at sink creation — you can't widen `_ints`/`_floats` later without recreating the sink.
- **Metrics live in separate tables/files** — reconstructing "an event with all its metrics"
  means a join (`_ints` / `_floats` back to main).
- **jText is a build dependency of the jText sink** even if you only ever use binary.

## Compromises: doubles

A double metric is stored exactly, and NaN, +Infinity and -Infinity are data like any other value.

- **Text formats write the shortest exact round-trip text** (`append_double` in
  `PersistCommon.hpp`, `std::to_chars`): every finite double reads back with the same bits.
  The non-finite ones are spelled `NaN`, `Infinity`, `-Infinity`.
- **NaN and the infinities round-trip** through binary, jText, HTML, JSON and PostgreSQL. JSON has
  no number for them, so the JSON sink writes them as the strings `"NaN"`, `"Infinity"`,
  `"-Infinity"`, and `JsonEventLogReader` reads those strings back as the doubles.
- **SQLite cannot hold NaN: it stores NULL.** Read-back regards a NULL double from SQLite as the
  NaN that was stored. This is the one NULL allowed in this project; any other NULL fails
  read-back, and so does any NULL from PostgreSQL.
- **SQLite keeps +/-Infinity** as REAL. The debug `.sql` file writes them as `9e999` / `-9e999`
  (SQLite has no infinity keyword and reads those as +/-Inf) and NaN as `NULL`.
- **SQLite drops the sign of -0.0**: it stores a whole REAL as an integer, so -0.0 reads back as
  0. Read-back regards that 0 as the -0.0 that was stored.

> Persistence is the densest part of Store and where most of the real engineering lives (mmap
> growth, batched draining, the binary codec). It's the prime candidate for an AI-assisted deep
> read against your durability/throughput requirements.

---

## JSON split layout

The JSON sink writes **NDJSON** — one JSON object per line after the standard `//` metadata
header — in three files (main, `_Ints`, `_Floats`) so the comparison to jText stays fair.
Read back with `JsonEventLogReader` (`next(JsonRecord&)`), mirroring the binary reader pattern.
