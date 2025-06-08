#### PbP correction
rm residualCorrection/residualCorrection-PbP.root
rm ../ExecuteCorrelationAnalysis
make -C ..

./pPb-analysis.sh 0
./PbPMC_Gen-analysis.sh
./PbPMC-analysis.sh --UseZCor true --ZCor ZCorrection/ZCorrection-PbP.root --UseResidualCor false
cp output/PbPMC_ZPT0_100-0_40.root output/PbPMC_ZPT0_100-0_40-0.root

for i in 1 2; do
    echo "Iteration $i"

    # pt reweighting
    root -l -b -q "TrackResidualCorrection.C(\"output/PbPMC_ZPT0_100-0_40.root\", \"output/PbPMC_Gen_ZPT0_100-0_40.root\", \"residualCorrection/residualCorrection-PbP.root\", $i, true, false, false)"
    rm ../ExecuteCorrelationAnalysis
    make -C ..

    ./PbPMC-analysis.sh --UseZCor true --ZCor ZCorrection/ZCorrection-PbP.root --UseResidualCor true --ResidualCor residualCorrection/residualCorrection-PbP.root
    cp output/PbPMC_ZPT0_100-0_40.root output/PbPMC_ZPT0_100-0_40-$i-pt.root

    # eta reweighting
    root -l -b -q "TrackResidualCorrection.C(\"output/PbPMC_ZPT0_100-0_40.root\", \"output/PbPMC_Gen_ZPT0_100-0_40.root\", \"residualCorrection/residualCorrection-PbP.root\", $i, false, true, false)"
    make -C ..

    ./PbPMC-analysis.sh --UseZCor true --ZCor ZCorrection/ZCorrection-PbP.root --UseResidualCor true --ResidualCor residualCorrection/residualCorrection-PbP.root
    cp output/PbPMC_ZPT0_100-0_40.root output/PbPMC_ZPT0_100-0_40-$i-pteta.root

    # phi reweighting
    root -l -b -q "TrackResidualCorrection.C(\"output/PbPMC_ZPT0_100-0_40.root\", \"output/PbPMC_Gen_ZPT0_100-0_40.root\", \"residualCorrection/residualCorrection-PbP.root\", $i, false, false, true)"
    rm ../ExecuteCorrelationAnalysis
    make -C ..

    ./PbPMC-analysis.sh --UseZCor true --ZCor ZCorrection/ZCorrection-PbP.root --UseResidualCor true --ResidualCor residualCorrection/residualCorrection-PbP.root
    cp output/PbPMC_ZPT0_100-0_40.root output/PbPMC_ZPT0_100-0_40-$i-ptetaphi.root
done

root -l -b -q "plotResidualCor.C(\"0_100\", \"0_40\", \"PbP\", false)"

