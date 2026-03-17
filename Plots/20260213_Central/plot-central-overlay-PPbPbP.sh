#source ./clean.sh
make ExecuteCentralOverlayPPbPbPPlot


# input your Z and track selections here
PPB_TAG="ZV5_trkV23_nmix10"

./ExecuteCentralOverlayPPbPbPPlot --zPtRange 5_30 --trkPtRange 0.5_4 --pPbtag $PPB_TAG
./ExecuteCentralOverlayPPbPbPPlot --zPtRange 5_30 --trkPtRange 4_500 --pPbtag $PPB_TAG
./ExecuteCentralOverlayPPbPbPPlot --zPtRange 30_500 --trkPtRange 0.5_4 --pPbtag $PPB_TAG
./ExecuteCentralOverlayPPbPbPPlot --zPtRange 30_500 --trkPtRange 4_500 --pPbtag $PPB_TAG
./ExecuteCentralOverlayPPbPbPPlot --zPtRange 5_500 --trkPtRange 0.5_500 --pPbtag $PPB_TAG

exit
