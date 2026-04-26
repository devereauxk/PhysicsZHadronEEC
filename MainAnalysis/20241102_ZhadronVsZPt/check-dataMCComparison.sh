
DOPP=$1
DOPPB=$2
DOPBP=$3

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh

PP_MCGENINPUT=${PP_MCGENINPUT:-${OFFICIAL_MCGENINPUT_PP}}
PPB_MCGENINPUT=${PPB_MCGENINPUT:-${OFFICIAL_MCGENINPUT_PPB}}
PPB_MCRECOINPUT=${PPB_MCRECOINPUT:-${OFFICIAL_MCRECOINPUT_PPB}}
PPB_DATAINPUT=${PPB_DATAINPUT:-${OFFICIAL_DATAINPUT_PPB}}
PPB_EPOSINPUT=${PPB_EPOSINPUT:-${OFFICIAL_EPOSINPUT_PPB}}
PBP_MCGENINPUT=${PBP_MCGENINPUT:-${OFFICIAL_MCGENINPUT_PBP}}
PBP_MCRECOINPUT=${PBP_MCRECOINPUT:-${OFFICIAL_MCRECOINPUT_PBP}}
PBP_DATAINPUT=${PBP_DATAINPUT:-${OFFICIAL_DATAINPUT_PBP}}
PBP_EPOSINPUT=${PBP_EPOSINPUT:-${OFFICIAL_EPOSINPUT_PBP}}

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

# Use the official dictionary-backed inputs so the direct-label V0.1 PA convention stays consistent across comparison reruns.

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
        --Input ${PP_MCGENINPUT}  \
        --MixFile ${PP_MCGENINPUT}  \
        --UseEventWeight true --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --yBoost 0 --nMix $nMix \
        --EPOSFile ${PPB_EPOSINPUT} --Fraction 1 \
        --UseVZWeight true --VZWeightFile $VZWeightFile_PP

    ./system-analysis.sh "pPbNoEPOSMC_Gen_nominal${TAG}" \
        --IsPP false --IsGenZ true --IsData false --IsPPb true \
        --Input ${PPB_MCGENINPUT} \
        --MixFile ${PPB_MCGENINPUT} \
        --UseEventWeight true --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --Fraction 1 \
        --yBoost 0 --nMix $nMix \
        --UseVZWeight true --VZWeightFile $VZWeightFile_PPb

    ./system-analysis.sh "pPbMC_Gen_nominal${TAG}" \
        --IsPP false --IsGenZ true --IsData false --IsPPb true \
        --Input ${PPB_MCGENINPUT} \
        --MixFile ${PPB_MCGENINPUT} \
        --UseEventWeight true --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --EPOSFile ${PPB_EPOSINPUT} --Fraction 1 \
        --yBoost 0 --nMix $nMix \
        --UseVZWeight true --VZWeightFile $VZWeightFile_PPb

    ./system-analysis.sh "pPbMC_nominal${TAG}" \
        --IsPP false --IsGenZ false --IsData false --IsPPb true \
        --Input ${PPB_MCRECOINPUT} \
        --MixFile ${PPB_MCRECOINPUT} \
        --UseEventWeight true --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --yBoost 0 --nMix $nMix \
        --UseVZWeight true --VZWeightFile $VZWeightFile_PPb

    ./system-analysis.sh "pPbMC_trkResidual${TAG}" \
        --IsPP false --IsGenZ false --IsData false --IsPPb true \
        --Input ${PPB_MCRECOINPUT} \
        --MixFile ${PPB_MCRECOINPUT} \
        --UseEventWeight true --UseZWeight true \
        --UseTrackWeight true --UseResidualWeight true \
        --yBoost 0 --nMix $nMix \
        --UseVZWeight true --VZWeightFile $VZWeightFile_PPb \
        --ZWeightFile $ZWeightFile_PPb \
        --ResidualWeightFile $RWeightFile_PPb

    ./system-analysis.sh "pPb_nominal${TAG}" \
        --IsPP false --IsGenZ false --IsData true --UseVZWeight false --IsPPb true \
        --Input ${PPB_DATAINPUT} \
        --MixFile ${PPB_DATAINPUT} \
        --UseEventWeight true --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --yBoost 0 --nMix $nMix

    ./system-analysis.sh "pPb_trkResidual${TAG}" \
        --IsPP false --IsGenZ false --IsData true --UseVZWeight false --IsPPb true \
        --Input ${PPB_DATAINPUT} \
        --MixFile ${PPB_DATAINPUT} \
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
        --Input ${PP_MCGENINPUT}  \
        --MixFile ${PP_MCGENINPUT}  \
        --UseEventWeight false --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --yBoost 0 --nMix $nMix \
        --EPOSFile ${PBP_EPOSINPUT} --Fraction 1 \
        --UseVZWeight true --VZWeightFile $VZWeightFile_PP

    ./system-analysis.sh "PbPNoEPOSMC_Gen_nominal${TAG}" \
        --IsPP false --IsGenZ true --IsData false --IsPPb false \
        --Input ${PBP_MCGENINPUT} \
        --MixFile ${PBP_MCGENINPUT} \
        --UseEventWeight true --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --Fraction 1 \
        --yBoost 0 --nMix $nMix \
        --UseVZWeight true --VZWeightFile $VZWeightFile_PbP

    ./system-analysis.sh "PbPMC_Gen_nominal${TAG}" \
        --IsPP false --IsGenZ true --IsData false --IsPPb false \
        --Input ${PBP_MCGENINPUT} \
        --MixFile ${PBP_MCGENINPUT} \
        --UseEventWeight true --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --EPOSFile ${PBP_EPOSINPUT} --Fraction 1 \
        --yBoost 0 --nMix $nMix \
        --UseVZWeight true --VZWeightFile $VZWeightFile_PbP

    ./system-analysis.sh "PbPMC_nominal${TAG}" \
        --IsPP false --IsGenZ false --IsData false --IsPPb false \
        --Input ${PBP_MCRECOINPUT} \
        --MixFile ${PBP_MCRECOINPUT} \
        --UseEventWeight true --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --yBoost 0 --nMix $nMix \
        --UseVZWeight true --VZWeightFile $VZWeightFile_PbP

    ./system-analysis.sh "PbPMC_trkResidual${TAG}" \
        --IsPP false --IsGenZ false --IsData false --IsPPb false \
        --Input ${PBP_MCRECOINPUT} \
        --MixFile ${PBP_MCRECOINPUT} \
        --UseEventWeight true --UseZWeight true \
        --UseTrackWeight true --UseResidualWeight true \
        --yBoost 0 --nMix $nMix \
        --UseVZWeight true --VZWeightFile $VZWeightFile_PbP \
        --ZWeightFile $ZWeightFile_PbP \
        --ResidualWeightFile $RWeightFile_PbP

    ./system-analysis.sh "PbP_nominal${TAG}" \
        --IsPP false --IsGenZ false --IsData true --UseVZWeight false --IsPPb false \
        --Input ${PBP_DATAINPUT} \
        --MixFile ${PBP_DATAINPUT} \
        --UseEventWeight true --UseZWeight false \
        --UseTrackWeight true --UseResidualWeight false \
        --yBoost 0 --nMix $nMix

    ./system-analysis.sh "PbP_trkResidual${TAG}" \
        --IsPP false --IsGenZ false --IsData true --UseVZWeight false --IsPPb false \
        --Input ${PBP_DATAINPUT} \
        --MixFile ${PBP_DATAINPUT} \
        --UseEventWeight true --UseZWeight true \
        --UseTrackWeight true --UseResidualWeight true \
        --yBoost 0 --nMix $nMix \
        --ZWeightFile $ZWeightFile_PbP \
        --ResidualWeightFile $RWeightFile_PbP

fi
