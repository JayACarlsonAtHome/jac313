#!/bin/bash
# Build-time matrix: for each test, compile+link it in each (front-end × compiler) config and
# record the COMPILE time (only) plus the run pass/fail. Renders via `jac313_test_cli --report`
# into test-summary/build/.
#
#   usage:  tools/build_matrix.sh [test ...]      # default: a 2-test smoke set
#
# Configs (5): gcc15 {headers, modules, import-std} + clang {headers, modules}.
# import-std is a g++-15 pilot only, so clang has no import-std config (by design).
#
# Method: configure fresh per config, warm the shared lib / module BMIs once, then per test
# `ninja -t clean <target>` + time `cmake --build --target <target>` (compile+link only),
# then run it untimed via ctest for pass/fail. This is a DELIBERATE, separately-run measurement —
# it is NOT part of the verify/verify-lite pre-push gate.
set -u
cd "$(dirname "$0")/.." || exit 1
GXX=/opt/rh/gcc-toolset-15/root/usr/bin/g++
CLXX=/usr/bin/clang++
ISTD="-DJAC313_BUILD_MODULES=ON -DJAC313_QLITE_IMPORT_STD=ON -DJAC313_JTEXT_IMPORT_STD=ON -DJAC313_STORE_IMPORT_STD=ON"
TESTS="${*:-jac313_store_001_TS jac313_store_002_TS}"
CLI=./jac313_test_cli

# Safety: only ever delete a build- tree we created. Refuse anything else (empty, absolute,
# a source dir, ..). Every cleanup below goes through this, so a bug can't nuke the wrong path.
safe_rmdir() {
  case "$1" in
    build-*) rm -rf "./$1" ;;
    *) echo "REFUSING to delete '$1' (not a build-* dir)" >&2 ;;
  esac
}

build_config() {
  local cc="$1"; local dir="$2"; shift 2; local feflags="$*"
  echo "===== CONFIG $dir ====="
  safe_rmdir "$dir"
  # logs go to SIBLING files (outside $dir) so they survive the cleanup rm at the end.
  if ! cmake -G Ninja -S . -B "$dir" -DCMAKE_CXX_COMPILER="$cc" -DCMAKE_BUILD_TYPE=Debug \
        -DJAC313_BUILD_STORE_TESTS=ON $feflags >"${dir}.cfg.log" 2>&1; then
    echo "[$dir] CONFIGURE FAILED (see ${dir}.cfg.log)"; safe_rmdir "$dir"; return; fi
  cmake --build "$dir" --target $TESTS >>"${dir}.cfg.log" 2>&1 || echo "[$dir] warm-build issues"
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
  # be nice to the user's disk: results are in the DB now, so drop the (big) build tree.
  safe_rmdir "$dir"
}

build_config "$GXX"  build-mx-gcc-hdr
build_config "$GXX"  build-mx-gcc-mod   -DJAC313_BUILD_MODULES=ON
build_config "$GXX"  build-mx-gcc-istd  $ISTD
build_config "$CLXX" build-mx-clang-hdr
build_config "$CLXX" build-mx-clang-mod -DJAC313_BUILD_MODULES=ON

# clang import-std: validated (clang-21 + libstdc++ 15) — now a real config, not NA.
build_config "$CLXX" build-mx-clang-istd $ISTD

"$CLI" --report
echo "######## BUILD-MATRIX DONE ########"
