#!/bin/bash
myfile="plots/PbPb0_30-result.root,plots/hybridPbPb030-result.root,plots/hybridPbPbNoWake030-result.root,plots/pyquenPbPb030-result.root,plots/jewelPbPb030-result.root,plots/jewelPbPbNoRecoil030-result.root,plots/PbPb_CCNU_v4.root,plots/20pythiaGen-result.root,plots/pp-result.root,plots/hybridPP-result.root,plots/hybridPP-result.root,plots/pyquenPP-result.root,plots/jewelPP-result.root,plots/jewelPP-result.root,plots/pp_CCNU_v4.root,plots/pythiaGen-result.root"
sys="plots/sys_PbPb0_30-result.root,plots/sys_pp-result.root,plots/sys_pp-result.root,plots/sys_pp-result.root,plots/sys_pp-result.root,plots/sys_pp-result.root,plots/sys_pp-result.root,plots/sys_pp-result.root,plots/sys_pp-result.root,plots/sys_pp-result.root,plots/sys_pp-result.root,plots/sys_pp-result.root,plots/sys_pp-result.root,plots/sys_pp-result.root,plots/sys_pp-result.root,plots/sys_pp-result.root,plots/sys_pp-result.root,plots/sys_pp-result.root"
#label=PbPb,Hybrid,HybridNoWake,Pyquen,Jewel,JewelNoRecoil
label="PbPb@0-30%","Hybrid","Hybrid@No@wake","PYQUEN","Jewel@v2.2.0","Jewel@No@recoil","CoLBT","PYTHIA8@p_{T}^{Z}>20@GeV"
#colors=1181,1180,1179,1182,15,1183,97,0,1184,1185,1186,1187,1188
colors=1181,1179,1179,1183,1180,1180,7,1,97,0,1184,1185,1186,1187,1188
markers=20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20
extraInfoPhi="40<p_{T}^{Z}<350@GeV","|y_{Z}|<2.4",""
extraInfoEta="40<p_{T}^{Z}<350@GeV","|y_{Z}|<2.4!@|#Delta#phi_{ch!Z}|<#frac{#pi}{2}"
#"\"PbPb 0-30%\",\"HYBRID No Wake\",\"HYBRID\",\"JEWEL\",PYQUEN,\"JEWEL No Recoil\""
./ExecuteDiff --ExtraInfo $extraInfoPhi --Markers $markers --Colors $colors --XAxisLabel "#Delta#phi_{ch,Z}" --YAxisLabel "d#LT#DeltaN_{ch}#GT/d#Delta#phi_{ch,Z}" --SolidXMin 0 --SolidXMax 3.1415926 --XMin -1.5758 --XMax 4.7275 --Rebin 1 --ToPlot DeltaPhi --DataFiles $myfile --SystematicFiles $sys --YMin -2.5 --YMax 4.5 --RMin -2.5 --RMax 2.5 --OutputBase summary/result-DeltaPhi-PbPb0_30MC --CurveLabels $label --lines 0,1,2,2,1,2,1,2
./ExecuteDiff --ExtraInfo $extraInfoEta --Markers $markers --Colors $colors --XAxisLabel "#Deltay_{ch,Z}" --YAxisLabel "d#LT#DeltaN_{ch}#GT/d#Delta y_{ch,Z}" --SolidXMin 0 --SolidXMax 3.9999 --XMin -3.999 --XMax 3.9999 --Rebin 2 --ToPlot DeltaEta --DataFiles $myfile --SystematicFiles $sys --YMin -1.45 --YMax 1.45 --RMin -1.55 --RMax 1.55 --OutputBase summary/result-DeltaEta-PbPb0_30MC --CurveLabels $label --lines 0,1,2,2,1,2,1,2


