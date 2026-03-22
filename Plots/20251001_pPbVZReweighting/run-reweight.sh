#!/bin/bash

set -euo pipefail

make ExecuteVZ

PP_OUTPUT_TAG="20260321_ZPT0_500"
PA_OUTPUT_TAG="20260321_ZPT0_500"

./ExecuteVZ \
  --collisionType pp \
  --mcCollisionType pp \
  --dataCollisionType pp \
  --zPtRange 0_500 \
  --trkPtRange 1_10 \
  --tag "noVZWeight_nmix0" \
  --output "${PP_OUTPUT_TAG}"

./ExecuteVZ \
  --collisionType pPb \
  --mcCollisionType pPb \
  --dataCollisionType pPb \
  --zPtRange 0_500 \
  --trkPtRange 1_10 \
  --tag "noVZWeight_nmix0" \
  --output "${PA_OUTPUT_TAG}"

./ExecuteVZ \
  --collisionType PbP \
  --mcCollisionType PbP \
  --dataCollisionType PbP \
  --zPtRange 0_500 \
  --trkPtRange 1_10 \
  --tag "noVZWeight_nmix0" \
  --output "${PA_OUTPUT_TAG}"
