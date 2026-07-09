# Store — Use cases (when & for what)

What `ts_store` is *for*: a **what-happened-and-where** event log. The fixed int/float metric
counts are the **dimensionality** of an event; the **Event** (flags) carries its **intent**. Same
substrate, many domains — switched at compile time.

← [Store docs index](../../Store/README.md) · [main README](../../README.md) ·
related: [logging](logging.md) · [flags](flags.md) · [persistence](persistence.md)

---

## The reframe

Most of the Store docs explain *how* the machine works. This one explains *what it's good for* —
the intent behind the shape.

`ts_store` is **not** "a logger" in the printf sense. It's a record of **what happened, where, and
why**, captured at memory speed from many threads at once:

| Part of the record | Field(s) | Carries |
|---|---|---|
| **Where** | `int_metrics` / `dbl_metrics` (fixed-arity numeric tuple) | the *numbers* — a point, a state vector, a reading |
| **What / intent** | `event_flags` (the **Event**) | *why this row exists* — the semantic event |
| **Kind** | `category` (+ `payload` for sub-detail) | the *bucket* it belongs to |

The headline consequence: **the metric count is a knob you turn to fit the domain.** Three floats
is a 3-D point. Two is a 2-D point. Zero is a pure event with no coordinates. You size the record
to the world you're modelling — see [logging](logging.md) for the compile-time field counts.

---

## Dimensionality → domain

| Int/Float count | "Where" | Event = intent | Domain |
|---|---|---|---|
| **3** | (x, y, z) | move / grasp / fault… | Robot positioning |
| **2** | (x, y) or (lat, lon) | depart / arrive / reroute… | Trucking / train / fleet |
| **0** | — | start / step / error… | Program execution (category + subcategories) |
| **1 / 4+ / mixed** | scalar / quaternion / OHLC / id+coord | … | **…and many more** (§ Gallery) |

---

## Worked examples

All snippets use the real API: a `ts_store_config<UseTimestamps, MaxTypeLength, MaxCategoryLength,
MaxPayloadLength, IntMetrics, DblMetrics, EnableMetrics>` and
`save_event(thread_id, event_id, value, event_flags, category, debug, int_metrics, dbl_metrics)`.

### Robot positioning — 3-D point + intent (`DblMetrics = 3`)

```cpp
using Cfg = jac313::Store::v002::ts_store_config<true, 6, 20, 43, 0, 3, false>;
jac313::Store::v002::ts_store<Cfg> store(threads, events_per_thread);

// where = (x, y, z); intent = the Event flags; kind = category
store.save_event(tid, eid, Cfg::ValueT("end-effector"), kEventGrasp,
                 Cfg::CategoryT("arm.move"), false,
                 /*int*/ {}, /*dbl*/ {0.412, -0.118, 0.305});
```

### Trucking / rail — 2-D track + intent (`DblMetrics = 2`)

```cpp
using Cfg = jac313::Store::v002::ts_store_config<true, 6, 20, 43, 0, 2, false>;
// where = (lat, lon); intent = depart/arrive/reroute; kind = vehicle/leg
store.save_event(tid, eid, Cfg::ValueT("unit-7"), kEventArrive,
                 Cfg::CategoryT("fleet.leg"), false,
                 {}, {47.6062, -122.3321});
```

Prefer **int** fixed-point coordinates when you need exact, reproducible values (e.g. lat/lon
scaled by 1e7): `IntMetrics = 2`, `DblMetrics = 0`.

### Program execution — no coordinates, just category + subcategories (`0 / 0`)

```cpp
using Cfg = jac313::Store::v002::ts_store_config<true, 6, 20, 80, 0, 0, false>;
// no numbers — the Event + category(.subcategory) + payload tell the whole story
store.save_event(tid, eid, Cfg::ValueT("connecting to db"), kEventStep,
                 Cfg::CategoryT("startup.db"), false);
```

With zero metrics each row is lean — the `std::array<…, 0>` storage collapses to nothing, and the
record is carried entirely by the Event flags, the category (use a `parent.child` convention for
subcategories), and the bounded payload.

### Composite arity / multi-waypoint — robot-arm safe-transit precheck (`DblMetrics = 9`)

Arity doesn't have to be "one point." One arm **pose** = `3 joints × 3-D = 9 floats`
(`DblMetrics = 9`, which happens to be the default). A planned **4-waypoint transit** is then four
events sharing one **Event Name**, and the *sequence* of rows encodes the path:

```cpp
using Cfg = jac313::Store::v002::ts_store_config<true, 6, 20, 43, 0, 9, false>;
for (int wp = 0; wp < 4; ++wp)
    store.save_event(tid, eid + wp, Cfg::ValueT("transit-A"), kEventPrecheck,
                     Cfg::CategoryT("arm.transit"), false,
                     {}, pose_floats[wp]);   // 9 floats: 3 joints × (x,y,z)
```

The store records the four poses that were checked, in order, with their intent — it does **not**
decide whether the transit is safe (see *Recorder, not calculator*).

### Timestamps headline — IoT / industrial sensor telemetry (`UseTimestamps = true`)

When *time* is the spine of the data, set `UseTimestamps = true` and read it back in time order
with `get_ids_sorted_by_timestamp()`. Many sensors stream readings concurrently; each row is
stamped on capture:

```cpp
using Cfg = jac313::Store::v002::ts_store_config<true, 6, 20, 43, 0, 1, false>;
store.save_event(tid, eid, Cfg::ValueT("boiler-3"), kEventSample,
                 Cfg::CategoryT("plant.temp"), false, {}, {temperature_c});
// later, offline:
for (auto id : store.get_ids_sorted_by_timestamp()) { /* replay in capture order */ }
```

This is a domain that would genuinely **integrate a general library** rather than build bespoke —
which is why it headlines the timestamp story. (Financial tick data has the *same* shape —
timestamped values, ordered replay, audit trail — and is a fine way to *picture* it, but trading
shops run their own customized low-latency stacks, so treat it as a shape illustration, not a
target market.)

### The most concrete adopter: this monorepo

The realest user of `ts_store` is **jac313 itself** — `store_bench` instruments throughput
measurements through it, and the matrix/stress tests (`tests/matrix/`) drive it under load. If you
want a living reference for any of the patterns above, read those.

---

## Recorder, not calculator

The store **records; it does not compute.** It will not decide whether the transit is safe, the
trajectory is valid, or a reading is in range — that logic belongs to the caller. What it
guarantees is the **auditable trail**: the exact tuples that were observed or checked, in order,
with their intent, captured without perturbing the hot path, ready for deterministic replay.

- **Feature:** clean separation of concerns. Your domain code owns the *meaning*; the store owns
  the *faithful, ordered, low-overhead record*. The audit/replay artifact is a byproduct you get
  for free.
- **Footgun:** don't reach for it expecting validation, alerting, or derived metrics. It's a
  sink, not an engine. Compute on the data *after* you read it back (see [reporting](reporting.md)).

---

## Gallery — …and many more

The examples above are a few of many. The pattern is always the same — **a fixed numeric arity
("where") + a semantic Event ("what/intent"), optionally timestamped** — so it retargets widely.
All civilian/commercial; pick the arity that fits your tuple:

- **1-D sensor / IoT time-series** — a single scalar + timestamp (`DblMetrics = 1`): temperature,
  pressure, voltage, flow.
- **Quaternion / IMU attitude** — 4 floats `(w, x, y, z)`, optionally ±3 gyro = composite arity
  (phones, VR/AR, industrial motion).
- **Financial OHLC / ticks** — 4 floats `(open, high, low, close)` or price+volume, timestamped
  (shape illustration; see headline caveat).
- **Mixed int + float** — an int entity/zone id plus float coordinates: warehouse AGVs,
  factory-floor asset tags.
- **Vehicle / telematics (CAN)** — speed / rpm / throttle as a fixed float tuple + event flags.
- **Game / simulation entity state** — position + velocity + state-id, captured for deterministic
  replay.
- **Manufacturing / process control** — machine setpoints + readings per step, kept as an audit
  trail.

If you size the record to your world, the list keeps going.

---

## How to choose your Config

1. **Arity** — count the numbers per event; set `IntMetrics` / `DblMetrics` to exactly that
   (either can be 0). Composite tuples (joints × dims) are fine.
2. **int vs float** — exact / reproducible values (fixed-point coords, ids, counters) → **int**;
   physical measurements → **float**.
3. **Timestamps** — set `UseTimestamps = true` when capture *time* matters; read back via
   `get_ids_sorted_by_timestamp()`.
4. **Text** — size `MaxCategoryLength` / `MaxPayloadLength` to your real strings (UTF-8 codepoints;
   over-length is silently truncated — see [categories](categories.md)).
5. **Persistence** — choose a sink (binary / jtext / html / json / sql / flag-routing) for replay & audit; the
   drain never blocks the logger — see [persistence](persistence.md).

---

## Strengths

- **One substrate, many domains** — a single tested core retargets from robotics to logging to
  telemetry by changing compile-time counts, not by forking code.
- **Best fit when the schema is fixed and known** — arity decided at compile time, volume
  bounded/preallocable, many concurrent producers, and deterministic replay/audit matters.
- **The audit trail is free** — faithful, ordered capture is a byproduct of how it records.

## Weaknesses / footguns

- **Per-event schema must be fixed.** If different events need wildly different field shapes,
  you're fighting the design (or instantiating many configs — watch template explosion).
- **Volume must be bounded.** Capacity is pre-allocated; unbounded/unknown-size streams must be
  chunked or capped by the consumer — see [logging](logging.md).
- **No key lookup, no mutation/erase.** It's append-by-id, read-by-id — not an associative
  container. Need content lookup? Build a secondary index beside it.
- **It does not validate or compute.** Expecting safety checks, alerting, or derived metrics from
  the store itself is a category error — see *Recorder, not calculator*.

> These are a starting map, not the last word. The metric-count-as-dimensionality idea stretches
> further than the examples here — running an AI over `ts_store.hpp` + `impl_details/core.hpp`
> against *your* workload is a good way to find the shape that fits you.
