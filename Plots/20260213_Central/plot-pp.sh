make ExecutePPPlot

PP_TAG="ZV6_trkV24_vz20260320_nmix10"

for trkPtRange in 1_2 2_4 4_10
do
    echo "Processing tag: ${PP_TAG}, zPtRange: 40_350, trkPtRange: ${trkPtRange}"
    ./ExecutePPPlot --zPtRange 40_350 --trkPtRange "${trkPtRange}" --pptag "${PP_TAG}"
done

for zPtRange in 20_40 40_60 60_500
do
    echo "Processing tag: ${PP_TAG}, zPtRange: ${zPtRange}, trkPtRange: 2_500"
    ./ExecutePPPlot --zPtRange "${zPtRange}" --trkPtRange 2_500 --pptag "${PP_TAG}"
done

exit
