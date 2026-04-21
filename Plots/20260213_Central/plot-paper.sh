#!/bin/bash

set -e

THISDIR=$(cd "$(dirname "$0")" && pwd)
cd "$THISDIR"

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh

make ExecutePaperPlot

PP_TAG="$OFFICIAL_TAG_PP"
PPB_TAG="$OFFICIAL_TAG_PPB"

./ExecutePaperPlot --pptag "$PP_TAG" --pPbtag "$PPB_TAG" --includeMC true
