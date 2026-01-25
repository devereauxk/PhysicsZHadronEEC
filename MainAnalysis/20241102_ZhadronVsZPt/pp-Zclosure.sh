nMix=10
TAG="_Zclosure"

./pythia_Gen-analysis.sh "nominal${TAG}" \
    --Input mergedSample/pythia-gen-v11-Zpt0.root  \
    --MixFile mergedSample/pythia-gen-v11-Zpt0.root  \
    --UseEventWeight true --UseZWeight false \
    --UseTrackWeight true --UseResidualWeight false \
   --yBoost 0 --nMix $nMix

./pythia-analysis.sh "nominal${TAG}" \
    --Input mergedSample/pythia-v11-Zpt0.root \
    --MixFile mergedSample/pythia-v11-Zpt0.root \
    --UseEventWeight true --UseZWeight false \
    --UseTrackWeight true --UseResidualWeight false \
    --yBoost 0 --nMix $nMix

./pythia-analysis.sh "residual${TAG}" \
    --Input mergedSample/pythia-v11-Zpt0.root \
    --MixFile mergedSample/pythia-v11-Zpt0.root \
    --UseEventWeight true --UseZWeight true \
    --UseTrackWeight true --UseResidualWeight false \
    --yBoost 0 --nMix $nMix \
    --ZWeightFile my_ZWeights/20260123_ZCorrection_V3_pp_zPt0-500.root


# luna skim correction
#./pPbMC-analysis.sh "nominal_V16${TAG}" \
#    --Input pPbSample/V0.4/PPbMC_Reco_Weight.root \
#    --MixFile pPbSample/V0.4/PPbMC_Reco_Weight.root \
#    --UseEventWeight false --UseTrackWeight true --UseResidualWeight false \
#    --yBoost 0 --nMix $nMix
#./pPbMC-analysis.sh "residual_V16${TAG}" \
#    --Input pPbSample/V0.4/PPbMC_Reco_Weight.root \
#    --MixFile pPbSample/V0.4/PPbMC_Reco_Weight.root \
#    --UseEventWeight false --UseTrackWeight true --UseResidualWeight true \
#    --yBoost 0 --nMix $nMix

# nominal = track + event, (if gen) EPOS
# residual = nominal + residual

# track_nominal = track, (if gen) EPOS
# track_residual = track_nominal + residual
