#!/bin/bash

# Number of threads per instance
nThread=35 # NOT MORE THAT 50 (week spent), even 40 segfaults sometimes

# Array to hold the names of the output files
declare -a outputFileNames

outputPrefix=$1
ptRange=$2
shift 2
analysisArgs=("$@")

# Loop over the number of chunks
for (( chunk=1; chunk<=nThread; chunk++ ))
do
    outputFileName="$outputPrefix-$ptRange"_"${chunk}.root"
    outputFileNames+=($outputFileName)
    echo "Starting analysis of chunk $chunk"
    echo ./ExecuteCorrelationAnalysis "${analysisArgs[@]}" --nThread "$nThread" --nChunk "$chunk" --Output "$outputFileName"
    ./ExecuteCorrelationAnalysis "${analysisArgs[@]}" --nThread "$nThread" --nChunk "$chunk" --Output "$outputFileName" &
done

# Wait for all background processes to finish
wait

# Merge the output files into a single file
echo "Merging output files into $outputPrefix-$ptRange.root"
hadd -f "$outputPrefix-$ptRange.root" "${outputFileNames[@]}"
# Remove the individual chunk files
echo "Cleaning up individual chunk files..."
for fileName in "${outputFileNames[@]}"
do
    rm -f $fileName
done
echo "All chunks have been processed and merged into output.root."

#root -l -q -b "makeProjection.C(\"$1-$2.root\",\"$1-$2-result.root\",\"$2\", 1)"
#root -l -q -b "makeProjection.C(\"$1-$2.root\",\"$1-$2-nosub.root\",\"$2\",0)"
