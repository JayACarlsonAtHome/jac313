# Store — Categories (and payload sizing)

How events are tagged and how their text fields are sized — the `bounded_string` mechanism
shared by the category and the payload.

← [Store docs index](../../Store/README.md) · [main README](../../README.md) ·
related: [logging](logging.md) · [persistence](persistence.md) · [reporting](reporting.md)

---

## Category vs payload

Every event carries two text fields:

- a **category** — a light tag for grouping (`"network"`, `"disk_io"`, `"cache_hit"`, …)
- a **payload** — the event body / message

Both use the same fixed-size storage type, sized independently by the config.

## `bounded_string<MaxCodepoints>`

`Store/include/jac313/Store/v002/headers/ts_store_config.hpp` (≈ lines 25–87). A fixed-capacity,
inline, **UTF-8-codepoint-bounded** string:

- The template parameter is a **codepoint** count, not a byte count. The inline buffer is
  `MaxCodepoints * 4 + 1` bytes (worst-case UTF-8 expansion + null terminator).
- `assign_truncated(string_view, max_cp)` walks the UTF-8 bytes, counts codepoints, and stops at
  the limit **without splitting a multibyte sequence**.
- Converts implicitly to `std::string_view`; `.str()` makes a `std::string` copy.

The config wires the two field types:

| Type alias | Capacity (codepoints, default) |
|------------|-------------------------------|
| `Config::CategoryT` = `bounded_string<MaxCategoryLength>` | 20 |
| `Config::ValueT` = `bounded_string<MaxPayloadLength>` | 80 |

So a payload of `MaxPayloadLength = 80` reserves `80*4 + 1 = 321` bytes inline per row — safe for
any UTF-8, generous for ASCII.

---

## Strengths

- **UTF-8-correct truncation** — never breaks a multibyte character.
- **Zero-copy, allocation-free** — direct write into the row's inline buffer after counting.
- **Predictable layout** — every field is at a known offset; no pointer chasing, no heap.

## Weaknesses / footguns

- **4× byte overhead for ASCII.** The codepoint→byte expansion means ASCII-heavy logs waste ~¾
  of each text buffer; there's no compact ASCII variant.
- **Silent truncation.** Over-length category/payload is dropped with no exception or warning —
  size your limits to your real data, or you lose the tail without noticing.
- **Codepoint counting costs.** The UTF-8 walk inspects every byte; high-rate ASCII logging pays
  for correctness it may not need.
- **Template explosion.** Each distinct `(MaxCategoryLength, MaxPayloadLength)` pair is a new row
  layout and a new `ts_store` instantiation — keep the set of configs small.

> Categories are intentionally lightweight (a tag, not a schema). If you need rich, queryable
> grouping, lean on the SQL sink's columns + your own indexes (see [reporting](reporting.md))
> rather than encoding structure into the category string.
