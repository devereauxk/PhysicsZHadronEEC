#!/bin/bash
set -euo pipefail

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
cd "$(dirname "$0")"

ITERATIONS=${ITERATIONS:-3}

PPB_RAW="output/pPb_raw.root"
PBP_RAW="output/PbP_raw.root"

echo "=== Deriving Z correction (pPb→PbP), ${ITERATIONS} iterations ==="

# Iteration 1: initial correction from raw
root -l -q -b "z_correction.C(\"${PPB_RAW}\",\"${PBP_RAW}\",\"output/z_correction_iter1.root\")"
MERGED="output/z_correction_iter1.root"

for (( iter=2; iter<=ITERATIONS; iter++ )); do
    echo "--- Iteration ${iter} ---"
    NTHREAD=${NTHREAD:-25} ./runner_zcorr.sh "${MERGED}" "output/pPb_zcorr_iter${iter}.root"

    root -l -q -b "z_correction.C(\"output/pPb_zcorr_iter${iter}.root\",\"${PBP_RAW}\",\"output/z_correction_iter${iter}.root\")"

    PREV="${MERGED}"
    MERGED="output/z_correction_merged_iter${iter}.root"
    root -l -q -b "merge_z_corrections.C(\"${PREV}\",\"output/z_correction_iter${iter}.root\",\"${MERGED}\")"
done

echo "Final Z correction: ${MERGED}"
cp "${MERGED}" "output/z_correction_final.root"
echo "=== Done ==="
