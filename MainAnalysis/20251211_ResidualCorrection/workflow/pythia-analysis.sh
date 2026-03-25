minZpt=$1
maxZpt=$2
name=$3

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh

ZWeightFile_pp="${ZWeightFile_PP}"
VZWeightFile="${VZWeightFile_PP}"

# pp
./finalAnalysis.sh output/DY RECO  $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} --MinZPT $minZpt --MaxZPT $maxZpt -MinTrackPT 0.5 --MaxTrackPT 500  --Input mergedSample/pythia-v11-Zpt0.root     --IsGen false --IsPP true --IsGenZ false --ZWeightFile $ZWeightFile_pp --UseVZWeight true --VZWeightFile $VZWeightFile
cp output/DY-RECO.root output/DY-RECO-noResidual.root
./finalAnalysis.sh output/DY GEN   $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} --MinZPT $minZpt --MaxZPT $maxZpt -MinTrackPT 0.5 --MaxTrackPT 500  --Input mergedSample/pythia-gen-v11-Zpt0.root --IsGen true --IsPP true --IsGenZ true --UseVZWeight true --VZWeightFile $VZWeightFile
root -l -q -b correction.C
mv correction.root output/correction_1.root
./finalAnalysis.sh output/DY RECO  $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} --MinZPT $minZpt --MaxZPT $maxZpt -MinTrackPT 0.5 --MaxTrackPT 500  --Input mergedSample/pythia-v11-Zpt0.root     --residualFile output/correction_1.root --IsGen false --IsPP true --IsGenZ false --ZWeightFile $ZWeightFile_pp --UseVZWeight true --VZWeightFile $VZWeightFile
root -l -q -b correction.C
mv correction.root output/correction_2.root
root -l -q -b "merge_corrections.C(\"output/correction_1.root\",\"output/correction_2.root\",\"output/correction_12.root\")"
./finalAnalysis.sh output/DY RECO  $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} --MinZPT $minZpt --MaxZPT $maxZpt -MinTrackPT 0.5 --MaxTrackPT 500  --Input mergedSample/pythia-v11-Zpt0.root     --residualFile output/correction_12.root --IsGen false --IsPP true --IsGenZ false --ZWeightFile $ZWeightFile_pp --UseVZWeight true --VZWeightFile $VZWeightFile
root -l -q -b correction.C
mv correction.root output/correction_3.root

root -l -q -b plot_corrections.C
mv corrections.pdf plots/corrections_pp_${name}_${minZpt}_${maxZpt}.pdf
root -l -q -b "merge_corrections.C(\"output/correction_12.root\",\"output/correction_3.root\",\"output/${name}_pp.root\")"
mv output/${name}_pp.root output/${name}_pp_zPt$minZpt-$maxZpt.root

./finalAnalysis.sh output/DY RECO $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} --MinZPT $minZpt --MaxZPT $maxZpt -MinTrackPT 0.5 --MaxTrackPT 500  --Input mergedSample/pythia-v11-Zpt0.root     --residualFile output/${name}_pp_zPt$minZpt-$maxZpt.root --IsGen false --IsPP true --IsGenZ false --ZWeightFile $ZWeightFile_pp --UseVZWeight true --VZWeightFile $VZWeightFile
root -l -q -b "plot_closure.C(\"plots/ispp_ZPT${minZpt}_${maxZpt}\")"
