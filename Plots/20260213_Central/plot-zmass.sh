source clean.sh

./ExecuteZMassPlot --collisionType pPb --zPtRange 0_500 --trkPtRange 0.5_500 --tag ZV5_trkV23_nmix10
./ExecuteZMassPlot --collisionType PbP --zPtRange 0_500 --trkPtRange 0.5_500 --tag ZV5_trkV23_nmix10
./ExecuteZMassPlot --collisionType pp --zPtRange 0_500 --trkPtRange 0.5_500 --tag ZV4_trkV22_nmix10

exit
