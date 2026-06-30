#!/usr/bin/env bash
#
# jac313 bootstrap — the thin, bare-metal entry point.
#
# Shell is unavoidable here: you cannot run a compiled tool before you have a
# compiler. So this stays as small as possible and pushes all the real intelligence
# into jac313_test_cli (the data-driven, activation-aware tool), which it builds once
# and then hands off to.
#
#   1. Detect the OS family (minimal mirror of Setup::sense_host).
#   2. Find a C++23 g++ (+ activation prefix), Ninja >= 1.11, and CMake.
#   3. If any baseline is missing -> generate a reviewable Setup.sh from
#      Setup/recipes.conf and stop, so you run it (with sudo) by hand.
#   3b. If the present CMake is not the EXACT version the `import std` pilot is pinned to
#      (PINNED_CMAKE_VER), OFFER a no-sudo CMake install into ~/.local (y/N). The pilot's
#      gate UUID is version-specific, so >= 3.30 is necessary but NOT sufficient. Baseline
#      never requires it.
#   4. Otherwise -> build jac313_test_cli once and hand off ('setup' for readiness;
#      'matrix run-all' for the full platform-aware build).
#
# Re-run after Setup.sh. Idempotent.
set -eu

ROOT="$(cd "$(dirname "$0")" && pwd)"
RECIPES="$ROOT/Setup/recipes.conf"

lc() { printf '%s' "$1" | tr '[:upper:]' '[:lower:]'; }

# --- 1. detect OS family (tokens match Setup/recipes.conf + compilers.conf) ---
family="unknown"
if [ -r /etc/os-release ]; then
  # shellcheck disable=SC1091
  . /etc/os-release 2>/dev/null || true
  id_l=$(lc "${ID:-}"); like_l=$(lc "${ID_LIKE:-}"); pretty_l=$(lc "${PRETTY_NAME:-}")
  case "$id_l" in
    linuxmint) family=linuxmint ;;
    fedora)    family=fedora ;;
    rhel|centos|rocky|almalinux|ol|scientific|sl) family=rhel ;;
    *)
      case "$pretty_l" in *"linux mint"*) family=linuxmint ;; esac
      case "$like_l" in *rhel*|*fedora*) [ "$family" = unknown ] && family=rhel ;; esac
      [ "$family" = unknown ] && [ -n "$id_l" ] && family="$id_l"
      ;;
  esac
fi
echo "Platform family: $family"

# --- enable a UTF-8 console so output renders (not mojibake) ---
# Reports + logs are plain UTF-8 (═ · –): they render in editors (gedit, VS Code) AND on
# GitHub. ANSI colour is terminal-native and lives only in live console output, never in
# files (an editor shows raw escape bytes). UTF-8 just needs a locale — prefer an already
# -UTF-8 LC_*/LANG, else fall back to C.UTF-8. The export carries into the build + tests.
enable_utf8_console() {
  case "${LC_ALL:-${LANG:-}}" in
    *UTF-8*|*UTF8*|*utf-8*|*utf8*) return 0 ;;   # already UTF-8
  esac
  if locale -a 2>/dev/null | grep -qiE '^C\.UTF-?8$'; then
    export LANG=C.UTF-8 LC_ALL=C.UTF-8
  elif locale -a 2>/dev/null | grep -qiE 'en_US\.UTF-?8$'; then
    export LANG=en_US.UTF-8 LC_ALL=en_US.UTF-8
  else
    echo "WARNING: no UTF-8 locale found — UTF-8 box-drawing output may render as mojibake."
    echo "  Install one (Fedora/RHEL: sudo dnf install glibc-langpack-en;"
    echo "  Debian/Mint: sudo locale-gen en_US.UTF-8), then re-run."
    return 0
  fi
  echo "Console: UTF-8 enabled (LANG=$LANG)"
}
enable_utf8_console

# look up an install command from recipes.conf:  recipe_cmd <component> <family>
recipe_cmd() {
  [ -r "$RECIPES" ] || return 1
  sed -n "s/^[[:space:]]*$1\\.$2[[:space:]]*=[[:space:]]*//p" "$RECIPES" | head -n1
}

ninja_ok() {
  v=$(ninja --version 2>/dev/null) && [ -n "$v" ] \
    && [ "$(printf '1.11\n%s\n' "$v" | sort -V | head -n1)" = "1.11" ]
}

# --- optional: a newer CMake (no sudo) for the `import std` pilot ----------------
# The `import std;` pilot (-DJAC313_*_IMPORT_STD=ON) uses CMake's native std-module
# support. That feature is EXPERIMENTAL and armed by a CMake-version-specific gate UUID:
# IMPORT_STD_MIN (>= 3.30) is the floor at which the feature exists at all, but it is NOT
# sufficient — only the EXACT PINNED_CMAKE_VER carries the UUID pinned in CMakeLists.txt's
# CMAKE_EXPERIMENTAL_CXX_IMPORT_STD. Any other version (older OR newer) has a different UUID
# and the pilot will not configure. The baseline build needs only CMake >= 3.26, so the
# pinned CMake is OFFERED, never required. PINNED_CMAKE_VER MUST stay in lockstep with that
# UUID in CMakeLists.txt.
IMPORT_STD_MIN="3.30"
PINNED_CMAKE_VER="4.3.3"

cmake_version() { cmake --version 2>/dev/null | sed -n '1s/.*version \([0-9][0-9.]*\).*/\1/p'; }
ver_ge() { [ "$(printf '%s\n%s\n' "$2" "$1" | sort -V | head -n1)" = "$2" ]; }  # ver_ge A B → A>=B

download_to() {  # download_to <url> <outfile>
  if   command -v curl >/dev/null 2>&1; then curl -fsSL --max-time 180 -o "$2" "$1"
  elif command -v wget >/dev/null 2>&1; then wget -qO "$2" "$1"
  else echo "neither curl nor wget found" >&2; return 127; fi
}

install_local_cmake() {  # install PINNED_CMAKE_VER into ~/.local (no sudo). Non-fatal on failure.
  V="$PINNED_CMAKE_VER"; dest="$HOME/.local/opt/cmake-$V"
  if [ ! -x "$dest/bin/cmake" ]; then
    command -v sha256sum >/dev/null 2>&1 || { echo "sha256sum missing — cannot verify download; aborting auto-install."; return 1; }
    tmp=$(mktemp -d) || return 1
    tb="cmake-$V-linux-x86_64.tar.gz"; base="https://github.com/Kitware/CMake/releases/download/v$V"
    echo "Downloading $tb (verified)…"
    download_to "$base/$tb"                 "$tmp/$tb"     || { echo "download failed (network?). See README 'Newer CMake on Linux Mint'."; rm -rf "$tmp"; return 1; }
    download_to "$base/cmake-$V-SHA-256.txt" "$tmp/sha.txt" || { echo "checksum download failed."; rm -rf "$tmp"; return 1; }
    ( cd "$tmp" && grep "$tb" sha.txt | sha256sum -c - ) || { echo "CHECKSUM MISMATCH — refusing to install."; rm -rf "$tmp"; return 1; }
    mkdir -p "$dest" "$HOME/.local/bin"
    tar -xzf "$tmp/$tb" -C "$dest" --strip-components=1 || { echo "extract failed."; rm -rf "$tmp"; return 1; }
    rm -rf "$tmp"
  else
    echo "CMake $V already installed at $dest — relinking."
  fi
  for t in cmake ctest cpack ccmake; do ln -sf "$dest/bin/$t" "$HOME/.local/bin/$t"; done
  export PATH="$HOME/.local/bin:$PATH"; hash -r
  echo "Installed CMake $V → ~/.local (symlinked into ~/.local/bin). System CMake untouched."
  case ":$PATH:" in *":$HOME/.local/bin:"*) : ;; *) echo "  NOTE: add ~/.local/bin to PATH (ahead of /usr/bin) to keep using it.";; esac
}

maybe_offer_local_cmake() {  # offer the pinned CMake whenever the import-std gate won't activate
  cv=$(cmake_version); [ -n "$cv" ] || return 0
  # The import-std gate UUID is version-specific, so the ONLY CMake that activates the pilot
  # is the exact PINNED_CMAKE_VER. >= IMPORT_STD_MIN is necessary but not sufficient — a
  # different version (even a newer one) has a different UUID and fails to configure. Hence
  # we offer the no-sudo install whenever CMake is not exactly the pinned version.
  [ "$cv" = "$PINNED_CMAKE_VER" ] && return 0
  echo
  if ver_ge "$cv" "$IMPORT_STD_MIN"; then
    echo "CMake $cv clears the baseline and the >= $IMPORT_STD_MIN import-std floor, but the"
    echo "optional 'import std' pilot (-DJAC313_*_IMPORT_STD=ON) is gated on a CMake-version-"
    echo "specific UUID pinned to $PINNED_CMAKE_VER. On $cv the configure fails (different UUID),"
    echo "so the pilot needs *exactly* CMake $PINNED_CMAKE_VER."
  else
    echo "CMake $cv works for the baseline, but the optional 'import std' pilot"
    echo "(-DJAC313_*_IMPORT_STD=ON) needs *exactly* CMake $PINNED_CMAKE_VER (version-pinned gate UUID)."
  fi
  if [ ! -t 0 ]; then
    echo "(non-interactive — skipping; see README / docs/Setup.md for the no-sudo install.)"; return 0
  fi
  printf "Download CMake %s to ~/.local without sudo? [y/N] " "$PINNED_CMAKE_VER"
  read -r ans || ans=""
  case "$ans" in
    [yY]|[yY][eE][sS]) install_local_cmake || echo "Auto-install did not complete; baseline build continues." ;;
    *) echo "Skipped — continuing with CMake $cv (import std stays unavailable until $PINNED_CMAKE_VER is installed)." ;;
  esac
}

# --- 2. find a C++23 g++ (>= 14) and the activation prefix needed to invoke it ---
CXX=""; ACT=""
try_cxx() {  # $1 = activation prefix (may be empty), $2 = compiler name
  p=$($1 sh -c "command -v $2" 2>/dev/null) || return 1
  [ -n "$p" ] || return 1
  major=$($1 "$2" -dumpversion 2>/dev/null | cut -d. -f1) || return 1
  case "$major" in ''|*[!0-9]*) return 1 ;; esac
  [ "$major" -ge 14 ] || return 1
  CXX="$p"; ACT="$1"; return 0
}
try_cxx "" g++-15 \
  || try_cxx "" g++-14 \
  || try_cxx "gcc-toolset-15-env" g++ \
  || try_cxx "scl enable gcc-toolset-15 --" g++ \
  || try_cxx "gcc-toolset-14-env" g++ \
  || try_cxx "scl enable gcc-toolset-14 --" g++ \
  || try_cxx "" g++ \
  || true

# --- 3. baseline check; generate Setup.sh if anything is missing ---
missing=""
[ -n "$CXX" ]                  || missing="$missing gcc15"
command -v cmake >/dev/null 2>&1 || missing="$missing cmake"
ninja_ok                       || missing="$missing ninja"
[ -e /usr/include/sqlite3.h ]  || missing="$missing sqlite"

if [ -n "$missing" ]; then
  echo "Baseline missing:$missing"

  # Write the handoff the committed provisioner reads. bootstrap senses; jac313_setup
  # executes. Regenerated every run; gitignored.
  HANDOFF="$ROOT/Setup/.setup_handoff"
  {
    echo "# jac313 setup handoff — written by bootstrap.sh, read by Setup/jac313_setup."
    echo "# Regenerated every run; safe to delete."
    echo "family=$family"
    [ -n "$ACT" ] && echo "activation=$ACT"
    for comp in $missing; do echo "component=$comp"; done
  } > "$HANDOFF"

  # Prefer the prebuilt, committed provisioner: it does real per-step error handling
  # (captured exit codes, [y/N] confirm, --dry-run, continue-on-failure summary) that a
  # generated shell script cannot. It is built static, so it runs even before any
  # compiler/ninja exists. Probe it with --version so a wrong-arch / unrunnable binary
  # falls back cleanly to the generated Setup.sh below.
  SETUP_EXE="$ROOT/Setup/jac313_setup"
  if [ -x "$SETUP_EXE" ] && "$SETUP_EXE" --version >/dev/null 2>&1; then
    echo
    echo "Provisioning via the committed Setup/jac313_setup (handoff: Setup/.setup_handoff)."
    echo "Preview without changes:  ./Setup/jac313_setup --source-dir \"$ROOT\" --dry-run \"$HANDOFF\""
    if "$SETUP_EXE" --source-dir "$ROOT" "$HANDOFF"; then
      echo; echo "Provisioning finished. Re-run: ./bootstrap.sh"
    else
      echo; echo "Provisioning did not fully complete (see above). Fix, then re-run: ./bootstrap.sh"
    fi
    exit 1
  fi

  # --- fallback: no usable committed binary -> generate a reviewable Setup.sh ---------
  echo "(committed Setup/jac313_setup not present/runnable here — falling back to Setup.sh)"
  {
    echo "#!/usr/bin/env bash"
    echo "# Auto-generated by jac313 bootstrap on $(date -u +%Y-%m-%dT%H:%M:%SZ) — REVIEW first."
    echo "# Installs the baseline needed to build jac313_test_cli on family: $family"
    echo "# These commands modify your system and usually need sudo. Read them."
    echo "# Each step runs independently; failures are reported at the end, not aborted on,"
    echo "# so one bad recipe can no longer hide the installs after it."
    echo "set -uo pipefail"
    echo
    echo "__jac_fail=0"
    echo
    for comp in $missing; do
      cmd=$(recipe_cmd "$comp" "$family" || true)
      if [ -n "$cmd" ]; then
        echo "echo; echo \"=== $comp ===\""
        echo "if $cmd"
        echo "then echo \"  [ok]   $comp\""
        echo "else echo \"  [FAIL] $comp\"; __jac_fail=1"
        echo "fi"
        echo
      else
        echo "echo \"  [FAIL] $comp — no recipe for family '$family'; install it manually.\"; __jac_fail=1"
        echo
      fi
    done
    echo "echo"
    echo "if [ \"\$__jac_fail\" -eq 0 ]; then"
    echo "  echo 'Baseline installed. Re-run: ./bootstrap.sh'"
    echo "else"
    echo "  echo 'One or more steps FAILED above — fix those, then re-run this script.'"
    echo "fi"
    echo "exit \"\$__jac_fail\""
  } > "$ROOT/Setup.sh"
  chmod +x "$ROOT/Setup.sh"
  echo
  echo "Wrote Setup.sh. Review it, then run it by hand:"
  echo "  bash Setup.sh"
  echo "When it finishes, re-run: ./bootstrap.sh"
  exit 1
fi

echo "Bootstrap compiler: ${ACT:+[$ACT] }$CXX"
echo "Ninja: $(ninja --version)   CMake: $(cmake --version | head -n1)"

# baseline is satisfied; optionally offer a newer (no-sudo) CMake for the import std pilot
maybe_offer_local_cmake

# --- 4. build jac313_test_cli once, then hand off to it ---
BUILD="$ROOT/build-bootstrap"
echo
echo "=== building jac313_test_cli (one-time bootstrap compile) ==="
$ACT cmake -G Ninja -S "$ROOT" -B "$BUILD" \
  -DCMAKE_CXX_COMPILER="$CXX" -DCMAKE_BUILD_TYPE=Debug
$ACT cmake --build "$BUILD" --target jac313_test_cli
$ACT cmake --build "$BUILD" --target jac313_wipe_new
CLI="$BUILD/tools/jac313_test_cli"

# Convenience entry points: symlinks at the v002/ root so everyday use is ./jac313_test_cli and
# ./jac313_wipe_new instead of the deep build path. Relative targets survive a repo move; both are
# gitignored (the targets live in the gitignored build-bootstrap/).
ln -sfn "build-bootstrap/tools/jac313_test_cli" "$ROOT/jac313_test_cli"
ln -sfn "build-bootstrap/tools/jac313_wipe_new" "$ROOT/jac313_wipe_new"
echo "Linked ./jac313_test_cli -> build-bootstrap/tools/jac313_test_cli"
echo "Linked ./jac313_wipe_new -> build-bootstrap/tools/jac313_wipe_new"

# --- compiler pin (Setup/compilers.pin): find-or-create THIS machine's row ---
# Already-pinned box -> no-op (the pin is never overwritten). New machine -> sense + pin the highest
# available gcc + clang, so every later run uses the SAME compilers deterministically. Non-fatal: a
# pin hiccup must never block bootstrap.
echo
echo "=== compiler pin (find-or-create) ==="
$ACT "$CLI" pin --ensure --source-dir "$ROOT" || echo "(pin --ensure skipped — non-fatal)"

# --- install the pre-push gate hook (git hooks are local, not version-controlled) ---
# Runs the valgrind verify-lite before every push. Bypass once with: git push --no-verify
# Under the v00N/ layout the repo's .git lives at the PARENT root (shared by every
# version), not inside this version dir — so resolve it via git rather than $ROOT/.git.
HOOK_SRC="$ROOT/tools/hooks/pre-push"
GIT_DIR=""
if command -v git >/dev/null 2>&1; then
  GIT_DIR="$(git -C "$ROOT" rev-parse --git-common-dir 2>/dev/null || true)"
  # git was run with -C "$ROOT", so a relative result is relative to $ROOT.
  case "$GIT_DIR" in
    "" ) : ;;
    /* ) : ;;
    *  ) GIT_DIR="$ROOT/$GIT_DIR" ;;
  esac
fi
if [ -n "$GIT_DIR" ] && [ -d "$GIT_DIR" ] && [ -f "$HOOK_SRC" ]; then
  mkdir -p "$GIT_DIR/hooks"
  HOOK_DST="$GIT_DIR/hooks/pre-push"
  if [ ! -e "$HOOK_DST" ] || ! cmp -s "$HOOK_SRC" "$HOOK_DST"; then
    cp "$HOOK_SRC" "$HOOK_DST"
    chmod +x "$HOOK_DST"
    echo "Installed pre-push hook -> $HOOK_DST (valgrind verify-lite; bypass: git push --no-verify)"
  fi
fi

echo
echo "=== handing off to jac313_test_cli ==="
$ACT "$CLI" setup

# Sense + pin THIS machine (host_spec + current_host) and show the hardware grid, so every later
# gate — and store_bench — record under the same jac313-### without re-sensing.
echo
$ACT "$CLI" host

echo
echo "Bootstrap complete. Run the everyday base check from v002/:"
echo "  ./jac313_test_cli --ctest --smoke      # ctest unit suite + smoke matrix"
echo
echo "It writes the exact commands to ./run_latest_config.sh and runs them — re-run that"
echo "script any time. Gates compose: add --bench --report, --verify-lite, or --verify."
echo
echo "Full battery (gcc15 + clang; smoke + full Debug/Release; modules) — see:"
echo "  docs/RunAllTests.md"
