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

mkdir -p "$ScriptDir/output" "$ScriptDir/work"

if [[ ! -d "$ProcessDir" || "$ForceProcess" == "1" ]]; then
   cat > "$ScriptDir/work/process_${Energy}.mg5" <<MG5
import model sm
generate p p > z, z > mu+ mu-
output ${ProcessDir} -f
MG5
   "$MG5AMC_PYTHON" "$MG5AMC_BASE/bin/mg5_aMC" "$ScriptDir/work/process_${Energy}.mg5"
fi

PROCESS_DIR="$ProcessDir" EBEAM="$EBeam" EVENTS="$Events" SEED="$Seed" \
LHAPDF6_BASE="$LHAPDF6_BASE" \
"$MG5AMC_PYTHON" - <<'PY'
import os
import re
from pathlib import Path

run_card = Path(os.environ["PROCESS_DIR"]) / "Cards" / "run_card.dat"
run_replacements = {
    "nevents": os.environ["EVENTS"],
    "iseed": os.environ["SEED"],
    "ebeam1": os.environ["EBEAM"],
    "ebeam2": os.environ["EBEAM"],
    "mmll": "60.0",
    "cut_decays": "False",
    "use_syst": "False",
}
config_card = Path(os.environ["PROCESS_DIR"]) / "Cards" / "me5_configuration.txt"
config_replacements = {
    "automatic_html_opening": "False",
    "eps_viewer": "None",
    "web_browser": "None",
    "lhapdf": f"{os.environ['LHAPDF6_BASE']}/bin/lhapdf-config",
}
pattern = re.compile(r'^(\s*)([^=!#]+?)(\s*=\s*)([A-Za-z0-9_]+)(.*)$')

for path, replacements in ((run_card, run_replacements), (config_card, config_replacements)):
    lines = []
    for line in path.read_text().splitlines():
        match = pattern.match(line)
        if match and match.group(4) in replacements:
            name = match.group(4)
            line = f"{match.group(1)}{replacements[name]}{match.group(3)}{name}{match.group(5)}"
        lines.append(line)
    path.write_text("\n".join(lines) + "\n")
PY

(
   cd "$ProcessDir"
   "$MG5AMC_PYTHON" ./bin/generate_events -f
)

LatestRun=$(ls -dt "$ProcessDir"/Events/run_* | head -n 1)
gzip -dc "$LatestRun/unweighted_events.lhe.gz" > "$OutputLHE"

echo "$OutputLHE"
