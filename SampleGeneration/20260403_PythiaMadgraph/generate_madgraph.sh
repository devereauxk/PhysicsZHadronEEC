#!/bin/bash
set -euo pipefail

ScriptDir=$(cd "$(dirname "$0")" && pwd)
source "$ScriptDir/env.sh"

Energy=""
Events=2000
Seed=12345
ProcessDir=""
OutputLHE=""
ForceProcess=0
LHAID=303600

while [[ $# -gt 0 ]]; do
   case "$1" in
      --Energy) Energy="$2"; shift 2 ;;
      --Events) Events="$2"; shift 2 ;;
      --Seed) Seed="$2"; shift 2 ;;
      --ProcessDir) ProcessDir="$2"; shift 2 ;;
      --OutputLHE) OutputLHE="$2"; shift 2 ;;
      --ForceProcess) ForceProcess="$2"; shift 2 ;;
      *) echo "Unknown option: $1"; exit 1 ;;
   esac
done

if [[ -z "$Energy" ]]; then
   echo "Usage: $0 --Energy {5020|8160} [--Events N] [--Seed S] [--OutputLHE file]"
   exit 1
fi

case "$Energy" in
   5020) EBeam=2510 ;;
   8160) EBeam=4080 ;;
   *) echo "Unsupported energy: $Energy"; exit 1 ;;
esac

ProcessDir=${ProcessDir:-${PYTHIA_LOCAL_BASE}/processes/ZMuMu_${Energy}}
OutputLHE=${OutputLHE:-${ScriptDir}/output/ZMuMu_${Energy}.lhe}
ProcessCard="$ScriptDir/work/process_${Energy}.mg5"
LaunchCard="$ScriptDir/work/launch_${Energy}.mg5"

mkdir -p "$ScriptDir/output" "$ScriptDir/work"

NeedProcess=0
if [[ ! -d "$ProcessDir" || "$ForceProcess" == "1" ]]; then
   NeedProcess=1
elif [[ ! -f "$ProcessDir/Cards/proc_card_mg5.dat" ]]; then
   NeedProcess=1
elif ! grep -Fq 'import model loop_sm' "$ProcessDir/Cards/proc_card_mg5.dat"; then
   NeedProcess=1
elif ! grep -Fq 'generate p p > z [QCD]' "$ProcessDir/Cards/proc_card_mg5.dat"; then
   NeedProcess=1
fi

if [[ "$NeedProcess" == "1" ]]; then
   cat > "$ProcessCard" <<MG5
set automatic_html_opening False --no_save
set lhapdf ${LHAPDF6_BASE}/bin/lhapdf-config --no_save
import model loop_sm
generate p p > z [QCD]
output ${ProcessDir} -f
quit
MG5
   "$MG5AMC_PYTHON" "$MG5AMC_BASE/bin/mg5_aMC" "$ProcessCard"
fi

cat > "$ProcessDir/Cards/madspin_card.dat" <<'MADSPIN'
decay z > mu+ mu-
launch
MADSPIN

cat > "$LaunchCard" <<MG5
set automatic_html_opening False --no_save
set lhapdf ${LHAPDF6_BASE}/bin/lhapdf-config --no_save
launch ${ProcessDir}
madspin=ON
shower=OFF
set nevents ${Events}
set iseed ${Seed}
set ebeam1 ${EBeam}
set ebeam2 ${EBeam}
set pdlabel lhapdf
set lhaid ${LHAID}
set parton_shower PYTHIA8
set event_norm average
set reweight_scale False
set reweight_PDF False
set store_rwgt_info False
done
quit
MG5

"$MG5AMC_PYTHON" "$MG5AMC_BASE/bin/mg5_aMC" "$LaunchCard"

mapfile -t LatestPaths < <("$MG5AMC_PYTHON" - <<'PY' "$ProcessDir"
from pathlib import Path
import sys

base = Path(sys.argv[1]) / "Events"
runs = sorted(
    p for p in base.iterdir()
    if p.is_dir() and p.name.startswith("run_") and "_decayed_" not in p.name
)
if not runs:
    raise SystemExit(1)
latest = runs[-1]
decayed = sorted(
    p for p in base.iterdir()
    if p.is_dir() and p.name.startswith(latest.name + "_decayed_")
)
if not decayed:
    raise SystemExit(1)
print(latest)
print(decayed[-1])
PY
)
LatestRun=${LatestPaths[0]}
LatestDecayedRun=${LatestPaths[1]}
gzip -dc "$LatestDecayedRun/events.lhe.gz" > "$OutputLHE"

echo "$OutputLHE"
