make ExecuteEnergyExtrapolationPlot

#ZPT_RANGES=("5_30" "30_500")
#PT_RANGES=("0.5_4" "0.7_4" "4_500")
ZPT_RANGES=("5_500")
PT_RANGES=("0.5_500")

for zPtRange in "${ZPT_RANGES[@]}"
do
    for trkPtRange in "${PT_RANGES[@]}"
    do
        echo "Processing zPtRange: $zPtRange, trkPtRange: $trkPtRange"

        ./ExecuteEnergyExtrapolationPlot --zPtRange $zPtRange --trkPtRange $trkPtRange --pptag EEV2_ZV4_trkV22_nmix10

    done
done

exit
