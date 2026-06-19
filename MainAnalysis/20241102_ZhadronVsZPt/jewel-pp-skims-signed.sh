#!/bin/bash
# Jewel pp 5.02 TeV comparison in signed common-CM convention.
# Runs local pp production and v9 (PbPb-study) pp skim with
# FillSigned, DEtaRange 3.87, 12x12 bins, |eta|<1.935.
set -euo pipefail

export SKIP_CLEAN=1

JEWEL_DIR=$ProjectBase/SampleGeneration/20260521_Jewel

ZPT_RANGES=("0_500")
PT_RANGES=("0.5_15")

COMMON_ARGS="--IsPP true --IsGenZ true --IsData false --IsJewel true \
 --UseEventWeight true --UseZWeight false \
 --UseTrackWeight true --UseResidualWeight false \
 --yBoost 0 --nMix 10 \
 --FillSigned true --DEtaRange 3.87 \
 --ResultDEtaBins 12 --ResultDPhiBins 12 \
 --MaxMixDeltaVZ 1.0 \
 --TrackEtaMin -1.935 --TrackEtaMax 1.935 \
 --ZYSignedMin -1.935 --ZYSignedMax 1.935"

run_sample() {
   local INPUT="$1"
   local PREFIX="$2"

   if [ ! -f "$INPUT" ]; then
      echo "Missing $INPUT — skipping"
      return 1
   fi

   echo "=== Running $PREFIX from $INPUT ==="

   for zpt_range in "${ZPT_RANGES[@]}"; do
      min_zpt=${zpt_range/_*/}
      max_zpt=${zpt_range/*_/}

      for pt_range in "${PT_RANGES[@]}"; do
         min_pt=${pt_range/_*/}
         max_pt=${pt_range/*_/}

         ./threader.sh "output/${PREFIX}_ZPT${min_zpt}_${max_zpt}" "$pt_range" \
            $COMMON_ARGS \
            --Input "$INPUT" --MixFile "$INPUT" \
            --MinTrackPT "$min_pt" --MaxTrackPT "$max_pt" \
            --MinZPT "$min_zpt" --MaxZPT "$max_zpt"
      done

      hadd -f "plots/${PREFIX}_ZPT${min_zpt}_${max_zpt}-result.root" \
          $(for pt_range in "${PT_RANGES[@]}"; do
               echo "output/${PREFIX}_ZPT${min_zpt}_${max_zpt}-${pt_range}-result.root"
            done)
      hadd -f "plots/${PREFIX}_ZPT${min_zpt}_${max_zpt}-nosub.root" \
          $(for pt_range in "${PT_RANGES[@]}"; do
               echo "output/${PREFIX}_ZPT${min_zpt}_${max_zpt}-${pt_range}-nosub.root"
            done)
   done

   echo "=== Done: $PREFIX ==="
}

run_sample "$JEWEL_DIR/output/jewel_pp_5020.root" "jewelPP5020signed"
run_sample "$ProjectBase/MainAnalysis/20241102_ZhadronVsZPt/mergedSample/jewel_pp-v9.root" "jewelPPv9signed"
