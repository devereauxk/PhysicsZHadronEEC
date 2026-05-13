#!/bin/bash
#
# pp energy-extrapolation MC vs DATA overlay runner.
#
# Produces six PDFs (ZPt / DeltaPhi / DeltaEta x data-driven / mc-driven EE weight)
# in plots/pp_ee at the inclusive selection ZPT0_500, trkPT0.5_15.
#
# Inputs come from the maintained main analysis productions in
# MainAnalysis/20241102_ZhadronVsZPt/plots:
#   - madgraphMC_Gen_nominal_<tag5020>_ZPT0_500-{nosub,result}.root   (curve 1)
#   - madgraphMC_Gen_nominal_<tag8160>_ZPT0_500-{nosub,result}.root   (curve 3)
#   - pp_EEtrkResidual_<pptag>_ZPT0_500-{nosub,result}.root           (curve 2)
#   - pp_trkResidual_<pptag>_ZPT0_500-{nosub,result}.root             (curve 4 data-driven)
#   - pp_trkResidual_<pptag>_EEPrivate_ZPT0_500-{nosub,result}.root   (curve 4 mc-driven)
#
# Optional knobs:
#   PRODUCE_MC=1  -> if the private MadGraph gen-level outputs at ZPT0_500 are
#                    missing, regenerate them via the maintained main-analysis
#                    runner pp-madgraphclosure.sh.
#   ZPT_RANGE / TRK_RANGE -> override the inclusive selection.

set -euo pipefail

SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)
cd "$SCRIPT_DIR"

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh

ZPT_RANGE=${ZPT_RANGE:-0_500}
TRK_RANGE=${TRK_RANGE:-0.5_15}
PPTAG=${PPTAG:-${OFFICIAL_TAG_PP}}
TAG5020=${TAG5020:-madgraphPP5020_nmix10}
TAG8160=${TAG8160:-madgraphPP8160_nmix10}

MAIN_PLOTS=/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots

ensure_main_outputs() {
    local missing=()
    local roots=(
        "${MAIN_PLOTS}/pp_EEtrkResidual_${PPTAG}_ZPT${ZPT_RANGE}-nosub.root"
        "${MAIN_PLOTS}/pp_EEtrkResidual_${PPTAG}_ZPT${ZPT_RANGE}-result.root"
        "${MAIN_PLOTS}/pp_trkResidual_${PPTAG}_ZPT${ZPT_RANGE}-nosub.root"
        "${MAIN_PLOTS}/pp_trkResidual_${PPTAG}_ZPT${ZPT_RANGE}-result.root"
        "${MAIN_PLOTS}/pp_trkResidual_${PPTAG}_EEPrivate_ZPT${ZPT_RANGE}-nosub.root"
        "${MAIN_PLOTS}/pp_trkResidual_${PPTAG}_EEPrivate_ZPT${ZPT_RANGE}-result.root"
    )
    for f in "${roots[@]}"; do
        [[ -e "$f" ]] || missing+=("$f")
    done
    if (( ${#missing[@]} > 0 )); then
        echo "[plot-pp-ee] Required main-analysis outputs are missing:" >&2
        printf '  %s\n' "${missing[@]}" >&2
        echo "[plot-pp-ee] Refresh them via the maintained pp runners:" >&2
        echo "  cd /home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt" >&2
        echo "  ./central.sh 1 0 0      # nominal pp_trkResidual (data-driven EE) at ZPT0_500" >&2
        echo "  ./pp-EE.sh   1 0 0      # pp_EEtrkResidual (no EE weight) at ZPT0_500" >&2
        echo "  ./systematics.sh 1 0 0  # pp_trkResidual_..._EEPrivate (mc-driven EE) at ZPT0_500" >&2
        exit 1
    fi
}

produce_private_mc_gen() {
    local input="$1"
    local tag="$2"
    local outNoSub="${MAIN_PLOTS}/madgraphMC_Gen_nominal_${tag}_ZPT${ZPT_RANGE}-nosub.root"
    local outResult="${MAIN_PLOTS}/madgraphMC_Gen_nominal_${tag}_ZPT${ZPT_RANGE}-result.root"
    if [[ -e "${outNoSub}" && -e "${outResult}" ]]; then
        echo "[plot-pp-ee] private gen output already present for tag=${tag}, skipping."
        return
    fi
    if [[ "${PRODUCE_MC:-0}" != "1" ]]; then
        echo "[plot-pp-ee] Missing private MC gen-level output for tag=${tag}:" >&2
        echo "  ${outNoSub}" >&2
        echo "  ${outResult}" >&2
        echo "[plot-pp-ee] Rerun with PRODUCE_MC=1 to regenerate via pp-madgraphclosure.sh." >&2
        exit 1
    fi

    echo "[plot-pp-ee] Producing private MC gen-level output for tag=${tag} from ${input}..."
    local cfg
    cfg=$(mktemp -p /tmp/kdeverea pp-ee-cfg.XXXXXX.sh)
    cat > "${cfg}" <<EOF
ZPT_RANGES=("${ZPT_RANGE}")
PT_RANGES=("${TRK_RANGE}")
EOF
    (
        cd /home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt
        SKIP_CLEAN=1 \
        CONFIG_FILE="${cfg}" \
        TAG="${tag}" \
        NMIX=10 \
        PP_PRIVATE_MCGENINPUT="${input}" \
        PP_OFFICIAL_MCGENINPUT="${input}" \
        ./pp-madgraphclosure.sh
    )
    rm -f "${cfg}"
}

# 1) Verify the corrected-data inputs exist (do not auto-regenerate; refer user to maintained runners).
ensure_main_outputs

# 2) Optionally regenerate the private MadGraph 5.02 / 8.16 TeV gen-level outputs.
produce_private_mc_gen "${PRIVATE_PP_5020_INPUT}" "${TAG5020}"
produce_private_mc_gen "${PRIVATE_PP_8160_INPUT}" "${TAG8160}"

# 3) Build the plotter and run both energy-weight variants.
make ExecutePPEEPlot
mkdir -p plots/pp_ee

for mode in data mc; do
    ./ExecutePPEEPlot \
        --zPtRange   "${ZPT_RANGE}" \
        --trkPtRange "${TRK_RANGE}" \
        --pptag      "${PPTAG}" \
        --tag5020    "${TAG5020}" \
        --tag8160    "${TAG8160}" \
        --eeMode     "${mode}" \
        --outputDir  "plots/pp_ee"
done
