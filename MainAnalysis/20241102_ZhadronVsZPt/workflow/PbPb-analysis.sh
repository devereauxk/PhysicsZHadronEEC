#!/bin/bash

# Define common arguments
COMMON_ARGS="--Input mergedSample/HISingleMuon-v6.root --MixFile mergedSample/HISingleMuon-v6.root --IsPP false --IsGenZ false --nMix 10"
PT_RANGES=("1_2" "2_4" "4_10")
HIBIN_RANGES=("0_50")
ZPT_RANGES=("30_40" "40_60" "60_100" "100_350")

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

         ./finalAnalysis.sh "output/$1PbPb${suffix}_ZPT${min_zpt}_${max_zpt}" "$pt_range" "$2" "$3" "$4" "$5" "$6" "$7" "$8" "$9" "${10}" "${11}" \
            $COMMON_ARGS --MinTrackPT "$min_pt" --MaxTrackPT "$max_pt" --MinHiBin "$min_hibinX2" --MaxHiBin "$max_hibinX2" \
            --MinZPT "$min_zpt" --MaxZPT "$max_zpt"
      done

      # Combine results for the current HiBin and ZPT range
      hadd -f "plots/$1PbPb${suffix}_ZPT${min_zpt}_${max_zpt}-result.root" \
         "output/$1PbPb${suffix}_ZPT${min_zpt}_${max_zpt}-1_2-result.root" \
         "output/$1PbPb${suffix}_ZPT${min_zpt}_${max_zpt}-2_4-result.root" \
         "output/$1PbPb${suffix}_ZPT${min_zpt}_${max_zpt}-4_10-result.root"
   done
done
