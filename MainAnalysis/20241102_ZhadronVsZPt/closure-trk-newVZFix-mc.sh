
DOPPB=$1
DOPBP=$2

cat > config.sh <<EOF
ZPT_RANGES=("0_10" "10_20" "20_40" "40_500")
PT_RANGES=("0.5_500")
EOF

if [ "${SKIP_CLEAN:-0}" != "1" ]; then
    source clean.sh
fi
export SKIP_CLEAN=1
export CUT_PARALLELISM=${CUT_PARALLELISM:-1}
export NTHREAD=${NTHREAD:-20}
export NSLICE_FACTOR=${NSLICE_FACTOR:-1}

TAG="_newVZFix_ZV6_trkV24_nmix10"
nMix=10

if [ "$DOPPB" == "1" ]; then
    VZWeightFile="/home/kdeverea/PhysicsZHadronEEC/Plots/20251001_pPbVZReweighting/summary/20260318_newVZFix_ZPT0_500_VzReweightFits_pPb.root"

    ./system-analysis.sh "pPbMC_Gen_nominal${TAG}" \
        --IsPP false --IsGenZ true --IsData false --IsPPb true \
        --Input pPbSample/V0.2/PPbMC_Gen.root \
        --MixFile pPbSample/V0.2/PPbMC_Gen.root \
        --UseEventWeight true --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --EPOSFile mergedEPOS/PPbMC_Gen.root --Fraction 1 \
        --yBoost 0 --nMix $nMix \
        --VZWeightFile $VZWeightFile

    ./system-analysis.sh "pPbMC_trkResidual${TAG}" \
        --IsPP false --IsGenZ false --IsData false --IsPPb true \
        --Input pPbSample/V0.2/PPbMC_Reco.root \
        --MixFile pPbSample/V0.2/PPbMC_Reco.root \
        --UseEventWeight true --UseZWeight true \
        --UseTrackWeight true --UseResidualWeight true \
        --yBoost 0 --nMix $nMix \
        --ZWeightFile my_ZWeights/20260318_ZCorrection_V6_newVZFix_PPb_zPt0-500.root \
        --ResidualWeightFile my_residualWeights/20260318_TrackResidualCorrection_V24_ZWeight_V6_newVZFix_PPb_zPt \
        --VZWeightFile $VZWeightFile
fi

if [ "$DOPBP" == "1" ]; then
    VZWeightFile="/home/kdeverea/PhysicsZHadronEEC/Plots/20251001_pPbVZReweighting/summary/20260318_newVZFix_ZPT0_500_VzReweightFits_PbP.root"

    ./system-analysis.sh "PbPMC_Gen_nominal${TAG}" \
        --IsPP false --IsGenZ true --IsData false --IsPPb false \
        --Input pPbSample/V0.2/PbPMC_Gen.root \
        --MixFile pPbSample/V0.2/PbPMC_Gen.root \
        --UseEventWeight true --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --EPOSFile mergedEPOS/PbPMC_Gen.root --Fraction 1 \
        --yBoost 0 --nMix $nMix \
        --VZWeightFile $VZWeightFile

    ./system-analysis.sh "PbPMC_trkResidual${TAG}" \
        --IsPP false --IsGenZ false --IsData false --IsPPb false \
        --Input pPbSample/V0.2/PbPMC_Reco.root \
        --MixFile pPbSample/V0.2/PbPMC_Reco.root \
        --UseEventWeight true --UseZWeight true \
        --UseTrackWeight true --UseResidualWeight true \
        --yBoost 0 --nMix $nMix \
        --ZWeightFile my_ZWeights/20260318_ZCorrection_V6_newVZFix_PbP_zPt0-500.root \
        --ResidualWeightFile my_residualWeights/20260318_TrackResidualCorrection_V24_ZWeight_V6_newVZFix_PbP_zPt \
        --VZWeightFile $VZWeightFile
fi
