minZpt="${1:-0}"
maxZpt="${2:-500}"
name="${3:-${NAME_TAG:-20260311_TrackResidualCorrection_V24_ZWeight_V6}}"

VZWeightFile_PPb="${VZ_WEIGHT_FILE_PPB:-/home/kdeverea/PhysicsZHadronEEC/Plots/20251001_pPbVZReweighting/summary/20260311_ZPT0_500_VzReweightFits_pPb.root}"
VZWeightFile_PbP="${VZ_WEIGHT_FILE_PBP:-/home/kdeverea/PhysicsZHadronEEC/Plots/20251001_pPbVZReweighting/summary/20260311_ZPT0_500_VzReweightFits_PbP.root}"

ZWeightFile_pPb="${Z_WEIGHT_FILE_PPB:-my_ZWeights/20260311_ZCorrection_V6_PPb_zPt0-500.root}"
ZWeightFile_PbP="${Z_WEIGHT_FILE_PBP:-my_ZWeights/20260311_ZCorrection_V6_PbP_zPt0-500.root}"

echo "[pPb-DY-analysis] minZpt=${minZpt} maxZpt=${maxZpt} name=${name}"
echo "[pPb-DY-analysis] VZWeightFile_PPb=${VZWeightFile_PPb}"
echo "[pPb-DY-analysis] VZWeightFile_PbP=${VZWeightFile_PbP}"
echo "[pPb-DY-analysis] ZWeightFile_pPb=${ZWeightFile_pPb}"
echo "[pPb-DY-analysis] ZWeightFile_PbP=${ZWeightFile_PbP}"

# pPb
./finalAnalysis.sh output/DY RECO  $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} --MinZPT $minZpt --MaxZPT $maxZpt -MinTrackPT 0.5 --MaxTrackPT 500  --Input pPbSample/V0.2/PbPMC_Reco.root     --IsGen false --IsPP false --IsGenZ false --ZWeightFile $ZWeightFile_pPb --VZWeightFile $VZWeightFile_PPb
cp output/DY-RECO.root output/DY-RECO-noResidual.root
./finalAnalysis.sh output/DY GEN   $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} --MinZPT $minZpt --MaxZPT $maxZpt -MinTrackPT 0.5 --MaxTrackPT 500  --Input pPbSample/V0.2/PbPMC_Gen.root --InputUE mergedEPOS/PPbMC_Gen.root --IsGen true --IsPP false --IsGenZ true --VZWeightFile $VZWeightFile_PPb
root -l -q -b correction.C
mv correction.root output/correction_1.root
./finalAnalysis.sh output/DY RECO  $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} --MinZPT $minZpt --MaxZPT $maxZpt -MinTrackPT 0.5 --MaxTrackPT 500  --Input pPbSample/V0.2/PbPMC_Reco.root     --residualFile output/correction_1.root --IsGen false --IsPP false --IsGenZ false --ZWeightFile $ZWeightFile_pPb --VZWeightFile $VZWeightFile_PPb
root -l -q -b correction.C
mv correction.root output/correction_2.root
root -l -q -b 'merge_corrections.C("output/correction_1.root","output/correction_2.root","output/correction_12.root")'
./finalAnalysis.sh output/DY RECO  $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} --MinZPT $minZpt --MaxZPT $maxZpt -MinTrackPT 0.5 --MaxTrackPT 500  --Input pPbSample/V0.2/PbPMC_Reco.root     --residualFile output/correction_12.root --IsGen false --IsPP false --IsGenZ false --ZWeightFile $ZWeightFile_pPb --VZWeightFile $VZWeightFile_PPb
root -l -q -b correction.C
mv correction.root output/correction_3.root

root -l -q -b plot_corrections.C
mv corrections.pdf plots/corrections_PPb_${name}_${minZpt}_${maxZpt}.pdf
root -l -q -b "merge_corrections.C(\"output/correction_12.root\",\"output/correction_3.root\",\"output/${name}_PPb.root\")"
mv output/${name}_PPb.root output/${name}_PPb_zPt$minZpt-$maxZpt.root

./finalAnalysis.sh output/DY RECO $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} --MinZPT $minZpt --MaxZPT $maxZpt -MinTrackPT 0.5 --MaxTrackPT 500  --Input pPbSample/V0.2/PbPMC_Reco.root     --residualFile output/${name}_PPb_zPt$minZpt-$maxZpt.root --IsGen false --IsPP false --IsGenZ false --ZWeightFile $ZWeightFile_pPb --VZWeightFile $VZWeightFile_PPb
root -l -q -b "plot_closure.C(\"plots/isPPb_ZPT${minZpt}_${maxZpt}\")"

# PbP
./finalAnalysis.sh output/DY RECO  $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} --MinZPT $minZpt --MaxZPT $maxZpt -MinTrackPT 0.5 --MaxTrackPT 500  --Input pPbSample/V0.2/PPbMC_Reco.root     --IsGen false --IsPP false --IsGenZ false --ZWeightFile $ZWeightFile_PbP --VZWeightFile $VZWeightFile_PbP
cp output/DY-RECO.root output/DY-RECO-noResidual.root
./finalAnalysis.sh output/DY GEN   $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} --MinZPT $minZpt --MaxZPT $maxZpt -MinTrackPT 0.5 --MaxTrackPT 500  --Input pPbSample/V0.2/PPbMC_Gen.root --InputUE mergedEPOS/PbPMC_Gen.root --IsGen true --IsPP false --IsGenZ true --VZWeightFile $VZWeightFile_PbP
root -l -q -b correction.C
mv correction.root output/correction_1.root
./finalAnalysis.sh output/DY RECO  $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} --MinZPT $minZpt --MaxZPT $maxZpt -MinTrackPT 0.5 --MaxTrackPT 500  --Input pPbSample/V0.2/PPbMC_Reco.root     --residualFile output/correction_1.root --IsGen false --IsPP false --IsGenZ false --ZWeightFile $ZWeightFile_PbP --VZWeightFile $VZWeightFile_PbP
root -l -q -b correction.C
mv correction.root output/correction_2.root
root -l -q -b 'merge_corrections.C("output/correction_1.root","output/correction_2.root","output/correction_12.root")'
./finalAnalysis.sh output/DY RECO  $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} --MinZPT $minZpt --MaxZPT $maxZpt -MinTrackPT 0.5 --MaxTrackPT 500  --Input pPbSample/V0.2/PPbMC_Reco.root     --residualFile output/correction_12.root --IsGen false --IsPP false --IsGenZ false --ZWeightFile $ZWeightFile_PbP --VZWeightFile $VZWeightFile_PbP
root -l -q -b correction.C
mv correction.root output/correction_3.root

root -l -q -b plot_corrections.C
mv corrections.pdf plots/corrections_PbP_${name}_${minZpt}_${maxZpt}.pdf
root -l -q -b "merge_corrections.C(\"output/correction_12.root\",\"output/correction_3.root\",\"output/${name}_PbP.root\")"
mv output/${name}_PbP.root output/${name}_PbP_zPt$minZpt-$maxZpt.root

./finalAnalysis.sh output/DY RECO $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} --MinZPT $minZpt --MaxZPT $maxZpt -MinTrackPT 0.5 --MaxTrackPT 500  --Input pPbSample/V0.2/PPbMC_Reco.root     --residualFile output/${name}_PbP_zPt$minZpt-$maxZpt.root --IsGen false --IsPP false --IsGenZ false --ZWeightFile $ZWeightFile_PbP --VZWeightFile $VZWeightFile_PbP
root -l -q -b "plot_closure.C(\"plots/isPbP_ZPT${minZpt}_${maxZpt}\")"
