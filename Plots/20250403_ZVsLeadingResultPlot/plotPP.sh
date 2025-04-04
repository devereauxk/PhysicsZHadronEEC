#!/bin/bash
zpt_range="40_350"
myfile="plots/pp_ZPT$zpt_range-nosub.root,plots/pythia_ZPT$zpt_range-nosub.root,plots/jewelPP_ZPT$zpt_range-nosub.root,plots/hybridPP_ZPT$zpt_range-nosub.root,plots/pp_ZPT$zpt_range-nosub.root,plots/pythia_ZPT$zpt_range-nosub.root,plots/jewelPP_ZPT$zpt_range-nosub.root,plots/hybridPP_ZPT$zpt_range-nosub.root"
curvelabels="pp","Pythia","Hybrid","Jewel@v2.2.0"
tags="Result3_6","Result6_10","Result10_20"
labels="3<p_{T}^{lead}<6@GeV","6<p_{T}^{lead}<10@GeV","10<p_{T}^{lead}<20@GeV"

colors=1181,1180,1179,1182,15,1183,97,0,1184,1185,1186,1187,1188
markers=20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20
extraInfoPhi="40<p_{T}^{Z}<350@GeV","|y_{Z}|<2.4",""
extraInfoEta="40<p_{T}^{Z}<350@GeV","|y_{Z}|<2.4",""

#"\"PbPb 0-30%\",\"HYBRID No Wake\",\"HYBRID\",\"JEWEL\",PYQUEN,\"JEWEL No Recoil\""
./ExecuteDiff --PlotDiff 0 --ExtraInfo $extraInfoPhi --LegendBottom 0.7 --Markers $markers --Colors $colors --XAxisLabel "#Delta#phi_{lead,Z}" --YAxisLabel "(1/N_{Z})dN_{lead}/d#Delta#phi_{lead,Z}" --SolidXMin 0 --SolidXMax 3.1415926 --XMin -1.5758 --XMax 4.7275 --Rebin 1 --ToPlot DeltaPhi --DataFiles $myfile --SkipSystematics true --YMin -0.25 --YMax 1.5 --RMin -2.99 --RMax 2.99 --OutputBase summary/nosub-DeltaPhi-pp --CurveLabels $curvelabels --lines 0,1,2,2,1,2,1,2 --Tags $tags --Lables $labels

./ExecuteDiff --PlotDiff 0 --ExtraInfo $extraInfoEta --LegendBottom 0.7 --Markers $markers --Colors $colors --XAxisLabel "#Deltay_{lead,Z}" --YAxisLabel "(1/N_{Z})dN_{lead}/d#Delta y_{lead,Z}" --SolidXMin 0 --SolidXMax 3.9999 --XMin -3.999 --XMax 3.9999 --Rebin 1 --ToPlot DeltaEta --DataFiles $myfile --SkipSystematics true --YMin -0.2 --YMax 0.6 --RMin -1.65 --RMax 1.65 --OutputBase summary/nosub-DeltaEta-pp --CurveLabels $curvelabels --lines 0,1,2,2,1,2,1,2 --Tags $tags --Lables $labels

