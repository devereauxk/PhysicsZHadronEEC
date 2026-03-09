make ExecutePPPlot

ZPT_RANGES=("40_350")
PT_RANGES=("1_2" "2_4" "4_10")

for zPtRange in "${ZPT_RANGES[@]}"
do
    for trkPtRange in "${PT_RANGES[@]}"
    do
        echo "Processing zPtRange: $zPtRange, trkPtRange: $trkPtRange"

        ./ExecutePPPlot --zPtRange $zPtRange --trkPtRange $trkPtRange --pptag ZV5_trkV23_nmix10

    done
done

exit
