#### PPb correction
rm residualCorrection/*
rm ../ExecuteCorrelationAnalysis
make -C ..

./pPb-analysis.sh 1

./pPbMC-analysis.sh --UseZCor true --ZCor ZCorrection/ZCorrection-PPb
cp output/pPbMC_ZPT0_100-0_40.root output/pPbMC_ZPT0_100-0_40-0.root

exit

for i in 1 2 3 4; do
    echo "Iteration $i"

    # pt reweighting
    root -l -b -q "ZCorrection.C(\"output/pPbMC_ZPT0_100-0_40.root\", \"output/1pPb_ZPT0_100-0_40.root\", \"PPb\", $i, true, false, false)"
    rm ../ExecuteCorrelationAnalysis
    make -C ..

    ./pPbMC-analysis.sh --UseZCor true --ZCor ZCorrection-PPb
    cp output/pPbMC_ZPT0_100-0_40.root output/pPbMC_ZPT0_100-0_40-$i-pt.root

    # eta reweighting
    root -l -b -q "ZCorrection.C(\"output/pPbMC_ZPT0_100-0_40.root\", \"output/1pPb_ZPT0_100-0_40.root\", \"PPb\", $i, false, true, false)"
    rm ../ExecuteCorrelationAnalysis
    make -C ..

    ./pPbMC-analysis.sh --UseZCor true --ZCor ZCorrection-PPb
    cp output/pPbMC_ZPT0_100-0_40.root output/pPbMC_ZPT0_100-0_40-$i-pteta.root

    # mult reweighting
    root -l -b -q "ZCorrection.C(\"output/pPbMC_ZPT0_100-0_40.root\", \"output/1pPb_ZPT0_100-0_40.root\", \"PPb\", $i, false, false, true)"
    rm ../ExecuteCorrelationAnalysis
    make -C ..

    ./pPbMC-analysis.sh --UseZCor true --ZCor ZCorrection-PPb
    cp output/pPbMC_ZPT0_100-0_40.root output/pPbMC_ZPT0_100-0_40-$i-ptetamult.root
done

root -l -b -q "plotZCorr.C(\"0_100\", \"0_40\", \"PPb\", true)"


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
