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

build_config() {
  local cc="$1"; local dir="$2"; shift 2; local feflags="$*"
  echo "===== CONFIG $dir ====="
  rm -rf "$dir"
  if ! cmake -G Ninja -S . -B "$dir" -DCMAKE_CXX_COMPILER="$cc" -DCMAKE_BUILD_TYPE=Debug \
        -DJAC313_BUILD_STORE_TESTS=ON $feflags >"$dir/.cfg.log" 2>&1; then
    echo "[$dir] CONFIGURE FAILED"; return; fi
  cmake --build "$dir" --target $TESTS >>"$dir/.cfg.log" 2>&1 || echo "[$dir] warm-build issues"
  for t in $TESTS; do
    ninja -C "$dir" -t clean "$t" >/dev/null 2>&1
    local t0 t1 ms rc st
    t0=$(date +%s%N)
    cmake --build "$dir" --target "$t" >"$dir/.$t.log" 2>&1; rc=$?
    t1=$(date +%s%N); ms=$(( (t1 - t0) / 1000000 ))
    if [ $rc -ne 0 ]; then st=error
    elif ctest --test-dir "$dir" -R "^${t}\$" >/dev/null 2>&1; then st=pass; else st=fail; fi
    "$CLI" record-build-test --build-dir "$dir" --test "$t" --build-ms "$ms" --status "$st"
  done
}

build_config "$GXX"  build-mx-gcc-hdr
build_config "$GXX"  build-mx-gcc-mod   -DJAC313_BUILD_MODULES=ON
build_config "$GXX"  build-mx-gcc-istd  $ISTD
build_config "$CLXX" build-mx-clang-hdr
build_config "$CLXX" build-mx-clang-mod -DJAC313_BUILD_MODULES=ON

# clang import-std: a g++-15 pilot only, so clang can't build it yet -> record NA so the column
# still appears. WHEN CLANG GAINS import-std, replace this loop with:
#   build_config "$CLXX" build-mx-clang-istd $ISTD
echo "===== CONFIG clang import-std (NA — not built by design) ====="
for t in $TESTS; do
  "$CLI" record-build-test --build-dir build-mx-clang-hdr --test "$t" --build-ms 0 --status NA --modules on --import-std on
done

"$CLI" --report
echo "######## BUILD-MATRIX DONE ########"
