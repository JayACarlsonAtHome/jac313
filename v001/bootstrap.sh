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
#   2. Find a C++23 g++ (+ activation prefix), clang, Ninja >= 1.11, CMake, the sqlite3
#      dev header, the libpq 17+ dev header (PG sink + PG gates), and valgrind with the
#      memcheck/helgrind/DRD dev headers — the full set
#      needed to build AND run the gates, so a fresh box is provisioned in ONE pass.
#   3. If any baseline is missing -> hand off to the committed Setup/jac313_setup
#      provisioner (reads Setup/.setup_handoff) which runs the sudo steps and continues
#      in the *same* bootstrap invocation; falls back to a generated Setup.sh (which
#      still needs a manual re-run of bootstrap after).
#   3b. If the present CMake is not the EXACT version the `import std` pilot is pinned to
#      (PINNED_CMAKE_VER), OFFER a no-sudo CMake install into ~/.local (y/N). The pilot's
#      gate UUID is version-specific, so >= 3.30 is necessary but NOT sufficient. Baseline
#      never requires it.
#   4. Otherwise -> build jac313_test_cli once and hand off ('setup' for readiness;
#      '--run-everything' for the full battery).
#   5. Sense + pin this machine as jac313-### (host_spec + current_host). Auto-pins when
#      unambiguous; otherwise bootstrap stops with --claim / --assign-new-### directions.
#
#   6. With --gui (optional): the U++ GUI build libraries (recipe upp_gui, offered [y/N]),
#      U++ itself into $HOME/u++ (pinned release, SHA-256 checked, no sudo) and then
#      Jac313Runner via `jac313_test_cli gui-build`. A plain bootstrap never does this.
#
# Idempotent. Re-run after a manual Setup.sh (fallback path) or after host pin.
# The preferred path (committed jac313_setup) provisions and continues in one run.
set -eu

ROOT="$(cd "$(dirname "$0")" && pwd)"
RECIPES="$ROOT/Setup/recipes.conf"

WANT_GUI=0
for arg in "$@"; do
  case "$arg" in
    --gui) WANT_GUI=1 ;;
    -h|--help)
      echo "usage: ./bootstrap.sh [--gui]"
      echo "  --gui   also install U++ (no sudo, into \$HOME/u++) and build the Jac313Runner GUI"
      exit 0 ;;
    *) echo "unknown argument: $arg (try --help)" >&2; exit 2 ;;
  esac
done

# Run only from inside this version directory: later steps (host pin, setup) derive the
# version from the working directory, and fail half-way when started from elsewhere.
if [ "$(pwd -P)" != "$(cd "$ROOT" && pwd -P)" ]; then
  echo "bootstrap.sh must be run from inside its version directory:" >&2
  echo "  cd \"$ROOT\" && ./bootstrap.sh $*" >&2
  exit 2
fi

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

# --- compiler + baseline detection (factored so we can re-detect after provisioning) ---
detect_cxx() {
  CXX=""; ACT=""
  try_cxx "" g++-15 \
    || try_cxx "" g++-14 \
    || try_cxx "gcc-toolset-15-env" g++ \
    || try_cxx "scl enable gcc-toolset-15 --" g++ \
    || try_cxx "gcc-toolset-14-env" g++ \
    || try_cxx "scl enable gcc-toolset-14 --" g++ \
    || try_cxx "" g++ \
    || true
}

compute_missing() {
  missing=""
  [ -n "$CXX" ]                    || missing="$missing gcc15"
  ( command -v clang >/dev/null 2>&1 || command -v clang-20 >/dev/null 2>&1 ) || missing="$missing clang"
  command -v cmake >/dev/null 2>&1 || missing="$missing cmake"
  ninja_ok                         || missing="$missing ninja"
  [ -e /usr/include/sqlite3.h ]    || missing="$missing sqlite"
  { [ -e /usr/pgsql-17/include/libpq-fe.h ] \
      || { v=$(pkg-config --modversion libpq 2>/dev/null); v=${v%%[!0-9]*}; [ -n "$v" ] && [ "$v" -ge 17 ]; }; } \
                                   || missing="$missing pg17"
  { command -v valgrind >/dev/null 2>&1 \
      && [ -e /usr/include/valgrind/valgrind.h ] \
      && [ -e /usr/include/valgrind/helgrind.h ] \
      && [ -e /usr/include/valgrind/drd.h ]; }  || missing="$missing valgrind"
}

try_cxx() {  # $1 = activation prefix (may be empty), $2 = compiler name
  p=$($1 sh -c "command -v $2" 2>/dev/null) || return 1
  [ -n "$p" ] || return 1
  major=$($1 "$2" -dumpversion 2>/dev/null | cut -d. -f1) || return 1
  case "$major" in ''|*[!0-9]*) return 1 ;; esac
  [ "$major" -ge 14 ] || return 1
  CXX="$p"; ACT="$1"; return 0
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

download_to() {  # download_to <url> <outfile> [max_seconds, default 180]
  if   command -v curl >/dev/null 2>&1; then curl -fsSL --max-time "${3:-180}" -o "$2" "$1"
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
    echo "(non-interactive — skipping; see the version README for the no-sudo install.)"; return 0
  fi
  printf "Download CMake %s to ~/.local without sudo? [y/N] " "$PINNED_CMAKE_VER"
  read -r ans || ans=""
  case "$ans" in
    [yY]|[yY][eE][sS]) install_local_cmake || echo "Auto-install did not complete; baseline build continues." ;;
    *) echo "Skipped — continuing with CMake $cv (import std stays unavailable until $PINNED_CMAKE_VER is installed)." ;;
  esac
}

# --- optional (--gui): U++ and the Jac313Runner GUI --------------------------------
# U++ comes from upp_basefix (github.com/JayACarlsonAtHome/upp_basefix): U++ pinned to a GitHub
# commit, plus patches that make it build with 0 warnings and run clean under the sanitizers
# and valgrind. Its patch 0001 also fixes the GCC 15 start-up crash of plain U++ 2026.1, so the
# GUI is built with GCC again. An install is $UPP_DIR/{umk,uppsrc,GCC.bm,CLANG.bm}.
UPP_DIR="${UPP_DIR:-$HOME/upp_basefix}"
UPP_BASEFIX_REPO="${UPP_BASEFIX_REPO:-https://github.com/JayACarlsonAtHome/upp_basefix.git}"
UPP_BASEFIX_SRC="${UPP_BASEFIX_SRC:-$HOME/upp_basefix_src}"   # its clone (the script, the patches)
UPP_METHOD="GCC"

upp_gui_libs_ok() {  # what U++ CtrlCore links for a GTK (Wayland-first) build, plus zlib (JTextGui .xlsx),
                     # and the patch + rsync that upp_basefix needs to make the patched tree
  command -v patch >/dev/null 2>&1 && command -v rsync >/dev/null 2>&1 \
    && command -v pkg-config >/dev/null 2>&1 \
    && pkg-config --exists gtk+-3.0 libnotify freetype2 fontconfig x11 xinerama xrender xft xcb xext zlib
}

install_upp() {  # U++ into $UPP_DIR via upp_basefix; keeps an existing install with umk + uppsrc
  if [ -x "$UPP_DIR/umk" ] && [ -d "$UPP_DIR/uppsrc" ]; then
    echo "U++ found at $UPP_DIR ($(head -1 "$UPP_DIR/VERSION" 2>/dev/null || echo 'umk + uppsrc')) — using it."; return 0
  fi
  if [ -e "$UPP_DIR" ] && [ -n "$(ls -A "$UPP_DIR" 2>/dev/null)" ]; then
    echo "$UPP_DIR exists but has no umk + uppsrc. Not touching it: move it aside and re-run."
    return 1
  fi
  if [ ! -x "$UPP_BASEFIX_SRC/upp_basefix.sh" ]; then
    command -v git >/dev/null 2>&1 || { echo "git missing — cannot fetch upp_basefix."; return 1; }
    echo "Fetching upp_basefix -> $UPP_BASEFIX_SRC…"
    git clone -q "$UPP_BASEFIX_REPO" "$UPP_BASEFIX_SRC" || { echo "git clone failed (network?)."; return 1; }
  fi
  # its bootstrap: the pinned U++ from GitHub, umk built from it, the patches; it lists the
  # system packages U++ needs and installs them only when asked (sudo)
  if ! "$UPP_BASEFIX_SRC/upp_basefix.sh" bootstrap; then
    if [ ! -t 0 ]; then echo "(non-interactive — not installing; run $UPP_BASEFIX_SRC/upp_basefix.sh bootstrap --install)"; return 1; fi
    printf "Install those packages now (sudo)? [y/N] "
    read -r ans || ans=""
    case "$ans" in
      [yY]|[yY][eE][sS]) "$UPP_BASEFIX_SRC/upp_basefix.sh" bootstrap --install || { echo "upp_basefix bootstrap failed."; return 1; } ;;
      *) echo "Skipped — U++ is not installed."; return 1 ;;
    esac
  fi
  "$UPP_BASEFIX_SRC/upp_basefix.sh" install "$UPP_DIR" || { echo "upp_basefix install failed."; return 1; }
  echo "Installed $(head -1 "$UPP_DIR/VERSION") -> $UPP_DIR"
}

bootstrap_gui() {  # --gui: build libraries ([y/N]), U++ (no sudo), then Jac313Runner
  echo
  echo "=== GUI (--gui): U++ (upp_basefix) + Jac313Runner ==="
  if ! upp_gui_libs_ok; then
    cmd=$(recipe_cmd upp_gui "$family")
    if [ -z "$cmd" ]; then
      echo "No upp_gui recipe for '$family' in Setup/recipes.conf. Install the GTK3, libnotify,"
      echo "FreeType, Fontconfig and X11 development packages by hand, then: ./bootstrap.sh --gui"
      return 1
    fi
    echo "U++ needs these build libraries:"
    echo "  $cmd"
    if [ ! -t 0 ]; then echo "(non-interactive — not installing; run that, then ./bootstrap.sh --gui)"; return 1; fi
    printf "Install them now? [y/N] "
    read -r ans || ans=""
    case "$ans" in
      [yY]|[yY][eE][sS]) sh -c "$cmd" || { echo "Installing the GUI libraries failed."; return 1; } ;;
      *) echo "Skipped — the GUI is not built."; return 1 ;;
    esac
    upp_gui_libs_ok || { echo "The GUI libraries are still missing (pkg-config)."; return 1; }
  fi
  install_upp || return 1
  # U++ keeps one object cache per build method; stale objects from an older U++ break the link,
  # so a failed build is retried once with --clean (rebuild everything).
  ( cd "$ROOT" && UPP_DIR="$UPP_DIR" "$CLI" gui-build --method "$UPP_METHOD" ) \
    || { echo "Retrying with a clean rebuild (stale U++ cache)…"; ( cd "$ROOT" && UPP_DIR="$UPP_DIR" "$CLI" gui-build --method "$UPP_METHOD" --clean ); } \
    || { echo "gui-build failed."; return 1; }
  echo "Start it as your own user: $(dirname "$ROOT")/gui/out/Jac313Runner"
}

# --- 2. find a C++23 g++ (>= 14) and the activation prefix needed to invoke it ---
# (use the helpers defined above so we can re-detect cleanly after sudo provisioning)
detect_cxx
compute_missing

# --- 3. baseline check; provision (exe or Setup.sh) if anything is missing ---
# The valgrind check requires the memcheck header AND the helgrind + DRD headers: the
# annotated verify tree (-DJAC313_STORE_HELGRIND_ANNOTATE=ON) #includes <valgrind/helgrind.h>
# and the DRD annotations, which ship in valgrind-devel (Debian bundles them in `valgrind`).
# So baseline can't pass with a partial valgrind that would break the verify/verify-lite gate.

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
      echo; echo "Provisioning finished."
      # Re-detect now that the (sudo) packages are present; continue in this same run
      # so the no-sudo CMake offer + build + cli handoff all happen without a manual re-run.
      detect_cxx
      compute_missing
      if [ -n "$missing" ]; then
        echo "Baseline still missing after provisioning:$missing"
        echo "Fix the issues and re-run: ./bootstrap.sh"
        exit 1
      fi
      # fall through to build/handoff in this invocation
    else
      echo; echo "Provisioning did not fully complete (see above). Fix, then re-run: ./bootstrap.sh"
      exit 1
    fi
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
$ACT cmake --build "$BUILD" --target jac313_wipe_all
$ACT cmake --build "$BUILD" --target jac313_wipe_one
$ACT cmake --build "$BUILD" --target jac313_wipe_jac
CLI="$BUILD/tools/jac313_test_cli"

# Convenience symlinks at the version root so everyday commands read `./jac313_test_cli ...`,
# `./jac313_wipe_all ...` (whole DB), `./jac313_wipe_one --<N> ...` (one run) and
# `./jac313_wipe_jac --<G> ...` (one machine) (targets live in the gitignored build-bootstrap/;
# the symlinks are gitignored too).
ln -sfn "build-bootstrap/tools/jac313_test_cli" "$ROOT/jac313_test_cli"
ln -sfn "build-bootstrap/tools/jac313_wipe_all" "$ROOT/jac313_wipe_all"
ln -sfn "build-bootstrap/tools/jac313_wipe_one" "$ROOT/jac313_wipe_one"
ln -sfn "build-bootstrap/tools/jac313_wipe_jac" "$ROOT/jac313_wipe_jac"
echo "Linked ./jac313_test_cli -> build-bootstrap/tools/jac313_test_cli"
echo "Linked ./jac313_wipe_all -> build-bootstrap/tools/jac313_wipe_all"
echo "Linked ./jac313_wipe_one -> build-bootstrap/tools/jac313_wipe_one"
echo "Linked ./jac313_wipe_jac -> build-bootstrap/tools/jac313_wipe_jac"

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

# --- optional GUI (--gui). Non-fatal: the test tools above are ready either way. ---
if [ "$WANT_GUI" = 1 ]; then
  bootstrap_gui || echo "(GUI step did not complete — the test tools are ready; fix the above and re-run ./bootstrap.sh --gui)"
fi

echo
echo "=== handing off to jac313_test_cli ==="
$ACT "$CLI" setup

# Sense + pin THIS machine (host_spec + current_host) and show the hardware grid, so every later
# gate records under the same jac313-### without re-sensing.
echo
if ! $ACT "$CLI" host; then
  echo
  echo "=== host pin required — bootstrap paused ==="
  echo "Every gate records under a fleet label (jac313-###). Auto-pin failed above."
  echo
  echo "  1. Inspect the fleet table and this machine's hardware (printed above)."
  echo "  2. Choose ONE:"
  echo "       $ACT $CLI host --claim jac313-###       # reuse an existing slot (same OS+hardware)"
  echo "       $ACT $CLI host --assign-new-###         # new slot (e.g. --assign-new-002)"
  echo "  3. Re-run: ./bootstrap.sh"
  echo
  echo "Read-only precheck: $ACT $CLI --group-id"
  echo "Details: the version README (Machine identity)"
  exit 1
fi

echo
echo "Bootstrap complete. Run the FULL battery (every gate on both compilers + build-times + report):"
echo "  $ACT $CLI --run-everything"
echo
echo "Or compose individual gates: --ctest, --smoke, --bench --report, --verify-lite, --verify."
echo "Full matrix details: the version README"

# --- the next step for a new user: the Jac313Runner window (built by --gui) ---
RUNNER="$(dirname "$ROOT")/gui/out/Jac313Runner"
if [ -x "$RUNNER" ]; then
  echo
  echo "=== Next: Jac313Runner, the window that runs the tests ==="
  echo "Choose the gates (start with ctest + smoke), press Start, and read the results in it."
  echo "Start it any time with:"
  echo "  $RUNNER --version $(basename "$ROOT")"
  if [ -t 0 ] && [ -n "${WAYLAND_DISPLAY:-}${DISPLAY:-}" ]; then
    printf "Start Jac313Runner now? [Y/n] "
    read -r ans || ans=n
    case "$ans" in
      [nN]*) ;;
      *) # Terminals like Ptyxis stop every process of a tab when it closes (systemd scope), so
         # setsid alone is not enough: systemd-run --user makes the window its own user service
         if command -v systemd-run > /dev/null 2>&1 &&
            systemd-run --user --collect --quiet --unit "jac313-runner-$$" \
              --working-directory "$ROOT" "$RUNNER" --version "$(basename "$ROOT")" 2> /dev/null; then
           :
         else
           ( cd "$ROOT" && setsid "$RUNNER" --version "$(basename "$ROOT")" > /dev/null 2>&1 < /dev/null & )
         fi
         echo "Started. It keeps running when this terminal is closed." ;;
    esac
  fi
elif [ "$WANT_GUI" != 1 ]; then
  echo
  echo "Prefer a window? ./bootstrap.sh --gui also builds Jac313Runner."
fi
