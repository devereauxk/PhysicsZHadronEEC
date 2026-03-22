make

TAG="${1:-ZV6_trkV24_nmix0}"
PLOT_SYSTEMS=(${PLOT_SYSTEMS:-pp pPb PbP})

for system in "${PLOT_SYSTEMS[@]}"
do
    for zPtRange in "0_500"
    do
        echo "Processing $system zPtRange: $zPtRange"
        ./ExecuteClosureTest --collisionType "$system" --zPtRange "$zPtRange" --trkPtRange 0.5_500 --tag "$TAG"
    done
done
