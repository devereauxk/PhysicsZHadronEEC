WORKINGDIR=$(pwd)
cd ..
./clean.sh
cd $WORKINGDIR
sleep 1

name="20260223_TrackResidualCorrection_V22_ZWeight_V4"
./pythia-analysis.sh 0 10 $name
./pythia-analysis.sh 10 20 $name
./pythia-analysis.sh 20 40 $name
./pythia-analysis.sh 40 500 $name