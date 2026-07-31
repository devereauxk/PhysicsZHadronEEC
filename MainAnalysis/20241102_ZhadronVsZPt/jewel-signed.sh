#!/bin/bash
# Jewel analysis in signed common-CM convention (12x12, DEtaRange 3.87).
# MODE=pp  : pp 8.16 TeV (~2M events)
# MODE=pPb : pPb 8.16 TeV (forthcoming)
#
# Usage:
#   MODE=pp  NTHREAD=30 bash jewel-signed.sh
#   MODE=pPb NTHREAD=30 bash jewel-signed.sh
set -euo pipefail

export SKIP_CLEAN=1
export NTHREAD=${NTHREAD:-30}

MODE="${MODE:-pp}"

ZPT_RANGES=("0_500")
PT_RANGES=("0.5_15")

COMMON_ARGS="--IsGenZ true --IsData false --IsJewel true \
 --UseEventWeight true --UseZWeight false \
 --UseTrackWeight false --UseResidualWeight false \
 --yBoost 0 --nMix 10 \
 --FillSigned true --DEtaRange 3.87 \
 --ResultDEtaBins 12 --ResultDPhiBins 12 \
 --MaxMixDeltaVZ 1.0 \
 --TrackEtaMin -1.935 --TrackEtaMax 1.935 \
 --ZYSignedMin -1.935 --ZYSignedMax 1.935"

run_sample() {
   local INPUT="$1"
   local PREFIX="$2"
   local EXTRA_ARGS="$3"

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
            $COMMON_ARGS $EXTRA_ARGS \
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

case "$MODE" in
  pp)
    run_sample ~/jewel/jewel_pp8160_2M.root "jewelPP8160signed" "--IsPP true"
    ;;
  ppMOD)
    run_sample ~/jewel/jewel_pp8160v3_MOD_500k.root "jewelPP8160signed_MOD" "--IsPP true"
    ;;
  pPb)
    echo "pPb Jewel input forthcoming — not yet available."
    exit 0
    ;;
  pPbMOD)
    run_sample ~/jewel/jewel_pPb_v2_MOD_500k.root "jewelPPb8160signed_MOD" "--IsPP false"
    ;;
  *)
    echo "Unknown MODE=$MODE — use pp, pPb, ppMOD, or pPbMOD"
    exit 1
    ;;
esac
