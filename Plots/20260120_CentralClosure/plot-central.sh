make

ZPT_RANGES=("0_10" "10_20" "20_40" "40_500")
PT_RANGES=("0.5_500")
PLOT_SYSTEMS=(${PLOT_SYSTEMS:-pp pPb PbP})

for zPtRange in "${ZPT_RANGES[@]}"
do
    for trkPtRange in "${PT_RANGES[@]}"
    do
        echo "Processing zPtRange: $zPtRange, trkPtRange: $trkPtRange"
        for system in "${PLOT_SYSTEMS[@]}"
        do
            ./ExecuteClosureTest --collisionType $system --zPtRange $zPtRange --trkPtRange $trkPtRange --tag ZV6_trkV24_nmix10
        done
    
    done
done

exit
