./finalAnalysis.sh output/$1hybridPbPb030 4_20  $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11}   --MinZPT 40 --MaxZPT 350 --MinTrackPT 1 --MaxTrackPT 40 --Input mergedSample/hybrid_PbPb030-v8.root --IsSelfMixing false --IsPP false --IsGenZ true --MinHiBin 0 --MaxHiBin 600000
root -l -q -b "plotBasic.C(\"output/$1hybridPbPb030-4_20.root\",\"plots/$1hybridPbPb030\")"
