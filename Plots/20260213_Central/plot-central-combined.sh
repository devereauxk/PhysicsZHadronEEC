#source ./clean.sh
make ExecuteCentralCombinedPlot


# input your Z and track selections here
PP_TAG="ZV6_trkV24_EEV3_nmix10"
PPB_TAG="ZV6_trkV24_vz20260320_nmix10"

run_one() {
    local ZPT=$1
    local TRKPT=$2
    ./ExecuteCentralCombinedPlot --zPtRange $ZPT --trkPtRange $TRKPT --pPbtag $PPB_TAG --pptag $PP_TAG --doCombine true
    ./ExecuteCentralCombinedPlot --zPtRange $ZPT --trkPtRange $TRKPT --pPbtag $PPB_TAG --pptag $PP_TAG --doCombine false --collisionType pPb
    ./ExecuteCentralCombinedPlot --zPtRange $ZPT --trkPtRange $TRKPT --pPbtag $PPB_TAG --pptag $PP_TAG --doCombine false --collisionType PbP
}

run_one 5_30 0.5_4
run_one 5_30 4_500
run_one 30_500 0.5_4
run_one 30_500 4_500
run_one 5_500 0.5_500

exit
