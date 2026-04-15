#!/bin/bash
set -euo pipefail
cd "$(dirname "$0")"
source /home/kdeverea/PhysicsZHadronEEC/SetupAnalysis.sh
make ExecutePlotComparison
./ExecutePlotComparison
echo "Plots saved to plots/"
