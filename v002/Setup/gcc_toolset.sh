#!/bin/bash
# File:    Setup/gcc_toolset.sh
# Purpose: The newest GCC Toolset on the RHEL family. Red Hat ships a new gcc-toolset-N beside the
#          old ones (RHEL 10.3 has gcc-toolset-16), and the system g++ stays at the release default,
#          so the newest toolset is the compiler jac313 wants there. Only one is needed: the newest
#          the enabled repositories offer, found by asking dnf, never named here. An older one that
#          is installed is left alone.
# Usage:   Setup/gcc_toolset.sh --newest | --check | --install | --gcc-install-dir
#            --newest    print the newest toolset's number (16), from dnf's cache
#            --check     exit 0 when the newest is installed, or when this is not the RHEL family
#            --install   install the newest when it is missing (sudo)
#            --gcc-install-dir
#                        print the newest installed toolset's GCC folder, for clang's --gcc-install-dir;
#                        nothing when this is not the RHEL family or none is installed. Red Hat's clang
#                        is set to one fixed toolset's C++ library (/etc/clang/*.cfg); CMakeLists.txt
#                        gives it this one, the newest, the same library gcc builds with.
# Exit:    0 done / installed   1 missing, or the install failed   2 bad usage
# Related: bsir.sh (compute_missing), Setup/recipes.jtext (gcc_toolset, gcc15, gcc16 on rhel),
#          Setup/compilers.jtext (the toolchains row that names each GCC major).
set -u

HERE=$(cd "$(dirname "$0")" && pwd)

# The search starts at FIRST_TOOLSET_TO_TRY and counts down to LAST_TOOLSET_TO_TRY, the GCC
# minimum (Setup/compilers.jtext). Raise the first when Red Hat gets close to it.
readonly FIRST_TOOLSET_TO_TRY=20
readonly LAST_TOOLSET_TO_TRY=15

# newest [cache]: the first N from FIRST_TOOLSET_TO_TRY down whose gcc-toolset-N-gcc-c++ is offered
# or installed; '' when none. dnf is asked once for every toolset, not once per N.
# "cache" asks only dnf's cache (fast, no network: the readiness check has 20 s).
newest() {
  known=$(
    if [ "${1:-}" = cache ]; then dnf -q -C repoquery --qf '%{name}\n' 'gcc-toolset-*-gcc-c++' 2>/dev/null
    else dnf -q repoquery --qf '%{name}\n' 'gcc-toolset-*-gcc-c++' 2>/dev/null; fi
    rpm -qa --qf '%{NAME}\n' 'gcc-toolset-*-gcc-c++' 2>/dev/null
  )
  n=$FIRST_TOOLSET_TO_TRY
  while [ "$n" -ge "$LAST_TOOLSET_TO_TRY" ]; do
    printf '%s\n' "$known" | grep -qx "gcc-toolset-$n-gcc-c++" && { echo "$n"; return 0; }
    n=$((n - 1))
  done
}

# a toolchains row in compilers.jtext for GCC major $1, or a note that one is needed
note_row() {
  . "$HERE/jtext_table.sh"
  [ -n "$(jtext_select "$HERE/compilers.jtext" toolchains label kind gcc version "$1")" ] && return 0
  echo "gcc-toolset-$1 is the newest, but Setup/compilers.jtext has no toolchains row for GCC $1:"
  echo "add a gcc$1 row (and its toolchain_os rows) so jac313 builds with it."
}

is_rhel() { [ -n "$(rpm -E '%{?rhel}' 2>/dev/null)" ]; }

# the GCC folder of the newest installed toolset, asked of its own g++; '' when none is installed
newest_installed_dir() {
  n=$FIRST_TOOLSET_TO_TRY
  while [ "$n" -ge "$LAST_TOOLSET_TO_TRY" ]; do
    if rpm -q "gcc-toolset-$n-gcc-c++" >/dev/null 2>&1; then
      for run in "gcc-toolset-$n-env" "scl enable gcc-toolset-$n --"; do
        lib=$($run g++ -print-libgcc-file-name 2>/dev/null) && [ -n "$lib" ] && { dirname "$lib"; return 0; }
      done
    fi
    n=$((n - 1))
  done
}

case "${1:-}" in
  --newest)
    n=$(newest cache); [ -n "$n" ] || exit 1; echo "$n" ;;
  --check)
    is_rhel || exit 0
    n=$(newest cache)
    [ -n "$n" ] || { echo "no gcc-toolset is offered or installed" >&2; exit 1; }
    rpm -q "gcc-toolset-$n-gcc-c++" >/dev/null 2>&1 ;;
  --install)
    is_rhel || { echo "not the RHEL family: no gcc-toolset to install" >&2; exit 1; }
    n=$(newest)
    [ -n "$n" ] || { echo "no gcc-toolset is offered by the enabled repositories" >&2; exit 1; }
    if ! rpm -q "gcc-toolset-$n-gcc-c++" >/dev/null 2>&1; then
      echo "Installing gcc-toolset-$n, the newest this system offers"
      sudo dnf install -y "gcc-toolset-$n" || exit 1
    fi
    note_row "$n" ;;
  --gcc-install-dir)
    is_rhel || exit 0
    newest_installed_dir ;;
  *)
    echo "usage: Setup/gcc_toolset.sh --newest | --check | --install | --gcc-install-dir" >&2; exit 2 ;;
esac
