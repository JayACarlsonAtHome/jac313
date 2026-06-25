#!/usr/bin/env bash
#
# The curated Store benchmark suite (Release-only). NOT a pass/fail ctest — it produces
# a throughput distribution per config (median + low–high band over N runs), wrapped in a
# hardware + version header (the numbers are meaningless without "on what box, what code").
#
#   1. flag-overhead sweep  — non-durable, flags = 0/2/4/6 (cost of setting flag bits)
#   2. durable backends     — jText / SQL / binary (honest: flush inside the timed region)
#
# Sizes are env-overridable for a quick smoke vs a real run:
#   ./bench_suite.sh                          # full: 10M×10 non-durable, 1M×3 durable
#   NDEV=20000 NRUN=3 DDEV=20000 DRUN=3 ./bench_suite.sh   # quick smoke
#   BENCH=/path/to/jac313_store_bench THREADS=12 ./bench_suite.sh
set -eu
ROOT="$(cd "$(dirname "$0")" && pwd)"
V2="$(cd "$ROOT/../../.." && pwd)"                       # the v002 world root
BIN="${BENCH:-$(find "$V2" -name jac313_store_bench -type f 2>/dev/null | head -1)}"
[ -x "$BIN" ] || { echo "store_bench not built — build the jac313_store_bench target first"; exit 1; }
THREADS=${THREADS:-50}
NDEV=${NDEV:-200000}; NRUN=${NRUN:-10}                   # non-durable: 200k×50thr = 10M, ×10 runs
DDEV=${DDEV:-20000};  DRUN=${DRUN:-3}                    # durable:      20k×50thr =  1M, ×3 runs
# friendlier: set the TOTAL event count directly and we compute per-thread for you
[ -n "${NEVENTS:-}" ] && NDEV=$((NEVENTS / THREADS))     # e.g. NEVENTS=5000000  -> 5M non-durable
[ -n "${DEVENTS:-}" ] && DDEV=$((DEVENTS / THREADS))     # e.g. DEVENTS=1000000  -> 1M durable
DRY=""; for a in "$@"; do [ "$a" = "--dry-run" ] && DRY=1; done   # list configs, don't run

val() { printf '%s\n' "$1" | grep -o "BENCHSTATS.*" | grep -o "$2=[0-9]*" | cut -d= -f2; }
commafy() { printf "%'d" "$1" 2>/dev/null || echo "$1"; }
mil() { awk "BEGIN{printf \"%.2fM\", $1/1000000}"; }
ver_of() { local f; f=$(ls $1 2>/dev/null | head -1); grep -oE 'return "v[0-9]+\.[0-9]+"' "$f" 2>/dev/null | grep -oE 'v[0-9]+\.[0-9]+' | head -1; }

print_header() {
  local cpu cores ram os disk
  cpu=$(grep -m1 'model name' /proc/cpuinfo 2>/dev/null | sed 's/.*: *//')
  cores=$(grep -c '^processor' /proc/cpuinfo 2>/dev/null)
  ram=$(awk '/MemTotal/{printf "%.0f", $2/1024/1024}' /proc/meminfo 2>/dev/null)
  os=$(. /etc/os-release 2>/dev/null; printf '%s' "${PRETTY_NAME:-$(uname -sr)}")
  disk=$(lsblk -ndo ROTA 2>/dev/null | awk 'NR==1{print ($1==0)?"ssd":"hdd"}')
  echo "## Store benchmark — $(uname -n)"
  echo
  echo "| | |"
  echo "|---|---|"
  echo "| **Hardware** | ${cpu:-?} · ${cores:-?} cores · ${ram:-?} GB · ${disk:-?} |"
  echo "| **OS / build** | ${os:-?} · Release · threads=$THREADS |"
  echo "| **Versions** | Store $(ver_of "$V2/Store/include/jac313/Store/v00*/headers/ts_store_config.hpp") · Qlite $(ver_of "$V2/Qlite/include/jac313/Qlite/v00*/Sqlite.body.hpp") · jText $(ver_of "$V2/jText/jText.api.inc") |"
  echo "| **Run (UTC)** | $(date -u +%Y-%m-%dT%H:%M:%SZ) |"
  echo
}

if [ -n "$DRY" ]; then
  # Emit copy-pasteable command strings (for README docs). The trailing "# ..." are
  # shell comments, so each whole line still runs verbatim if pasted into a terminal.
  echo "# Store benchmark suite — 7 configs"
  echo "# non-durable: $(($NDEV*$THREADS/1000000))M events x $NRUN runs   |   durable: $(($DDEV*$THREADS/1000000))M events x $DRUN runs"
  echo
  echo "cd $(cd "$(dirname "$BIN")" && pwd)"   # absolute path, so paste runs from anywhere
  _cmd() {   # $1=events/thread $2=runs $3=persist $4=flag-count(empty=durable) $5=comment
    local c
    if [ -n "$4" ]; then
      c=$(printf './jac313_store_bench --threads %2s --events-per-thread %7s --runs %2s --persist %-6s --flag-count %s' "$THREADS" "$1" "$2" "$3" "$4")
    else
      c=$(printf './jac313_store_bench --threads %2s --events-per-thread %7s --runs %2s --persist %-6s' "$THREADS" "$1" "$2" "$3")
    fi
    printf '%-104s # %s\n' "$c" "$5"
  }
  for n in 0 2 4 6; do _cmd "$NDEV" "$NRUN" none "$n" "$n flags, non-durable"; done
  for p in jtext sql binary; do _cmd "$DDEV" "$DRUN" "$p" "" "durable $p"; done
  exit 0
fi

print_header

echo "### Flag-overhead sweep — non-durable ($(($NDEV*$THREADS/1000000))M events × $NRUN runs)"
echo
printf "| %-8s | %16s | %-16s | %-10s |\n" "flags" "median ops/sec" "spread (lo–hi)" "vs 0 flags"
printf "|%s|%s|%s|%s|\n" "----------" "------------------" "------------------" "------------"
base=0
for n in 0 2 4 6; do
  out=$("$BIN" --threads "$THREADS" --events-per-thread "$NDEV" --runs "$NRUN" --persist none --flag-count "$n" 2>&1)
  med=$(val "$out" median); lo=$(val "$out" low); hi=$(val "$out" high)
  [ "$n" = 0 ] && base=$med
  delta=$(awk "BEGIN{if($base>0)printf \"%+.1f%%\", 100*($med-$base)/$base; else printf \"--\"}")
  printf "| %-8s | %16s | %-16s | %-10s |\n" "$n" "$(commafy "$med")" "$(mil "$lo")–$(mil "$hi")" "$delta"
done

echo
echo "### Durable backends — $(($DDEV*$THREADS/1000000))M events × $DRUN runs (flush inside the clock)"
echo
printf "| %-8s | %16s | %-16s |\n" "backend" "median ops/sec" "spread (lo–hi)"
printf "|%s|%s|%s|\n" "----------" "------------------" "------------------"
for p in jtext sql binary; do
  out=$("$BIN" --threads "$THREADS" --events-per-thread "$DDEV" --runs "$DRUN" --persist "$p" 2>&1)
  med=$(val "$out" median); lo=$(val "$out" low); hi=$(val "$out" high)
  printf "| %-8s | %16s | %-16s |\n" "$p" "$(commafy "$med")" "$(mil "$lo")–$(mil "$hi")"
done
echo
