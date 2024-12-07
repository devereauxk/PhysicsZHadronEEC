#!/bin/bash
myfile="plots/PbPb$1_$2-result.root,plots/pp-result.root,plots/pp-result.root,plots/pp-result.root"
sys="plots/sys_PbPb$1_$2-result.root,plots/sys_pp-result.root,plots/sys_pp-result.root,plots/sys_pp-result.root,plots/sys_pp-result.root,plots/sys_pp-result.root,plots/sys_pp-result.root,plots/sys_pp-result.root,plots/sys_pp-result.root,plots/sys_pp-result.root,plots/sys_pp-result.root,plots/sys_pp-result.root,plots/sys_pp-result.root,plots/sys_pp-result.root"
#label=PbPb,Hybrid,HybridNoWake,Pyquen,Jewel,JewelNoRecoil
label="PbPb@$1-$2%","pp"
#colors=1181,1180,1179,1182,15,1183,97,0,1184,1185,1186,1187,1188
colors=$3,1179,1179,15,1180,1180,1183,97,0,1184,1185,1186,1187,1188
markers=20,21,21,21,21,21,21
extraInfoPhi="40<p_{T}^{Z}<350@GeV","|y_{Z}|<2.4",""
extraInfoEta="40<p_{T}^{Z}<350@GeV","|y_{Z}|<2.4!@|#Delta#phi_{ch!Z}|<#frac{#pi}{2}"

#"\"PbPb 0-30%\",\"HYBRID No Wake\",\"HYBRID\",\"JEWEL\",PYQUEN,\"JEWEL No Recoil\""
./ExecuteDiff --PlotDiff 0 --ExtraInfo $extraInfoPhi --LegendBottom 0.7 --MarginTop $4 --MarginBottom $5 --Markers $markers --Colors $colors --XAxisLabel "#Delta#phi_{ch,Z}" --YAxisLabel "d#LT#DeltaN_{ch}#GT/d#Delta#phi_{ch,Z}" --SolidXMin 0 --SolidXMax 3.1415926 --XMin -1.5758 --XMax 4.7275 --Rebin $6 --ToPlot DeltaPhi --DataFiles $myfile --SystematicFiles $sys --YMin -3.5 --YMax 4.5 --RMin -2.99 --RMax 2.99 --OutputBase summary/result-DeltaPhi-PbPb$1_$2pp --CurveLabels $label --lines 0,-1
./ExecuteDiff --PlotDiff 0 --ExtraInfo $extraInfoEta --LegendBottom 0.7 --MarginTop $4 --MarginBottom $5 --Markers $markers --Colors $colors --XAxisLabel "#Deltay_{ch,Z}" --YAxisLabel "d#LT#DeltaN_{ch}#GT/d#Delta y_{ch,Z}" --SolidXMin 0 --SolidXMax 3.9999 --XMin -3.999 --XMax 3.9999 --Rebin 2 --ToPlot DeltaEta --DataFiles $myfile --SystematicFiles $sys --YMin -1.45 --YMax 1.45 --RMin -1.65 --RMax 1.65 --OutputBase summary/result-DeltaEta-PbPb$1_$2pp --CurveLabels $label --lines 0,-1


