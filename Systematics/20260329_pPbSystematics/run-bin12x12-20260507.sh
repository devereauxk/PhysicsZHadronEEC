#!/bin/bash

set -euo pipefail

THISDIR=$(cd "$(dirname "$0")" && pwd)
ANALYSISDIR=/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots

cd /home/kdeverea/PhysicsZHadronEEC
source /home/kdeverea/PhysicsZHadronEEC/SetupAnalysis.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh

cd "$THISDIR"
make

PP_TAG="${PP_TAG_OVERRIDE:-EEV5_ZV9_trkV27_nmix10_bin12x12_20260507}"
PPB_TAG="${PPB_TAG_OVERRIDE:-ZV9_trkV27_nmix10_bin12x12_20260507}"
SYSTEMS_CSV="${SYSTEMS:-pp,pPb,PbP,pPbPbp}"
DO_CALC="${DO_CALC:-1}"
DO_PLOT="${DO_PLOT:-1}"

IFS=',' read -ra SYSTEMS_ARRAY <<< "$SYSTEMS_CSV"

mkdir -p "$THISDIR/output" "$THISDIR/plots"

run_single_system() {
    local SYSTEM=$1
    local ZPT=$2
    local TRACK=$3
    local NOMINAL_FILE
    local TRACK_SELECTION_FILES
    local TRACK_CORRECTION_FILES
    local MUON_REJECTION_FILES
    local PU_ARG=()
    local SCALE_FACTOR_FILES
    local ENERGY_EXTRAPOLATION_FILES=""
    local INCLUDE_FAMILIES
    local PLOT_FAMILIES
    local TAG

    if [ "$SYSTEM" = "pp" ]; then
        TAG="$PP_TAG"
        NOMINAL_FILE="$ANALYSISDIR/pp_trkResidual_${TAG}_ZPT${ZPT}-result.root"
        TRACK_SELECTION_FILES="$ANALYSISDIR/pp_trkResidual_${TAG}_Loose_ZPT${ZPT}-result.root,$ANALYSISDIR/pp_trkResidual_${TAG}_Tight_ZPT${ZPT}-result.root"
        TRACK_CORRECTION_FILES="$ANALYSISDIR/pp_trkResidual_${TAG}_TrackCorrection0p976_ZPT${ZPT}-result.root,$ANALYSISDIR/pp_trkResidual_${TAG}_TrackCorrection1p024_ZPT${ZPT}-result.root"
        MUON_REJECTION_FILES="$ANALYSISDIR/pp_trkResidual_${TAG}_IsMuTaggedFalse_ZPT${ZPT}-result.root"
        PU_ARG=(--PUppFiles "$ANALYSISDIR/pp_trkResidual_${TAG}_IsPURejectTrue_ZPT${ZPT}-result.root")
        SCALE_FACTOR_FILES="$ANALYSISDIR/pp_trkResidual_${TAG}_MuVar0_ZPT${ZPT}-result.root,$ANALYSISDIR/pp_trkResidual_${TAG}_MuVar1_ZPT${ZPT}-result.root,$ANALYSISDIR/pp_trkResidual_${TAG}_MuVar2_ZPT${ZPT}-result.root,$ANALYSISDIR/pp_trkResidual_${TAG}_MuVar3_ZPT${ZPT}-result.root"
        ENERGY_EXTRAPOLATION_FILES="$ANALYSISDIR/pp_trkResidual_${TAG}_EEPrivate_ZPT${ZPT}-result.root"
        INCLUDE_FAMILIES="${INCLUDE_FAMILIES_PP:-TrackSelection,TrackCorrection,MuonRejection,PUpp,ScaleFactor,EnergyExtrapolation}"
        PLOT_FAMILIES="${PLOT_FAMILIES_PP:-$INCLUDE_FAMILIES}"
    else
        TAG="$PPB_TAG"
        NOMINAL_FILE="$ANALYSISDIR/${SYSTEM}_trkResidual_${TAG}_ZPT${ZPT}-result.root"
        TRACK_SELECTION_FILES="$ANALYSISDIR/${SYSTEM}_trkResidual_${TAG}_Loose_ZPT${ZPT}-result.root,$ANALYSISDIR/${SYSTEM}_trkResidual_${TAG}_Tight_ZPT${ZPT}-result.root"
        TRACK_CORRECTION_FILES="$ANALYSISDIR/${SYSTEM}_trkResidual_${TAG}_TrackCorrection0p976_ZPT${ZPT}-result.root,$ANALYSISDIR/${SYSTEM}_trkResidual_${TAG}_TrackCorrection1p024_ZPT${ZPT}-result.root"
        MUON_REJECTION_FILES="$ANALYSISDIR/${SYSTEM}_trkResidual_${TAG}_IsMuTaggedFalse_ZPT${ZPT}-result.root"
        PU_ARG=(--PUpPbFiles "$ANALYSISDIR/${SYSTEM}_trkResidual_${TAG}_IsPURejectTrue_ZPT${ZPT}-result.root")
        SCALE_FACTOR_FILES="$ANALYSISDIR/${SYSTEM}_trkResidual_${TAG}_MuVar0_ZPT${ZPT}-result.root,$ANALYSISDIR/${SYSTEM}_trkResidual_${TAG}_MuVar1_ZPT${ZPT}-result.root,$ANALYSISDIR/${SYSTEM}_trkResidual_${TAG}_MuVar2_ZPT${ZPT}-result.root,$ANALYSISDIR/${SYSTEM}_trkResidual_${TAG}_MuVar3_ZPT${ZPT}-result.root"
        INCLUDE_FAMILIES="${INCLUDE_FAMILIES_PA:-TrackSelection,TrackCorrection,MuonRejection,PUpPb,ScaleFactor}"
        PLOT_FAMILIES="${PLOT_FAMILIES_PA:-$INCLUDE_FAMILIES}"
    fi

    local OUTPUT_ROOT="$THISDIR/output/${SYSTEM}_${TAG}_ZPT${ZPT}_trkPT${TRACK}-systematics.root"
    local OUTPUT_BASE="$THISDIR/plots/${SYSTEM}_${TAG}_ZPT${ZPT}_trkPT${TRACK}-systematics"

    if [ "$DO_CALC" = "1" ]; then
        ./ExecuteCalculateSystematics \
            --Nominal "$NOMINAL_FILE" \
            --Output "$OUTPUT_ROOT" \
            --TrackTag "$TRACK" \
            --UseModified12x12 true \
            --IncludeFamilies "$INCLUDE_FAMILIES" \
            --TrackSelectionFiles "$TRACK_SELECTION_FILES" \
            --TrackCorrectionFiles "$TRACK_CORRECTION_FILES" \
            --MuonRejectionFiles "$MUON_REJECTION_FILES" \
            "${PU_ARG[@]}" \
            --ScaleFactorFiles "$SCALE_FACTOR_FILES" \
            --EnergyExtrapolationFiles "$ENERGY_EXTRAPOLATION_FILES"
    fi

    if [ "$DO_PLOT" = "1" ]; then
        ./ExecutePlotSystematics \
            --Input "$OUTPUT_ROOT" \
            --Nominal "$NOMINAL_FILE" \
            --OutputBase "$OUTPUT_BASE" \
            --Collision "$SYSTEM" \
            --UseModified12x12 true \
            --ZPTRange "$ZPT" \
            --TrackPTRange "$TRACK" \
            --Families "$PLOT_FAMILIES"
    fi
}

run_combined_system() {
    local ZPT=$1
    local TRACK=$2

    local OUTPUT_ROOT="$THISDIR/output/pPbPbp_${PPB_TAG}_ZPT${ZPT}_trkPT${TRACK}-systematics.root"
    local OUTPUT_BASE="$THISDIR/plots/pPbPbp_${PPB_TAG}_ZPT${ZPT}_trkPT${TRACK}-systematics"
    local INCLUDE_FAMILIES="${INCLUDE_FAMILIES_PA:-TrackSelection,TrackCorrection,MuonRejection,PUpPb,ScaleFactor}"
    local PLOT_FAMILIES="${PLOT_FAMILIES_COMBINED:-$INCLUDE_FAMILIES}"
    local DIFFERENCE_FAMILIES="${DIFFERENCE_FAMILIES:-TrackSelection,TrackCorrection,MuonRejection,PU,ScaleFactor,EnergyExtrapolation}"

    if [ "$DO_CALC" = "1" ]; then
        ./ExecuteCalculateSystematics \
            --NominalPP "$ANALYSISDIR/pp_trkResidual_${PP_TAG}_ZPT${ZPT}-result.root" \
            --NominalPPb "$ANALYSISDIR/pPb_trkResidual_${PPB_TAG}_ZPT${ZPT}-nosub.root" \
            --NominalPBP "$ANALYSISDIR/PbP_trkResidual_${PPB_TAG}_ZPT${ZPT}-nosub.root" \
            --Output "$OUTPUT_ROOT" \
            --TrackTag "$TRACK" \
            --UseModified12x12 true \
            --IncludeFamilies "$INCLUDE_FAMILIES" \
            --IncludeDifferenceFamilies "$DIFFERENCE_FAMILIES" \
            --TrackSelectionFilesPP "$ANALYSISDIR/pp_trkResidual_${PP_TAG}_Loose_ZPT${ZPT}-result.root,$ANALYSISDIR/pp_trkResidual_${PP_TAG}_Tight_ZPT${ZPT}-result.root" \
            --TrackSelectionFilesPPb "$ANALYSISDIR/pPb_trkResidual_${PPB_TAG}_Loose_ZPT${ZPT}-nosub.root,$ANALYSISDIR/pPb_trkResidual_${PPB_TAG}_Tight_ZPT${ZPT}-nosub.root" \
            --TrackSelectionFilesPBP "$ANALYSISDIR/PbP_trkResidual_${PPB_TAG}_Loose_ZPT${ZPT}-nosub.root,$ANALYSISDIR/PbP_trkResidual_${PPB_TAG}_Tight_ZPT${ZPT}-nosub.root" \
            --TrackCorrectionFilesPP "$ANALYSISDIR/pp_trkResidual_${PP_TAG}_TrackCorrection0p976_ZPT${ZPT}-result.root,$ANALYSISDIR/pp_trkResidual_${PP_TAG}_TrackCorrection1p024_ZPT${ZPT}-result.root" \
            --TrackCorrectionFilesPPb "$ANALYSISDIR/pPb_trkResidual_${PPB_TAG}_TrackCorrection0p976_ZPT${ZPT}-nosub.root,$ANALYSISDIR/pPb_trkResidual_${PPB_TAG}_TrackCorrection1p024_ZPT${ZPT}-nosub.root" \
            --TrackCorrectionFilesPBP "$ANALYSISDIR/PbP_trkResidual_${PPB_TAG}_TrackCorrection0p976_ZPT${ZPT}-nosub.root,$ANALYSISDIR/PbP_trkResidual_${PPB_TAG}_TrackCorrection1p024_ZPT${ZPT}-nosub.root" \
            --MuonRejectionFilesPP "$ANALYSISDIR/pp_trkResidual_${PP_TAG}_IsMuTaggedFalse_ZPT${ZPT}-result.root" \
            --MuonRejectionFilesPPb "$ANALYSISDIR/pPb_trkResidual_${PPB_TAG}_IsMuTaggedFalse_ZPT${ZPT}-nosub.root" \
            --MuonRejectionFilesPBP "$ANALYSISDIR/PbP_trkResidual_${PPB_TAG}_IsMuTaggedFalse_ZPT${ZPT}-nosub.root" \
            --PUppFilesPP "$ANALYSISDIR/pp_trkResidual_${PP_TAG}_IsPURejectTrue_ZPT${ZPT}-result.root" \
            --PUpPbFilesPPb "$ANALYSISDIR/pPb_trkResidual_${PPB_TAG}_IsPURejectTrue_ZPT${ZPT}-nosub.root" \
            --PUpPbFilesPBP "$ANALYSISDIR/PbP_trkResidual_${PPB_TAG}_IsPURejectTrue_ZPT${ZPT}-nosub.root" \
            --ScaleFactorFilesPP "$ANALYSISDIR/pp_trkResidual_${PP_TAG}_MuVar0_ZPT${ZPT}-result.root,$ANALYSISDIR/pp_trkResidual_${PP_TAG}_MuVar1_ZPT${ZPT}-result.root,$ANALYSISDIR/pp_trkResidual_${PP_TAG}_MuVar2_ZPT${ZPT}-result.root,$ANALYSISDIR/pp_trkResidual_${PP_TAG}_MuVar3_ZPT${ZPT}-result.root" \
            --ScaleFactorFilesPPb "$ANALYSISDIR/pPb_trkResidual_${PPB_TAG}_MuVar0_ZPT${ZPT}-nosub.root,$ANALYSISDIR/pPb_trkResidual_${PPB_TAG}_MuVar1_ZPT${ZPT}-nosub.root,$ANALYSISDIR/pPb_trkResidual_${PPB_TAG}_MuVar2_ZPT${ZPT}-nosub.root,$ANALYSISDIR/pPb_trkResidual_${PPB_TAG}_MuVar3_ZPT${ZPT}-nosub.root" \
            --ScaleFactorFilesPBP "$ANALYSISDIR/PbP_trkResidual_${PPB_TAG}_MuVar0_ZPT${ZPT}-nosub.root,$ANALYSISDIR/PbP_trkResidual_${PPB_TAG}_MuVar1_ZPT${ZPT}-nosub.root,$ANALYSISDIR/PbP_trkResidual_${PPB_TAG}_MuVar2_ZPT${ZPT}-nosub.root,$ANALYSISDIR/PbP_trkResidual_${PPB_TAG}_MuVar3_ZPT${ZPT}-nosub.root" \
            --EnergyExtrapolationFilesPP "$ANALYSISDIR/pp_trkResidual_${PP_TAG}_EEPrivate_ZPT${ZPT}-result.root"
    fi

    if [ "$DO_PLOT" = "1" ]; then
        ./ExecutePlotSystematics \
            --Input "$OUTPUT_ROOT" \
            --NominalPPb "$ANALYSISDIR/pPb_trkResidual_${PPB_TAG}_ZPT${ZPT}-nosub.root" \
            --NominalPBP "$ANALYSISDIR/PbP_trkResidual_${PPB_TAG}_ZPT${ZPT}-nosub.root" \
            --OutputBase "$OUTPUT_BASE" \
            --Collision pPbPbp \
            --UseModified12x12 true \
            --ZPTRange "$ZPT" \
            --TrackPTRange "$TRACK" \
            --Families "$PLOT_FAMILIES" \
            --DifferenceFamilies "$DIFFERENCE_FAMILIES"
    fi
}

run_note_bin() {
    local ZPT=$1
    local TRACK=$2
    local SYSTEM

    for SYSTEM in "${SYSTEMS_ARRAY[@]}"; do
        case "$SYSTEM" in
            pp|pPb|PbP)
                run_single_system "$SYSTEM" "$ZPT" "$TRACK"
                ;;
            pPbPbp)
                run_combined_system "$ZPT" "$TRACK"
                ;;
            *)
                echo "Unknown system '$SYSTEM'" >&2
                exit 1
                ;;
        esac
    done
}

run_note_bin "0_500" "0.5_15"
run_note_bin "0_30" "0.5_2"
run_note_bin "0_30" "2_4"
run_note_bin "0_30" "4_15"
run_note_bin "30_500" "0.5_2"
run_note_bin "30_500" "2_4"
run_note_bin "30_500" "4_15"
