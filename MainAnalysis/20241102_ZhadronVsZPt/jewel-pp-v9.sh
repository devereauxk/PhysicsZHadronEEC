#!/bin/bash
set -euo pipefail

export SKIP_CLEAN=1
source config.sh

INPUT="$ProjectBase/MainAnalysis/20241102_ZhadronVsZPt/mergedSample/jewel_pp-v9.root"
if [ ! -f "$INPUT" ]; then
   echo "Missing $INPUT"
   exit 1
fi

COMMON_ARGS="--IsPP true --IsGenZ true --IsData false --IsJewel true \
 --UseEventWeight true --UseZWeight false \
 --UseTrackWeight true --UseResidualWeight false \
 --yBoost 0 --nMix 10"

for zpt_range in "${ZPT_RANGES[@]}"; do
   min_zpt=${zpt_range/_*/}
   max_zpt=${zpt_range/*_/}

   for pt_range in "${PT_RANGES[@]}"; do
      min_pt=${pt_range/_*/}
      max_pt=${pt_range/*_/}

      ./threader.sh "output/jewelPPv9_ZPT${min_zpt}_${max_zpt}" "$pt_range" \
         $COMMON_ARGS \
         --Input "$INPUT" --MixFile "$INPUT" \
         --MinTrackPT "$min_pt" --MaxTrackPT "$max_pt" \
         --MinZPT "$min_zpt" --MaxZPT "$max_zpt"
   done

   hadd -f "plots/jewelPPv9_ZPT${min_zpt}_${max_zpt}-result.root" \
       $(for pt_range in "${PT_RANGES[@]}"; do
            echo "output/jewelPPv9_ZPT${min_zpt}_${max_zpt}-${pt_range}-result.root"
         done)
   hadd -f "plots/jewelPPv9_ZPT${min_zpt}_${max_zpt}-nosub.root" \
       $(for pt_range in "${PT_RANGES[@]}"; do
            echo "output/jewelPPv9_ZPT${min_zpt}_${max_zpt}-${pt_range}-nosub.root"
         done)
done
