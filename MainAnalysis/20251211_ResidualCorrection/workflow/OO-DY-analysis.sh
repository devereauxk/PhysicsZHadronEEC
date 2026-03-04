minZpt=$1
maxZpt=$2
name=$3

FILE_GEN=OOSample/V0.4/MCMuMu_Gen.root
FILE_GEN_UE=OOSample/V0.4/OOMB_Gen.root
FILE_RECO=OOSample/V0.4/MCMuMu_Reco.root

# OO
./finalAnalysis.sh output/DY RECO  $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} --MinZPT $minZpt --MaxZPT $maxZpt -MinTrackPT 0.5 --MaxTrackPT 500  --Input $FILE_RECO --IsGen false --IsPP false --IsOO true --IsGenZ false 
cp output/DY-RECO.root output/DY-RECO-noResidual.root
./finalAnalysis.sh output/DY GEN   $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} --MinZPT $minZpt --MaxZPT $maxZpt -MinTrackPT 0.5 --MaxTrackPT 500  --Input $FILE_GEN --IsGen true --IsPP false --IsOO true --IsGenZ true --InputUE $FILE_GEN_UE

root -l -q -b correction.C
mv correction.root output/correction_1.root
./finalAnalysis.sh output/DY RECO  $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} --MinZPT $minZpt --MaxZPT $maxZpt -MinTrackPT 0.5 --MaxTrackPT 500  --Input $FILE_RECO --residualFile output/correction_1.root --IsGen false --IsPP false --IsOO true --IsGenZ false 
root -l -q -b correction.C
mv correction.root output/correction_2.root
root -l -q -b "merge_corrections.C(\"output/correction_1.root\",\"output/correction_2.root\",\"output/correction_12.root\")"
./finalAnalysis.sh output/DY RECO  $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} --MinZPT $minZpt --MaxZPT $maxZpt -MinTrackPT 0.5 --MaxTrackPT 500  --Input $FILE_RECO     --residualFile output/correction_12.root --IsGen false --IsPP false --IsOO true --IsGenZ false 
root -l -q -b correction.C
mv correction.root output/correction_3.root

root -l -q -b plot_corrections.C
mv corrections.pdf plots/corrections_OO_${name}_${minZpt}_${maxZpt}.pdf
root -l -q -b "merge_corrections.C(\"output/correction_12.root\",\"output/correction_3.root\",\"output/${name}_OO.root\")"
mv output/${name}_OO.root output/${name}_OO_zPt$minZpt-$maxZpt.root

./finalAnalysis.sh output/DY RECO $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} --MinZPT $minZpt --MaxZPT $maxZpt -MinTrackPT 0.5 --MaxTrackPT 500  --Input $FILE_RECO     --residualFile output/${name}_OO_zPt$minZpt-$maxZpt.root --IsGen false --IsPP false --IsOO true --IsGenZ false 
root -l -q -b "plot_closure.C(\"plots/isOO_ZPT${minZpt}_${maxZpt}\")"
