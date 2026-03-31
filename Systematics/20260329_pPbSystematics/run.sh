#!/bin/bash

set -e

THISDIR=$(cd "$(dirname "$0")" && pwd)
ANALYSISDIR=/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh

cd "$THISDIR"

SYSTEMS_CSV=${SYSTEMS:-pPb}
ZPT_RANGES_CSV=${ZPT_RANGES:-40_350}
TRACK_RANGES_CSV=${TRACK_RANGES:-2_500}
DEFAULT_PP_FAMILIES=TrackSelection,TrackCorrection,MuonRejection,PUpp,ScaleFactor
DEFAULT_PA_FAMILIES=TrackSelection,TrackCorrection,MuonRejection,PUpPb,ScaleFactor
USER_INCLUDE_FAMILIES=${INCLUDE_FAMILIES:-}
USER_PLOT_FAMILIES=${PLOT_FAMILIES:-}
DO_CALC=${DO_CALC:-1}
DO_PLOT=${DO_PLOT:-1}

IFS=',' read -ra SYSTEMS_ARRAY <<< "$SYSTEMS_CSV"
IFS=',' read -ra ZPT_ARRAY <<< "$ZPT_RANGES_CSV"
IFS=',' read -ra TRACK_ARRAY <<< "$TRACK_RANGES_CSV"

mkdir -p "$THISDIR/output" "$THISDIR/plots"

for SYSTEM in "${SYSTEMS_ARRAY[@]}"; do
   if [ "$SYSTEM" = "pp" ]; then
      PREFIX="pp"
      OFFICIAL_TAG="$OFFICIAL_TAG_PP"
      PU_PP_FILE="$ANALYSISDIR/${PREFIX}_trkResidual_${OFFICIAL_TAG}_IsPURejectFalse"
      PU_PPB_FILE=""
      DEFAULT_FAMILIES="$DEFAULT_PP_FAMILIES"
   else
      PREFIX="$SYSTEM"
      OFFICIAL_TAG="$OFFICIAL_TAG_PPB"
      PU_PP_FILE=""
      PU_PPB_FILE="$ANALYSISDIR/${PREFIX}_trkResidual_${OFFICIAL_TAG}_IsPURejectFalse"
      DEFAULT_FAMILIES="$DEFAULT_PA_FAMILIES"
   fi

   INCLUDE_FAMILIES="${USER_INCLUDE_FAMILIES:-$DEFAULT_FAMILIES}"
   PLOT_FAMILIES="${USER_PLOT_FAMILIES:-$INCLUDE_FAMILIES}"

   for ZPT in "${ZPT_ARRAY[@]}"; do
      NOMINAL_FILE="$ANALYSISDIR/${PREFIX}_trkResidual_${OFFICIAL_TAG}_ZPT${ZPT}-result.root"
      TRACK_SELECTION_FILES="$ANALYSISDIR/${PREFIX}_trkResidual_${OFFICIAL_TAG}_Loose_ZPT${ZPT}-result.root,$ANALYSISDIR/${PREFIX}_trkResidual_${OFFICIAL_TAG}_Tight_ZPT${ZPT}-result.root"
      MUON_REJECTION_FILES="$ANALYSISDIR/${PREFIX}_trkResidual_${OFFICIAL_TAG}_IsMuTaggedFalse_ZPT${ZPT}-result.root"
      SCALE_FACTOR_FILES="$ANALYSISDIR/${PREFIX}_trkResidual_${OFFICIAL_TAG}_MuVar0_ZPT${ZPT}-result.root,$ANALYSISDIR/${PREFIX}_trkResidual_${OFFICIAL_TAG}_MuVar1_ZPT${ZPT}-result.root,$ANALYSISDIR/${PREFIX}_trkResidual_${OFFICIAL_TAG}_MuVar2_ZPT${ZPT}-result.root,$ANALYSISDIR/${PREFIX}_trkResidual_${OFFICIAL_TAG}_MuVar3_ZPT${ZPT}-result.root"

      for TRACK in "${TRACK_ARRAY[@]}"; do
         OUTPUT_ROOT="$THISDIR/output/${SYSTEM}_${OFFICIAL_TAG}_ZPT${ZPT}_trkPT${TRACK}-systematics.root"
         OUTPUT_BASE="$THISDIR/plots/${SYSTEM}_${OFFICIAL_TAG}_ZPT${ZPT}_trkPT${TRACK}-systematics"

         if [ "$DO_CALC" = "1" ]; then
            ./ExecuteCalculateSystematics \
               --Nominal "$NOMINAL_FILE" \
               --Output "$OUTPUT_ROOT" \
               --TrackTag "$TRACK" \
               --IncludeFamilies "$INCLUDE_FAMILIES" \
               --TrackSelectionFiles "$TRACK_SELECTION_FILES" \
               --MuonRejectionFiles "$MUON_REJECTION_FILES" \
               ${PU_PP_FILE:+--PUppFiles ${PU_PP_FILE}_ZPT${ZPT}-result.root} \
               ${PU_PPB_FILE:+--PUpPbFiles ${PU_PPB_FILE}_ZPT${ZPT}-result.root} \
               --ScaleFactorFiles "$SCALE_FACTOR_FILES"
         fi

         if [ "$DO_PLOT" = "1" ]; then
            ./ExecutePlotSystematics \
                --Input "$OUTPUT_ROOT" \
                --Nominal "$NOMINAL_FILE" \
                --OutputBase "$OUTPUT_BASE" \
                --Collision "$SYSTEM" \
                --ZPTRange "$ZPT" \
                --TrackPTRange "$TRACK" \
                --Families "$PLOT_FAMILIES"
         fi
      done
   done
done
