
DOPP=$1
DOPPB=$2
DOPBP=$3


cat > config.sh <<EOF
ZPT_RANGES=("0_10" "10_20" "20_40" "40_500")
PT_RANGES=("0.5_500")
EOF

# pp
if [ "$DOPP" == "1" ]; then
    nMix=10
    TAG="_ZV5_trkV23_nmix10"
    VZWeightFile=/home/kdeverea/PhysicsZHadronEEC/Plots/20251001_pPbVZReweighting/20260307_VzReweightFits_pp.root

    ./system-analysis.sh "pythiaMC_trkResidual${TAG}" \
        --IsPP true --IsGenZ false --IsData false \
        --Input mergedSample/pythia-v11-Zpt0.root \
        --MixFile mergedSample/pythia-v11-Zpt0.root \
        --UseEventWeight false --UseZWeight true \
        --UseTrackWeight true --UseResidualWeight true \
        --yBoost 0 --nMix $nMix \
        --VZWeightFile $VZWeightFile \
        --ZWeightFile my_ZWeights/20260308_ZCorrection_V5_pp_zPt0-500.root \
        --ResidualWeightFile my_residualWeights/20260308_TrackResidualCorrection_V23_ZWeight_V5_pp_zPt

    ./system-analysis.sh "pythiaMC_Gen_nominal${TAG}" \
        --IsPP true --IsGenZ true --IsData false \
        --Input mergedSample/pythia-gen-v11-Zpt0.root  \
        --MixFile mergedSample/pythia-gen-v11-Zpt0.root  \
        --UseEventWeight false --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --yBoost 0 --nMix $nMix --VZWeightFile $VZWeightFile

    ./system-analysis.sh "pythiaMC_nominal${TAG}" \
        --IsPP true --IsGenZ false --IsData false \
        --Input mergedSample/pythia-v11-Zpt0.root \
        --MixFile mergedSample/pythia-v11-Zpt0.root \
        --UseEventWeight false --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --yBoost 0 --nMix $nMix --VZWeightFile $VZWeightFile

    ./system-analysis.sh "pythiaMC_ZResidual${TAG}" \
        --IsPP true --IsGenZ false --IsData false \
        --Input mergedSample/pythia-v11-Zpt0.root \
        --MixFile mergedSample/pythia-v11-Zpt0.root \
        --UseEventWeight false --UseZWeight true \
        --UseTrackWeight true --UseResidualWeight false \
        --yBoost 0 --nMix $nMix \
        --VZWeightFile $VZWeightFile \
        --ZWeightFile my_ZWeights/20260308_ZCorrection_V5_pp_zPt0-500.root
        
    
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

    ./system-analysis.sh "pPbMC_nominal${TAG}" \
        --IsPP false --IsGenZ false --IsData false --IsPPb true \
        --Input pPbSample/V0.2/PPbMC_Reco.root \
        --MixFile pPbSample/V0.2/PPbMC_Reco.root \
        --UseEventWeight true --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --yBoost 0 --nMix $nMix

    ./system-analysis.sh "pPbMC_ZResidual${TAG}" \
        --IsPP false --IsGenZ false --IsData false --IsPPb true \
        --Input pPbSample/V0.2/PPbMC_Reco.root \
        --MixFile pPbSample/V0.2/PPbMC_Reco.root \
        --UseEventWeight true --UseZWeight true \
        --UseTrackWeight true --UseResidualWeight false \
        --yBoost 0 --nMix $nMix \
        --ZWeightFile my_ZWeights/20260202_ZCorrection_V5_PPb_zPt0-500.root

    ./system-analysis.sh "pPbMC_trkResidual${TAG}" \
        --IsPP false --IsGenZ false --IsData false --IsPPb true \
        --Input pPbSample/V0.2/PPbMC_Reco.root \
        --MixFile pPbSample/V0.2/PPbMC_Reco.root \
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

    ./system-analysis.sh "PbPMC_nominal${TAG}" \
        --IsPP false --IsGenZ false --IsData false --IsPPb false \
        --Input pPbSample/V0.2/PbPMC_Reco.root \
        --MixFile pPbSample/V0.2/PbPMC_Reco.root \
        --UseEventWeight true --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --yBoost 0 --nMix $nMix

    ./system-analysis.sh "PbPMC_ZResidual${TAG}" \
        --IsPP false --IsGenZ false --IsData false --IsPPb false \
        --Input pPbSample/V0.2/PbPMC_Reco.root \
        --MixFile pPbSample/V0.2/PbPMC_Reco.root \
        --UseEventWeight true --UseZWeight true \
        --UseTrackWeight true --UseResidualWeight false \
        --yBoost 0 --nMix $nMix \
        --ZWeightFile my_ZWeights/20260126_ZCorrection_V4_PbP_zPt0-500.root

    ./system-analysis.sh "PbPMC_trkResidual${TAG}" \
        --IsPP false --IsGenZ false --IsData false --IsPPb false \
        --Input pPbSample/V0.2/PbPMC_Reco.root \
        --MixFile pPbSample/V0.2/PbPMC_Reco.root \
        --UseEventWeight true --UseZWeight true \
        --UseTrackWeight true --UseResidualWeight true \
        --yBoost 0 --nMix $nMix \
        --ZWeightFile my_ZWeights/20260202_ZCorrection_V5_PbP_zPt0-500.root \
        --ResidualWeightFile my_residualWeights/20260202_TrackResidualCorrection_V23_ZWeight_V5_PbP_zPt
fi
