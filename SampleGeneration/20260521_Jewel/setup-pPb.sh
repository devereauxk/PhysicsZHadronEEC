#!/bin/bash
set -euo pipefail

SAMPLE=/home/kdeverea/Jewel/hydro/pPb/sample
WORK_DIR=$(cd "$(dirname "$0")" && pwd)
PARAMS=$WORK_DIR/params/pPb

echo "Setting up pPb params in ${PARAMS} for sample in ${SAMPLE}"

rm -rf "${PARAMS}"
mkdir -p "${PARAMS}"

shopt -s dotglob

for DIR in "$SAMPLE"/*/; do
    NCOLL=$(basename "$DIR")

cat > "${PARAMS}/medium.2D-pPb-${NCOLL}.dat" <<EOF
CENTRMIN 0.
CENTRMAX 100.
A 208
HYDRODIR ${SAMPLE}/${NCOLL}
EOF

cat > "${PARAMS}/params.2D-pPb-${NCOLL}.dat" <<EOF
NEVENT 5000
LOGFILE ${WORK_DIR}/logs/pPb/2D-pPb-${NCOLL}.log
HEPMCFILE ${WORK_DIR}/eventfiles/pPb/2D-pPb-${NCOLL}.hepmc
PTMIN 15
PTMAX 1200.
ETAMAX 3.
PROCESS PPZJ
NPROTON 1
SQRTS 8160.
WEXPO 4.5
HADRO T
MASS 1.
KEEPRECOILS T
WRITESCATCEN T
WRITEDUMMIES T
MEDIUMPARAMS ${PARAMS}/medium.2D-pPb-${NCOLL}.dat
EOF

done

shopt -u dotglob

echo "Created $(ls "${PARAMS}"/params.*.dat | wc -l) param files"
