
DOPPB=$1
DOPBP=$2

echo "[closure-trk-newVZFix-mc.sh] Delegating to full newVZFix closure chain (GEN, nominal, ZResidual, trkResidual)."
echo "[closure-trk-newVZFix-mc.sh] For central-closure production, use closure-trk-newVZFix.sh directly."

./closure-trk-newVZFix.sh 0 "$DOPPB" "$DOPBP"
