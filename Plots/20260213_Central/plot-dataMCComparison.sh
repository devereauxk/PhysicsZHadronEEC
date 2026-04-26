make ExecuteDataMCComparison

# input your Z and track selections here
ZPT_RANGES=("5_500")
PT_RANGES=("0.5_500") 

# note mixing is turned off for these studies
PPB_TAG="dataMCComparison_ZV6_trkV24_nmix0"

for zPtRange in "${ZPT_RANGES[@]}"
do
    for trkPtRange in "${PT_RANGES[@]}"
    do
        echo "Processing zPtRange: $zPtRange, trkPtRange: $trkPtRange"

        ./ExecuteDataMCComparison --collisionType pPb --zPtRange $zPtRange --trkPtRange $trkPtRange --tag $PPB_TAG
        ./ExecuteDataMCComparison --collisionType PbP --zPtRange $zPtRange --trkPtRange $trkPtRange --tag $PPB_TAG

    done
done


exit
