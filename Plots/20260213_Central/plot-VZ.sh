make ExecuteVZPlot

./ExecuteVZPlot --collisionType pp --zPtRange 0_350 --trkPtRange 1_10 --tag EvtWeight_VZOnly_nmix1
./ExecuteVZPlot --collisionType pp --zPtRange 0_350 --trkPtRange 1_10 --tag noEvtWeight_VZOnly_nmix1
./ExecuteVZPlot --collisionType pp --zPtRange 0_350 --trkPtRange 1_10 --tag myEvtWeight_VZOnly_nmix1

./ExecuteVZPlot --collisionType pp --zPtRange 0_350 --trkPtRange 1_10 --tag EvtWeight_nmix1
./ExecuteVZPlot --collisionType pp --zPtRange 0_350 --trkPtRange 1_10 --tag noEvtWeight_nmix1
./ExecuteVZPlot --collisionType pp --zPtRange 0_350 --trkPtRange 1_10 --tag myEvtWeight_nmix1

exit

./ExecuteVZPlot --collisionType pPb --zPtRange 0_350 --trkPtRange 1_10 --tag EvtWeight_nmix1
./ExecuteVZPlot --collisionType pPb --zPtRange 0_350 --trkPtRange 1_10 --tag noEvtWeight_nmix1

./ExecuteVZPlot --collisionType PbP --zPtRange 0_350 --trkPtRange 1_10 --tag EvtWeight_nmix1
./ExecuteVZPlot --collisionType PbP --zPtRange 0_350 --trkPtRange 1_10 --tag noEvtWeight_nmix1

exit
