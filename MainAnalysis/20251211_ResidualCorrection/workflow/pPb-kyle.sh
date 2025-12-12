minZpt=40
maxZpt=500

./finalAnalysis.sh output/$1DY RECO  $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} --MinZPT $minZpt --MaxZPT $maxZpt -MinTrackPT 0.5 --MaxTrackPT 500  --MinHiBin -10 --MaxHiBin 1000  --Input pPbSample/V0.2/PbPMC_Reco.root     --IsGen false --IsPP true --IsGenZ true
./finalAnalysis.sh output/$1DY GEN   $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} --MinZPT $minZpt --MaxZPT $maxZpt -MinTrackPT 0.5 --MaxTrackPT 500  --MinHiBin -10 --MaxHiBin 1000  --Input pPbSample/V0.2/PbPMC_Gen.root --InputUE mergedEPOS/PPbMC_Gen.root --IsGen true --IsPP true --IsGenZ true

root -l -q -b correction.C
mv correction.root output/correction_1.root
#./finalAnalysis.sh output/$1DY RECO  $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} --MinZPT $minZpt --MaxZPT $maxZpt -MinTrackPT 0.5 --MaxTrackPT 500  --MinHiBin -10 --MaxHiBin 1000  --Input mergedSample/PbPMC_Reco_v0.2.root     --residualFile output/correction_1.root --IsGen false --IsPP true --IsGenZ true
#root -l -q -b correction.C
#mv correction.root output/correction_2.root
#root -l -q -b "merge_corrections.C(\"output/correction_1.root\",\"output/correction_2.root\",\"output/correction_12.root\")"
#./finalAnalysis.sh output/$1DY RECO  $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} --MinZPT $minZpt --MaxZPT $maxZpt -MinTrackPT 0.5 --MaxTrackPT 500  --MinHiBin -10 --MaxHiBin 1000  --Input mergedSample/PbPMC_Reco_v0.2.root     --residualFile output/correction_12.root --IsGen false --IsPP true --IsGenZ true
#root -l -q -b correction.C
#mv correction.root output/correction_3.root

#root -l -q -b plot_corrections.C
#mv corrections.pdf corrections_PbP.pdf
#root -l -q -b "merge_corrections.C(\"output/correction_12.root\",\"output/correction_3.root\",\"output/20251023_TrackResidualCorrection_V14_PbP.root\")"

#mv output/20251023_TrackResidualCorrection_V14_PbP.root output/20251023_TrackResidualCorrection_V14_PbP_zPt$minZpt-$maxZpt.root