#!/bin/bash
# File:    setup/setup_pg_server.sh
# Purpose: Make THIS machine the shared "PG remote" server for jac313. Run it ON the server, as a
#          user with sudo (Jac313Runner's "Setup Remote PG Server..." runs it over ssh for you, or
#          shows you how to run it by hand). It is safe to run again: every step checks first.
#            - PostgreSQL 18 (Fedora: postgresql-server; RHEL: PGDG postgresql18-server;
#              Mint/Ubuntu: PGDG postgresql-18), started at boot
#            - role + database jac313; a role that already exists keeps its password
#            - logins from ONE network you name (a marked scram-sha-256 line at the top of
#              pg_hba.conf), listen_addresses = '*', and port 5432 open to that network only
#          The plan is shown, and nothing changes until you answer y (or pass --yes).
# Usage:   ./setup_pg_server.sh --allow 192.168.1.0/24 [--password-file F] [--yes] [--dry-run]
#            --allow CIDR         the network the test machines are on (required)
#            --password-file F    the new role's password (the Runner writes it; F is deleted).
#                                 Without it a random password is made and printed once at the end.
#            --dry-run            show the plan only
# NOTE:    A convenience for setting up TEST machines only. It opens PostgreSQL to a whole network
#          with one shared role and password, and the Runner copies that password between machines:
#          you would never do this on a real production server (there: one credential per client,
#          no readable password files, nothing gathered over ssh). The jac313 databases hold
#          throwaway test data of no value; that is the only reason it is OK here.
# Exit:    0 ready, role jac313 created with the new password   3 ready, role jac313 already
#          existed (its password is unchanged)   1 failed or declined   2 usage
# Related: v001/bsir.sh install_pg_local (the same install steps, for PG local on a test machine),
#          v001/schema/fleet_v0_1.sql (made by the first test run that registers, not here).
set -u

ALLOW="" PWFILE="" YES=0 DRY=0
while [ $# -gt 0 ]; do
  case "$1" in
    --allow) ALLOW="${2:-}"; shift 2 ;;
    --password-file) PWFILE="${2:-}"; shift 2 ;;
    --yes) YES=1; shift ;;
    --dry-run) DRY=1; shift ;;
    -h|--help) sed -n '2,25p' "$0"; exit 0 ;;
    *) echo "unknown option: $1 (see --help)"; exit 2 ;;
  esac
done
if ! [[ "$ALLOW" =~ ^([0-9]{1,3}\.){3}[0-9]{1,3}/([0-9]|[12][0-9]|3[0-2])$ ]]; then
  echo "--allow needs an IPv4 network such as 192.168.1.0/24 (got '${ALLOW}')"
  exit 2
fi
if [ "${ALLOW#*/}" -lt 8 ]; then
  echo "--allow $ALLOW is wider than a /8: name the test machines' own network"
  exit 2
fi
NEWPW=""
if [ -n "$PWFILE" ]; then
  NEWPW=$(head -n1 "$PWFILE" 2>/dev/null | tr -d '\r\n')
  rm -f "$PWFILE"
  [ -n "$NEWPW" ] || { echo "the password file is empty or unreadable"; exit 1; }
  [[ "$NEWPW" =~ ^[A-Za-z0-9]+$ ]] || { echo "the password must be letters and digits only"; exit 1; }
fi

# --- which recipe: fedora | rhel | linuxmint (Mint, Ubuntu, Debian: PGDG apt) ---
family=$(. /etc/os-release 2>/dev/null; case " ${ID:-} ${ID_LIKE:-} " in
  *" fedora "*) [ "${ID:-}" = fedora ] && echo fedora || echo rhel ;;
  *" rhel "*|*" centos "*|*" rocky "*|*" almalinux "*) echo rhel ;;
  *" linuxmint "*|*" ubuntu "*|*" debian "*) echo linuxmint ;;
  *) echo unknown ;; esac)
case "$family" in
  fedora) service=postgresql ;;
  rhel) service=postgresql-18 ;;
  linuxmint) service=postgresql ;;
  *) echo "no recipe for this OS ($(. /etc/os-release; echo "${PRETTY_NAME:-?}")): set PostgreSQL up by hand"; exit 1 ;;
esac

pg_psql() {
  local p
  for p in "$(command -v psql 2>/dev/null)" /usr/pgsql-18/bin/psql /usr/lib/postgresql/18/bin/psql; do
    [ -n "$p" ] && [ -x "$p" ] && { echo "$p"; return 0; }
  done
  return 1
}
pg_su() { ( cd / && sudo -u postgres "$(pg_psql)" -v ON_ERROR_STOP=1 -Atq "$@" ); }
installed() {
  case "$family" in
    fedora) rpm -q postgresql-server >/dev/null 2>&1 ;;
    rhel) rpm -q postgresql18-server >/dev/null 2>&1 ;;
    linuxmint) dpkg-query -W -f='${Status}' postgresql-18 2>/dev/null | grep -q 'install ok installed' ;;
  esac
}
running() { systemctl is-active --quiet "$service"; }
MARK="jac313 pg_remote $ALLOW"

# --- the plan: what is there, what would change ---
echo "=== jac313 PG remote server setup on $(hostname) ($family) ==="
if [ "$DRY" = 0 ] && [ "$YES" = 0 ] && ! [ -t 0 ]; then
  echo "This needs a terminal to answer sudo and the y/N question (or pass --yes, or --dry-run)."
  exit 1
fi
if [ "$DRY" = 0 ] || sudo -n true 2>/dev/null; then CANSU=1; else CANSU=0; fi
if [ "$DRY" = 0 ] && ! sudo -n true 2>/dev/null; then
  echo "sudo asks for $(id -un)'s password on $(hostname) now (it reads the server's setup):"
  sudo -v || { echo "sudo is needed"; exit 1; }
fi
plan=()
installed || plan+=("install PostgreSQL 18 ($family recipe)")
running || plan+=("start $service and enable it at boot")
role="?" db="?" hba="" listen="?"
if installed && running && [ "$CANSU" = 1 ]; then
  role=$(pg_su -c "select 1 from pg_roles where rolname = 'jac313'" 2>/dev/null)
  db=$(pg_su -c "select 1 from pg_database where datname = 'jac313'" 2>/dev/null)
  hba=$(pg_su -c "show hba_file" 2>/dev/null)
  listen=$(pg_su -c "show listen_addresses" 2>/dev/null)
fi
case "$role" in
  1) plan+=("keep role jac313 (it exists: its password is NOT changed)") ;;
  "") plan+=("create role jac313 (login, $( [ -n "$NEWPW" ] && echo "the password from the Runner" || echo "a new random password, printed at the end"))") ;;
  *) plan+=("create role jac313 if missing (checked once the server runs)") ;;
esac
[ "$db" = 1 ] || plan+=("create database jac313 owned by jac313 if missing")
if [ -n "$hba" ] && sudo grep -qF "# $MARK" "$hba" 2>/dev/null; then :; else
  plan+=("pg_hba.conf: allow jac313 from $ALLOW (scram-sha-256, first line, marked '$MARK')"); fi
case "$listen" in
  "*"|0.0.0.0*) ;;
  "?") plan+=("listen_addresses = '*' and restart PostgreSQL, if not already '*'") ;;
  *) plan+=("listen_addresses = '*' (now: ${listen:-unset}) and restart PostgreSQL") ;;
esac
fw="none"
if systemctl is-active --quiet firewalld 2>/dev/null; then fw=firewalld
elif command -v ufw >/dev/null && [ "$CANSU" = 1 ] && sudo ufw status 2>/dev/null | grep -q '^Status: active'; then fw=ufw; fi
RULE="rule family=ipv4 source address=$ALLOW port port=5432 protocol=tcp accept"
case "$fw" in
  firewalld) [ "$CANSU" = 1 ] && sudo firewall-cmd --permanent --query-rich-rule="$RULE" >/dev/null 2>&1 \
               || plan+=("firewalld: open 5432/tcp to $ALLOW only (permanent rich rule) and reload") ;;
  ufw) sudo ufw status 2>/dev/null | grep -q "5432/tcp.*ALLOW.*$ALLOW" \
         || plan+=("ufw: allow 5432/tcp from $ALLOW") ;;
  *) echo "(no active firewalld or ufw: nothing to open)" ;;
esac
[ "$CANSU" = 0 ] && echo "(dry run without sudo: the role, database, pg_hba and firewall are checked when it runs)"

if [ ${#plan[@]} -eq 0 ]; then
  echo "Nothing to change: this server is already set up for jac313 from $ALLOW."
else
  echo "Planned changes:"
  for p in "${plan[@]}"; do echo "  - $p"; done
fi
[ "$DRY" = 1 ] && { echo "(dry run: nothing changed)"; exit 0; }
if [ ${#plan[@]} -gt 0 ] && [ "$YES" = 0 ]; then
  printf "Apply these changes? [y/N] "
  read -r ans || ans=""
  case "$ans" in [yY]|[yY][eE][sS]) ;; *) echo "Nothing changed."; exit 1 ;; esac
fi

# --- 1. the server (same recipes as bsir.sh install_pg_local) ---
step() { echo "--> $*"; }
if ! installed; then
  step "installing PostgreSQL 18"
  case "$family" in
    fedora)
      sudo dnf install -y postgresql-server || exit 1
      sudo test -f /var/lib/pgsql/data/PG_VERSION || sudo postgresql-setup --initdb || exit 1 ;;
    rhel)
      rpm -q pgdg-redhat-repo >/dev/null 2>&1 || sudo dnf install -y \
        "https://download.postgresql.org/pub/repos/yum/reporpms/EL-$(rpm -E %rhel)-$(uname -m)/pgdg-redhat-repo-latest.noarch.rpm" || exit 1
      sudo dnf install -y postgresql18-server || exit 1
      sudo test -f /var/lib/pgsql/18/data/PG_VERSION || sudo /usr/pgsql-18/bin/postgresql-18-setup initdb || exit 1 ;;
    linuxmint)
      [ -x /usr/share/postgresql-common/pgdg/apt.postgresql.org.sh ] || sudo apt-get install -y postgresql-common || exit 1
      # Mint's own codename (xia, zara) is unknown to PGDG: give it the Ubuntu one
      sudo /usr/share/postgresql-common/pgdg/apt.postgresql.org.sh -y \
        "$(. /etc/os-release; echo "${UBUNTU_CODENAME:-$VERSION_CODENAME}")" || exit 1
      sudo apt-get install -y postgresql-18 || exit 1 ;;
  esac
fi
step "starting $service (and at boot)"
sudo systemctl enable --now "$service" || exit 1
for _ in $(seq 1 20); do pg_su -c "select 1" >/dev/null 2>&1 && break; sleep 1; done

# --- 2. role + database (an existing role keeps its password: other machines use it) ---
rc=0
if [ "$(pg_su -c "select 1 from pg_roles where rolname = 'jac313'")" = 1 ]; then
  step "role jac313 exists: password unchanged"
  rc=3
else
  [ -n "$NEWPW" ] || NEWPW=$(head -c 24 /dev/urandom | base64 | tr -dc 'A-Za-z0-9' | head -c 24)
  step "creating role jac313"
  pg_su -c "create role jac313 with login password '$NEWPW'" || exit 1
fi
if [ "$(pg_su -c "select 1 from pg_database where datname = 'jac313'")" != 1 ]; then
  step "creating database jac313"
  pg_su -c "create database jac313 owner jac313" || exit 1
fi

# --- 3. logins from $ALLOW: pg_hba (first match wins, so first lines), listen, firewall ---
hba=$(pg_su -c "show hba_file") || exit 1
if ! sudo grep -qF "# $MARK" "$hba"; then
  step "pg_hba.conf: jac313 from $ALLOW"
  sudo sed -i "1i # $MARK: jac313 logins from the test machines (setup_pg_server.sh)\nhost    jac313    jac313    $ALLOW    scram-sha-256" "$hba" || exit 1
fi
restart=0
case "$(pg_su -c "show listen_addresses")" in
  "*"|0.0.0.0*) ;;
  *) step "listen_addresses = '*'"; pg_su -c "alter system set listen_addresses = '*'" || exit 1; restart=1 ;;
esac
case "$fw" in
  firewalld)
    if ! sudo firewall-cmd --permanent --query-rich-rule="$RULE" >/dev/null 2>&1; then
      step "firewalld: 5432/tcp from $ALLOW"
      sudo firewall-cmd --permanent --add-rich-rule="$RULE" >/dev/null && sudo firewall-cmd --reload >/dev/null || exit 1
    fi ;;
  ufw)
    sudo ufw status | grep -q "5432/tcp.*ALLOW.*$ALLOW" || { step "ufw: 5432/tcp from $ALLOW"; sudo ufw allow from "$ALLOW" to any port 5432 proto tcp || exit 1; } ;;
esac
if [ "$restart" = 1 ]; then
  step "restarting $service"
  sudo systemctl restart "$service" || exit 1
  for _ in $(seq 1 20); do pg_su -c "select 1" >/dev/null 2>&1 && break; sleep 1; done
else
  pg_su -c "select pg_reload_conf()" >/dev/null || exit 1
fi

echo
echo "=== ready: PostgreSQL $(pg_su -c "show server_version") on $(hostname), database jac313, logins from $ALLOW ==="
if [ "$rc" = 3 ]; then
  echo "Role jac313 already existed, so its password is unchanged: use that password on the test machines."
elif [ -z "$PWFILE" ]; then
  echo "The new password for role jac313 (shown once; paste it into the Runner's Setup dialog):"
  echo "  $NEWPW"
fi
exit "$rc"
