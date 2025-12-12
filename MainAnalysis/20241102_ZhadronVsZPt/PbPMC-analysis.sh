#!/bin/bash

# Define common arguments
source clean.sh
source config.sh
#Input="pPbSample/V0.2/PPbMC_Reco.root"
#Input="pPbSample/V0.2/PbPMC_Reco.root"
Input="mergedSample/PbPMC_Reco_v0.2.root"
ResidualWeightFile="my_residualWeights/20251212_TrackResidualCorrection_V16_PbP_zPt"

COMMON_ARGS="--UseLeadingTrk $UseLeadingTrk --Input $Input --MixFile $Input --IsPP false --IsGenZ true --nMix 1 --IsData false --IsPPb false --yBoost $yBoost --UseTrackWeight $UseTrackWeight --UseEventWeight $UseEventWeight --UseResidualWeight $UseResidualWeight --ResidualWeightFile $ResidualWeightFile"

for zpt_range in "${ZPT_RANGES[@]}"; do
   min_zpt=${zpt_range/_*/}
   max_zpt=${zpt_range/*_/}

   for pt_range in "${PT_RANGES[@]}"; do
      min_pt=${pt_range/_*/}
      max_pt=${pt_range/*_/}

      echo ./finalAnalysis.sh "output/PbPMC_${TAG}_ZPT${min_zpt}_${max_zpt}" "$pt_range" "$1" "$2" \
         $COMMON_ARGS --MinTrackPT "$min_pt" --MaxTrackPT "$max_pt" --MinZPT "$min_zpt" --MaxZPT "$max_zpt" | bash
   done

   # Combine results for the current HiBin and ZPT range
   hadd -f "plots/PbPMC_${TAG}_ZPT${min_zpt}_${max_zpt}-result.root" \
       $(for pt_range in "${PT_RANGES[@]}"; do
            echo "output/PbPMC_${TAG}_ZPT${min_zpt}_${max_zpt}-${pt_range}-result.root"
         done)
   hadd -f "plots/PbPMC_${TAG}_ZPT${min_zpt}_${max_zpt}-nosub.root" \
       $(for pt_range in "${PT_RANGES[@]}"; do
            echo "output/PbPMC_${TAG}_ZPT${min_zpt}_${max_zpt}-${pt_range}-nosub.root"
         done)
done
