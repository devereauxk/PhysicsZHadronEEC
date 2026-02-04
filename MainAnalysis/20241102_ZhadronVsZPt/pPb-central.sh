nMix=10
TAG="_test_noVZ"

./pPbMC_Gen-analysis.sh "nominal${TAG}" \
    --Input pPbSample/V0.2/PPbMC_Gen.root \
    --MixFile pPbSample/V0.2/PPbMC_Gen.root \
    --UseEventWeight true --UseZWeight false \
    --UseTrackWeight true --UseResidualWeight false \
    --EPOSFile mergedEPOS/PPbMC_Gen.root --Fraction 1 \
    --yBoost 0 --nMix $nMix

./pPbMC-analysis.sh "nominal${TAG}" \
    --Input pPbSample/V0.2/PPbMC_Reco.root \
    --MixFile pPbSample/V0.2/PPbMC_Reco.root \
    --UseEventWeight true --UseZWeight false \
    --UseTrackWeight true --UseResidualWeight false \
    --yBoost 0 --nMix $nMix

./pPbMC-analysis.sh "ZResidual${TAG}" \
    --Input pPbSample/V0.2/PPbMC_Reco.root \
    --MixFile pPbSample/V0.2/PPbMC_Reco.root \
    --UseEventWeight true --UseZWeight true \
    --UseTrackWeight true --UseResidualWeight false \
    --ZWeightFile my_ZWeights/20260202_ZCorrection_V5_PPb_zPt0-500.root \
    --yBoost 0 --nMix $nMix

./pPbMC-analysis.sh "trkResidual${TAG}" \
    --Input pPbSample/V0.2/PPbMC_Reco.root \
    --MixFile pPbSample/V0.2/PPbMC_Reco.root \
    --UseEventWeight true --UseZWeight true \
    --UseTrackWeight true --UseResidualWeight true \
    --yBoost 0 --nMix $nMix \
    --ZWeightFile my_ZWeights/20260202_ZCorrection_V5_PPb_zPt0-500.root \
    --ResidualWeightFile my_residualWeights/20260202_TrackResidualCorrection_V23_ZWeight_V5_PPb_zPt
    