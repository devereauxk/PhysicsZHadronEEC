
DOPP=$1
DOPPB=$2
DOPBP=$3

cat > config.sh <<EOF
ZPT_RANGES=("0_10" "10_500")
PT_RANGES=("0.5_2" "2_500")
EOF

# pp
if [ "$DOPP" == "1" ]; then
    nMix=10
    TAG="_ZV4_trkV22_nmix10"
    
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
        --ZWeightFile my_ZWeights/20260129_ZCorrection_V4_pp_zPt0-500.root

    ./system-analysis.sh "pp_trkResidual${TAG}" \
        --IsPP true --IsGenZ false --IsData true \
        --Input mergedSample/pp-v11-Zpt0.root \
        --MixFile mergedSample/pp-v11-Zpt0.root \
        --UseEventWeight true --UseZWeight true \
        --UseTrackWeight true --UseResidualWeight true \
        --yBoost 0 --nMix $nMix \
        --ZWeightFile my_ZWeights/20260129_ZCorrection_V4_pp_zPt0-500.root \
        --ResidualWeightFile my_residualWeights/20260129_TrackResidualCorrection_V22_ZWeight_V4_pp_zPt
fi

# pPb
if [ "$DOPPB" == "1" ]; then
    nMix=10
    TAG="_ZV5_trkV23_nmix10"

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
