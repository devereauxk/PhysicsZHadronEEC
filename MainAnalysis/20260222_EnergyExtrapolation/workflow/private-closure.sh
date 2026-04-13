#!/bin/bash
set -euo pipefail

ScriptDir=$(cd "$(dirname "$0")" && pwd)
AnalysisDir=$(cd "${ScriptDir}/.." && pwd)
source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh
cd "${AnalysisDir}"
./clean.sh
cd "${ScriptDir}"

minZpt=0
maxZpt=500
name="${BASENAME:-20260405_EnergyExtrapolation_private8160over5020}"
input5020="${PRIVATE_PP_5020_INPUT}"
input8160="${PRIVATE_PP_8160_INPUT}"

stage502="output/${name}-pp-private-502.root"
stage816="output/${name}-pp-private-816.root"
stage816Extrapolated="output/${name}-pp-private-extrapolated-816.root"
correctionRoot="output/${name}.root"
correctionPdf="output/${name}.pdf"
closurePdf="output/${name}-closure.pdf"

for file in "${stage502}" "${stage816}" "${stage816Extrapolated}" "${correctionRoot}" "${correctionPdf}" "${closurePdf}"; do
   if [[ -e "${file}" ]]; then
      echo "Refusing to overwrite existing output: ${file}" >&2
      exit 1
   fi
done

./finalAnalysis.sh output/${name}-pp-private              502 "$@" --MinZPT "${minZpt}" --MaxZPT "${maxZpt}" --MinTrackPT 0.5 --MaxTrackPT 500 --Input "${input5020}" --IsGen false --IsPP true --IsGenZ false
./finalAnalysis.sh output/${name}-pp-private              816 "$@" --MinZPT "${minZpt}" --MaxZPT "${maxZpt}" --MinTrackPT 0.5 --MaxTrackPT 500 --Input "${input8160}" --IsGen false --IsPP true --IsGenZ false

root -l -q -b "correction.C(\"${stage502}\",\"${stage816}\",\"${correctionRoot}\")"
root -l -q -b "plot_corrections.C(\"${correctionRoot}\",\"${correctionPdf}\")"

./finalAnalysis.sh output/${name}-pp-private-extrapolated 816 "$@" --MinZPT "${minZpt}" --MaxZPT "${maxZpt}" --MinTrackPT 0.5 --MaxTrackPT 500 --Input "${input5020}" --IsGen false --IsPP true --IsGenZ false --EnergyExtraFile "${correctionRoot}"

root -l -q -b "plot_closure.C(\"${closurePdf}\",\"${stage816}\",\"${stage502}\",\"${stage816Extrapolated}\",\"pp private 8.16 TeV\",\"pp private 5.02 TeV\",\"pp extrapolated 8.16 TeV\",\"Ratio to private 8.16 TeV\",\"pp 5.02/8.16 TeV\",\"pp\")"
