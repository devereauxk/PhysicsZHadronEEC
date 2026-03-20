#!/bin/bash

# Controlled matrix for Z-step audit (reduced scope by default):
#   ZPT=20_40, trkPT=0.5_500, pPb/PbP only.
# Cases:
#   A: OLD-reference reproduction
#   B: OLD + PbP ZResidual file fix only
#   C: OLD + VZ-file-on (OLD Z files)
#   D: NEW full chain

CASE_SEL=${1:-ALL}   # A|B|C|D|ALL
DOPPB=${2:-1}
DOPBP=${3:-1}

if [ "${AUDIT_FULL:-0}" == "1" ]; then
    cat > config.sh <<EOF
ZPT_RANGES=("0_10" "10_20" "20_40" "40_500")
PT_RANGES=("0.5_500")
EOF
else
    cat > config.sh <<EOF
ZPT_RANGES=("20_40")
PT_RANGES=("0.5_500")
EOF
fi

if [ "${SKIP_CLEAN:-0}" != "1" ]; then
    source clean.sh
fi
export SKIP_CLEAN=1
export CUT_PARALLELISM=${CUT_PARALLELISM:-1}
export NTHREAD=${NTHREAD:-8}
export NSLICE_FACTOR=${NSLICE_FACTOR:-1}

nMix=10

run_case_A() {
    TAG="_zstepAuditA_oldref_ZV5_trkV23_nmix10"
    VZWeightFile_PPB="/home/kdeverea/PhysicsZHadronEEC/Plots/20251001_pPbVZReweighting/summary/20260311_ZPT0_500_VzReweightFits_pPb.root"
    VZWeightFile_PBP="/home/kdeverea/PhysicsZHadronEEC/Plots/20251001_pPbVZReweighting/summary/20260311_ZPT0_500_VzReweightFits_PbP.root"

    if [ "$DOPPB" == "1" ]; then
        ./system-analysis.sh "pPbMC_Gen_nominal${TAG}" \
            --IsPP false --IsGenZ true --IsData false --IsPPb true \
            --Input pPbSample/V0.2/PPbMC_Gen.root --MixFile pPbSample/V0.2/PPbMC_Gen.root \
            --UseEventWeight true --UseZWeight false --UseTrackWeight true --UseResidualWeight false \
            --EPOSFile mergedEPOS/PPbMC_Gen.root --Fraction 1 --yBoost 0 --nMix $nMix --VZWeightFile "$VZWeightFile_PPB"
        ./system-analysis.sh "pPbMC_nominal${TAG}" \
            --IsPP false --IsGenZ false --IsData false --IsPPb true \
            --Input pPbSample/V0.2/PPbMC_Reco.root --MixFile pPbSample/V0.2/PPbMC_Reco.root \
            --UseEventWeight true --UseZWeight false --UseTrackWeight true --UseResidualWeight false \
            --yBoost 0 --nMix $nMix --VZWeightFile "$VZWeightFile_PPB"
        ./system-analysis.sh "pPbMC_ZResidual${TAG}" \
            --IsPP false --IsGenZ false --IsData false --IsPPb true \
            --Input pPbSample/V0.2/PPbMC_Reco.root --MixFile pPbSample/V0.2/PPbMC_Reco.root \
            --UseEventWeight true --UseZWeight true --UseTrackWeight true --UseResidualWeight false \
            --yBoost 0 --nMix $nMix --ZWeightFile my_ZWeights/20260202_ZCorrection_V5_PPb_zPt0-500.root \
            --VZWeightFile "$VZWeightFile_PPB"
        ./system-analysis.sh "pPbMC_trkResidual${TAG}" \
            --IsPP false --IsGenZ false --IsData false --IsPPb true \
            --Input pPbSample/V0.2/PPbMC_Reco.root --MixFile pPbSample/V0.2/PPbMC_Reco.root \
            --UseEventWeight true --UseZWeight true --UseTrackWeight true --UseResidualWeight true \
            --yBoost 0 --nMix $nMix --ZWeightFile my_ZWeights/20260202_ZCorrection_V5_PPb_zPt0-500.root \
            --ResidualWeightFile my_residualWeights/20260223_TrackResidualCorrection_V23_ZWeight_V5_PPb_zPt \
            --VZWeightFile "$VZWeightFile_PPB"
    fi

    if [ "$DOPBP" == "1" ]; then
        ./system-analysis.sh "PbPMC_Gen_nominal${TAG}" \
            --IsPP false --IsGenZ true --IsData false --IsPPb false \
            --Input pPbSample/V0.2/PbPMC_Gen.root --MixFile pPbSample/V0.2/PbPMC_Gen.root \
            --UseEventWeight true --UseZWeight false --UseTrackWeight true --UseResidualWeight false \
            --EPOSFile mergedEPOS/PbPMC_Gen.root --Fraction 1 --yBoost 0 --nMix $nMix --VZWeightFile "$VZWeightFile_PBP"
        ./system-analysis.sh "PbPMC_nominal${TAG}" \
            --IsPP false --IsGenZ false --IsData false --IsPPb false \
            --Input pPbSample/V0.2/PbPMC_Reco.root --MixFile pPbSample/V0.2/PbPMC_Reco.root \
            --UseEventWeight true --UseZWeight false --UseTrackWeight true --UseResidualWeight false \
            --yBoost 0 --nMix $nMix --VZWeightFile "$VZWeightFile_PBP"
        ./system-analysis.sh "PbPMC_ZResidual${TAG}" \
            --IsPP false --IsGenZ false --IsData false --IsPPb false \
            --Input pPbSample/V0.2/PbPMC_Reco.root --MixFile pPbSample/V0.2/PbPMC_Reco.root \
            --UseEventWeight true --UseZWeight true --UseTrackWeight true --UseResidualWeight false \
            --yBoost 0 --nMix $nMix --ZWeightFile my_ZWeights/20260202_ZCorrection_V5_PPb_zPt0-500.root \
            --VZWeightFile "$VZWeightFile_PBP"
        ./system-analysis.sh "PbPMC_trkResidual${TAG}" \
            --IsPP false --IsGenZ false --IsData false --IsPPb false \
            --Input pPbSample/V0.2/PbPMC_Reco.root --MixFile pPbSample/V0.2/PbPMC_Reco.root \
            --UseEventWeight true --UseZWeight true --UseTrackWeight true --UseResidualWeight true \
            --yBoost 0 --nMix $nMix --ZWeightFile my_ZWeights/20260202_ZCorrection_V5_PbP_zPt0-500.root \
            --ResidualWeightFile my_residualWeights/20260223_TrackResidualCorrection_V23_ZWeight_V5_PbP_zPt \
            --VZWeightFile "$VZWeightFile_PBP"
    fi
}

run_case_B() {
    TAG="_zstepAuditB_oldfixPbP_ZV5_trkV23_nmix10"
    VZWeightFile_PPB="/home/kdeverea/PhysicsZHadronEEC/Plots/20251001_pPbVZReweighting/summary/20260311_ZPT0_500_VzReweightFits_pPb.root"
    VZWeightFile_PBP="/home/kdeverea/PhysicsZHadronEEC/Plots/20251001_pPbVZReweighting/summary/20260311_ZPT0_500_VzReweightFits_PbP.root"

    if [ "$DOPPB" == "1" ]; then
        ./system-analysis.sh "pPbMC_Gen_nominal${TAG}" \
            --IsPP false --IsGenZ true --IsData false --IsPPb true \
            --Input pPbSample/V0.2/PPbMC_Gen.root --MixFile pPbSample/V0.2/PPbMC_Gen.root \
            --UseEventWeight true --UseZWeight false --UseTrackWeight true --UseResidualWeight false \
            --EPOSFile mergedEPOS/PPbMC_Gen.root --Fraction 1 --yBoost 0 --nMix $nMix --VZWeightFile "$VZWeightFile_PPB"
        ./system-analysis.sh "pPbMC_nominal${TAG}" \
            --IsPP false --IsGenZ false --IsData false --IsPPb true \
            --Input pPbSample/V0.2/PPbMC_Reco.root --MixFile pPbSample/V0.2/PPbMC_Reco.root \
            --UseEventWeight true --UseZWeight false --UseTrackWeight true --UseResidualWeight false \
            --yBoost 0 --nMix $nMix --VZWeightFile "$VZWeightFile_PPB"
        ./system-analysis.sh "pPbMC_ZResidual${TAG}" \
            --IsPP false --IsGenZ false --IsData false --IsPPb true \
            --Input pPbSample/V0.2/PPbMC_Reco.root --MixFile pPbSample/V0.2/PPbMC_Reco.root \
            --UseEventWeight true --UseZWeight true --UseTrackWeight true --UseResidualWeight false \
            --yBoost 0 --nMix $nMix --ZWeightFile my_ZWeights/20260202_ZCorrection_V5_PPb_zPt0-500.root \
            --VZWeightFile "$VZWeightFile_PPB"
        ./system-analysis.sh "pPbMC_trkResidual${TAG}" \
            --IsPP false --IsGenZ false --IsData false --IsPPb true \
            --Input pPbSample/V0.2/PPbMC_Reco.root --MixFile pPbSample/V0.2/PPbMC_Reco.root \
            --UseEventWeight true --UseZWeight true --UseTrackWeight true --UseResidualWeight true \
            --yBoost 0 --nMix $nMix --ZWeightFile my_ZWeights/20260202_ZCorrection_V5_PPb_zPt0-500.root \
            --ResidualWeightFile my_residualWeights/20260223_TrackResidualCorrection_V23_ZWeight_V5_PPb_zPt \
            --VZWeightFile "$VZWeightFile_PPB"
    fi

    if [ "$DOPBP" == "1" ]; then
        ./system-analysis.sh "PbPMC_Gen_nominal${TAG}" \
            --IsPP false --IsGenZ true --IsData false --IsPPb false \
            --Input pPbSample/V0.2/PbPMC_Gen.root --MixFile pPbSample/V0.2/PbPMC_Gen.root \
            --UseEventWeight true --UseZWeight false --UseTrackWeight true --UseResidualWeight false \
            --EPOSFile mergedEPOS/PbPMC_Gen.root --Fraction 1 --yBoost 0 --nMix $nMix --VZWeightFile "$VZWeightFile_PBP"
        ./system-analysis.sh "PbPMC_nominal${TAG}" \
            --IsPP false --IsGenZ false --IsData false --IsPPb false \
            --Input pPbSample/V0.2/PbPMC_Reco.root --MixFile pPbSample/V0.2/PbPMC_Reco.root \
            --UseEventWeight true --UseZWeight false --UseTrackWeight true --UseResidualWeight false \
            --yBoost 0 --nMix $nMix --VZWeightFile "$VZWeightFile_PBP"
        ./system-analysis.sh "PbPMC_ZResidual${TAG}" \
            --IsPP false --IsGenZ false --IsData false --IsPPb false \
            --Input pPbSample/V0.2/PbPMC_Reco.root --MixFile pPbSample/V0.2/PbPMC_Reco.root \
            --UseEventWeight true --UseZWeight true --UseTrackWeight true --UseResidualWeight false \
            --yBoost 0 --nMix $nMix --ZWeightFile my_ZWeights/20260202_ZCorrection_V5_PbP_zPt0-500.root \
            --VZWeightFile "$VZWeightFile_PBP"
        ./system-analysis.sh "PbPMC_trkResidual${TAG}" \
            --IsPP false --IsGenZ false --IsData false --IsPPb false \
            --Input pPbSample/V0.2/PbPMC_Reco.root --MixFile pPbSample/V0.2/PbPMC_Reco.root \
            --UseEventWeight true --UseZWeight true --UseTrackWeight true --UseResidualWeight true \
            --yBoost 0 --nMix $nMix --ZWeightFile my_ZWeights/20260202_ZCorrection_V5_PbP_zPt0-500.root \
            --ResidualWeightFile my_residualWeights/20260223_TrackResidualCorrection_V23_ZWeight_V5_PbP_zPt \
            --VZWeightFile "$VZWeightFile_PBP"
    fi
}

run_case_C() {
    TAG="_zstepAuditC_oldVzOn_ZV5_trkV23_nmix10"
    VZWeightFile_PPB="/home/kdeverea/PhysicsZHadronEEC/Plots/20251001_pPbVZReweighting/summary/20260318_newVZFix_ZPT0_500_VzReweightFits_pPb.root"
    VZWeightFile_PBP="/home/kdeverea/PhysicsZHadronEEC/Plots/20251001_pPbVZReweighting/summary/20260318_newVZFix_ZPT0_500_VzReweightFits_PbP.root"

    if [ "$DOPPB" == "1" ]; then
        ./system-analysis.sh "pPbMC_Gen_nominal${TAG}" \
            --IsPP false --IsGenZ true --IsData false --IsPPb true \
            --Input pPbSample/V0.2/PPbMC_Gen.root --MixFile pPbSample/V0.2/PPbMC_Gen.root \
            --UseEventWeight true --UseZWeight false --UseTrackWeight true --UseResidualWeight false \
            --EPOSFile mergedEPOS/PPbMC_Gen.root --Fraction 1 --yBoost 0 --nMix $nMix --VZWeightFile "$VZWeightFile_PPB"
        ./system-analysis.sh "pPbMC_nominal${TAG}" \
            --IsPP false --IsGenZ false --IsData false --IsPPb true \
            --Input pPbSample/V0.2/PPbMC_Reco.root --MixFile pPbSample/V0.2/PPbMC_Reco.root \
            --UseEventWeight true --UseZWeight false --UseTrackWeight true --UseResidualWeight false \
            --yBoost 0 --nMix $nMix --VZWeightFile "$VZWeightFile_PPB"
        ./system-analysis.sh "pPbMC_ZResidual${TAG}" \
            --IsPP false --IsGenZ false --IsData false --IsPPb true \
            --Input pPbSample/V0.2/PPbMC_Reco.root --MixFile pPbSample/V0.2/PPbMC_Reco.root \
            --UseEventWeight true --UseZWeight true --UseTrackWeight true --UseResidualWeight false \
            --yBoost 0 --nMix $nMix --ZWeightFile my_ZWeights/20260202_ZCorrection_V5_PPb_zPt0-500.root \
            --VZWeightFile "$VZWeightFile_PPB"
        ./system-analysis.sh "pPbMC_trkResidual${TAG}" \
            --IsPP false --IsGenZ false --IsData false --IsPPb true \
            --Input pPbSample/V0.2/PPbMC_Reco.root --MixFile pPbSample/V0.2/PPbMC_Reco.root \
            --UseEventWeight true --UseZWeight true --UseTrackWeight true --UseResidualWeight true \
            --yBoost 0 --nMix $nMix --ZWeightFile my_ZWeights/20260202_ZCorrection_V5_PPb_zPt0-500.root \
            --ResidualWeightFile my_residualWeights/20260223_TrackResidualCorrection_V23_ZWeight_V5_PPb_zPt \
            --VZWeightFile "$VZWeightFile_PPB"
    fi

    if [ "$DOPBP" == "1" ]; then
        ./system-analysis.sh "PbPMC_Gen_nominal${TAG}" \
            --IsPP false --IsGenZ true --IsData false --IsPPb false \
            --Input pPbSample/V0.2/PbPMC_Gen.root --MixFile pPbSample/V0.2/PbPMC_Gen.root \
            --UseEventWeight true --UseZWeight false --UseTrackWeight true --UseResidualWeight false \
            --EPOSFile mergedEPOS/PbPMC_Gen.root --Fraction 1 --yBoost 0 --nMix $nMix --VZWeightFile "$VZWeightFile_PBP"
        ./system-analysis.sh "PbPMC_nominal${TAG}" \
            --IsPP false --IsGenZ false --IsData false --IsPPb false \
            --Input pPbSample/V0.2/PbPMC_Reco.root --MixFile pPbSample/V0.2/PbPMC_Reco.root \
            --UseEventWeight true --UseZWeight false --UseTrackWeight true --UseResidualWeight false \
            --yBoost 0 --nMix $nMix --VZWeightFile "$VZWeightFile_PBP"
        ./system-analysis.sh "PbPMC_ZResidual${TAG}" \
            --IsPP false --IsGenZ false --IsData false --IsPPb false \
            --Input pPbSample/V0.2/PbPMC_Reco.root --MixFile pPbSample/V0.2/PbPMC_Reco.root \
            --UseEventWeight true --UseZWeight true --UseTrackWeight true --UseResidualWeight false \
            --yBoost 0 --nMix $nMix --ZWeightFile my_ZWeights/20260202_ZCorrection_V5_PPb_zPt0-500.root \
            --VZWeightFile "$VZWeightFile_PBP"
        ./system-analysis.sh "PbPMC_trkResidual${TAG}" \
            --IsPP false --IsGenZ false --IsData false --IsPPb false \
            --Input pPbSample/V0.2/PbPMC_Reco.root --MixFile pPbSample/V0.2/PbPMC_Reco.root \
            --UseEventWeight true --UseZWeight true --UseTrackWeight true --UseResidualWeight true \
            --yBoost 0 --nMix $nMix --ZWeightFile my_ZWeights/20260202_ZCorrection_V5_PbP_zPt0-500.root \
            --ResidualWeightFile my_residualWeights/20260223_TrackResidualCorrection_V23_ZWeight_V5_PbP_zPt \
            --VZWeightFile "$VZWeightFile_PBP"
    fi
}

run_case_D() {
    TAG="_zstepAuditD_newfull_ZV6_trkV24_nmix10"
    VZWeightFile_PPB="/home/kdeverea/PhysicsZHadronEEC/Plots/20251001_pPbVZReweighting/summary/20260318_newVZFix_ZPT0_500_VzReweightFits_pPb.root"
    VZWeightFile_PBP="/home/kdeverea/PhysicsZHadronEEC/Plots/20251001_pPbVZReweighting/summary/20260318_newVZFix_ZPT0_500_VzReweightFits_PbP.root"

    if [ "$DOPPB" == "1" ]; then
        ./system-analysis.sh "pPbMC_Gen_nominal${TAG}" \
            --IsPP false --IsGenZ true --IsData false --IsPPb true \
            --Input pPbSample/V0.2/PPbMC_Gen.root --MixFile pPbSample/V0.2/PPbMC_Gen.root \
            --UseEventWeight true --UseZWeight false --UseTrackWeight true --UseResidualWeight false \
            --EPOSFile mergedEPOS/PPbMC_Gen.root --Fraction 1 --yBoost 0 --nMix $nMix --VZWeightFile "$VZWeightFile_PPB"
        ./system-analysis.sh "pPbMC_nominal${TAG}" \
            --IsPP false --IsGenZ false --IsData false --IsPPb true \
            --Input pPbSample/V0.2/PPbMC_Reco.root --MixFile pPbSample/V0.2/PPbMC_Reco.root \
            --UseEventWeight true --UseZWeight false --UseTrackWeight true --UseResidualWeight false \
            --yBoost 0 --nMix $nMix --VZWeightFile "$VZWeightFile_PPB"
        ./system-analysis.sh "pPbMC_ZResidual${TAG}" \
            --IsPP false --IsGenZ false --IsData false --IsPPb true \
            --Input pPbSample/V0.2/PPbMC_Reco.root --MixFile pPbSample/V0.2/PPbMC_Reco.root \
            --UseEventWeight true --UseZWeight true --UseTrackWeight true --UseResidualWeight false \
            --yBoost 0 --nMix $nMix --ZWeightFile my_ZWeights/20260318_ZCorrection_V6_newVZFix_PPb_zPt0-500.root \
            --VZWeightFile "$VZWeightFile_PPB"
        ./system-analysis.sh "pPbMC_trkResidual${TAG}" \
            --IsPP false --IsGenZ false --IsData false --IsPPb true \
            --Input pPbSample/V0.2/PPbMC_Reco.root --MixFile pPbSample/V0.2/PPbMC_Reco.root \
            --UseEventWeight true --UseZWeight true --UseTrackWeight true --UseResidualWeight true \
            --yBoost 0 --nMix $nMix --ZWeightFile my_ZWeights/20260318_ZCorrection_V6_newVZFix_PPb_zPt0-500.root \
            --ResidualWeightFile my_residualWeights/20260318_TrackResidualCorrection_V24_ZWeight_V6_newVZFix_PPb_zPt \
            --VZWeightFile "$VZWeightFile_PPB"
    fi

    if [ "$DOPBP" == "1" ]; then
        ./system-analysis.sh "PbPMC_Gen_nominal${TAG}" \
            --IsPP false --IsGenZ true --IsData false --IsPPb false \
            --Input pPbSample/V0.2/PbPMC_Gen.root --MixFile pPbSample/V0.2/PbPMC_Gen.root \
            --UseEventWeight true --UseZWeight false --UseTrackWeight true --UseResidualWeight false \
            --EPOSFile mergedEPOS/PbPMC_Gen.root --Fraction 1 --yBoost 0 --nMix $nMix --VZWeightFile "$VZWeightFile_PBP"
        ./system-analysis.sh "PbPMC_nominal${TAG}" \
            --IsPP false --IsGenZ false --IsData false --IsPPb false \
            --Input pPbSample/V0.2/PbPMC_Reco.root --MixFile pPbSample/V0.2/PbPMC_Reco.root \
            --UseEventWeight true --UseZWeight false --UseTrackWeight true --UseResidualWeight false \
            --yBoost 0 --nMix $nMix --VZWeightFile "$VZWeightFile_PBP"
        ./system-analysis.sh "PbPMC_ZResidual${TAG}" \
            --IsPP false --IsGenZ false --IsData false --IsPPb false \
            --Input pPbSample/V0.2/PbPMC_Reco.root --MixFile pPbSample/V0.2/PbPMC_Reco.root \
            --UseEventWeight true --UseZWeight true --UseTrackWeight true --UseResidualWeight false \
            --yBoost 0 --nMix $nMix --ZWeightFile my_ZWeights/20260318_ZCorrection_V6_newVZFix_PbP_zPt0-500.root \
            --VZWeightFile "$VZWeightFile_PBP"
        ./system-analysis.sh "PbPMC_trkResidual${TAG}" \
            --IsPP false --IsGenZ false --IsData false --IsPPb false \
            --Input pPbSample/V0.2/PbPMC_Reco.root --MixFile pPbSample/V0.2/PbPMC_Reco.root \
            --UseEventWeight true --UseZWeight true --UseTrackWeight true --UseResidualWeight true \
            --yBoost 0 --nMix $nMix --ZWeightFile my_ZWeights/20260318_ZCorrection_V6_newVZFix_PbP_zPt0-500.root \
            --ResidualWeightFile my_residualWeights/20260318_TrackResidualCorrection_V24_ZWeight_V6_newVZFix_PbP_zPt \
            --VZWeightFile "$VZWeightFile_PBP"
    fi
}

run_selected() {
    case "$CASE_SEL" in
        A) run_case_A ;;
        B) run_case_B ;;
        C) run_case_C ;;
        D) run_case_D ;;
        ALL)
            run_case_A
            run_case_B
            run_case_C
            run_case_D
            ;;
        *)
            echo "Unknown CASE_SEL=$CASE_SEL. Use A|B|C|D|ALL."
            exit 1
            ;;
    esac
}

run_selected
