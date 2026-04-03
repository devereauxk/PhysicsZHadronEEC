#!/bin/bash

set -euo pipefail

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
make ExecuteZSpectrumPlot

./ExecuteZSpectrumPlot --zPtRange 5_500 --trkPtRange 0.5_500 --pPbtag "$OFFICIAL_TAG_PPB" --pptag "$OFFICIAL_TAG_PP"
./ExecuteZSpectrumPlot --zPtRange 5_500 --trkPtRange 0.5_500 --pPbtag "$OFFICIAL_TAG_PPB" --pptag "$OFFICIAL_TAG_PP" --doCombine true

exit
