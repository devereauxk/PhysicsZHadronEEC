#!/bin/bash

echo Yay

echo Running job with argument = "$@"

Type=$1
InputFile=$2
RecoOutputFile=$3
GenOutputFile=$4

MinZPT=0
MinTrackPT=0.5
TrackingCorrectionFile=/home/kdeverea/PhysicsZHadronEEC/SampleGeneration/Corrections/Tracking/Hijing_8TeV_dataBS.root

if [[ "$Type" == "PAMC8TeV" ]];
then
   ./Execute --Input "${InputFile}" \
      --Output Output.root \
      --TrackEfficiencyPath $TrackingCorrectionFile \
      --DoGenLevel false \
      --IsData false \
      --Is8TeV true \
      --IsBackground false \
      --CheckZ true \
      --MinZPT $MinZPT \
      --MinTrackPT $MinTrackPT
   mv Output.root "${RecoOutputFile}"

   ./Execute --Input "${InputFile}" \
      --Output Output.root \
      --TrackEfficiencyPath $TrackingCorrectionFile \
      --DoGenLevel true \
      --IsData false \
      --Is8TeV true \
      --IsBackground false \
      --CheckZ true \
      --MinZPT $MinZPT \
      --MinTrackPT $MinTrackPT
   mv Output.root "${GenOutputFile}"

elif [[ "$Type" == "PAData8TeV" ]];
then
   ./Execute --Input "${InputFile}" \
      --Output Output.root \
      --TrackEfficiencyPath $TrackingCorrectionFile \
      --DoGenLevel false \
      --IsData true --RunStart 285852 --RunEnd 286496 \
      --Is8TeV true \
      --IsBackground false \
      --CheckZ true \
      --IgnoreEventWeight true \
      --MinZPT $MinZPT \
      --MinTrackPT $MinTrackPT
   mv Output.root "${RecoOutputFile/.root}_AP.root"
 
   ./Execute --Input "${InputFile}" \
      --Output Output.root \
      --TrackEfficiencyPath $TrackingCorrectionFile \
      --DoGenLevel false \
      --IsData true --RunStart 285479 --RunEnd 285832 \
      --Is8TeV true \
      --IsBackground false \
      --CheckZ true \
      --IgnoreEventWeight true \
      --MinZPT $MinZPT \
      --MinTrackPT $MinTrackPT
   mv Output.root "${RecoOutputFile/.root}_PA.root"
   
fi
