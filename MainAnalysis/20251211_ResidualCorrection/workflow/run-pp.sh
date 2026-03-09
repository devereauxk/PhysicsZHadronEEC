WORKINGDIR=$(pwd)
cd ..
./clean.sh
cd $WORKINGDIR
sleep 1

name="20260308_TrackResidualCorrection_V23_ZWeight_V5"
./pythia-analysis.sh 0 10 $name
./pythia-analysis.sh 10 20 $name
./pythia-analysis.sh 20 40 $name
./pythia-analysis.sh 40 500 $name