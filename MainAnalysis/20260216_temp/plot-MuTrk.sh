#!/bin/bash

set -e

if [ "${SKIP_CLEAN:-0}" != "1" ]; then
    source ./clean.sh
fi

MODE="${1:-All}"
TAG="${2:-MCStudies_V01Remake}"
DOPP="${3:-1}"
DOPPB="${4:-1}"
DOPBP="${5:-1}"
INPUT_SUBDIR="${INPUT_SUBDIR:-${OUTPUT_SUBDIR:-}}"
OUTPUT_SUBDIR="${OUTPUT_SUBDIR:-}"
CIRCLE_RADII="${CIRCLE_RADII:-}"

INPUT_SUBDIR="${INPUT_SUBDIR#/}"
INPUT_SUBDIR="${INPUT_SUBDIR%/}"
OUTPUT_SUBDIR="${OUTPUT_SUBDIR#/}"
OUTPUT_SUBDIR="${OUTPUT_SUBDIR%/}"
ZPT_RANGE="${ZPT_RANGE:-5_500}"
TRKPT_RANGE="${TRKPT_RANGE:-0.5_500}"

if [ -n "${OUTPUT_SUBDIR}" ]; then
    mkdir -p "plots/${OUTPUT_SUBDIR}"
fi

plot_case() {
    local collisionType=$1
    local sample=$2

    local args=(
        ./ExecuteDeltaRMuTrkPlot
        --collisionType "${collisionType}" \
        --sample "${sample}" \
        --zPtRange "${ZPT_RANGE}" \
        --trkPtRange "${TRKPT_RANGE}" \
        --tag "${TAG}" \
        --inputSubdir "${INPUT_SUBDIR}" \
        --outputSubdir "${OUTPUT_SUBDIR}"
    )

    if [ -n "${CIRCLE_RADII}" ]; then
        args+=(--CircleRadii "${CIRCLE_RADII}")
    fi

    "${args[@]}"
}

plot_requested_samples() {
    local collisionType=$1

    case "${MODE}" in
        Data|Reco|Gen)
            plot_case "${collisionType}" "${MODE}"
            ;;
        All)
            for sample in Data Reco Gen; do
                plot_case "${collisionType}" "${sample}"
            done
            ;;
        *)
            echo "Unsupported mode '${MODE}'. Use Data, Reco, Gen, or All." >&2
            exit 1
            ;;
    esac
}

if [ "${DOPP}" = "1" ]; then
    plot_requested_samples pp
fi

if [ "${DOPPB}" = "1" ]; then
    plot_requested_samples pPb
fi

if [ "${DOPBP}" = "1" ]; then
    plot_requested_samples PbP
fi
