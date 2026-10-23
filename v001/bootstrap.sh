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
#      dev header, the libpq 18+ dev header (PG sink + PG gates), and valgrind with the
#      memcheck/helgrind/DRD dev headers — the full set
#      needed to build AND run the gates, so a fresh box is provisioned in ONE pass.
#   3. If any of it is missing -> install it here, item by item, from Setup/recipes.conf
#      (after the one "Install everything?" yes and one sudo password), then go on.
#   4. Build jac313_test_cli once and let it check this machine ('setup' for readiness;
#      '--run-everything' for the full battery).
#   5. Sense + pin this machine as jac313-### (host_spec + current_host). Auto-pins when
#      unambiguous; otherwise bootstrap stops with --claim / --assign-new-### directions.
#
#   6. A PostgreSQL 18 server for the PG local gate (role + database jac313).
#   7. The GUI (skipped with --no-gui): the U++ GUI build libraries (recipe upp_gui), U++ from
#      upp_basefix into $HOME/upp_basefix, Jac313Runner via `jac313_test_cli gui-build`, and
#      DBeaver; then the Runner opens.
#
# Idempotent: re-run it any time; what is already there is left alone.
set -eu

ROOT="$(cd "$(dirname "$0")" && pwd)"
RECIPES="$ROOT/Setup/recipes.conf"

WANT_GUI=1   # the Runner window is part of a normal bootstrap; --no-gui skips it
ASSUME_YES=0
for arg in "$@"; do
  case "$arg" in
    --gui) WANT_GUI=1 ;;
    --no-gui) WANT_GUI=0 ;;
    -y|--yes) ASSUME_YES=1 ;;
    -h|--help)
      echo "usage: ./bootstrap.sh [--yes] [--no-gui]"
      echo "Installs what is missing, builds the tests and the Jac313Runner window (U++ into"
      echo "\$HOME/upp_basefix), adds DBeaver (a database browser), then opens the Runner."
      echo "  --yes      install everything that is missing without asking"
      echo "  --no-gui   no window: skip U++, the Runner and DBeaver (e.g. a server without a screen)"
      echo "Without --yes it asks once \"Install everything?\" and then asks for the sudo password"
      echo "once; that one answer and password cover the whole run."
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
  { [ -e /usr/pgsql-18/include/libpq-fe.h ] \
      || { v=$(pkg-config --modversion libpq 2>/dev/null); v=${v%%[!0-9]*}; [ -n "$v" ] && [ "$v" -ge 18 ]; }; } \
                                   || missing="$missing pg18"
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

# --- the GUI (skipped with --no-gui): U++ and the Jac313Runner window ----------------
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
  # system packages U++ needs and installs them only when asked (sudo). A yes to the GUI
  # libraries above already covers these, so the user is asked once.
  if [ "${UPP_PKGS_AGREED:-0}" = 1 ] || [ "$ASSUME_YES" = 1 ]; then
    "$UPP_BASEFIX_SRC/upp_basefix.sh" bootstrap --install || { echo "upp_basefix bootstrap failed."; return 1; }
  elif ! "$UPP_BASEFIX_SRC/upp_basefix.sh" bootstrap; then
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

bootstrap_gui() {  # build libraries ([y/N]), U++ (no sudo), then Jac313Runner
  echo
  echo "=== GUI: U++ (upp_basefix) + Jac313Runner ==="
  if ! upp_gui_libs_ok; then
    cmd=$(recipe_cmd upp_gui "$family")
    if [ -z "$cmd" ]; then
      echo "No upp_gui recipe for '$family' in Setup/recipes.conf. Install the GTK3, libnotify,"
      echo "FreeType, Fontconfig and X11 development packages by hand, then: ./bootstrap.sh"
      return 1
    fi
    echo "U++ needs these build libraries (and a few more that upp_basefix finds once U++ is fetched):"
    echo "  $cmd"
    if [ "$ASSUME_YES" = 1 ]; then
      ans=y
    else
      if [ ! -t 0 ]; then echo "(non-interactive — not installing; run that, then ./bootstrap.sh)"; return 1; fi
      printf "Install them now? [y/N] "
      read -r ans || ans=""
    fi
    case "$ans" in
      [yY]|[yY][eE][sS]) sh -c "$cmd" || { echo "Installing the GUI libraries failed."; return 1; }
                         UPP_PKGS_AGREED=1 ;;
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
  install_dbeaver || true   # optional; the GUI is ready either way
}

dbeaver_ok() { sh -c "$(recipe_cmd dbeaver check)"; }

install_dbeaver() {  # DBeaver for browsing results.db and the database tables ([Y/n])
  dbeaver_ok && { echo "DBeaver: installed."; return 0; }
  cmd=$(recipe_cmd dbeaver "$family")
  [ -n "$cmd" ] || { echo "No dbeaver recipe for '$family' in Setup/recipes.conf."; return 1; }
  ans=y
  if [ "$ASSUME_YES" != 1 ]; then
    [ -t 0 ] || { echo "(non-interactive — DBeaver not installed; run: $cmd)"; return 0; }
    echo
    printf "Install DBeaver (database browser for results.db and PostgreSQL; ~130 MB download)? [Y/n] "
    read -r ans || ans=""
  fi
  case "$ans" in
    [nN]*) echo "Skipped DBeaver."; return 0 ;;
  esac
  sh -c "$cmd" || { echo "DBeaver install failed (network?). Later: $cmd"; return 1; }
  echo "DBeaver installed (in the app menu)."
}

# --- PG local: a PostgreSQL 18 server on THIS machine for the "PG local" gate ---
# The gate connects with "host=localhost dbname=jac313 user=jac313" (PgClient.hpp). install_pg_local
# installs the server (Fedora: postgresql-server; RHEL: PGDG postgresql18-server; Mint: PGDG
# postgresql-18), starts it at boot, creates the role + database jac313 with a random password,
# allows that login over localhost (scram-sha-256 lines at the top of pg_hba.conf, marked
# "jac313 pg_local") and stores the password in your pgpass ($PGPASSFILE or ~/.pgpass, mode 600),
# never in git. A working login is left alone.
pg_psql() {
  local p
  for p in "$(command -v psql 2>/dev/null)" /usr/pgsql-18/bin/psql /usr/lib/postgresql/18/bin/psql; do
    [ -n "$p" ] && [ -x "$p" ] && { echo "$p"; return 0; }
  done
  return 1
}
pg_local_ok() {
  local p
  p=$(pg_psql) || return 1
  PGCONNECT_TIMEOUT=5 "$p" -w -h localhost -U jac313 -d jac313 -Atc "select 1" >/dev/null 2>&1
}
pg_su() { ( cd / && sudo -u postgres "$(pg_psql)" -v ON_ERROR_STOP=1 -Atq "$@" ); }
install_pg_local() {
  local service pw hba pgpass
  case "$family" in
    fedora)
      rpm -q postgresql-server >/dev/null 2>&1 || sudo dnf install -y postgresql-server || return 1
      sudo test -f /var/lib/pgsql/data/PG_VERSION || sudo postgresql-setup --initdb || return 1
      service=postgresql ;;
    rhel)
      rpm -q pgdg-redhat-repo >/dev/null 2>&1 || sudo dnf install -y \
        "https://download.postgresql.org/pub/repos/yum/reporpms/EL-$(rpm -E %rhel)-$(uname -m)/pgdg-redhat-repo-latest.noarch.rpm" || return 1
      rpm -q postgresql18-server >/dev/null 2>&1 || sudo dnf install -y postgresql18-server || return 1
      sudo test -f /var/lib/pgsql/18/data/PG_VERSION || sudo /usr/pgsql-18/bin/postgresql-18-setup initdb || return 1
      service=postgresql-18 ;;
    linuxmint)
      if ! dpkg -s postgresql-18 >/dev/null 2>&1; then
        [ -x /usr/share/postgresql-common/pgdg/apt.postgresql.org.sh ] || sudo apt-get install -y postgresql-common || return 1
        [ -e /etc/apt/sources.list.d/pgdg.sources ] || [ -e /etc/apt/sources.list.d/pgdg.list ] \
          || sudo /usr/share/postgresql-common/pgdg/apt.postgresql.org.sh -y || return 1
        sudo apt-get install -y postgresql-18 || return 1   # creates and starts the cluster 18/main
      fi
      service=postgresql ;;
    *) echo "PG local: no recipe for '$family'."; return 1 ;;
  esac
  sudo systemctl enable --now "$service" || return 1
  for _ in $(seq 1 20); do pg_su -c "select 1" >/dev/null 2>&1 && break; sleep 1; done
  pw=$(head -c 24 /dev/urandom | base64 | tr -dc 'A-Za-z0-9' | head -c 24)
  if [ "$(pg_su -c "select 1 from pg_roles where rolname = 'jac313'")" = 1 ]; then
    pg_su -c "alter role jac313 with login password '$pw'" || return 1
  else
    pg_su -c "create role jac313 with login password '$pw'" || return 1
  fi
  [ "$(pg_su -c "select 1 from pg_database where datname = 'jac313'")" = 1 ] \
    || pg_su -c "create database jac313 owner jac313" || return 1
  hba=$(pg_su -c "show hba_file") || return 1
  if ! sudo grep -q "jac313 pg_local" "$hba"; then
    # pg_hba.conf is read top-down and the first match wins, so these go first.
    sudo sed -i '1i # jac313 pg_local: the PG local gate (bootstrap.sh)\nhost    jac313    jac313    127.0.0.1/32    scram-sha-256\nhost    jac313    jac313    ::1/128         scram-sha-256' "$hba" || return 1
  fi
  pg_su -c "select pg_reload_conf()" >/dev/null || return 1
  pgpass="${PGPASSFILE:-$HOME/.pgpass}"
  touch "$pgpass" && chmod 600 "$pgpass" || return 1
  { grep -vE '^(localhost|127\.0\.0\.1|::1):(5432|\*):jac313:jac313:' "$pgpass"
    printf 'localhost:5432:jac313:jac313:%s\n127.0.0.1:5432:jac313:jac313:%s\n' "$pw" "$pw"; } > "$pgpass.new" \
    && cat "$pgpass.new" > "$pgpass"; rm -f "$pgpass.new"
  if pg_local_ok; then
    echo "PG local: PostgreSQL $(pg_su -c "show server_version") on localhost; role + database jac313; password in $pgpass."
  else
    echo "PG local: set up, but jac313 still cannot log in on localhost — check $hba and $pgpass."
    return 1
  fi
}

# --- 2. find a C++23 g++ (>= 14) and the activation prefix needed to invoke it ---
# (use the helpers defined above so we can re-detect cleanly after sudo provisioning)
detect_cxx
compute_missing

# --- one question, one password: "install" means yes to every step of this run ---
need_install=0
[ -n "$missing" ] && need_install=1
pg_local_ok || need_install=1   # the PG local gate's server
if [ "$WANT_GUI" = 1 ]; then
  upp_gui_libs_ok || need_install=1
  { [ -x "$UPP_DIR/umk" ] && [ -d "$UPP_DIR/uppsrc" ]; } || need_install=1
  dbeaver_ok || need_install=1
fi
if [ "$need_install" = 1 ] && [ "$ASSUME_YES" = 0 ] && [ -t 0 ]; then
  echo
  echo "This bootstrap installs what is missing:"
  [ -n "$missing" ] && echo "  - build tools and libraries:$missing (dnf / apt, with sudo)"
  pg_local_ok || echo "  - a PostgreSQL 18 server on this machine for the PG local gate (sudo; role + database jac313)"
  if [ "$WANT_GUI" = 1 ]; then
    echo "  - the U++ GUI libraries (sudo), U++ itself (no sudo) and the Jac313Runner window"
    echo "  - DBeaver, a database browser (~130 MB download, sudo)"
  fi
  printf "Install everything? [Y/n] "
  read -r ans || ans=n
  case "$ans" in
    [nN]*) echo "OK — each step will ask on its own." ;;
    *) ASSUME_YES=1 ;;
  esac
fi
if [ "$ASSUME_YES" = 1 ] && [ "$need_install" = 1 ]; then
  # The sudo password once, now; then keep it fresh (sudo forgets after ~5 minutes, and the
  # downloads and builds take longer), until this script ends.
  if sudo -v; then
    ( while kill -0 $$ 2>/dev/null; do sudo -n true 2>/dev/null; sleep 50; done ) &
    SUDO_KEEPALIVE=$!
    trap 'kill "$SUDO_KEEPALIVE" 2>/dev/null' EXIT
  fi
fi

# --- 3. install what is missing (recipes in Setup/recipes.conf), then continue ---
# The valgrind check requires the memcheck header AND the helgrind + DRD headers: the
# annotated verify tree (-DJAC313_STORE_HELGRIND_ANNOTATE=ON) #includes <valgrind/helgrind.h>
# and the DRD annotations, which ship in valgrind-devel (Debian bundles them in `valgrind`).
# So baseline can't pass with a partial valgrind that would break the verify/verify-lite gate.
# One failing item never stops the others; what is still missing is named at the end.

if [ -n "$missing" ]; then
  if [ "$ASSUME_YES" != 1 ] && [ ! -t 0 ]; then
    echo "Missing:$missing — run ./bootstrap.sh in a terminal (or ./bootstrap.sh --yes)."
    exit 1
  fi
  for comp in $missing; do
    desc=$(recipe_cmd "$comp" desc || true)
    cmd=$(recipe_cmd "$comp" "$family" || true)
    echo
    echo "=== ${desc:-$comp} ==="
    if [ -z "$cmd" ]; then
      echo "  [FAIL] no recipe for '$family' — install it with your package manager."
      continue
    fi
    if [ "$ASSUME_YES" != 1 ]; then
      printf "Install it? [Y/n] "
      read -r ans || ans=n
      case "$ans" in [nN]*) echo "  skipped"; continue ;; esac
    fi
    if sh -c "$cmd"; then echo "  [ok]"; else echo "  [FAIL] (see above)"; fi
  done
  detect_cxx
  compute_missing
  if [ -n "$missing" ]; then
    echo
    echo "Still missing:$missing — fix the errors above, then re-run ./bootstrap.sh"
    exit 1
  fi
  echo
fi

echo "Bootstrap compiler: ${ACT:+[$ACT] }$CXX"
echo "Ninja: $(ninja --version)   CMake: $(cmake --version | head -n1)"

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

# --- the PostgreSQL server for the PG local gate (install_pg_local). Non-fatal. ---
if ! pg_local_ok; then
  ans=y
  if [ "$ASSUME_YES" != 1 ]; then
    if [ -t 0 ]; then
      printf "Set up a PostgreSQL 18 server on this machine for the PG local gate? [Y/n] "
      read -r ans || ans=n
    else
      ans=n
    fi
  fi
  case "$ans" in
    [nN]*) echo "PG local: skipped (re-run ./bootstrap.sh to set it up)." ;;
    *) install_pg_local || echo "(PG local did not complete — the other gates are ready; fix the above and re-run ./bootstrap.sh)" ;;
  esac
else
  echo "PG local: ready (jac313 logs in on localhost)."
fi

# --- the GUI (unless --no-gui). Non-fatal: the test tools above are ready either way. ---
if [ "$WANT_GUI" = 1 ]; then
  bootstrap_gui || echo "(GUI step did not complete — the test tools are ready; fix the above and re-run ./bootstrap.sh)"
fi

echo
echo "=== checking this machine ==="
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
echo "Bootstrap complete."

# --- the Jac313Runner window: opened at once when there is a screen ---
RUNNER="$(dirname "$ROOT")/gui/out/Jac313Runner"
if [ -x "$RUNNER" ] && [ -n "${WAYLAND_DISPLAY:-}${DISPLAY:-}" ]; then
  # Terminals like Ptyxis stop every process of a tab when it closes (systemd scope), so
  # setsid alone is not enough: systemd-run --user makes the window its own user service
  if command -v systemd-run > /dev/null 2>&1 &&
     systemd-run --user --collect --quiet --unit "jac313-runner-$$" \
       --working-directory "$ROOT" "$RUNNER" --version "$(basename "$ROOT")" 2> /dev/null; then
    :
  else
    ( cd "$ROOT" && setsid "$RUNNER" --version "$(basename "$ROOT")" > /dev/null 2>&1 < /dev/null & )
  fi
  echo "Opened Jac313Runner: choose the gates (start with ctest + smoke) and press Start."
  echo "It keeps running when this terminal is closed."
elif [ -x "$RUNNER" ]; then
  echo "Jac313Runner is built; no screen here to open it on."
fi
