
DOPP=$1
DOPPB=$2
DOPBP=$3

nMixPP=1
nMixPA=0
VZWeightFile_PPb="${VZ_WEIGHT_FILE_PPB:-/home/kdeverea/PhysicsZHadronEEC/Plots/20251001_pPbVZReweighting/summary/20260311_ZPT0_500_VzReweightFits_pPb.root}"
VZWeightFile_PbP="${VZ_WEIGHT_FILE_PBP:-/home/kdeverea/PhysicsZHadronEEC/Plots/20251001_pPbVZReweighting/summary/20260311_ZPT0_500_VzReweightFits_PbP.root}"

write_config() {
    local zpt_list="$1"
    local pt_list="$2"
    cat > config.sh <<EOF
ZPT_RANGES=(${zpt_list})
PT_RANGES=(${pt_list})
EOF
}

run_pp_chain() {
    local TAG=$1
    local USE_EVENT_WEIGHT=$2
    local USE_TRACK_WEIGHT=$3
    local VZ_FILE=$4

    ./system-analysis.sh "pythiaMC_nominal${TAG}"         --IsPP true --IsGenZ false --IsData false         --Input mergedSample/pythia-v11-Zpt0.root         --MixFile mergedSample/pythia-v11-Zpt0.root         --UseEventWeight ${USE_EVENT_WEIGHT} --UseZWeight false         --UseTrackWeight ${USE_TRACK_WEIGHT} --UseResidualWeight false         --yBoost 0 --nMix ${nMixPP}         ${VZ_FILE:+--VZWeightFile ${VZ_FILE}}

    ./system-analysis.sh "pythiaMC_Gen_nominal${TAG}"         --IsPP true --IsGenZ true --IsData false         --Input mergedSample/pythia-gen-v11-Zpt0.root         --MixFile mergedSample/pythia-gen-v11-Zpt0.root         --UseEventWeight ${USE_EVENT_WEIGHT} --UseZWeight false         --UseTrackWeight ${USE_TRACK_WEIGHT} --UseResidualWeight false         --yBoost 0 --nMix ${nMixPP}         ${VZ_FILE:+--VZWeightFile ${VZ_FILE}}

    ./system-analysis.sh "pp_nominal${TAG}"         --IsPP true --IsGenZ false --IsData true         --Input mergedSample/pp-v11-Zpt0.root         --MixFile mergedSample/pp-v11-Zpt0.root         --UseEventWeight true --UseZWeight false         --UseTrackWeight true --UseResidualWeight false         --yBoost 0 --nMix ${nMixPP}
}

run_pap_chain() {
    local MCPREFIX=$1
    local DATAPREFIX=$2
    local ISPPB=$3
    local MCRECOINPUT=$4
    local MCGENINPUT=$5
    local EPOSINPUT=$6
    local DATAINPUT=$7
    local TAG=$8
    local VZ_FILE=$9

    ./system-analysis.sh "${MCPREFIX}_nominal${TAG}"         --IsPP false --IsGenZ false --IsData false --IsPPb ${ISPPB}         --Input ${MCRECOINPUT}         --MixFile ${MCRECOINPUT}         --UseEventWeight true --UseZWeight false         --UseTrackWeight true --UseResidualWeight false         --yBoost 0 --nMix ${nMixPA}         ${VZ_FILE:+--VZWeightFile ${VZ_FILE}}

    ./system-analysis.sh "${MCPREFIX}_Gen_nominal${TAG}"         --IsPP false --IsGenZ true --IsData false --IsPPb ${ISPPB}         --Input ${MCGENINPUT}         --MixFile ${MCGENINPUT}         --UseEventWeight true --UseZWeight false         --UseTrackWeight true --UseResidualWeight false         --EPOSFile ${EPOSINPUT} --Fraction 1         --yBoost 0 --nMix ${nMixPA}         ${VZ_FILE:+--VZWeightFile ${VZ_FILE}}

    ./system-analysis.sh "${DATAPREFIX}_nominal${TAG}"         --IsPP false --IsGenZ false --IsData true --IsPPb ${ISPPB}         --Input ${DATAINPUT}         --MixFile ${DATAINPUT}         --UseEventWeight true --UseZWeight false         --UseTrackWeight true --UseResidualWeight false         --yBoost 0 --nMix ${nMixPA}
}

write_config '"0_500"' '"1_10"'

if [ "${DOPP}" == "1" ]; then
    run_pp_chain "_myEvtWeight_VZOnly_nmix1" true true         "/home/kdeverea/PhysicsZHadronEEC/Plots/20251001_pPbVZReweighting/summary/20260317_ZV6_ZPT0_350_VzReweightFits_pp.root"
    run_pp_chain "_EvtWeight_VZOnly_nmix1" true true ""
    run_pp_chain "_noEvtWeight_VZOnly_nmix1" false false ""
fi

if [ "${DOPPB}" == "1" ]; then
    run_pap_chain "pPbMC" "pPb" true         "pPbSample/V0.2/PbPMC_Reco.root"         "pPbSample/V0.2/PbPMC_Gen.root"         "mergedEPOS/PPbMC_Gen.root"         "pPbSample/V0.2/PbPData_Reco.root"         "_noVZWeight_nmix0" ""
fi

if [ "${DOPPB}" == "2" ]; then
    run_pap_chain "pPbMC" "pPb" true         "pPbSample/V0.2/PbPMC_Reco.root"         "pPbSample/V0.2/PbPMC_Gen.root"         "mergedEPOS/PPbMC_Gen.root"         "pPbSample/V0.2/PbPData_Reco.root"         "_VZWeight_nmix0"         "${VZWeightFile_PPb}"
fi

if [ "${DOPBP}" == "1" ]; then
    run_pap_chain "PbPMC" "PbP" false         "pPbSample/V0.2/PPbMC_Reco.root"         "pPbSample/V0.2/PPbMC_Gen.root"         "mergedEPOS/PbPMC_Gen.root"         "pPbSample/V0.2/PPbData_Reco.root"         "_noVZWeight_nmix0" ""
fi

if [ "${DOPBP}" == "2" ]; then
    run_pap_chain "PbPMC" "PbP" false         "pPbSample/V0.2/PPbMC_Reco.root"         "pPbSample/V0.2/PPbMC_Gen.root"         "mergedEPOS/PbPMC_Gen.root"         "pPbSample/V0.2/PPbData_Reco.root"         "_VZWeight_nmix0"         "${VZWeightFile_PbP}"
fi
