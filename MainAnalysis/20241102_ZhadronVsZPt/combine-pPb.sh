source config.sh
for zpt_range in "${ZPT_RANGES[@]}"; do
   min_zpt=${zpt_range/_*/}
   max_zpt=${zpt_range/*_/}

    hadd -f "plots/$1pPbMC_All_ZPT${min_zpt}_${max_zpt}-result.root" "plots/$1pPbMC_ZPT${min_zpt}_${max_zpt}-result.root" "plots/$1PbPMC_ZPT${min_zpt}_${max_zpt}-result.root"
    hadd -f "plots/$1pPbMC_All_ZPT${min_zpt}_${max_zpt}-nosub.root" "plots/$1pPbMC_ZPT${min_zpt}_${max_zpt}-nosub.root" "plots/$1PbPMC_ZPT${min_zpt}_${max_zpt}-nosub.root" 

    hadd -f "plots/$1pPbMC_AllGen_ZPT${min_zpt}_${max_zpt}-result.root" "plots/$1pPbMC_Gen_ZPT${min_zpt}_${max_zpt}-result.root" "plots/$1PbPMC_Gen_ZPT${min_zpt}_${max_zpt}-result.root"
    hadd -f "plots/$1pPbMC_AllGen_ZPT${min_zpt}_${max_zpt}-nosub.root" "plots/$1pPbMC_Gen_ZPT${min_zpt}_${max_zpt}-nosub.root" "plots/$1PbPMC_Gen_ZPT${min_zpt}_${max_zpt}-nosub.root"

done