WORKINGDIR=$(pwd)
cd ..
./clean.sh
cd $WORKINGDIR
sleep 1

minZpt=0
maxZpt=500

./finalAnalysis.sh output/DY RECO  $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} --MinZPT $minZpt --MaxZPT $maxZpt -MinTrackPT 0.5 --MaxTrackPT 500  --MinHiBin -10 --MaxHiBin 1000  --Input mergedSample/PPbMC_Reco_v0.2.root     --IsGen false --IsPP false --IsGenZ false
cp output/DY-RECO.root output/DY-RECO-eventWeight-pPb.root
./finalAnalysis.sh output/DY RECO  $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} --MinZPT $minZpt --MaxZPT $maxZpt -MinTrackPT 0.5 --MaxTrackPT 500  --MinHiBin -10 --MaxHiBin 1000  --Input mergedSample/PbPMC_Reco_v0.2.root     --IsGen false --IsPP false --IsGenZ false
cp output/DY-RECO.root output/DY-RECO-eventWeight-PbP.root 

root -l -q -b "plot_eventweights.C(\"RECO\", \"plots/RECO-V1-trackweight\")"

./finalAnalysis.sh output/DY GEN  $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} --MinZPT $minZpt --MaxZPT $maxZpt -MinTrackPT 0.5 --MaxTrackPT 500  --MinHiBin -10 --MaxHiBin 1000  --Input mergedSample/PPbMC_Gen_v0.2.root     --IsGen false --IsPP false --IsGenZ false --InputUE mergedSample/PPbEPOS_Gen_v0.2_1XX.root
cp output/DY-GEN.root output/DY-GEN-eventWeight-pPb.root
./finalAnalysis.sh output/DY GEN  $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} --MinZPT $minZpt --MaxZPT $maxZpt -MinTrackPT 0.5 --MaxTrackPT 500  --MinHiBin -10 --MaxHiBin 1000  --Input mergedSample/PbPMC_Gen_v0.2.root     --IsGen false --IsPP false --IsGenZ false --InputUE mergedSample/PbPEPOS_Gen_v0.2_1XX.root
cp output/DY-GEN.root output/DY-GEN-eventWeight-PbP.root 

root -l -q -b "plot_eventweights.C(\"GEN\", \"plots/GEN-V1-trackweight\")"