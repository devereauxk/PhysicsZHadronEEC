#!/bin/bash

# Official promoted correction-stack weights (20260321).
# Scripts should source this file instead of hardcoding weight paths.
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

export VZWeightFile_PP VZWeightFile_PPb VZWeightFile_PbP
export ZWeightFile_PP ZWeightFile_PPb ZWeightFile_PbP
export RWeightFile_PP RWeightFile_PPb RWeightFile_PbP
export EEWeightFile_PP
