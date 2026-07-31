#!/bin/bash

set -e

THISDIR=$(cd "$(dirname "$0")" && pwd)
cd "$THISDIR"

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialProductDictionary.sh

make ExecuteComposite

PP_TAG="${OFFICIAL_TAG_PP}_scan"
PP_MC_TAG="${OFFICIAL_TAG_PP}_scan"
PPB_TAG="${OFFICIAL_TAG_PPB}_scan"
PPB_MC_TAG="${OFFICIAL_TAG_PPB}_scan"
PPB_SYST_TAG="${OFFICIAL_TAG_PPB}_scan"
PP_SYST_TAG="${OFFICIAL_TAG_PP}_scan"
INCLUDE_MC="${INCLUDE_MC:-false}"

for OBS in DeltaPhi DeltaEta; do
    OBS_LC=$(echo "$OBS" | tr '[:upper:]' '[:lower:]')
    DO_ETA="false"
    if [ "$OBS" = "DeltaEta" ]; then DO_ETA="true"; fi
    echo "=== Composite $OBS ==="
    ./ExecuteComposite \
        --zPtRange 0_500 \
        --pPbtag "$PPB_TAG" \
        --pPbMCTag "$PPB_MC_TAG" \
        --pPbSystematicsTag "$PPB_SYST_TAG" \
        --pptag "$PP_TAG" \
        --ppMCTag "$PP_MC_TAG" \
        --ppSystematicsTag "$PP_SYST_TAG" \
        --includeMC "$INCLUDE_MC" \
        --UseSystematics true \
        --doEta "$DO_ETA" \
        --output "plots/composite/composite_${OBS_LC}.pdf" \
        --BaseDir "${OFFICIAL_RESULT_DIR}"
done
