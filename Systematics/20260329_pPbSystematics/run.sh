#!/bin/bash

set -e

THISDIR=$(cd "$(dirname "$0")" && pwd)
ANALYSISDIR=/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh

cd "$THISDIR"

SYSTEMS_CSV=${SYSTEMS:-pp,pPb,PbP,pPbPbp}
ZPT_RANGES_CSV=${ZPT_RANGES:-5_500}
TRACK_RANGES_CSV=${TRACK_RANGES:-0.5_500}
DEFAULT_PP_FAMILIES=TrackCorrection,MuonRejection,PUpp,ScaleFactor
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
      PU_PP_FILE="$ANALYSISDIR/${PREFIX}_trkResidual_${OFFICIAL_TAG}_IsPURejectTrue"
      PU_PPB_FILE=""
      DEFAULT_FAMILIES="$DEFAULT_PP_FAMILIES"
      NOMINAL_FILE_BASE="$ANALYSISDIR/${PREFIX}_trkResidual_${OFFICIAL_TAG}"
      TRACK_SELECTION_FILES_PPB=""
      TRACK_SELECTION_FILES_PBP=""
      TRACK_CORRECTION_FILES_PPB=""
      TRACK_CORRECTION_FILES_PBP=""
      MUON_REJECTION_FILES_PPB=""
      MUON_REJECTION_FILES_PBP=""
      PU_PPB_FILE_PBP=""
      SCALE_FACTOR_FILES_PPB=""
      SCALE_FACTOR_FILES_PBP=""
   elif [ "$SYSTEM" = "pPbPbp" ]; then
      PREFIX="$SYSTEM"
      OFFICIAL_TAG="$OFFICIAL_TAG_PPB"
      DEFAULT_FAMILIES="$DEFAULT_PA_FAMILIES"
   else
      PREFIX="$SYSTEM"
      OFFICIAL_TAG="$OFFICIAL_TAG_PPB"
      PU_PP_FILE=""
      PU_PPB_FILE="$ANALYSISDIR/${PREFIX}_trkResidual_${OFFICIAL_TAG}_IsPURejectTrue"
      DEFAULT_FAMILIES="$DEFAULT_PA_FAMILIES"
   fi

   INCLUDE_FAMILIES="${USER_INCLUDE_FAMILIES:-$DEFAULT_FAMILIES}"
   PLOT_FAMILIES="${USER_PLOT_FAMILIES:-$INCLUDE_FAMILIES}"

   for ZPT in "${ZPT_ARRAY[@]}"; do
      if [ "$SYSTEM" = "pPbPbp" ]; then
         NOMINAL_FILE_PPB="$ANALYSISDIR/pPb_trkResidual_${OFFICIAL_TAG}_ZPT${ZPT}-nosub.root"
         NOMINAL_FILE_PBP="$ANALYSISDIR/PbP_trkResidual_${OFFICIAL_TAG}_ZPT${ZPT}-nosub.root"
         TRACK_SELECTION_FILES_PPB="$ANALYSISDIR/pPb_trkResidual_${OFFICIAL_TAG}_Loose_ZPT${ZPT}-nosub.root,$ANALYSISDIR/pPb_trkResidual_${OFFICIAL_TAG}_Tight_ZPT${ZPT}-nosub.root"
         TRACK_SELECTION_FILES_PBP="$ANALYSISDIR/PbP_trkResidual_${OFFICIAL_TAG}_Loose_ZPT${ZPT}-nosub.root,$ANALYSISDIR/PbP_trkResidual_${OFFICIAL_TAG}_Tight_ZPT${ZPT}-nosub.root"
         TRACK_CORRECTION_FILES_PPB="$ANALYSISDIR/pPb_trkResidual_${OFFICIAL_TAG}_TrackCorrection0p976_ZPT${ZPT}-nosub.root,$ANALYSISDIR/pPb_trkResidual_${OFFICIAL_TAG}_TrackCorrection1p024_ZPT${ZPT}-nosub.root"
         TRACK_CORRECTION_FILES_PBP="$ANALYSISDIR/PbP_trkResidual_${OFFICIAL_TAG}_TrackCorrection0p976_ZPT${ZPT}-nosub.root,$ANALYSISDIR/PbP_trkResidual_${OFFICIAL_TAG}_TrackCorrection1p024_ZPT${ZPT}-nosub.root"
         MUON_REJECTION_FILES_PPB="$ANALYSISDIR/pPb_trkResidual_${OFFICIAL_TAG}_IsMuTaggedFalse_ZPT${ZPT}-nosub.root"
         MUON_REJECTION_FILES_PBP="$ANALYSISDIR/PbP_trkResidual_${OFFICIAL_TAG}_IsMuTaggedFalse_ZPT${ZPT}-nosub.root"
         PU_PPB_FILE_PPB="$ANALYSISDIR/pPb_trkResidual_${OFFICIAL_TAG}_IsPURejectTrue"
         PU_PPB_FILE_PBP="$ANALYSISDIR/PbP_trkResidual_${OFFICIAL_TAG}_IsPURejectTrue"
         SCALE_FACTOR_FILES_PPB="$ANALYSISDIR/pPb_trkResidual_${OFFICIAL_TAG}_MuVar0_ZPT${ZPT}-nosub.root,$ANALYSISDIR/pPb_trkResidual_${OFFICIAL_TAG}_MuVar1_ZPT${ZPT}-nosub.root,$ANALYSISDIR/pPb_trkResidual_${OFFICIAL_TAG}_MuVar2_ZPT${ZPT}-nosub.root,$ANALYSISDIR/pPb_trkResidual_${OFFICIAL_TAG}_MuVar3_ZPT${ZPT}-nosub.root"
         SCALE_FACTOR_FILES_PBP="$ANALYSISDIR/PbP_trkResidual_${OFFICIAL_TAG}_MuVar0_ZPT${ZPT}-nosub.root,$ANALYSISDIR/PbP_trkResidual_${OFFICIAL_TAG}_MuVar1_ZPT${ZPT}-nosub.root,$ANALYSISDIR/PbP_trkResidual_${OFFICIAL_TAG}_MuVar2_ZPT${ZPT}-nosub.root,$ANALYSISDIR/PbP_trkResidual_${OFFICIAL_TAG}_MuVar3_ZPT${ZPT}-nosub.root"
      else
         NOMINAL_FILE="$ANALYSISDIR/${PREFIX}_trkResidual_${OFFICIAL_TAG}_ZPT${ZPT}-result.root"
      fi
      TRACK_SELECTION_FILES=""
      if [ "$SYSTEM" != "pp" ] && [ "$SYSTEM" != "pPbPbp" ]; then
          TRACK_SELECTION_FILES="$ANALYSISDIR/${PREFIX}_trkResidual_${OFFICIAL_TAG}_Loose_ZPT${ZPT}-result.root,$ANALYSISDIR/${PREFIX}_trkResidual_${OFFICIAL_TAG}_Tight_ZPT${ZPT}-result.root"
         TRACK_CORRECTION_FILES="$ANALYSISDIR/${PREFIX}_trkResidual_${OFFICIAL_TAG}_TrackCorrection0p976_ZPT${ZPT}-result.root,$ANALYSISDIR/${PREFIX}_trkResidual_${OFFICIAL_TAG}_TrackCorrection1p024_ZPT${ZPT}-result.root"
         MUON_REJECTION_FILES="$ANALYSISDIR/${PREFIX}_trkResidual_${OFFICIAL_TAG}_IsMuTaggedFalse_ZPT${ZPT}-result.root"
         SCALE_FACTOR_FILES="$ANALYSISDIR/${PREFIX}_trkResidual_${OFFICIAL_TAG}_MuVar0_ZPT${ZPT}-result.root,$ANALYSISDIR/${PREFIX}_trkResidual_${OFFICIAL_TAG}_MuVar1_ZPT${ZPT}-result.root,$ANALYSISDIR/${PREFIX}_trkResidual_${OFFICIAL_TAG}_MuVar2_ZPT${ZPT}-result.root,$ANALYSISDIR/${PREFIX}_trkResidual_${OFFICIAL_TAG}_MuVar3_ZPT${ZPT}-result.root"
      elif [ "$SYSTEM" = "pp" ]; then
         TRACK_CORRECTION_FILES="$ANALYSISDIR/${PREFIX}_trkResidual_${OFFICIAL_TAG}_TrackCorrection0p976_ZPT${ZPT}-result.root,$ANALYSISDIR/${PREFIX}_trkResidual_${OFFICIAL_TAG}_TrackCorrection1p024_ZPT${ZPT}-result.root"
         MUON_REJECTION_FILES="$ANALYSISDIR/${PREFIX}_trkResidual_${OFFICIAL_TAG}_IsMuTaggedFalse_ZPT${ZPT}-result.root"
         SCALE_FACTOR_FILES="$ANALYSISDIR/${PREFIX}_trkResidual_${OFFICIAL_TAG}_MuVar0_ZPT${ZPT}-result.root,$ANALYSISDIR/${PREFIX}_trkResidual_${OFFICIAL_TAG}_MuVar1_ZPT${ZPT}-result.root,$ANALYSISDIR/${PREFIX}_trkResidual_${OFFICIAL_TAG}_MuVar2_ZPT${ZPT}-result.root,$ANALYSISDIR/${PREFIX}_trkResidual_${OFFICIAL_TAG}_MuVar3_ZPT${ZPT}-result.root"
      fi

      for TRACK in "${TRACK_ARRAY[@]}"; do
         OUTPUT_ROOT="$THISDIR/output/${SYSTEM}_${OFFICIAL_TAG}_ZPT${ZPT}_trkPT${TRACK}-systematics.root"
         OUTPUT_BASE="$THISDIR/plots/${SYSTEM}_${OFFICIAL_TAG}_ZPT${ZPT}_trkPT${TRACK}-systematics"

         if [ "$DO_CALC" = "1" ]; then
            if [ "$SYSTEM" = "pPbPbp" ]; then
               ./ExecuteCalculateSystematics \
                  --NominalPPb "$NOMINAL_FILE_PPB" \
                  --NominalPBP "$NOMINAL_FILE_PBP" \
                  --Output "$OUTPUT_ROOT" \
                  --TrackTag "$TRACK" \
                  --IncludeFamilies "$INCLUDE_FAMILIES" \
                  --TrackSelectionFilesPPb "$TRACK_SELECTION_FILES_PPB" \
                  --TrackSelectionFilesPBP "$TRACK_SELECTION_FILES_PBP" \
                  --TrackCorrectionFilesPPb "$TRACK_CORRECTION_FILES_PPB" \
                  --TrackCorrectionFilesPBP "$TRACK_CORRECTION_FILES_PBP" \
                  --MuonRejectionFilesPPb "$MUON_REJECTION_FILES_PPB" \
                  --MuonRejectionFilesPBP "$MUON_REJECTION_FILES_PBP" \
                  --PUpPbFilesPPb "${PU_PPB_FILE_PPB}_ZPT${ZPT}-nosub.root" \
                  --PUpPbFilesPBP "${PU_PPB_FILE_PBP}_ZPT${ZPT}-nosub.root" \
                  --ScaleFactorFilesPPb "$SCALE_FACTOR_FILES_PPB" \
                  --ScaleFactorFilesPBP "$SCALE_FACTOR_FILES_PBP"
            else
               ./ExecuteCalculateSystematics \
                  --Nominal "$NOMINAL_FILE" \
                  --Output "$OUTPUT_ROOT" \
                  --TrackTag "$TRACK" \
                  --IncludeFamilies "$INCLUDE_FAMILIES" \
                  --TrackSelectionFiles "$TRACK_SELECTION_FILES" \
                  --TrackCorrectionFiles "$TRACK_CORRECTION_FILES" \
                  --MuonRejectionFiles "$MUON_REJECTION_FILES" \
                  ${PU_PP_FILE:+--PUppFiles ${PU_PP_FILE}_ZPT${ZPT}-result.root} \
                  ${PU_PPB_FILE:+--PUpPbFiles ${PU_PPB_FILE}_ZPT${ZPT}-result.root} \
                  --ScaleFactorFiles "$SCALE_FACTOR_FILES"
            fi
         fi

         if [ "$DO_PLOT" = "1" ]; then
            if [ "$SYSTEM" = "pPbPbp" ]; then
               ./ExecutePlotSystematics \
                   --Input "$OUTPUT_ROOT" \
                   --NominalPPb "$NOMINAL_FILE_PPB" \
                   --NominalPBP "$NOMINAL_FILE_PBP" \
                   --OutputBase "$OUTPUT_BASE" \
                   --Collision "$SYSTEM" \
                   --ZPTRange "$ZPT" \
                   --TrackPTRange "$TRACK" \
                   --Families "$PLOT_FAMILIES"
            else
               ./ExecutePlotSystematics \
                   --Input "$OUTPUT_ROOT" \
                   --Nominal "$NOMINAL_FILE" \
                   --OutputBase "$OUTPUT_BASE" \
                   --Collision "$SYSTEM" \
                   --ZPTRange "$ZPT" \
                   --TrackPTRange "$TRACK" \
                   --Families "$PLOT_FAMILIES"
            fi
         fi
      done
   done
done
