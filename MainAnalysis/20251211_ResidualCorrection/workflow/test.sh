./finalAnalysis.sh output2023/$1DY RECO  $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} --MinZPT 40 --MaxZPT 200 -MinTrackPT 1 --MaxTrackPT 500  --MinHiBin 0 --MaxHiBin 20  --Input mergedSample/DY-2023-v10.root     --IsGen false --IsPP false --IsGenZ true
./finalAnalysis.sh output2023/$1DY GEN   $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} --MinZPT 40 --MaxZPT 200 -MinTrackPT 1 --MaxTrackPT 500  --MinHiBin 0 --MaxHiBin 20  --Input mergedSample/DY-2023-gen-v10.root --IsGen true --IsPP false --IsGenZ true
root -l -q -b "correction.C+(\"output2023/DY-RECO.root\", \"output2023/DY-GEN.root\");"
