#!/bin/bash

# Define common arguments
source config.sh
COMMON_ARGS="--UseLeadingTrk $UseLeadingTrk --Input mergedSample/pythia-v11-Zpt0.root  --MixFile mergedSample/pythia-v11-Zpt0.root  --IsPP true --IsGenZ false --nMix 1"

for zpt_range in "${ZPT_RANGES[@]}"; do
   min_zpt=${zpt_range/_*/}
   max_zpt=${zpt_range/*_/}

   for pt_range in "${PT_RANGES[@]}"; do
      min_pt=${pt_range/_*/}
      max_pt=${pt_range/*_/}

      ./finalAnalysis.sh "output/$1pythia_ZPT${min_zpt}_${max_zpt}" "$pt_range" "$2" "$3" "$4" "$5" "$6" "$7" "$8" "$9" "${10}" "${11}" \
         $COMMON_ARGS --MinTrackPT "$min_pt" --MaxTrackPT "$max_pt" \
         --MinZPT "$min_zpt" --MaxZPT "$max_zpt"
   done

   # Combine results for the current HiBin and ZPT range
       #hadd -f "plots/$1pythia${suffix}_ZPT${min_zpt}_${max_zpt}-result.root" \
        # $(for pt_range in "${PT_RANGES[@]}"; do
         #   echo "output/$1pythia${suffix}_ZPT${min_zpt}_${max_zpt}-${pt_range}-result.root"
         # done)

done      

#./finalAnalysis.sh output/$1pythia 4_20   $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11}   --MinZPT 20 --MaxZPT 60 --MinTrackPT 1 --MaxTrackPT 40  --Input mergedSample/pythia-v11-Zpt0.root --IsSelfMixing false  --IsPP true --IsGenZ false
#root -l -q -b "plotBasic.C(\"output/$1pythia-4_20.root\",\"plots/$1pythia\")"