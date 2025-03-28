#!/bin/bash

# Define common arguments
source config.sh
#COMMON_ARGS="--UseLeadingTrk $UseLeadingTrk --Input mergedSample/pp-v10-trackPt05.root --MixFile mergedSample/pp-v10-trackPt05.root --IsPP true --IsGenZ false --nMix 1"
COMMON_ARGS="--UseLeadingTrk $UseLeadingTrk --Input mergedSample/pp-v11-Zpt0.root --MixFile mergedSample/pp-v11-Zpt0.root --IsPP true --IsGenZ false --nMix 1"

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
       hadd -f "plots/$1pp${suffix}_ZPT${min_zpt}_${max_zpt}-result.root" \
         $(for pt_range in "${PT_RANGES[@]}"; do
            echo "output/$1pp${suffix}_ZPT${min_zpt}_${max_zpt}-${pt_range}-result.root"
         done)

done      
