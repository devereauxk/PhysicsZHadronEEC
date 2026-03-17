make ExecutePPPlot

ZPT_RANGES=("40_350")
PT_RANGES=("1_2" "2_4" "4_10")
PP_TAGS=("evtWeightOn_ZV6_trkV24_nmix10" "evtWeightOff_ZV6_trkV24_nmix10")

for tag in "${PP_TAGS[@]}"
do
    for zPtRange in "${ZPT_RANGES[@]}"
    do
        for trkPtRange in "${PT_RANGES[@]}"
        do
            echo "Processing tag: $tag, zPtRange: $zPtRange, trkPtRange: $trkPtRange"

            ./ExecutePPPlot --zPtRange $zPtRange --trkPtRange $trkPtRange --pptag $tag

        done
    done
done

exit
