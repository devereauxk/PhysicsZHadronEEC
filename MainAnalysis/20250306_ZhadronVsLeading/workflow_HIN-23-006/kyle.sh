# pp
input="/home/kdeverea/zdata/pp-v11-Zpt0.root"
output="./output_pp.root"
./ExecuteCorrelationAnalysis --Input $input --Output $output --IsPP true --IsSelfMixing false
root -q "plotResult.C(\"$output\",\"pp\")"

# PbPb
input="/home/kdeverea/zdata/HISingleMuon-v11-Zpt0.root"
output="./output_PbPb.root"
./ExecuteCorrelationAnalysis --Input $input --Output $output --IsSelfMixing false
root -q "plotResult.C(\"$output\",\"PbPb\")"

# Pythia(Drell-Yan) : pp, Pythia(Drell-Yan)+HYDJET : PbPb
input="/home/kdeverea/zdata/pythia-gen-v11-Zpt0.root"
output="./output_pythia_gen.root"
./ExecuteCorrelationAnalysis --Input $input --Output $output --IsPP true --IsSelfMixing false --IsGenZ true
root -q "plotResult.C(\"$output\",\"pythia\")"
input="/home/kdeverea/zdata/DY-gen-v11-Zpt0.root"
output="./output_DY_HYDJET.root"
./ExecuteCorrelationAnalysis --Input $input --Output $output --IsSelfMixing false
root -q "plotResult.C(\"$output\",\"DY_hydjet\")"

# Jewel
input="/home/kdeverea/zdata/jewel_pp-v9.root"
output="./output_jewel_pp.root"
./ExecuteCorrelationAnalysis --Input $input --Output $output --IsSelfMixing false --isJewel true --MaxHiBin 40000 --IsGenZ true
root -q "plotResult.C(\"$output\",\"jewel_pp\")"

input="/home/kdeverea/zdata/jewel_PbPb030-v9.root"
output="./output_jewel.root"
./ExecuteCorrelationAnalysis --Input $input --Output $output --IsSelfMixing false --isJewel true --MaxHiBin 40000 --IsGenZ true
root -q "plotResult.C(\"$output\",\"jewel_PbPb\")"
