WORKINGDIR=$(pwd)
cd ..
./clean.sh
cd $WORKINGDIR
sleep 1

name="20260117_TrackResidualCorrection_V20"
./pPb-DY-analysis.sh 0 10 $name
./pPb-DY-analysis.sh 10 20 $name
./pPb-DY-analysis.sh 20 40 $name
./pPb-DY-analysis.sh 40 500 $name