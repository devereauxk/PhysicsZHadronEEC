#!/bin/bash

set -euo pipefail

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialProductDictionary.sh
make ExecuteZSpectrumPlot

./ExecuteZSpectrumPlot --zPtRange 5_500 --trkPtRange 0.5_500 --pPbtag "$OFFICIAL_TAG_PPB" --pptag "$OFFICIAL_TAG_PP" --BaseDir "${OFFICIAL_RESULT_DIR}"
./ExecuteZSpectrumPlot --zPtRange 5_500 --trkPtRange 0.5_500 --pPbtag "$OFFICIAL_TAG_PPB" --pptag "$OFFICIAL_TAG_PP" --doCombine true --BaseDir "${OFFICIAL_RESULT_DIR}"

exit
