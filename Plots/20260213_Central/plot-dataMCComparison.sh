#!/bin/bash

set -e

THISDIR=$(cd "$(dirname "$0")" && pwd)
cd "$THISDIR"

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh

make ExecuteDataMCComparison

SYSTEMS_CSV=${SYSTEMS:-pp,pPb,PbP}
ZPT_RANGES_CSV=${ZPT_RANGES:-0_500}
TRACK_RANGES_CSV=${TRACK_RANGES:-0.5_15}
INPUT_ROOT_DIR=${INPUT_ROOT_DIR:-/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots}
OUTPUT_DIR=${OUTPUT_DIR:-plots/detaMCComparison}
PP_TAG_BASE=${PP_TAG_BASE:-${OFFICIAL_TAG_PP/%_nmix10/_nmix0}}
PA_TAG_BASE=${PA_TAG_BASE:-${OFFICIAL_TAG_PPB/%_nmix10/_nmix0}}
PP_TAG=${PP_TAG:-dataMCComparison_${PP_TAG_BASE/_nmix0/_noEE_nmix0}}
PA_TAG=${PA_TAG:-dataMCComparison_${PA_TAG_BASE}}

IFS=',' read -ra SYSTEMS_ARRAY <<< "$SYSTEMS_CSV"
IFS=',' read -ra ZPT_ARRAY <<< "$ZPT_RANGES_CSV"
IFS=',' read -ra TRACK_ARRAY <<< "$TRACK_RANGES_CSV"

mkdir -p "$OUTPUT_DIR"

for system in "${SYSTEMS_ARRAY[@]}"; do
    for zPtRange in "${ZPT_ARRAY[@]}"; do
        for trkPtRange in "${TRACK_ARRAY[@]}"; do
            echo "Processing system: $system, zPtRange: $zPtRange, trkPtRange: $trkPtRange"
            tag="$PA_TAG"
            if [ "$system" = "pp" ]; then
                tag="$PP_TAG"
            fi

            ./ExecuteDataMCComparison \
                --collisionType "$system" \
                --zPtRange "$zPtRange" \
                --trkPtRange "$trkPtRange" \
                --tag "$tag" \
                --inputRootDir "$INPUT_ROOT_DIR" \
                --outputDir "$OUTPUT_DIR"
        done
    done
done

exit
