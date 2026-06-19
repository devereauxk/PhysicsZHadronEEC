#!/bin/bash

set -e

THISDIR=$(cd "$(dirname "$0")" && pwd)
source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialProductDictionary.sh
ANALYSISDIR="${OFFICIAL_RESULT_DIR}"

cd "$THISDIR"

SYSTEMS_CSV=${SYSTEMS:-pp,pPb,PbP,pPbPbp}
ZPT_RANGES_CSV=${ZPT_RANGES:-5_500}
TRACK_RANGES_CSV=${TRACK_RANGES:-0.5_500}

IFS=',' read -ra SYSTEMS_ARRAY <<< "$SYSTEMS_CSV"
IFS=',' read -ra ZPT_ARRAY <<< "$ZPT_RANGES_CSV"
IFS=',' read -ra TRACK_ARRAY <<< "$TRACK_RANGES_CSV"

mkdir -p "$THISDIR/plots/trackCorrection"

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
         OUTPUT_BASE="$THISDIR/plots/trackCorrection/${SYSTEM}_${OFFICIAL_TAG}_ZPT${ZPT}_trkPT${TRACK}-TrackCorrection"
         if [ "$SYSTEM" = "pPbPbp" ]; then
            ./ExecutePlotTrackCorrectionDiff \
               --NominalPPb "$ANALYSISDIR/pPb_trkResidual_${OFFICIAL_TAG}_ZPT${ZPT}-nosub.root" \
               --NominalPBP "$ANALYSISDIR/PbP_trkResidual_${OFFICIAL_TAG}_ZPT${ZPT}-nosub.root" \
               --DownPPb "$ANALYSISDIR/pPb_trkResidual_${OFFICIAL_TAG}_TrackCorrection0p976_ZPT${ZPT}-nosub.root" \
               --DownPBP "$ANALYSISDIR/PbP_trkResidual_${OFFICIAL_TAG}_TrackCorrection0p976_ZPT${ZPT}-nosub.root" \
               --UpPPb "$ANALYSISDIR/pPb_trkResidual_${OFFICIAL_TAG}_TrackCorrection1p024_ZPT${ZPT}-nosub.root" \
               --UpPBP "$ANALYSISDIR/PbP_trkResidual_${OFFICIAL_TAG}_TrackCorrection1p024_ZPT${ZPT}-nosub.root" \
               --OutputBase "$OUTPUT_BASE" \
               --Collision "$SYSTEM" \
               --ZPTRange "$ZPT" \
               --TrackPTRange "$TRACK" \
               --UseModified12x12 true
         else
            NOMINAL_FILE="$ANALYSISDIR/${PREFIX}_trkResidual_${OFFICIAL_TAG}_ZPT${ZPT}-result.root"
            DOWN_FILE="$ANALYSISDIR/${PREFIX}_trkResidual_${OFFICIAL_TAG}_TrackCorrection0p976_ZPT${ZPT}-result.root"
            UP_FILE="$ANALYSISDIR/${PREFIX}_trkResidual_${OFFICIAL_TAG}_TrackCorrection1p024_ZPT${ZPT}-result.root"
            ./ExecutePlotTrackCorrectionDiff \
               --Nominal "$NOMINAL_FILE" \
               --Down "$DOWN_FILE" \
               --Up "$UP_FILE" \
               --OutputBase "$OUTPUT_BASE" \
               --Collision "$SYSTEM" \
               --ZPTRange "$ZPT" \
               --TrackPTRange "$TRACK" \
               --UseModified12x12 true
         fi
      done
   done
done
