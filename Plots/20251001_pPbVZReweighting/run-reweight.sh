make ExecuteVZ

OUTPUT_TAG="${OUTPUT_TAG:-20260311_ZPT0_500}"
INPUT_TAG="${INPUT_TAG:-noVZWeight_nmix0}"

echo "[official VZ6] Building pPb weight from the real pPb mapping"
./ExecuteVZ   --collisionType pPb   --mcCollisionType pPb   --dataCollisionType pPb   --zPtRange 0_500   --trkPtRange 1_10   --tag "${INPUT_TAG}"   --output "${OUTPUT_TAG}"

echo "[official VZ6] Building PbP weight from the real PbP mapping"
./ExecuteVZ   --collisionType PbP   --mcCollisionType PbP   --dataCollisionType PbP   --zPtRange 0_500   --trkPtRange 1_10   --tag "${INPUT_TAG}"   --output "${OUTPUT_TAG}"

#./ExecuteVZ --collisionType pp --zPtRange 0_350 --trkPtRange 1_10 --tag noEvtWeight_nmix1 --output 20260311_ZPT0_350
