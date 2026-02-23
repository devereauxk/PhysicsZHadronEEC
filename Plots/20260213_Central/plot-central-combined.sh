#source ./clean.sh
make ExecuteCentralCombinedPlot


# input your Z and track selections here
#ZPT_RANGES=("0_5" "5_10" "10_20" "20_30" "0_30" "30_500")
#PT_RANGES=("0.5_4" "4_500")

ZPT_RANGES=("5_500" "5_30" "30_500")
PT_RANGES=("0.5_4" "4_500" "0.5_500")


for zPtRange in "${ZPT_RANGES[@]}"
do
    for trkPtRange in "${PT_RANGES[@]}"
    do
        echo "Processing zPtRange: $zPtRange, trkPtRange: $trkPtRange"

        ./ExecuteCentralCombinedPlot --zPtRange $zPtRange --trkPtRange $trkPtRange --pPbtag ZV5_trkV23_nmix10 --pptag ZV4_trkV22_nmix10 --doCombine true

        ./ExecuteCentralCombinedPlot --zPtRange $zPtRange --trkPtRange $trkPtRange --pPbtag ZV5_trkV23_nmix10 --pptag ZV4_trkV22_nmix10 --doCombine false --collisionType pPb
        ./ExecuteCentralCombinedPlot --zPtRange $zPtRange --trkPtRange $trkPtRange --pPbtag ZV5_trkV23_nmix10 --pptag ZV4_trkV22_nmix10 --doCombine false --collisionType PbP
    done
done

exit

