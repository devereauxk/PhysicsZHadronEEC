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

# Array to hold the names of the output files
declare -a outputFileNames

echo "Using ${nThread} workers over ${nSlice} slices with executable ${analysisExecutable}"
for (( chunk=1; chunk<=nSlice; chunk++ ))
do
    outputFileName="$1-$2_${chunk}.root"
    outputFileNames+=($outputFileName)
    echo "Starting analysis of slice $chunk / $nSlice"
    echo "$analysisExecutable" $3 $4 $5 $6 $7 $8 $9 ${10} ${11} ${12} ${13} ${14} ${15} ${16} ${17} ${18} ${19} ${20} ${21} ${22} ${23} ${24} ${25} ${26} ${27} ${28} ${29} ${30} ${31} ${32} ${33} ${34} ${35} ${36} ${37} ${38} ${39} ${40} ${41} ${42} ${43} ${44} ${45} ${46} ${47} ${48} ${49} ${50} --nThread $nSlice --nChunk $chunk --Output $outputFileName
    "$analysisExecutable" $3 $4 $5 $6 $7 $8 $9 ${10} ${11} ${12} ${13} ${14} ${15} ${16} ${17} ${18} ${19} ${20} ${21} ${22} ${23} ${24} ${25} ${26} ${27} ${28} ${29} ${30} ${31} ${32} ${33} ${34} ${35} ${36} ${37} ${38} ${39} ${40} ${41} ${42} ${43} ${44} ${45} ${46} ${47} ${48} ${49} ${50} --nThread $nSlice --nChunk $chunk --Output $outputFileName &
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

root -l -q -b "makeProjection.C(\"$1-$2.root\",\"$1-$2-nosub.root\",\"$2\",0)"
root -l -q -b "makeProjection.C(\"$1-$2.root\",\"$1-$2-result.root\",\"$2\", 1)"
