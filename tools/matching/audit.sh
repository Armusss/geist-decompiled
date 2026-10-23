#!/bin/sh
# audit.sh [UNIT...]: the merge audit, run from the repo root. Exit 0 only if the tree has no conflict markers,
# the full build succeeds, the built main.dol's SHA-1 equals config/GITE01/build.sha1 (the original DOL), each
# named unit (e.g. SpiritGun, u_800DE0C4) is linked from its own compiled source object, and no game source uses
# a forbidden fake-match pattern (tools/matching/scan_fakes.sh).
if grep -rlE '^(<<<<<<<|>>>>>>>)' config/GITE01/*.txt configure.py src 2>/dev/null | head -1 | grep -q .; then
  echo "AUDIT FAIL: conflict markers"; exit 1; fi
python3 configure.py >/dev/null || { echo "AUDIT FAIL: configure"; exit 1; }
rm -f build/GITE01/main.dol
LOG=build/audit-ninja.log
ninja >"$LOG" 2>&1 || { echo "AUDIT FAIL: ninja failed"; grep -E "Caused by|FAILED|rror" -A2 "$LOG" | head -8; exit 1; }
want=$(cut -c1-40 config/GITE01/build.sha1); got=$(sha1sum build/GITE01/main.dol 2>/dev/null | cut -c1-40)
[ "$want" = "$got" ] || { echo "AUDIT FAIL: main.dol $got != $want"; tail -5 "$LOG"; exit 1; }
[ $# -eq 0 ] || python3 tools/matching/linked.py "$@" || { echo "AUDIT FAIL: unit(s) above not linked from src"; exit 1; }
sh tools/matching/scan_fakes.sh src/game || { echo "AUDIT FAIL: forbidden patterns (above)"; exit 1; }
echo "AUDIT OK: main.dol $got"
