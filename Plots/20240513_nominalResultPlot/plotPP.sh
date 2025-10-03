#!/bin/bash
myfile="plots/pp-result.root,plots/hybridPP-result.root,plots/pyquenPP-result.root,plots/jewelPP-result.root,plots/pp-v11-result.root,plots/pp-result.root,plots/hybridPP-result.root,plots/pyquenPP-result.root,plots/jewelPP-result.root,plots/pp-v11-result.root"
label="pp","Hybrid","PYQUEN","Jewel@v2.2.0,pp@v11"
colors=1181,1180,1179,1182,15,1183,97,0,1184,1185,1186,1187,1188
markers=20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20
extraInfoPhi="40<p_{T}^{Z}<350@GeV","|y_{Z}|<2.4",""
extraInfoEta="40<p_{T}^{Z}<350@GeV","|y_{Z}|<2.4!@|#Delta#phi_{ch!Z}|<#frac{#pi}{2}"

#"\"PbPb 0-30%\",\"HYBRID No Wake\",\"HYBRID\",\"JEWEL\",PYQUEN,\"JEWEL No Recoil\""
./ExecuteDiff --PlotDiff 0 --ExtraInfo $extraInfoPhi --LegendBottom 0.7 --Markers $markers --Colors $colors --XAxisLabel "#Delta#phi_{ch,Z}" --YAxisLabel "d#LT#DeltaN_{ch}#GT/d#Delta#phi_{ch,Z}" --SolidXMin 0 --SolidXMax 3.1415926 --XMin -1.5758 --XMax 4.7275 --Rebin 1 --ToPlot DeltaPhi --DataFiles $myfile --SkipSystematics true --YMin -1.5 --YMax 3 --RMin -2.99 --RMax 2.99 --OutputBase summary/result-DeltaPhi-pp --CurveLabels $label --lines 0,1,2,2,1,2,1,2
./ExecuteDiff --PlotDiff 0 --ExtraInfo $extraInfoEta --LegendBottom 0.7 --Markers $markers --Colors $colors --XAxisLabel "#Deltay_{ch,Z}" --YAxisLabel "d#LT#DeltaN_{ch}#GT/d#Delta y_{ch,Z}" --SolidXMin 0 --SolidXMax 3.9999 --XMin -3.999 --XMax 3.9999 --Rebin 2 --ToPlot DeltaEta --DataFiles $myfile --SkipSystematics true --YMin -1.45 --YMax 1.45 --RMin -1.65 --RMax 1.65 --OutputBase summary/result-DeltaEta-pp --CurveLabels $label --lines 0,1,2,2,1,2,1,2


