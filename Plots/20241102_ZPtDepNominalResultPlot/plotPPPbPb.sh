#!/bin/bash
source config.sh

colors=1181,7,1179,1183,1180,1180,7,1,97,0,1184,1185,1186,1187,1188
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

    #myfile="plots/PbPb0_30_ZPT$zpt_range-result.root,plots/DY0_30_ZPT$zpt_range-result.root,plots/jewelPbPb030_ZPT$zpt_range-result.root,plots/hybridPbPb030_ZPT$zpt_range-result.root,plots/pp_ZPT$zpt_range-result.root,plots/pythia_ZPT$zpt_range-result.root,plots/jewelPP_ZPT$zpt_range-result.root,plots/hybridPP_ZPT$zpt_range-result.root"
    #curvelabels="PbPb@0-30%","PythiaDY","Jewel@v2.2.0","Hybrid"

    #myfile="plots/pPb_ZPT$zpt_range-result.root,plots/pPbMC_All_ZPT$zpt_range-result.root,plots/pPbMC_AllGen_ZPT$zpt_range-result.root,plots/pPbMC_All_ZPT$zpt_range-result.root,plots/pPbMC_All_ZPT$zpt_range-result.root,plots/pPbMC_All_ZPT$zpt_range-result.root"
    #curvelabels="pPb@Data","PPb+PbP@MC@Reco","PPb+PbP@MC@Gen"
    #scales=1,2,2,2,2,2

    #myfile="plots/1pPb_ZPT$zpt_range-result.root,plots/pPbMC_ZPT$zpt_range-result.root,plots/pPbMC_Gen_ZPT$zpt_range-result.root,plots/pPbMC_ZPT$zpt_range-result.root,plots/pPbMC_ZPT$zpt_range-result.root,plots/pPbMC_ZPT$zpt_range-result.root"
    #curvelabels="pPb@Data","PPb@MC@Reco","PPb@MC@Gen"
    #scales=1,1,1,1,1,1

    myfile="plots/0pPb_ZPT$zpt_range-result.root,plots/PbPMC_ZPT$zpt_range-result.root,plots/PbPMC_Gen_ZPT$zpt_range-result.root,plots/PbPMC_ZPT$zpt_range-result.root,plots/PbPMC_ZPT$zpt_range-result.root,plots/PbPMC_ZPT$zpt_range-result.root"
    curvelabels="PbP@Data","PbP@MC@Reco","PbP@MC@Gen"
    scales=1,1,1,1,1,1
    

    extraInfoPhi="${zpt_range/_*/}<p_{T}^{Z}<${zpt_range/*_/}@GeV","|y_{Z}|<2.4",""
    extraInfoEta="${zpt_range/_*/}<p_{T}^{Z}<${zpt_range/*_/}@GeV","|y_{Z}|<2.4",""

    echo $extraInfoEta

    #"\"PbPb 0-30%\",\"HYBRID No Wake\",\"HYBRID\",\"JEWEL\",PYQUEN,\"JEWEL No Recoil\""
    ./ExecuteDiff --PlotDiff 1 --ExtraInfo $extraInfoPhi --Markers $markers --Colors $colors --XAxisLabel "#Delta#phi_{ch,Z}" --YAxisLabel "d#LT#DeltaN_{ch}#GT/d#Delta#phi_{ch,Z}" --SolidXMin 0 --SolidXMax 3.1415926 --XMin -1.5758 --XMax 4.7275 --Rebin 1 --ToPlot DeltaPhi --DataFiles $myfile --SkipSystematics true --YMin -0.5 --YMax 1.2 --RMin -0.2 --RMax 0.2 --OutputBase summary/result-DeltaPhi-Diff0_30_ZPT$zpt_range-0 --CurveLabels $curvelabels --lines 0,1,1,2,1,2,1,2 --Tags $tags --Labels $labels --RAxisLabel "pPb - MC" --scales $scales

    ./ExecuteDiff --PlotDiff 1 --ExtraInfo $extraInfoEta --Markers $markers --Colors $colors --XAxisLabel "#Deltay_{ch,Z}" --YAxisLabel "d#LT#DeltaN_{ch}#GT/d#Delta y_{ch,Z}" --SolidXMin 0 --SolidXMax 3.9999 --XMin -3.999 --XMax 3.9999 --Rebin 2 --ToPlot DeltaEta --DataFiles $myfile --SkipSystematics true --YMin -0.5 --YMax 0.5 --RMin -0.5 --RMax 0.5 --OutputBase summary/result-DeltaEta-Diff0_30_ZPT$zpt_range-0 --CurveLabels $curvelabels --lines 0,1,1,2,1,2,1,2 --Tags $tags --Labels $labels --scales $scales

done

