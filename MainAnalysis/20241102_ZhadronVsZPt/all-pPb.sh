make -C ..
./pPb-analysis.sh 0
./pPbMC-analysis.sh
./pPbMC_Gen-analysis.sh
./PbPMC-analysis.sh
./PbPMC_Gen-analysis.sh

./combine-pPb.sh
