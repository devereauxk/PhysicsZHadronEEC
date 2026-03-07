
DOPP=$1
DOPPB=$2
DOPBP=$3

#ZPT_RANGES=("40_350")
#PT_RANGES=("1_2" "2_4" "4_10") 

cat > config.sh <<EOF
ZPT_RANGES=("5_30" "30_500")
PT_RANGES=("0.5_4" "4_500") 
EOF


# pp
# PU rejection turned off, PU=1 by default as set by IsPURejection flag
nMix=10
TAG="_PUNominal_EEV2_ZV4_trkV22_nmix10"

#./system-analysis.sh "pp_nominal${TAG}" \
#    --IsPP true --IsGenZ false --IsData true \
#    --Input mergedSample/pp-v11-Zpt0.root \
#    --MixFile mergedSample/pp-v11-Zpt0.root \
#    --UseEventWeight true --UseZWeight false \
#    --UseTrackWeight true --UseResidualWeight false \
#    --yBoost 0 --nMix $nMix \
#    --IsPUReject false

#./system-analysis.sh "pp_ZResidual${TAG}" \
#    --IsPP true --IsGenZ false --IsData true \
#    --Input mergedSample/pp-v11-Zpt0.root \
#    --MixFile mergedSample/pp-v11-Zpt0.root \
#    --UseEventWeight true --UseZWeight true \
#    --UseTrackWeight true --UseResidualWeight false \
#    --yBoost 0 --nMix $nMix \
#    --ZWeightFile my_ZWeights/20260129_ZCorrection_V4_pp_zPt0-500.root \
#    --IsPUReject false

./system-analysis.sh "pp_trkResidual${TAG}" \
    --IsPP true --IsGenZ false --IsData true \
    --Input mergedSample/pp-v11-Zpt0.root \
    --MixFile mergedSample/pp-v11-Zpt0.root \
    --UseEventWeight true --UseZWeight true \
    --UseTrackWeight true --UseResidualWeight true \
    --yBoost 0 --nMix $nMix \
    --ZWeightFile my_ZWeights/20260129_ZCorrection_V4_pp_zPt0-500.root \
    --ResidualWeightFile my_residualWeights/20260129_TrackResidualCorrection_V22_ZWeight_V4_pp_zPt \
    --IsPUReject false

./system-analysis.sh "pythiaMC_Gen_nominal${TAG}" \
    --IsPP true --IsGenZ true --IsData false \
    --Input mergedSample/pythia-gen-v11-Zpt0.root  \
    --MixFile mergedSample/pythia-gen-v11-Zpt0.root  \
    --UseEventWeight false --UseZWeight false \
    --UseTrackWeight false --UseResidualWeight false \
    --yBoost 0 --nMix $nMix \
    --IsPUReject false

