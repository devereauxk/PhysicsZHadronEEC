
DOPP=$1
DOPPB=$2
DOPBP=$3

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh

# CENTRAL CLOSURE
#ZPT_RANGES=("0_10" "10_20" "20_40" "40_500")
#PT_RANGES=("0.5_500")

# ENERGY EXTRAPOLATION
#ZPT_RANGES=("5_500")
#PT_RANGES=("0.5_500")

cat > config.sh <<EOF
ZPT_RANGES=("5_500")
PT_RANGES=("0.5_500") 
EOF

# IMPORTANT: the following is intentionally hard coded to read files we KNOW are wrongly labeled. pPb and Pbp files are known to be swapped in many of the sets. Here we code files LITERALLY in order to test out the different labels.

# pp
if [ "$DOPP" == "1" ]; then

    echo "not implemented"
    exit 

fi

# pPb
if [ "$DOPPB" == "1" ]; then
    nMix=0
    TAG="_dataMCComparison_ZV6_trkV24_nmix0"

    ./system-analysis.sh "pythiapPbEPOSMC_Gen_nominal${TAG}" \
        --IsPP true --IsGenZ true --IsData false \
        --Input mergedSample/pythia-gen-v11-Zpt0.root  \
        --MixFile mergedSample/pythia-gen-v11-Zpt0.root  \
        --UseEventWeight true --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --yBoost 0 --nMix $nMix \
        --EPOSFile mergedEPOS/PPbMC_Gen.root --Fraction 1 \
        --UseVZWeight true --VZWeightFile $VZWeightFile_PP

    ./system-analysis.sh "pPbNoEPOSMC_Gen_nominal${TAG}" \
        --IsPP false --IsGenZ true --IsData false --IsPPb true \
        --Input pPbSample/V0.2/PPbMC_Gen.root \
        --MixFile pPbSample/V0.2/PPbMC_Gen.root \
        --UseEventWeight true --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --Fraction 1 \
        --yBoost 0 --nMix $nMix \
        --UseVZWeight true --VZWeightFile $VZWeightFile_PPb

    ./system-analysis.sh "pPbMC_Gen_nominal${TAG}" \
        --IsPP false --IsGenZ true --IsData false --IsPPb true \
        --Input pPbSample/V0.2/PPbMC_Gen.root \
        --MixFile pPbSample/V0.2/PPbMC_Gen.root \
        --UseEventWeight true --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --EPOSFile mergedEPOS/PPbMC_Gen.root --Fraction 1 \
        --yBoost 0 --nMix $nMix \
        --UseVZWeight true --VZWeightFile $VZWeightFile_PPb

    ./system-analysis.sh "pPbMC_nominal${TAG}" \
        --IsPP false --IsGenZ false --IsData false --IsPPb true \
        --Input pPbSample/V0.2/PPbMC_Reco.root \
        --MixFile pPbSample/V0.2/PPbMC_Reco.root \
        --UseEventWeight true --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --yBoost 0 --nMix $nMix \
        --UseVZWeight true --VZWeightFile $VZWeightFile_PPb

    ./system-analysis.sh "pPbMC_trkResidual${TAG}" \
        --IsPP false --IsGenZ false --IsData false --IsPPb true \
        --Input pPbSample/V0.2/PPbMC_Reco.root \
        --MixFile pPbSample/V0.2/PPbMC_Reco.root \
        --UseEventWeight true --UseZWeight true \
        --UseTrackWeight true --UseResidualWeight true \
        --yBoost 0 --nMix $nMix \
        --UseVZWeight true --VZWeightFile $VZWeightFile_PPb \
        --ZWeightFile $ZWeightFile_PPb \
        --ResidualWeightFile $RWeightFile_PPb

    ./system-analysis.sh "pPb_nominal${TAG}" \
        --IsPP false --IsGenZ false --IsData true --UseVZWeight false --IsPPb true \
        --Input pPbSample/V0.2/PPbData_Reco.root \
        --MixFile pPbSample/V0.2/PPbData_Reco.root \
        --UseEventWeight true --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --yBoost 0 --nMix $nMix

    ./system-analysis.sh "pPb_trkResidual${TAG}" \
        --IsPP false --IsGenZ false --IsData true --UseVZWeight false --IsPPb true \
        --Input pPbSample/V0.2/PPbData_Reco.root \
        --MixFile pPbSample/V0.2/PPbData_Reco.root \
        --UseEventWeight true --UseZWeight true \
        --UseTrackWeight true --UseResidualWeight true \
        --yBoost 0 --nMix $nMix \
        --ZWeightFile $ZWeightFile_PPb \
        --ResidualWeightFile $RWeightFile_PPb

    
fi

# PbP
if [ "$DOPBP" == "1" ]; then
    nMix=0
    TAG="_dataMCComparison_ZV6_trkV24_nmix0"

    
    ./system-analysis.sh "pythiaPbPEPOSMC_Gen_nominal${TAG}" \
        --IsPP true --IsGenZ true --IsData false \
        --Input mergedSample/pythia-gen-v11-Zpt0.root  \
        --MixFile mergedSample/pythia-gen-v11-Zpt0.root  \
        --UseEventWeight false --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --yBoost 0 --nMix $nMix \
        --EPOSFile mergedEPOS/PbPMC_Gen.root --Fraction 1 \
        --UseVZWeight true --VZWeightFile $VZWeightFile_PP

    ./system-analysis.sh "PbPNoEPOSMC_Gen_nominal${TAG}" \
        --IsPP false --IsGenZ true --IsData false --IsPPb false \
        --Input pPbSample/V0.2/PbPMC_Gen.root \
        --MixFile pPbSample/V0.2/PbPMC_Gen.root \
        --UseEventWeight true --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --Fraction 1 \
        --yBoost 0 --nMix $nMix \
        --UseVZWeight true --VZWeightFile $VZWeightFile_PbP

    ./system-analysis.sh "PbPMC_Gen_nominal${TAG}" \
        --IsPP false --IsGenZ true --IsData false --IsPPb false \
        --Input pPbSample/V0.2/PbPMC_Gen.root \
        --MixFile pPbSample/V0.2/PbPMC_Gen.root \
        --UseEventWeight true --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --EPOSFile mergedEPOS/PbPMC_Gen.root --Fraction 1 \
        --yBoost 0 --nMix $nMix \
        --UseVZWeight true --VZWeightFile $VZWeightFile_PbP

    ./system-analysis.sh "PbPMC_nominal${TAG}" \
        --IsPP false --IsGenZ false --IsData false --IsPPb false \
        --Input pPbSample/V0.2/PbPMC_Reco.root \
        --MixFile pPbSample/V0.2/PbPMC_Reco.root \
        --UseEventWeight true --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --yBoost 0 --nMix $nMix \
        --UseVZWeight true --VZWeightFile $VZWeightFile_PbP

    ./system-analysis.sh "PbPMC_trkResidual${TAG}" \
        --IsPP false --IsGenZ false --IsData false --IsPPb false \
        --Input pPbSample/V0.2/PbPMC_Reco.root \
        --MixFile pPbSample/V0.2/PbPMC_Reco.root \
        --UseEventWeight true --UseZWeight true \
        --UseTrackWeight true --UseResidualWeight true \
        --yBoost 0 --nMix $nMix \
        --UseVZWeight true --VZWeightFile $VZWeightFile_PbP \
        --ZWeightFile $ZWeightFile_PbP \
        --ResidualWeightFile $RWeightFile_PbP

    ./system-analysis.sh "PbP_nominal${TAG}" \
        --IsPP false --IsGenZ false --IsData true --UseVZWeight false --IsPPb false \
        --Input pPbSample/V0.2/PbPData_Reco.root \
        --MixFile pPbSample/V0.2/PbPData_Reco.root \
        --UseEventWeight true --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --yBoost 0 --nMix $nMix

    ./system-analysis.sh "PbP_trkResidual${TAG}" \
        --IsPP false --IsGenZ false --IsData true --UseVZWeight false --IsPPb false \
        --Input pPbSample/V0.2/PbPData_Reco.root \
        --MixFile pPbSample/V0.2/PbPData_Reco.root \
        --UseEventWeight true --UseZWeight true \
        --UseTrackWeight true --UseResidualWeight true \
        --yBoost 0 --nMix $nMix \
        --ZWeightFile $ZWeightFile_PbP \
        --ResidualWeightFile $RWeightFile_PbP

fi
