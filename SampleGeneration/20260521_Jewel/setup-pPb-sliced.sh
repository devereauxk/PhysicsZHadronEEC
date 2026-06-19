#!/bin/bash
set -euo pipefail

# Generates params for pT-hat sliced pPb production.
# Three slices: lo=[15,50], mi=[50,200], hi=[200,1200].
# Reuses the medium files already written to ~/Jewel/jewel-2.4.0/params/pPb/
# by setup-pPb.sh (those must exist before running this script).

SAMPLE=/home/kdeverea/Jewel/hydro/pPb/sample
JEWEL_PARAMS=/home/kdeverea/Jewel/jewel-2.4.0/params/pPb
WORK_DIR=$(cd "$(dirname "$0")" && pwd)

echo "Setting up pPb sliced params (lo/mi/hi subdirs)"
for SLICE in lo mi hi; do
    rm -rf "${WORK_DIR}/params/${SLICE}"
    mkdir -p "${WORK_DIR}/params/${SLICE}"
    mkdir -p "${WORK_DIR}/logs/${SLICE}"
    mkdir -p "${WORK_DIR}/eventfiles/${SLICE}"
done

shopt -s dotglob

declare -A SLICE_PTMIN=([lo]=15  [mi]=50  [hi]=200)
declare -A SLICE_PTMAX=([lo]=50  [mi]=200 [hi]=1200)

for SLICE in lo mi hi; do
    PTMIN=${SLICE_PTMIN[$SLICE]}
    PTMAX=${SLICE_PTMAX[$SLICE]}

    for DIR in "$SAMPLE"/*/; do
        NCOLL=$(basename "$DIR")

        MEDFILE="${JEWEL_PARAMS}/medium.2D-pPb-${NCOLL}.dat"
        if [ ! -f "$MEDFILE" ]; then
            echo "ERROR: medium file missing for Ncoll=${NCOLL} — run setup-pPb.sh first"
            exit 1
        fi

        # Keep all paths short: params/${SLICE}/, logs/${SLICE}/, eventfiles/${SLICE}/
        # to avoid exceeding the Fortran CHARACTER buffer (~97 chars) in jewel-2.4.0-2D.
        cat > "${WORK_DIR}/params/${SLICE}/params.2D-pPb-${NCOLL}.dat" <<EOF
NEVENT 5000
LOGFILE ${WORK_DIR}/logs/${SLICE}/2D-pPb-${NCOLL}.log
HEPMCFILE ${WORK_DIR}/eventfiles/${SLICE}/2D-pPb-${NCOLL}.hepmc
PTMIN ${PTMIN}
PTMAX ${PTMAX}.
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
MEDIUMPARAMS ${MEDFILE}
EOF
    done
done

shopt -u dotglob

total=$(ls "${WORK_DIR}/params/lo/"*.dat "${WORK_DIR}/params/mi/"*.dat "${WORK_DIR}/params/hi/"*.dat | wc -l)
echo "Created ${total} param files (3 slices × 100 Ncoll bins)"
