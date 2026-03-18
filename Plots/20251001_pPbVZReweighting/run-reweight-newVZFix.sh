make ExecuteVZ

DATE_TAG=20260318
INPUT_TAG="noVZWeight_nmix0"
OUTPUT_TAG="${DATE_TAG}_newVZFix_ZPT0_500"

echo "[newVZFix] Building pPb weight from MC=PPbMC, Data=PbPData"
./ExecuteVZ \
  --collisionType pPb \
  --mcCollisionType pPb \
  --dataCollisionType PbP \
  --zPtRange 0_500 \
  --trkPtRange 1_10 \
  --tag "${INPUT_TAG}" \
  --output "${OUTPUT_TAG}"

echo "[newVZFix] Building PbP weight from MC=PbPMC, Data=PPbData"
./ExecuteVZ \
  --collisionType PbP \
  --mcCollisionType PbP \
  --dataCollisionType pPb \
  --zPtRange 0_500 \
  --trkPtRange 1_10 \
  --tag "${INPUT_TAG}" \
  --output "${OUTPUT_TAG}"
