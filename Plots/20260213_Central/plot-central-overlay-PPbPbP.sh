#source ./clean.sh
make ExecuteCentralOverlayPPbPbPPlot


# input your Z and track selections here
ZPT_RANGES=("5_30" "30_500")
PT_RANGES=("0.5_4" "4_500")

PPB_TAG="ZV5_trkV23_nmix10"

for zPtRange in "${ZPT_RANGES[@]}"
do
    for trkPtRange in "${PT_RANGES[@]}"
    do
        echo "Processing zPtRange: $zPtRange, trkPtRange: $trkPtRange"

        ./ExecuteCentralOverlayPPbPbPPlot --zPtRange $zPtRange --trkPtRange $trkPtRange --pPbtag $PPB_TAG

    done
done

exit

