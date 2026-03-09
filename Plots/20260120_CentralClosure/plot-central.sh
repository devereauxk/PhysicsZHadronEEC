make

ZPT_RANGES=("0_10" "10_20" "20_40" "40_500")
PT_RANGES=("0.5_500")

for zPtRange in "${ZPT_RANGES[@]}"
do
    for trkPtRange in "${PT_RANGES[@]}"
    do
        echo "Processing zPtRange: $zPtRange, trkPtRange: $trkPtRange"

        ./ExecuteClosureTest --collisionType pp --zPtRange $zPtRange --trkPtRange $trkPtRange --tag ZV5_trkV23_nmix10
        #./ExecuteClosureTest --collisionType pPb --zPtRange $zPtRange --trkPtRange $trkPtRange --tag ZV5_trkV23_nmix10
        #./ExecuteClosureTest --collisionType PbP --zPtRange $zPtRange --trkPtRange $trkPtRange --tag ZV5_trkV23_nmix10
    
    done
done

exit
