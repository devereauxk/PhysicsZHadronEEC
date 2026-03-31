#!/bin/bash

set -e

THISDIR=$(cd "$(dirname "$0")" && pwd)
ANALYSISDIR=/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh

cd "$THISDIR"

SYSTEMS_CSV=${SYSTEMS:-pp,pPb,PbP}
ZPT_RANGES_CSV=${ZPT_RANGES:-40_350}
TRACK_RANGES_CSV=${TRACK_RANGES:-2_500}

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
      NOMINAL_FILE="$ANALYSISDIR/${PREFIX}_trkResidual_${OFFICIAL_TAG}_ZPT${ZPT}-result.root"
      LOOSE_FILE="$ANALYSISDIR/${PREFIX}_trkResidual_${OFFICIAL_TAG}_Loose_ZPT${ZPT}-result.root"
      TIGHT_FILE="$ANALYSISDIR/${PREFIX}_trkResidual_${OFFICIAL_TAG}_Tight_ZPT${ZPT}-result.root"

      for TRACK in "${TRACK_ARRAY[@]}"; do
         OUTPUT_BASE="$THISDIR/plots/trackSelection/${SYSTEM}_${OFFICIAL_TAG}_ZPT${ZPT}_trkPT${TRACK}-TrackSelection"

         ./ExecutePlotTrackSelectionDiff \
            --Nominal "$NOMINAL_FILE" \
            --Loose "$LOOSE_FILE" \
            --Tight "$TIGHT_FILE" \
            --OutputBase "$OUTPUT_BASE" \
            --Collision "$SYSTEM" \
            --ZPTRange "$ZPT" \
            --TrackPTRange "$TRACK"
      done
   done
done
