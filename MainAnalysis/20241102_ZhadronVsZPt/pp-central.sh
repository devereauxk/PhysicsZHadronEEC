nMix=10
TAG="_ZV4_trkV22_nmix10"

./pythia_Gen-analysis.sh "nominal${TAG}" \
    --Input mergedSample/pythia-gen-v11-Zpt0.root \
    --MixFile mergedSample/pythia-gen-v11-Zpt0.root \
    --UseEventWeight true --UseZWeight false \
    --UseTrackWeight true --UseResidualWeight false \
    --yBoost 0 --nMix $nMix

./pythia-analysis.sh "nominal${TAG}" \
    --Input mergedSample/pythia-v11-Zpt0.root \
    --MixFile mergedSample/pythia-v11-Zpt0.root \
    --UseEventWeight true --UseZWeight false \
    --UseTrackWeight true --UseResidualWeight false \
    --yBoost 0 --nMix $nMix

./pythia-analysis.sh "ZResidual${TAG}" \
    --Input mergedSample/pythia-v11-Zpt0.root \
    --MixFile mergedSample/pythia-v11-Zpt0.root \
    --UseEventWeight true --UseZWeight true \
    --UseTrackWeight true --UseResidualWeight false \
    --yBoost 0 --nMix $nMix \
    --ZWeightFile my_ZWeights/20260129_ZCorrection_V4_pp_zPt0-500.root

./pythia-analysis.sh "trkResidual${TAG}" \
    --Input mergedSample/pythia-v11-Zpt0.root \
    --MixFile mergedSample/pythia-v11-Zpt0.root \
    --UseEventWeight true --UseZWeight true \
    --UseTrackWeight true --UseResidualWeight true \
    --yBoost 0 --nMix $nMix \
    --ZWeightFile my_ZWeights/20260129_ZCorrection_V4_pp_zPt0-500.root \
    --ResidualWeightFile my_residualWeights/20260129_TrackResidualCorrection_V22_ZWeight_V4_pp_zPt
