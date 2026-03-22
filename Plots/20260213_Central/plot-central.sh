make ExecuteCentralPlot

PPB_TAG="ZV6_trkV24_vz20260320_nmix10"
PP_TAG="ZV6_trkV24_EEV3_nmix10"

run_one() {
    local ZPT=$1
    local TRKPT=$2
    ./ExecuteCentralPlot --collisionType pPb --zPtRange "$ZPT" --trkPtRange "$TRKPT" --pPbtag "$PPB_TAG" --pptag "$PP_TAG"
    ./ExecuteCentralPlot --collisionType PbP --zPtRange "$ZPT" --trkPtRange "$TRKPT" --pPbtag "$PPB_TAG" --pptag "$PP_TAG"
}

run_one "5_30" "0.5_4"
run_one "5_30" "4_500"
run_one "30_500" "0.5_4"
run_one "30_500" "4_500"
run_one "5_500" "0.5_500"

exit
