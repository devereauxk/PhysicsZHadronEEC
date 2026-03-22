make ExecuteEnergyExtrapolationPlot

ZPT_RANGES=("5_30" "30_500")
PT_RANGES=("0.5_4" "4_500")

for zPtRange in "${ZPT_RANGES[@]}"
do
    for trkPtRange in "${PT_RANGES[@]}"
    do
        echo "Processing zPtRange: $zPtRange, trkPtRange: $trkPtRange"

        ./ExecuteEnergyExtrapolationPlot --zPtRange $zPtRange --trkPtRange $trkPtRange --pptag ZV6_trkV24_EEV3_nmix10

    done
done

./ExecuteEnergyExtrapolationPlot --zPtRange 5_500 --trkPtRange 0.5_500 --pptag ZV6_trkV24_EEV3_nmix10

exit
