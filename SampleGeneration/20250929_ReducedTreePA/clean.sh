export ProjectBase=/home/kdeverea/PhysicsZHadronEEC
export AnalysisBasePA=$ProjectBase/SampleGeneration
THISFOLDER=$(pwd)
rm Execute
rm -rf ../../CommonCode/binary/
rm -rf ../../CommonCode/library/

cd ../../
source SetupAnalysis.sh
cd CommonCode/
make
echo "Cleaning CommonCode"
cd ..
cd ${THISFOLDER}
make Execute
