#!/bin/bash
# Plot pPb vs Pbp overlay for sub-binned track pT (0.5_1, 1_2)
# alongside the standard 0.5_2 bin, for ZPT 0_30, 30_500, 0_500.
# Output goes to plots/central_overlay_PPbPbP/ (same as the standard script).

set -euo pipefail

make ExecuteCentralOverlayPPbPbPPlot

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
PPB_TAG="${PPB_TAG:-$OFFICIAL_TAG_PPB}"

run_one() {
    local ZPT=$1
    local TRKPT=$2
    ./ExecuteCentralOverlayPPbPbPPlot --zPtRange "$ZPT" --trkPtRange "$TRKPT" --pPbtag "$PPB_TAG" --pPbMCTag "$PPB_TAG"
}

for ZPT in 0_30 30_500 0_500; do
    for TRKPT in 0.5_2 0.5_1 1_2; do
        run_one "$ZPT" "$TRKPT"
    done
done

echo "=== Done: plots/central_overlay_PPbPbP/ ==="
