#!/bin/bash
source clean.sh
source config.sh

colors=1181,1179,1179,1183,1180,1180,7,1,97,0,1184,1185,1186,1187,1188
markers=20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20

for zpt_range in "${ZPT_RANGES[@]}"; do

    tags=""
    labels=""
    for pt_range in "${PT_RANGES[@]}"; do

        tags+="Result${pt_range}"
        tags+=","
        labels+="${pt_range/_*/}<p_{T}^{ch}<${pt_range/*_/}@GeV"
        labels+=","

    done

    echo $tags
    echo $labels

    #myfile="plots/pp_ZPT$zpt_range-nosub.root,plots/pythia_ZPT$zpt_range-nosub.root,plots/jewelPP_ZPT$zpt_range-nosub.root,plots/hybridPP_ZPT$zpt_range-nosub.root,plots/pp_ZPT$zpt_range-nosub.root,plots/pythia_ZPT$zpt_range-nosub.root,plots/jewelPP_ZPT$zpt_range-nosub.root,plots/hybridPP_ZPT$zpt_range-nosub.root"
    myfile="plots/pp_nominal_ZPT$zpt_range-result.root,plots/pPb_w1_nominal_ZPT$zpt_range-result.root,plots/pPbMC_nominal_ZPT$zpt_range-result.root,plots/pp_nominal_ZPT$zpt_range-result.root,plots/pPb_w1_nominal_ZPT$zpt_range-result.root,plots/pPbMC_nominal_ZPT$zpt_range-result.root"
    curvelabels="PP,pPb@data,pPbMC@reco" #,"Pythia","Jewel@v2.2.0","Hybrid"

    extraInfoPhi="${zpt_range/_*/}<p_{T}^{Z}<${zpt_range/*_/}@GeV","|y_{Z}|<2.4",""
    extraInfoEta="${zpt_range/_*/}<p_{T}^{Z}<${zpt_range/*_/}@GeV","|y_{Z}|<2.4",""

    echo $extraInfoEta

    ./ExecuteDiff --PlotDiff 0 --ExtraInfo $extraInfoPhi --Markers $markers --Colors $colors --XAxisLabel "#Delta#varphi_{ch,Z}" --YAxisLabel "(1/N_{Z})dN_{ch}/d#Delta #varphi_{ch,Z}" --SolidXMin 0 --SolidXMax 3.1415926 --XMin -1.5758 --XMax 4.7275 --Rebin 1 --ToPlot DeltaPhi --DataFiles $myfile --SkipSystematics true --YMin -1 --YMax 2 --RMin 0 --RMax 10 --OutputBase summary/result-DeltaPhi-PP_ZPT$zpt_range --CurveLabels $curvelabels --lines 0,0,2,2,1,2,1,2 --Tags $tags --Labels $labels

    ./ExecuteDiff --PlotDiff 0 --ExtraInfo $extraInfoEta --Markers $markers --Colors $colors --XAxisLabel "#Deltay_{ch,Z}" --YAxisLabel "(1/N_{Z})dN_{ch}/d#Delta y_{ch,Z}" --SolidXMin 0 --SolidXMax 3.9999 --XMin -3.999 --XMax 3.9999 --Rebin 2 --ToPlot DeltaEta --DataFiles $myfile --SkipSystematics true --YMin -1 --YMax 1 --RMin 0 --RMax 6 --OutputBase summary/result-DeltaEta-PP_ZPT$zpt_range --CurveLabels $curvelabels --lines 0,0,2,2,1,2,1,2 --Tags $tags --Labels $labels

done

