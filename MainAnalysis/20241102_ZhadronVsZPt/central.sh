DOPP=$1
DOPPB=$2
DOPBP=$3

nMix=10
VZWeightFile_PPb="${VZ_WEIGHT_FILE_PPB:-/home/kdeverea/PhysicsZHadronEEC/Plots/20251001_pPbVZReweighting/summary/20260320_pPbPbpRelabelFix_skimVZOff_ZPT0_500_VzReweightFits_pPb.root}"
VZWeightFile_PbP="${VZ_WEIGHT_FILE_PBP:-/home/kdeverea/PhysicsZHadronEEC/Plots/20251001_pPbVZReweighting/summary/20260320_pPbPbpRelabelFix_skimVZOff_ZPT0_500_VzReweightFits_PbP.root}"
ZWeightFile_PPb="${Z_WEIGHT_FILE_PPB:-my_ZWeights/20260320_ZCorrection_V6_skimVZOff_PPb_zPt0-500.root}"
ZWeightFile_PbP="${Z_WEIGHT_FILE_PBP:-my_ZWeights/20260320_ZCorrection_V6_skimVZOff_PbP_zPt0-500.root}"
RWeightFile_PPb="${R_WEIGHT_FILE_PPB:-my_residualWeights/20260320_TrackResidualCorrection_V24_ZWeight_V6_skimVZOff_PPb_zPt}"
RWeightFile_PbP="${R_WEIGHT_FILE_PBP:-my_residualWeights/20260320_TrackResidualCorrection_V24_ZWeight_V6_skimVZOff_PbP_zPt}"

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
    local MCGENINPUT=$4
    local EPOSINPUT=$5
    local DATAINPUT=$6
    local ZWEIGHT=$7
    local RWEIGHT=$8
    local VZWEIGHT=$9
    local TAG=${10}

    ./system-analysis.sh "${MCPREFIX}_Gen_nominal${TAG}"         --IsPP false --IsGenZ true --IsData false --IsPPb ${ISPPB}         --Input "${MCGENINPUT}"         --MixFile "${MCGENINPUT}"         --UseEventWeight true --UseZWeight false         --UseTrackWeight true --UseResidualWeight false         --EPOSFile "${EPOSINPUT}" --Fraction 1         --yBoost 0 --nMix ${nMix}         --UseVZWeight true --VZWeightFile "${VZWEIGHT}"

    ./system-analysis.sh "${DATAPREFIX}_nominal${TAG}"         --IsPP false --IsGenZ false --IsData true --UseVZWeight false --IsPPb ${ISPPB}         --Input "${DATAINPUT}"         --MixFile "${DATAINPUT}"         --UseEventWeight true --UseZWeight false         --UseTrackWeight true --UseResidualWeight false         --yBoost 0 --nMix ${nMix}

    ./system-analysis.sh "${DATAPREFIX}_ZResidual${TAG}"         --IsPP false --IsGenZ false --IsData true --UseVZWeight false --IsPPb ${ISPPB}         --Input "${DATAINPUT}"         --MixFile "${DATAINPUT}"         --UseEventWeight true --UseZWeight true         --UseTrackWeight true --UseResidualWeight false         --yBoost 0 --nMix ${nMix}         --ZWeightFile "${ZWEIGHT}"

    ./system-analysis.sh "${DATAPREFIX}_trkResidual${TAG}"         --IsPP false --IsGenZ false --IsData true --UseVZWeight false --IsPPb ${ISPPB}         --Input "${DATAINPUT}"         --MixFile "${DATAINPUT}"         --UseEventWeight true --UseZWeight true         --UseTrackWeight true --UseResidualWeight true         --yBoost 0 --nMix ${nMix}         --ZWeightFile "${ZWEIGHT}"         --ResidualWeightFile "${RWEIGHT}"
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
    ./system-analysis.sh "pp_trkResidual${TAG}"         --IsPP true --IsGenZ false --IsData true --UseVZWeight false         --Input mergedSample/pp-v11-Zpt0.root         --MixFile mergedSample/pp-v11-Zpt0.root         --UseEventWeight false --UseZWeight true         --UseTrackWeight true --UseResidualWeight true         --yBoost 0 --nMix ${nMix}         --ZWeightFile my_ZWeights/20260320_ZCorrection_V6_skimVZOff_pp_zPt0-500.root         --ResidualWeightFile my_residualWeights/20260320_TrackResidualCorrection_V24_ZWeight_V6_skimVZOff_pp_zPt
fi

if [ "$DOPPB" == "1" ]; then
    TAG="_ZV6_trkV24_nmix10"
    run_ppb_chain "pPbMC" "pPb" true         "pPbSample/V0.2/PbPMC_Gen.root"         "mergedEPOS/PPbMC_Gen.root"         "pPbSample/V0.2/PbPData_Reco.root"         "${ZWeightFile_PPb}"         "${RWeightFile_PPb}"         "${VZWeightFile_PPb}"         "${TAG}"
fi

if [ "$DOPBP" == "1" ]; then
    TAG="_ZV6_trkV24_nmix10"
    run_ppb_chain "PbPMC" "PbP" false         "pPbSample/V0.2/PPbMC_Gen.root"         "mergedEPOS/PbPMC_Gen.root"         "pPbSample/V0.2/PPbData_Reco.root"         "${ZWeightFile_PbP}"         "${RWeightFile_PbP}"         "${VZWeightFile_PbP}"         "${TAG}"
fi

# INCLUSIVE selection
write_config '"5_500"' '"0.5_500"'

if [ "$DOPPB" == "1" ]; then
    TAG="_ZV6_trkV24_nmix10"
    run_ppb_chain "pPbMC" "pPb" true         "pPbSample/V0.2/PbPMC_Gen.root"         "mergedEPOS/PPbMC_Gen.root"         "pPbSample/V0.2/PbPData_Reco.root"         "${ZWeightFile_PPb}"         "${RWeightFile_PPb}"         "${VZWeightFile_PPb}"         "${TAG}"
fi

if [ "$DOPBP" == "1" ]; then
    TAG="_ZV6_trkV24_nmix10"
    run_ppb_chain "PbPMC" "PbP" false         "pPbSample/V0.2/PPbMC_Gen.root"         "mergedEPOS/PbPMC_Gen.root"         "pPbSample/V0.2/PPbData_Reco.root"         "${ZWeightFile_PbP}"         "${RWeightFile_PbP}"         "${VZWeightFile_PbP}"         "${TAG}"
fi
