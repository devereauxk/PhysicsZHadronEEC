#!/bin/bash

# Define common arguments
source clean.sh
source config.sh

OUTPUT_PREFIX=$1
shift
ANALYSIS_ARGS=("$@")

for zpt_range in "${ZPT_RANGES[@]}"; do
   min_zpt=${zpt_range/_*/}
   max_zpt=${zpt_range/*_/}

   for pt_range in "${PT_RANGES[@]}"; do
      min_pt=${pt_range/_*/}
      max_pt=${pt_range/*_/}

      ./threader.sh "output/${OUTPUT_PREFIX}_ZPT${min_zpt}_${max_zpt}" "$pt_range" \
         "${ANALYSIS_ARGS[@]}" \
         --MinTrackPT "$min_pt" --MaxTrackPT "$max_pt" \
         --MinZPT "$min_zpt" --MaxZPT "$max_zpt"
   done

    # Combine results for the current HiBin and ZPT range
     hadd -f "output/${OUTPUT_PREFIX}_ZPT${min_zpt}_${max_zpt}.root" \
         $(for pt_range in "${PT_RANGES[@]}"; do
             echo "output/${OUTPUT_PREFIX}_ZPT${min_zpt}_${max_zpt}-${pt_range}.root"
           done)
done
