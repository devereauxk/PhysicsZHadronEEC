#!/bin/bash
set -euo pipefail

ScriptDir=$(cd "$(dirname "$0")" && pwd)
RepoBase=$(cd "$ScriptDir/../.." && pwd)
export ProjectBase=${ProjectBase:-$RepoBase}
source "$ScriptDir/env.sh"

Events=${1:-2000}
Seed=${2:-50203}

cd "$ScriptDir"
mkdir -p output

./generate_madgraph.sh --Energy 5020 --Events "$Events" --Seed "$Seed" \
   --OutputLHE "$ScriptDir/output/ZMuMu_5020_validation.lhe"

./ExecuteShowerLHE --Input "$ScriptDir/output/ZMuMu_5020_validation.lhe" \
   --Output "$ScriptDir/output/ZMuMu_5020_validation.hepmc" \
   --Config "$ScriptDir/CP5.cmnd" --Events "$Events" --Seed "$((Seed + 1))"

./ExecuteConvertHepMC3 --Input "$ScriptDir/output/ZMuMu_5020_validation.hepmc" \
   --Output "$ScriptDir/output/ZMuMu_5020_validation.root" \
   --MirrorGenToReco true

./ExecuteQuickDndEta --Input "$ScriptDir/output/ZMuMu_5020_validation.root" \
   --Output "$ScriptDir/output/ZMuMu_5020_validation_dndeta.root" \
   --TrackPlot "$ScriptDir/output/ZMuMu_5020_tracks_dndeta.pdf" \
   --ZPlot "$ScriptDir/output/ZMuMu_5020_z_dndeta.pdf"

echo "Validation outputs written under $ScriptDir/output"
