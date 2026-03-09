#source ./clean.sh
make ExecuteCentralCombinedPlot


# input your Z and track selections here
ZPT_RANGES=("5_500")
PT_RANGES=("0.5_500")

PP_TAG="ZV5_trkV23_nmix10"
PPB_TAG="ZV5_trkV23_nmix10"

for zPtRange in "${ZPT_RANGES[@]}"
do
    for trkPtRange in "${PT_RANGES[@]}"
    do
        echo "Processing zPtRange: $zPtRange, trkPtRange: $trkPtRange"

        ./ExecuteCentralCombinedPlot --zPtRange $zPtRange --trkPtRange $trkPtRange --pPbtag $PPB_TAG --pptag $PP_TAG --doCombine true

        ./ExecuteCentralCombinedPlot --zPtRange $zPtRange --trkPtRange $trkPtRange --pPbtag $PPB_TAG --pptag $PP_TAG --doCombine false --collisionType pPb
        ./ExecuteCentralCombinedPlot --zPtRange $zPtRange --trkPtRange $trkPtRange --pPbtag $PPB_TAG --pptag $PP_TAG --doCombine false --collisionType PbP
    done
done

exit

