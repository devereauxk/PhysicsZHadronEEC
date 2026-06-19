#!/bin/bash
# Harvest pp 5 TeV systematics into the same format as the 8.16 TeV pp systematics.
# Produces Total_DeltaEta / Total_DeltaPhi histograms consumed by the plotter.
# Families: TrackSelection, TrackCorrection, MuonRejection, PUpp, ScaleFactor.
# No EnergyExtrapolation (nothing to vary at native 5 TeV).
#
# Usage: bash run-pp5tev.sh

set -e

THISDIR=$(cd "$(dirname "$0")" && pwd)
source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialProductDictionary.sh
ANALYSISDIR="${OFFICIAL_RESULT_DIR}"

cd "$THISDIR"

TAG_5TEV="${OFFICIAL_PP_5TEV_TAG}"
INCLUDE_FAMILIES="TrackSelection,TrackCorrection,MuonRejection,PUpp,ScaleFactor"

mkdir -p "$THISDIR/output"

run_bin() {
    local ZPT=$1
    local TRACK=$2
    local TAG_SUFFIX=$3

    local TAG="${TAG_5TEV}${TAG_SUFFIX}"
    local NOMINAL="$ANALYSISDIR/pp_trkResidual_${TAG}_ZPT${ZPT}-result.root"

    local TRACK_SELECTION="$ANALYSISDIR/pp_trkResidual_${TAG}_Loose_ZPT${ZPT}-result.root,$ANALYSISDIR/pp_trkResidual_${TAG}_Tight_ZPT${ZPT}-result.root"
    local TRACK_CORRECTION="$ANALYSISDIR/pp_trkResidual_${TAG}_TrackCorrection0p976_ZPT${ZPT}-result.root,$ANALYSISDIR/pp_trkResidual_${TAG}_TrackCorrection1p024_ZPT${ZPT}-result.root"
    local MUON_REJECTION="$ANALYSISDIR/pp_trkResidual_${TAG}_IsMuTaggedFalse_ZPT${ZPT}-result.root"
    local PU_PP="$ANALYSISDIR/pp_trkResidual_${TAG}_IsPURejectTrue_ZPT${ZPT}-result.root"
    local SCALE_FACTOR="$ANALYSISDIR/pp_trkResidual_${TAG}_MuVar0_ZPT${ZPT}-result.root,$ANALYSISDIR/pp_trkResidual_${TAG}_MuVar1_ZPT${ZPT}-result.root,$ANALYSISDIR/pp_trkResidual_${TAG}_MuVar2_ZPT${ZPT}-result.root,$ANALYSISDIR/pp_trkResidual_${TAG}_MuVar3_ZPT${ZPT}-result.root"

    local OUTPUT="$THISDIR/output/pp5tev_${TAG}_ZPT${ZPT}_trkPT${TRACK}-systematics.root"

    echo "=== pp 5 TeV systematics: ZPT=${ZPT} trkPT=${TRACK} tag=${TAG} ==="
    ./ExecuteCalculateSystematics \
        --Nominal "$NOMINAL" \
        --Output "$OUTPUT" \
        --TrackTag "$TRACK" \
        --UseModified12x12 true \
        --IncludeFamilies "$INCLUDE_FAMILIES" \
        --TrackSelectionFiles "$TRACK_SELECTION" \
        --TrackCorrectionFiles "$TRACK_CORRECTION" \
        --MuonRejectionFiles "$MUON_REJECTION" \
        --PUppFiles "$PU_PP" \
        --ScaleFactorFiles "$SCALE_FACTOR" \
        --EnergyExtrapolationFiles ""
}

# Inclusive
run_bin 0_500 0.5_15 ""

# Scan
for ZPT in 0_500 0_30 30_500; do
    for TRACK in 0.5_2 2_4 4_15; do
        run_bin "$ZPT" "$TRACK" "_scan"
    done
done

echo "=== pp 5 TeV systematics harvesting complete ==="
