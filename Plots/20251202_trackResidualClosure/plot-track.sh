make

#ZPT_RANGES=("0_5" "5_10" "10_20" "20_30" "0_30" "30_500" "0_500")

ZPT_RANGES=("20_30")

for zPtRange in "${ZPT_RANGES[@]}"
do
    echo "Processing zPtRange: $zPtRange"

    ./ExecuteClosureTest --collisionType pp --zPtRange $zPtRange --trkPtRange 0.5_500 --tag ZV4_trkV22_nmix10
    #./ExecuteClosureTest --collisionType pPb --zPtRange $zPtRange --trkPtRange 0.5_500 --tag ZV5_trkV23_nmix10
    #./ExecuteClosureTest --collisionType PbP --zPtRange $zPtRange --trkPtRange 0.5_500 --tag ZV5_trkV23_nmix10
done

exit
