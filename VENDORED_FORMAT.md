# VENDORED.jtext — vendored-copy manifest format (jac313, jac314), 2026-10-16

The Captain, 2026-10-16: vendored copies (work on a new version without breaking anything),
with **warnings on drift, not breaking**; the same model for jac313 and jac314.

A vendored copy is a snapshot of a canonical repo's directory, copied (and possibly
transformed) into a consuming project. Beside the copy's root sits `VENDORED.jtext`.

## Locations

- `projects dir` = the directory that holds the git checkouts (parent of the consuming
  project's git root), wherever that is: e.g. the drive's `git` folder, mounted at a
  different place on each OS. Nothing records it; it is found from the project's own location.
- `canonical_dir` = path of the canonical directory **relative to the projects dir**, e.g.
  `jac414/v001/gsttime`, `jText/v001`, `Qlite/v001/Qlite`.
- The manifest lives in the vendored root, e.g. `jac314/v001/xgox/third_party/jac414/gsttime/
  VENDORED.jtext`, `jac313/v001/jText/VENDORED.jtext`. File paths in it are relative to the
  manifest's directory (vendored side) and to canonical_dir (canonical side), `/`-separated.

## Layout (jText; two sections)

```
//File:    VENDORED.jtext
//Purpose: Vendored copy of <canonical_dir> (do not edit the copied files here; change the
//         canonical repo, then sync). Drift is reported as warnings, never as failures.
//
=== jText File ===
 1. #?# VENDORED
 3. #?# Vendored copy of <canonical_dir>


=== Section: source ===


=== Fields ===
 1. #/# id/Identity(1,1)/Not Null
 2. #/# key/String/Not Null
 3. #/# value/String
=== End Fields ===


=== Data ===
1. #|# 1|canonical_repo|jac414

1. #|# 2|canonical_dir|jac414/v001/gsttime
...
=== End Data ===
=== End Section ===


=== Section: files ===


=== Fields ===
 1. #/# id/Identity(1,1)/Not Null
 2. #/# path/String/Not Null             # vendored file, relative to the manifest dir
 3. #/# canonical_path/String/Not Null   # canonical file, relative to canonical_dir
 4. #/# canonical_sha256/String/Not Null # canonical file content when copied (hex, lower case)
 5. #/# vendored_sha256/String/Not Null  # vendored file content as written (after transform)
=== End Fields ===


=== Data ===
1. #|# 1|gst.go|gst.go|<64 hex>|<64 hex>
...
=== End Data ===
=== End Section ===
=== End File ===
```

Rows are compact (`1. #|# ...`, a blank line after each). Values never contain `|`.
Empty value = NULL (write the Unit Separator byte 0x1F). Field 1 counts from 1 per section.

### source keys (one row each, this order)

| key | value |
|---|---|
| canonical_repo | repo name, e.g. `jac414`, `jText`, `Qlite` |
| canonical_dir | relative to the projects dir |
| canonical_commit | `git rev-parse --short=12 HEAD` of the canonical repo at copy time |
| canonical_dirty | `Yes` when the canonical dir had uncommitted changes at copy time, else `No` |
| canonical_version | the canonical version string if known (e.g. jac414 `Version`, jText `v001.006`), else NULL |
| transform | `none` or `jac313-prefix` (rules below) |
| ported | date `YYYY-MM-DD` of the copy |
| baseline | `Yes` = a starting line written over an existing copy (no record of its true origin); `No` = written by a real sync |
| local | comma list of vendored paths/globs owned by the consumer, never synced or checked (e.g. `CMakeLists.txt,*/CMakeLists.txt,README.md,VENDORED.jtext`) |
| exclude | comma list of canonical paths/globs not copied (e.g. `build,build-*,tools/tests`) |
| notes | free text (e.g. known drift at baseline), or NULL |

Globs: `*` matches within one path segment; a pattern without `/` matches the base name in
any directory; a pattern naming a directory matches everything under it.

## Drift check (warnings only; always exit 0)

For each manifest:
1. **Edited in place:** a listed vendored file whose current SHA-256 != vendored_sha256, or is
   missing. Warning: `<copy>: <path> edited in place (fix it in <canonical_repo> first)`.
2. **Extra file:** a file under the vendored root that is not listed, not `local`, not the
   manifest. Warning `... not from canonical`.
3. **Canonical moved ahead:** if `<projects>/<canonical_dir>` exists: a listed canonical file
   whose current SHA-256 != canonical_sha256 (changed) or missing (removed), and canonical files
   not listed and not excluded (added). One summary line per copy plus one line per file:
   `<copy>: canonical <repo> changed N files since <commit> (now <HEAD>[ dirty])`.
4. **Canonical missing:** info line only.
Output lines start with `vendor-check: ` and `WARN`/`INFO`/`OK`. A final line
`vendor-check: <N> warning(s)`. Exit code 0 always (a `--strict` flag may return 1 for CI).

## Sync (writes)

Copy every canonical file under canonical_dir (minus exclude), apply the transform to the
content and the path, write it into the vendored root (never touching `local` paths), delete
vendored files that are listed in the old manifest but no longer exist in canonical, then
write a fresh manifest (baseline = No). `--dry-run` (the default in jac313) lists what would be
written/deleted and the content changes, writing nothing.

## Transforms

- `none`: path and content unchanged.
- `jac313-prefix` (PROCESS.md): for library L in {jText, Qlite}, lower-case l:
  - content of `.h .hpp .cpp .inc .cppm` files:
    - `L::` -> `jac313::L::` where the character before `L` is not a letter, digit, `_` or `:`
    - `<L/` -> `<jac313/L/` and `"L/` -> `"jac313/L/`
    - `L.vNNN` -> `jac313.l.vNNN` where the character before `L` is not a letter, digit or `_`
  - path: a file name starting `L.vNNN` -> `jac313.l.vNNN`; directory `include/L/` ->
    `include/jac313/L/`.
  - Other files (CMake, Markdown, data) are copied unchanged unless `local`.
