nMix=10

./PbPMC_Gen-analysis.sh "nominal_V16_nmix10" --UseEventWeight false --UseTrackWeight true --UseResidualWeight false --yBoost 0 --EPOSFile mergedEPOS/PbPMC_Gen.root --Fraction 1 --nMix $nMix
./PbPMC-analysis.sh "nominal_V16_nmix10" --UseEventWeight false --UseTrackWeight true --UseResidualWeight false --yBoost 0 --nMix $nMix
./PbPMC-analysis.sh "residual_V16_nmix10" --UseEventWeight false --UseTrackWeight true --UseResidualWeight true --ResidualWeightFile my_residualWeights/20251212_TrackResidualCorrection_V16_PbP_zPt --yBoost 0 --nMix $nMix

# nominal = track + event, (if gen) EPOS
# residual = nominal + residual

# track_nominal = track, (if gen) EPOS
# track_residual = track_nominal + residual
