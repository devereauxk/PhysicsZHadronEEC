
DOPP=$1
DOPPB=$2
DOPBP=$3

# CENTRAL CLOSURE
#ZPT_RANGES=("0_10" "10_20" "20_40" "40_500")
#PT_RANGES=("0.5_500")

# ENERGY EXTRAPOLATION
#ZPT_RANGES=("5_500")
#PT_RANGES=("0.5_500")

cat > config.sh <<EOF
ZPT_RANGES=("5_500")
PT_RANGES=("0.5_500") 
EOF

VZWeightFile_PP="/home/kdeverea/PhysicsZHadronEEC/Plots/20251001_pPbVZReweighting/summary/20260320_skimVZOff_ZPT0_350_VzReweightFits_pp.root"
VZWeightFile_PPb="/home/kdeverea/PhysicsZHadronEEC/Plots/20251001_pPbVZReweighting/summary/20260320_pPbPbpRelabelFix_skimVZOff_ZPT0_500_VzReweightFits_pPb.root"
VZWeightFile_PbP="/home/kdeverea/PhysicsZHadronEEC/Plots/20251001_pPbVZReweighting/summary/20260320_pPbPbpRelabelFix_skimVZOff_ZPT0_500_VzReweightFits_PbP.root"
ZWeightFile_PPb="my_ZWeights/20260320_ZCorrection_V6_skimVZOff_PPb_zPt0-500.root"
ZWeightFile_PbP="my_ZWeights/20260320_ZCorrection_V6_skimVZOff_PbP_zPt0-500.root"
ResidualWeightFile_PPb="my_residualWeights/20260320_TrackResidualCorrection_V24_ZWeight_V6_skimVZOff_PPb_zPt"
ResidualWeightFile_PbP="my_residualWeights/20260320_TrackResidualCorrection_V24_ZWeight_V6_skimVZOff_PbP_zPt"

# pp
if [ "$DOPP" == "1" ]; then

    echo "not implemented"
    exit

    nMix=10
    TAG="_ZV5_trkV23_nmix10"
    VZWeightFile=/home/kdeverea/PhysicsZHadronEEC/Plots/20251001_pPbVZReweighting/20260320_skimVZOff_ZPT0_350_VzReweightFits_pp.root

    #./system-analysis.sh "pythiaMC_Gen_nominal${TAG}" \
    #    --IsPP true --IsGenZ true --IsData false \
    #    --Input mergedSample/pythia-gen-v11-Zpt0.root  \
    #    --MixFile mergedSample/pythia-gen-v11-Zpt0.root  \
    #    --UseEventWeight false --UseZWeight false \
    #    --UseTrackWeight true --UseResidualWeight false \
    #    --yBoost 0 --nMix $nMix \
    #    --UseVZWeight true --VZWeightFile $VZWeightFile

    ./system-analysis.sh "pp_nominal${TAG}" \
        --IsPP true --IsGenZ false --IsData true --UseVZWeight false \
        --Input mergedSample/pp-v11-Zpt0.root \
        --MixFile mergedSample/pp-v11-Zpt0.root \
        --UseEventWeight true --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --yBoost 0 --nMix $nMix

    ./system-analysis.sh "pp_ZResidual${TAG}" \
        --IsPP true --IsGenZ false --IsData true --UseVZWeight false \
        --Input mergedSample/pp-v11-Zpt0.root \
        --MixFile mergedSample/pp-v11-Zpt0.root \
        --UseEventWeight true --UseZWeight true \
        --UseTrackWeight true --UseResidualWeight false \
        --yBoost 0 --nMix $nMix \
        --ZWeightFile my_ZWeights/20260308_ZCorrection_V5_pp_zPt0-500.root

    ./system-analysis.sh "pp_trkResidual${TAG}" \
        --IsPP true --IsGenZ false --IsData true --UseVZWeight false \
        --Input mergedSample/pp-v11-Zpt0.root \
        --MixFile mergedSample/pp-v11-Zpt0.root \
        --UseEventWeight true --UseZWeight true \
        --UseTrackWeight true --UseResidualWeight true \
        --yBoost 0 --nMix $nMix \
        --ZWeightFile my_ZWeights/20260308_ZCorrection_V5_pp_zPt0-500.root \
        --ResidualWeightFile my_residualWeights/20260308_TrackResidualCorrection_V23_ZWeight_V5_pp_zPt   

fi

# pPb
if [ "$DOPPB" == "1" ]; then
    nMix=0
    TAG="_ZV6_trkV24_vz20260320_nmix0"

    ./system-analysis.sh "pythiapPbEPOSMC_Gen_nominal${TAG}" \
        --IsPP true --IsGenZ true --IsData false \
        --Input mergedSample/pythia-gen-v11-Zpt0.root  \
        --MixFile mergedSample/pythia-gen-v11-Zpt0.root  \
        --UseEventWeight true --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --yBoost 0 --nMix $nMix \
        --EPOSFile mergedEPOS/PPbMC_Gen.root --Fraction 1 \
        --UseVZWeight true --VZWeightFile $VZWeightFile_PP

    ./system-analysis.sh "pPbNoEPOSMC_Gen_nominal${TAG}" \
        --IsPP false --IsGenZ true --IsData false --IsPPb true \
        --Input pPbSample/V0.2/PPbMC_Gen.root \
        --MixFile pPbSample/V0.2/PPbMC_Gen.root \
        --UseEventWeight true --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --Fraction 1 \
        --yBoost 0 --nMix $nMix \
        --UseVZWeight true --VZWeightFile $VZWeightFile_PPb

    ./system-analysis.sh "pPbMC_Gen_nominal${TAG}" \
        --IsPP false --IsGenZ true --IsData false --IsPPb true \
        --Input pPbSample/V0.2/PPbMC_Gen.root \
        --MixFile pPbSample/V0.2/PPbMC_Gen.root \
        --UseEventWeight true --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --EPOSFile mergedEPOS/PPbMC_Gen.root --Fraction 1 \
        --yBoost 0 --nMix $nMix \
        --UseVZWeight true --VZWeightFile $VZWeightFile_PPb

    ./system-analysis.sh "pPbMC_nominal${TAG}" \
        --IsPP false --IsGenZ false --IsData false --IsPPb true \
        --Input pPbSample/V0.2/PPbMC_Reco.root \
        --MixFile pPbSample/V0.2/PPbMC_Reco.root \
        --UseEventWeight true --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --yBoost 0 --nMix $nMix \
        --UseVZWeight true --VZWeightFile $VZWeightFile_PPb

    ./system-analysis.sh "pPbMC_trkResidual${TAG}" \
        --IsPP false --IsGenZ false --IsData false --IsPPb true \
        --Input pPbSample/V0.2/PPbMC_Reco.root \
        --MixFile pPbSample/V0.2/PPbMC_Reco.root \
        --UseEventWeight true --UseZWeight true \
        --UseTrackWeight true --UseResidualWeight true \
        --yBoost 0 --nMix $nMix \
        --UseVZWeight true --VZWeightFile $VZWeightFile_PPb \
        --ZWeightFile $ZWeightFile_PPb \
        --ResidualWeightFile $ResidualWeightFile_PPb

    ./system-analysis.sh "pPb_nominal${TAG}" \
        --IsPP false --IsGenZ false --IsData true --UseVZWeight false --IsPPb true \
        --Input pPbSample/V0.2/PPbData_Reco.root \
        --MixFile pPbSample/V0.2/PPbData_Reco.root \
        --UseEventWeight true --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --yBoost 0 --nMix $nMix

    ./system-analysis.sh "pPb_trkResidual${TAG}" \
        --IsPP false --IsGenZ false --IsData true --UseVZWeight false --IsPPb true \
        --Input pPbSample/V0.2/PPbData_Reco.root \
        --MixFile pPbSample/V0.2/PPbData_Reco.root \
        --UseEventWeight true --UseZWeight true \
        --UseTrackWeight true --UseResidualWeight true \
        --yBoost 0 --nMix $nMix \
        --ZWeightFile $ZWeightFile_PPb \
        --ResidualWeightFile $ResidualWeightFile_PPb

    
fi

# PbP
if [ "$DOPBP" == "1" ]; then
    nMix=0
    TAG="_ZV6_trkV24_vz20260320_nmix0"

    
    ./system-analysis.sh "pythiaPbPEPOSMC_Gen_nominal${TAG}" \
        --IsPP true --IsGenZ true --IsData false \
        --Input mergedSample/pythia-gen-v11-Zpt0.root  \
        --MixFile mergedSample/pythia-gen-v11-Zpt0.root  \
        --UseEventWeight false --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --yBoost 0 --nMix $nMix \
        --EPOSFile mergedEPOS/PbPMC_Gen.root --Fraction 1 \
        --UseVZWeight true --VZWeightFile $VZWeightFile_PP

    ./system-analysis.sh "PbPNoEPOSMC_Gen_nominal${TAG}" \
        --IsPP false --IsGenZ true --IsData false --IsPPb false \
        --Input pPbSample/V0.2/PbPMC_Gen.root \
        --MixFile pPbSample/V0.2/PbPMC_Gen.root \
        --UseEventWeight true --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --Fraction 1 \
        --yBoost 0 --nMix $nMix \
        --UseVZWeight true --VZWeightFile $VZWeightFile_PbP

    ./system-analysis.sh "PbPMC_Gen_nominal${TAG}" \
        --IsPP false --IsGenZ true --IsData false --IsPPb false \
        --Input pPbSample/V0.2/PbPMC_Gen.root \
        --MixFile pPbSample/V0.2/PbPMC_Gen.root \
        --UseEventWeight true --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --EPOSFile mergedEPOS/PbPMC_Gen.root --Fraction 1 \
        --yBoost 0 --nMix $nMix \
        --UseVZWeight true --VZWeightFile $VZWeightFile_PbP

    ./system-analysis.sh "PbPMC_nominal${TAG}" \
        --IsPP false --IsGenZ false --IsData false --IsPPb false \
        --Input pPbSample/V0.2/PbPMC_Reco.root \
        --MixFile pPbSample/V0.2/PbPMC_Reco.root \
        --UseEventWeight true --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --yBoost 0 --nMix $nMix \
        --UseVZWeight true --VZWeightFile $VZWeightFile_PbP

    ./system-analysis.sh "PbPMC_trkResidual${TAG}" \
        --IsPP false --IsGenZ false --IsData false --IsPPb false \
        --Input pPbSample/V0.2/PbPMC_Reco.root \
        --MixFile pPbSample/V0.2/PbPMC_Reco.root \
        --UseEventWeight true --UseZWeight true \
        --UseTrackWeight true --UseResidualWeight true \
        --yBoost 0 --nMix $nMix \
        --UseVZWeight true --VZWeightFile $VZWeightFile_PbP \
        --ZWeightFile $ZWeightFile_PbP \
        --ResidualWeightFile $ResidualWeightFile_PbP

    ./system-analysis.sh "PbP_nominal${TAG}" \
        --IsPP false --IsGenZ false --IsData true --UseVZWeight false --IsPPb false \
        --Input pPbSample/V0.2/PbPData_Reco.root \
        --MixFile pPbSample/V0.2/PbPData_Reco.root \
        --UseEventWeight true --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --yBoost 0 --nMix $nMix

    ./system-analysis.sh "PbP_trkResidual${TAG}" \
        --IsPP false --IsGenZ false --IsData true --UseVZWeight false --IsPPb false \
        --Input pPbSample/V0.2/PbPData_Reco.root \
        --MixFile pPbSample/V0.2/PbPData_Reco.root \
        --UseEventWeight true --UseZWeight true \
        --UseTrackWeight true --UseResidualWeight true \
        --yBoost 0 --nMix $nMix \
        --ZWeightFile $ZWeightFile_PbP \
        --ResidualWeightFile $ResidualWeightFile_PbP

fi
