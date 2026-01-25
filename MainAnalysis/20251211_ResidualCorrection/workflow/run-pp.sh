WORKINGDIR=$(pwd)
cd ..
./clean.sh
cd $WORKINGDIR
sleep 1

name="20260123_TrackResidualCorrection_V21_ZWeight_V3"
./pythia-analysis.sh 0 10 $name
./pythia-analysis.sh 10 20 $name
./pythia-analysis.sh 20 40 $name
./pythia-analysis.sh 40 500 $name