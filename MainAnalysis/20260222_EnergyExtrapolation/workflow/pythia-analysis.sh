WORKINGDIR=$(pwd)
cd ..
./clean.sh
cd $WORKINGDIR
sleep 1

minZpt=0
maxZpt=500
name="20260317_EnergyExtrapolation_EEV3"

# pp
./finalAnalysis.sh output/pp 502  $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} --MinZPT $minZpt --MaxZPT $maxZpt -MinTrackPT 0.5 --MaxTrackPT 500  --Input mergedSample/pp-v11-Zpt0.root     --IsGen false --IsPP true --IsGenZ false --residualFile my_ZWeights/20260317_ZCorrection_V6_pp_zPt0-500.root --VZWeightFile /home/kdeverea/PhysicsZHadronEEC/Plots/20251001_pPbVZReweighting/summary/20260317_ZV6_ZPT0_350_VzReweightFits_pp.root

./finalAnalysis.sh output/pPb 816   $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} --MinZPT $minZpt --MaxZPT $maxZpt -MinTrackPT 0.5 --MaxTrackPT 500  --Input pPbSample/V0.2/PPbData_Reco.root --IsGen false --IsPP false --IsGenZ false --residualFile my_ZWeights/20260202_ZCorrection_V5_PPb_zPt0-500.root
./finalAnalysis.sh output/PbP 816   $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} --MinZPT $minZpt --MaxZPT $maxZpt -MinTrackPT 0.5 --MaxTrackPT 500  --Input pPbSample/V0.2/PbPData_Reco.root --IsGen false --IsPP false --IsGenZ false --residualFile my_ZWeights/20260202_ZCorrection_V5_PbP_zPt0-500.root
hadd -f output/pPb-PbP-816.root output/pPb-816.root output/PbP-816.root

root -l -q -b correction.C

root -l -q -b plot_corrections.C
mv correction.root output/${name}.root
mv corrections.pdf output/${name}.pdf

./finalAnalysis.sh output/pp 816  $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} --MinZPT $minZpt --MaxZPT $maxZpt -MinTrackPT 0.5 --MaxTrackPT 500  --Input mergedSample/pp-v11-Zpt0.root     --IsGen false --IsPP true --IsGenZ false --residualFile my_ZWeights/20260317_ZCorrection_V6_pp_zPt0-500.root --VZWeightFile /home/kdeverea/PhysicsZHadronEEC/Plots/20251001_pPbVZReweighting/summary/20260317_ZV6_ZPT0_350_VzReweightFits_pp.root --EnergyExtraFile output/${name}.root
root -l -q -b "plot_closure.C(\"output/${name}-closure.pdf\")"
