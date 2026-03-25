#!/bin/bash

set -e

echo Running test skim with argument = "$@"

Type=$1
InputFile=$2
RecoOutputFile=$3
GenOutputFile=$4

MinZPT=${MIN_Z_PT:-0}
MinTrackPT=${MIN_TRACK_PT:-0.5}
TrackingCorrectionFile=${TRACKING_CORRECTION_FILE:-/home/kdeverea/PhysicsZHadronEEC/SampleGeneration/Corrections/Tracking/Hijing_8TeV_dataBS.root}
TrackingCorrectionFileLoose=${TRACKING_CORRECTION_FILE_LOOSE:-/home/kdeverea/PhysicsZHadronEEC/SampleGeneration/Corrections/Tracking/Hijing_8TeV_MB_eff_v3_loose.root}
TrackingCorrectionFileTight=${TRACKING_CORRECTION_FILE_TIGHT:-/home/kdeverea/PhysicsZHadronEEC/SampleGeneration/Corrections/Tracking/Hijing_8TeV_MB_eff_v3_tight.root}
WriteAllTrackSelectionTrees=${WRITE_ALL_TRACK_SELECTION_TREES:-true}
SkimmerExecutable=${SKIMMER_TEST_EXECUTABLE:-./ExecuteTest}
FractionArgument=

if [[ -n "${SKIM_FRACTION:-}" ]]; then
   FractionArgument="--Fraction ${SKIM_FRACTION}"
fi

# PbP is the FIRST block of runs in 2016 PbPb, pPb is the SECOND block of runs in 2016 PbPb
GoldenJSONPbP=/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/HI/Cert_285479-285832_HI8TeV_PromptReco_pPb_Collisions16_JSON_NoL1T.txt
GoldenJSONPPb=/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/HI/Cert_285952-286496_HI8TeV_PromptReco_Pbp_Collisions16_JSON_NoL1T.txt


if [[ "$Type" == "PAMC8TeV" ]];
then
   ${SkimmerExecutable} --Input "${InputFile}" \
      --Output "${RecoOutputFile}" \
      --TrackEfficiencyPath "${TrackingCorrectionFile}" \
      --TrackEfficiencyPathLoose "${TrackingCorrectionFileLoose}" \
      --TrackEfficiencyPathTight "${TrackingCorrectionFileTight}" \
      --DoGenLevel false \
      --IsData false \
      --IsPP false \
      --Is8TeV true \
      --IsBackground false \
      --CheckZ true \
      --WriteAllTrackSelectionTrees "${WriteAllTrackSelectionTrees}" \
      --MinZPT $MinZPT \
      --MinTrackPT $MinTrackPT \
      ${FractionArgument}

   ${SkimmerExecutable} --Input "${InputFile}" \
      --Output "${GenOutputFile}" \
      --DoGenLevel true \
      --IsData false \
      --IsPP false \
      --Is8TeV true \
      --IsBackground false \
      --CheckZ true \
      --MinZPT $MinZPT \
      --MinTrackPT $MinTrackPT \
      ${FractionArgument}

elif [[ "$Type" == "PAData8TeV" ]];
then
   # PPb
   ${SkimmerExecutable} --Input "${InputFile}" \
      --Output "${RecoOutputFile/.root}_PA.root" \
      --TrackEfficiencyPath "${TrackingCorrectionFile}" \
      --TrackEfficiencyPathLoose "${TrackingCorrectionFileLoose}" \
      --TrackEfficiencyPathTight "${TrackingCorrectionFileTight}" \
      --DoGenLevel false \
      --IsData true \
      --JSONPath "${GoldenJSONPPb}" \
      --IsPP false \
      --Is8TeV true \
      --IsBackground false \
      --CheckZ true \
      --IgnoreEventWeight true \
      --WriteAllTrackSelectionTrees "${WriteAllTrackSelectionTrees}" \
      --MinZPT $MinZPT \
      --MinTrackPT $MinTrackPT \
      ${FractionArgument}

   # PbP
   ${SkimmerExecutable} --Input "${InputFile}" \
      --Output "${RecoOutputFile/.root}_AP.root" \
      --TrackEfficiencyPath "${TrackingCorrectionFile}" \
      --TrackEfficiencyPathLoose "${TrackingCorrectionFileLoose}" \
      --TrackEfficiencyPathTight "${TrackingCorrectionFileTight}" \
      --DoGenLevel false \
      --IsData true \
      --JSONPath "${GoldenJSONPbP}" \
      --IsPP false \
      --Is8TeV true \
      --IsBackground false \
      --CheckZ true \
      --IgnoreEventWeight true \
      --WriteAllTrackSelectionTrees "${WriteAllTrackSelectionTrees}" \
      --MinZPT $MinZPT \
      --MinTrackPT $MinTrackPT \
      ${FractionArgument}
  
fi
