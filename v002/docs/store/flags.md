# Store — Flags (the complete catalog)

`StoreFlags` (`Store/include/jac313/Store/v002/headers/ts_store_flags.hpp`) is a single
**64-bit** word attached to every event. This is the full reference — every bit, its
meaning, and an **honest status**: whether it actually drives behavior, is set by the
store itself, is consumer-advisory, or is declared intent not yet wired.

← [Store docs index](../../Store/README.md) · [main README](../../README.md) ·
related: [bitmaps / routing](bitmaps.md) (how flags route events) · [logging](logging.md) · [metric view](#metric-summary-bits)

> **Status legend**
> ✅ **wired** — drives real behavior · ⚙️ **auto** — set by the store on `save_event` ·
> 📌 **advisory** — meaningful, but you set it; nothing auto-sets or enforces it ·
> 🕳️ **placeholder** — declared intent, not yet wired to anything.

---

## User flags (bits 0–6)

| Bit | Name | Meaning | Status |
|----|------|---------|--------|
| 0 | `LogConsole` | echo the event to the console | 🕳️ placeholder — set in test data; no console sink consumes it |
| 1 | `KeeperRecord` | **keep this event in the keeper (file) sink** | ✅ wired — routing + `PersistMode::KeeperOnly` |
| 2 | `DatabaseEntry` | **write this event to the SQL sink** | ✅ wired — routing + `PersistMode::DatabaseOnly` |
| 3 | `SendNetwork` | send the event over the network | 🕳️ placeholder — no network sink exists |
| 4 | `HotCacheHint` | hint that the event is cache-hot | 🕳️ placeholder — nothing consumes it |
| 5 | `IsResult` | the event represents a result | 🕳️ placeholder — not wired |
| 6 | `IsExplicitNull` | the value is an explicit null | 🕳️ placeholder — not wired in the sinks |

`KeeperRecord` and `DatabaseEntry` are the two that matter: they decide where an event
goes. See [bitmaps / routing](bitmaps.md) for the `FlagRoutingEventSink` + `PersistMode`
mechanism. (`KeeperRecord` is the bit the planned "keep only the last X entries" retention
will hang off of.)

## Internal flags (bits 16–17) — set by the store

| Bit | Name | Meaning | Status |
|----|------|---------|--------|
| 16 | `HasData` | the event carries data | ⚙️ auto — set on `save_event` (`flags_set_has_data`) |
| 17 | `IsInvalid` | the event is marked invalid | 🕳️ placeholder — not set anywhere |

## Metric bits (bits 18–21) <a id="metric-summary-bits"></a>

| Bit | Name | Meaning | Status |
|----|------|---------|--------|
| 18 | `HasIntData` | the event has integer metrics | ⚙️ auto — set by core when ints are present |
| 19 | `HasIntStats` | **an integer summary view is available** (count/min/max/avg) | 📌 advisory — consumer-set |
| 20 | `HasDblData` | the event has float metrics | ⚙️ auto — set by core when floats are present |
| 21 | `HasDblStats` | **a float summary view is available** | 📌 advisory — consumer-set |

`HasIntStats` / `HasDblStats` advertise that the read-side **[metric view](logging.md)**
(`metric_view`) applies to this event — i.e. you can collapse its raw metric values into a
single summary row. They are advisory: the store does not auto-set them, and nothing
enforces them; you set them when you intend the summary to be used.

## Severity (bits 32–34)

A 3-bit field — `NotSet(0)`, `Trace`, `Debug`, `Info`, `Warn`, `Error`, `Critical`,
`Fatal`. Set/get via `set_severity` / `get_severity`, decoded by `to_string`. Used for
display and filtering; **not** consulted by routing.

---

## The honest summary

Of the 13 named flags, only **five drive behavior today**:

- **Routing:** `KeeperRecord`, `DatabaseEntry`
- **Auto-set markers:** `HasData`, `HasIntData`, `HasDblData`

`HasIntStats` / `HasDblStats` are **advisory** (the metric-view availability bits). The
rest — `LogConsole`, `SendNetwork`, `HotCacheHint`, `IsResult`, `IsExplicitNull`,
`IsInvalid` — are **declared intent**: reserved meanings with no sink or logic behind them
yet. They're real bits in the word (you can set/read/serialize them), just not acted upon.

## API quick reference

```cpp
using jac313::Store::v002::StoreFlags;

StoreFlags f;
f.set(StoreFlags::UserFlag::KeeperRecord);        // route to the file sink
f.set(StoreFlags::MetricFlag::HasIntStats);       // advertise the int summary view
f.set_severity(StoreFlags::Severity::Info);
bool kept = f.is_set(StoreFlags::UserFlag::KeeperRecord);
std::uint64_t raw = f.raw();                        // store on the event (PersistedEvent::flags)
std::string human = f.to_string();                  // "KeeperRecord, HasIntStats | Severity: Info"
```

Free helpers operate on a raw `uint64_t` (for hot paths that don't want a `StoreFlags`
object): `set_user_flag`, `set_internal_flag`, `set_metric_flag`, `set_severity`,
`flags_set_has_data`, `flags_clear_has_data`.
