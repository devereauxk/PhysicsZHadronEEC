#!/bin/bash

set -e

THISDIR=$(cd "$(dirname "$0")" && pwd)
source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialProductDictionary.sh
ANALYSISDIR="${OFFICIAL_RESULT_DIR}"

cd "$THISDIR"

SYSTEMS_CSV=${SYSTEMS:-pPb,PbP,pPbPbp}
ZPT_RANGES_CSV=${ZPT_RANGES:-5_500}
TRACK_RANGES_CSV=${TRACK_RANGES:-0.5_500}

IFS=',' read -ra SYSTEMS_ARRAY <<< "$SYSTEMS_CSV"
IFS=',' read -ra ZPT_ARRAY <<< "$ZPT_RANGES_CSV"
IFS=',' read -ra TRACK_ARRAY <<< "$TRACK_RANGES_CSV"

mkdir -p "$THISDIR/plots/trackSelection"

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
         OUTPUT_BASE="$THISDIR/plots/trackSelection/${SYSTEM}_${OFFICIAL_TAG}_ZPT${ZPT}_trkPT${TRACK}-TrackSelection"
         if [ "$SYSTEM" = "pPbPbp" ]; then
            ./ExecutePlotTrackSelectionDiff \
               --NominalPPb "$ANALYSISDIR/pPb_trkResidual_${OFFICIAL_TAG}_ZPT${ZPT}-nosub.root" \
               --NominalPBP "$ANALYSISDIR/PbP_trkResidual_${OFFICIAL_TAG}_ZPT${ZPT}-nosub.root" \
               --LoosePPb "$ANALYSISDIR/pPb_trkResidual_${OFFICIAL_TAG}_Loose_ZPT${ZPT}-nosub.root" \
               --LoosePBP "$ANALYSISDIR/PbP_trkResidual_${OFFICIAL_TAG}_Loose_ZPT${ZPT}-nosub.root" \
               --TightPPb "$ANALYSISDIR/pPb_trkResidual_${OFFICIAL_TAG}_Tight_ZPT${ZPT}-nosub.root" \
               --TightPBP "$ANALYSISDIR/PbP_trkResidual_${OFFICIAL_TAG}_Tight_ZPT${ZPT}-nosub.root" \
               --OutputBase "$OUTPUT_BASE" \
               --Collision "$SYSTEM" \
               --ZPTRange "$ZPT" \
               --TrackPTRange "$TRACK"
         else
            NOMINAL_FILE="$ANALYSISDIR/${PREFIX}_trkResidual_${OFFICIAL_TAG}_ZPT${ZPT}-result.root"
            LOOSE_FILE="$ANALYSISDIR/${PREFIX}_trkResidual_${OFFICIAL_TAG}_Loose_ZPT${ZPT}-result.root"
            TIGHT_FILE="$ANALYSISDIR/${PREFIX}_trkResidual_${OFFICIAL_TAG}_Tight_ZPT${ZPT}-result.root"
            ./ExecutePlotTrackSelectionDiff \
               --Nominal "$NOMINAL_FILE" \
               --Loose "$LOOSE_FILE" \
               --Tight "$TIGHT_FILE" \
               --OutputBase "$OUTPUT_BASE" \
               --Collision "$SYSTEM" \
               --ZPTRange "$ZPT" \
               --TrackPTRange "$TRACK"
         fi
      done
   done
done
