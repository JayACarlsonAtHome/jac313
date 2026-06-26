#!/bin/bash
# Build-time matrix: for each test, compile+link it in each (front-end × compiler) config and
# record the COMPILE time (only) + run pass/fail + exe size. Renders via `jac313_test_cli --report`
# into test-summary/build/.
#
#   usage:  tools/build_matrix.sh [test ...]      # default: a 2-test smoke set
#
# ALL build trees live under  v002/tmp_build/<config>  so cleanup is a single `rm -rf tmp_build`.
# Each config's tree is also dropped right after it records (peak disk = one config). Every delete
# goes through safe_rmdir(), which REFUSES any path not under tmp_build.
#
# Configs (6): gcc15 {headers, modules, import-std} + clang {headers, modules, import-std}.
# (import std validated on g++-15 and clang-21 + libstdc++.) This is a DELIBERATE measurement —
# NOT part of the verify/verify-lite pre-push gate.
set -u
cd "$(dirname "$0")/.." || exit 1
# Compilers: override via env on a machine where these paths differ (the fresh-VM case). Defaults are
# this author's gcc-toolset-15 + system clang. build_matrix is a deliberate local measurement, so it
# names the compilers explicitly rather than going through the compilers.conf registry — but fail
# fast with a clear message instead of a cryptic cmake error if a path is wrong.
GXX="${JAC313_MATRIX_GXX:-/opt/rh/gcc-toolset-15/root/usr/bin/g++}"
CLXX="${JAC313_MATRIX_CLXX:-/usr/bin/clang++}"
for cc in "$GXX" "$CLXX"; do
  [ -x "$cc" ] || { echo "build_matrix: compiler not found: $cc  (override with JAC313_MATRIX_GXX / JAC313_MATRIX_CLXX)" >&2; exit 1; }
done
ISTD="-DJAC313_BUILD_MODULES=ON -DJAC313_QLITE_IMPORT_STD=ON -DJAC313_JTEXT_IMPORT_STD=ON -DJAC313_STORE_IMPORT_STD=ON"
TESTS="${*:-jac313_store_001_TS jac313_store_002_TS}"
CLI=./jac313_test_cli
BUILD_ROOT="tmp_build"

# Safety: only ever delete tmp_build or something inside it. Refuse anything else (a source dir,
# an absolute path, empty, ..) so a bug can never nuke the wrong tree.
safe_rmdir() {
  case "$1" in
    tmp_build|tmp_build/*) rm -rf "./$1" ;;
    *) echo "REFUSING to delete '$1' (not under tmp_build)" >&2 ;;
  esac
}

build_config() {
  local cc="$1"; local name="$2"; shift 2; local feflags="$*"
  local dir="$BUILD_ROOT/$name"
  echo "===== CONFIG $name ====="
  safe_rmdir "$dir"
  # logs are SIBLING files under tmp_build, so per-config cleanup leaves them for debugging;
  # the final `rm -rf tmp_build` takes them too.
  if ! cmake -G Ninja -S . -B "$dir" -DCMAKE_CXX_COMPILER="$cc" -DCMAKE_BUILD_TYPE=Debug \
        -DJAC313_BUILD_STORE_TESTS=ON $feflags >"${dir}.cfg.log" 2>&1; then
    echo "[$name] CONFIGURE FAILED (see ${dir}.cfg.log)"; safe_rmdir "$dir"; return; fi
  cmake --build "$dir" --target $TESTS >>"${dir}.cfg.log" 2>&1 || echo "[$name] warm-build issues"
  for t in $TESTS; do
    ninja -C "$dir" -t clean "$t" >/dev/null 2>&1
    local t0 t1 ms rc st bin sz
    t0=$(date +%s%N)
    cmake --build "$dir" --target "$t" >"${dir}.$t.log" 2>&1; rc=$?
    t1=$(date +%s%N); ms=$(( (t1 - t0) / 1000000 ))
    if [ $rc -ne 0 ]; then st=error
    elif ctest --test-dir "$dir" -R "^${t}\$" >/dev/null 2>&1; then st=pass; else st=fail; fi
    bin=$(find "$dir" -name "$t" -type f -perm -u+x 2>/dev/null | head -1)
    sz=$([ -n "$bin" ] && stat -c%s "$bin" || echo 0)
    "$CLI" record-build-test --build-dir "$dir" --test "$t" --build-ms "$ms" --bytes "$sz" --status "$st"
  done
  safe_rmdir "$dir"   # results are in the DB; drop this config's tree
}

mkdir -p "$BUILD_ROOT"
build_config "$GXX"  gcc-hdr
build_config "$GXX"  gcc-mod    -DJAC313_BUILD_MODULES=ON
build_config "$GXX"  gcc-istd   $ISTD
build_config "$CLXX" clang-hdr
build_config "$CLXX" clang-mod  -DJAC313_BUILD_MODULES=ON
build_config "$CLXX" clang-istd $ISTD

"$CLI" --report
safe_rmdir "$BUILD_ROOT"   # final wholesale sweep — one place, all gone
echo "######## BUILD-MATRIX DONE ########"
