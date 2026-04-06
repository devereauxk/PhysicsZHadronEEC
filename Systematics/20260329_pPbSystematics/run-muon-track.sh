#!/bin/bash

set -e

THISDIR=$(cd "$(dirname "$0")" && pwd)
ANALYSISDIR=/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh

cd "$THISDIR"

SYSTEMS_CSV=${SYSTEMS:-pp,pPb,PbP,pPbPbp}
ZPT_RANGES_CSV=${ZPT_RANGES:-5_500}
TRACK_RANGES_CSV=${TRACK_RANGES:-0.5_500}
VARIATION_TAGS_CSV=${VARIATION_TAGS:-IsMuTaggedFalse,TrackMuDR0p001,TrackMuDR0p0025,TrackMuDR0p0035,TrackMuClosestTrue}
VARIATION_LABELS_CSV=${VARIATION_LABELS:-No rejection,#DeltaR < 0.001,#DeltaR < 0.0025,#DeltaR < 0.0035,2 closest tracks}

IFS=',' read -ra SYSTEMS_ARRAY <<< "$SYSTEMS_CSV"
IFS=',' read -ra ZPT_ARRAY <<< "$ZPT_RANGES_CSV"
IFS=',' read -ra TRACK_ARRAY <<< "$TRACK_RANGES_CSV"
IFS=',' read -ra VARIATION_TAGS_ARRAY <<< "$VARIATION_TAGS_CSV"
IFS=',' read -ra VARIATION_LABELS_ARRAY <<< "$VARIATION_LABELS_CSV"

mkdir -p "$THISDIR/plots/muonTrack"

if [ "${#VARIATION_TAGS_ARRAY[@]}" -ne "${#VARIATION_LABELS_ARRAY[@]}" ]; then
   echo "Variation tag/label count mismatch for muon-track plotting" >&2
   exit 1
fi

for SYSTEM in "${SYSTEMS_ARRAY[@]}"; do
   if [ "$SYSTEM" = "pp" ]; then
      PREFIX="pp"
      OFFICIAL_TAG="$OFFICIAL_TAG_PP"
   else
      PREFIX="$SYSTEM"
      OFFICIAL_TAG="$OFFICIAL_TAG_PPB"
   fi

   for ZPT in "${ZPT_ARRAY[@]}"; do
      for TRACK in "${TRACK_ARRAY[@]}"; do
         OUTPUT_BASE="$THISDIR/plots/muonTrack/${SYSTEM}_${OFFICIAL_TAG}_ZPT${ZPT}_trkPT${TRACK}-MuonTrack"
         VARIATION_PPB_FILES=()
         VARIATION_PBP_FILES=()
         VARIATION_FILES=()
         for TAG_SUFFIX in "${VARIATION_TAGS_ARRAY[@]}"; do
            if [ "$SYSTEM" = "pPbPbp" ]; then
               VARIATION_PPB_FILES+=("$ANALYSISDIR/pPb_trkResidual_${OFFICIAL_TAG}_${TAG_SUFFIX}_ZPT${ZPT}-nosub.root")
               VARIATION_PBP_FILES+=("$ANALYSISDIR/PbP_trkResidual_${OFFICIAL_TAG}_${TAG_SUFFIX}_ZPT${ZPT}-nosub.root")
            else
               VARIATION_FILES+=("$ANALYSISDIR/${PREFIX}_trkResidual_${OFFICIAL_TAG}_${TAG_SUFFIX}_ZPT${ZPT}-result.root")
            fi
         done
         VARIATION_PPB_FILES_CSV=$(IFS=,; echo "${VARIATION_PPB_FILES[*]}")
         VARIATION_PBP_FILES_CSV=$(IFS=,; echo "${VARIATION_PBP_FILES[*]}")
         VARIATION_FILES_CSV=$(IFS=,; echo "${VARIATION_FILES[*]}")
          if [ "$SYSTEM" = "pPbPbp" ]; then
             ./ExecutePlotMuonTrackComparison \
                --NominalPPb "$ANALYSISDIR/pPb_trkResidual_${OFFICIAL_TAG}_ZPT${ZPT}-nosub.root" \
                --NominalPBP "$ANALYSISDIR/PbP_trkResidual_${OFFICIAL_TAG}_ZPT${ZPT}-nosub.root" \
                --VariationPPbFiles "$VARIATION_PPB_FILES_CSV" \
                --VariationPBPFiles "$VARIATION_PBP_FILES_CSV" \
                --VariationLabels "$VARIATION_LABELS_CSV" \
                --OutputBase "$OUTPUT_BASE" \
                --Collision "$SYSTEM" \
                --ZPTRange "$ZPT" \
                --TrackPTRange "$TRACK"
          else
             NOMINAL_FILE="$ANALYSISDIR/${PREFIX}_trkResidual_${OFFICIAL_TAG}_ZPT${ZPT}-result.root"
             ./ExecutePlotMuonTrackComparison \
                --Nominal "$NOMINAL_FILE" \
                --VariationFiles "$VARIATION_FILES_CSV" \
                --VariationLabels "$VARIATION_LABELS_CSV" \
                --OutputBase "$OUTPUT_BASE" \
                --Collision "$SYSTEM" \
                --ZPTRange "$ZPT" \
               --TrackPTRange "$TRACK"
         fi
      done
   done
done
