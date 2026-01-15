nMix=20
TAG="_V19_nmix20"

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
    --UseEventWeight true --UseZWeight false \
    --UseTrackWeight true --UseResidualWeight true \
    --yBoost 0 --nMix $nMix \
    --ResidualWeightFile my_residualWeights/20260117_TrackResidualCorrection_V19_PbP_zPt


# luna skim correction
#./PbPMC-analysis.sh "nominal_V16${TAG}" \
#    --Input pPbSample/V0.4/PbPMC_Reco_Weight.root \
#    --MixFile pPbSample/V0.4/PbPMC_Reco_Weight.root \
#    --UseEventWeight false --UseTrackWeight true --UseResidualWeight false \
#    --yBoost 0 --nMix $nMix
#./PbPMC-analysis.sh "residual_V16${TAG}" \
#    --Input pPbSample/V0.4/PbPMC_Reco_Weight.root \
#    --MixFile pPbSample/V0.4/PbPMC_Reco_Weight.root \
#    --UseEventWeight false --UseTrackWeight true --UseResidualWeight true \
#    --yBoost 0 --nMix $nMix

# nominal = track + event, (if gen) EPOS
# residual = nominal + residual

# track_nominal = track, (if gen) EPOS
# track_residual = track_nominal + residual
