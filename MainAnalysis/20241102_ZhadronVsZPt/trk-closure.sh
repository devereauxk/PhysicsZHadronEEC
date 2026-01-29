nMix=10
TAG="_V22_nmix10"

DOPP=$1
DOPPB=$2
DOPBP=$3

# pp
if [ "$DOPP" == "1" ]; then
    ./pythia_Gen-analysis.sh "nominal${TAG}" \
        --Input mergedSample/pythia-gen-v11-Zpt0.root \
        --MixFile mergedSample/pythia-gen-v11-Zpt0.root \
        --UseEventWeight true --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --yBoost 0 --nMix $nMix

    ./pythia-analysis.sh "nominal${TAG}" \
        --Input mergedSample/pythia-v11-Zpt0.root \
        --MixFile mergedSample/pythia-v11-Zpt0.root \
        --UseEventWeight true --UseZWeight true \
        --UseTrackWeight true --UseResidualWeight false \
        --ZWeightFile my_ZWeights/20260129_ZCorrection_V4_pp_zPt0-500.root \
        --yBoost 0 --nMix $nMix

    ./pythia-analysis.sh "residual${TAG}" \
        --Input mergedSample/pythia-v11-Zpt0.root \
        --MixFile mergedSample/pythia-v11-Zpt0.root \
        --UseEventWeight true --UseZWeight true \
        --UseTrackWeight true --UseResidualWeight true \
        --yBoost 0 --nMix $nMix \
        --ZWeightFile my_ZWeights/20260129_ZCorrection_V4_pp_zPt0-500.root \
        --ResidualWeightFile my_residualWeights/20260129_TrackResidualCorrection_V22_ZWeight_V4_pp_zPt
fi

# pPb
if [ "$DOPPB" == "1" ]; then
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
        --UseEventWeight true --UseZWeight true \
        --UseTrackWeight true --UseResidualWeight false \
        --ZWeightFile my_ZWeights/20260126_ZCorrection_V4_PPb_zPt0-500.root \
        --yBoost 0 --nMix $nMix

    ./pPbMC-analysis.sh "residual${TAG}" \
        --Input pPbSample/V0.2/PPbMC_Reco.root \
        --MixFile pPbSample/V0.2/PPbMC_Reco.root \
        --UseEventWeight true --UseZWeight true \
        --UseTrackWeight true --UseResidualWeight true \
        --yBoost 0 --nMix $nMix \
        --ZWeightFile my_ZWeights/20260126_ZCorrection_V4_PPb_zPt0-500.root \
        --ResidualWeightFile my_residualWeights/20260126_TrackResidualCorrection_V22_ZWeight_V4_PPb_zPt
fi

# PbP
if [ "$DOPBP" == "1" ]; then
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
        --UseEventWeight true --UseZWeight true \
        --UseTrackWeight true --UseResidualWeight false \
        --ZWeightFile my_ZWeights/20260126_ZCorrection_V4_PbP_zPt0-500.root \
        --yBoost 0 --nMix $nMix

    ./PbPMC-analysis.sh "residual${TAG}" \
        --Input pPbSample/V0.2/PbPMC_Reco.root \
        --MixFile pPbSample/V0.2/PbPMC_Reco.root \
        --UseEventWeight true --UseZWeight true \
        --UseTrackWeight true --UseResidualWeight true \
        --yBoost 0 --nMix $nMix \
        --ZWeightFile my_ZWeights/20260126_ZCorrection_V4_PbP_zPt0-500.root \
        --ResidualWeightFile my_residualWeights/20260126_TrackResidualCorrection_V22_ZWeight_V4_PbP_zPt
fi