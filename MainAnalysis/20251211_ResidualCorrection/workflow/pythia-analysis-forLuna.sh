minZpt=$1
maxZpt=$2
name=$3

FILE_GEN=OOSample-local/V0.7/PPMCMuMu_Gen.root
FILE_RECO=OOSample-local/V0.7/PPMCMuMu_Reco.root

# isPP functionally doesn't do anything

# pp
./finalAnalysis.sh output/DY RECO  --MinZPT $minZpt --MaxZPT $maxZpt  --Input $FILE_RECO --IsGen false --IsPP true --IsGenZ false 
cp output/DY-RECO.root output/DY-RECO-noResidual.root
./finalAnalysis.sh output/DY GEN   --MinZPT $minZpt --MaxZPT $maxZpt  --Input $FILE_GEN --IsGen true --IsPP true --IsGenZ true

root -l -q -b correction.C
mv correction.root output/correction_1.root
./finalAnalysis.sh output/DY RECO  --MinZPT $minZpt --MaxZPT $maxZpt  --Input $FILE_RECO --residualFile output/correction_1.root --IsGen false --IsPP true --IsGenZ false 
root -l -q -b correction.C
mv correction.root output/correction_2.root
root -l -q -b "merge_corrections.C(\"output/correction_1.root\",\"output/correction_2.root\",\"output/correction_12.root\")"
./finalAnalysis.sh output/DY RECO  --MinZPT $minZpt --MaxZPT $maxZpt  --Input $FILE_RECO     --residualFile output/correction_12.root --IsGen false --IsPP true --IsGenZ false 
root -l -q -b correction.C
mv correction.root output/correction_3.root

root -l -q -b plot_corrections.C
mv corrections.pdf plots/corrections_pp_${name}_${minZpt}_${maxZpt}.pdf
root -l -q -b "merge_corrections.C(\"output/correction_12.root\",\"output/correction_3.root\",\"output/${name}_pp.root\")"
mv output/${name}_pp.root output/${name}_pp_zPt$minZpt-$maxZpt.root

./finalAnalysis.sh output/DY RECO --MinZPT $minZpt --MaxZPT $maxZpt  --Input $FILE_RECO     --residualFile output/${name}_pp_zPt$minZpt-$maxZpt.root --IsGen false --IsPP true --IsGenZ false 
root -l -q -b "plot_closure.C(\"plots/ispp_forLuna_ZPT${minZpt}_${maxZpt}\")"
