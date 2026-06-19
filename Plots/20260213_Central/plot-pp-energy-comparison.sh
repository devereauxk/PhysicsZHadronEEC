#!/bin/bash
# Plot pp 5.02 vs 8.16 TeV energy comparison with stat + systematic bands.
# Produces per-bin DeltaEta/DeltaPhi PDFs for inclusive and scan selections.
#
# Usage: bash plot-pp-energy-comparison.sh

set -e

THISDIR=$(cd "$(dirname "$0")" && pwd)
cd "$THISDIR"

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialProductDictionary.sh

make ExecutePPEnergyComparisonPlot

PP5_TAG="${OFFICIAL_PP_5TEV_TAG}"
PP8_TAG="${OFFICIAL_TAG_PP}"
OUTPUT_BASE="${PLOT_OUTPUT_BASE:-plots/pp_energy_comparison}"
SYST_DIR="/home/kdeverea/PhysicsZHadronEEC/Systematics/20260329_pPbSystematics/output"

run_one() {
    local ZPT=$1
    local TRKPT=$2
    local IS_SCAN=$3
    ./ExecutePPEnergyComparisonPlot \
        --zPtRange "$ZPT" --trkPtRange "$TRKPT" \
        --pp5tevTag "$PP5_TAG" --pp8tevTag "$PP8_TAG" \
        --BaseDir "${OFFICIAL_RESULT_DIR}" \
        --systematicsDir5tev "$SYST_DIR" \
        --systematicsDir8tev "$SYST_DIR" \
        --outputBase "$OUTPUT_BASE" \
        --isScan "$IS_SCAN"
}

# Inclusive
run_one 0_500 0.5_15 false

# Scan
for ZPT in 0_500 0_30 30_500; do
    for TRKPT in 0.5_2 2_4 4_15; do
        run_one "$ZPT" "$TRKPT" true
    done
done

echo "=== pp energy comparison plots complete ==="
echo "Output: ${OUTPUT_BASE}/${PP8_TAG}/"
