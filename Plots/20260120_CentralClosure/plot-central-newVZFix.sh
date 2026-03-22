make

ZPT_RANGES=("0_10" "10_20" "20_40" "40_500")
PT_RANGES=("0.5_500")
TAG="ZV6_trkV24_vz20260320_nmix10"

for zPtRange in "${ZPT_RANGES[@]}"
do
    for trkPtRange in "${PT_RANGES[@]}"
    do
        echo "Processing zPtRange: $zPtRange, trkPtRange: $trkPtRange"
        ./ExecuteClosureTest --collisionType pPb --zPtRange $zPtRange --trkPtRange $trkPtRange --tag $TAG
        ./ExecuteClosureTest --collisionType PbP --zPtRange $zPtRange --trkPtRange $trkPtRange --tag $TAG
    done
done

exit
