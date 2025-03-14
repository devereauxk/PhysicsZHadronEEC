./finalAnalysis.sh output/$1hybridPP 4_20  $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11}   --MinZPT 40 --MaxZPT 350 --MinTrackPT 4 --MaxTrackPT 20 --Input mergedSample/hybrid_pp-v8.root --IsSelfMixing false  --IsPP false --IsGenZ true  --MinHiBin 0 --MaxHiBin 600000
root -l -q -b "plotBasic.C(\"output/$1hybridPP-4_20.root\",\"plots/$1hybridPP\")"

./finalAnalysis.sh output/$1hybridPbPb030 4_20  $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11}   --MinZPT 40 --MaxZPT 350 --MinTrackPT 4 --MaxTrackPT 20 --Input mergedSample/hybrid_PbPb030-v8.root --IsSelfMixing false --IsPP false --IsGenZ true --MinHiBin 0 --MaxHiBin 600000
root -l -q -b "plotBasic.C(\"output/$1hybridPbPb030-4_20.root\",\"plots/$1hybridPbPb030\")"
