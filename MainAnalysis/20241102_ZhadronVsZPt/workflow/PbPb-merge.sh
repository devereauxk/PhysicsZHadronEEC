#!/bin/bash

# Define common arguments
. config.sh

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

         cd output
         hadd -f $1PbPbMerged${suffix}_ZPT${min_zpt}_${max_zpt}-${min_pt}_${max_pt}.root $1PbPb${suffix}_ZPT${min_zpt}_${max_zpt}-${min_pt}_${max_pt}.root $1PbPb2023${suffix}_ZPT${min_zpt}_${max_zpt}-${min_pt}_${max_pt}.root
         root -l -q -b "../makeProjection.C(\"$1PbPbMerged${suffix}_ZPT${min_zpt}_${max_zpt}-${min_pt}_${max_pt}.root\",\"$1PbPbMerged${suffix}_ZPT${min_zpt}_${max_zpt}-${min_pt}_${max_pt}-result.root\",\"${min_pt}_${max_pt}\", 1)"
      done

      # Combine results for the current HiBin and ZPT range
      cd ..
      hadd -f "plots/$1PbPbMerged${suffix}_ZPT${min_zpt}_${max_zpt}-result.root" \
         "output/$1PbPbMerged${suffix}_ZPT${min_zpt}_${max_zpt}-1_2-result.root" \
         "output/$1PbPbMerged${suffix}_ZPT${min_zpt}_${max_zpt}-2_4-result.root" \
         "output/$1PbPbMerged${suffix}_ZPT${min_zpt}_${max_zpt}-4_10-result.root"
   done
done
