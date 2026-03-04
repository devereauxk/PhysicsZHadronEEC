WORKINGDIR=$(pwd)
cd ..
./clean.sh
cd $WORKINGDIR
sleep 1

name="20260223_TrackResidualCorrection_V1"
./OO-DY-analysis.sh 0 10 $name
./OO-DY-analysis.sh 10 20 $name
./OO-DY-analysis.sh 20 40 $name
./OO-DY-analysis.sh 40 500 $name
