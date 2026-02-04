nMix=10
TAG="_ZV5_trkV23_nmix10"

./PbPMC_Gen-analysis.sh "nominal${TAG}" \
    --Input pPbSample/V0.2/PbPMC_Gen.root \
    --MixFile pPbSample/V0.2/PbPMC_Gen.root \
    --UseEventWeight true --UseZWeight false \
    --UseTrackWeight true --UseResidualWeight false \
    --EPOSFile mergedEPOS/PbPMC_Gen.root --Fraction 1 \
    --yBoost 0 --nMix $nMix

./PbPMC-analysis.sh "nominal${TAG}" \
    --Input pPbSample/V0.2/PbPMC_Reco.root \
    --MixFile pPbSample/V0.2/PbPMC_Reco.root \
    --UseEventWeight true --UseZWeight false \
    --UseTrackWeight true --UseResidualWeight false \
    --yBoost 0 --nMix $nMix

./PbPMC-analysis.sh "ZResidual${TAG}" \
    --Input pPbSample/V0.2/PbPMC_Reco.root \
    --MixFile pPbSample/V0.2/PbPMC_Reco.root \
    --UseEventWeight true --UseZWeight true \
    --UseTrackWeight true --UseResidualWeight false \
    --ZWeightFile my_ZWeights/20260202_ZCorrection_V5_PbP_zPt0-500.root \
    --yBoost 0 --nMix $nMix

./PbPMC-analysis.sh "trkResidual${TAG}" \
    --Input pPbSample/V0.2/PbPMC_Reco.root \
    --MixFile pPbSample/V0.2/PbPMC_Reco.root \
    --UseEventWeight true --UseZWeight true \
    --UseTrackWeight true --UseResidualWeight true \
    --yBoost 0 --nMix $nMix \
    --ZWeightFile my_ZWeights/20260202_ZCorrection_V5_PbP_zPt0-500.root \
    --ResidualWeightFile my_residualWeights/20260202_TrackResidualCorrection_V23_ZWeight_V5_PbP_zPt
    