#!/bin/bash

set -e

THISDIR=$(cd "$(dirname "$0")" && pwd)
ANALYSISDIR=/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh

cd "$THISDIR"

SYSTEMS_CSV=${SYSTEMS:-pp}
ZPT_RANGES_CSV=${ZPT_RANGES:-5_500}
TRACK_RANGES_CSV=${TRACK_RANGES:-0.5_500}

IFS=',' read -ra SYSTEMS_ARRAY <<< "$SYSTEMS_CSV"
IFS=',' read -ra ZPT_ARRAY <<< "$ZPT_RANGES_CSV"
IFS=',' read -ra TRACK_ARRAY <<< "$TRACK_RANGES_CSV"

mkdir -p "$THISDIR/plots/energyExtrapolation"

for SYSTEM in "${SYSTEMS_ARRAY[@]}"; do
   if [ "$SYSTEM" != "pp" ]; then
      continue
   fi

   PREFIX="pp"
   OFFICIAL_TAG="$OFFICIAL_TAG_PP"

   for ZPT in "${ZPT_ARRAY[@]}"; do
      for TRACK in "${TRACK_ARRAY[@]}"; do
         NOMINAL_FILE="$ANALYSISDIR/${PREFIX}_trkResidual_${OFFICIAL_TAG}_ZPT${ZPT}-result.root"
         VARIATION_FILE="$ANALYSISDIR/${PREFIX}_trkResidual_${OFFICIAL_TAG}_EEPrivate_ZPT${ZPT}-result.root"
         OUTPUT_BASE="$THISDIR/plots/energyExtrapolation/${SYSTEM}_${OFFICIAL_TAG}_ZPT${ZPT}_trkPT${TRACK}-EnergyExtrapolation"
         ./ExecutePlotEnergyExtrapolationComparison \
            --Nominal "$NOMINAL_FILE" \
            --Variation "$VARIATION_FILE" \
            --OutputBase "$OUTPUT_BASE" \
            --Collision "$SYSTEM" \
            --ZPTRange "$ZPT" \
            --TrackPTRange "$TRACK"
      done
   done
done
