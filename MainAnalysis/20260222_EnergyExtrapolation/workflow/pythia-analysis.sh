WORKINGDIR=$(pwd)
cd ..
./clean.sh
cd $WORKINGDIR
sleep 1

minZpt=0
maxZpt=500
name="20260321_EnergyExtrapolation_EEV3"

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh

# pp
./finalAnalysis.sh output/pp 502  $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} --MinZPT $minZpt --MaxZPT $maxZpt -MinTrackPT 0.5 --MaxTrackPT 500  --Input mergedSample/pp-v11-Zpt0.root     --IsGen false --IsPP true --IsGenZ false --residualFile "${ZWeightFile_PP}"

./finalAnalysis.sh output/pPb 816   $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} --MinZPT $minZpt --MaxZPT $maxZpt -MinTrackPT 0.5 --MaxTrackPT 500  --Input pPbSample/V0.2/PPbData_Reco.root --IsGen false --IsPP false --IsGenZ false --residualFile "${ZWeightFile_PPb}"
./finalAnalysis.sh output/PbP 816   $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} --MinZPT $minZpt --MaxZPT $maxZpt -MinTrackPT 0.5 --MaxTrackPT 500  --Input pPbSample/V0.2/PbPData_Reco.root --IsGen false --IsPP false --IsGenZ false --residualFile "${ZWeightFile_PbP}"
hadd -f output/pPb-PbP-816.root output/pPb-816.root output/PbP-816.root

root -l -q -b correction.C

root -l -q -b plot_corrections.C
mv correction.root output/${name}.root
mv corrections.pdf output/${name}.pdf

./finalAnalysis.sh output/pp 816  $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} --MinZPT $minZpt --MaxZPT $maxZpt -MinTrackPT 0.5 --MaxTrackPT 500  --Input mergedSample/pp-v11-Zpt0.root     --IsGen false --IsPP true --IsGenZ false --residualFile "${ZWeightFile_PP}" --EnergyExtraFile output/${name}.root
root -l -q -b "plot_closure.C(\"output/${name}-closure.pdf\")"
