make ExecutePUPlot

#ZPT_RANGES=("40_350")
#PT_RANGES=("1_2" "2_4" "4_10")

ZPT_RANGES=("5_30" "30_500")
PT_RANGES=("0.5_4" "4_500") 

for zPtRange in "${ZPT_RANGES[@]}"
do
    for trkPtRange in "${PT_RANGES[@]}"
    do
        echo "Processing zPtRange: $zPtRange, trkPtRange: $trkPtRange"
        
        ./ExecutePUPlot --zPtRange $zPtRange --trkPtRange $trkPtRange --PU1Flag EEV2_ZV4_trkV22_nmix10 --PUNominalFlag PUNominal_EEV2_ZV4_trkV22_nmix10

    done
done

exit
