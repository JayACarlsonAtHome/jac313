#!/usr/bin/env bash
#
# Rebuild the COMMITTED jac313_setup provisioner binary.
#
# jac313_setup is checked into the repo (it arrives with a clone and runs before any
# compiler/ninja exists), so it is built STATIC to run across the fleet's distros and
# glibc versions, and at C++23 so it stays backport-friendly to v001. Re-run this and
# commit Setup/jac313_setup whenever the Setup library or its recipe handling changes.
#
# Usage:  cd v002 && ./Setup/build_setup_exe.sh
set -eu

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
OUT="$ROOT/Setup/jac313_setup"

# --- find a C++23 g++ (>= 14), with the activation prefix this host needs -----------
CXX=""; ACT=""
try_cxx() {  # $1 = activation prefix (may be empty), $2 = compiler name
  p=$($1 sh -c "command -v $2" 2>/dev/null) || return 1
  [ -n "$p" ] || return 1
  major=$($1 "$2" -dumpversion 2>/dev/null | cut -d. -f1) || return 1
  case "$major" in ''|*[!0-9]*) return 1 ;; esac
  [ "$major" -ge 14 ] || return 1
  CXX="$2"; ACT="$1"; return 0
}
try_cxx "" g++-15 \
  || try_cxx "" g++-14 \
  || try_cxx "gcc-toolset-15-env" g++ \
  || try_cxx "scl enable gcc-toolset-15 --" g++ \
  || try_cxx "gcc-toolset-14-env" g++ \
  || try_cxx "scl enable gcc-toolset-14 --" g++ \
  || try_cxx "" g++ \
  || { echo "no C++23 g++ (>= 14) found — cannot build jac313_setup" >&2; exit 1; }

echo "Compiler: ${ACT:+[$ACT] }$CXX"

# --- preflight: a full-static link needs the static C/C++ runtime archives ----------
# These are a BUILD-HOST-ONLY dependency (the committed binary is static, so clone hosts
# need nothing). If a -static link fails, provision them on demand from the recipes.conf
# `static_runtime` recipe (CRB repo + glibc-static/libstdc++-static on RHEL; libc6-dev on
# Debian/Mint). The recipe is the single source of these commands; this just runs it.
probedir="$(mktemp -d)"
static_ok() { printf 'int main(){return 0;}\n' | $ACT "$CXX" -static -x c++ - -o "$probedir/probe" 2>/dev/null; }

if ! static_ok; then
  echo
  echo "Static C/C++ runtime archives are missing — needed only to build jac313_setup -static."

  # detect OS family (tokens match Setup/recipes.conf) to pick the static_runtime recipe
  family=unknown
  if [ -r /etc/os-release ]; then
    # shellcheck disable=SC1091
    . /etc/os-release 2>/dev/null || true
    case "$(printf '%s' "${ID:-}" | tr '[:upper:]' '[:lower:]')" in
      linuxmint) family=linuxmint ;;
      fedora)    family=fedora ;;
      rhel|centos|rocky|almalinux|ol|scientific|sl) family=rhel ;;
      *) case "$(printf '%s' "${ID_LIKE:-}" | tr '[:upper:]' '[:lower:]')" in
           *rhel*|*fedora*) family=rhel ;;
         esac ;;
    esac
  fi
  cmd=$(sed -n "s/^[[:space:]]*static_runtime\\.$family[[:space:]]*=[[:space:]]*//p" "$ROOT/Setup/recipes.conf" | head -n1)

  if [ -z "$cmd" ]; then
    echo "No static_runtime recipe for family '$family' — install glibc-static + libstdc++-static manually, then re-run." >&2
    rm -rf "$probedir"; exit 1
  fi
  echo "Provision recipe (family: $family):"
  echo "  $cmd"
  ans=""
  [ -t 0 ] && { printf "Run it now? (uses sudo) [y/N] "; read -r ans || ans=""; }
  case "$ans" in
    [yY]|[yY][eE][sS])
      sh -c "$cmd" || { echo "Provisioning failed — fix the cause and re-run." >&2; rm -rf "$probedir"; exit 1; }
      static_ok || { echo "Still cannot static-link after install — see errors above." >&2; rm -rf "$probedir"; exit 1; }
      echo "Static runtime installed."
      ;;
    *)
      echo "Skipped. Install it (recipe above), then re-run: ./Setup/build_setup_exe.sh" >&2
      rm -rf "$probedir"; exit 1
      ;;
  esac
fi
rm -rf "$probedir"

# --- compile: static, C++23, the Setup library TU + this main ----------------------
# -static            : fleet-portable single binary (no glibc-version dependency)
# -static-libstdc++  : (implied by -static) keep libstdc++ in the binary too
set -x
$ACT "$CXX" -std=c++23 -O2 -static -pthread \
  -I "$ROOT/Setup/include" \
  "$ROOT/Setup/src/v002.cpp" \
  "$ROOT/Setup/setup_main.cpp" \
  -o "$OUT"
set +x

strip "$OUT" 2>/dev/null || true
chmod +x "$OUT"

echo
echo "Built: $OUT"
echo "  size:    $(du -h "$OUT" | cut -f1)"
echo "  linkage: $(ldd "$OUT" 2>&1 | head -n1)"
echo "  kind:    $(file -b "$OUT" 2>/dev/null | cut -d, -f1-2)"
echo
echo "Commit it:  git add -f Setup/jac313_setup"
