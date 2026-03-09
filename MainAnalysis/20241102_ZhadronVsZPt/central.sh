
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
ZPT_RANGES=("5_30" "30_500")
PT_RANGES=("0.5_4" "4_500") 
EOF

# pp
if [ "$DOPP" == "1" ]; then
    nMix=10
    TAG="_ZV5_trkV23_nmix10"
    VZWeightFile=/home/kdeverea/PhysicsZHadronEEC/Plots/20251001_pPbVZReweighting/20260307_VzReweightFits_pp.root

    ./system-analysis.sh "pythiaMC_Gen_nominal${TAG}" \
        --IsPP true --IsGenZ true --IsData false \
        --Input mergedSample/pythia-gen-v11-Zpt0.root  \
        --MixFile mergedSample/pythia-gen-v11-Zpt0.root  \
        --UseEventWeight false --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --yBoost 0 --nMix $nMix \
        --VZWeightFile $VZWeightFile

    exit

    ./system-analysis.sh "pp_nominal${TAG}" \
        --IsPP true --IsGenZ false --IsData true \
        --Input mergedSample/pp-v11-Zpt0.root \
        --MixFile mergedSample/pp-v11-Zpt0.root \
        --UseEventWeight true --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --yBoost 0 --nMix $nMix

    ./system-analysis.sh "pp_ZResidual${TAG}" \
        --IsPP true --IsGenZ false --IsData true \
        --Input mergedSample/pp-v11-Zpt0.root \
        --MixFile mergedSample/pp-v11-Zpt0.root \
        --UseEventWeight true --UseZWeight true \
        --UseTrackWeight true --UseResidualWeight false \
        --yBoost 0 --nMix $nMix \
        --ZWeightFile my_ZWeights/20260308_ZCorrection_V5_pp_zPt0-500.root

    ./system-analysis.sh "pp_trkResidual${TAG}" \
        --IsPP true --IsGenZ false --IsData true \
        --Input mergedSample/pp-v11-Zpt0.root \
        --MixFile mergedSample/pp-v11-Zpt0.root \
        --UseEventWeight true --UseZWeight true \
        --UseTrackWeight true --UseResidualWeight true \
        --yBoost 0 --nMix $nMix \
        --ZWeightFile my_ZWeights/20260308_ZCorrection_V5_pp_zPt0-500.root \
        --ResidualWeightFile my_residualWeights/20260308_TrackResidualCorrection_V23_ZWeight_V5_pp_zPt   

    ./system-analysis.sh "pp_EExtrapolation${TAG}" \
        --IsPP true --IsGenZ false --IsData true \
        --Input mergedSample/pp-v11-Zpt0.root \
        --MixFile mergedSample/pp-v11-Zpt0.root \
        --UseEventWeight true --UseZWeight true \
        --UseTrackWeight true --UseResidualWeight true \
        --yBoost 0 --nMix $nMix \
        --ZWeightFile my_ZWeights/20260308_ZCorrection_V5_pp_zPt0-500.root \
        --ResidualWeightFile my_residualWeights/20260308_TrackResidualCorrection_V23_ZWeight_V5_pp_zPt \
        --EnergyExtraFile my_EnergyExtrapolation/20260308_EnergyExtrapolation_V2.root

    exit

    
     

fi

# pPb
if [ "$DOPPB" == "1" ]; then
    nMix=10
    TAG="_ZV5_trkV23_nmix10"

    ./system-analysis.sh "pPbMC_Gen_nominal${TAG}" \
        --IsPP false --IsGenZ true --IsData false --IsPPb true \
        --Input pPbSample/V0.2/PPbMC_Gen.root \
        --MixFile pPbSample/V0.2/PPbMC_Gen.root \
        --UseEventWeight true --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --EPOSFile mergedEPOS/PPbMC_Gen.root --Fraction 1 \
        --yBoost 0 --nMix $nMix

    ./system-analysis.sh "pPb_nominal${TAG}" \
        --IsPP false --IsGenZ false --IsData true --IsPPb true \
        --Input pPbSample/V0.2/PPbData_Reco.root \
        --MixFile pPbSample/V0.2/PPbData_Reco.root \
        --UseEventWeight true --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --yBoost 0 --nMix $nMix

    ./system-analysis.sh "pPb_ZResidual${TAG}" \
        --IsPP false --IsGenZ false --IsData true --IsPPb true \
        --Input pPbSample/V0.2/PPbData_Reco.root \
        --MixFile pPbSample/V0.2/PPbData_Reco.root \
        --UseEventWeight true --UseZWeight true \
        --UseTrackWeight true --UseResidualWeight false \
        --yBoost 0 --nMix $nMix \
        --ZWeightFile my_ZWeights/20260202_ZCorrection_V5_PPb_zPt0-500.root

    ./system-analysis.sh "pPb_trkResidual${TAG}" \
        --IsPP false --IsGenZ false --IsData true --IsPPb true \
        --Input pPbSample/V0.2/PPbData_Reco.root \
        --MixFile pPbSample/V0.2/PPbData_Reco.root \
        --UseEventWeight true --UseZWeight true \
        --UseTrackWeight true --UseResidualWeight true \
        --yBoost 0 --nMix $nMix \
        --ZWeightFile my_ZWeights/20260202_ZCorrection_V5_PPb_zPt0-500.root \
        --ResidualWeightFile my_residualWeights/20260202_TrackResidualCorrection_V23_ZWeight_V5_PPb_zPt

    
fi

# PbP
if [ "$DOPBP" == "1" ]; then
    nMix=10
    TAG="_ZV5_trkV23_nmix10"

    ./system-analysis.sh "PbPMC_Gen_nominal${TAG}" \
        --IsPP false --IsGenZ true --IsData false --IsPPb false \
        --Input pPbSample/V0.2/PbPMC_Gen.root \
        --MixFile pPbSample/V0.2/PbPMC_Gen.root \
        --UseEventWeight true --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --EPOSFile mergedEPOS/PbPMC_Gen.root --Fraction 1 \
        --yBoost 0 --nMix $nMix

    ./system-analysis.sh "PbP_nominal${TAG}" \
        --IsPP false --IsGenZ false --IsData true --IsPPb false \
        --Input pPbSample/V0.2/PbPData_Reco.root \
        --MixFile pPbSample/V0.2/PbPData_Reco.root \
        --UseEventWeight true --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --yBoost 0 --nMix $nMix

    ./system-analysis.sh "PbP_ZResidual${TAG}" \
        --IsPP false --IsGenZ false --IsData true --IsPPb false \
        --Input pPbSample/V0.2/PbPData_Reco.root \
        --MixFile pPbSample/V0.2/PbPData_Reco.root \
        --UseEventWeight true --UseZWeight true \
        --UseTrackWeight true --UseResidualWeight false \
        --yBoost 0 --nMix $nMix \
        --ZWeightFile my_ZWeights/20260126_ZCorrection_V4_PbP_zPt0-500.root

    ./system-analysis.sh "PbP_trkResidual${TAG}" \
        --IsPP false --IsGenZ false --IsData true --IsPPb false \
        --Input pPbSample/V0.2/PbPData_Reco.root \
        --MixFile pPbSample/V0.2/PbPData_Reco.root \
        --UseEventWeight true --UseZWeight true \
        --UseTrackWeight true --UseResidualWeight true \
        --yBoost 0 --nMix $nMix \
        --ZWeightFile my_ZWeights/20260202_ZCorrection_V5_PbP_zPt0-500.root \
        --ResidualWeightFile my_residualWeights/20260202_TrackResidualCorrection_V23_ZWeight_V5_PbP_zPt

fi
