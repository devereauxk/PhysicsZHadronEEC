#!/bin/bash

set -euo pipefail

#source ./clean.sh
make ExecuteCentralOverlayPPbPbPPlot

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
PPB_TAG="${PPB_TAG:-$OFFICIAL_TAG_PPB}"

./ExecuteCentralOverlayPPbPbPPlot --zPtRange 5_30 --trkPtRange 0.5_4 --pPbtag "$PPB_TAG"
./ExecuteCentralOverlayPPbPbPPlot --zPtRange 5_30 --trkPtRange 4_500 --pPbtag "$PPB_TAG"
./ExecuteCentralOverlayPPbPbPPlot --zPtRange 30_500 --trkPtRange 0.5_4 --pPbtag "$PPB_TAG"
./ExecuteCentralOverlayPPbPbPPlot --zPtRange 30_500 --trkPtRange 4_500 --pPbtag "$PPB_TAG"
./ExecuteCentralOverlayPPbPbPPlot --zPtRange 5_500 --trkPtRange 0.5_500 --pPbtag "$PPB_TAG"

exit
