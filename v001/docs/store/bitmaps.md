# Store — Routing (flags → sinks)

Every event carries a 64-bit `StoreFlags` word. This page is about how those flags **route
events to sinks**. For the full bit-by-bit catalog with meanings and an honest
wired/advisory/placeholder status, see **[Flags](flags.md)**.

← [Store docs index](../../Store/README.md) · [main README](../../README.md) ·
related: [flags (full catalog)](flags.md) · [logging](logging.md) · [persistence](persistence.md)

---

## The two flags that route

Of all the flags, only two decide *where* an event goes — both set at log time, no schema
change (the `event_flags` argument to [`save_event`](logging.md) carries them):

| Flag | Bit | Sends the event to |
|------|-----|--------------------|
| `KeeperRecord` | 1 | the **keeper (file)** sink (binary / jtext / html / json) |
| `DatabaseEntry` | 2 | the **SQL** sink |

---

## Routing: flags + `PersistMode`

Two layers decide where an event goes:

**1. Per-event flags** say where the event should go:

- `KEEPER_MASK` (bit 1, `KeeperRecord`) → the **file** sink (binary / jtext / html / json)
- `DATABASE_MASK` (bit 2, `DatabaseEntry`) → the **SQL** sink
- An event can route to both, one, or neither.

**2. `PersistMode`** on each sink (`persistence/PersistCommon.hpp`) picks the events it keeps:

| Mode | Behaviour |
|------|-----------|
| `All` | write every event (ignores per-event flags — for single-sink setups) |
| `KeeperOnly` | write only if `KeeperRecord` (bit 1) is set |
| `DatabaseOnly` | write only if `DatabaseEntry` (bit 2) is set |

A typical split: file sink in `KeeperOnly`, SQL sink in `DatabaseOnly`, both attached to the
store (`attach_persistence` once per sink; each gets its own worker, see
[persistence](persistence.md)) — the app just tags events; the sink modes do the rest. Each sink
must own its own file or connection.

---

## Strengths

- **Dynamic, per-event routing** with no schema or config change — tag at log time.
- **Single-instruction** bitset ops; cheap on the hot path.
- **Orthogonal flags** express cross-cutting intent (`KeeperRecord` *and* `SendNetwork` at once).

## Weaknesses / footguns

- **Magic bit positions.** Callers hardcode shifts (`1ULL << 1` for `KeeperRecord`); there isn't
  a friendly named-constant API in the consumer namespace, so getting a bit wrong is a silent
  routing bug. Wrap the masks in your own named constants.
- **Additive only.** You express *what is* set, never "don't route here" — there's no negative
  flag.
- **Severity is baked into the bitset** (a 3-bit field, max 8 levels) rather than a first-class
  enum, and routing doesn't consult it — logging level and routing are coupled in one word.

> Routing is the seam where Store, [persistence](persistence.md), and your domain meet. If you're
> evaluating Store, the flag/`PersistMode` matrix is the first thing worth modelling against your
> real "keep vs query vs drop" policy.
