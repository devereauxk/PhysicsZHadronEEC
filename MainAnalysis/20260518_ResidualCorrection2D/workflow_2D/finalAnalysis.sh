#!/bin/bash

nThread=${NTHREAD:-20}
analysisArgs=("${@:3}")

declare -a outputFileNames

for (( chunk=1; chunk<=nThread; chunk++ ))
do
    outputFileName="$1-$2_${chunk}.root"
    outputFileNames+=($outputFileName)
    echo "Starting analysis of chunk $chunk"
    echo ../ExecuteCorrelationAnalysis "${analysisArgs[@]}" --nThread $nThread --nChunk $chunk --Output $outputFileName &
    ../ExecuteCorrelationAnalysis "${analysisArgs[@]}" --nThread $nThread --nChunk $chunk --Output $outputFileName &
done

wait

echo "Merging output files into $1-$2.root"
hadd -f $1-$2.root "${outputFileNames[@]}"
echo "Cleaning up individual chunk files..."
for fileName in "${outputFileNames[@]}"
do
    rm -f $fileName
done
echo "All chunks have been processed and merged into output.root."
