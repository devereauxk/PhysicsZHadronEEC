WORKINGDIR=$(pwd)
cd ..
./clean.sh
cd $WORKINGDIR
sleep 1

name="${NAME_TAG:-20260321_ZV6_trkV24_TrackResidualCorrection}"
echo "[run-pPb] NAME_TAG=${name}"
./pPb-DY-analysis.sh 0 10 $name
./pPb-DY-analysis.sh 10 20 $name
./pPb-DY-analysis.sh 20 40 $name
./pPb-DY-analysis.sh 40 500 $name
