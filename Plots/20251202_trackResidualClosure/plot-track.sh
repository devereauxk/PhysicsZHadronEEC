make

ZPT_RANGES=("0_10" "10_20" "20_40" "40_500")

for zPtRange in "${ZPT_RANGES[@]}"
do
    echo "Processing zPtRange: $zPtRange"

    ./ExecuteClosureTest --collisionType pp --zPtRange $zPtRange --trkPtRange 0.5_500 --tag ZV5_trkV23_nmix1
    #./ExecuteClosureTest --collisionType pPb --zPtRange $zPtRange --trkPtRange 0.5_500 --tag ZV5_trkV23_nmix10
    #./ExecuteClosureTest --collisionType PbP --zPtRange $zPtRange --trkPtRange 0.5_500 --tag ZV5_trkV23_nmix10
done

exit
