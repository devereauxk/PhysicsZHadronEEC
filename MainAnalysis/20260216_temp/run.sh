
nMix=1
TAG="_MCStudies"

DOPP=$1
DOPPB=$2
DOPBP=$3

cat > config.sh <<EOF
ZPT_RANGES=("5_500")
PT_RANGES=("0.5_500")
EOF

# pp
if [ "$DOPP" == "1" ]; then

    ./system-analysis.sh "pythiaMC_Gen${TAG}" \
        --IsPP true --IsGenZ true --IsData false \
        --Input mergedSample/pythia-gen-v11-Zpt0.root  \
        --MixFile mergedSample/pythia-gen-v11-Zpt0.root  \
        --UseEventWeight true --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --yBoost 0 --nMix $nMix
    
fi

# pPb
if [ "$DOPPB" == "1" ]; then

    ./system-analysis.sh "pPbMC_Gen${TAG}" \
        --IsPP false --IsGenZ true --IsData false --IsPPb true \
        --Input pPbSample/V0.2/PPbMC_Gen.root \
        --MixFile pPbSample/V0.2/PPbMC_Gen.root \
        --UseEventWeight true --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --EPOSFile mergedEPOS/PPbMC_Gen.root --Fraction 1 \
        --yBoost 0 --nMix $nMix

fi

# PbP
if [ "$DOPBP" == "1" ]; then

    ./system-analysis.sh "PbPMC_Gen${TAG}" \
        --IsPP false --IsGenZ true --IsData false --IsPPb false \
        --Input pPbSample/V0.2/PbPMC_Gen.root \
        --MixFile pPbSample/V0.2/PbPMC_Gen.root \
        --UseEventWeight true --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --EPOSFile mergedEPOS/PbPMC_Gen.root --Fraction 1 \
        --yBoost 0 --nMix $nMix

fi
