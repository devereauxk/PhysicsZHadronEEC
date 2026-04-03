#!/bin/bash

set -e

THISDIR=$(cd "$(dirname "$0")" && pwd)
ANALYSISDIR=/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh

cd "$THISDIR"

SYSTEMS_CSV=${SYSTEMS:-pp,pPb,PbP,pPbPbp}
ZPT_RANGES_CSV=${ZPT_RANGES:-5_500}
TRACK_RANGES_CSV=${TRACK_RANGES:-0.5_500}

IFS=',' read -ra SYSTEMS_ARRAY <<< "$SYSTEMS_CSV"
IFS=',' read -ra ZPT_ARRAY <<< "$ZPT_RANGES_CSV"
IFS=',' read -ra TRACK_ARRAY <<< "$TRACK_RANGES_CSV"

mkdir -p "$THISDIR/plots/pu"

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
         OUTPUT_BASE="$THISDIR/plots/pu/${SYSTEM}_${OFFICIAL_TAG}_ZPT${ZPT}_trkPT${TRACK}-PU"
         if [ "$SYSTEM" = "pPbPbp" ]; then
            ./ExecutePlotPUComparison \
               --NominalPPb "$ANALYSISDIR/pPb_trkResidual_${OFFICIAL_TAG}_ZPT${ZPT}-nosub.root" \
               --NominalPBP "$ANALYSISDIR/PbP_trkResidual_${OFFICIAL_TAG}_ZPT${ZPT}-nosub.root" \
               --VariationPPb "$ANALYSISDIR/pPb_trkResidual_${OFFICIAL_TAG}_IsPURejectTrue_ZPT${ZPT}-nosub.root" \
               --VariationPBP "$ANALYSISDIR/PbP_trkResidual_${OFFICIAL_TAG}_IsPURejectTrue_ZPT${ZPT}-nosub.root" \
               --OutputBase "$OUTPUT_BASE" \
               --Collision "$SYSTEM" \
               --ZPTRange "$ZPT" \
               --TrackPTRange "$TRACK"
         else
            NOMINAL_FILE="$ANALYSISDIR/${PREFIX}_trkResidual_${OFFICIAL_TAG}_ZPT${ZPT}-result.root"
            VARIATION_FILE="$ANALYSISDIR/${PREFIX}_trkResidual_${OFFICIAL_TAG}_IsPURejectTrue_ZPT${ZPT}-result.root"
            ./ExecutePlotPUComparison \
               --Nominal "$NOMINAL_FILE" \
               --Variation "$VARIATION_FILE" \
               --OutputBase "$OUTPUT_BASE" \
               --Collision "$SYSTEM" \
               --ZPTRange "$ZPT" \
               --TrackPTRange "$TRACK"
         fi
      done
   done
done
