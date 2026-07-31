#!/bin/bash
set -e

THISDIR=$(cd "$(dirname "$0")" && pwd)
cd "$THISDIR"

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialProductDictionary.sh

make ExecuteJewelZPt

BASEDIR="${OFFICIAL_RESULT_DIR}"
mkdir -p plots

./ExecuteJewelZPt \
    --jewelFile "${BASEDIR}/jewelPP8160signed_v3_ZPT0_500-nosub.root" \
    --dataFile "${OFFICIAL_PP_RESULT_PREFIX}_ZPT0_500-nosub.root" \
    --jewelLabel "JEWEL v3 pp 8.16 TeV" \
    --dataLabel "pp data 8.16 TeV" \
    --output plots/jewel_v3_zpt_comparison.pdf
