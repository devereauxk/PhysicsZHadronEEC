source config.sh

today=$(date +%Y%m%d)

for zpt_range in "${ZPT_RANGES[@]}"; do
    min_zpt=${zpt_range/_*/}
    max_zpt=${zpt_range/*_/}

    # pp plot
    root -l -q -b "plot_single.C(\"./plots/pp_ZPT${min_zpt}_${max_zpt}-result.root\", \"summary/${today}_pp_ZPT${min_zpt}_${max_zpt}\", \"pp\")"

    # pPb plot
    root -l -q -b "plot_single.C(\"./plots/1pPb_ZPT${min_zpt}_${max_zpt}-result.root\", \"summary/${today}_pPb_ZPT${min_zpt}_${max_zpt}\", \"pPb\")"

done