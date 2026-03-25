DOPP=$1
DOPPB=$2
DOPBP=$3

source /home/kdeverea/PhysicsZHadronEEC/OfficialWeightDictionary.sh

cat > config.sh <<EOF
ZPT_RANGES=("0_500")
PT_RANGES=("0.5_500")
EOF

if [ "${SKIP_CLEAN:-0}" != "1" ]; then
    source clean.sh
fi
export SKIP_CLEAN=1
export CUT_PARALLELISM=${CUT_PARALLELISM:-1}
export NTHREAD=${NTHREAD:-20}
export NSLICE_FACTOR=${NSLICE_FACTOR:-1}

nMix=0
TAG="${TAG_SUFFIX:-_ZV6_trkV24_nmix0}"

# pp
if [ "$DOPP" == "1" ]; then

    ./system-analysis.sh "pythiaMC_Gen_nominal${TAG}"         --IsPP true --IsGenZ true --IsData false         --Input mergedSample/pythia-gen-v11-Zpt0.root          --MixFile mergedSample/pythia-gen-v11-Zpt0.root          --UseEventWeight true --UseZWeight false         --UseTrackWeight true --UseResidualWeight false         --yBoost 0 --nMix $nMix         --UseVZWeight true --VZWeightFile ${VZWeightFile_PP}

    ./system-analysis.sh "pythiaMC_nominal${TAG}"         --IsPP true --IsGenZ false --IsData false         --Input mergedSample/pythia-v11-Zpt0.root         --MixFile mergedSample/pythia-v11-Zpt0.root         --UseEventWeight true --UseZWeight false         --UseTrackWeight true --UseResidualWeight false         --yBoost 0 --nMix $nMix         --UseVZWeight true --VZWeightFile ${VZWeightFile_PP}

    ./system-analysis.sh "pythiaMC_ZResidual${TAG}"         --IsPP true --IsGenZ false --IsData false         --Input mergedSample/pythia-v11-Zpt0.root         --MixFile mergedSample/pythia-v11-Zpt0.root         --UseEventWeight true --UseZWeight true         --UseTrackWeight true --UseResidualWeight false         --yBoost 0 --nMix $nMix         --ZWeightFile ${ZWeightFile_PP}         --UseVZWeight true --VZWeightFile ${VZWeightFile_PP}
fi

# pPb
if [ "$DOPPB" == "1" ]; then

    ./system-analysis.sh "pPbMC_Gen_nominal${TAG}"         --IsPP false --IsGenZ true --IsData false --IsPPb true         --Input pPbSample/V0.2/PbPMC_Gen.root         --MixFile pPbSample/V0.2/PbPMC_Gen.root         --UseEventWeight true --UseZWeight false         --UseTrackWeight true --UseResidualWeight false         --EPOSFile mergedEPOS/PPbMC_Gen.root --Fraction 1         --yBoost 0 --nMix $nMix         --UseVZWeight true --VZWeightFile ${VZWeightFile_PPb}

    ./system-analysis.sh "pPbMC_nominal${TAG}"         --IsPP false --IsGenZ false --IsData false --IsPPb true         --Input pPbSample/V0.2/PbPMC_Reco.root         --MixFile pPbSample/V0.2/PbPMC_Reco.root         --UseEventWeight true --UseZWeight false         --UseTrackWeight true --UseResidualWeight false         --yBoost 0 --nMix $nMix         --UseVZWeight true --VZWeightFile ${VZWeightFile_PPb}

    ./system-analysis.sh "pPbMC_ZResidual${TAG}"         --IsPP false --IsGenZ false --IsData false --IsPPb true         --Input pPbSample/V0.2/PbPMC_Reco.root         --MixFile pPbSample/V0.2/PbPMC_Reco.root         --UseEventWeight true --UseZWeight true         --UseTrackWeight true --UseResidualWeight false         --yBoost 0 --nMix $nMix         --ZWeightFile ${ZWeightFile_PPb}         --UseVZWeight true --VZWeightFile ${VZWeightFile_PPb}
fi

# PbP
if [ "$DOPBP" == "1" ]; then

    ./system-analysis.sh "PbPMC_Gen_nominal${TAG}"         --IsPP false --IsGenZ true --IsData false --IsPPb false         --Input pPbSample/V0.2/PPbMC_Gen.root         --MixFile pPbSample/V0.2/PPbMC_Gen.root         --UseEventWeight true --UseZWeight false         --UseTrackWeight true --UseResidualWeight false         --EPOSFile mergedEPOS/PbPMC_Gen.root --Fraction 1         --yBoost 0 --nMix $nMix         --UseVZWeight true --VZWeightFile ${VZWeightFile_PbP}

    ./system-analysis.sh "PbPMC_nominal${TAG}"         --IsPP false --IsGenZ false --IsData false --IsPPb false         --Input pPbSample/V0.2/PPbMC_Reco.root         --MixFile pPbSample/V0.2/PPbMC_Reco.root         --UseEventWeight true --UseZWeight false         --UseTrackWeight true --UseResidualWeight false         --yBoost 0 --nMix $nMix         --UseVZWeight true --VZWeightFile ${VZWeightFile_PbP}

    ./system-analysis.sh "PbPMC_ZResidual${TAG}"         --IsPP false --IsGenZ false --IsData false --IsPPb false         --Input pPbSample/V0.2/PPbMC_Reco.root         --MixFile pPbSample/V0.2/PPbMC_Reco.root         --UseEventWeight true --UseZWeight true         --UseTrackWeight true --UseResidualWeight false         --yBoost 0 --nMix $nMix         --ZWeightFile ${ZWeightFile_PbP}         --UseVZWeight true --VZWeightFile ${VZWeightFile_PbP}
fi
