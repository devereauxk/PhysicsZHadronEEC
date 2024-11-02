#!/bin/bash

# Define common arguments
COMMON_ARGS="--Input mergedSample/pp-v6.root --MixFile mergedSample/pp-v6.root --IsPP true --IsGenZ false --nMix 10"
PT_RANGES=("1_2" "2_4" "4_10")
ZPT_RANGES=("30_40" "40_60" "60_100" "100_350")

for zpt_range in "${ZPT_RANGES[@]}"; do
   min_zpt=${zpt_range/_*/}
   max_zpt=${zpt_range/*_/}

   for pt_range in "${PT_RANGES[@]}"; do
      min_pt=${pt_range/_*/}
      max_pt=${pt_range/*_/}

      ./finalAnalysis.sh "output/$1pp_ZPT${min_zpt}_${max_zpt}" "$pt_range" "$2" "$3" "$4" "$5" "$6" "$7" "$8" "$9" "${10}" "${11}" \
         $COMMON_ARGS --MinTrackPT "$min_pt" --MaxTrackPT "$max_pt" \
         --MinZPT "$min_zpt" --MaxZPT "$max_zpt"
   done

   # Combine results for the current HiBin and ZPT range
   hadd -f "plots/$1pp_ZPT${min_zpt}_${max_zpt}-result.root" \
      "output/$1pp_ZPT${min_zpt}_${max_zpt}-1_2-result.root" \
      "output/$1pp_ZPT${min_zpt}_${max_zpt}-2_4-result.root" \
      "output/$1pp_ZPT${min_zpt}_${max_zpt}-4_10-result.root"
done
