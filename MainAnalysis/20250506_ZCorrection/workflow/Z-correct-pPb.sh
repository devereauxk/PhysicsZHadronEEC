#### PPb correction
rm Zcorrectionfactor/*
rm ../ExecuteCorrelationAnalysis
make -C ..

# Data(reco)
./pPb-analysis.sh 1 --UseZCor false

# MC(gen)
./pPbMC_Gen-analysis.sh --UseZCor false

# MC(reco)(0)
./pPbMC-analysis.sh --UseZCor false
cp output/pPbMC_ZPT0_100-0_40.root output/pPbMC_ZPT0_100-0_40-0.root

for i in 1 2 3 4; do
    echo "Iteration $i"

    ############ pt correction ############
    # c(0) = 1
    # c(i) = c(i-1) * Data(reco) / MC(reco)(i-1)
    root -l -b -q "ZCorrection.C(\"output/pPbMC_ZPT0_100-0_40-$((i-1)).root\", \"output/1pPb_ZPT0_100-0_40.root\", \"Zcorrectionfactor/totalCorrection.root\", true, false)"

    # MC(reco)(i) = MC(gen) * c(i)
    rm ../ExecuteCorrelationAnalysis
    make -C ..
    ./pPbMC-analysis.sh --UseZCor true --ZCor Zcorrectionfactor/totalCorrection.root
    cp output/pPbMC_ZPT0_100-0_40.root output/pPbMC_ZPT0_100-0_40-$i-pt.root


    ############ eta correction ############
    # c(0) = 1
    # c(i) = c(i-1) * Data(reco) / MC(reco)(i-1)
    root -l -b -q "ZCorrection.C(\"output/pPbMC_ZPT0_100-0_40-$i-pt.root\", \"output/1pPb_ZPT0_100-0_40.root\", \"Zcorrectionfactor/totalCorrection.root\", false, true)"

    # MC(reco)(i) = MC(gen) * c(i)
    rm ../ExecuteCorrelationAnalysis
    make -C ..
    ./pPbMC-analysis.sh --UseZCor true --ZCor Zcorrectionfactor/totalCorrection.root
    cp output/pPbMC_ZPT0_100-0_40.root output/pPbMC_ZPT0_100-0_40-$i.root

done

root -l -b -q "plotResidualCor.C(\"0_100\", \"0_40\", \"PPb\", true)"
