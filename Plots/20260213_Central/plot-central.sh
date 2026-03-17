make ExecuteCentralPlot

PPB_TAG="ZV5_trkV23_nmix10"
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


./ExecuteCentralPlot --zPtRange 0_30 --trkPtRange 0.5_4 --pPbtag ZV5_trkV23_nmix10 --pptag ZV4_trkV22_nmix10
./ExecuteCentralPlot --zPtRange 0_30 --trkPtRange 4_500 --pPbtag ZV5_trkV23_nmix10 --pptag ZV4_trkV22_nmix10
./ExecuteCentralPlot --zPtRange 30_500 --trkPtRange 0.5_4 --pPbtag ZV5_trkV23_nmix10 --pptag ZV4_trkV22_nmix10
./ExecuteCentralPlot --zPtRange 30_500 --trkPtRange 4_500 --pPbtag ZV5_trkV23_nmix10 --pptag ZV4_trkV22_nmix10

exit


./ExecuteCentralPlot --zPtRange 0_10 --trkPtRange 0.5_2 --pPbtag ZV5_trkV23_nmix10 --pptag ZV4_trkV22_nmix10
./ExecuteCentralPlot --zPtRange 0_10 --trkPtRange 2_500 --pPbtag ZV5_trkV23_nmix10 --pptag ZV4_trkV22_nmix10
./ExecuteCentralPlot --zPtRange 10_500 --trkPtRange 0.5_2 --pPbtag ZV5_trkV23_nmix10 --pptag ZV4_trkV22_nmix10
./ExecuteCentralPlot --zPtRange 10_500 --trkPtRange 2_500 --pPbtag ZV5_trkV23_nmix10 --pptag ZV4_trkV22_nmix10

exit

./ExecuteCentralPlot --zPtRange 10_500 --trkPtRange 2_500 --tag ZV5_trkV23_nmix10
./ExecuteCentralPlot --zPtRange 0_10 --trkPtRange 0.5_2 --tag ZV5_trkV23_nmix10
./ExecuteCentralPlot --zPtRange 0_10 --trkPtRange 2_500 --tag ZV5_trkV23_nmix10


exit
