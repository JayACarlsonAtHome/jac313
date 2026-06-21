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
| **Queryable** | SQL sink → `main` + `_ints` + `_floats` tables | SQL, e.g. `SELECT * FROM base WHERE flags_raw & (1<<2)` for `DatabaseEntry` events |
| **Compact / fast** | binary sink → `.bin` (mmap, length-prefixed) | `BinaryEventLogReader` (`next(BinaryRecord&)`), or `convert_to_jtext()` to turn a capture into readable logs in post |

The common pattern: **capture fast** (binary, on the hot path), **report slow** (convert to jText
or load into SQL) in post-processing.

## jText split, by design

The jText sink writes **three** files — main events, integer metrics, float metrics — so a
high-rate event stream doesn't drag its metric columns through every line. Store writes its own
`//` metadata comments first, then hands events to jText's `JTextWriter` (with high-throughput
batching). Profiles (light/full) are a jText concern; Store just feeds the writer.

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

- **No unified reporting layer.** jText, SQL, and binary are independent; there's no dashboard or
  cross-view query — you assemble reports yourself.
- **Metric joins are manual** in SQL (`main` + `_ints` + `_floats`).
- **Binary is opaque and unversioned** — you need the reader, and a layout change breaks old
  captures silently.
- **No streaming reader** — `BinaryEventLogReader` reconstructs whole records in memory.

> "Reporting" here is deliberately thin — Store gives you durable, legible, queryable *data* and
> leaves the presentation to standard tools (text, SQL) or your own layer. If you want dashboards
> or cross-view analytics, that's a build-on-top, and a good place to point an AI at the SQL
> schema + the jText format to scaffold it.
