#!/bin/bash

# Number of concurrent workers.
# Use NTHREAD to override (default lowered for better stability on segfault-prone samples).
nThread=${NTHREAD:-20}
# Over-decompose work to reduce straggler bottlenecks. 1 reproduces legacy slicing.
sliceFactor=${NSLICE_FACTOR:-1}
if [ "$sliceFactor" -lt 1 ]; then
    sliceFactor=1
fi
nSlice=$((nThread * sliceFactor))
analysisExecutable=${ANALYSIS_EXECUTABLE:-./ExecuteCorrelationAnalysis}

resultDEtaBins=20
resultDPhiBins=20
analysisArgs=("${@:3}")
for ((i=0; i<${#analysisArgs[@]}; i++)); do
    case "${analysisArgs[$i]}" in
        --ResultDEtaBins)
            if [ $((i + 1)) -lt ${#analysisArgs[@]} ]; then
                resultDEtaBins="${analysisArgs[$((i + 1))]}"
                i=$((i + 1))
            fi
            ;;
        --ResultDPhiBins)
            if [ $((i + 1)) -lt ${#analysisArgs[@]} ]; then
                resultDPhiBins="${analysisArgs[$((i + 1))]}"
                i=$((i + 1))
            fi
            ;;
    esac
done

# Array to hold the names of the output files
declare -a outputFileNames

echo "Using ${nThread} workers over ${nSlice} slices with executable ${analysisExecutable}"
for (( chunk=1; chunk<=nSlice; chunk++ ))
do
    outputFileName="$1-$2_${chunk}.root"
    outputFileNames+=($outputFileName)
    echo "Starting analysis of slice $chunk / $nSlice"
    echo "$analysisExecutable" "${@:3}" --nThread $nSlice --nChunk $chunk --Output $outputFileName
    "$analysisExecutable" "${@:3}" --nThread $nSlice --nChunk $chunk --Output $outputFileName &
    while [ "$(jobs -rp | wc -l)" -ge "$nThread" ]; do
        wait -n
    done
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
projectionMacro="makeProjection.C"
if [ "$resultDEtaBins" = "10" ] && [ "$resultDPhiBins" = "10" ]; then
    projectionMacro="makeProjectionShifted10x10.C"
elif [ "$resultDEtaBins" = "12" ] && [ "$resultDPhiBins" = "12" ]; then
    projectionMacro="makeProjectionModified12x12.C"
elif [ "$resultDEtaBins" != "20" ] || [ "$resultDPhiBins" != "20" ]; then
    echo "Unsupported result binning: ResultDEtaBins=$resultDEtaBins ResultDPhiBins=$resultDPhiBins" >&2
    exit 1
fi

root -l -q -b "${projectionMacro}(\"$1-$2.root\",\"$1-$2-nosub.root\",\"$2\",0)"
root -l -q -b "${projectionMacro}(\"$1-$2.root\",\"$1-$2-result.root\",\"$2\",1)"
