make

# ZPT_RANGES=("0_10" "10_500")
# PT_RANGES=("0.5_2" "2_500")

./ExecuteClosureTest --collisionType pPb --zPtRange 10_500 --trkPtRange 0.5_2 --tag ZV5_trkV23_nmix10
./ExecuteClosureTest --collisionType pPb --zPtRange 10_500 --trkPtRange 2_500 --tag ZV5_trkV23_nmix10
./ExecuteClosureTest --collisionType pPb --zPtRange 0_10 --trkPtRange 0.5_2 --tag ZV5_trkV23_nmix10
./ExecuteClosureTest --collisionType pPb --zPtRange 0_10 --trkPtRange 2_500 --tag ZV5_trkV23_nmix10


./ExecuteClosureTest --collisionType pp --zPtRange 10_500 --trkPtRange 0.5_2 --tag ZV4_trkV22_nmix10
./ExecuteClosureTest --collisionType pp --zPtRange 10_500 --trkPtRange 2_500 --tag ZV4_trkV22_nmix10
./ExecuteClosureTest --collisionType pp --zPtRange 0_10 --trkPtRange 0.5_2 --tag ZV4_trkV22_nmix10
./ExecuteClosureTest --collisionType pp --zPtRange 0_10 --trkPtRange 2_500 --tag ZV4_trkV22_nmix10


exit
