# pp
input="/home/kdeverea/zdata/pp-v11-Zpt0.root"
output="./output_pp.root"
./ExecuteCorrelationAnalysis --Input $input --Output $output --IsPP true --IsSelfMixing false
root -q "plotResult.C(\"$output\",\"pp\")"

# note: Hibin/2 = centrality, here we set all centrality to 0-30%
# PbPb
input="/home/kdeverea/zdata/HISingleMuon-v11-Zpt0.root"
output="./output_PbPb.root"
./ExecuteCorrelationAnalysis --Input $input --Output $output --IsSelfMixing false --MinHiBin 0 --MaxHiBin 60
root -q "plotResult.C(\"$output\",\"PbPb\")"

# Pythia(Drell-Yan) : pp, Pythia(Drell-Yan)+HYDJET : PbPb
input="/home/kdeverea/zdata/pythia-gen-v11-Zpt0.root"
output="./output_pythia.root"
./ExecuteCorrelationAnalysis --Input $input --Output $output --IsPP true --IsSelfMixing false --IsGenZ true
root -q "plotResult.C(\"$output\",\"pythia\")"
input="/home/kdeverea/zdata/DY-gen-v11-Zpt0.root"
output="./output_DY_HYDJET.root"
./ExecuteCorrelationAnalysis --Input $input --Output $output --IsSelfMixing false --MinHiBin 0 --MaxHiBin 60
root -q "plotResult.C(\"$output\",\"DY_hydjet\")"

# Jewel
# already binned in centrality
input="/home/kdeverea/zdata/jewel_pp-v9.root"
output="./output_jewel_pp.root"
./ExecuteCorrelationAnalysis --Input $input --Output $output --IsPP false --IsSelfMixing false --isJewel true --IsGenZ true --MaxHiBin 40000
root -q "plotResult.C(\"$output\",\"jewel_pp\")"

input="/home/kdeverea/zdata/jewel_PbPb030-v9.root"
output="./output_jewel_PbPb.root"
./ExecuteCorrelationAnalysis --Input $input --Output $output --IsPP false --IsSelfMixing false --isJewel true --IsGenZ true --MaxHiBin 40000
root -q "plotResult.C(\"$output\",\"jewel_PbPb\")"


# HYBRID
input="/home/kdeverea/zdata/hybrid_pp-v9.root"
output="./output_hybrid_pp.root"
./ExecuteCorrelationAnalysis --Input $input --Output $output --IsPP false --IsSelfMixing false --IsGenZ true --MaxHiBin 40000
root -q "plotResult.C(\"$output\",\"hybrid_pp\")"

input="/home/kdeverea/zdata/hybrid_PbPb030-v9.root"
output="./output_hybrid_PbPb.root"
./ExecuteCorrelationAnalysis --Input $input --Output $output --IsPP false --IsSelfMixing false --IsGenZ true --MaxHiBin 40000
root -q "plotResult.C(\"$output\",\"hybrid_PbPb\")"