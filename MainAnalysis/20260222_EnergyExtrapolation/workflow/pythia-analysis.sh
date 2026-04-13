set -euo pipefail

WORKINGDIR=$(pwd)
cd ..
./clean.sh
cd "$WORKINGDIR"
sleep 1

minZpt=0
maxZpt=500
name="${NAME_TAG:-20260412_EnergyExtrapolation_EEV5}"

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh

EXTRA_ARGS=("$@")
PP_DATAINPUT=${PP_DATAINPUT:-${OFFICIAL_DATAINPUT_PP}}
PPB_DATAINPUT=${PPB_DATAINPUT:-${OFFICIAL_DATAINPUT_PPB}}
PBP_DATAINPUT=${PBP_DATAINPUT:-${OFFICIAL_DATAINPUT_PBP}}

# pp
./finalAnalysis.sh output/pp 502 "${EXTRA_ARGS[@]}" --MinZPT "$minZpt" --MaxZPT "$maxZpt" --MinTrackPT 0.5 --MaxTrackPT 500 --Input "$PP_DATAINPUT" --IsGen false --IsPP true --IsGenZ false --residualFile "${ZWeightFile_PP}"

./finalAnalysis.sh output/pPb 816 "${EXTRA_ARGS[@]}" --MinZPT "$minZpt" --MaxZPT "$maxZpt" --MinTrackPT 0.5 --MaxTrackPT 500 --Input "$PPB_DATAINPUT" --IsGen false --IsPP false --IsGenZ false --residualFile "${ZWeightFile_PPb}"
./finalAnalysis.sh output/PbP 816 "${EXTRA_ARGS[@]}" --MinZPT "$minZpt" --MaxZPT "$maxZpt" --MinTrackPT 0.5 --MaxTrackPT 500 --Input "$PBP_DATAINPUT" --IsGen false --IsPP false --IsGenZ false --residualFile "${ZWeightFile_PbP}"
hadd -f output/pPb-PbP-816.root output/pPb-816.root output/PbP-816.root

root -l -q -b correction.C

root -l -q -b plot_corrections.C
mv correction.root output/${name}.root
mv corrections.pdf output/${name}.pdf

./finalAnalysis.sh output/pp 816 "${EXTRA_ARGS[@]}" --MinZPT "$minZpt" --MaxZPT "$maxZpt" --MinTrackPT 0.5 --MaxTrackPT 500 --Input "$PP_DATAINPUT" --IsGen false --IsPP true --IsGenZ false --residualFile "${ZWeightFile_PP}" --EnergyExtraFile "output/${name}.root"
root -l -q -b "plot_closure.C(\"output/${name}-closure.pdf\")"
