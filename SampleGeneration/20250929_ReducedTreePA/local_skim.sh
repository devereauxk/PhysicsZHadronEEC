#!/bin/bash

set -euo pipefail

echo Yay

echo Running job with argument = "$@"

Type=$1
InputFile=$2
RecoOutputFile=$3
GenOutputFile=$4

MinZPT=0
MinTrackPT=0.5
TrackingCorrectionFile=/home/kdeverea/PhysicsZHadronEEC/SampleGeneration/Corrections/Tracking/Hijing_8TeV_dataBS.root
TrackingCorrectionFileLoose=/home/kdeverea/PhysicsZHadronEEC/SampleGeneration/Corrections/Tracking/Hijing_8TeV_MB_eff_v3_loose.root
TrackingCorrectionFileTight=/home/kdeverea/PhysicsZHadronEEC/SampleGeneration/Corrections/Tracking/Hijing_8TeV_MB_eff_v3_tight.root

TrackingCorrectionFilePP=/home/kdeverea/PhysicsZHadronEEC/SampleGeneration/Corrections/Tracking/20240807_TrackResidualCorrection_V13_pp.root
TrackingCorrectionFilePPLoose=/home/kdeverea/PhysicsZHadronEEC/SampleGeneration/Corrections/Tracking/20240807_TrackResidualCorrection_V13_pp.root
TrackingCorrectionFilePPTight=/home/kdeverea/PhysicsZHadronEEC/SampleGeneration/Corrections/Tracking/20240807_TrackResidualCorrection_V13_pp.root

# PbP is the FIRST block of runs in 2016 PbPb, pPb is the SECOND block of runs in 2016 PbPb
GoldenJSONPbP=/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/HI/Cert_285479-285832_HI8TeV_PromptReco_pPb_Collisions16_JSON_NoL1T.txt
GoldenJSONPPb=/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/HI/Cert_285952-286496_HI8TeV_PromptReco_Pbp_Collisions16_JSON_NoL1T.txt
GoldenJSONPP=/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions17/5TeV/ReReco/Cert_306546-306826_5TeV_EOY2017ReReco_Collisions17_JSON.txt



if [[ "$Type" == "PAMC8TeV" ]];
then
   ./Execute --Input "${InputFile}" \
      --Output "${RecoOutputFile}" \
      --WriteAllTrackSelectionTrees true \
      --TrackEfficiencyPath $TrackingCorrectionFile \
      --TrackEfficiencyPathLoose $TrackingCorrectionFileLoose \
      --TrackEfficiencyPathTight $TrackingCorrectionFileTight \
      --DoGenLevel false \
      --IsData false \
      --IsPP false \
      --Is8TeV true \
      --IsBackground false \
      --CheckZ true \
      --MinZPT $MinZPT \
      --MinTrackPT $MinTrackPT

   ./Execute --Input "${InputFile}" \
      --Output "${GenOutputFile}" \
      --DoGenLevel true \
      --IsData false \
      --IsPP false \
      --Is8TeV true \
      --IsBackground false \
      --CheckZ true \
      --MinZPT $MinZPT \
      --MinTrackPT $MinTrackPT

elif [[ "$Type" == "PAData8TeV" ]];
then
   # PPb
   ./Execute --Input "${InputFile}" \
      --Output "${RecoOutputFile/.root}_PA.root" \
      --WriteAllTrackSelectionTrees true \
      --TrackEfficiencyPath $TrackingCorrectionFile \
      --TrackEfficiencyPathLoose $TrackingCorrectionFileLoose \
      --TrackEfficiencyPathTight $TrackingCorrectionFileTight \
      --DoGenLevel false \
      --IsData true \
      --JSONPath $GoldenJSONPPb \
      --IsPP false \
      --Is8TeV true \
      --IsBackground false \
      --CheckZ true \
      --IgnoreEventWeight true \
      --MinZPT $MinZPT \
      --MinTrackPT $MinTrackPT

   # PbP
   ./Execute --Input "${InputFile}" \
      --Output "${RecoOutputFile/.root}_AP.root" \
      --WriteAllTrackSelectionTrees true \
      --TrackEfficiencyPath $TrackingCorrectionFile \
      --TrackEfficiencyPathLoose $TrackingCorrectionFileLoose \
      --TrackEfficiencyPathTight $TrackingCorrectionFileTight \
      --DoGenLevel false \
      --IsData true \
      --JSONPath $GoldenJSONPbP \
      --IsPP false \
      --Is8TeV true \
      --IsBackground false \
      --CheckZ true \
      --IgnoreEventWeight true \
      --MinZPT $MinZPT \
      --MinTrackPT $MinTrackPT
 
elif [[ "$Type" == "PPData5TeV" ]];
then
   ./Execute --Input "${InputFile}" \
      --Output "${RecoOutputFile}" \
      --WriteAllTrackSelectionTrees true \
      --TrackEfficiencyPath $TrackingCorrectionFilePP \
      --TrackEfficiencyPathLoose $TrackingCorrectionFilePPLoose \
      --TrackEfficiencyPathTight $TrackingCorrectionFilePPTight \
      --DoGenLevel false \
      --IsData true \
      --JSONPath $GoldenJSONPP \
      --IsPP true \
      --IsBackground false \
      --CheckZ true \
      --IgnoreEventWeight true \
      --MinZPT $MinZPT \
      --MinTrackPT $MinTrackPT

fi