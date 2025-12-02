source config.sh

# combines data files if 0 is an arguement, MC reco if 1, MC gen if 2
# ex: ./combine_pPbPbP.sh 0 1 2

for ZPT_RANGE in "${ZPT_RANGES[@]}"; do
    # data
    if [ "$0" = "0" ] || [ "$1" = "0" ] || [ "$2" = "0" ]; then
        hadd -f plots/pPbPbP_${TAG}_ZPT${ZPT_RANGE}-result.root plots/pPb_${TAG}_ZPT${ZPT_RANGE}-result.root plots/PbP_${TAG}_ZPT${ZPT_RANGE}-result.root
        hadd -f plots/pPbPbP_${TAG}_ZPT${ZPT_RANGE}-nosub.root plots/pPb_${TAG}_ZPT${ZPT_RANGE}-nosub.root plots/PbP_${TAG}_ZPT${ZPT_RANGE}-nosub.root
    fi

    # MC reco
    if [ "$0" = "1" ] || [ "$1" = "1" ] || [ "$2" = "1" ]; then
        hadd -f plots/pPbPbPMC_${TAG}_ZPT${ZPT_RANGE}-result.root plots/pPbMC_${TAG}_ZPT${ZPT_RANGE}-result.root plots/PbPMC_${TAG}_ZPT${ZPT_RANGE}-result.root
        hadd -f plots/pPbPbPMC_${TAG}_ZPT${ZPT_RANGE}-nosub.root plots/pPbMC_${TAG}_ZPT${ZPT_RANGE}-nosub.root plots/PbPMC_${TAG}_ZPT${ZPT_RANGE}-nosub.root
    fi

    # MC gen
    if [ "$0" = "2" ] || [ "$1" = "2" ] || [ "$2" = "2" ]; then
        hadd -f plots/pPbPbPMC_Gen_${TAG}_ZPT${ZPT_RANGE}-result.root plots/pPbMC_Gen_${TAG}_ZPT${ZPT_RANGE}-result.root plots/PbPMC_Gen_${TAG}_ZPT${ZPT_RANGE}-result.root
        hadd -f plots/pPbPbPMC_Gen_${TAG}_ZPT${ZPT_RANGE}-nosub.root plots/pPbMC_Gen_${TAG}_ZPT${ZPT_RANGE}-nosub.root plots/PbPMC_Gen_${TAG}_ZPT${ZPT_RANGE}-nosub.root
    fi
done
