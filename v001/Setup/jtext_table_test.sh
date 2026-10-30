#!/bin/sh
# jtext_table_test.sh — checks Setup/jtext_table.sh (the sh + awk jText reader bsir.sh uses)
# against Setup/compilers.jtext and Setup/recipes.jtext, and its machine-local overrides (the
# Setup/*.local.jtext.example files as local files). Registered with ctest.
set -u
HERE=$(cd "$(dirname "$0")" && pwd)
. "$HERE/jtext_table.sh"
fails=0
check() {  # check NAME GOT WANT
  if [ "$2" = "$3" ]; then echo "ok   $1"; else echo "FAIL $1: got [$2], want [$3]"; fails=$((fails + 1)); fi
}
tab=$(printf '\t')
C="$HERE/compilers.jtext"; R="$HERE/recipes.jtext"

check "toolchains rows" "$(jtext_rows "$C" toolchains | wc -l | tr -d ' ')" 3
check "toolchain_os rows" "$(jtext_rows "$C" toolchain_os | wc -l | tr -d ' ')" 24
check "toolchains fields" "$(jtext_fields "$C" toolchains | tr '\n' ' ')" \
      "id label kind version min_version build_dir flag cc_names scan_deps "
check "clang row" "$(jtext_rows "$C" toolchains | sed -n 3p)" \
      "3${tab}clang${tab}clang${tab}20${tab}20${tab}build-clang20${tab}--clang${tab}clang++-20 clang++20 clang++${tab}clang-scan-deps-20 clang-scan-deps"
check "gcc15 empty scan_deps" "$(jtext_select "$C" toolchains scan_deps label gcc15)" ""
check "gcc15 rhel activation 4" \
      "$(jtext_select "$C" toolchain_os activation toolchain gcc15 os rhel activation_seq 4)" \
      "bash -c 'source /opt/rh/gcc-toolset-15/enable && {CMD}'"
check "gcc16 required on fedora" "$(jtext_select "$C" toolchain_os required toolchain gcc16 os fedora | sort -u)" true

check "components rows" "$(jtext_rows "$R" components | wc -l | tr -d ' ')" 12
check "recipe_os rows" "$(jtext_rows "$R" recipe_os | wc -l | tr -d ' ')" 36
check "clang on fedora" "$(jtext_select "$R" recipe_os command component clang os fedora)" \
      "sudo dnf install -y clang clang-tools-extra"
check "pg18 check" "$(jtext_select "$R" components check component pg18)" \
      '[ -e /usr/pgsql-18/include/libpq-fe.h ] || { v=$(pkg-config --modversion libpq 2>/dev/null); v=${v%%[!0-9]*}; [ -n "$v" ] && [ "$v" -ge 18 ]; }'
check "valgrind required" "$(jtext_select "$R" components required component valgrind)" true
check "upp_gui no check" "$(jtext_select "$R" components check component upp_gui)" ""

# comments on a row, comment lines, and other sections are skipped
T=$(mktemp)
cat > "$T" <<'JT'
=== jText File ===
 1. ?# t.jtext
=== Section: a ===
=== Fields ===
 1. /# id/Integer/Not Null
 2. /# v/String/Not Null
=== End Fields ===
=== Data ===
// a comment line
 1. ^~ 1^x y ~ a comment
 1. ^~ 2^ ^z~w
=== End Data ===
=== End Section ===
=== Section: b ===
=== Fields ===
 1. /# id/Integer/Not Null
=== End Fields ===
=== Data ===
 1. ^~ 9
=== End Data ===
=== End Section ===
=== End File ===
JT
check "rows of a" "$(jtext_rows "$T" a | tr '\n' '/')" "1${tab}x y/2${tab}${tab}z~w/"
check "unknown column" "$(jtext_select "$T" a nope 2>/dev/null; echo "rc=$?")" "rc=1"
rm -f "$T"

# machine-local overrides: the .example files as the local files of copies of the tracked tables
D=$(mktemp -d)
cp "$C" "$R" "$D/"
LC="$D/compilers.jtext"; LR="$D/recipes.jtext"
check "no local file: gcc16 build_dir" "$(jtext_select "$LC" toolchains build_dir label gcc16)" build-gcc16
check "no local file: check passes" "$(jtext_local_check "$LC" toolchains toolchain_os; echo "rc=$?")" "rc=0"
cp "$HERE/compilers.local.jtext.example" "$D/compilers.local.jtext"
cp "$HERE/recipes.local.jtext.example" "$D/recipes.local.jtext"
check "local check passes" "$(jtext_local_check "$LC" toolchains toolchain_os && jtext_local_check "$LR" components recipe_os; echo "rc=$?")" "rc=0"
check "override: gcc16 build_dir" "$(jtext_select "$LC" toolchains build_dir label gcc16)" build-gcc16-local
check "override keeps the row in place" "$(jtext_select "$LC" toolchains label | tr '\n' ' ')" "gcc15 gcc16 clang "
check "add: fedora gcc16 activation 4" "$(jtext_select "$LC" toolchain_os activation toolchain gcc16 os fedora activation_seq 4)" \
      "bash -c 'source /opt/rh/gcc-toolset-16/enable && {CMD}'"
check "remove: fedora gcc15 activation 3" "$(jtext_select "$LC" toolchain_os activation toolchain gcc15 os fedora activation_seq 3)" ""
check "toolchain_os rows (+1 -1)" "$(jtext_select "$LC" toolchain_os id | wc -l | tr -d ' ')" 24
check "override: fedora ninja" "$(jtext_select "$LR" recipe_os command component ninja os fedora)" "sudo dnf install -y ninja-build"
check "add: tmux last" "$(jtext_select "$LR" components component | tail -n1)" tmux
check "remove: dbeaver" "$(jtext_select "$LR" components component component dbeaver)" ""
check "rhel ninja unchanged" "$(jtext_select "$LR" recipe_os command component ninja os rhel)" \
      "sudo dnf install -y python3-pip && python3 -m pip install --user ninja"
# a malformed local file (a section without the column build_dir) is an error naming it
sed 's|^ 6\. /# build_dir/.*$| 6. /# build_dirx/String/Not Null|' "$HERE/compilers.local.jtext.example" > "$D/compilers.local.jtext"
check "malformed: check fails" "$(jtext_local_check "$LC" toolchains toolchain_os 2>/dev/null; echo "rc=$?")" "rc=1"
check "malformed: names the file" \
      "$(jtext_local_check "$LC" toolchains toolchain_os 2>&1 >/dev/null | grep -c "^$D/compilers.local.jtext (section toolchains): no column build_dir")" 1
rm -rf "$D"

[ "$fails" -eq 0 ] && echo "jtext_table_test: all passed" || echo "jtext_table_test: $fails failed"
[ "$fails" -eq 0 ]
