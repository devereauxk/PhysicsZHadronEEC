#!/bin/bash
set -euo pipefail

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialProductDictionary.sh

ANALYSISDIR="${OFFICIAL_RESULT_DIR}"
TAG="${OFFICIAL_TAG_PPB}"
TRACK="0.5_15"

PPB_NOSUB="${ANALYSISDIR}/pPb_trkResidual_${TAG}_ZPT0_500-nosub.root"
PBP_NOSUB="${ANALYSISDIR}/PbP_trkResidual_${TAG}_ZPT0_500-nosub.root"

mkdir -p output

./Execute \
    --PPb "$PPB_NOSUB" \
    --PBP "$PBP_NOSUB" \
    --TrackRange "$TRACK" \
    --Output "output/20260607_MEWeight_${TAG}.root"

echo "ME weight file: output/20260607_MEWeight_${TAG}.root"
