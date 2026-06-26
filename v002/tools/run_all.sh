#!/bin/bash
# jac313 v002 — one command from a fresh clone: bootstrap, then run EVERY gate on BOTH compilers,
# the build-time matrix, and render the report into test-summary/. This is the full battery —
# verify (valgrind) and bench run on two compilers, so it takes a while. Grab a coffee.
#
#   usage:  tools/run_all.sh
#
# Everyday checks are much cheaper — see docs/Setup.md "Gate commands" (e.g. ./jac313_test_cli --ctest --smoke).
set -u
cd "$(dirname "$0")/.." || exit 1
echo "######## jac313 run-all START ########"

# 1. bootstrap (sense toolchains, build the ./jac313_test_cli runner, install the pre-push hook)
./bootstrap.sh < /dev/null || { echo "bootstrap FAILED"; exit 1; }

# 2. fresh results DB (regenerable; re-run repopulates it)
rm -f test-summary/results.db

# 3. functional (ctest + smoke) on both compilers, in separate per-compiler trees
for pair in "gcc15:build-gcc15" "clang:build-clang"; do
  cc=${pair%%:*}; dir=${pair##*:}
  echo "===== FUNCTIONAL ($cc) ====="
  ./jac313_test_cli configure --build-dir "$dir" --"$cc"; echo "[exit $?] configure $cc"
  ./jac313_test_cli build      --build-dir "$dir";        echo "[exit $?] build $cc"
  ./jac313_test_cli run        --build-dir "$dir";        echo "[exit $?] ctest $cc"
  ./jac313_test_cli matrix run --build-dir "$dir";        echo "[exit $?] smoke $cc"
done

# 4. throughput bench on both compilers (Release), recorded into results.db
for pair in "gcc15:build-bench-gcc" "clang:build-bench-clang"; do
  cc=${pair%%:*}; dir=${pair##*:}
  echo "===== BENCH ($cc) ====="
  ./jac313_test_cli configure --release --build-dir "$dir" --"$cc"; echo "[exit $?] configure bench $cc"
  cmake --build "$dir" --target jac313_store_bench;                 echo "[exit $?] build bench $cc"
  "$dir/Store/tests/matrix/jac313_store_bench" --suite --db test-summary/results.db --jtext-ver v002.002
  echo "[exit $?] bench $cc"
done

# 5. valgrind verify (memcheck + helgrind + DRD) on both compilers
echo "===== VERIFY (gcc15) ====="; ./jac313_test_cli matrix verify --gcc15; echo "[exit $?] verify gcc15"
echo "===== VERIFY (clang) ====="; ./jac313_test_cli matrix verify --clang; echo "[exit $?] verify clang"

# 6. build-time matrix (per test × front-end × compiler) — self-cleaning under tmp_build/
echo "===== BUILD MATRIX ====="; tools/build_matrix.sh

# 7. render the report from results.db
./jac313_test_cli --report
echo "######## jac313 run-all DONE — open test-summary/README.md ########"
