#!/bin/bash

# Define common arguments
source config.sh
COMMON_ARGS="--UseLeadingTrk $UseLeadingTrk --Input mergedSample/hybrid_pp-v8.root --MixFile mergedSample/hybrid_pp-v8.root --IsPP true --IsGenZ true --nMix 10 --MinHiBin 0 --MaxHiBin 600000"

for zpt_range in "${ZPT_RANGES[@]}"; do
   min_zpt=${zpt_range/_*/}
   max_zpt=${zpt_range/*_/}

   for pt_range in "${PT_RANGES[@]}"; do
      min_pt=${pt_range/_*/}
      max_pt=${pt_range/*_/}

      echo ./finalAnalysis.sh "output/$1hybridPP_ZPT${min_zpt}_${max_zpt}" "$pt_range" "$2" "$3" "$4" "$5" "$6" "$7" "$8" "$9" "${10}" "${11}" \
         $COMMON_ARGS --MinTrackPT "$min_pt" --MaxTrackPT "$max_pt" \
         --MinZPT "$min_zpt" --MaxZPT "$max_zpt"|bash
   done

   # Combine results for the current HiBin and ZPT range
   hadd -f "plots/$1hybridPP_ZPT${min_zpt}_${max_zpt}-result.root" \
      "output/$1hybridPP_ZPT${min_zpt}_${max_zpt}-1_2-result.root" \
      "output/$1hybridPP_ZPT${min_zpt}_${max_zpt}-2_4-result.root" \
      "output/$1hybridPP_ZPT${min_zpt}_${max_zpt}-4_10-result.root"
done
