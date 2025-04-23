#!/bin/bash

source config.sh

# Loop over Pt ranges, and ZPT ranges, running analysis and combining results
for zpt_range in "${ZPT_RANGES[@]}"; do
    for pt_range in "${PT_RANGES[@]}"; do

        root -l -q -b "plotOverlay.C(\"$zpt_range\",\"$pt_range\")"

    done
done
