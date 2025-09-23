source config.sh

today=$(date +%Y%m%d)

for zpt_range in "${ZPT_RANGES[@]}"; do
    min_zpt=${zpt_range/_*/}
    max_zpt=${zpt_range/*_/}

    # pp plot
    root -l -q -b "plot_single.C(\"/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/pp_ZPT${min_zpt}_${max_zpt}-result.root\", \"plots/${today}_pp_ZPT${min_zpt}_${max_zpt}\", \"pp\")"

    # pPb plot
    root -l -q -b "plot_single.C(\"/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/plots/1pPb_ZPT${min_zpt}_${max_zpt}-result.root\", \"plots/${today}_pPb_ZPT${min_zpt}_${max_zpt}\", \"pPb\")"

done