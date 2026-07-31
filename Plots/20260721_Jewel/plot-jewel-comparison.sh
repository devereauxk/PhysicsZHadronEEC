#!/bin/bash
set -e

THISDIR=$(cd "$(dirname "$0")" && pwd)
cd "$THISDIR"

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialProductDictionary.sh

make ExecuteJewelComparison

BASEDIR="${OFFICIAL_RESULT_DIR}"
mkdir -p plots

./ExecuteJewelComparison \
    --files "${BASEDIR}/jewelPP8160signed_ZPT0_500","${BASEDIR}/jewelPP8160signed_v9ref_ZPT0_500","${BASEDIR}/jewelPP8160signed_v2_ZPT0_500","${BASEDIR}/jewelPP8160signed_v3_ZPT0_500" \
    --labels "official (2M)","v9 ref (2M)","v2 (500k)","v3 (500k)" \
    --refLabel "official" \
    --output plots/jewel_comparison_deltaeta.pdf --doEta true

./ExecuteJewelComparison \
    --files "${BASEDIR}/jewelPP8160signed_ZPT0_500","${BASEDIR}/jewelPP8160signed_v9ref_ZPT0_500","${BASEDIR}/jewelPP8160signed_v2_ZPT0_500","${BASEDIR}/jewelPP8160signed_v3_ZPT0_500" \
    --labels "official (2M)","v9 ref (2M)","v2 (500k)","v3 (500k)" \
    --refLabel "official" \
    --output plots/jewel_comparison_deltaphi.pdf --doEta false
