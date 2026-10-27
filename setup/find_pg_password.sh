#!/bin/bash
# File:    setup/find_pg_password.sh
# Purpose: Run ON the PG remote server (Jac313Runner's Setup dialog, "Fetch password", runs it over
#          ssh): print every jac313 password stored on this machine, one per line, so the Runner can
#          try them. PostgreSQL keeps only a hash of a password, so a stored copy is the only way
#          back to it. Looks, with sudo (the files are their owners' alone, mode 600), in every
#          SQL_Secrets/*.pgpass under /home, /root and the mounted drives (the GFS drive's included),
#          and in the .pgpass files of the users and of postgres: lines whose user is jac313 (or *).
#          The passwords go to stdout only (the Runner sends it to a private file); counts to stderr.
# Usage:   bash find_pg_password.sh > found
# NOTE:    A convenience for setting up TEST machines only. It reads stored passwords with sudo and
#          copies them between machines, which you would never do on a real production server (there:
#          one credential per client, no readable password files, nothing gathered over ssh). The
#          jac313 databases hold throwaway test data of no value; that is the only reason it is OK here.
set -u
echo "sudo asks for $(id -un)'s password on $(hostname) now (to read the stored jac313 passwords):" >&2
sudo -v || exit 1
sudo bash -c '
  {
    find /home /root /mnt /media /run/media -maxdepth 5 -type d -name SQL_Secrets 2>/dev/null |
      while read -r d; do ls "$d"/*.pgpass 2>/dev/null; done
    ls /home/*/.pgpass /root/.pgpass /var/lib/pgsql/.pgpass /var/lib/postgresql/.pgpass 2>/dev/null
  } | sort -u > /tmp/.jac313-pgfiles.$$
  n=$(grep -c . /tmp/.jac313-pgfiles.$$)
  # host:port:db:user:password; the password may hold \: and \\ (pgpass escapes)
  while read -r f; do
    awk -F: '"'"'!/^[[:space:]]*#/ && ($4 == "jac313" || $4 == "*") { sub(/^([^:]*:){4}/, ""); print }'"'"' "$f"
  done < /tmp/.jac313-pgfiles.$$ | sed '"'"'s/\\\(.\)/\1/g'"'"' | awk '"'"'NF && !seen[$0]++'"'"'
  rm -f /tmp/.jac313-pgfiles.$$
  echo "searched $n password file(s)" >&2
'
