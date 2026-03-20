make

PLOT_SYSTEMS=(${PLOT_SYSTEMS:-pp pPb PbP})

for system in "${PLOT_SYSTEMS[@]}"
do
    ./ExecuteClosureTest --collisionType $system --zPtRange 0_500 --trkPtRange 0.5_500 --tag ZV6_trkV24_nmix10
done
