nMix=10
TAG="_V16_nmix10"

./pPbMC_Gen-analysis.sh "nominal${TAG}" \
    --Input pPbSample/V0.2/PPbMC_Gen.root \
    --MixFile pPbSample/V0.2/PPbMC_Gen.root \
    --UseEventWeight false --UseTrackWeight true --UseResidualWeight false \
    --EPOSFile mergedEPOS/PPbMC_Gen.root --Fraction 1 \
    --yBoost 0 --nMix $nMix
./pPbMC-analysis.sh "nominal${TAG}" \
    --Input pPbSample/V0.2/PPbMC_Reco.root \
    --MixFile pPbSample/V0.2/PPbMC_Reco.root \
    --UseEventWeight false --UseTrackWeight true -UseResidualWeight false \
    --yBoost 0 --nMix $nMix
./pPbMC-analysis.sh "residual${TAG}" \
    --Input pPbSample/V0.2/PPbMC_Reco.root \
    --MixFile pPbSample/V0.2/PPbMC_Reco.root \
    --UseEventWeight false --UseTrackWeight true --UseResidualWeight true \
    --yBoost 0 --nMix $nMix \
    --ResidualWeightFile my_residualWeights/20251212_TrackResidualCorrection_V16_PPb_zPt


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
