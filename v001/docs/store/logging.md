# Store — Logging (the core)

The hot path: how a consumer logs an event, and how the record is shaped to the **exact
number of integer and float fields** the consumer needs.

← [Store docs index](../../Store/README.md) · [main README](../../README.md) ·
related: [categories](categories.md) · [bitmaps/flags](bitmaps.md) · [persistence](persistence.md)

---

## The model

Store is `ts_store<Config>` — a class template
(`Store/include/jac313/Store/v001/headers/ts_store.hpp`) whose `Config` (a
`ts_store_config<>`) fixes the **record shape at compile time**. A consumer logs one event:

```cpp
auto [ok, global_id] = store.save_event(
    thread_id,            // per-thread namespace (0 .. max_threads-1)
    event_id,             // per-thread counter
    payload,              // bounded_string<MaxPayloadLength>
    event_flags,          // routing/severity bitset — see bitmaps.md
    category,             // bounded_string<MaxCategoryLength>
    debug,
    int_metrics,          // std::array<int64_t, IntMetrics>
    dbl_metrics);         // std::array<double,  DblMetrics>
```

Each event becomes one `row_data` slot: flags, ids, the fixed int/double arrays, a category
and payload `bounded_string`, a debug bit, and an optional timestamp — no `std::string`, no heap
allocation on the hot path.

---

## Exact field counts — the headline

The integer and float field counts are **template parameters on the config**, so each row holds
exactly the metrics you declared and nothing more:

| Config parameter | Default | Meaning |
|------------------|---------|---------|
| `IntMetrics` | 9 | number of `int64_t` slots per event |
| `DblMetrics` | 6 | number of `double` slots per event |
| `MaxCategoryLength` | 20 | category capacity (UTF-8 codepoints) |
| `MaxPayloadLength` | 80 | payload capacity (UTF-8 codepoints) |

They're `constexpr`, validated with `static_assert`, and compile to fixed-size `std::array`s
inside each row — so the per-event footprint is known exactly, with no runtime resizing. You
size the store to your record, not the other way round.

---

## Throughput

- **Zero-copy hot path** (`impl_details/core.hpp::save_event`): a slot index is claimed with an
  atomic `fetch_add` (wait-free, thread-safe across loggers), then fields are written directly
  into the pre-sized row — no locks, no allocations.
- **Capacity is pre-allocated** at construction (`max_threads × events_per_thread` rows); a
  `sysinfo` RAM check warns/exits if the box can't hold it.
- **Persistence is decoupled.** If a `DoubleBufferedWriter` is attached, each saved event is
  submitted to a background drain queue **non-blocking** to the caller — the log call returns at
  memory speed regardless of how slow the sink is. See [persistence](persistence.md).

---

## Strengths

- Predictable, allocation-free hot path with a compile-time-known footprint — no runtime surprises.
- Wait-free atomic slot assignment: many threads log concurrently without contention.
- Persistence never blocks the logger (double-buffered drain).

## Weaknesses / footguns

- **Fixed field counts are rigid.** You must know your max int/double metric counts at compile
  time; overshooting wastes memory, undershooting needs a recompile (and a new template
  instantiation).
- **Pre-allocation can be brutal.** Sizing for a worst-case row count reserves that RAM up front
  even if you use 1% of it; the `sysinfo` check is advisory, not a hard cap.
- **No native streaming API.** It's log-to-memory-then-drain/query; very large datasets must be
  bounded or chunked by the consumer.
- **Silent truncation** of over-long category/payload — see [categories](categories.md).

> Store is the largest part of jac313 and rewards a guided read. The strengths/weaknesses above
> are a starting map, not the last word — running an AI over `ts_store.hpp` +
> `impl_details/core.hpp` for your specific workload is a good way to find the edges that matter
> to *you*.
