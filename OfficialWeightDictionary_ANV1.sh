#!/bin/bash

# Official promoted correction-stack weights (20260321).
# Scripts should source this file instead of hardcoding weight paths.
# Official promoted analysis tags are also defined here so runner scripts
# pair the tag string directly with the promoted correction stack.
# Environment overrides are still supported through:
#   VZ_WEIGHT_FILE_PP / VZ_WEIGHT_FILE_PPB / VZ_WEIGHT_FILE_PBP
#   Z_WEIGHT_FILE_PP  / Z_WEIGHT_FILE_PPB  / Z_WEIGHT_FILE_PBP
#   R_WEIGHT_FILE_PP  / R_WEIGHT_FILE_PPB  / R_WEIGHT_FILE_PBP
#   EE_WEIGHT_FILE_PP

OFFICIAL_VZ_WEIGHT_FILE_PP="/home/kdeverea/PhysicsZHadronEEC/Plots/20251001_pPbVZReweighting/summary/20260321_ZPT0_500_VzReweightFits_pp.root"
OFFICIAL_VZ_WEIGHT_FILE_PPB="/home/kdeverea/PhysicsZHadronEEC/Plots/20251001_pPbVZReweighting/summary/20260321_ZPT0_500_VzReweightFits_pPb.root"
OFFICIAL_VZ_WEIGHT_FILE_PBP="/home/kdeverea/PhysicsZHadronEEC/Plots/20251001_pPbVZReweighting/summary/20260321_ZPT0_500_VzReweightFits_PbP.root"

OFFICIAL_Z_WEIGHT_FILE_PP="/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20260115_ZCorrection/workflow/output/20260321_ZCorrection_V6_pp_zPt0-500.root"
OFFICIAL_Z_WEIGHT_FILE_PPB="/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20260115_ZCorrection/workflow/output/20260321_ZCorrection_V6_PPb_zPt0-500.root"
OFFICIAL_Z_WEIGHT_FILE_PBP="/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20260115_ZCorrection/workflow/output/20260321_ZCorrection_V6_PbP_zPt0-500.root"

OFFICIAL_R_WEIGHT_FILE_PP="/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20251211_ResidualCorrection/workflow/output/20260321_ZV6_trkV24_TrackResidualCorrection_pp_zPt"
OFFICIAL_R_WEIGHT_FILE_PPB="/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20251211_ResidualCorrection/workflow/output/20260321_ZV6_trkV24_TrackResidualCorrection_PPb_zPt"
OFFICIAL_R_WEIGHT_FILE_PBP="/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20251211_ResidualCorrection/workflow/output/20260321_ZV6_trkV24_TrackResidualCorrection_PbP_zPt"

OFFICIAL_EE_WEIGHT_FILE_PP="/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20260222_EnergyExtrapolation/workflow/output/20260321_EnergyExtrapolation_EEV3.root"

OFFICIAL_MCGENINPUT_PP="/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/mergedSample/pythia-gen-v11-Zpt0.root"
OFFICIAL_MCRECOINPUT_PP="/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/mergedSample/pythia-v11-Zpt0.root"
OFFICIAL_EPOSINPUT_PP=""
OFFICIAL_DATAINPUT_PP="/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/mergedSample/pp-v11-Zpt0.root"

OFFICIAL_MCGENINPUT_PPB="/home/kdeverea/PhysicsZHadronEEC/SampleGeneration/20250929_ReducedTreePA/V0.1/PPbMC_Gen.root"
OFFICIAL_MCRECOINPUT_PPB="/home/kdeverea/PhysicsZHadronEEC/SampleGeneration/20250929_ReducedTreePA/V0.1/PPbMC_Reco.root"
OFFICIAL_EPOSINPUT_PPB="/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/mergedEPOS/PPbMC_Gen.root"
OFFICIAL_DATAINPUT_PPB="/home/kdeverea/PhysicsZHadronEEC/SampleGeneration/20250929_ReducedTreePA/V0.1/PPbData_Reco.root"

OFFICIAL_MCGENINPUT_PBP="/home/kdeverea/PhysicsZHadronEEC/SampleGeneration/20250929_ReducedTreePA/V0.1/PbPMC_Gen.root"
OFFICIAL_MCRECOINPUT_PBP="/home/kdeverea/PhysicsZHadronEEC/SampleGeneration/20250929_ReducedTreePA/V0.1/PbPMC_Reco.root"
OFFICIAL_EPOSINPUT_PBP="/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/mergedEPOS/PbPMC_Gen.root"
OFFICIAL_DATAINPUT_PBP="/home/kdeverea/PhysicsZHadronEEC/SampleGeneration/20250929_ReducedTreePA/V0.1/PbPData_Reco.root"

OFFICIAL_TAG_PP="EEV3_ZV6_trkV24_nmix10"
OFFICIAL_TAG_PPB="ZV6_trkV24_nmix10"

VZWeightFile_PP="${VZ_WEIGHT_FILE_PP:-${OFFICIAL_VZ_WEIGHT_FILE_PP}}"
VZWeightFile_PPb="${VZ_WEIGHT_FILE_PPB:-${OFFICIAL_VZ_WEIGHT_FILE_PPB}}"
VZWeightFile_PbP="${VZ_WEIGHT_FILE_PBP:-${OFFICIAL_VZ_WEIGHT_FILE_PBP}}"

ZWeightFile_PP="${Z_WEIGHT_FILE_PP:-${OFFICIAL_Z_WEIGHT_FILE_PP}}"
ZWeightFile_PPb="${Z_WEIGHT_FILE_PPB:-${OFFICIAL_Z_WEIGHT_FILE_PPB}}"
ZWeightFile_PbP="${Z_WEIGHT_FILE_PBP:-${OFFICIAL_Z_WEIGHT_FILE_PBP}}"

RWeightFile_PP="${R_WEIGHT_FILE_PP:-${OFFICIAL_R_WEIGHT_FILE_PP}}"
RWeightFile_PPb="${R_WEIGHT_FILE_PPB:-${OFFICIAL_R_WEIGHT_FILE_PPB}}"
RWeightFile_PbP="${R_WEIGHT_FILE_PBP:-${OFFICIAL_R_WEIGHT_FILE_PBP}}"

EEWeightFile_PP="${EE_WEIGHT_FILE_PP:-${OFFICIAL_EE_WEIGHT_FILE_PP}}"

export OFFICIAL_VZ_WEIGHT_FILE_PP OFFICIAL_VZ_WEIGHT_FILE_PPB OFFICIAL_VZ_WEIGHT_FILE_PBP
export OFFICIAL_Z_WEIGHT_FILE_PP OFFICIAL_Z_WEIGHT_FILE_PPB OFFICIAL_Z_WEIGHT_FILE_PBP
export OFFICIAL_R_WEIGHT_FILE_PP OFFICIAL_R_WEIGHT_FILE_PPB OFFICIAL_R_WEIGHT_FILE_PBP
export OFFICIAL_EE_WEIGHT_FILE_PP
export OFFICIAL_MCGENINPUT_PP OFFICIAL_MCRECOINPUT_PP OFFICIAL_EPOSINPUT_PP OFFICIAL_DATAINPUT_PP
export OFFICIAL_MCGENINPUT_PPB OFFICIAL_MCRECOINPUT_PPB OFFICIAL_EPOSINPUT_PPB OFFICIAL_DATAINPUT_PPB
export OFFICIAL_MCGENINPUT_PBP OFFICIAL_MCRECOINPUT_PBP OFFICIAL_EPOSINPUT_PBP OFFICIAL_DATAINPUT_PBP
export OFFICIAL_TAG_PP OFFICIAL_TAG_PPB

export VZWeightFile_PP VZWeightFile_PPb VZWeightFile_PbP
export ZWeightFile_PP ZWeightFile_PPb ZWeightFile_PbP
export RWeightFile_PP RWeightFile_PPb RWeightFile_PbP
export EEWeightFile_PP
