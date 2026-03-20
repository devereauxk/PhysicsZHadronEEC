make

TAG="${1:-newVZFix_ZV6_trkV24_nmix10}"
ZPT_RANGES=("0_10" "10_20" "20_40" "40_500")

for zPtRange in "${ZPT_RANGES[@]}"
do
    echo "Processing zPtRange: $zPtRange"
    ./ExecuteClosureTest --collisionType pPb --zPtRange "$zPtRange" --trkPtRange 0.5_500 --tag "$TAG"
    ./ExecuteClosureTest --collisionType PbP --zPtRange "$zPtRange" --trkPtRange 0.5_500 --tag "$TAG"
done
