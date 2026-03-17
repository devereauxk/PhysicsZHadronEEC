DOPP=$1
DOPPB=$2
DOPBP=$3

nMix=10

write_config() {
    local zpt_list="$1"
    local pt_list="$2"
    cat > config.sh <<EOF
ZPT_RANGES=(${zpt_list})
PT_RANGES=(${pt_list})
EOF
}

run_ppb_chain() {
    local MCPREFIX=$1
    local DATAPREFIX=$2
    local ISPPB=$3
    local MCINPUT=$4
    local DATAINPUT=$5
    local ZWEIGHT=$6
    local RWEIGHT=$7
    local TAG=$8

    ./system-analysis.sh "${MCPREFIX}_Gen_nominal${TAG}" \
        --IsPP false --IsGenZ true --IsData false --IsPPb ${ISPPB} \
        --Input "${MCINPUT}" \
        --MixFile "${MCINPUT}" \
        --UseEventWeight true --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --EPOSFile "mergedEPOS/$(basename "${MCINPUT}")" --Fraction 1 \
        --yBoost 0 --nMix ${nMix}

    ./system-analysis.sh "${DATAPREFIX}_nominal${TAG}" \
        --IsPP false --IsGenZ false --IsData true --IsPPb ${ISPPB} \
        --Input "${DATAINPUT}" \
        --MixFile "${DATAINPUT}" \
        --UseEventWeight true --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --yBoost 0 --nMix ${nMix}

    ./system-analysis.sh "${DATAPREFIX}_ZResidual${TAG}" \
        --IsPP false --IsGenZ false --IsData true --IsPPb ${ISPPB} \
        --Input "${DATAINPUT}" \
        --MixFile "${DATAINPUT}" \
        --UseEventWeight true --UseZWeight true \
        --UseTrackWeight true --UseResidualWeight false \
        --yBoost 0 --nMix ${nMix} \
        --ZWeightFile "${ZWEIGHT}"

    ./system-analysis.sh "${DATAPREFIX}_trkResidual${TAG}" \
        --IsPP false --IsGenZ false --IsData true --IsPPb ${ISPPB} \
        --Input "${DATAINPUT}" \
        --MixFile "${DATAINPUT}" \
        --UseEventWeight true --UseZWeight true \
        --UseTrackWeight true --UseResidualWeight true \
        --yBoost 0 --nMix ${nMix} \
        --ZWeightFile "${ZWEIGHT}" \
        --ResidualWeightFile "${RWEIGHT}"
}

# Speedup path for repeated system-analysis calls:
# build once, then skip per-call clean/rebuild and allow optional cross-cut parallelism.
if [ "${SKIP_CLEAN:-0}" != "1" ]; then
    source clean.sh
fi
export SKIP_CLEAN=1
export CUT_PARALLELISM=${CUT_PARALLELISM:-1}
export NTHREAD=${NTHREAD:-20}
export NSLICE_FACTOR=${NSLICE_FACTOR:-1}

# CENTRAL VALUES (requested): 4 selections
write_config '"5_30" "30_500"' '"0.5_4" "4_500"'

if [ "$DOPP" == "1" ]; then
    TAG="_noEvtWeight_ZV6_trkV24_nmix10"
    ./system-analysis.sh "pp_trkResidual${TAG}" \
        --IsPP true --IsGenZ false --IsData true \
        --Input mergedSample/pp-v11-Zpt0.root \
        --MixFile mergedSample/pp-v11-Zpt0.root \
        --UseEventWeight false --UseZWeight true \
        --UseTrackWeight true --UseResidualWeight true \
        --yBoost 0 --nMix ${nMix} \
        --ZWeightFile my_ZWeights/20260317_ZCorrection_V6_pp_zPt0-500.root \
        --ResidualWeightFile my_residualWeights/20260317_TrackResidualCorrection_V24_ZWeight_V6_pp_zPt
fi

if [ "$DOPPB" == "1" ]; then
    TAG="_ZV5_trkV23_nmix10"
    run_ppb_chain "pPbMC" "pPb" true \
        "pPbSample/V0.2/PPbMC_Gen.root" \
        "pPbSample/V0.2/PbPData_Reco.root" \
        "my_ZWeights/20260202_ZCorrection_V5_PPb_zPt0-500.root" \
        "my_residualWeights/20260223_TrackResidualCorrection_V23_ZWeight_V5_PPb_zPt" \
        "${TAG}"
fi

if [ "$DOPBP" == "1" ]; then
    TAG="_ZV5_trkV23_nmix10"
    run_ppb_chain "PbPMC" "PbP" false \
        "pPbSample/V0.2/PbPMC_Gen.root" \
        "pPbSample/V0.2/PPbData_Reco.root" \
        "my_ZWeights/20260202_ZCorrection_V5_PbP_zPt0-500.root" \
        "my_residualWeights/20260223_TrackResidualCorrection_V23_ZWeight_V5_PbP_zPt" \
        "${TAG}"
fi

# INCLUSIVE selection
write_config '"5_500"' '"0.5_500"'

if [ "$DOPPB" == "1" ]; then
    TAG="_ZV5_trkV23_nmix10"
    run_ppb_chain "pPbMC" "pPb" true \
        "pPbSample/V0.2/PPbMC_Gen.root" \
        "pPbSample/V0.2/PbPData_Reco.root" \
        "my_ZWeights/20260202_ZCorrection_V5_PPb_zPt0-500.root" \
        "my_residualWeights/20260223_TrackResidualCorrection_V23_ZWeight_V5_PPb_zPt" \
        "${TAG}"
fi

if [ "$DOPBP" == "1" ]; then
    TAG="_ZV5_trkV23_nmix10"
    run_ppb_chain "PbPMC" "PbP" false \
        "pPbSample/V0.2/PbPMC_Gen.root" \
        "pPbSample/V0.2/PPbData_Reco.root" \
        "my_ZWeights/20260202_ZCorrection_V5_PbP_zPt0-500.root" \
        "my_residualWeights/20260223_TrackResidualCorrection_V23_ZWeight_V5_PbP_zPt" \
        "${TAG}"
fi
