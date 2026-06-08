#!/bin/bash

set -e

THISDIR=$(cd "$(dirname "$0")" && pwd)
source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialProductDictionary.sh
ANALYSISDIR="${OFFICIAL_RESULT_DIR}"

cd "$THISDIR"

ZPT_RANGES_CSV=${ZPT_RANGES:-0_500}
TRACK_RANGES_CSV=${TRACK_RANGES:-0.5_15}
OFFICIAL_TAG="$OFFICIAL_TAG_PPB"

IFS=',' read -ra ZPT_ARRAY <<< "$ZPT_RANGES_CSV"
IFS=',' read -ra TRACK_ARRAY <<< "$TRACK_RANGES_CSV"

mkdir -p "$THISDIR/plots/eventMixing"

for ZPT in "${ZPT_ARRAY[@]}"; do
   for TRACK in "${TRACK_ARRAY[@]}"; do
      OUTPUT_BASE="$THISDIR/plots/eventMixing/pPbPbp_${OFFICIAL_TAG}_ZPT${ZPT}_trkPT${TRACK}-EventMixing"

      ./ExecutePlotEventMixingComparison \
         --NominalPPb "$ANALYSISDIR/pPb_trkResidual_${OFFICIAL_TAG}_ZPT${ZPT}-nosub.root" \
         --NominalPBP "$ANALYSISDIR/PbP_trkResidual_${OFFICIAL_TAG}_ZPT${ZPT}-nosub.root" \
         --MEWeightFile "$OFFICIAL_ME_WEIGHT_FILE" \
         --OutputBase "$OUTPUT_BASE" \
         --Collision pPbPbp \
         --ZPTRange "$ZPT" \
         --TrackPTRange "$TRACK"
   done
done
