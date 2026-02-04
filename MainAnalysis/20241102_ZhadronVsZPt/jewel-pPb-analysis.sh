#!/bin/bash

# Define common arguments
source clean.sh
source config.sh
COMMON_ARGS="--IsPP false --IsPPb true --IsGenZ true --IsData false --IsJewel true\
 --Input /home/kdeverea/PhysicsZHadronEEC/SampleGeneration/20240607_PredictionConversion/output/pPb/merged.root \
 --MixFile /home/kdeverea/PhysicsZHadronEEC/SampleGeneration/20240607_PredictionConversion/output/pPb/merged.root \
 --UseEventWeight true -UseZWeight false \
 --UseTrackWeight true --UseResidualWeight false \
 --yBoost 0 --nMix 10"

for zpt_range in "${ZPT_RANGES[@]}"; do
   min_zpt=${zpt_range/_*/}
   max_zpt=${zpt_range/*_/}

   for pt_range in "${PT_RANGES[@]}"; do
      min_pt=${pt_range/_*/}
      max_pt=${pt_range/*_/}

      echo ./finalAnalysis.sh "output/jewelPPBMC_ZPT${min_zpt}_${max_zpt}" "$pt_range" "$2" "$3" "$4" "$5" "$6" "$7" "$8" "$9" "${10}" "${11}" "${12}" "${13}" "${14}" "${15}" "${16}" "${17}" "${18}" "${19}" "${20}" "${21}" "${22}" "${23}" "${24}" "${25}" "${26}" "${27}" "${28}" "${29}" "${30}"\
         $COMMON_ARGS --MinTrackPT "$min_pt" --MaxTrackPT "$max_pt" \
         --MinZPT "$min_zpt" --MaxZPT "$max_zpt" | bash
   done

   # Combine results for the current HiBin and ZPT range
   hadd -f "plots/jewelPPBMC_ZPT${min_zpt}_${max_zpt}-result.root" \
       $(for pt_range in "${PT_RANGES[@]}"; do
            echo "output/jewelPPBMC_ZPT${min_zpt}_${max_zpt}-${pt_range}-result.root"
         done)
   hadd -f "plots/jewelPPBMC_ZPT${min_zpt}_${max_zpt}-nosub.root" \
       $(for pt_range in "${PT_RANGES[@]}"; do
            echo "output/jewelPPBMC_ZPT${min_zpt}_${max_zpt}-${pt_range}-nosub.root"
         done)
done
