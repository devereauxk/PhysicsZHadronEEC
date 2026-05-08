#!/bin/bash
# One-shot: project existing shifted-geometry raw outputs → plots/ files.
set -euo pipefail
cd "$(dirname "$0")"
source /home/kdeverea/PhysicsZHadronEEC/SetupAnalysis.sh
source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh

PP_TAG="${OFFICIAL_TAG_PP}_bin10x10shifted_20260506"
PPB_TAG="${OFFICIAL_TAG_PPB}_bin10x10shifted_20260506"

NOTE_BINS=("0_500:0.5_15" "0_30:0.5_2" "0_30:2_4" "0_30:4_15" "30_500:0.5_2" "30_500:2_4" "30_500:4_15")

project_one() {
    local prefix=$1 zpt=$2 track=$3
    local input="output/${prefix}_ZPT${zpt}-${track}.root"
    local nosub="output/${prefix}_ZPT${zpt}-${track}-nosub.root"
    local result="output/${prefix}_ZPT${zpt}-${track}-result.root"
    echo "  projecting ${prefix} ZPT${zpt} trk${track}"
    root -l -q -b "makeProjectionShifted10x10.C(\"${input}\",\"${nosub}\",\"${track}\",0)" 2>&1 | tail -3
    root -l -q -b "makeProjectionShifted10x10.C(\"${input}\",\"${result}\",\"${track}\",1)" 2>&1 | tail -3
}

merge_zpt() {
    local prefix=$1 zpt=$2
    shift 2
    local tracks=("$@") nosubs=() results=()
    for t in "${tracks[@]}"; do
        nosubs+=("output/${prefix}_ZPT${zpt}-${t}-nosub.root")
        results+=("output/${prefix}_ZPT${zpt}-${t}-result.root")
    done
    echo "  hadd → plots/${prefix}_ZPT${zpt}"
    hadd -f "plots/${prefix}_ZPT${zpt}-nosub.root" "${nosubs[@]}" >/dev/null
    hadd -f "plots/${prefix}_ZPT${zpt}-result.root" "${results[@]}" >/dev/null
    rm -f "${nosubs[@]}" "${results[@]}"
}

process() {
    local prefix=$1 zpt track
    for e in "${NOTE_BINS[@]}"; do
        IFS=':' read -r zpt track <<< "$e"
        project_one "$prefix" "$zpt" "$track"
    done
    merge_zpt "$prefix" "0_500" "0.5_15"
    merge_zpt "$prefix" "0_30" "0.5_2" "2_4" "4_15"
    merge_zpt "$prefix" "30_500" "0.5_2" "2_4" "4_15"
}

PP_VARIATIONS=(Loose Tight IsMuTaggedFalse IsPURejectTrue MuVar0 MuVar1 MuVar2 MuVar3 EEPrivate TrackCorrection0p976 TrackCorrection1p024)
PA_VARIATIONS=(Loose Tight IsMuTaggedFalse IsPURejectTrue MuVar0 MuVar1 MuVar2 MuVar3 TrackCorrection0p976 TrackCorrection1p024)

for v in "${PP_VARIATIONS[@]}"; do process "pp_trkResidual_${PP_TAG}_${v}"; done
for v in "${PA_VARIATIONS[@]}"; do
    process "pPb_trkResidual_${PPB_TAG}_${v}"
    process "PbP_trkResidual_${PPB_TAG}_${v}"
done

echo "All done."
