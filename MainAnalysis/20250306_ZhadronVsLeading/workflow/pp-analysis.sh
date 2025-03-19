./finalAnalysis.sh output/$1pp 4_20   $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11}   --MinZPT 40 --MaxZPT 350 --MinTrackPT 1 --MaxTrackPT 40  --Input mergedSample/pp-v11-Zpt0.root --IsSelfMixing false --IsPP true --IsGenZ false
root -l -q -b "plotBasic.C(\"output/$1pp-4_20.root\",\"plots/$1pp\")"

#./finalAnalysis.sh output/$1pp 1_2   $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11}   --MinZPT 40 --MaxZPT 350 --MinTrackPT 1 --MaxTrackPT 2  --Input mergedSample/pp-v6.root --MixFile mergedSample/pp-v6.root  --IsPP true --IsGenZ false --nMix 20
#./finalAnalysis.sh output/$1pp 2_4   $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11}   --MinZPT 40 --MaxZPT 350 --MinTrackPT 2 --MaxTrackPT 4  --Input mergedSample/pp-v6.root --MixFile mergedSample/pp-v6.root  --IsPP true --IsGenZ false --nMix 20
#./finalAnalysis.sh output/$1pp 4_10  $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11}   --MinZPT 40 --MaxZPT 350 --MinTrackPT 4 --MaxTrackPT 10 --Input mergedSample/pp-v6.root --MixFile mergedSample/pp-v6.root  --IsPP true --IsGenZ false --nMix 20
#hadd -f plots/$1pp-result.root output/$1pp-1_2-result.root output/$1pp-2_4-result.root output/$1pp-4_10-result.root
#hadd -f plots/$1pp-nosub.root output/$1pp-1_2-nosub.root output/$1pp-2_4-nosub.root output/$1pp-4_10-nosub.root
