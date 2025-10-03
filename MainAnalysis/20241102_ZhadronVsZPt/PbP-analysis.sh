#!/bin/bash

# Define common arguments
source clean.sh
source config.sh
COMMON_ARGS="--UseLeadingTrk $UseLeadingTrk --Input pPbSample/PPbData_Reco.root --MixFile pPbSample/PPbData_Reco.root --IsPP false --IsGenZ false --nMix 10 --IsData true --IsPPb false --yBoost $yBoost --UseTrackWeight $UseTrackWeight --UseEventWeight $UseEventWeight"

for zpt_range in "${ZPT_RANGES[@]}"; do
   min_zpt=${zpt_range/_*/}
   max_zpt=${zpt_range/*_/}

   for pt_range in "${PT_RANGES[@]}"; do
      min_pt=${pt_range/_*/}
      max_pt=${pt_range/*_/}

      echo ./finalAnalysis.sh "output/PbP_${TAG}_ZPT${min_zpt}_${max_zpt}" "$pt_range" "$2" "$3" "$4" "$5" "$6" "$7" "$8" "$9" "${10}" "${11}" \
         $COMMON_ARGS --MinTrackPT "$min_pt" --MaxTrackPT "$max_pt" \
         --MinZPT "$min_zpt" --MaxZPT "$max_zpt" | bash
   done

   # Combine results for the current HiBin and ZPT range
      hadd -f "plots/PbP_${TAG}_ZPT${min_zpt}_${max_zpt}-result.root" \
         $(for pt_range in "${PT_RANGES[@]}"; do
            echo "output/PbP_${TAG}_ZPT${min_zpt}_${max_zpt}-${pt_range}-result.root"
          done)
      hadd -f "plots/PbP_${TAG}_ZPT${min_zpt}_${max_zpt}-nosub.root" \
         $(for pt_range in "${PT_RANGES[@]}"; do
            echo "output/PbP_${TAG}_ZPT${min_zpt}_${max_zpt}-${pt_range}-nosub.root"
          done)

done      
