#!/bin/bash

# Define common arguments
source config.sh
COMMON_ARGS="--UseLeadingTrk $UseLeadingTrk --Input mergedSample/HISingleMuon-v11-Zpt0.root --MixFile mergedSample/HISingleMuon-v11-Zpt0.root --IsPP false --IsGenZ false --nMix 10"

# Loop over HiBin ranges, Pt ranges, and ZPT ranges, running analysis and combining results
for hibin in "${HIBIN_RANGES[@]}"; do
   suffix=$hibin
   min_hibin=${hibin/_*/}
   max_hibin=${hibin/*_/}
   min_hibinX2=$((2 * min_hibin))
   max_hibinX2=$((2 * max_hibin))

   for zpt_range in "${ZPT_RANGES[@]}"; do
      min_zpt=${zpt_range/_*/}
      max_zpt=${zpt_range/*_/}

      for pt_range in "${PT_RANGES[@]}"; do
         min_pt=${pt_range/_*/}
         max_pt=${pt_range/*_/}

         ./finalAnalysis.sh "output/$1PbPb${suffix}_ZPT${min_zpt}_${max_zpt}" "$pt_range" "$2" "$3" "$4" "$5" "$6" "$7" --MinZPT "$min_zpt" --MaxZPT "$max_zpt" $COMMON_ARGS --MinTrackPT "$min_pt" --MaxTrackPT "$max_pt" --MinHiBin "$min_hibinX2" --MaxHiBin "$max_hibinX2" 
      done

      # Combine results for the current HiBin and ZPT range
      #hadd -f "plots/$1PbPb${suffix}_ZPT${min_zpt}_${max_zpt}-result.root" \
         #$(for pt_range in "${PT_RANGES[@]}"; do
         #   echo "output/$1PbPb${suffix}_ZPT${min_zpt}_${max_zpt}-${pt_range}-result.root"
         #done)
   done
done


# ./finalAnalysis.sh output/$1PbPb0_30 4_20  $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} --MinZPT 40 --MaxZPT 350 --MinTrackPT 1 --MaxTrackPT 40  --MinHiBin 0 --MaxHiBin 60  --Input mergedSample/HISingleMuon-v11-Zpt0.root --IsSelfMixing false --IsPP false --IsGenZ false --UseResidualCor true --ResidualCor residualCorrection2023/20241022_2023_TrackResidualCorrection_V13
#root -l -q -b "plotBasic.C(\"output/$1PbPb0_30-4_20.root\",\"plots/$1PbPb0_30\")"