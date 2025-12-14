nMix=5

./pythia_Gen-analysis.sh "nominal_V17_nmix5" --UseEventWeight false --UseTrackWeight true --UseResidualWeight false --yBoost 0 --nMix $nMix
./pythia-analysis.sh "nominal_V17_nmix5" --UseEventWeight false --UseTrackWeight true --UseResidualWeight false --yBoost 0 --nMix $nMix
./pythia-analysis.sh "residual_V17_nmix5" --UseEventWeight false --UseTrackWeight true --UseResidualWeight true --ResidualWeightFile my_residualWeights/20251214_TrackResidualCorrection_V17_pp_zPt --yBoost 0 --nMix $nMix

# nominal = track + event, (if gen) EPOS
# residual = nominal + residual

# track_nominal = track, (if gen) EPOS
# track_residual = track_nominal + residual
