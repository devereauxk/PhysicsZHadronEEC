

cat > config.sh <<EOF
ZPT_RANGES=("0.5_500")
PT_RANGES=("0.5_500")
EOF

TAG="_EEV1_ZV4_trkV22_nmix10"


./system-analysis.sh "pp_EE${TAG}" \
    --IsPP true --IsGenZ false --IsData true \
    --Input mergedSample/pp-v11-Zpt0.root \
    --MixFile mergedSample/pp-v11-Zpt0.root \
    --UseEventWeight true --UseZWeight true \
    --UseTrackWeight true --UseResidualWeight true \
    --yBoost 0 --nMix $nMix \
    --ZWeightFile my_ZWeights/20260129_ZCorrection_V4_pp_zPt0-500.root \
    --ResidualWeightFile my_residualWeights/20260129_TrackResidualCorrection_V22_ZWeight_V4_pp_zPt \
    --EnergyExtraFile my_EnergyExtrapolation/20260222_EnergyExtrapolation_V1.root

./system-analysis.sh "pp_ZResidual${TAG}" \
    --IsPP true --IsGenZ false --IsData true \
    --Input mergedSample/pp-v11-Zpt0.root \
    --MixFile mergedSample/pp-v11-Zpt0.root \
    --UseEventWeight true --UseZWeight true \
    --UseTrackWeight true --UseResidualWeight false \
    --yBoost 0 --nMix $nMix \
    --ZWeightFile my_ZWeights/20260129_ZCorrection_V4_pp_zPt0-500.root



TAG="_ZV5_trkV23_nmix10"

./system-analysis.sh "pPb_ZResidual${TAG}" \
    --IsPP false --IsGenZ false --IsData true --IsPPb true \
    --Input pPbSample/V0.2/PPbData_Reco.root \
    --MixFile pPbSample/V0.2/PPbData_Reco.root \
    --UseEventWeight true --UseZWeight true \
    --UseTrackWeight true --UseResidualWeight false \
    --yBoost 0 --nMix $nMix \
    --ZWeightFile my_ZWeights/20260202_ZCorrection_V5_PPb_zPt0-500.root

./system-analysis.sh "PbP_ZResidual${TAG}" \
    --IsPP false --IsGenZ false --IsData true --IsPPb false \
    --Input pPbSample/V0.2/PbPData_Reco.root \
    --MixFile pPbSample/V0.2/PbPData_Reco.root \
    --UseEventWeight true --UseZWeight true \
    --UseTrackWeight true --UseResidualWeight false \
    --yBoost 0 --nMix $nMix \
    --ZWeightFile my_ZWeights/20260126_ZCorrection_V4_PbP_zPt0-500.root

