#!/bin/bash
set -euo pipefail
cd "$(dirname "$0")"
source /home/kdeverea/PhysicsZHadronEEC/SetupAnalysis.sh
make ExecutePlotComparison
./ExecutePlotComparison \
    --InputDir output/mc \
    --PlotDir plots/mc \
    --PPbPrefix pPbMC \
    --PbPPrefix PbPMC \
    --PPbLabel "pPb MC reco" \
    --PbPLabel "Pbp MC reco"
echo "MC plots saved to plots/mc/"
