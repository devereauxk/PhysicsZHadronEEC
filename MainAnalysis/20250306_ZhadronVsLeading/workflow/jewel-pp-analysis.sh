./finalAnalysis.sh output/$1jewelPP 4_20  $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11}   --IsJewel true --MinZPT 20 --MaxZPT 60 --MinTrackPT 1 --MaxTrackPT 40 --Input mergedSample/jewel_pp-v9.root --IsSelfMixing false  --IsPP true --IsGenZ true --MinHiBin 0 --MaxHiBin 600000  
root -l -q -b "plotBasic.C(\"output/$1jewelPP-4_20.root\",\"plots/$1jewelPP\")"
