nMix=10
TAG="_Zclosure"

# pPb
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

./pPbMC-analysis.sh "residual${TAG}" \
    --Input pPbSample/V0.2/PPbMC_Reco.root \
    --MixFile pPbSample/V0.2/PPbMC_Reco.root \
    --UseEventWeight true --UseZWeight true \
    --UseTrackWeight true --UseResidualWeight false \
    --yBoost 0 --nMix $nMix \
    --ZWeightFile my_ZWeights/20260126_ZCorrection_V4_PPb_zPt0-500.root

# PbP
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

./PbPMC-analysis.sh "residual${TAG}" \
    --Input pPbSample/V0.2/PbPMC_Reco.root \
    --MixFile pPbSample/V0.2/PbPMC_Reco.root \
    --UseEventWeight true --UseZWeight true \
    --UseTrackWeight true --UseResidualWeight false \
    --yBoost 0 --nMix $nMix \
    --ZWeightFile my_ZWeights/20260126_ZCorrection_V4_PbP_zPt0-500.root

