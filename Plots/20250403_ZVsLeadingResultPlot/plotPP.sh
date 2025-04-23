#!/bin/bash
source ../../MainAnalysis/20250306_ZhadronVsLeading/workflow/config.sh

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

    myfile="plots/pp_ZPT$zpt_range-nosub.root,plots/pythia_ZPT$zpt_range-nosub.root,plots/jewelPP_ZPT$zpt_range-nosub.root,plots/hybridPP_ZPT$zpt_range-nosub.root,plots/pp_ZPT$zpt_range-nosub.root,plots/pythia_ZPT$zpt_range-nosub.root,plots/jewelPP_ZPT$zpt_range-nosub.root,plots/hybridPP_ZPT$zpt_range-nosub.root"
    curvelabels="PP","Pythia","Jewel@v2.2.0","Hybrid"

    extraInfoPhi="${zpt_range/_*/}<p_{T}^{Z}<${zpt_range/*_/}@GeV","|y_{Z}|<2.4",""
    extraInfoEta="${zpt_range/_*/}<p_{T}^{Z}<${zpt_range/*_/}@GeV","|y_{Z}|<2.4",""

    echo $extraInfoEta

    #"\"PbPb 0-30%\",\"HYBRID No Wake\",\"HYBRID\",\"JEWEL\",PYQUEN,\"JEWEL No Recoil\""
    ./ExecuteDiff --PlotDiff 0 --ExtraInfo $extraInfoPhi --Markers $markers --Colors $colors --XAxisLabel "#Delta#phi_{lead,Z}" --YAxisLabel "(1/N_{Z})dN_{lead}/d#phi_{lead,Z}" --SolidXMin 0 --SolidXMax 3.1415926 --XMin -1.5758 --XMax 4.7275 --Rebin 1 --ToPlot DeltaPhi --DataFiles $myfile --SkipSystematics true --YMin 0 --YMax 1 --RMin 0 --RMax 10 --OutputBase summary/nosub-DeltaPhi-PP_ZPT$zpt_range --CurveLabels $curvelabels --lines 0,1,2,2,1,2,1,2 --Tags $tags --Labels $labels

    ./ExecuteDiff --PlotDiff 0 --ExtraInfo $extraInfoEta --Markers $markers --Colors $colors --XAxisLabel "#Deltay_{lead,Z}" --YAxisLabel "(1/N_{Z})dN_{lead}/d#Delta y_{lead,Z}" --SolidXMin 0 --SolidXMax 3.9999 --XMin -3.999 --XMax 3.9999 --Rebin 1 --ToPlot DeltaEta --DataFiles $myfile --SkipSystematics true --YMin 0 --YMax 0.9 --RMin 0 --RMax 6 --OutputBase summary/nosub-DeltaEta-PP_ZPT$zpt_range --CurveLabels $curvelabels --lines 0,1,2,2,1,2,1,2 --Tags $tags --Labels $labels

done

