WORKINGDIR=$(pwd)
cd ..
./clean.sh
cd $WORKINGDIR
sleep 1

name="20260126_TrackResidualCorrection_V22_ZWeight_V4"
./pPb-DY-analysis.sh 0 10 $name
./pPb-DY-analysis.sh 10 20 $name
./pPb-DY-analysis.sh 20 40 $name
./pPb-DY-analysis.sh 40 500 $name
