#!/bin/bash
# File:    Setup/pgdg_repo.sh
# Purpose: The PostgreSQL (PGDG) dnf repository on the RHEL family, in a state dnf can use.
#          pgdg-redhat-repo writes its URLs per minor release (rhel-9.9-x86_64), and PGDG publishes
#          a new minor's tree some time after Red Hat ships it. Until then every dnf command fails
#          on the 404, the system update and unrelated installs too. So: when this machine's
#          versioned tree is missing upstream (a 404, not a network error), point the repo file at
#          the major release's tree (rhel-9-x86_64), which PGDG always publishes; the original is
#          kept beside it as .orig. A later pgdg-redhat-repo update brings the versioned URLs back,
#          and the next run checks again.
# Usage:   Setup/pgdg_repo.sh [--install]
#            --install   install pgdg-redhat-repo first when it is missing
#          Without it, a machine with no PGDG repo is left alone. Uses sudo for changes.
# Exit:    0 the repo is usable (or absent, without --install)   1 install or edit failed
# Related: bsir.sh (before the system update; install_pg_local), Setup/recipes.jtext (pg18, rhel),
#          Setup/setup_pg_server.sh (a copy of pgdg_fix_release: the Runner copies that script to
#          the server alone; keep the two the same).
set -u

# pgdg_fix_release: the check and the edit (copied in Setup/setup_pg_server.sh)
pgdg_fix_release() {
  local f=/etc/yum.repos.d/pgdg-redhat-all.repo major ver code
  [ -f "$f" ] || return 0
  # already on the major release's tree (the file's comments name $releasever_minor too)
  grep -q '^[[:space:]]*baseurl=.*\$releasever_minor' "$f" || return 0
  major=$(rpm -E %rhel)
  # the release dnf puts in the URL (a "subscription-manager release --set" lock moves it),
  # else the installed one
  ver=$(python3 -c 'import dnf; s = dnf.Base().conf.substitutions
print(s["releasever_major"] + "." + s["releasever_minor"])' 2>/dev/null) ||
    ver=$(. /etc/os-release 2>/dev/null; echo "${VERSION_ID:-}")
  case "$ver" in
    "$major".[0-9]*)
      code=$(curl -s -o /dev/null -w '%{http_code}' --max-time 20 \
        "https://download.postgresql.org/pub/repos/yum/common/redhat/rhel-$ver-$(uname -m)/repodata/repomd.xml")
      case "$code" in
        200) return 0 ;;
        404) echo "PGDG has no rhel-$ver tree yet: using its rhel-$major tree ($f)" ;;
        *) echo "PGDG: could not check rhel-$ver (HTTP '$code'); $f left as it is"; return 0 ;;
      esac ;;
    *) echo "PGDG: no minor release in VERSION_ID '$ver': using the rhel-$major tree ($f)" ;;
  esac
  sudo cp -p "$f" "$f.orig" &&
    sudo sed -i 's/\$releasever_major\.\$releasever_minor/$releasever_major/g' "$f"
}

if [ "${1:-}" = --install ] && ! rpm -q pgdg-redhat-repo >/dev/null 2>&1; then
  sudo dnf install -y \
    "https://download.postgresql.org/pub/repos/yum/reporpms/EL-$(rpm -E %rhel)-$(uname -m)/pgdg-redhat-repo-latest.noarch.rpm" ||
    exit 1
fi
pgdg_fix_release || exit 1
