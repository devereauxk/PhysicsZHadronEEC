#### PPb correction
rm residualCorrection/*
rm ../ExecuteCorrelationAnalysis
make -C ..

./pPbMC_Gen-analysis.sh
./pPbMC-analysis.sh --UseZCor true --ZCor ZCorrection/ZCorrection-PPb.root --UseResidualCor false
cp output/pPbMC_ZPT0_100-0_40.root output/pPbMC_ZPT0_100-0_40-0.root

for i in 1; do
    echo "Iteration $i"

    # pt reweighting
    root -l -b -q "TrackResidualCorrection.C(\"output/pPbMC_ZPT0_100-0_40.root\", \"output/pPbMC_Gen_ZPT0_100-0_40.root\", \"residualCorrection/residualCorrection-PPb.root\", $i, true, false, false)"
    rm ../ExecuteCorrelationAnalysis
    make -C ..

    ./pPbMC-analysis.sh --UseZCor true --ZCor ZCorrection/ZCorrection-PPb.root --UseResidualCor true --ResidualCor residualCorrection/residualCorrection-PPb.root
    cp output/pPbMC_ZPT0_100-0_40.root output/pPbMC_ZPT0_100-0_40-$i-pt.root

    # eta reweighting
    root -l -b -q "TrackResidualCorrection.C(\"output/pPbMC_ZPT0_100-0_40.root\", \"output/pPbMC_Gen_ZPT0_100-0_40.root\", \"residualCorrection/residualCorrection-PPb.root\", $i, false, true, false)"
    make -C ..

    ./pPbMC-analysis.sh --UseZCor true --ZCor ZCorrection/ZCorrection-PPb.root --UseResidualCor true --ResidualCor residualCorrection/residualCorrection-PPb.root
    cp output/pPbMC_ZPT0_100-0_40.root output/pPbMC_ZPT0_100-0_40-$i-pteta.root

    # phi reweighting
    root -l -b -q "TrackResidualCorrection.C(\"output/pPbMC_ZPT0_100-0_40.root\", \"output/pPbMC_Gen_ZPT0_100-0_40.root\", \"residualCorrection/residualCorrection-PPb.root\", $i, false, false, true)"
    rm ../ExecuteCorrelationAnalysis
    make -C ..

    ./pPbMC-analysis.sh --UseZCor true --ZCor ZCorrection/ZCorrection-PPb.root --UseResidualCor true --ResidualCor residualCorrection/residualCorrection-PPb.root
    cp output/pPbMC_ZPT0_100-0_40.root output/pPbMC_ZPT0_100-0_40-$i-ptetaphi.root
done

root -l -b -q "plotResidualCor.C(\"0_100\", \"0_40\", \"PPb\", true)"

exit


#### PbP correction
rm ZCorrection-PbP.root
rm ../ExecuteCorrelationAnalysis
make -C ..

./pPb-analysis.sh 0

./PbPMC-analysis.sh --UseZCor false
cp output/PbPMC_ZPT0_100-0_40.root output/PbPMC_ZPT0_100-0_40-0.root

root -l -b -q "plotZCorr.C(\"0_100\", \"0_40\", \"PbP\", false)"

for i in 1 2 3 4; do
    echo "Iteration $i"

    # pt reweighting
    root -l -b -q "ZCorrection.C(\"output/PbPMC_ZPT0_100-0_40.root\", \"output/0pPb_ZPT0_100-0_40.root\", \"PbP\", $i, true, false, false)"
    rm ../ExecuteCorrelationAnalysis
    make -C ..

    ./PbPMC-analysis.sh --UseZCor true --ZCor ZCorrection-PbP
    cp output/PbPMC_ZPT0_100-0_40.root output/PbPMC_ZPT0_100-0_40-$i-pt.root

    # eta reweighting
    root -l -b -q "ZCorrection.C(\"output/PbPMC_ZPT0_100-0_40.root\", \"output/0pPb_ZPT0_100-0_40.root\", \"PbP\", $i, false, true, false)"
    rm ../ExecuteCorrelationAnalysis
    make -C ..

    ./PbPMC-analysis.sh --UseZCor true --ZCor ZCorrection-PbP
    cp output/PbPMC_ZPT0_100-0_40.root output/PbPMC_ZPT0_100-0_40-$i-pteta.root

    # mult reweighting
    root -l -b -q "ZCorrection.C(\"output/PbPMC_ZPT0_100-0_40.root\", \"output/0pPb_ZPT0_100-0_40.root\", \"PbP\", $i, false, false, true)"
    rm ../ExecuteCorrelationAnalysis
    make -C ..

    ./PbPMC-analysis.sh --UseZCor true --ZCor ZCorrection-PbP
    cp output/PbPMC_ZPT0_100-0_40.root output/PbPMC_ZPT0_100-0_40-$i-ptetamult.root
done

root -l -b -q "plotZCorr.C(\"0_100\", \"0_40\", \"PbP\", false)"
