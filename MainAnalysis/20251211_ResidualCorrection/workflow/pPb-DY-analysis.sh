minZpt=$1
maxZpt=$2
name=$3

ZWeightFile_pPb="my_ZWeights/20260115_ZCorrection_V1_PPb_zPt0-500.root"
ZWeightFile_PbP="my_ZWeights/20260115_ZCorrection_V1_PbP_zPt0-500.root"

./finalAnalysis.sh output/DY RECO  $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} --MinZPT $minZpt --MaxZPT $maxZpt -MinTrackPT 0.5 --MaxTrackPT 500  --MinHiBin -10 --MaxHiBin 1000  --Input mergedSample/PPbMC_Reco_v0.2.root     --IsGen false --IsPP false --IsGenZ false --ZWeightFile $ZWeightFile_pPb
cp output/DY-RECO.root output/DY-RECO-noResidual.root
./finalAnalysis.sh output/DY GEN   $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} --MinZPT $minZpt --MaxZPT $maxZpt -MinTrackPT 0.5 --MaxTrackPT 500  --MinHiBin -10 --MaxHiBin 1000  --Input mergedSample/PPbMC_Gen_v0.2.root --InputUE mergedSample/PPbEPOS_Gen_v0.2_1XX.root --IsGen true --IsPP false --IsGenZ true
root -l -q -b correction.C
mv correction.root output/correction_1.root
./finalAnalysis.sh output/DY RECO  $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} --MinZPT $minZpt --MaxZPT $maxZpt -MinTrackPT 0.5 --MaxTrackPT 500  --MinHiBin -10 --MaxHiBin 1000  --Input mergedSample/PPbMC_Reco_v0.2.root     --residualFile output/correction_1.root --IsGen false --IsPP false --IsGenZ true --ZWeightFile $ZWeightFile_pPb
root -l -q -b correction.C
mv correction.root output/correction_2.root
root -l -q -b "merge_corrections.C(\"output/correction_1.root\",\"output/correction_2.root\",\"output/correction_12.root\")"
./finalAnalysis.sh output/DY RECO  $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} --MinZPT $minZpt --MaxZPT $maxZpt -MinTrackPT 0.5 --MaxTrackPT 500  --MinHiBin -10 --MaxHiBin 1000  --Input mergedSample/PPbMC_Reco_v0.2.root     --residualFile output/correction_12.root --IsGen false --IsPP false --IsGenZ true --ZWeightFile $ZWeightFile_pPb
root -l -q -b correction.C
mv correction.root output/correction_3.root

root -l -q -b plot_corrections.C
mv corrections.pdf plots/corrections_PPb_${name}_${minZpt}_${maxZpt}.pdf
root -l -q -b "merge_corrections.C(\"output/correction_12.root\",\"output/correction_3.root\",\"output/${name}_PPb.root\")"
root -l -q -b "plot_closure.C(\"plots/isPPb_ZPT${minZpt}_${maxZpt}\")"

mv output/${name}_PPb.root output/${name}_PPb_zPt$minZpt-$maxZpt.root

./finalAnalysis.sh output/DY RECO  $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} --MinZPT $minZpt --MaxZPT $maxZpt -MinTrackPT 0.5 --MaxTrackPT 500  --MinHiBin -10 --MaxHiBin 1000  --Input mergedSample/PbPMC_Reco_v0.2.root     --IsGen false --IsPP false --IsGenZ false --ZWeightFile $ZWeightFile_PbP
cp output/DY-RECO.root output/DY-RECO-noResidual.root
./finalAnalysis.sh output/DY GEN   $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} --MinZPT $minZpt --MaxZPT $maxZpt -MinTrackPT 0.5 --MaxTrackPT 500  --MinHiBin -10 --MaxHiBin 1000  --Input mergedSample/PbPMC_Gen_v0.2.root --InputUE mergedSample/PbPEPOS_Gen_v0.2_1XX.root --IsGen true --IsPP false --IsGenZ false
root -l -q -b correction.C
mv correction.root output/correction_1.root
./finalAnalysis.sh output/DY RECO  $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} --MinZPT $minZpt --MaxZPT $maxZpt -MinTrackPT 0.5 --MaxTrackPT 500  --MinHiBin -10 --MaxHiBin 1000  --Input mergedSample/PbPMC_Reco_v0.2.root     --residualFile output/correction_1.root --IsGen false --IsPP false --IsGenZ false --ZWeightFile $ZWeightFile_PbP
root -l -q -b correction.C
mv correction.root output/correction_2.root
root -l -q -b "merge_corrections.C(\"output/correction_1.root\",\"output/correction_2.root\",\"output/correction_12.root\")"
./finalAnalysis.sh output/DY RECO  $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} --MinZPT $minZpt --MaxZPT $maxZpt -MinTrackPT 0.5 --MaxTrackPT 500  --MinHiBin -10 --MaxHiBin 1000  --Input mergedSample/PbPMC_Reco_v0.2.root     --residualFile output/correction_12.root --IsGen false --IsPP false --IsGenZ false --ZWeightFile $ZWeightFile_PbP
root -l -q -b correction.C
mv correction.root output/correction_3.root

root -l -q -b plot_corrections.C
mv corrections.pdf plots/corrections_PbP_${name}_${minZpt}_${maxZpt}.pdf
root -l -q -b "merge_corrections.C(\"output/correction_12.root\",\"output/correction_3.root\",\"output/${name}_PbP.root\")"
root -l -q -b "plot_closure.C(\"plots/isPbP_ZPT${minZpt}_${maxZpt}\")"

mv output/${name}_PbP.root output/${name}_PbP_zPt$minZpt-$maxZpt.root
