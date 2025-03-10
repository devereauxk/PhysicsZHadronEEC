# pp
input="/home/kdeverea/zdata/pp-v11-Zpt0.root"
output="./output_pp.root"
./ExecuteCorrelationAnalysis --Input $input --Output $output --IsPP true --IsSelfMixing false

# PbPb
input="/home/kdeverea/zdata/HISingleMuon-v11-Zpt0.root"
output="./output_PbPb.root"
./ExecuteCorrelationAnalysis --Input $input --Output $output --IsSelfMixing false

# Pythia(Drell-Yan) : pp, Pythia(Drell-Yan)+HYDJET : PbPb
input="/home/kdeverea/zdata/pythia-gen-v11-Zpt0.root"
output="./output_pythia.root"
./ExecuteCorrelationAnalysis --Input $input --Output $output --IsPP true --IsSelfMixing false
input="/home/kdeverea/zdata/DY-gen-v11-Zpt0.root"
output="./output_DY_HYDJET.root"
./ExecuteCorrelationAnalysis --Input $input --Output $output --IsSelfMixing false

# Jewel
input="/home/kdeverea/zdata/jewel_PbPb030-v9.root"
output="./output_jewel.root"
./ExecuteCorrelationAnalysis --Input $input --Output $output --IsSelfMixing false
