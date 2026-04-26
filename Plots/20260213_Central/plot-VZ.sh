#!/bin/bash

set -euo pipefail

make ExecuteVZPlot

for system in pPb PbP
do
    ./ExecuteVZPlot --collisionType "${system}" --zPtRange 0_500 --trkPtRange 1_10 --tag noVZWeight_nmix0
    ./ExecuteVZPlot --collisionType "${system}" --zPtRange 0_500 --trkPtRange 1_10 --tag VZWeight_nmix0
done

./ExecuteVZPlot --collisionType pp --zPtRange 0_350 --trkPtRange 1_10 --tag noEvtWeight_VZOnly_nmix1
./ExecuteVZPlot --collisionType pp --zPtRange 0_350 --trkPtRange 1_10 --tag myEvtWeight_VZOnly_nmix1

exit
