# Store — Reporting (reading the logs back)

How logged events are read, inspected, and queried after the fact. Reporting in Store is not a
separate subsystem — it's the set of **views** over what [persistence](persistence.md) wrote.

← [Store docs index](../../Store/README.md) · [main README](../../README.md) ·
related: [persistence](persistence.md) · [logging](logging.md) · [sample output](../samples/)

---

## Three views of the same events

| View | Produced by | Read with |
|------|-------------|-----------|
| **Human-readable** | jText sink → `base.jtext` + `base_Ints.jtext` + `base_Floats.jtext` | any text tool; the [jText](../../jText/README.md) format is self-describing (light/full profiles, `//` metadata header) |
| **Browser-readable** | HTML sink → `base.html` + `base_Ints.html` + `base_Floats.html` | any web browser; `//` metadata header then HTML tables (writer only — no Store reader) |
| **Machine-readable text** | JSON sink → `base.json` + `base_Ints.json` + `base_Floats.json` (NDJSON) | `JsonEventLogReader` (`next(JsonRecord&)`), or any JSON/NDJSON tool |
| **Queryable** | SQL sink → `main` + `_ints` + `_floats` tables | SQL, e.g. `SELECT * FROM base WHERE flags_raw & (1<<2)` for `DatabaseEntry` events |
| **Compact / fast** | binary sink → `.bin` (mmap, length-prefixed) | `BinaryEventLogReader` (`next(BinaryRecord&)`), or `convert_to_jtext()` to turn a capture into readable logs in post |

The common pattern: **capture fast** (binary, on the hot path), **report slow** (convert to jText,
browse HTML, parse JSON, or load into SQL) in post-processing.

## Split text sinks (jText, HTML, JSON)

The jText, HTML, and JSON sinks each write **three** files — main events, integer metrics, float metrics — so a
high-rate event stream doesn't drag its metric columns through every line. Store writes its own
`//` metadata comments first. jText hands events to `JTextWriter` (with high-throughput batching;
profiles light/full are a jText concern). HTML and JSON emit tables / NDJSON lines directly from
the same split layout.

## SQL schema

Three tables — `main` (one row/event: ids, flags, category, payload, timestamp), `_ints`, and
`_floats` (metrics keyed by event id). Events with fewer metrics than the configured count are
zero-padded. There's no built-in dashboard: you write the queries (and indexes) you need.

---

## Strengths

- **Choose the view at sink time** — speed, readability, or queryability, or several together via
  [flag routing](bitmaps.md).
- **Binary captures are replayable/validatable** after the fact (`BinaryEventLogReader`,
  `convert_to_jtext`).
- The jText format is **self-describing** — logs are legible without the schema.

## Weaknesses / footguns

- **No unified reporting layer.** jText, HTML, JSON, SQL, and binary are independent; there's no dashboard or
  cross-view query — you assemble reports yourself.
- **Metric joins are manual** in SQL (`main` + `_ints` + `_floats`).
- **Binary is opaque and unversioned** — you need the reader, and a layout change breaks old
  captures silently.
- **No streaming reader** — `BinaryEventLogReader` reconstructs whole records in memory.

> "Reporting" here is deliberately thin — Store gives you durable, legible, queryable *data* and
> leaves the presentation to standard tools (text, SQL) or your own layer. If you want dashboards
> or cross-format scroll/filter at 1M+ scale, see the planned
> [log viewer](log-viewer-plan.md) (wxWidgets + sidecar index — not started yet).
