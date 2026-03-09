
DOPP=$1
DOPPB=$2
DOPBP=$3


#ZPT_RANGES=("5_30" "30_500")
#PT_RANGES=("0.5_4" "4_500") 

#ZPT_RANGES=("40_350")
#PT_RANGES=("1_2" "2_4" "4_10")

cat > config.sh <<EOF
ZPT_RANGES=("5_30" "30_500")
PT_RANGES=("0.5_4" "4_500") 
EOF


# pp
# PU rejection turned off, PU=1 by default as set by IsPUReject=true flag
nMix=10
TAG="_PUNominal_ZV5_trkV23_nmix10"
VZWeightFile=/home/kdeverea/PhysicsZHadronEEC/Plots/20251001_pPbVZReweighting/20260307_VzReweightFits_pp.root

./system-analysis.sh "pythiaMC_Gen_nominal${TAG}" \
    --IsPP true --IsGenZ true --IsData false \
    --Input mergedSample/pythia-gen-v11-Zpt0.root  \
    --MixFile mergedSample/pythia-gen-v11-Zpt0.root  \
    --UseEventWeight false --UseZWeight false \
    --UseTrackWeight true --UseResidualWeight false \
    --yBoost 0 --nMix $nMix \
    --VZWeightFile $VZWeightFile \
    --IsPUReject false

exit

./system-analysis.sh "pp_nominal${TAG}" \
    --IsPP true --IsGenZ false --IsData true \
    --Input mergedSample/pp-v11-Zpt0.root \
    --MixFile mergedSample/pp-v11-Zpt0.root \
    --UseEventWeight true --UseZWeight false \
    --UseTrackWeight true --UseResidualWeight false \
    --yBoost 0 --nMix $nMix \
    --IsPUReject false

./system-analysis.sh "pp_ZResidual${TAG}" \
    --IsPP true --IsGenZ false --IsData true \
    --Input mergedSample/pp-v11-Zpt0.root \
    --MixFile mergedSample/pp-v11-Zpt0.root \
    --UseEventWeight true --UseZWeight true \
    --UseTrackWeight true --UseResidualWeight false \
    --yBoost 0 --nMix $nMix \
    --ZWeightFile my_ZWeights/20260308_ZCorrection_V5_pp_zPt0-500.root \
    --IsPUReject false

./system-analysis.sh "pp_trkResidual${TAG}" \
    --IsPP true --IsGenZ false --IsData true \
    --Input mergedSample/pp-v11-Zpt0.root \
    --MixFile mergedSample/pp-v11-Zpt0.root \
    --UseEventWeight true --UseZWeight true \
    --UseTrackWeight true --UseResidualWeight true \
    --yBoost 0 --nMix $nMix \
    --ZWeightFile my_ZWeights/20260308_ZCorrection_V5_pp_zPt0-500.root \
    --ResidualWeightFile my_residualWeights/20260308_TrackResidualCorrection_V23_ZWeight_V5_pp_zPt \
    --IsPUReject false


