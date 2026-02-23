source ./clean.sh

ZPT_RANGES=("0_30" "30_500" "0_500")
PT_RANGES=("0.5_500")

for zPtRange in "${ZPT_RANGES[@]}"
do
    for trkPtRange in "${PT_RANGES[@]}"
    do
        echo "Processing zPtRange: $zPtRange, trkPtRange: $trkPtRange"

        ./ExecuteCentralPlot --collisionType pPb --zPtRange $zPtRange --trkPtRange $trkPtRange --pPbtag ZV5_trkV23_nmix10 --pptag ZV4_trkV22_nmix10
        ./ExecuteCentralPlot --collisionType PbP --zPtRange $zPtRange --trkPtRange $trkPtRange --pPbtag ZV5_trkV23_nmix10 --pptag ZV4_trkV22_nmix10

    done
done

exit


./ExecuteCentralPlot --zPtRange 0_30 --trkPtRange 0.5_4 --pPbtag ZV5_trkV23_nmix10 --pptag ZV4_trkV22_nmix10
./ExecuteCentralPlot --zPtRange 0_30 --trkPtRange 4_500 --pPbtag ZV5_trkV23_nmix10 --pptag ZV4_trkV22_nmix10
./ExecuteCentralPlot --zPtRange 30_500 --trkPtRange 0.5_4 --pPbtag ZV5_trkV23_nmix10 --pptag ZV4_trkV22_nmix10
./ExecuteCentralPlot --zPtRange 30_500 --trkPtRange 4_500 --pPbtag ZV5_trkV23_nmix10 --pptag ZV4_trkV22_nmix10

exit


./ExecuteCentralPlot --zPtRange 0_10 --trkPtRange 0.5_2 --pPbtag ZV5_trkV23_nmix10 --pptag ZV4_trkV22_nmix10
./ExecuteCentralPlot --zPtRange 0_10 --trkPtRange 2_500 --pPbtag ZV5_trkV23_nmix10 --pptag ZV4_trkV22_nmix10
./ExecuteCentralPlot --zPtRange 10_500 --trkPtRange 0.5_2 --pPbtag ZV5_trkV23_nmix10 --pptag ZV4_trkV22_nmix10
./ExecuteCentralPlot --zPtRange 10_500 --trkPtRange 2_500 --pPbtag ZV5_trkV23_nmix10 --pptag ZV4_trkV22_nmix10

exit

./ExecuteCentralPlot --zPtRange 10_500 --trkPtRange 2_500 --tag ZV5_trkV23_nmix10
./ExecuteCentralPlot --zPtRange 0_10 --trkPtRange 0.5_2 --tag ZV5_trkV23_nmix10
./ExecuteCentralPlot --zPtRange 0_10 --trkPtRange 2_500 --tag ZV5_trkV23_nmix10


exit
