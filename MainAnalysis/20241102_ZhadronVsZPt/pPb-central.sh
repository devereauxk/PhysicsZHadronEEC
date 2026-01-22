nMix=10
TAG="_ZV3_trkV21_nmix10"

./pPbMC_Gen-analysis.sh "nominal${TAG}" \
    --Input pPbSample/V0.2/PPbMC_Gen.root \
    --MixFile pPbSample/V0.2/PPbMC_Gen.root \
    --UseEventWeight true --UseZWeight false \
    --UseTrackWeight true --UseResidualWeight false \
    --EPOSFile mergedEPOS/PPbMC_Gen.root --Fraction 1 \
    --yBoost 0 --nMix $nMix

exit

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
    --ZWeightFile my_ZWeights/20260120_ZCorrection_V3_PPb_zPt0-500.root \
    --yBoost 0 --nMix $nMix
./pPbMC-analysis.sh "trkResidual${TAG}" \
    --Input pPbSample/V0.2/PPbMC_Reco.root \
    --MixFile pPbSample/V0.2/PPbMC_Reco.root \
    --UseEventWeight true --UseZWeight true \
    --UseTrackWeight true --UseResidualWeight true \
    --yBoost 0 --nMix $nMix \
    --ZWeightFile my_ZWeights/20260120_ZCorrection_V3_PPb_zPt0-500.root \
    --ResidualWeightFile my_residualWeights/20260120_TrackResidualCorrection_V21_ZWeight_V3_PPb_zPt


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
