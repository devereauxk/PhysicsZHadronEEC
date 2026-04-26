#!/bin/bash

# Number of chunks to process in parallel.
# Use NTHREAD to override (default lowered for better stability on segfault-prone samples).
nThread=${NTHREAD:-20}
analysisArgs=("${@:3}")

# Array to hold the names of the output files
declare -a outputFileNames

# Loop over the number of chunks
for (( chunk=1; chunk<=nThread; chunk++ ))
do
    outputFileName="$1-$2_${chunk}.root"
    outputFileNames+=($outputFileName)
    echo "Starting analysis of chunk $chunk"
    echo ./ExecuteCorrelationAnalysis "${analysisArgs[@]}" --nThread $nThread --nChunk $chunk --Output $outputFileName &
    ./ExecuteCorrelationAnalysis "${analysisArgs[@]}" --nThread $nThread --nChunk $chunk --Output $outputFileName &
done

# Wait for all background processes to finish
wait

# Merge the output files into a single file
echo "Merging output files into $1-$2.root"
hadd -f $1-$2.root "${outputFileNames[@]}"
# Remove the individual chunk files
echo "Cleaning up individual chunk files..."
for fileName in "${outputFileNames[@]}"
do
    rm -f $fileName
done
echo "All chunks have been processed and merged into output.root."
