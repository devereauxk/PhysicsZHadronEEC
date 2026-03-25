source clean.sh

./ExecuteZMassPlot --collisionType pPb --zPtRange 0_500 --trkPtRange 0.5_500 --tag ZV6_trkV24_nmix10
./ExecuteZMassPlot --collisionType PbP --zPtRange 0_500 --trkPtRange 0.5_500 --tag ZV6_trkV24_nmix10
./ExecuteZMassPlot --collisionType pp --zPtRange 5_500 --trkPtRange 0.5_500 --tag EEV3_ZV6_trkV24_nmix10

exit
