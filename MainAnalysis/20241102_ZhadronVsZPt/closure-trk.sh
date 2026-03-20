
DOPP=$1
DOPPB=$2
DOPBP=$3

cat > config.sh <<EOF
ZPT_RANGES=("0_10" "10_20" "20_40" "40_500")
PT_RANGES=("0.5_500")
EOF

# Speedup path for repeated system-analysis calls:
# build once, then skip per-call clean/rebuild and allow optional cross-cut parallelism.
# example: SKIP_CLEAN=1 CUT_PARALLELISM=3 NTHREAD=8 NSLICE_FACTOR=1
if [ "${SKIP_CLEAN:-0}" != "1" ]; then
    source clean.sh
fi
export SKIP_CLEAN=1
export CUT_PARALLELISM=${CUT_PARALLELISM:-1}
export NTHREAD=${NTHREAD:-20}
export NSLICE_FACTOR=${NSLICE_FACTOR:-1}

VZWeightFile_PPb="${VZ_WEIGHT_FILE_PPB:-/home/kdeverea/PhysicsZHadronEEC/Plots/20251001_pPbVZReweighting/summary/20260311_ZPT0_500_VzReweightFits_pPb.root}"
VZWeightFile_PbP="${VZ_WEIGHT_FILE_PBP:-/home/kdeverea/PhysicsZHadronEEC/Plots/20251001_pPbVZReweighting/summary/20260311_ZPT0_500_VzReweightFits_PbP.root}"
ZWeightFile_PPb="${Z_WEIGHT_FILE_PPB:-my_ZWeights/20260311_ZCorrection_V6_PPb_zPt0-500.root}"
ZWeightFile_PbP="${Z_WEIGHT_FILE_PBP:-my_ZWeights/20260311_ZCorrection_V6_PbP_zPt0-500.root}"
RWeightFile_PPb="${R_WEIGHT_FILE_PPB:-my_residualWeights/20260311_TrackResidualCorrection_V24_ZWeight_V6_PPb_zPt}"
RWeightFile_PbP="${R_WEIGHT_FILE_PBP:-my_residualWeights/20260311_TrackResidualCorrection_V24_ZWeight_V6_PbP_zPt}"

# pp
if [ "$DOPP" == "1" ]; then
    nMix=10
    TAG="_ZV6_trkV24_nmix10"
    VZWeightFile=/home/kdeverea/PhysicsZHadronEEC/Plots/20251001_pPbVZReweighting/summary/20260317_ZV6_ZPT0_350_VzReweightFits_pp.root

    ./system-analysis.sh "pythiaMC_trkResidual${TAG}"         --IsPP true --IsGenZ false --IsData false         --Input mergedSample/pythia-v11-Zpt0.root         --MixFile mergedSample/pythia-v11-Zpt0.root         --UseEventWeight false --UseZWeight true         --UseTrackWeight true --UseResidualWeight true         --yBoost 0 --nMix $nMix         --VZWeightFile $VZWeightFile         --ZWeightFile my_ZWeights/20260317_ZCorrection_V6_pp_zPt0-500.root         --ResidualWeightFile my_residualWeights/20260317_TrackResidualCorrection_V24_ZWeight_V6_pp_zPt

    ./system-analysis.sh "pythiaMC_Gen_nominal${TAG}"         --IsPP true --IsGenZ true --IsData false         --Input mergedSample/pythia-gen-v11-Zpt0.root          --MixFile mergedSample/pythia-gen-v11-Zpt0.root          --UseEventWeight false --UseZWeight false         --UseTrackWeight true --UseResidualWeight false         --yBoost 0 --nMix $nMix --VZWeightFile $VZWeightFile

    ./system-analysis.sh "pythiaMC_nominal${TAG}"         --IsPP true --IsGenZ false --IsData false         --Input mergedSample/pythia-v11-Zpt0.root         --MixFile mergedSample/pythia-v11-Zpt0.root         --UseEventWeight false --UseZWeight false         --UseTrackWeight true --UseResidualWeight false         --yBoost 0 --nMix $nMix --VZWeightFile $VZWeightFile

    ./system-analysis.sh "pythiaMC_ZResidual${TAG}"         --IsPP true --IsGenZ false --IsData false         --Input mergedSample/pythia-v11-Zpt0.root         --MixFile mergedSample/pythia-v11-Zpt0.root         --UseEventWeight false --UseZWeight true         --UseTrackWeight true --UseResidualWeight false         --yBoost 0 --nMix $nMix         --VZWeightFile $VZWeightFile         --ZWeightFile my_ZWeights/20260317_ZCorrection_V6_pp_zPt0-500.root
fi

# pPb
if [ "$DOPPB" == "1" ]; then
    nMix=10
    TAG="_ZV6_trkV24_nmix10"

    ./system-analysis.sh "pPbMC_Gen_nominal${TAG}"         --IsPP false --IsGenZ true --IsData false --IsPPb true         --Input pPbSample/V0.2/PbPMC_Gen.root         --MixFile pPbSample/V0.2/PbPMC_Gen.root         --UseEventWeight true --UseZWeight false         --UseTrackWeight true --UseResidualWeight false         --EPOSFile mergedEPOS/PPbMC_Gen.root --Fraction 1         --yBoost 0 --nMix $nMix         --VZWeightFile ${VZWeightFile_PPb}

    ./system-analysis.sh "pPbMC_nominal${TAG}"         --IsPP false --IsGenZ false --IsData false --IsPPb true         --Input pPbSample/V0.2/PbPMC_Reco.root         --MixFile pPbSample/V0.2/PbPMC_Reco.root         --UseEventWeight true --UseZWeight false         --UseTrackWeight true --UseResidualWeight false         --yBoost 0 --nMix $nMix         --VZWeightFile ${VZWeightFile_PPb}

    ./system-analysis.sh "pPbMC_ZResidual${TAG}"         --IsPP false --IsGenZ false --IsData false --IsPPb true         --Input pPbSample/V0.2/PbPMC_Reco.root         --MixFile pPbSample/V0.2/PbPMC_Reco.root         --UseEventWeight true --UseZWeight true         --UseTrackWeight true --UseResidualWeight false         --yBoost 0 --nMix $nMix         --ZWeightFile ${ZWeightFile_PPb}         --VZWeightFile ${VZWeightFile_PPb}

    ./system-analysis.sh "pPbMC_trkResidual${TAG}"         --IsPP false --IsGenZ false --IsData false --IsPPb true         --Input pPbSample/V0.2/PbPMC_Reco.root         --MixFile pPbSample/V0.2/PbPMC_Reco.root         --UseEventWeight true --UseZWeight true         --UseTrackWeight true --UseResidualWeight true         --yBoost 0 --nMix $nMix         --ZWeightFile ${ZWeightFile_PPb}         --ResidualWeightFile ${RWeightFile_PPb}         --VZWeightFile ${VZWeightFile_PPb}
fi

# PbP
if [ "$DOPBP" == "1" ]; then
    nMix=10
    TAG="_ZV6_trkV24_nmix10"

    ./system-analysis.sh "PbPMC_Gen_nominal${TAG}"         --IsPP false --IsGenZ true --IsData false --IsPPb false         --Input pPbSample/V0.2/PPbMC_Gen.root         --MixFile pPbSample/V0.2/PPbMC_Gen.root         --UseEventWeight true --UseZWeight false         --UseTrackWeight true --UseResidualWeight false         --EPOSFile mergedEPOS/PbPMC_Gen.root --Fraction 1         --yBoost 0 --nMix $nMix         --VZWeightFile ${VZWeightFile_PbP}

    ./system-analysis.sh "PbPMC_nominal${TAG}"         --IsPP false --IsGenZ false --IsData false --IsPPb false         --Input pPbSample/V0.2/PPbMC_Reco.root         --MixFile pPbSample/V0.2/PPbMC_Reco.root         --UseEventWeight true --UseZWeight false         --UseTrackWeight true --UseResidualWeight false         --yBoost 0 --nMix $nMix         --VZWeightFile ${VZWeightFile_PbP}

    ./system-analysis.sh "PbPMC_ZResidual${TAG}"         --IsPP false --IsGenZ false --IsData false --IsPPb false         --Input pPbSample/V0.2/PPbMC_Reco.root         --MixFile pPbSample/V0.2/PPbMC_Reco.root         --UseEventWeight true --UseZWeight true         --UseTrackWeight true --UseResidualWeight false         --yBoost 0 --nMix $nMix         --ZWeightFile ${ZWeightFile_PbP}         --VZWeightFile ${VZWeightFile_PbP}

    ./system-analysis.sh "PbPMC_trkResidual${TAG}"         --IsPP false --IsGenZ false --IsData false --IsPPb false         --Input pPbSample/V0.2/PPbMC_Reco.root         --MixFile pPbSample/V0.2/PPbMC_Reco.root         --UseEventWeight true --UseZWeight true         --UseTrackWeight true --UseResidualWeight true         --yBoost 0 --nMix $nMix         --ZWeightFile ${ZWeightFile_PbP}         --ResidualWeightFile ${RWeightFile_PbP}         --VZWeightFile ${VZWeightFile_PbP}
fi
