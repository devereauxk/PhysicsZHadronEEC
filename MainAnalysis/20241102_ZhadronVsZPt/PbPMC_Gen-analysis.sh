#!/bin/bash

# Define common arguments
source clean.sh
source config.sh
COMMON_ARGS="--Input pPbSample/V0.2/PbPMC_Gen.root --MixFile pPbSample/V0.2/PbPMC_Gen.root --IsPP false --IsGenZ true --IsData false --IsPPb false"

for zpt_range in "${ZPT_RANGES[@]}"; do
   min_zpt=${zpt_range/_*/}
   max_zpt=${zpt_range/*_/}

   echo "max_zpt : $max_zpt" 

   for pt_range in "${PT_RANGES[@]}"; do
      min_pt=${pt_range/_*/}
      max_pt=${pt_range/*_/}

      echo ./finalAnalysis.sh "output/PbPMC_Gen_${1}_ZPT${min_zpt}_${max_zpt}" "$pt_range" "$1" "$2" "$3" "$4" "$5" "$6" "$7" "$8" "$9" "${10}" "${11}" "${12}" "${13}" "${14}" "${15}" "${16}" "${17}" "${18}" "${19}" "${20}" \
         $COMMON_ARGS --MinTrackPT "$min_pt" --MaxTrackPT "$max_pt" \
         --MinZPT "$min_zpt" --MaxZPT "$max_zpt"|bash
   done

   # Combine results for the current HiBin and ZPT range
   hadd -f "plots/PbPMC_Gen_${1}_ZPT${min_zpt}_${max_zpt}-result.root" \
       $(for pt_range in "${PT_RANGES[@]}"; do
            echo "output/PbPMC_Gen_${1}_ZPT${min_zpt}_${max_zpt}-${pt_range}-result.root"
         done)
   hadd -f "plots/PbPMC_Gen_${1}_ZPT${min_zpt}_${max_zpt}-nosub.root" \
       $(for pt_range in "${PT_RANGES[@]}"; do
            echo "output/PbPMC_Gen_${1}_ZPT${min_zpt}_${max_zpt}-${pt_range}-nosub.root"
         done)
done
