#!/bin/bash
source config.sh

colors=1181,1179,1179,1183,1180,1180,7,1,97,0,1184,1185,1186,1187,1188
markers=20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20

for zpt_range in "${ZPT_RANGES[@]}"; do

    tags=""
    labels=""
    for pt_range in "${PT_RANGES[@]}"; do

        tags+="Result${pt_range}"
        tags+=","
        labels+="${pt_range/_*/}<p_{T}^{lead}<${pt_range/*_/}@GeV"
        labels+=","

    done

    echo $tags
    echo $labels

    myfile="plots/pp_ZPT$zpt_range-result.root,plots/pythia_ZPT$zpt_range-result.root,plots/jewelPP_ZPT$zpt_range-result.root,plots/hybridPP_ZPT$zpt_range-result.root,plots/pp_ZPT$zpt_range-result.root,plots/pythia_ZPT$zpt_range-result.root,plots/jewelPP_ZPT$zpt_range-result.root,plots/hybridPP_ZPT$zpt_range-result.root"
    curvelabels="PP","Pythia","Jewel@v2.2.0","Hybrid"

    extraInfoPhi="${zpt_range/_*/}<p_{T}^{Z}<${zpt_range/*_/}@GeV","|y_{Z}|<2.4",""
    extraInfoEta="${zpt_range/_*/}<p_{T}^{Z}<${zpt_range/*_/}@GeV","|y_{Z}|<2.4",""

    echo $extraInfoEta

    #"\"PbPb 0-30%\",\"HYBRID No Wake\",\"HYBRID\",\"JEWEL\",PYQUEN,\"JEWEL No Recoil\""
    ./ExecuteDiff --PlotDiff 0 --ExtraInfo $extraInfoPhi --Markers $markers --Colors $colors --XAxisLabel "#Delta#phi_{ch,Z}" --YAxisLabel "d#LT#DeltaN_{ch}#GT/d#Delta#phi_{ch,Z}" --SolidXMin 0 --SolidXMax 3.1415926 --XMin -1.5758 --XMax 4.7275 --Rebin 1 --ToPlot DeltaPhi --DataFiles $myfile --SkipSystematics true --YMin -1.5 --YMax 3 --RMin -2.99 --RMax 2.99 --OutputBase summary/result-DeltaPhi-PP_ZPT$zpt_range --CurveLabels $curvelabels --lines 0,1,2,2,1,2,1,2 --Tags $tags --Labels $labels

    ./ExecuteDiff --PlotDiff 0 --ExtraInfo $extraInfoEta --Markers $markers --Colors $colors --XAxisLabel "#Deltay_{ch,Z}" --YAxisLabel "d#LT#DeltaN_{ch}#GT/d#Delta y_{ch,Z}" --SolidXMin 0 --SolidXMax 3.9999 --XMin -3.999 --XMax 3.9999 --Rebin 2 --ToPlot DeltaEta --DataFiles $myfile --SkipSystematics true --YMin -1.45 --YMax 1.45 --RMin -1.65 --RMax 1.65 --OutputBase summary/result-DeltaEta-PP_ZPT$zpt_range --CurveLabels $curvelabels --lines 0,1,2,2,1,2,1,2 --Tags $tags --Labels $labels

done

