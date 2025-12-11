./finalAnalysis.sh output/$1DY RECO  $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} --MinZPT 40 --MaxZPT 200 -MinTrackPT 1 --MaxTrackPT 500  --MinHiBin 0 --MaxHiBin 20  --Input mergedSample/DY-v7.root     --IsGen false --IsPP false --IsGenZ true
./finalAnalysis.sh output/$1DY GEN   $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} --MinZPT 40 --MaxZPT 200 -MinTrackPT 1 --MaxTrackPT 500  --MinHiBin 0 --MaxHiBin 20  --Input mergedSample/DY-gen-v7.root --IsGen true --IsPP false --IsGenZ true
root -l -q -b correction.C
mv correction.root output/correction_1.root
./finalAnalysis.sh output/$1DY RECO  $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} --MinZPT 40 --MaxZPT 200 -MinTrackPT 1 --MaxTrackPT 500  --MinHiBin 0 --MaxHiBin 20  --Input mergedSample/DY-v7.root     --residualFile output/correction_1.root --IsGen false --IsPP false --IsGenZ true
root -l -q -b correction.C
mv correction.root output/correction_2.root
root -l -q -b "merge_corrections.C(\"output/correction_1.root\",\"output/correction_2.root\",\"output/correction_12.root\")"
./finalAnalysis.sh output/$1DY RECO  $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} --MinZPT 40 --MaxZPT 200 -MinTrackPT 1 --MaxTrackPT 500  --MinHiBin 0 --MaxHiBin 20  --Input mergedSample/DY-v7.root     --residualFile output/correction_12.root --IsGen false --IsPP false --IsGenZ true
root -l -q -b correction.C
mv correction.root output/correction_3.root

root -l -q -b plot_corrections.C
mv corrections.pdf corrections_0_20.pdf
root -l -q -b "merge_corrections.C(\"output/correction_12.root\",\"output/correction_3.root\",\"output/20240807_TrackResidualCorrection_V13_0_20.root\")"


#################### 20-60
./finalAnalysis.sh output/$1DY RECO  $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} --MinZPT 40 --MaxZPT 200 -MinTrackPT 1 --MaxTrackPT 500  --MinHiBin 20 --MaxHiBin 60  --Input mergedSample/DY-v7.root     --IsGen false --IsPP false --IsGenZ true
./finalAnalysis.sh output/$1DY GEN   $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} --MinZPT 40 --MaxZPT 200 -MinTrackPT 1 --MaxTrackPT 500  --MinHiBin 20 --MaxHiBin 60  --Input mergedSample/DY-gen-v7.root --IsGen true --IsPP false --IsGenZ true
root -l -q -b correction.C
mv correction.root output/correction_1.root
./finalAnalysis.sh output/$1DY RECO  $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} --MinZPT 40 --MaxZPT 200 -MinTrackPT 1 --MaxTrackPT 500  --MinHiBin 20 --MaxHiBin 60  --Input mergedSample/DY-v7.root     --residualFile output/correction_1.root --IsGen false --IsPP false --IsGenZ true
root -l -q -b correction.C
mv correction.root output/correction_2.root
root -l -q -b "merge_corrections.C(\"output/correction_1.root\",\"output/correction_2.root\",\"output/correction_12.root\")"
./finalAnalysis.sh output/$1DY RECO  $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} --MinZPT 40 --MaxZPT 200 -MinTrackPT 1 --MaxTrackPT 500  --MinHiBin 20 --MaxHiBin 60  --Input mergedSample/DY-v7.root     --residualFile output/correction_12.root --IsGen false --IsPP false --IsGenZ true
root -l -q -b correction.C
mv correction.root output/correction_3.root

root -l -q -b plot_corrections.C
mv corrections.pdf corrections_20_60.pdf
root -l -q -b "merge_corrections.C(\"output/correction_12.root\",\"output/correction_3.root\",\"output/20240807_TrackResidualCorrection_V13_20_60.root\")"

#################### 60-100
./finalAnalysis.sh output/$1DY RECO  $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} --MinZPT 40 --MaxZPT 200 -MinTrackPT 1 --MaxTrackPT 500  --MinHiBin 60 --MaxHiBin 100  --Input mergedSample/DY-v7.root     --IsGen false --IsPP false --IsGenZ true
./finalAnalysis.sh output/$1DY GEN   $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} --MinZPT 40 --MaxZPT 200 -MinTrackPT 1 --MaxTrackPT 500  --MinHiBin 60 --MaxHiBin 100  --Input mergedSample/DY-gen-v7.root --IsGen true --IsPP false --IsGenZ true
root -l -q -b correction.C
mv correction.root output/correction_1.root
./finalAnalysis.sh output/$1DY RECO  $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} --MinZPT 40 --MaxZPT 200 -MinTrackPT 1 --MaxTrackPT 500  --MinHiBin 60 --MaxHiBin 100  --Input mergedSample/DY-v7.root     --residualFile output/correction_1.root --IsGen false --IsPP false --IsGenZ true
root -l -q -b correction.C
mv correction.root output/correction_2.root
root -l -q -b "merge_corrections.C(\"output/correction_1.root\",\"output/correction_2.root\",\"output/correction_12.root\")"
./finalAnalysis.sh output/$1DY RECO  $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} --MinZPT 40 --MaxZPT 200 -MinTrackPT 1 --MaxTrackPT 500  --MinHiBin 60 --MaxHiBin 100  --Input mergedSample/DY-v7.root     --residualFile output/correction_12.root --IsGen false --IsPP false --IsGenZ true
root -l -q -b correction.C
mv correction.root output/correction_3.root

root -l -q -b plot_corrections.C
mv corrections.pdf corrections_60_100.pdf
root -l -q -b "merge_corrections.C(\"output/correction_12.root\",\"output/correction_3.root\",\"output/20240807_TrackResidualCorrection_V13_60_100.root\")"


#################### 100-200
./finalAnalysis.sh output/$1DY RECO  $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} --MinZPT 40 --MaxZPT 200 -MinTrackPT 1 --MaxTrackPT 500  --MinHiBin 100 --MaxHiBin 200  --Input mergedSample/DY-v7.root     --IsGen false --IsPP false --IsGenZ true
./finalAnalysis.sh output/$1DY GEN   $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} --MinZPT 40 --MaxZPT 200 -MinTrackPT 1 --MaxTrackPT 500  --MinHiBin 100 --MaxHiBin 200  --Input mergedSample/DY-gen-v7.root --IsGen true --IsPP false --IsGenZ true
root -l -q -b correction.C
mv correction.root output/correction_1.root
./finalAnalysis.sh output/$1DY RECO  $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} --MinZPT 40 --MaxZPT 200 -MinTrackPT 1 --MaxTrackPT 500  --MinHiBin 100 --MaxHiBin 200  --Input mergedSample/DY-v7.root     --residualFile output/correction_1.root --IsGen false --IsPP false --IsGenZ true
root -l -q -b correction.C
mv correction.root output/correction_2.root
root -l -q -b "merge_corrections.C(\"output/correction_1.root\",\"output/correction_2.root\",\"output/correction_12.root\")"
./finalAnalysis.sh output/$1DY RECO  $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} --MinZPT 40 --MaxZPT 200 -MinTrackPT 1 --MaxTrackPT 500  --MinHiBin 100 --MaxHiBin 200  --Input mergedSample/DY-v7.root     --residualFile output/correction_12.root --IsGen false --IsPP false --IsGenZ true
root -l -q -b correction.C
mv correction.root output/correction_3.root

root -l -q -b plot_corrections.C
mv corrections.pdf corrections_100_200.pdf
root -l -q -b "merge_corrections.C(\"output/correction_12.root\",\"output/correction_3.root\",\"output/20240807_TrackResidualCorrection_V13_100_200.root\")"

